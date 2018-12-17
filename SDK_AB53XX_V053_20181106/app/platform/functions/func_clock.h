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

typedef struct {
    u8 type     : 1,                    //0: 时钟界面， 1：闹钟界面
       setting  : 1,                    //时间调整标志
       res      : 6;                    //保留

    u8 index;                           //时间调整index
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

#endif // _FUNC_CLOCK_H
