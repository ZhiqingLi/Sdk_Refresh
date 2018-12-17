#include "fm_drv.h"
#include "fm_qn8035.h"

//获取当前天线状态
int sFM_GetAnten(void* antenna, void* null2, void* null3)
{
    uint8* p_antenna;
    unsigned char adcdat;

    p_antenna = (uint8*) antenna;

    //Demo 用LRADC1 进行耳机检测
    adcdat = act_readb(LRADC1_DATA) & 0x3f;
    //天线连接上
    if ((adcdat >= 0x2B) && (adcdat < 0x34))
    {
        *p_antenna = 1;
    }
    else
    {
        *p_antenna = 0;
    }
    //获取状态成功
    return 0;
}

