#include "plat_addr_map.h"

#ifdef BT_CMU_BASE

#include "hal_intersys.h"
#include "hal_trace.h"
#include "hal_sleep.h"
#include CHIP_SPECIFIC_HDR(reg_cmu)
#include CHIP_SPECIFIC_HDR(reg_btcmu)
#include "cmsis_nvic.h"
#include "hal_timer.h"

#define MAX_SEND_RECORD_COUNT               3

// MCU-CMU ISIRQ_SET
#define CMU_BT2MCU_DATA_DONE_SET            (1 << 0)
#define CMU_BT2MCU_DATA1_DONE_SET           (1 << 1)
#define CMU_MCU2BT_DATA_IND_SET             (1 << 2)
#define CMU_MCU2BT_DATA1_IND_SET            (1 << 3)
#define CMU_BT_ALLIRQ_MASK_SET              (1 << 4)

// MCU-CMU ISIRQ_CLR
#define CMU_BT2MCU_DATA_DONE_CLR            (1 << 0)
#define CMU_BT2MCU_DATA1_DONE_CLR           (1 << 1)
#define CMU_MCU2BT_DATA_IND_CLR             (1 << 2)
#define CMU_MCU2BT_DATA1_IND_CLR            (1 << 3)
#define CMU_BT_ALLIRQ_MASK_CLR              (1 << 4)

// BT-CMU ISIRQ_SET
#define BTCMU_MCU2BT_DATA_DONE_SET          (1 << 0)
#define BTCMU_MCU2BT_DATA1_DONE_SET         (1 << 1)
#define BTCMU_BT2MCU_DATA_IND_SET           (1 << 2)
#define BTCMU_BT2MCU_DATA1_IND_SET          (1 << 3)
#define BTCMU_MCU_ALLIRQ_MASK_SET           (1 << 4)

// BT-CMU ISIRQ_CLR
#define BTCMU_MCU2BT_DATA_DONE_CLR          (1 << 0)
#define BTCMU_MCU2BT_DATA1_DONE_CLR         (1 << 1)
#define BTCMU_BT2MCU_DATA_IND_CLR           (1 << 2)
#define BTCMU_BT2MCU_DATA1_IND_CLR          (1 << 3)
#define BTCMU_MCU_ALLIRQ_MASK_CLR           (1 << 4)

enum HAL_INTERSYS_IRQ_TYPE_T {
    HAL_INTERSYS_IRQ_SEND_IND,
    HAL_INTERSYS_IRQ_RECV_DONE,

    HAL_INTERSYS_IRQ_TYPE_QTY
};

struct HAL_INTERSYS_MSG_T {
    struct HAL_INTERSYS_MSG_T *next;        // pointer to next element in the list
    enum HAL_INTERSYS_MSG_TYPE_T type;      // message type
    unsigned int len;                       // message data length in bytes
    const unsigned char *data;              // pointer to the message data
};

struct HAL_INTERSYS_SEND_RECORD_T {
    struct HAL_INTERSYS_MSG_T msg;
    bool in_use;
};

static const IRQn_Type rx_irq_id[HAL_INTERSYS_ID_QTY] = {
    ISDATA_IRQn,
    ISDATA1_IRQn,
};

static const IRQn_Type tx_irq_id[HAL_INTERSYS_ID_QTY] = {
    ISDONE_IRQn,
    ISDONE1_IRQn,
};

static struct HAL_INTERSYS_MSG_T *** const bt_recv_msg_list_ppp[HAL_INTERSYS_ID_QTY] = {
    (struct HAL_INTERSYS_MSG_T ***)(BT_RAM_BASE + 0x4),
    (struct HAL_INTERSYS_MSG_T ***)(BT_RAM_BASE+ 0xC),
};

static struct HAL_INTERSYS_MSG_T ** const recv_msg_list_p[HAL_INTERSYS_ID_QTY] = {
    (struct HAL_INTERSYS_MSG_T **)BT_RAM_BASE,
    (struct HAL_INTERSYS_MSG_T **)(BT_RAM_BASE + 0x8),
};

static struct HAL_INTERSYS_MSG_T * send_msg_list_p[HAL_INTERSYS_ID_QTY];
static struct HAL_INTERSYS_MSG_T * send_pending_list_p[HAL_INTERSYS_ID_QTY];

static struct HAL_INTERSYS_MSG_T recv_pending_head[HAL_INTERSYS_ID_QTY];

static struct HAL_INTERSYS_SEND_RECORD_T send_msgs[HAL_INTERSYS_ID_QTY][MAX_SEND_RECORD_COUNT];

static HAL_INTERSYS_RX_IRQ_HANDLER rx_irq_handler[HAL_INTERSYS_ID_QTY][HAL_INTERSYS_MSG_TYPE_QTY];
static HAL_INTERSYS_TX_IRQ_HANDLER tx_irq_handler[HAL_INTERSYS_ID_QTY][HAL_INTERSYS_MSG_TYPE_QTY];

static bool chan_opened[HAL_INTERSYS_ID_QTY] = { false, false, };

static bool chan_busy[HAL_INTERSYS_ID_QTY] = { false, false, };

static bool need_flow_ctrl[HAL_INTERSYS_ID_QTY] = { false, false, };

static bool busy_now = false;

static struct CMU_T * const cmu = (struct CMU_T *)CMU_BASE;

static struct BTCMU_T * const btcmu = (struct BTCMU_T *)BT_CMU_BASE;

static void hal_intersys_busy(enum HAL_INTERSYS_ID_T id, bool busy)
{
    int i;
    bool new_state;

    if (chan_busy[id] == busy) {
        return;
    }

    chan_busy[id] = busy;

    if (busy_now == busy) {
        return;
    }

    if (busy) {
        hal_sys_wake_lock(HAL_SYS_WAKE_LOCK_USER_INTERSYS);
        busy_now = true;
    } else {
        new_state = false;
        for (i = 0; i < HAL_INTERSYS_ID_QTY; i++) {
            if (chan_busy[i]) {
                new_state = true;
                break;
            }
        }
        if (!new_state) {
            hal_sys_wake_unlock(HAL_SYS_WAKE_LOCK_USER_INTERSYS);
            busy_now = false;
        }
    }
}

static int hal_intersys_peer_irq_set(enum HAL_INTERSYS_ID_T id, enum HAL_INTERSYS_IRQ_TYPE_T type)
{
    uint32_t value;

    if (id == HAL_INTERSYS_ID_0) {
        if (type == HAL_INTERSYS_IRQ_SEND_IND) {
            value = CMU_MCU2BT_DATA_IND_SET;
        } else {
            value = CMU_BT2MCU_DATA_DONE_SET;
        }
    } else {
        if (type == HAL_INTERSYS_IRQ_SEND_IND) {
            value = CMU_MCU2BT_DATA1_IND_SET;
        } else {
            value = CMU_BT2MCU_DATA1_DONE_SET;
        }
    }

    cmu->ISIRQ_SET = value;
    return 0;
}

static int hal_intersys_local_irq_clear(enum HAL_INTERSYS_ID_T id, enum HAL_INTERSYS_IRQ_TYPE_T type)
{
    uint32_t value;

    if (id == HAL_INTERSYS_ID_0) {
        if (type == HAL_INTERSYS_IRQ_SEND_IND) {
            value = BTCMU_BT2MCU_DATA_IND_CLR;
        } else {
            value = BTCMU_MCU2BT_DATA_DONE_CLR;
        }
    } else {
        if (type == HAL_INTERSYS_IRQ_SEND_IND) {
            value = BTCMU_BT2MCU_DATA1_IND_CLR;
        } else {
            value = BTCMU_MCU2BT_DATA1_DONE_CLR;
        }
    }

    btcmu->ISIRQ_CLR = value;
    return 0;
}

static int hal_intersys_local_irq_set(enum HAL_INTERSYS_ID_T id, enum HAL_INTERSYS_IRQ_TYPE_T type)
{
    uint32_t value;

    if (id == HAL_INTERSYS_ID_0) {
        if (type == HAL_INTERSYS_IRQ_SEND_IND) {
            value = BTCMU_BT2MCU_DATA_IND_SET;
        } else {
            value = BTCMU_MCU2BT_DATA_DONE_SET;
        }
    } else {
        if (type == HAL_INTERSYS_IRQ_SEND_IND) {
            value = BTCMU_BT2MCU_DATA1_IND_SET;
        } else {
            value = BTCMU_MCU2BT_DATA1_DONE_SET;
        }
    }

    btcmu->ISIRQ_SET = value;
    return 0;
}


//static void hal_intersys_wait_done_idle(void)
//{
//    while(hal_cmu_get_address()->ISIRQ_SET & (CMU_BT2MCU_DATA_DONE_SET |CMU_BT2MCU_DATA1_DONE_SET));
//}


//static void hal_intersys_wait_data_idle(void)
//{
//    while(hal_cmu_get_address()->ISIRQ_SET & (CMU_MCU2BT_DATA_IND_SET |CMU_MCU2BT_DATA1_IND_SET));
//}


static void hal_intersys_rx_irq(void)
{
    int id;
    struct HAL_INTERSYS_MSG_T *msg_ptr;
    enum HAL_INTERSYS_MSG_TYPE_T type;
    unsigned int processed;

    for (id = HAL_INTERSYS_ID_0; id < HAL_INTERSYS_ID_QTY; id++) {
        if (NVIC_GetActive(rx_irq_id[id])) {
            hal_intersys_local_irq_clear(id, HAL_INTERSYS_IRQ_SEND_IND);

            if (recv_pending_head[id].data) {
                // Previous unprocessed message
                msg_ptr = &recv_pending_head[id];
            } else {
                // New message
                msg_ptr = *recv_msg_list_p[id];
            }
            while (msg_ptr) {
                type = msg_ptr->type;
                if (type >= HAL_INTERSYS_MSG_TYPE_QTY) {
                    // Error
                    ASSERT(false, "INTERSYS-RX: Invalid msg type: %d", type);
                    break;
                }
                if (rx_irq_handler[id][type]) {
                    processed = rx_irq_handler[id][type](msg_ptr->data, msg_ptr->len);
                    // Check if flow control needed
                    if (processed < msg_ptr->len) {
                        recv_pending_head[id].next = msg_ptr->next;
                        recv_pending_head[id].type = msg_ptr->type;
                        recv_pending_head[id].len = msg_ptr->len - processed;
                        recv_pending_head[id].data = msg_ptr->data + processed;
                        break;
                    }
                } else {
                    // Error
                    ASSERT(false, "INTERSYS-RX: Handler missing");
                    break;
                }
                msg_ptr = msg_ptr->next;
            }

            if (msg_ptr == NULL) {
                if (!need_flow_ctrl[id]){
                    hal_intersys_peer_irq_set(id, HAL_INTERSYS_IRQ_RECV_DONE);
                }
                recv_pending_head[id].data = NULL;
            }
        }
    }
}

static void hal_intersys_tx_irq(void)
{
    int id;
    struct HAL_INTERSYS_MSG_T *msg_ptr;
    enum HAL_INTERSYS_MSG_TYPE_T type;

    for (id = HAL_INTERSYS_ID_0; id < HAL_INTERSYS_ID_QTY; id++) {
        if (NVIC_GetActive(tx_irq_id[id])) {
            hal_intersys_local_irq_clear(id, HAL_INTERSYS_IRQ_RECV_DONE);

            msg_ptr = send_msg_list_p[id];
            while (msg_ptr) {
                type = msg_ptr->type;
                if (type >= HAL_INTERSYS_MSG_TYPE_QTY) {
                    // Error
                    ASSERT(false, "INTERSYS-TX: Invalid msg type: %d", type);
                    break;
                }
                if (tx_irq_handler[id][type]) {
                    tx_irq_handler[id][type](msg_ptr->data, msg_ptr->len);
                };
                CONTAINER_OF(msg_ptr, struct HAL_INTERSYS_SEND_RECORD_T, msg)->in_use = false;
                msg_ptr = msg_ptr->next;
            }

            if (send_pending_list_p[id]) {
                send_msg_list_p[id] = send_pending_list_p[id];
                send_pending_list_p[id] = NULL;
                hal_intersys_peer_irq_set(id, HAL_INTERSYS_IRQ_SEND_IND);
            } else {
                send_msg_list_p[id] = NULL;
                // Allow sleep
                hal_intersys_busy(id, false);
            }
        }
    }
}

int hal_intersys_open(enum HAL_INTERSYS_ID_T id, enum HAL_INTERSYS_MSG_TYPE_T type,
                      HAL_INTERSYS_RX_IRQ_HANDLER rxhandler, HAL_INTERSYS_TX_IRQ_HANDLER txhandler, bool rx_flowctrl)
{
    int i;

    if (id >= HAL_INTERSYS_ID_QTY) {
        return 1;
    }
    if (type >= HAL_INTERSYS_MSG_TYPE_QTY) {
        return 2;
    }

    if (!chan_opened[id]) {
        if (id == HAL_INTERSYS_ID_0) {
            cmu->ISIRQ_CLR = CMU_BT2MCU_DATA_DONE_CLR | CMU_MCU2BT_DATA_IND_CLR;
        } else {
            cmu->ISIRQ_CLR = CMU_BT2MCU_DATA1_DONE_CLR | CMU_MCU2BT_DATA1_IND_CLR;
        }
        *bt_recv_msg_list_ppp[id] = &send_msg_list_p[id];

        NVIC_SetVector(rx_irq_id[id], (uint32_t)hal_intersys_rx_irq);
        NVIC_SetPriority(rx_irq_id[id], IRQ_PRIORITY_NORMAL);

        NVIC_SetVector(tx_irq_id[id], (uint32_t)hal_intersys_tx_irq);
        NVIC_SetPriority(tx_irq_id[id], IRQ_PRIORITY_NORMAL);

        chan_opened[id] = true;;
    }

    // Stop IRQs by default
    NVIC_DisableIRQ(rx_irq_id[id]);
    NVIC_DisableIRQ(tx_irq_id[id]);

    send_msg_list_p[id] = NULL;
    send_pending_list_p[id] = NULL;
    recv_pending_head[id].data = NULL;
    for (i = 0; i < MAX_SEND_RECORD_COUNT; i++) {
        send_msgs[id][i].in_use = false;
    }

    rx_irq_handler[id][type] = rxhandler;
    tx_irq_handler[id][type] = txhandler;

    need_flow_ctrl[id] = rx_flowctrl;

    return 0;
}

int hal_intersys_close(enum HAL_INTERSYS_ID_T id,enum HAL_INTERSYS_MSG_TYPE_T type)
{
    if (id >= HAL_INTERSYS_ID_QTY) {
        return 1;
    }
    chan_opened[id] = false;
    // Stop IRQs by default
    NVIC_DisableIRQ(rx_irq_id[id]);
    NVIC_DisableIRQ(tx_irq_id[id]);

    send_msg_list_p[id] = NULL;
    send_pending_list_p[id] = NULL;
    recv_pending_head[id].data = NULL;
    rx_irq_handler[id][type] = NULL;
    tx_irq_handler[id][type] = NULL;
    need_flow_ctrl[id] = false;
    return 0;
}

int hal_intersys_start_recv(enum HAL_INTERSYS_ID_T id)
{
    if (id >= HAL_INTERSYS_ID_QTY) {
        return 1;
    }

    NVIC_EnableIRQ(rx_irq_id[id]);
    // Check if there is any previous unprocessed message
    if (recv_pending_head[id].data) {
        hal_intersys_local_irq_set(id, HAL_INTERSYS_IRQ_SEND_IND);
    }
    return 0;
}

int hal_intersys_stop_recv(enum HAL_INTERSYS_ID_T id)
{
    if (id >= HAL_INTERSYS_ID_QTY) {
        return 1;
    }

    NVIC_DisableIRQ(rx_irq_id[id]);
    return 0;
}

int hal_intersys_send(enum HAL_INTERSYS_ID_T id, enum HAL_INTERSYS_MSG_TYPE_T type,
                      const unsigned char *data, unsigned int len)
{
    uint32_t lock;
    int ret;
    struct HAL_INTERSYS_SEND_RECORD_T *record;
    struct HAL_INTERSYS_MSG_T *next;
    int i;

    if (id >= HAL_INTERSYS_ID_QTY) {
        return 1;
    }
    if (type >= HAL_INTERSYS_MSG_TYPE_QTY) {
        return 2;
    }
    if (!chan_opened[id]) {
        return 3;
    }

    NVIC_EnableIRQ(tx_irq_id[id]);

    ret = -1;
    record = &send_msgs[id][0];

    lock = int_lock();
    for (i = 0; i < MAX_SEND_RECORD_COUNT; i++) {
        if (record->in_use) {
            record++;
            continue;
        }
        record->in_use = true;
        record->msg.next = NULL;
        record->msg.type = type;
        record->msg.len = len;
        record->msg.data = data;
        if (send_msg_list_p[id] == NULL) {
            send_msg_list_p[id] = &record->msg;
            hal_intersys_peer_irq_set(id, HAL_INTERSYS_IRQ_SEND_IND);
        } else if (send_pending_list_p[id] == NULL) {
            send_pending_list_p[id] = &record->msg;
        } else {
            next = send_pending_list_p[id];
            while (next->next) {
                next = next->next;
            }
            next->next = &record->msg;
        }
        ret = 0;
        // Prohibit sleep here
        hal_intersys_busy(id, true);
        break;
    }
    int_unlock(lock);

    return ret;
}

void hal_intersys_rx_done(enum HAL_INTERSYS_ID_T id)
{
    hal_intersys_peer_irq_set(id, HAL_INTERSYS_IRQ_RECV_DONE);
}

#endif

