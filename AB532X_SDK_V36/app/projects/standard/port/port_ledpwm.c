#include "include.h"
#include "func.h"
#include "func_idle.h"

#if PWM_TIMER_EN					//timer pwm显示

#define RGB_MAX_DUTY				32				//RGB 最大占空比，单位0.1%
#define GUI_MAX_DUTY				1000			//GUI 显示最大占空比，单位0.1%
#define DUTY_SCAN_CNT				50				//颜色切换分辨率

#define PWM_RED_DUTY_SET(Duty) 		timer_pwm_config(PWM_TIMER5|PWM_CHANNEL2, 500, Duty, 0)
#define PWM_GREEN_DUTY_SET(Duty)	timer_pwm_config(PWM_TIMER5|PWM_CHANNEL1, 500, Duty, 0)
#define PWM_BLUE_DUTY_SET(Duty) 	timer_pwm_config(PWM_TIMER5|PWM_CHANNEL0, 500, Duty, 0)
#define PWM_GUI_DUTY_SET(Duty)		timer_pwm_config(PWM_TIMER4|PWM_CHANNEL2, 50000, Duty, 1)

typedef struct {
	uint8_t		red;
	uint8_t		green;
	uint8_t		blue;
}rgb_duty;

const rgb_duty rgb_duty_tbl[COLOUR_MAX] = {
	{0, 	0,		0,	}, 							//灭
	{0,		100,	100,},							//青
	{100,	0,		100,}, 							//紫
	{100,	15,		0,	}, 							//橙
	{100,	100,	100,}, 							//白
	{100,	50,		0,	}, 							//黄
};

rgb_duty	set_rgb, cur_rgb, range;
uint32_t	rgb_timer = 0, gui_timer = 0;
uint16_t	gui_period = DEF_SCAN_TIMER, rgb_period = DEF_SCAN_TIMER;


void pwm_gpio_init(void)
{
	GPIOADE |= (BIT(5)|BIT(6)|BIT(7));
	GPIOADIR &= ~(BIT(5)|BIT(6)|BIT(7));
	GPIOASET = (BIT(5)|BIT(6)|BIT(7));
	func_water_pump_process(0);
}
/*****************************************************************************
 函 数 名  : pwm_init
 功能描述  : pwm接口初始化
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年6月19日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void timer_pwm_init(void)
{
	pwm_gpio_init();
	PWM_GUI_DUTY_SET(0);
	timer_pwm_enable(PWM_TIMER4|PWM_CHANNEL2, TIMEPWMMAP_G2);
	PWM_RED_DUTY_SET(0);
	timer_pwm_enable(PWM_TIMER5|PWM_CHANNEL0, TIMEPWMMAP_G1);
	PWM_GREEN_DUTY_SET(0);
	timer_pwm_enable(PWM_TIMER5|PWM_CHANNEL1, TIMEPWMMAP_G1);
	PWM_BLUE_DUTY_SET(0);
	timer_pwm_enable(PWM_TIMER5|PWM_CHANNEL2, TIMEPWMMAP_G1);
	memset(&set_rgb, 0, sizeof(rgb_duty));
}

/*****************************************************************************
 函 数 名  : gui_light_scan
 功能描述  : gui显示LED扫描处理
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年6月22日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
static void gui_light_scan(void)
{
	static uint16_t gui_duty;
	static bool	direction;

	if (direction) {
		gui_duty += (GUI_MAX_DUTY/DUTY_SCAN_CNT);
		
		if (gui_duty >= GUI_MAX_DUTY) {
			direction = !direction;
		}
	}
	else {
		if (gui_duty >= (GUI_MAX_DUTY/DUTY_SCAN_CNT)) {
			gui_duty -= (GUI_MAX_DUTY/DUTY_SCAN_CNT);
		}
		else {
			direction = !direction;
		}
	}
	//printf ("gui set duty %d;\n", gui_duty);
	
	PWM_GUI_DUTY_SET(gui_duty);
}
/*****************************************************************************
 函 数 名  : pwm_rgb_duty_calc
 功能描述  : 计算当前应该显示的占空比
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年6月19日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
static void pwm_rgb_scan(void)
{
	static uint16_t red, green, blue;
	
	if (cur_rgb.red != set_rgb.red) {
		if (cur_rgb.red < set_rgb.red) {
			red += range.red*100/DUTY_SCAN_CNT;
		}
		else {
			red -= range.red*100/DUTY_SCAN_CNT;
		}
		cur_rgb.red = red/100;
		
		PWM_RED_DUTY_SET(cur_rgb.red);
	}

	if (cur_rgb.green != set_rgb.green) {
		if (cur_rgb.green < set_rgb.green) {
			green += range.green*100/DUTY_SCAN_CNT;
		}
		else {
			green -= range.green*100/DUTY_SCAN_CNT;
		}
		cur_rgb.green = green/100;
		
		PWM_GREEN_DUTY_SET(cur_rgb.green);
	}

	if (cur_rgb.blue != set_rgb.blue) {
		if (cur_rgb.blue < set_rgb.blue) {
			blue += range.blue*100/DUTY_SCAN_CNT;
		}
		else {
			blue -= range.blue*100/DUTY_SCAN_CNT;
		}
		cur_rgb.blue = blue/100;
		
		PWM_BLUE_DUTY_SET(cur_rgb.blue);
	}
}

/*****************************************************************************
 函 数 名  : pwm_gui_setting
 功能描述  : 设置gui 显示变化周期
 输入参数  : uint16_t period  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年6月24日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void pwm_gui_setting(uint16_t period)
{
	gui_period = period/DUTY_SCAN_CNT;
}

/*****************************************************************************
 函 数 名  : pwm_rgb_write
 功能描述  : 设置rgb 的显示颜色及变化周期
 输入参数  : uint8_t rgb_colour   
             uint16_t period_rgb  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年6月24日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void pwm_rgb_setting(uint8_t rgb_colour, uint16_t period_rgb)
{
	static uint8_t prev_disp_colour;

	rgb_period = period_rgb/DUTY_SCAN_CNT;

	if (prev_disp_colour != rgb_colour) {
		printf ("cur disp colour %d;\n", rgb_colour);
		prev_disp_colour = rgb_colour;
		set_rgb.red = RGB_MAX_DUTY*rgb_duty_tbl[rgb_colour].red/100;
		if (set_rgb.red >= cur_rgb.red) {
			range.red = (set_rgb.red - cur_rgb.red);
		}
		else {
			range.red = (cur_rgb.red - set_rgb.red);
		}
	 
		set_rgb.green = RGB_MAX_DUTY*rgb_duty_tbl[rgb_colour].green/100;
		if (set_rgb.green >= cur_rgb.green) {
			range.green = (set_rgb.green - cur_rgb.green);
		}
		else {
			range.green = (cur_rgb.green - set_rgb.green);
		}
		
		set_rgb.blue = RGB_MAX_DUTY*rgb_duty_tbl[rgb_colour].blue/100;
		if (set_rgb.blue >= cur_rgb.blue) {
			range.blue = (set_rgb.blue - cur_rgb.blue);
		}
		else {
			range.blue = (cur_rgb.blue - set_rgb.blue);
		}
	}
}

/*****************************************************************************
 函 数 名  : 显示处理
 功能描述  : 
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年6月19日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
AT(.text.ledpwm)
void func_led_pwm_scan(void)
{
	if (tick_check_expire(rgb_timer, rgb_period)) {
		rgb_timer = tick_get();
		pwm_rgb_scan();
	}

	if (tick_check_expire(gui_timer, gui_period)) {
		gui_timer = tick_get();
		gui_light_scan();
	}
}

#endif
