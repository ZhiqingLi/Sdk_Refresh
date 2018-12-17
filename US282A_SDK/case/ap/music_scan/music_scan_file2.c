
#include "music_scan_file.h"
#include "case_independent.h"
#include "common_func.h"



uint32 g_vm_file_newest_time = 0;

uint32 g_file_newest_time = 0;

bool g_vfs_changed = FALSE;//文件系统是否改动



//时间从2000-1-1 00:00:00 ~ 2128-12-31  23:59:58
uint32 rtc_to_uint32(file_time_t * rtc)
{
    uint32 t = (rtc->year > 2000)?(rtc->year - 2000):0;
    t = (t << 25) + (rtc->month << 21) + (rtc->day << 16);
    t += (rtc->hour << 11) + (rtc->minute << 5) + (rtc->second/2);
    return t;
}

/*
void uint32_to_rtc(file_time_t * rtc, uint32 t)
{
   rtc->year = 2000 + (t >> 25);
   rtc->month = (t >> 21)&0x0F;
   rtc->day = (t >> 16)&0x1F;
   rtc->hour = (t >> 11)&0x0F;
   rtc->minute = (t >> 5)&0x3F;
   rtc->second = (t & 0x1F)*2;
}

*/


void get_newest_file_time(int32 fs_id)
{
    file_time_t ft_tmp;
    uint32 tmp;
    if(vfs_get_time(fs_id, &ft_tmp, NULL, FS_TIME_CREATE) != 0)
    {
        tmp = rtc_to_uint32(&ft_tmp);
        if(tmp > g_file_newest_time)
        {
            g_file_newest_time = tmp;
            //PRINT_INFO_INT("new file:", tmp);
        }
    }
}




