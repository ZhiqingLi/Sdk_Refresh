#include "hal_i2c.h"
#include "string.h"
#include "hal_trace.h"
#include "bes_hal_i2c.h"
#include "bes_hal_board.h"
#include "bes_hal_uart.h"

#define BES_HAL_I2C_READ_BUF_LEN        (128)
#define BES_HAL_I2C_WRITE_BUF_LEN        (4096)
#ifdef CHIP_HAS_PSRAM
#include "hal_location.h"
static PSRAM_BSS_LOC uint8_t i2cWriteBuf[BES_HAL_I2C_WRITE_BUF_LEN];
static PSRAM_BSS_LOC uint8_t i2cReadBuf[BES_HAL_I2C_READ_BUF_LEN];
#else
static uint8_t i2cWriteBuf[BES_HAL_I2C_WRITE_BUF_LEN];
static uint8_t i2cReadBuf[BES_HAL_I2C_READ_BUF_LEN];
#endif

static enum HAL_I2C_ID_T gI2CId = HAL_I2C_ID_0;
static uint8_t gI2CInitFlag = 0;
int32_t bes_hal_i2c_open(void)
{
    struct HAL_I2C_CONFIG_T bes_hal_i2c_cfg;
    TRACE("\n%s: enter, gI2CInitFlag = %d!\n",__func__,gI2CInitFlag);

    #if UART_OTA_PORT_SEL == UART_OTA_PORT_DIFF_SYS_DEBUG
    // close the ota transport uart;
    bes_hal_uart_deinit();
    bes_hal_i2c_close();
    #endif

    if(gI2CInitFlag == 0)
    {
        hal_iomux_set_i2c_bus();
        gI2CId = HAL_I2C_ID_0;
        bes_hal_i2c_cfg.mode = HAL_I2C_API_MODE_TASK;
        bes_hal_i2c_cfg.use_dma  = 0;
        bes_hal_i2c_cfg.use_sync = 1;
        bes_hal_i2c_cfg.speed = 100000;
        bes_hal_i2c_cfg.as_master = 1;
        hal_i2c_open(gI2CId, &bes_hal_i2c_cfg);
        gI2CInitFlag = 1;
    }
    return 0;
}

int32_t bes_hal_i2c_close(void)
{
    if(gI2CInitFlag)
        hal_i2c_close(gI2CId);
    gI2CInitFlag = 0;
    return 0;
}

int32_t bes_hal_i2c_write(uint8_t i2cDevAddr, uint32_t regAddr, uint8_t regAddrLen, uint8_t* pBuf, uint32_t len)
{
    uint32_t ret = 0, i = 0;

    for(i = 0; i < regAddrLen; i++)
        i2cWriteBuf[i] = (uint8_t)(regAddr >> ((regAddrLen - 1 -i)*8));
    memcpy(&i2cWriteBuf[regAddrLen],pBuf,len);
    ret = hal_i2c_send(gI2CId, i2cDevAddr, i2cWriteBuf, regAddrLen, len, 0, 0);
    if(ret != 0)
        return -1;
    return 0;
}

int32_t bes_hal_i2c_read(uint8_t i2cDevAddr, uint32_t regAddr, uint8_t regAddrLen, uint8_t* pBuf, uint32_t len)
{
    uint32_t ret = 0, i = 0;
    
    for(i = 0; i < regAddrLen; i++)
        i2cReadBuf[i] = (uint8_t)(regAddr >> ((regAddrLen - 1 -i)*8));
    ret = hal_i2c_recv(gI2CId, i2cDevAddr, i2cReadBuf, regAddrLen, len, 1, 0, 0); // restart after write
    if(ret == 0)
    {
        memcpy(pBuf,&i2cReadBuf[regAddrLen],len);
        return 0;
    }
    return -2;
}