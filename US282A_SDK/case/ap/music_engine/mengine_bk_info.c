#include "app_mengine.h"

//写rtc域寄存器密码
void flush_rtc_reg(void)
{
    act_writel(0xA596, RTC_REGUPDATA);
    while (act_readl(RTC_REGUPDATA) != 0x5A69)
    {
        ;//qac
    }
}

//标记ESD RESTART标志：在正常进入播放时标记，如果能够正常退出播放，会调用 clear_esd_restart_tag 清除掉
void set_esd_restart_tag(void)
{
    act_writel(act_readl(RTC_BAK0) | (1 << MY_RTC_ESD_FLAG), RTC_BAK0);
    flush_rtc_reg();
}

//清除ESD RESTART标志：在正在退出播放时调用
void clear_esd_restart_tag(void)
{
    act_writel(act_readl(RTC_BAK0) & (~(1 << MY_RTC_ESD_FLAG)), RTC_BAK0);
    flush_rtc_reg();
}

//是否ESD RESTART
bool is_esd_restart(void)
{
    if ((act_readl(RTC_BAK0) & (1 << MY_RTC_ESD_FLAG)) != 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//断点恢复
bool get_bk_info(void)
{
    bool ret = FALSE;
    if (is_esd_restart() == TRUE)
    {
        g_eg_cfg_p->bk_infor.bp_file_offset = (int)act_readl(RTC_BAK1);
        g_eg_cfg_p->bk_infor.bp_time_offset = (int)act_readl(RTC_BAK2);
        ret = TRUE;
        libc_print("re-bk-info", g_eg_cfg_p->bk_infor.bp_file_offset, 2);
    }
    return ret;
}

