#ifndef __LP_PDD_MAIN_H_
#define __LP_PDD_MAIN_H_

#include "lp_type.h"

#define DEFAULT_LP_PDD_BLE_DATA_SEC_SIZE    185
#define DEFAULT_LP_PDD_SPP_DATA_SEC_SIZE    225

#define LP_PDD_EVENT_IS_CONNECTED           1
#define LP_PDD_EVENT_IS_DISCONNECTED        0

#define VOB_VOICE_PCM_FRAME_SIZE            640
#define VOICE_OPUS_PCM_DATA_SIZE_PER_FRAME  40

#define LP_PDD_SEND_MSG_MAX_COUNT           20
#define LP_PDD_SEND_DATA_TIME_INTERVAL      1 // 10ms

#define LP_PDD_DATA_HEAD_SIZE		        (sizeof(uint16_t) + 1) // cmd code + seq num

typedef enum {
    PDD_MSG_SEND_CMD_DATA,
    PDD_MSG_SEND_AUDIO_DATA, 
    PDD_MSG_SEND_OTA_DATA,
}PDD_MSG_TYPE;

typedef struct{
    uint8_t         msg_type;
    uint32_t        msg_dealt_index;
    uint32_t        msg_length;
    uint8_t         msg_buf[0];
}LP_PDD_MSG;

typedef struct{
    struct list_head entry;
    LP_PDD_MSG lp_pdd_msg;
}LP_PDD_MSG_CONTEXT;


typedef struct {
    volatile uint8_t            isSvStreamRunning   : 1;
    volatile uint8_t            isMicStreamOpened   : 1;
    volatile uint8_t            isDataXferOnGoing   : 1;
    volatile uint8_t            isThroughputTestOn  : 1;
    volatile uint8_t            reserve             : 4;
    volatile uint32_t           stateEvent;
    uint8_t                     seqNumWithInFrame;
    struct list_head            lp_pdd_msg_list;
    uint8_t                     lp_pdd_max_msg_num;
    volatile uint8_t            lp_pdd_msg_count;
} lp_pdd_context_t;

uint8_t app_get_mic_state(void);

void lp_pdd_connected(uint8_t connType);
void lp_pdd_disconnected(uint8_t disconnType);
int  lp_pdd_receive_cmd_data(uint8_t* ptrData, uint8_t dataLength);
int  lp_pdd_init(void);

#endif  // __LP_PDD_MAIN_H_

