 /**
 **************************************************************************************
 * @file    mcu_circular_buf.h
 * @brief   MCU management cycle buf
 *
 * @author  Sam
 * @version V1.1.0
 *
 * $Created: 2015-11-02 15:56:11$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#ifndef		__MCU_CIRCULAR_BUF_H__
#define		__MCU_CIRCULAR_BUF_H__

/**
 * @addtogroup 驱动
 * @{
 * @defgroup MCUCricularBuf MCUCricularBuf
 * @{
 */

typedef struct __MCU_CIRCULAR_CONTEXT__
{
    uint32_t    R;              //循环buf读指针,byte数
    uint32_t    W;              //循环buf写指针,byte数
    uint32_t    BufDepth;       //循环长度,byte数  
    int8_t*     CircularBuf;    //循环buf指针
} MCU_CIRCULAR_CONTEXT;

extern MCU_CIRCULAR_CONTEXT OTGCircularBuf;

void MCUCircular_Config(MCU_CIRCULAR_CONTEXT* CircularBuf, void* Buf, uint32_t Len);

int32_t MCUCircular_GetSpaceLen(MCU_CIRCULAR_CONTEXT* CircularBuf);

void MCUCircular_PutData(MCU_CIRCULAR_CONTEXT* CircularBuf, void* InBuf, uint16_t Len);

int32_t MCUCircular_GetData(MCU_CIRCULAR_CONTEXT* CircularBuf, void* OutBuf, uint16_t MaxLen);



/**
 * @}
 * @}
 */
#endif
//
