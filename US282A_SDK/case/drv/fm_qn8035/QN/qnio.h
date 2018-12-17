#ifndef _QNIO_H__
#define _QNIO_H__

#include "psp_includes.h"

/* modify this according to I2C device address when you use standard I2C function like I2C_XXXX except AI2C_XXXX*/
#define I2C_DEV0_ADDRESS 	0x20  // 00100000B, LSB will be set to "1" (address = 0x21) when do read accessing 

/***************************************************************************************************/
extern UINT8 QND_ReadReg(UINT8 adr) __FAR__;
extern UINT8 QND_WriteReg(UINT8 adr, UINT8 value) __FAR__;

#endif//#ifndef _QNIO_H__
