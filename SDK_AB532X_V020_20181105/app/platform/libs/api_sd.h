#ifndef _API_SD_H
#define _API_SD_H

#define sd_insert()                 sd0_insert(0)
#define sd_remove()                 sd0_remove(0)
#define sd1_insert()                sd0_insert(1)
#define sd1_remove()                sd0_remove(1)

void sd_disk_init(void);
void sd0_insert(u8 index);
void sd0_remove(u8 index);

bool sd0_read(void *buf, u32 lba);
bool sd0_write(void* buf, u32 lba);
bool sd0_stop(bool type);            //复用SDCLK检测时type: 0->sdclk设置为输出， 1->sdclk设置为输入上拉，用于检测
bool sd0_init(void);
bool sd_soft_detect(void);
void sd0_go_idle_state(void);
void ude_sd_remove(void);
#endif // _API_SD_H
