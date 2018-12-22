#include "include.h"
#include "func.h"
#include "func_fmam_freq.h"

#if FUNC_FMAM_FREQ_EN

#define FMAM32KSEL_EN             0             //FMAM模块选择32K时钟
u32 fmam_freq;

//返回读取频率， 单位KHz
AT(.text.func.fmam_freq)
u32 get_fmam_freq_do(void)
{
    u32 fdt_cnt;

//    printf("%s\n", __func__);

    //先用4ms检测AM信号
    FMAMFDTCON |= BIT(7);                       //Frequency detect timer select 4ms
    FMAMFDTCON |= BIT(10);                      //Frequency detect reset
    FMAMFDTCON |= BIT(9);                       //Frequency detect kick start
    while (!(FMAMFDTCON & BIT(31))) {           //FMAM FDT finish pending
        WDT_CLR();
    }
    FMAMFDTCPND = BIT(31);                      //Clear FMAM detect finish pending
    fdt_cnt = FMAMFDTCNT;

    if (fdt_cnt >= 131000) {
        FMAMFDTCON |=  BIT(25);                 //low pass filter selection, fm mode
        FMAMFDTCON &= ~BIT(7);                  //切换成1ms检测FM信号
        FMAMFDTCON |= BIT(10);                  //Frequency detect reset
        FMAMFDTCON |= BIT(9);                   //Frequency detect kick start
        while (!(FMAMFDTCON & BIT(31))) {       //FMAM FDT finish pending
            WDT_CLR();
        }
        FMAMFDTCPND = BIT(31);                  //Clear FMAM detect finish pending
        fdt_cnt = FMAMFDTCNT;
        fdt_cnt = (fdt_cnt + 5) / 10;
        fdt_cnt *= 10;
        return fdt_cnt;
    } else {
        FMAMFDTCON &= ~BIT(25);                 //low pass filter selection, am mode
    }
    return ((fdt_cnt + 2) >> 2);                //转成KHZ
}

AT(.text.func.fmam_freq)
u32 get_fmam_freq(void)
{
    int i;
    u32 cfreq = 0, valid_cnt;
    static int stable_cnt = 0;
    static u32 bfreq = 0;

    FMAMFDTCON |= BIT(24) | BIT(25);

    for (i = 0; i < 30; i++) {
        cfreq = get_fmam_freq_do();
        valid_cnt = 5;
        if (cfreq == bfreq) {
            stable_cnt++;
            if (bfreq < 13100) {
                valid_cnt = valid_cnt * 2;
            }
            if (stable_cnt >= valid_cnt) {
                return cfreq;
            }
        } else {
            bfreq = cfreq;
            stable_cnt = 0;
        }
        delay_5ms(2);
    }
    return 0;
}

AT(.text.func.fmam_freq)
static void func_fmam_enter(void)
{
    GPIOBDE |= BIT(0);
    GPIOBDIR |= BIT(0);                         //PB0: FMAM input io channel 0
//    GPIOBDE |= BIT(1);
//    GPIOBDIR |= BIT(1);                         //PB1: FMAM input io channel 1

    FMAMFDTCON  = BIT(25) | BIT(24) | BIT(22) | BIT(20); //default value
    FMAMFDTCON |= BIT(10);                      //Frequency detect reset
//    FMAMFDTCON |= BIT(16);                      //AMP1 enable
    FMAMFDTCON |= BIT(17);                      //AMP2 enable
    FMAMFDTCON |= BIT(18);                      //Channel 0 enable
//    FMAMFDTCON |= BIT(19);                      //Channel 1 enable
    FMAMFDTCPND = BIT(31);                      //Clear FMAM detect finish pending

#if FMAM32KSEL_EN
    CLKCON1 &= ~BIT(7);
    CLKCON1 |= BIT(6);                          //26M分频32K
    FMAMFDTCON |= BIT(8);                       //FMAM clock source select 32K
#endif // FMAM32KSEL_EN

    fmam_freq = 0;
}


AT(.text.func.fmam_freq)
static void func_fmam_process(void)
{
    u32 ret;
    func_process();
    ret = get_fmam_freq();
    if (ret > 0) {
        if (ret != fmam_freq) {
            fmam_freq = ret;
//            printf("FMAM Freq: %dKHz\n", fmam_freq);
        }
    }
}

AT(.text.func.fmam_freq)
static void func_fmam_exit(void)
{
    FMAMFDTCON = BIT(10);                       //Frequency detect reset
    func_cb.last = FUNC_FMAM_FREQ;
}

AT(.text.func.fmam_freq)
void func_fmam_freq(void)
{
    printf("%s\n", __func__);

    func_fmam_enter();

    while (func_cb.sta == FUNC_FMAM_FREQ) {
        func_fmam_process();
        func_fmam_message(msg_dequeue());
        func_fmam_display();
    }
    func_fmam_exit();
}
#endif // FUNC_FMAM_FREQ_EN
