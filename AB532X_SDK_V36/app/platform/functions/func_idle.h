#ifndef _FUNC_IDLE_H
#define _FUNC_IDLE_H

#define	WAVE_MIN_FREQ					490			
#define	WAVE_MAX_FREQ					510
#define	WAVE_STILL_FREQ					500
#define WAVE_MAX_OPERATION				80

#define DEF_SCAN_TIMER					1000
#define GUI_IDLE_TIMER					5000
#define GUI_WORK_TIMER					2000				
#define GUI_ERRO_TIMER					500
#define RGB_SWAP_TIMER					7000			//全彩颜色切换时间
#define RGB_STOP_TIMER					5000			//全彩颜色停留时间

enum {													//与rgb_duty_tbl对应
	COLOUR_IDLE = 0,
	COLOUR_CYAN,
	COLOUR_PURPLE,
	COLOUR_ORANGE,
	COLOUR_WHITE,
	COLOUR_YELLOW,
	COLOUR_MAX,
};											

typedef struct {
	gpio_t		wave_gpio;
	uint16_t	wave_freq;
	uint32_t	wave_timer;
	uint32_t	rgb_blink;
	uint8_t		rgb_colour;
	uint8_t		wave_cnt;
	uint8_t		cur_status;
	bool		out_level;
	bool		is_wave_wait;
	uint32_t	wave_wait_timer;
}func_idle_t;

extern func_idle_t		f_idle;

void func_idle_message(u16 msg);
void func_water_pump_process(bool work);
uint8_t func_idle_work_state_get(void);


#if (GUI_SELECT != GUI_NO)
void func_idle_display(void);
#else
#define func_idle_display()
#endif

#endif // _FUNC_IDLE_H
