#include "fm_drv.h"
#include "fm_qn8035.h"

//FM初始化: 设置搜索门限 * \retval   成功 返回0;
//返回值: 失败->-1, 成功->0
int sFM_Init(radio_band_e band, uint8 level, uint32 freq)
{
    if (QND_Init() != (UINT8) -1)
    {
        QNF_SetRegBit(SYSTEM1, STNBY_RX_MASK, RX_MODE);
        sFM_Mute(SetMUTE, 0, 0);
        //音量设置到0dB(最大)
        QNF_SetRegBit(VOL_CTL, 0x3f, 0x07);
        //设置搜台门限值
        QND_RXSetTH(level);

        return 0;
    }

    return -1;
}

//fm休眠
int sFM_Standby(void* null1, void* null2, void* null3)
{
    //make sure that Power down control by FSM control
    QNF_SetRegBit(REG_DAC, 0x08, 0x00);
    //enter mute
    //QNF_SetMute(SetMUTE);
    //QNF_SetRegBit(REG_DAC, 0x0b, 0x0b);
    //SYSTEM1 bit5 enter standby
    QNF_SetRegBit(SYSTEM1, STNBY_RX_MASK, STNBY_MODE);
    return 0;
}

