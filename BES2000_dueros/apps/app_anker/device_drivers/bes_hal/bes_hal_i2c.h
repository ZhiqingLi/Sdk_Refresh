#ifndef __BES_HAL_I2C_H__
#define __BES_HAL_I2C_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "bttypes.h"

int32_t bes_hal_i2c_open(void);
int32_t bes_hal_i2c_close(void);
int32_t bes_hal_i2c_write(uint8_t i2cDevAddr, uint32_t regAddr, uint8_t regAddrLen, uint8_t* pBuf, uint32_t len);
int32_t bes_hal_i2c_read(uint8_t i2cDevAddr, uint32_t regAddr, uint8_t regAddrLen, uint8_t* pBuf, uint32_t len);

#ifdef __cplusplus
}
#endif
#endif