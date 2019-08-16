#include<string.h>
#include "type.h"
#include "app_config.h"
#include "i2c.h"
#include "clk.h"
#include "gpio.h"
#include "dev_detect_driver.h"
#include "i2c.h"

static void* TestI2cHandler = NULL;
uint8_t ChipVer;	

void test_iic(void)
{
    uint8_t TempData[3];
    
    if(TestI2cHandler == NULL)
    {
        TestI2cHandler = I2cMasterCreate(PORT_C, 3, PORT_C, 4);
    }

    I2cReadNByte(TestI2cHandler, 0xAE, 0x08, (uint8_t*)(&TempData), 1);
    if ( TempData[0] != MSG_NONE )
    {
        APP_DBG("IIC read MSG = %x;\n", TempData[0]);
        TempData[0] = MSG_NONE;
    }
}
