#ifndef _API_SYS_H_
#define _API_SYS_H_

#define IRQ_TMR1_VECTOR                 4
#define IRQ_TMR245_VECTOR               5
#define IRQ_IRRX_VECTOR                 6
#define IRQ_UART_VECTOR                 14

typedef void (*isr_t)(void);

extern u16 tmr5ms_cnt;

void sys_set_tmr_enable(bool tmr5ms_en, bool tmr1ms_en);
u32 sys_get_rand_key(void);

void xosc_init(void);

bool sys_irq_init(int vector, int pr, isr_t isr);           //初始化一个中断, pr为优先级


//GPIO输出时钟   type: 0 -> xosc26m, 1 -> x26m_13m, 2 -> x26m_32k, 3 -> osc32k, 4 -> 12m, 5 -> 24m
//mapping: {CLKOMAP_PA5, CLKOMAP_PA6, CLKOMAP_PB0, CLKOMAP_PB1, CLKOMAP_PE5, CLKOMAP_PE6}
//输出osc32k时钟需要外挂32K晶振且使能EXT_32K_EN
void sys_clk_output(u32 type, u32 mapping);

u16 get_random(u16 num);                                    //获取[0, num-1]之间的随机数

void set_uart0_baud(u32 baud);                              //设置uart0波特率, 默认是15000000

u16 convert_uni2gbk(u16 code, u32 addr, u32 len);           //输入utf16, 返回gbk编码, 找不到编码，使用'□'
u8 utf8_char_size(u8 code);                                 //返回UTF8占用几字节
u16 utf8_convert_to_unicode(u8 *in, u8 char_size);          //UTF-8转成UTF-16 LE编码

#endif // _API_SYS_H_

