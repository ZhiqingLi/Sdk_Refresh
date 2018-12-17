 /**
 **************************************************************************************
 * @file    mcu_circular_buf.c
 * @brief   mcu
 *
 * @author  Sam
 * @version V1.1.0
 *
 * $Created: 2015-11-02 15:56:11$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include <string.h>
#include "type.h"
#include "mcu_circular_buf.h"
#include "debug.h"


void MCUCircular_Config(MCU_CIRCULAR_CONTEXT* CircularBuf, void* Buf, uint32_t Len)
{
    CircularBuf->CircularBuf = Buf;
    CircularBuf->BufDepth = Len;
    CircularBuf->R = 0;
    CircularBuf->W = 0;
}

int32_t MCUCircular_GetSpaceLen(MCU_CIRCULAR_CONTEXT* CircularBuf)
{
	//读写指针重合，只发生在MCU 循环fifo 初始化或者播空的时候（此时异常，系统强制行为）
	if(CircularBuf->R == CircularBuf->W)
	{
		return CircularBuf->BufDepth;
	}
	else
	{
		return (CircularBuf->BufDepth + CircularBuf->R - CircularBuf->W) % CircularBuf->BufDepth;
	}   
}

void MCUCircular_PutData(MCU_CIRCULAR_CONTEXT* CircularBuf, void* InBuf, uint16_t Len)
{
    if(Len == 0)
	{
		return;
	}
	if(CircularBuf->W + Len <= CircularBuf->BufDepth)
    {
        memcpy((void *)&CircularBuf->CircularBuf[CircularBuf->W], InBuf, Len);
    }
    else
    {
        memcpy((void *)&CircularBuf->CircularBuf[CircularBuf->W], InBuf, CircularBuf->BufDepth - CircularBuf->W);
        memcpy((void *)&CircularBuf->CircularBuf[0], (uint8_t*)InBuf + CircularBuf->BufDepth - CircularBuf->W, CircularBuf->W + Len - CircularBuf->BufDepth);
    }
    CircularBuf->W = (CircularBuf->W + Len) % CircularBuf->BufDepth;    
}


int32_t MCUCircular_GetData(MCU_CIRCULAR_CONTEXT* CircularBuf, void* OutBuf, uint16_t MaxLen)
{
    uint16_t R, W;
    uint16_t Len;
    
	if(MaxLen == 0)
	{
		return 0;
	}
    R = CircularBuf->R;
    W = CircularBuf->W;
    
    Len = MaxLen;
    if(R == W)
    {
        DBG("e\n");
        return 0;
    }
    else if(R < W)
    {
        Len = W - R;
    }
    else
    {
        Len = CircularBuf->BufDepth + W - R;
    }
    
    if(Len > MaxLen)
    {
        Len = MaxLen;
    }
    

    if(Len + R > CircularBuf->BufDepth)
    {
        memcpy(OutBuf, (void *)&CircularBuf->CircularBuf[CircularBuf->R], CircularBuf->BufDepth - CircularBuf->R);
        memcpy((uint8_t* )OutBuf + CircularBuf->BufDepth - R, (void *)&CircularBuf->CircularBuf[0], Len + R - CircularBuf->BufDepth);
    }
    else
    {
        memcpy(OutBuf, (void *)&CircularBuf->CircularBuf[CircularBuf->R], Len);
    }
    
    R = (R + Len) % CircularBuf->BufDepth;
    
    CircularBuf->R = R;
        
    return Len;
}
