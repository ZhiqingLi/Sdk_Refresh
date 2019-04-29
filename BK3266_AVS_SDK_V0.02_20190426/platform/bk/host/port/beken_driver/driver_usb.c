#include "driver_beken_includes.h"
#include "app_beken_includes.h"

#ifdef CONFIG_APP_USB 
extern uint8 MGC_MsdGetMediumstatus(void);
extern uint8 MUSB_GetConnect_Flag(void);
extern void MUSB_Host_init(void);
extern uint32 MUSB_NoneRunBackground(void);
extern void MUSB_Host_uninit(void);
extern uint32 MUSB_HfiRead(uint32 first_block, uint32 block_num, uint8 *dest);
extern void MGC_AfsUdsIsr(uint16 wIntrTxValue,uint16 wIntrRxValue,uint8 bIntrUsbValue);
extern uint32 get_HfiMedium_blksize(void);
extern uint32 get_HfiMedium_size(void);
extern int usb_sw_init(void);


static driver_udisk_t driver_udisk;

void pre_usb_init(void)
{
    uint32 tmp;

    BK3000_PMU_GATE_CFG = 0xFFFFFFFF;
    // open DPLL
    tmp = BK3000_A0_CONFIG;
    tmp &= ~((1<<0) |(1<<15) | (1<<20)| (1<<29) | (1<<31));// OPEN 48M, open  DPLL
    tmp |= (1<<15) | (1<<20) | (1<<31);
    BK3000_A0_CONFIG = tmp;  

    BK3000_XVR_REG_0x0E = 0xE0B99358; // bit30: DPLL clock for USB
    BK3000_XVR_REG_0x0F = 0x3B13B13B;
    BK3000_XVR_REG_0x0E = 0xE0B99358 | (1<<18);
    Delay(50);
    BK3000_XVR_REG_0x0E = 0xE0B99358 & (~(1<<18));   
    Delay(5000);
    
    BK3000_A0_CONFIG &= (~(1<<5));
    BK3000_PMU_PERI_PWDS &= (~bit_PMU_USB_PWD);
    Delay(20);
    //关闭所有USB 中断使能
    REG_USB_INTRRX1E = 0x0;
    REG_USB_INTRTX1E = 0x0;
    REG_USB_INTRUSBE = 0x0;
    REG_AHB2_USB_VTH &= ~(1<<7);
}

void post_usb_init(void)
{
    REG_AHB2_USB_OTG_CFG = 0x50; //USB 默认为HOST 模式 打开DP DN下拉
    REG_AHB2_USB_DEV_CFG = 0x00;
    Delay(500);
    uint32 ahb_int_state = REG_AHB2_USB_INT;
    Delay(500);
    REG_AHB2_USB_INT = ahb_int_state;
    Delay(500);

    //配置USB DP DN 模拟驱动配置
    REG_AHB2_USB_GEN |= (0x7<<4);   //  DP EN
    REG_AHB2_USB_GEN |= (0x7<<0);   //  DN EN
    Delay(5);

    //ananog_printf();

    memory_usage_show();
    usb_sw_init();//初始化
    memory_usage_show();
}

uint8 udisk_is_attached(void)
{
    app_env_handle_t env_h = app_env_get_handle();
    if(env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_USB_ENA)
    {
        return MUSB_GetConnect_Flag();
    }
    else
        return 0;
}

uint8 udisk_init(void)
{
    uint32 ret = USB_RET_ERROR;
    int ret_val;
    MSG_T msg;
    if(MGC_MsdGetMediumstatus())
        ret = USB_RET_OK;
    else
    {
        //os_printf("===musb host init===\r\n");
        enable_timeout_timer(1);
        MUSB_Host_init();
        driver_udisk.total_block = 0;
        driver_udisk.block_size = 0;
        driver_udisk.InitFlag = 1;
        while(1)
        {
            ret_val = msg_get(&msg);
            if(MSG_FAILURE_RET != ret_val)
            {
                if(msg.id != MSG_KEY_MODE)
                {
                    msg_put(msg.id);
                }
                else
                    break;
            }
            if(timeout_handle())
            {
                ret = USB_RET_ERROR ;
                break;
            }

            ret = MUSB_NoneRunBackground();
            if(MGC_MsdGetMediumstatus())
            {
                driver_udisk.total_block =get_HfiMedium_size();
                driver_udisk.block_size =get_HfiMedium_blksize();
                ret = USB_RET_OK;
                break;
            }
            else
            {
                if((USB_RET_DISCONNECT == ret)||(USB_RET_ERROR == ret))
                    break;
            }	
        }
    }
    os_printf("===udisk init end: ret=%x\r\n",ret);
    disable_timeout_timer();
    return ret;
}
void udisk_uninit(void)
{
    uint8 tmp0,tmp1,tmp2;
    tmp0 = REG_USB_INTRUSBE;
    tmp1 = REG_USB_INTRRX1E;
    tmp2 = REG_USB_INTRTX1E;
    if(driver_udisk.InitFlag == 1)
    {
        driver_udisk.InitFlag = 0;
    	 MUSB_Host_uninit();
    }
    if(!udisk_is_attached())
    {
    	REG_AHB2_USB_RESET = 1;
    	
    	REG_AHB2_USB_OTG_CFG = 0x50;
        Delay(5);
    	uint8 ahb_int_state = REG_AHB2_USB_INT;
    	Delay(5);
    	REG_AHB2_USB_INT = ahb_int_state;
    	Delay(5);
    	
    	REG_USB_INTRUSBE = tmp0;
    	REG_USB_INTRRX1E = tmp1;
    	REG_USB_INTRTX1E = tmp2;
    	
        REG_AHB2_USB_GEN |= (0x7<<4);   //  DP EN
        REG_AHB2_USB_GEN |= (0x7<<0);   //  DN EN
    }
}

//extern uint8 test_timeout_print(void);
int udisk_rd_blk_sync(uint32 first_block, uint32 block_num, uint8 *dest )
{
    int ret = USB_RET_ERROR;
    if(!MGC_MsdGetMediumstatus())
        return ret;
    enable_timeout_timer(1);	
    ret = MUSB_HfiRead(first_block,block_num,dest);
    if(ret)
        goto Exit;

    while(1)
    {
        if(timeout_handle())
        {
            ret = USB_RET_ERROR ;
            break;
        }

        ret = MUSB_NoneRunBackground();
        if( (USB_RET_ERROR == ret) 
            ||(USB_RET_DISCONNECT == ret) 
            ||(USB_RET_READ_OK == ret) )
            break;
    }

    if(USB_RET_READ_OK == ret)
        ret = USB_RET_OK;
    else
        ret = USB_RET_ERROR;
Exit:
    //test_timeout_print();
    disable_timeout_timer();
    return ret;
}
uint32 udisk_get_size(void)
{
    return driver_udisk.total_block;
}

void usb_isr(void)
{
    uint8 ahb_int_state = REG_AHB2_USB_INT;
    uint8 bIntrUsbValue = REG_USB_INTRUSB;
    uint16 wIntrTxValue = REG_USB_INTRTX1 | (REG_USB_INTRTX2 << 8 );
    uint16 wIntrRxValue = REG_USB_INTRRX1 | (REG_USB_INTRRX1 << 8 );

#if 0//def CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
    extern boolean SYSpwr_Is_Low_Power_Mode_Active(void);
    extern void SYSpwr_Handle_Early_Wakeup(void);
    
    app_exit_sniff_mode();
    SYSpwr_Handle_Early_Wakeup();
#endif
    (void) ahb_int_state;
    bIntrUsbValue &= ~(0xc0);
    MGC_AfsUdsIsr(wIntrTxValue,wIntrRxValue,bIntrUsbValue);	
}
#endif
