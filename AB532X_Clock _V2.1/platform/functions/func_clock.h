#ifndef _FUNC_CLOCK_H
#define _FUNC_CLOCK_H

typedef struct {
	u8 tm_sec;
	u8 tm_min;
	u8 tm_hour;
	u8 tm_mday;
	u8 tm_mon;
	u8 tm_year;
	u8 tm_wday;
} rtc_time_t;
extern rtc_time_t rtc_tm;
extern rtc_time_t rtcalm_tm1, rtcalm_tm2, rtc_temp;

typedef struct {
    u8 type     	 : 1,                   //0: 时钟界面， 1：闹钟界面
       setting  	 : 1,                   //时间调整标志
       alarm_type	 : 2,					//闹钟类型，关闭，第一个 第二个 两个
       rtc_set_t	 : 2,					//设置哪种模式
       rtc_show_delay: 2;					//调整时钟时候不闪烁时间

    u8 index;                           	//时间调整index
    u8 alarm_flag	 : 1,					//闹钟有效
	   alarm_time_out: 1,					//闹钟time时间到
	   alarm_poweron : 1;					//闹钟唤醒开机
	u8 alarm_play_cnt;						//闹钟响铃次数  
	u8 sleep_mode;
} time_cb_t;
extern time_cb_t tm_cb;

void func_clock_message(u16 msg);
void rtc_clock_init(void);
void rtc_set_alarm_relative_time(u32 nsec);
void rtc_set_alarm_wakeup(u32 nsec);

#if (GUI_SELECT != GUI_NO)
void func_clock_display(void);
void func_clock_enter_display(void);
void func_clock_exit_display(void);
#else
#define func_clock_display()
#define func_clock_enter_display()
#define func_clock_exit_display()
#endif //(GUI_SELECT != GUI_NO)


void clock_user_init(void);
void gui_disp_clock(void);
unsigned char clock_message(u16 msg);
void clock_alarm_set_value(void);
void clock_alarm_timeout(void);


#endif // _FUNC_CLOCK_H
