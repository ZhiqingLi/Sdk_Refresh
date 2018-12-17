#ifndef _API_SYS_H_
#define _API_SYS_H_

extern u16 tmr5ms_cnt;

void sys_set_tmr_enable(bool tmr5ms_en, bool tmr1ms_en);
u32 sys_get_rand_key(void);

void xosc_init(void);

u16 get_random(u16 num);                                    //获取[0, num-1]之间的随机数
void sys_clk_output(u32 type, u32 mapping);                 //输出时钟   type: 0->xosc26m, 1->x26m_13m, 2->12m, 3->24m

#endif // _API_SYS_H_

