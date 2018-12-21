#include "include.h"

#if (IRRX_SW_EN || IRRX_HW_EN)
typedef struct {
    u16 cnt;                            //ir data bit counter
    u16 rpt_cnt;                        //ir repeat counter
    u16 addr;                           //address,  inverted address   Extended NEC: 16bits address
    u16 cmd;                            //command,  inverted command
} ir_cb_t;
ir_cb_t ir_cb AT(.buf.ir.cb);

AT(.com_text.ir)
u8 get_irkey(void)
{
    u8 key_val = NO_KEY;
    if (ir_cb.cnt != 32) {
        return NO_KEY;
    }

//    printf("get_irkey: %04x, %04x\n",ir_cb.addr, ir_cb.cmd);
#if IR_ADDR_FF00_EN
    if (ir_cb.addr == IR_ADDR_FF00) {
        key_val = ir_tbl_FF00[ir_cb.cmd & 0xff];
    }
#endif // IR_ADDR_FF00_EN

#if IR_ADDR_BF00_EN
    if (ir_cb.addr == IR_ADDR_BF00) {
        key_val = ir_tbl_BF00[ir_cb.cmd & 0xff];
    }
#endif // IR_ADDR_BF00_EN

#if IR_ADDR_FD02_EN
    if (ir_cb.addr == IR_ADDR_FD02) {
        key_val = ir_tbl_FD02[ir_cb.cmd & 0xff];
    }
#endif // IR_ADDR_FD02_EN

#if IR_ADDR_FE01_EN
    if (ir_cb.addr == IR_ADDR_FE01) {
        key_val = ir_tbl_FE01[ir_cb.cmd & 0xff];
    }
#endif // IR_ADDR_FE01_EN

#if IR_ADDR_7F80_EN
    if (ir_cb.addr == IR_ADDR_7F80) {
        key_val = ir_tbl_7F80[ir_cb.cmd & 0xff];
    }
#endif
    return key_val;
}
#endif //(IRRX_SW_EN || IRRX_HW_EN)


//IR receiver
#if IRRX_HW_EN
AT(.com_text.isr.irrx)
void irrx_isr(void)
{
    //IR RX data finish interrupt
    if (IRRXCON & BIT(16)) {
        IRRXCPND = BIT(16);
        ir_cb.addr = (u16)IRRXDAT;
        ir_cb.cmd = (u16)(IRRXDAT >> 16);
        ir_cb.cnt = 32;
    }

    //IR key release interrupt
    if (IRRXCON & BIT(17)) {
        IRRXCPND = BIT(17);
        ir_cb.cnt = 0;
    }
}

AT(.text.bsp.ir)
void ir_key_clr(void)
{
    ir_cb.cnt = 0;
}

AT(.text.bsp.ir)
void irrx_hw_init(void)
{
    IR_CAPTURE_PORT();
    FUNCMCON2 = IRRX_MAPPING;                          //IR mapping to G6
    memset(&ir_cb, 0, sizeof(ir_cb));

    IRRXERR0 = (RPTERR_CNT << 16) | DATERR_CNT;     //RPTERR[27:16], DATERR[11:0]
    IRRXERR1 = (TOPR_CNT << 20) | (ONEERR_CNT << 10) | ZEROERR_CNT;    //TOPR[31:20], ONEERR[19:10], ZEROERR[9:0]

#if IR32KSEL_EN
    CLKCON1 &= ~BIT(5);
    CLKCON1 |= BIT(4);                              //enable 26M分频32K
    IRRXCON |= BIT(3);                              //ir clock source select 32K
#endif // IR32KSEL_EN
    irrx_irq_init();
    IRRXCON = 0x03;

//    printf("IRRXCON:%08x\n", IRRXCON);
}
#endif // IRRX_HW_EN


///软件timer capture irrx decode
#if IRRX_SW_EN
#define TMR3_RCLK               1000            //xosc26m_div 1M
//timer3 capture
AT(.com_text.isr.timer1)
void irrx_isr(void)
{
    u32 tmrcnt;

    if (TMR3CON & BIT(17)) {
        //timer1 capture interrupt
        TMR3CNT  = TMR3CNT - TMR3CPT;
        tmrcnt = TMR3CPT;
        TMR3CPND = BIT(17);
        tmrcnt /= TMR3_RCLK;                //convert to ms
    } else if (TMR3CON & BIT(16)){
        //timer1 overflow interrupt
        TMR3CPND = BIT(16);
        tmrcnt = 110;                   //110ms overflow
    } else {
        return;
    }

    //processing repeat code
    if (ir_cb.cnt == 32) {
        if ((tmrcnt >= 10) && (tmrcnt <= 12)) {
            //repeat code is simply 9ms+2.25ms
            ir_cb.rpt_cnt = 0;
        } else {
            ir_cb.rpt_cnt += tmrcnt;
            if (ir_cb.rpt_cnt > 108) {
                ir_cb.rpt_cnt = 0;
                ir_cb.cnt = 0;          //ir key release
            }
        }
        return;
    } else if ((tmrcnt > 7) || (tmrcnt == 0)) {     //A message is started by a 9ms AGC burst
        ir_cb.rpt_cnt = 0;
        ir_cb.cnt = 0;                  //ir key message started
        return;
    }

    ir_cb.cmd >>= 1;
    ir_cb.cnt++;
    if (tmrcnt == 2) {                  //Bit time of 1.125ms(0) or 2.25ms(1)
        ir_cb.cmd |= 0x8000;
    }

    if (ir_cb.cnt == 16) {
        ir_cb.addr = ir_cb.cmd;         //save address data
    } else if (ir_cb.cnt == 32) {
        //got ir key message
        if ((u8)ir_cb.cmd > 96) {
            ir_cb.cmd = NO_KEY;
        }
//        printf("ir: %04x, %02x\n",ir_cb.addr, (u8)ir_cb.cmd);
    }
}

AT(.text.bsp.ir)
void timer3_init(void)
{
    irrx_irq_init();
	TMR3CNT  = 0;
	TMR3PR   = TMR3_RCLK*110 - 1;                                //110ms Timer overflow interrupt
	TMR3CON  = BIT(8) | BIT(7) | BIT(5) | BIT(2) | BIT(1) | BIT(0);   //capture & overflow interrupt enable, falling edge, Capture Mode
}

AT(.text.bsp.ir)
void irrx_sw_init(void)
{
    IR_CAPTURE_PORT();
    FUNCMCON2 = TMR3CAP_MAPPING;                            //tmr3 g6  PE5
    memset(&ir_cb, 0, sizeof(ir_cb));
    timer3_init();
}
#endif // IRRX_SW_EN





