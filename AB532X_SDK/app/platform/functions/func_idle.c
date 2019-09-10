#include "include.h"
#include "func.h"
#include "func_idle.h"

#if FUNC_IDLE_EN

typedef struct {
	uint32_t Timer;
	uint16_t Freq;
}wave_param;

const wave_param wave_tbl[WAVE_MAX_OPERATION] = {
	{6000*3, WAVE_MIN_FREQ},    //49HZ显示18S
	{330, WAVE_MIN_FREQ+1},		//以下从49.1HZ开始每300毫秒增加0.1HZ，直到50HZ
	{330, WAVE_MIN_FREQ+2},        
	{330, WAVE_MIN_FREQ+3},
	{330, WAVE_MIN_FREQ+4},
	{330, WAVE_MIN_FREQ+5},
	{330, WAVE_MIN_FREQ+6},
	{330, WAVE_MIN_FREQ+7},        
	{330, WAVE_MIN_FREQ+8},
	{330, WAVE_MIN_FREQ+9},
	
	{3000, WAVE_STILL_FREQ},	//在50HZ停留3秒       
	{330, WAVE_MAX_FREQ-9},		//从50.1HZ开始每300h毫秒增加0.1HZ，直到51HZ
	{330, WAVE_MAX_FREQ-8},
	{330, WAVE_MAX_FREQ-7},
	{330, WAVE_MAX_FREQ-6},
	{330, WAVE_MAX_FREQ-5},        
	{330, WAVE_MAX_FREQ-4},
	{330, WAVE_MAX_FREQ-3},
	{330, WAVE_MAX_FREQ-2},
	{330, WAVE_MAX_FREQ-1},  

	{5000, WAVE_MAX_FREQ},		//在51HZ停留5秒      
	{330, WAVE_MAX_FREQ-1},     //从51HZ开始，每300毫秒减少0.1HZ，直到50.1HZ
	{330, WAVE_MAX_FREQ-2},
	{330, WAVE_MAX_FREQ-3},
	{330, WAVE_MAX_FREQ-4},
	{330, WAVE_MAX_FREQ-5},
	{330, WAVE_MAX_FREQ-6},
	{330, WAVE_MAX_FREQ-7},
	{330, WAVE_MAX_FREQ-8},
	{330, WAVE_MAX_FREQ-9},

	{1000, WAVE_STILL_FREQ},	//在50HZ停留1秒
	{500, WAVE_MIN_FREQ+9},		//从50.1HZ开始，每500毫秒减少0.1HZ，直到49.1HZ
	{500, WAVE_MIN_FREQ+8},
	{500, WAVE_MIN_FREQ+7},
	{500, WAVE_MIN_FREQ+6},
	{500, WAVE_MIN_FREQ+5},
	{500, WAVE_MIN_FREQ+4},
	{500, WAVE_MIN_FREQ+3},   
	{500, WAVE_MIN_FREQ+2},        
	{500, WAVE_MIN_FREQ+1},

	{5000*2, WAVE_MIN_FREQ},    //49HZ显示10S
	{400, WAVE_MIN_FREQ+1},		//以下从49.1HZ开始每400毫秒增加0.1HZ，直到50HZ
	{400, WAVE_MIN_FREQ+2},        
	{400, WAVE_MIN_FREQ+3},
	{400, WAVE_MIN_FREQ+4},
	{400, WAVE_MIN_FREQ+5},
	{400, WAVE_MIN_FREQ+6},
	{400, WAVE_MIN_FREQ+7},        
	{400, WAVE_MIN_FREQ+8},
	{400, WAVE_MIN_FREQ+9},

	{400, WAVE_STILL_FREQ},		//从50HZ开始每400h毫秒增加0.1HZ，直到51HZ   
	{400, WAVE_MAX_FREQ-9},		
	{400, WAVE_MAX_FREQ-8},
	{400, WAVE_MAX_FREQ-7},
	{400, WAVE_MAX_FREQ-6},
	{400, WAVE_MAX_FREQ-5},        
	{400, WAVE_MAX_FREQ-4},
	{400, WAVE_MAX_FREQ-3},
	{400, WAVE_MAX_FREQ-2},
	{400, WAVE_MAX_FREQ-1},

	{500, WAVE_MAX_FREQ},		//从51HZ开始，每500毫秒减少0.1HZ，直到50.1HZ    
	{500, WAVE_MAX_FREQ-1},     
	{500, WAVE_MAX_FREQ-2},
	{500, WAVE_MAX_FREQ-3},
	{500, WAVE_MAX_FREQ-4},
	{500, WAVE_MAX_FREQ-5},
	{500, WAVE_MAX_FREQ-6},
	{500, WAVE_MAX_FREQ-7},
	{500, WAVE_MAX_FREQ-8},
	{500, WAVE_MAX_FREQ-9},

	{500, WAVE_STILL_FREQ},		//从50HZ开始，每500毫秒减少0.1HZ，直到49.1HZ
	{500, WAVE_MIN_FREQ+9},		
	{500, WAVE_MIN_FREQ+8},
	{500, WAVE_MIN_FREQ+7},
	{500, WAVE_MIN_FREQ+6},
	{500, WAVE_MIN_FREQ+5},
	{500, WAVE_MIN_FREQ+4},
	{500, WAVE_MIN_FREQ+3},   
	{500, WAVE_MIN_FREQ+2},        
	{500, WAVE_MIN_FREQ+1},
};

func_idle_t		f_idle;

AT(.com_rodata.isr)
const char str_t3[] = "T3->";

AT(.com_text.isr)
void timer3_isr(void)
{
    if (TMR3CON & BIT(16)) {
        TMR3CPND = BIT(16);              //Clear Pending
    }

    if ((0 != f_idle.wave_freq)
    && (10000000/(f_idle.wave_freq*2)-1 != TMR3PR)) {
    	TMR3PR = 10000000/(f_idle.wave_freq*2) - 1;
    }

	f_idle.out_level = !f_idle.out_level;
}

void timer3_init(void)
{
	TMR3CON =  BIT(7);                  //Timer overflow interrupt enable
	TMR3CNT = 0;
	TMR3PR  = 1000000/100 - 1;          //10ms, select xosc26_div 1M clk
	TMR3CON |= BIT(2) | BIT(0);         //Timer works in Counter Mode
    sys_irq_init(IRQ_TMR3_IRRX_VECTOR, 1, timer3_isr);

    bsp_gpio_cfg_init(&f_idle.wave_gpio, 24);
    if (NULL == f_idle.wave_gpio.sfr) {
    	printf ("pwm gpio config failed!!!\n");
    }
    printf ("wave num = %d;\n", f_idle.wave_gpio.num);
}

AT(.text.func.idle)
void func_pwm_wave_process(void)
{
	static bool	out_level;
	gpio_t *p;

	p = &f_idle.wave_gpio;
	if ((p->sfr == NULL) || (out_level == f_idle.out_level)) {
		return;
	}
    out_level = f_idle.out_level;
	p->sfr[GPIOxDE] |= BIT(p->num);
    p->sfr[GPIOxDIR] &= ~BIT(p->num);
    if (out_level) {
    	p->sfr[GPIOxSET] = BIT(p->num);
    }
    else {
    	p->sfr[GPIOxCLR] = BIT(p->num);
    }
}

AT(.text.func.idle)
void func_water_pump_process(bool work)
{
	GPIOFDE |= BIT(0);
	GPIOFDIR &= ~BIT(0);
	if (work) {
		GPIOFSET = BIT(0);
	}
	else {
		GPIOFCLR = BIT(0);
	}
}

AT(.text.func.idle)
uint8_t func_idle_work_state_get(void)
{
	return f_idle.cur_status;
}


AT(.text.func.idle)
void func_idle_process(void)
{
    func_process();

    switch (f_idle.cur_status) {
        case 1:
            pwm_gui_setting(GUI_WORK_TIMER);
            pwm_rgb_setting(COLOUR_WHITE, DEF_SCAN_TIMER);
            break;
            
        case 2:
            pwm_gui_setting(GUI_WORK_TIMER);
            pwm_rgb_setting(COLOUR_YELLOW, DEF_SCAN_TIMER);
            break;

		case 3:
        	if (tick_check_expire(f_idle.rgb_blink, RGB_SWAP_TIMER+RGB_STOP_TIMER)) {
        		f_idle.rgb_blink = tick_get();
        		f_idle.rgb_colour++;
        		f_idle.rgb_colour %= COLOUR_MAX;
        		if (COLOUR_IDLE == f_idle.rgb_colour) {
        			f_idle.rgb_colour++;
        		}
        		pwm_rgb_setting(f_idle.rgb_colour, RGB_SWAP_TIMER);
        	}
            pwm_gui_setting(GUI_WORK_TIMER);
            break;
            
        case 4:
            pwm_gui_setting(GUI_ERRO_TIMER);
            pwm_rgb_setting(COLOUR_IDLE, DEF_SCAN_TIMER);
            break;
            
        default:
        	f_idle.rgb_colour = COLOUR_IDLE;
        	pwm_gui_setting(GUI_IDLE_TIMER);
            pwm_rgb_setting(f_idle.rgb_colour, DEF_SCAN_TIMER);
            break;
    }

	if (!f_idle.cur_status || (4 == f_idle.cur_status)) {
#if USER_EXT_POWERON_EN
		EXT_GPIO_POWEROFF(); 			
#endif //USER_EXT_POWERON_EN

		if (f_idle.is_wave_wait) {
			f_idle.is_wave_wait = 0;
			f_idle.wave_wait_timer = tick_get();
		}
		//水泵延时0.5S关闭
		if (!f_idle.is_wave_wait && tick_check_expire(f_idle.wave_wait_timer, 500)) {
			func_water_pump_process(0);
		}
		bsp_loudspeaker_mute();
    	f_idle.wave_cnt = 0;
    	f_idle.out_level = 0;
    }
    else {
#if USER_EXT_POWERON_EN
		EXT_GPIO_POWERON();			
#endif //USER_EXT_POWERON_EN
    	func_water_pump_process(1);
    	bsp_loudspeaker_unmute();

    	if (!f_idle.is_wave_wait) {
    		f_idle.is_wave_wait = 1;
    		f_idle.wave_wait_timer = tick_get();
    	}
    	//喇叭输出延时3S启动
		if (f_idle.is_wave_wait && tick_check_expire(f_idle.wave_wait_timer, 3000)) {
			if (tick_check_expire(f_idle.wave_timer, wave_tbl[f_idle.wave_cnt].Timer)) {
				f_idle.wave_cnt++;
				f_idle.wave_cnt %= WAVE_MAX_OPERATION;
				f_idle.wave_timer = tick_get();
				f_idle.wave_freq = wave_tbl[f_idle.wave_cnt].Freq;
			}

			func_pwm_wave_process();
		}
	}
}

static void func_idle_enter(void)
{
#if PLUGIN_FUNC_IDLE_ENTER_CHECK
    if (!plugin_func_idle_enter_check()) {  //可以处理开机, 判断是否要停在idle模式, 等待设备插入的方案.
        return;
    }
#endif

#if BT_PWRKEY_5S_DISCOVER_EN
	if(bsp_bt_pwrkey5s_check()) {
		func_cb.sta = FUNC_NULL;
	}
	bsp_bt_pwrkey5s_clr();
#endif

	memset(&f_idle, 0, sizeof(func_idle_t));
	f_idle.wave_timer = tick_get();
	f_idle.wave_wait_timer = tick_get();
	f_idle.wave_freq = wave_tbl[f_idle.wave_cnt].Freq;
	timer3_init();
    led_idle();
}

static void func_idle_exit(void)
{
    func_cb.last = FUNC_IDLE;
    func_water_pump_process(0);
}

AT(.text.func.idle)
void func_idle(void)
{
    printf("%s\n", __func__);

    func_idle_enter();

    while (func_cb.sta == FUNC_IDLE) {
        func_idle_process();
        func_idle_message(msg_dequeue());
        func_idle_display();
    }

    func_idle_exit();
}

#endif
