#ifndef _BSP_IR_H
#define _BSP_IR_H

///硬件IR receiver参数
#define IR32KSEL_EN         0                     //IR clock source select 32K
#if IR32KSEL_EN
    #define RPTERR_CNT      33                    //配置11.25ms ± (RPTERR_CNT + 1)*32K的repeat code允许范围
    #define DATERR_CNT      33                    //配置13.5ms ± (DATERR_CNT + 1)*32K引导码允许范围
    #define ONEERR_CNT      7                     //配置2.25ms ± (ONEERR_CNT + 1)*32K的logic "1"允许范围
    #define ZEROERR_CNT     3                     //配置1.12ms ± (ONEERR_CNT + 1)*32K数logic "0"允许范围
    #define TOPR_CNT        55                    //IR time out length = (TOPR + 1) * 64 *32K
#else
    #define RPTERR_CNT      1000                  //配置11.25ms ± (RPTERR_CNT + 1)us的repeat code允许范围
    #define DATERR_CNT      1000                  //配置13.5ms ± (DATERR_CNT + 1)us引导码允许范围
    #define ONEERR_CNT      250                   //配置2.25ms ± (ONEERR_CNT + 1)us的logic "1"允许范围
    #define ZEROERR_CNT     125                   //配置1.12ms ± (ONEERR_CNT + 1)us数logic "0"允许范围
    #define TOPR_CNT        1718                  //IR time out length = (TOPR + 1) * 64 us
#endif // IR32KSEL_EN


///红外遥控器地址码
#define IR_ADDR_FF00      0xFF00
#define IR_ADDR_BF00      0xBF00
#define IR_ADDR_FD02      0xFD02
#define IR_ADDR_FE01      0xFE01
#define IR_ADDR_7F80      0x7F80

extern const u8 ir_tbl_FF00[96];
extern const u8 ir_tbl_BF00[32];
extern const u8 ir_tbl_FD02[32];
extern const u8 ir_tbl_FE01[32];
extern const u8 ir_tbl_7F80[32];

void irrx_hw_init(void);
void irrx_irq_init(void);
void irrx_sw_init(void);
void timer1_irq_init(void);
void ir_key_clr(void);
u8 get_irkey(void);

#endif // _BSP_IR_H
