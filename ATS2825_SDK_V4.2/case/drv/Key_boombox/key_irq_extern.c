#include "key_inner.h"

//外部中断服务程序
#if defined(SUPPORT_EXTERNAL_INTERRUPT)

void notify_key_msg(void)
{
#if defined(BOARD_LINKPLAY_FAR_DEMO) || defined(BOARD_DPI_7C370_FAR) || defined(BOARD_JL_36410_FAR)
    private_msg_t msg;

    msg.msg.type = MSG_SMART_VOICE_TRIGGER;
    msg.msg.content.addr = NULL;
    sys_mq_send(MQ_ID_SYS, (void *)&msg);
#endif
}

void SIRQ_int_process(void)
{
#if (EXTERN_INTERRUPT_PIN_B11 == 1 || EXTERN_INTERRUPT_PIN_A22 == 1)    //SIRQ0
    if((act_readl(INTC_EXTIP) & (1 << INTC_EXTIP_E0PD)) != 0) {
        //清除中断标志
        act_writel(act_readl(INTC_EXTIP) | (1 << INTC_EXTIP_E0PD), INTC_EXTIP);

        notify_key_msg();
    }
#endif

#if (EXTERN_INTERRUPT_PIN_A1 == 1 || EXTERN_INTERRUPT_PIN_A7 == 1)      //SIRQ1
    if((act_readl(INTC_EXTIP) & (1 << INTC_EXTIP_E1PD)) == 0) {
        //清除中断标志
        act_writel(act_readl(INTC_EXTIP) | (1 << INTC_EXTIP_E1PD), INTC_EXTIP);
        notify_key_msg();
    }

#endif
}

#endif

