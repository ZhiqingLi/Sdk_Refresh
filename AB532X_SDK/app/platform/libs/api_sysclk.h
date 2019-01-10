#ifndef _API_SYSCLK_H_
#define _API_SYSCLK_H_

enum {
    SYS_2M,
    SYS_26M,
    SYS_48M,
    SYS_52M,
    SYS_80M,
    SYS_120M,
};

u8 get_cur_sysclk(void);
void set_sys_clk(u32 clk_sel);
u8 pll1_disable(void);
void pll1_enable(void);
void lopll_2_pll1out_init(void);
#endif
