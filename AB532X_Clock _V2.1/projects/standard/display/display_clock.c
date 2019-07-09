#include "include.h"
#include "func.h"
#include "func_clock.h"

#if ((GUI_SELECT != GUI_NO) && (FUNC_CLOCK_EN))
AT(.text.func.clock.display)
void func_clock_display(void)
{
    gui_box_process();

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
        if (tm_cb.setting) {
            gui_display(DISP_TIME_SET);
        } else {
            gui_display(DISP_RTCTIME);
        }
        break;
    default:
        gui_box_display();
        break;
    }
}

AT(.text.func.clock.display)
void func_clock_enter_display(void)
{
	rtc_time_to_tm(irtc_time_read(RTCCNT_CMD), &rtc_tm);
    gui_display(DISP_RTCTIME);
}

AT(.text.func.clock.display)
void func_clock_exit_display(void)
{
	
}


//---------------------------------------------------------------------------------------
//
AT(.text.ledseg)
u32 alarm_number_deal(rtc_time_t rtc_tm)
{
	rtc_time_t time;
	u32 temp_alarm_deal = 0,temp_time = 0;
	temp_alarm_deal = ((rtc_tm.tm_hour * 60 * 60) + (rtc_tm.tm_min * 60));//闹钟设置时间，转化为S
	rtc_time_to_tm(irtc_time_read(RTCCNT_CMD), &time);//读取当前时间
	temp_time = ((time.tm_hour * 60 * 60) + (time.tm_min * 60));//当前时间，转化为S
	if(temp_alarm_deal > temp_time){
		temp_alarm_deal = temp_alarm_deal - temp_time;
	}else{
		temp_alarm_deal = (24 * 60 * 60) - (temp_time - temp_alarm_deal);
	}
	return (temp_alarm_deal - time.tm_sec); //返回设置闹钟的数值
}

//---------------------------------------------------------------------------------------
//
AT(.text.ledseg)
void clock_alarm_set_value(void)
{
	if (!tm_cb.setting) {
		if(tm_cb.alarm_type == 1){
			rtc_set_alarm_relative_time(alarm_number_deal(rtcalm_tm1));
		}

		if(tm_cb.alarm_type == 2){
			rtc_set_alarm_relative_time(alarm_number_deal(rtcalm_tm2));
		}
		
		if(tm_cb.alarm_type == 3){
			unsigned int rtc_value, ralm1_value, ralm2_value;

			rtc_time_to_tm(irtc_time_read(RTCCNT_CMD), &rtc_tm);		 //更新时间结构体
			rtc_value = rtc_tm.tm_hour*60 + rtc_tm.tm_min;
			ralm1_value = rtcalm_tm1.tm_hour*60 + rtcalm_tm1.tm_min;
			ralm2_value = rtcalm_tm2.tm_hour*60 + rtcalm_tm2.tm_min;

			if((rtc_value >= ralm1_value) && (rtc_value >= ralm2_value)){
				if(ralm1_value <= ralm2_value){
					rtc_set_alarm_relative_time(alarm_number_deal(rtcalm_tm1));
				}else{
					rtc_set_alarm_relative_time(alarm_number_deal(rtcalm_tm2));
				}
			}else if((rtc_value < ralm1_value) && (rtc_value < ralm2_value)){
				if(ralm1_value <= ralm2_value){
					rtc_set_alarm_relative_time(alarm_number_deal(rtcalm_tm1));
				}else{
					rtc_set_alarm_relative_time(alarm_number_deal(rtcalm_tm2));
				}
			}else{
				if(ralm1_value >= ralm2_value){
					rtc_set_alarm_relative_time(alarm_number_deal(rtcalm_tm1));
				}else{
					rtc_set_alarm_relative_time(alarm_number_deal(rtcalm_tm2));
				}
			}
		}
	}
}

//---------------------------------------------------------------------------------------
//
AT(.text.ledseg)
void clock_alarm_timeout(void)
{
	if (RTCCON & BIT(17)) {
        RTCCPND = BIT(17);                      //clear RTC alarm pending
        irtc_sfr_write(RTCCON8_CMD, 0x02);      //clear alarm pending
		printf("Alarm Timeout\r\n");
        if(tm_cb.alarm_type){
        	printf("Alarm tyep1\r\n");
			clock_alarm_set_value();
			tm_cb.alarm_time_out = 1;
			tm_cb.alarm_play_cnt = 0;
        }
    }
}
//---------------------------------------------------------------------------------------
//
AT(.text.ledseg)
void clock_user_init(void)
{
    memset(&tm_cb, 0, sizeof(tm_cb));
    sys_cb.rtc_first_pwron = 0;
}


//---------------------------------------------------------------------------------------
//
AT(.text.ledseg)
void gui_disp_clock(void)
{
	if (tm_cb.setting) {
		gui_display(DISP_TIME_SET);
	} else {
		tm_cb.rtc_set_t = 0;
		gui_display(DISP_RTCTIME);
	}
}




//---------------------------------------------------------------------------------------
//clock message
AT(.text.ledseg)
unsigned char clock_message(u16 msg)
{
	if((tm_cb.alarm_time_out == 1) && (msg > 0x7ff)){
		tm_cb.alarm_time_out = 0;
		if(tm_cb.alarm_poweron == 1){
			msg = KLH_POWER;
			msg_enqueue(msg);
		}
		msg = 0;
	}

	switch(msg){
		case KU_CLOCK:
			if(tm_cb.setting == 0){
				if((tm_cb.alarm_type == 0) || (box_cb.sta == GUI_BOX_ALRAM_1) || (box_cb.sta == GUI_BOX_ALRAM_2)){
					tm_cb.alarm_type++;
				}
				switch(tm_cb.alarm_type){
					case 1:
						tm_cb.rtc_set_t = 1;
						gui_box_show_alarm_one();
						break;
					case 2:						
						tm_cb.rtc_set_t = 2;
						gui_box_show_alarm_two();
						break;
					case 3:
						tm_cb.rtc_set_t = 3;
						gui_box_show_alarm_two();
						break;
				}
				clock_alarm_set_value();
			}else{
				tm_cb.index = (tm_cb.index == 1) ? 2 : 1;
			}
			msg = NO_KEY;
			break;
		case KL_CLOCK:
			if(tm_cb.setting == 0){
				switch(tm_cb.rtc_set_t){
					case 0:
						tm_cb.setting = 1;
						tm_cb.index = 1;
						rtc_time_to_tm(irtc_time_read(RTCCNT_CMD), &rtc_temp);
						break;
					case 1:
						tm_cb.setting = 1;
						tm_cb.index = 1;
						rtc_temp = rtcalm_tm1;
						break;
					case 2:
						tm_cb.setting = 1;
						tm_cb.index = 1;
						rtc_temp = rtcalm_tm2;
						break;
				}
				box_cb.times = 0;
			}else{
				switch(tm_cb.rtc_set_t){
					case 0:
						rtc_tm = rtc_temp;
						irtc_time_write(RTCCNT_CMD, rtc_tm_to_time(&rtc_tm));
						break;
					case 1:
						rtcalm_tm1 = rtc_temp;
						param_sys_arlam1_write();
						break;
					case 2:
						rtcalm_tm2 = rtc_temp;
						param_sys_arlam2_write();
						break;
				}
				tm_cb.setting = 0;
				clock_alarm_set_value();
			}
			msg = NO_KEY;
			break;
		case KU_MODE:
			if(tm_cb.setting){
				tm_cb.setting = 0;
				msg = NO_KEY;
			}
			break;
			
		case KL_NEXT_VOL_UP:
		case KH_NEXT_VOL_UP:
		case KU_NEXT_VOL_UP:
			if(tm_cb.setting){
				if (tm_cb.index == 1) {
					rtc_temp.tm_min++;
					if (rtc_temp.tm_min > 59) {
						rtc_temp.tm_min = 0;
					}
				} else if (tm_cb.index == 2) {
					rtc_temp.tm_hour++;
					if (rtc_temp.tm_hour > 23) {
						rtc_temp.tm_hour = 0;
					}
				}
				tm_cb.rtc_show_delay = 2;
				msg = 0;
			}
			break;
		
		case KL_PREV_VOL_DOWN:
		case KH_PREV_VOL_DOWN:
		case KU_PREV_VOL_DOWN:
			if(tm_cb.setting){
				if (tm_cb.index == 1) {
					if (rtc_temp.tm_min > 0) {
						rtc_temp.tm_min--;
					} else {
						rtc_temp.tm_min = 59;
					}
				} else if (tm_cb.index == 2) {
					if (rtc_temp.tm_hour > 0) {
						rtc_temp.tm_hour--;
					} else {
						rtc_temp.tm_hour = 23;
					}
				}
				tm_cb.rtc_show_delay = 2;
				msg = 0;
			}
			break;
			
	}

	return msg;
}

#endif
