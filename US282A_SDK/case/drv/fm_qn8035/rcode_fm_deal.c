#include "fm_drv.h"
#include "fm_qn8035.h"

//设置数据的全局变量
set_value_t g_set_value;

FM_init_parm_t fm_init_parm;

uint8 qnd_xtal_div0, qnd_xtal_div1, qnd_xtal_div2;


//QN8035模组bug，855设置不成功
uint8 g_freq_855;
//设置频点
//freq: 输入的频率值(单位:kHz)
//返回值: 0->成功, -1->失败
int sFM_SetFreq(uint32 freq, void* null2, void* null3)
{
    PRINTD_DBG_INT("SetFreq:", freq);
    QND_TuneToCH((uint16) (freq / 10));
    return 0;
}

//设置波段
int __section__(".bank") sFM_SetBand(radio_band_e band, void* null2, void* null3)
{
    //int result;
    //PRINTD_DBG_INT("FM_SetBand=", band);
    if (band == Band_Japan)
    {
        //日本波段
        g_set_value.band = 1;
        //步长
        g_set_value.span = 100;
        g_set_value.step = QND_FSTEP_100KHZ;
        //最低频率
        g_set_value.freq_low = 76000;
        //最高频率
        g_set_value.freq_high = 90000;

    }
    else if (band == Band_Europe)
    {
        //欧洲频段步长50
        g_set_value.band = 0;
        //步长
        g_set_value.span = 50;
        g_set_value.step = QND_FSTEP_50KHZ;
        //最低频率
        g_set_value.freq_low = 87500;
        //最高频率
        g_set_value.freq_high = 108000;
    }
    else
    {
        //普通频段及其他
        g_set_value.band = 0;
        //步长
        g_set_value.span = 100;
        g_set_value.step = QND_FSTEP_100KHZ;
        //最低频率
        g_set_value.freq_low = 87500;
        //最高频率
        g_set_value.freq_high = 108000;
    }

    return 0;
}

//静音/解除静音
int __section__(".bank") sFM_Mute(FM_MUTE_e mode, void* null2, void* null3)
{
    QNF_SetMute(mode);
    return 0;
}

//设置门限
int __section__(".bank") sFM_SetThrod(uint8 level, void* null2, void* null3)
{
    //设置门限
    QND_RXSetTH(level);
    return 0;
}

//软件搜台，判断是否有效电台
//freq: 输入的频率值
//direct: 搜台方向
//返回值：  -1:没信号
//          0: 有信号
//          1: 立体声
int sFM_Search(uint32 freq, uint8 direct, void* null3)
{
#if(HARD_SEEK == 0)
    int result;

    PRINTD_DBG_INT("频率(kHz):", freq);
    result = QND_RXValidCH(freq / 10, g_set_value.step);
    if(result == 1)
    {
        return 0;
    }
#endif

    return -1;

}

//启动硬件搜台
//freq: 输入的频率值
int sFM_HardSeek(uint32 freq, uint8 direct, void* null3)
{
#if(HARD_SEEK != 0)
    uint16 val;
    if(direct != 0)
    {
        val = g_set_value.freq_high / 10;
    }
    else
    {
        val =g_set_value.freq_low / 10;
    }
    QND_RXSeekCH((uint16) (freq / 10), val, g_set_value.step);
#endif
    return 0;
}

//硬件seek 过程中，手动停止seek 过程
int sFM_BreakSeek(void* null1, void* null2, void* null3)
{
#if(HARD_SEEK != 0)

#endif
    return 0;
}

//获取硬件seek 过程是否结束标志
int sFM_GetHardSeekflag(void* flag, void* null2, void* null3)
{
#if(HARD_SEEK != 0)
    uint8* pend = (uint8 *) flag;
    uint8 regValue = QND_ReadReg(SYSTEM1);
    //if it seeks a potential channel, the loop will be quited
    if ((regValue & CHSC) == 0)
    {
        //搜完了
        *pend = 1;
    }
    else
    {
        *pend = 0;
    }

    if (*pend != 0)
    {
        uint8 snr = QND_ReadReg(SNR);
        uint8 rssi = QND_ReadReg(RSSISIG);
        uint8 failed = (QND_ReadReg(STATUS1) & RXCCA_FAIL);

        PRINTD_DBG_INT("locked:", QNF_GetCh() * 10);
        PRINTD_DBG_INT("", (failed << 16) | (snr << 8) | rssi);

        if((snr > 21) && (rssi > 69) && (failed == 0))
        {
            *pend |= (1 << 1);
        }
    }
#endif
    return 0;
}

//获取当前频率
int sFM_GetStatus(void * pstruct_buf, uint8 mode, void* null3)
{
    Engine_Status_t * ptr_buf = (Engine_Status_t *) pstruct_buf;

    ptr_buf->FM_CurrentFreq = QNF_GetCh() * 10;

    return 0;
}

//获取当前波段信息
int __section__(".bank") sFM_GetBand(void* band, void* null2, void* null3)
{
    radio_band_e* bandinfo;
    int result = 0;

    bandinfo = (radio_band_e*) band;

    //不用去取，直接从驱动层全局变量获取到
    //中国/ 美国电台
    //频段
    if (g_set_value.band == 1)
    {
        *bandinfo = Band_Japan;
    }
    else if ((g_set_value.band == 0) && (g_set_value.span == 50))
    {
        *bandinfo = Band_Europe;
    }
    else
    {
        *bandinfo = Band_China_US;
    }
    return result;

}

//获取当前频率
int sFM_GetFreq(void* freq, void* null2, void* null3)
{
    uint32* point_freq = (uint32*) freq;
    *point_freq = QNF_GetCh() * 10;
    if(g_freq_855)
    {
        //g_freq_855 = 0;
        *point_freq = 85500;
    }
    return 0;
}

//获取当前电台信号强度
int __section__(".bank") sFM_GetIntsity(void* intensity, void* null2, void* null3)
{
    return 0;
}

//获取当前电台立体声信息
int __section__(".bank") sFM_GetStereo(void* stereo, void* null2, void* null3)
{
    return 0;
}

int __section__(".bank") sFM_Debug(void* null1, void* null2, void* null3)
{
    PRINTD_DBG_INT("SNR = ", (uint32)QND_ReadReg(SNR));
    PRINTD_DBG_INT("RSSISIG = ", (uint32)QND_ReadReg(RSSISIG));
    return 0;
}
