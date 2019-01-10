#ifndef __HAL_I2C_H__
#define __HAL_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

/* hal i2c demo
struct HAL_I2C_CONFIG_T i2c_cfg;

i2c_cfg.use_dma  = 0;
i2c_cfg.use_sync = 1;
i2c_cfg.speed = 200000;
i2c_cfg.as_master = 1;
hal_i2c_open(HAL_I2C_ID_0, &i2c_cfg);
// select register page 0
buf[0] = 0x0;
buf[1] = 0x0;
TRACE("[i2c]    select tlv320aic32 register page 0\n");
hal_i2c_send(HAL_I2C_ID_0, 0x18, &buf, 1, 1, 0);

// soft reset
buf[0] = 0x1;
buf[1] = 0x80;
TRACE("[i2c]    soft reset tlv320aic32\n");
hal_i2c_send(HAL_I2C_ID_0, 0x18, &buf, 1, 1, 0);

// write sample rate
buf[0] = 0x2;
buf[1] = 0xa;
TRACE("[i2c]    write sample rate\n");
hal_i2c_send(HAL_I2C_ID_0, 0x18, &buf, 1, 1, 0);

// read sample rate
buf[0] = 0x2;
buf[1] = 0x0;
TRACE("[i2c]    read sample rate\n");
hal_i2c_recv(HAL_I2C_ID_0, 0x18, &buf, 1, 1, HAL_I2C_RESTART_AFTER_WRITE, 0);
TRACE("[i2c]    back value 0x%x\n", buf[1]);
*/

#include "plat_types.h"
#include "hal_gpio.h"

enum {
    HAL_I2C_STOP_AFTER_WRITE = 0,
    HAL_I2C_RESTART_AFTER_WRITE,
};

enum HAL_I2C_ID_T {
    HAL_I2C_ID_0 = 0,
#if (CHIP_HAS_I2C > 1)
    HAL_I2C_ID_1,
#endif

    HAL_I2C_ID_NUM,
};

enum HAL_I2C_API_MODE_T {
    HAL_I2C_API_MODE_TASK = 0,
    HAL_I2C_API_MODE_SIMPLE,
};

enum HAL_I2C_INT_STATUS_T {
    HAL_I2C_INT_STATUS_GEN_CALL_MASK = 0x1<<11,
    HAL_I2C_INT_STATUS_START_DET_MASK = 0x1<<10,
    HAL_I2C_INT_STATUS_STOP_DET_MASK = 0x1<<9,
    HAL_I2C_INT_STATUS_ACTIVITY_MASK = 0x1<<8,
    HAL_I2C_INT_STATUS_RX_DONE_MASK = 0x1<<7,
    HAL_I2C_INT_STATUS_TX_ABRT_MASK = 0x1<<6,
    HAL_I2C_INT_STATUS_RD_REQ_MASK = 0x1<<5,
    HAL_I2C_INT_STATUS_TX_EMPTY_MASK = 0x1<<4,
    HAL_I2C_INT_STATUS_TX_OVER_MASK = 0x1<<3,
    HAL_I2C_INT_STATUS_RX_FULL_MASK = 0x1<<2,
    HAL_I2C_INT_STATUS_RX_OVER_MASK = 0x1<<1,
    HAL_I2C_INT_STATUS_RX_UNDER_MASK = 0x1<<0,
    /* FIXME : same as i2cip definitions
    #define I2CIP_INT_STATUS_GEN_CALL_SHIFT (11)
    #define I2CIP_INT_STATUS_START_DET_SHIFT (10)
    #define I2CIP_INT_STATUS_STOP_DET_SHIFT (9)
    #define I2CIP_INT_STATUS_ACTIVITY_SHIFT (8)
    #define I2CIP_INT_STATUS_RX_DONE_SHIFT  (7)
    #define I2CIP_INT_STATUS_TX_ABRT_SHIFT (6)
    #define I2CIP_INT_STATUS_RD_REQ_SHIFT (5)
    #define I2CIP_INT_STATUS_TX_EMPTY_SHIFT (4)
    #define I2CIP_INT_STATUS_TX_OVER_SHIFT (3)
    #define I2CIP_INT_STATUS_RX_FULL_SHIFT (2)
    #define I2CIP_INT_STATUS_RX_OVER_SHIFT (1)
    #define I2CIP_INT_STATUS_RX_UNDER_SHIFT (0)
     */
};

enum HAL_I2C_ERRCODE_T {
    HAL_I2C_ERRCODE_SLVRD_INTX = 0x1<<15,
    HAL_I2C_ERRCODE_SLV_ARBLOST = 0x1<<14,
    HAL_I2C_ERRCODE_SLVFLUSH_TXFIFO = 0x1<<13,
    HAL_I2C_ERRCODE_ARB_LOST = 0x1<<12,
    HAL_I2C_ERRCODE_MASTER_DIS = 0x1<<11,
    HAL_I2C_ERRCODE_10B_RD_NORSTRT = 0x1<<10,
    HAL_I2C_ERRCODE_SBYTE_NORSTRT = 0x1<<9,
    HAL_I2C_ERRCODE_HS_NORSTRT = 0x1<<8,
    HAL_I2C_ERRCODE_SBYTE_ACKDET = 0x1<<7,
    HAL_I2C_ERRCODE_HS_ACKDET = 0x1<<6,
    HAL_I2C_ERRCODE_GCALL_READ = 0x1<<5,
    HAL_I2C_ERRCODE_GCALL_NOACK = 0x1<<4,
    HAL_I2C_ERRCODE_TXDATA_NOACK = 0x1<<3,
    HAL_I2C_ERRCODE_10ADDR2_NOACK = 0x1<<2,
    HAL_I2C_ERRCODE_10ADDR1_NOACK = 0x1<<1,
    HAL_I2C_ERRCODE_7B_ADDR_NOACK = 0x1<<0,

    HAL_I2C_ERRCODE_INV_PARAM = 1<<31,
    HAL_I2C_ERRCODE_SETUP_IRQ = 1<<30,
    HAL_I2C_ERRCODE_EN_IRQ    = 1<<29,
    HAL_I2C_ERRCODE_SYNC_TIMEOUT = 1<<28,

    /* FIXME : same as i2cip definitions
    #define I2CIP_TX_ABRT_SOURCE_ABRT_SLVRD_INTX_SHIFT (15)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_SLV_ARBLOST_SHIFT (14)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_SLVFLUSH_TXFIFO_SHIFT (13)
    #define I2CIP_TX_ABRT_SOURCE_ARB_LOST_SHIFT (12)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_MASTER_DIS_SHIFT (11)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_10B_RD_NORSTRT_SHIFT (10)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_SBYTE_NORSTRT_SHIFT (9)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_HS_NORSTRT_SHIFT (8)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_SBYTE_ACKDET_SHIFT (7)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_HS_ACKDET_SHIFT (6)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_GCALL_READ_SHIFT (5)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_GCALL_NOACK_SHIFT (4)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK_SHIFT (3)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_10ADDR2_NOACK_SHIFT (2)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_10ADDR1_NOACK_SHIFT (1)
    #define I2CIP_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK_SHIFT (0)
    */
};

struct HAL_I2C_CONFIG_T {
    /* mode
     SIMPLE_MODE : pulling when reading or writing (always sync), or use dma external;master or slave;only enable slave related irq;
     TASK_MODE : task to driven reading or writing (sync or async, dma or non-dma);only master
     */
    enum HAL_I2C_API_MODE_T mode;
    /* used when mode == HAL_I2C_API_MODE_TASK */
    uint8_t use_sync;
    uint8_t use_dma;
    /* end */

    uint8_t as_master;
    uint32_t speed;
    uint32_t addr_as_slave;
};

struct HAL_GPIO_I2C_CONFIG_T {
    enum HAL_GPIO_PIN_T scl;
    enum HAL_GPIO_PIN_T sda;
    uint32_t speed;
};

uint32_t hal_i2c_open(enum HAL_I2C_ID_T id, struct HAL_I2C_CONFIG_T *cfg);
/* for master task mode */
typedef void (*HAL_I2C_TRANSFER_HANDLER_T)(enum HAL_I2C_ID_T id, uint32_t transfer_id, uint8_t *buf, uint32_t len, enum HAL_I2C_ERRCODE_T errcode);
uint32_t hal_i2c_send(enum HAL_I2C_ID_T id, uint32_t device_addr, uint8_t *buf, uint32_t reg_len, uint32_t value_len,
        uint32_t transfer_id, HAL_I2C_TRANSFER_HANDLER_T handler);
uint32_t hal_i2c_recv(enum HAL_I2C_ID_T id, uint32_t device_addr, uint8_t *buf, uint32_t reg_len, uint32_t value_len,
        uint8_t restart_after_write, uint32_t transfer_id, HAL_I2C_TRANSFER_HANDLER_T handler);
/* for master task mode end */
/* for slave and simple master mode */
typedef void (*HAL_I2C_INT_HANDLER_T)(enum HAL_I2C_INT_STATUS_T status, uint32_t errocode);
uint32_t hal_i2c_slv_write(enum HAL_I2C_ID_T id, uint8_t *buf, uint32_t buf_len, uint32_t *act_len);
uint32_t hal_i2c_slv_read(enum HAL_I2C_ID_T id, uint8_t *buf, uint32_t buf_len, uint32_t *act_len);
uint32_t hal_i2c_set_interrupt_handler(enum HAL_I2C_ID_T id, HAL_I2C_INT_HANDLER_T handler);
uint32_t hal_i2c_mst_write(enum HAL_I2C_ID_T id, uint32_t device_addr, uint8_t *buf, uint32_t buf_len, uint32_t *act_len, uint32_t restart, uint32_t stop);
uint32_t hal_i2c_mst_read(enum HAL_I2C_ID_T id, uint32_t device_addr, uint8_t *buf, uint32_t buf_len, uint32_t *act_len, uint32_t restart, uint32_t stop);
/* for slave and simple master mode end */
uint32_t hal_i2c_close(enum HAL_I2C_ID_T id);

int hal_gpio_i2c_open(struct HAL_GPIO_I2C_CONFIG_T *cfg);

int hal_gpio_i2c_close(void);

uint32_t hal_gpio_i2c_send(uint32_t device_addr,
                uint8_t *buf, uint32_t reg_len, uint32_t value_len);

uint32_t hal_gpio_i2c_recv(uint32_t device_addr,
                        uint8_t *buf, uint32_t reg_len, uint32_t value_len,
                        uint8_t restart_after_write);

#ifdef __cplusplus
}
#endif

#endif /* __HAL_I2C_H__ */
