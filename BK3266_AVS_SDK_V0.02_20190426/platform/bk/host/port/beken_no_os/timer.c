/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#include "includes.h"
#include "app_beken_includes.h"
#include "lp_pdd_main.h" // add by cai.zhong 20190220 for audio data

static os_timer_t *timers_head = NULL;
static __VOLATILE__ uint64_t tick_counter = 0;
volatile uint32 flag_clr_wdt = 0;
volatile int16 s_sys_idle_cnt = 0;


#if (CONFIG_APP_MP3PLAYER == 1)
extern void MGC_AfsTimerExpired(void);
#define	UDISK_TIMER_TIMEOUT_VAL 1200// 12s
#define	SD_TIMER_TIMEOUT_VAL   400 // 4s
static volatile uint16 timeout_timer;
static uint8 timeoutEnableflag;
static uint8 mediaflag;

void enable_timeout_timer(uint8 flag)
{
	uint32 tmp,mask;

	mediaflag = flag;
	timeoutEnableflag = 1;
	VICMR_disable_interrupts(&tmp,&mask);
	timeout_timer = 0;
	VICMR_restore_interrupts(tmp,mask);
}
void disable_timeout_timer(void)
{
	uint32 tmp,mask;
	timeoutEnableflag = 0;
	VICMR_disable_interrupts(&tmp,&mask);
	timeout_timer = 0;
	VICMR_restore_interrupts(tmp,mask);
}

uint8 timeout_handle(void)
{
    if(timeoutEnableflag)
    {
    #ifdef CONFIG_APP_USB
        if((timeout_timer > UDISK_TIMER_TIMEOUT_VAL)&&(1 == mediaflag))
            return 1;
        else
    #endif
            if((timeout_timer > SD_TIMER_TIMEOUT_VAL)&&(0 == mediaflag))
            return 1;
        else
            return 0;
    }
    return 0;
}

/*uint8 test_timeout_print(void)
{
	uint8 ret=0;
	if(timeoutEnableflag)
	{
		if((timeout_timer > 8)&&(1 == mediaflag))
			ret = 1;
		else if((timeout_timer > 8)&&(0 == mediaflag))
			ret= 1;
		else
			ret= 0;
	}
	if(ret)
		os_printf("===read timeout:%x====\r\n",timeout_timer);
	return ret;
}*/
#endif
/* Initialize new timer */
result_t os_timer_init(os_timer_h *timer_h)
{
    DECLARE_FNAME("os_timer_init");

    *timer_h = (os_timer_h)jmalloc(sizeof(os_timer_t), M_ZERO);

    return UWE_OK;
}

/* Un-Initialize timer */
void os_timer_uninit(os_timer_h timer_h)
{
    os_timer_t *node = (os_timer_t *)timer_h;
    DECLARE_FNAME("os_timer_uninit");

    if (node->is_pending)
        os_timer_stop(timer_h);

    if (timer_h)
    {
        jfree(timer_h);
        timer_h = NULL;
    }
}

int os_timer_is_pending(os_timer_h timer_h)
{
    os_timer_t *node = (os_timer_t *)timer_h;

    return (node->is_pending);
}

/* Reset/Start timer with specific timeout (in msec) */
// the last parameter "arg" need be global
result_t os_timer_reset(os_timer_h timer_h, uint32_t msec, jthread_func func,
                        void *arg)
{
    os_timer_t *node = (os_timer_t *)timer_h;
    os_timer_t **iter = &timers_head;
    DECLARE_FNAME("os_timer_reset");

    if (node->is_pending)
        os_timer_stop(timer_h);

    node->is_pending = 1;
    node->ticks = (msec + (MSEC_IN_TICK - 1))/MSEC_IN_TICK;
    node->ticks += tick_counter;
    node->f = func;
    node->arg = arg;

    while (*iter && ((*iter)->ticks < node->ticks))
        iter = &(*iter)->next;

    node->next = *iter;
    *iter = node;

    return UWE_OK;
}

/* Stop timer */
void os_timer_stop(os_timer_h timer_h)
{
    os_timer_t *node = (os_timer_t *)timer_h;
    os_timer_t **iter = &timers_head;
    DECLARE_FNAME("os_timer_stop");

    if (!node->is_pending)
        return;

    node->is_pending = 0;

    while (*iter && *iter != node)
        iter = &(*iter)->next;

    if (*iter)
        *iter = (*iter)->next;
}

/* Check if timer is waiting to run */
bool_t os_timer_pending(os_timer_h timer_h)
{
    DECLARE_FNAME("os_timer_pending");
    return ((os_timer_t *)timer_h)->is_pending;
}

//#pragma push
//#pragma O0
static float loops_per_usec;

/* Gauge execution rate per clock tick */
void compute_cpu_speed(void)
{
#define LOOPS_FOR_COMPUTE 1000000
    __VOLATILE__ uint64_t a;
    __VOLATILE__ uint64_t b;
    __VOLATILE__ uint32_t count = LOOPS_FOR_COMPUTE;

    a = tick_counter;
    while (count)
        count--;

    b = tick_counter;

    loops_per_usec = LOOPS_FOR_COMPUTE /((b - a) * MSEC_IN_TICK);
    loops_per_usec /= 1000;
}

/* Delay execution for N micro-seconds (don't lose context) */
void os_delay_us(uint32_t usec) {
    __VOLATILE__ uint32_t loops;
    /* loops = usec * loops_per_usec; */
    /* while (loops) */
    /*     loops--; */
    while (usec--) {
        loops=10;
        while (loops--);
    }
}
//#pragma pop

void os_delay_ms (uint32_t msec) {        /**< 78mHz */
    __VOLATILE__ uint32_t loops;
    while (msec--) {
        loops=9700;
        while (loops--);
    }
}

/* Get current time (real or from system boot) */
void os_get_time(os_time_t *time)
{
    uint64_t timer = tick_counter;
    uint32_t tick_in_sec = 1000 / MSEC_IN_TICK;

    time->tv_sec = timer / tick_in_sec;
    time->tv_usec = (timer % tick_in_sec) * MSEC_IN_TICK * 1000;
}

/* Get ticks-in-second */
uint32_t os_get_hz(void)
{
    return 1000 / MSEC_IN_TICK;
}
__INLINE__ void set_system_idle_cnt(int16 cnt_10ms)
{
    s_sys_idle_cnt = cnt_10ms;
}
__INLINE__ int16 get_system_idle_cnt(void)
{
    return s_sys_idle_cnt;
}

#if (CONFIG_APP_ESD == 1)
static void app_esd_process(void)
{
	uint32 temp;
	static uint32 s_cnt_for = 0;
	
    if(++s_cnt_for % 200 == 0)
    {
        temp = BK3000_A5_CONFIG;
        BK3000_A5_CONFIG = temp;

        temp = BK3000_A6_CONFIG;
        BK3000_A6_CONFIG = temp;

        temp = BK3000_A7_CONFIG;
        BK3000_A7_CONFIG = temp;
    }	
}
#endif

extern volatile u_int32 syspwr_num_frames_to_sleep_bak;
#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
extern u_int8 syspwr_cpu_halt;
#endif

void timer_pwm1_pt2_isr(void)
{
    uint32 step;
    static uint64 last_tick_counter = 0;
    BK3000_PWM1_PT2_CONF |= bit_PWM_PT2_INT_FLAG;           /* clear the interrupt*/
    step = 1;
    sniff_tick_counter();
    tick_counter += step;
    timer_polling_handler(tick_counter-last_tick_counter);
    last_tick_counter = tick_counter;
    if (!sniffmode_wakeup_dly)
        bt_flag2_operate(APP_FLAG2_WAKEUP_DLY, 0);
    if (sniffmode_wakeup_dly)
        --sniffmode_wakeup_dly;
//    syspwr_num_frames_to_sleep_bak = 0;
#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
    syspwr_cpu_halt = 0;
#endif
    if (adcmute_cnt > 0)
        adcmute_cnt --;
    if(s_sys_idle_cnt > 0) // for close dac in idle;
        s_sys_idle_cnt--;
#if (CONFIG_AUD_FADE_IN_OUT == 1)
    aud_fade_in_out_process();
#endif

#if (CONFIG_APP_ESD == 1)
	app_esd_process();
#endif
}

void disable_pwm1_pt2(void) 
{
    BK3000_PWM1_PT2_CONF = 0;
}
void enable_pwm1_pt2(void) 
{
    BK3000_PWM1_PT2_CONF = (PWM1_PT2_COUNT << sft_PWM_PT2_CNT )
        | (1 << sft_PWM_PT2_ENABLE);
    //if (sniffmode_wakeup_dly)
	//    bt_flag2_operate(APP_FLAG2_WAKEUP_DLY, 1);

}
#if defined(CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE)
void sniff_enable_pwm1_pt2(void)
{
    //enable_pwm1_pt2();
    BK3000_PWM1_PT2_CONF = (PWM1_PT2_COUNT << sft_PWM_PT2_CNT )
        | (1 << sft_PWM_PT2_ENABLE);
    if (sniffmode_wakeup_dly)
	    bt_flag2_operate(APP_FLAG2_WAKEUP_DLY, 1);
    timer_pwm1_pt2_isr();

}

void sniff_tick_counter(void)
{
    uint32 step = 0;
#ifdef CONFIG_PWM_NOT_SLEEP
    extern uint8 g_sniff_flag;
    if ((syspwr_num_frames_to_sleep_bak) && (syspwr_cpu_halt) && (g_sniff_flag==2))
#else
    if ((syspwr_num_frames_to_sleep_bak) && (syspwr_cpu_halt) )
#endif
    {        
        step = (2*1250*(syspwr_num_frames_to_sleep_bak))/10000;
        tick_counter += step;        
        syspwr_num_frames_to_sleep_bak = 0;
    }
}
#else
void sniff_enable_pwm1_pt2(void)
{

}
void sniff_tick_counter(void)
{

}
#endif

void timer_pwm1_pt2_init(void)
{
    enable_pwm1_pt2();
    BK3000_PMU_PERI_PWDS &= ~(bit_PMU_PWM1_PWD | PMU_PWM1_CLK_SEL_ROSC);
    /* WDT CLK 1/16 32K */
    BK3000_PMU_PERI_PWDS &= ~(PMU_WDT_CLK_SEL_ROSC_DIV2);//wd timer select
}

void RAM_CODE timer_pwm1_pt2_swi(void)
{
    MSG_T msg;
    os_timer_t *tmp;

    while (timers_head && timers_head->ticks <= tick_counter)
    {
    	tmp = timers_head;
        timers_head = timers_head->next;
        tmp->is_pending = 0;
        msg.id = MSG_TIMER_PWM1_PT2_HDL;
        msg.hdl = (void *)tmp->f;
        msg.arg = (uint32)tmp->arg;        
        
        if(msg_lush_put(&msg)==MSG_FAILURE_RET)
        {
            os_timer_t **iter = &timers_head;
            tmp->is_pending = 1;
            tmp->ticks += 100;
            while (*iter && ((*iter)->ticks < tmp->ticks))
                iter = &(*iter)->next;

             tmp->next = *iter;
            *iter = tmp;
        }
    }
}

uint64_t os_get_tick_counter(void)
{
    return tick_counter;
}

void os_tick_delay(uint32 count)
{
    uint64_t t0 = os_get_tick_counter();

    while((t0 + (count)) > os_get_tick_counter())
    {
        os_printf("-");
    }
}

void timer_polling_handler(uint32 step)
{
#if 1
    flag_clr_wdt += step;
#else
    CLEAR_WDT;
#endif   
    sleep_tick += step;
    app_button_led_action(step);
    app_low_power_scanning(step);
    //app_smartvoice_state_detect(step); // add by cai.zhong 20190220 for audio data
    /* app_linein_scanning(); */
//	irDaKeyShortDeal(); 
    aud_clr_PAmute();
#if defined(CONFIG_APP_SDCARD)
    app_sd_scanning();
#endif
    timer_pwm1_pt2_swi();
#if (CONFIG_APP_MP3PLAYER == 1)
    if(timeoutEnableflag)
        timeout_timer += step;//timeout_timer++;
#ifdef CONFIG_APP_USB
    MGC_AfsTimerExpired();
#endif
#endif
}

void timer_clear_watch_dog(void)
{
    if(flag_clr_wdt > 100) // clr wdt :1s
    {
        CLEAR_WDT;
        flag_clr_wdt = 0;
    #if 1
        static uint8 test_i = 0;
        test_i ++;
        if(test_i >10)
        {
            test_i = 0;
            memory_usage_show();
        }
    #endif
    }
}

#ifdef CONFIG_TWS
void timer_pt1_start(uint32_t val)
{
	BK3000_PWM1_PT1_CONF = val<<sft_PWM_PT01_CNT_TO;
	BK3000_PWM1_PWM_CTRL = (1<<sft_PWM_PT1_ENABLE)|bit_PWM_PT1_INT_FLAG;
}
void pwm1_pt1_isr(void)
{
    uint32_t timer_status = BK3000_PWM1_PWM_CTRL;
    uint32_t oldmask = get_spr(SPR_VICMR(0));
    set_spr( SPR_VICMR(0), oldmask & 0x4 );

    if(timer_status&bit_PWM_PT1_INT_FLAG)
    {
        //STEREO_DEBUG_LOW;
        aud_open();
        bt_flag2_operate(APP_FLAG2_STEREO_PLAYING,1);
        BK3000_PWM1_PWM_CTRL = timer_status&(~(1<<sft_PWM_PT1_ENABLE));
        //STEREO2_DEBUG(1);
        sbc_show_all_status();
        //os_printf("b:%d,l:%d,n:%d,d:%d\n",aud_get_buffer_size(),sbc_remain(),sbc_src_num(),sbc_decode_cnt());
    }

    set_spr( SPR_VICMR(0), oldmask);

}


#endif


//#endif


/* end of file*/
