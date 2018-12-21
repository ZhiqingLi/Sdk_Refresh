#ifndef _API_RTC_H
#define _API_RTC_H

void irtc_sfr_write(u32 cmd, u8 dat);
u8 irtc_sfr_read(u32 cmd);

u32 irtc_time_read(u32 cmd);
void irtc_time_write(u32 cmd, u32 dat);

void rtcram_write(u8 *buf, u8 addr, u8 cnt);
void rtcram_read(u8 *buf, u8 addr, u8 cnt);

void rtc_time_to_tm(unsigned long time, void *param);
unsigned long rtc_tm_to_time(void *param);
u8 get_weekday(u8 year, u8 month, u8 day);

#endif // _API_RTC_H
