#include "include.h"

#if SD_SUPPORT_EN
AT(.com_text.sdio.init)
void sd_gpio_init(u8 type)
{
#if (SDCLK_MUX_DETECT_SD || SDCMD_MUX_DETECT_EN)
    //SDCLK/SDCMD复用检测时初始化
    if (type == 0) {
        SD_MUX_IO_INIT();
    #if SDCMD_MUX_DETECT_EN
        SD_CMD_MUX_PU300R();                    //正常通信，SDCMD上拉300R
    #endif
    } else if (type == 1) {
        SD_CLK_DIR_OUT();
    #if SDCMD_MUX_DETECT_EN
        SD_CMD_MUX_PU300R();                    //正常通信，SDCMD上拉300R
    #endif
    } else {
        SD_CLK_DIR_IN();
    #if SDCMD_MUX_DETECT_EN
        SD_CMD_MUX_PU10K();                     //IDLE状态，SDCMD上拉10K检测外设
    #endif
    }
#else // (SDCLK_MUX_DETECT_SD || SDCMD_MUX_DETECT_EN)
    //无复用检测时初始化
    if (type != 0) {
        return;
    }
    SD_IO_INIT();
#endif // (SDCLK_MUX_DETECT_SD || SDCMD_MUX_DETECT_EN)
}

#else
AT(.com_text.sdio.init)
void sd_gpio_init(u8 type)
{
}
#endif          // SD_SUPPORT_EN
