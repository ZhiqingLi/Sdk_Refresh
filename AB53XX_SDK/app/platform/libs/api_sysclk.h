#ifndef _API_SYSCLK_H_
#define _API_SYSCLK_H_

enum {
    SYS_2M,
    SYS_12M,
    SYS_13M,
    SYS_24M,
    SYS_26M,
    SYS_30M,
    SYS_48M,
    SYS_60M,
    SYS_80M,
    SYS_120M,
};

u8 get_cur_sysclk(void);
void set_sys_clk(u32 clk_sel);
#endif
