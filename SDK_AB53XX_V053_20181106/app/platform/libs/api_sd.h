#ifndef _API_SD_H
#define _API_SD_H

void sd_disk_init(void);
void sd_insert(void);
void sd_remove(void);
bool sd_is_online(void);
bool sd0_read(void *buf, u32 lba);
bool sd0_write(void* buf, u32 lba);
bool sd0_stop(bool type);            //复用SDCLK检测时type: 0->sdclk设置为输出， 1->sdclk设置为输入上拉，用于检测
bool sd0_init(void);
bool sd_soft_detect(void);
bool is_sd_abend(void);
void sd0_go_idle_state(void);
void sd_other_param_set(u32 param);  //dev_init 后调用, 补充设置一些SD参数.
#endif // _API_SD_H
