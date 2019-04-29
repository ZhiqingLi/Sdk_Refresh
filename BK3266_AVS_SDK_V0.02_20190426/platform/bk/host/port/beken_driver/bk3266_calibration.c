/**
 **************************************************************************************
 * @file    bk3266_calibration.c
 * @brief   Calibrations for BK3266, such as audio dac DC offset, charger, sar-adc, tx, etc
 * 
 * @author  Aixing.Li
 * @version V1.0.0
 *
 * &copy; 2017 BEKEN Corporation Ltd. All rights reserved.
 **************************************************************************************
 */

#include <string.h>
#include "types.h"
#include "bautil.h"
#include "sys_config.h"
#include "sys_irq.h"
#include "bk3000_reg.h"

#define CALIB_DBG_ENABLE                (1)
#if     CALIB_DBG_ENABLE
#define CALIB_DBG(fmt, ...)             //if(calib_dbg_en) os_printf("[CALIB]: "fmt, ##__VA_ARGS__)
int                                     os_printf(const char *fmt, ...);
#else
#define CALIB_DBG(fmt, ...)
#endif

#define CFG_DAC_DC_OFFSET_CALIB_IO_L    BK3000_GPIO_18_CONFIG
#define CFG_DAC_DC_OFFSET_CALIB_IO_R    BK3000_GPIO_19_CONFIG

#define TX_CALIB_ALGORITHM_ERGODIC      (0) //遍历法
#define TX_CALIB_ALGORITHM_DICHOTOMY    (1) //二分法
#define CFG_TX_CALIB_ALGORITHM          (TX_CALIB_ALGORITHM_ERGODIC)

#define SAR_ADC_READ_AVG_COUNT          (2)
#define SAR_ADC_WAIT_BUSY_COUNT         (1000)

#define ADDR_GPIO_BASE                  (0x00F40000)
#define ADDR_GPIO_0x30                  (ADDR_GPIO_BASE + 0x30 * 4)
#define ADDR_GPIO_0x31                  (ADDR_GPIO_BASE + 0x31 * 4)
#define ADDR_GPIO_0x32                  (ADDR_GPIO_BASE + 0x32 * 4)
#define ADDR_GPIO_0x33                  (ADDR_GPIO_BASE + 0x33 * 4)
#define ADDR_GPIO_0x34                  (ADDR_GPIO_BASE + 0x34 * 4)
#define ADDR_GPIO_0x35                  (ADDR_GPIO_BASE + 0x35 * 4)
#define ADDR_GPIO_0x36                  (ADDR_GPIO_BASE + 0x36 * 4)
#define ADDR_GPIO_0x37                  (ADDR_GPIO_BASE + 0x37 * 4)
#define ADDR_GPIO_0x3C                  (ADDR_GPIO_BASE + 0x3C * 4)
#define ADDR_ANA_REG0                   (ADDR_GPIO_0x30)
#define ADDR_ANA_REG1                   (ADDR_GPIO_0x31)
#define ADDR_ANA_REG2                   (ADDR_GPIO_0x32)
#define ADDR_ANA_REG3                   (ADDR_GPIO_0x33)
#define ADDR_ANA_REG4                   (ADDR_GPIO_0x34)
#define ADDR_ANA_REG5                   (ADDR_GPIO_0x35)
#define ADDR_ANA_REG6                   (ADDR_GPIO_0x36)
#define ADDR_ANA_REG7                   (ADDR_GPIO_0x37)
#define ANA_REG0                        (*(volatile uint32_t*)ADDR_ANA_REG0)
#define ANA_REG1                        (*(volatile uint32_t*)ADDR_ANA_REG1)
#define ANA_REG2                        (*(volatile uint32_t*)ADDR_ANA_REG2)
#define ANA_REG3                        (*(volatile uint32_t*)ADDR_ANA_REG3)
#define ANA_REG4                        (*(volatile uint32_t*)ADDR_ANA_REG4)
#define ANA_REG5                        (*(volatile uint32_t*)ADDR_ANA_REG5)
#define ANA_REG6                        (*(volatile uint32_t*)ADDR_ANA_REG6)
#define ANA_REG7                        (*(volatile uint32_t*)ADDR_ANA_REG7)

#define ADDR_XVR_BASE                   (0x00F00000)
#define ADDR_XVR_0x00                   (ADDR_XVR_BASE + 0x00 * 4)
#define ADDR_XVR_0x01                   (ADDR_XVR_BASE + 0x01 * 4)
#define ADDR_XVR_0x02                   (ADDR_XVR_BASE + 0x02 * 4)
#define ADDR_XVR_0x03                   (ADDR_XVR_BASE + 0x03 * 4)
#define ADDR_XVR_0x04                   (ADDR_XVR_BASE + 0x04 * 4)
#define ADDR_XVR_0x05                   (ADDR_XVR_BASE + 0x05 * 4)
#define ADDR_XVR_0x06                   (ADDR_XVR_BASE + 0x06 * 4)
#define ADDR_XVR_0x07                   (ADDR_XVR_BASE + 0x07 * 4)
#define ADDR_XVR_0x08                   (ADDR_XVR_BASE + 0x08 * 4)
#define ADDR_XVR_0x09                   (ADDR_XVR_BASE + 0x09 * 4)
#define ADDR_XVR_0x0A                   (ADDR_XVR_BASE + 0x0A * 4)
#define ADDR_XVR_0x0B                   (ADDR_XVR_BASE + 0x0B * 4)
#define ADDR_XVR_0x0C                   (ADDR_XVR_BASE + 0x0C * 4)
#define ADDR_XVR_0x0D                   (ADDR_XVR_BASE + 0x0D * 4)
#define ADDR_XVR_0x0E                   (ADDR_XVR_BASE + 0x0E * 4)
#define ADDR_XVR_0x0F                   (ADDR_XVR_BASE + 0x0F * 4)
#define ADDR_XVR_0x10                   (ADDR_XVR_BASE + 0x10 * 4)
#define ADDR_XVR_0x11                   (ADDR_XVR_BASE + 0x11 * 4)
#define ADDR_XVR_0x12                   (ADDR_XVR_BASE + 0x12 * 4)
#define ADDR_XVR_0x13                   (ADDR_XVR_BASE + 0x13 * 4)
#define ADDR_XVR_0x14                   (ADDR_XVR_BASE + 0x14 * 4)
#define ADDR_XVR_0x15                   (ADDR_XVR_BASE + 0x15 * 4)
#define ADDR_XVR_0x16                   (ADDR_XVR_BASE + 0x16 * 4)
#define ADDR_XVR_0x17                   (ADDR_XVR_BASE + 0x17 * 4)
#define ADDR_XVR_0x18                   (ADDR_XVR_BASE + 0x18 * 4)
#define ADDR_XVR_0x19                   (ADDR_XVR_BASE + 0x19 * 4)
#define ADDR_XVR_0x1A                   (ADDR_XVR_BASE + 0x1A * 4)
#define ADDR_XVR_0x1B                   (ADDR_XVR_BASE + 0x1B * 4)
#define ADDR_XVR_0x1C                   (ADDR_XVR_BASE + 0x1C * 4)
#define ADDR_XVR_0x1D                   (ADDR_XVR_BASE + 0x1D * 4)
#define ADDR_XVR_0x1E                   (ADDR_XVR_BASE + 0x1E * 4)
#define ADDR_XVR_0x1F                   (ADDR_XVR_BASE + 0x1F * 4)
#define ADDR_XVR_0x20                   (ADDR_XVR_BASE + 0x20 * 4)
#define ADDR_XVR_0x21                   (ADDR_XVR_BASE + 0x21 * 4)
#define ADDR_XVR_0x22                   (ADDR_XVR_BASE + 0x22 * 4)
#define ADDR_XVR_0x23                   (ADDR_XVR_BASE + 0x23 * 4)
#define ADDR_XVR_0x24                   (ADDR_XVR_BASE + 0x24 * 4)
#define ADDR_XVR_0x25                   (ADDR_XVR_BASE + 0x25 * 4)
#define ADDR_XVR_0x26                   (ADDR_XVR_BASE + 0x26 * 4)
#define ADDR_XVR_0x27                   (ADDR_XVR_BASE + 0x27 * 4)
#define ADDR_XVR_0x28                   (ADDR_XVR_BASE + 0x28 * 4)
#define ADDR_XVR_0x29                   (ADDR_XVR_BASE + 0x29 * 4)
#define ADDR_XVR_0x2A                   (ADDR_XVR_BASE + 0x2A * 4)
#define ADDR_XVR_0x2B                   (ADDR_XVR_BASE + 0x2B * 4)
#define ADDR_XVR_0x2C                   (ADDR_XVR_BASE + 0x2C * 4)
#define ADDR_XVR_0x2D                   (ADDR_XVR_BASE + 0x2D * 4)
#define ADDR_XVR_0x2E                   (ADDR_XVR_BASE + 0x2E * 4)
#define ADDR_XVR_0x2F                   (ADDR_XVR_BASE + 0x2F * 4)
#define ADDR_XVR_0x30                   (ADDR_XVR_BASE + 0x30 * 4)
#define ADDR_XVR_0x31                   (ADDR_XVR_BASE + 0x31 * 4)
#define ADDR_XVR_0x32                   (ADDR_XVR_BASE + 0x32 * 4)
#define ADDR_XVR_0x33                   (ADDR_XVR_BASE + 0x33 * 4)
#define ADDR_XVR_0x34                   (ADDR_XVR_BASE + 0x34 * 4)
#define ADDR_XVR_0x35                   (ADDR_XVR_BASE + 0x35 * 4)
#define ADDR_XVR_0x36                   (ADDR_XVR_BASE + 0x36 * 4)
#define ADDR_XVR_0x37                   (ADDR_XVR_BASE + 0x37 * 4)
#define ADDR_XVR_0x38                   (ADDR_XVR_BASE + 0x38 * 4)
#define ADDR_XVR_0x39                   (ADDR_XVR_BASE + 0x39 * 4)
#define ADDR_XVR_0x3A                   (ADDR_XVR_BASE + 0x3A * 4)
#define ADDR_XVR_0x3B                   (ADDR_XVR_BASE + 0x3B * 4)
#define ADDR_XVR_0x3C                   (ADDR_XVR_BASE + 0x3C * 4)
#define ADDR_XVR_0x3D                   (ADDR_XVR_BASE + 0x3D * 4)
#define ADDR_XVR_0x3E                   (ADDR_XVR_BASE + 0x3E * 4)
#define ADDR_XVR_0x3F                   (ADDR_XVR_BASE + 0x3F * 4)
#define ADDR_XVR_0x40                   (ADDR_XVR_BASE + 0x40 * 4)
#define ADDR_XVR_0x41                   (ADDR_XVR_BASE + 0x41 * 4)
#define ADDR_XVR_0x42                   (ADDR_XVR_BASE + 0x42 * 4)
#define ADDR_XVR_0x43                   (ADDR_XVR_BASE + 0x43 * 4)
#define ADDR_XVR_0x44                   (ADDR_XVR_BASE + 0x44 * 4)
#define ADDR_XVR_0x45                   (ADDR_XVR_BASE + 0x45 * 4)
#define ADDR_XVR_0x46                   (ADDR_XVR_BASE + 0x46 * 4)
#define ADDR_XVR_0x47                   (ADDR_XVR_BASE + 0x47 * 4)
#define ADDR_XVR_0x48                   (ADDR_XVR_BASE + 0x48 * 4)
#define ADDR_XVR_0x49                   (ADDR_XVR_BASE + 0x49 * 4)
#define ADDR_XVR_0x4A                   (ADDR_XVR_BASE + 0x4A * 4)
#define ADDR_XVR_0x4B                   (ADDR_XVR_BASE + 0x4B * 4)
#define ADDR_XVR_0x4C                   (ADDR_XVR_BASE + 0x4C * 4)
#define ADDR_XVR_0x4D                   (ADDR_XVR_BASE + 0x4D * 4)
#define ADDR_XVR_0x4E                   (ADDR_XVR_BASE + 0x4E * 4)
#define ADDR_XVR_0x4F                   (ADDR_XVR_BASE + 0x4F * 4)

//#define TX_Q_CONST_IQCAL_P              (0x50)
//#define TX_Q_CONST_IQCAL_P_4_PHASE      (57)

uint32_t TX_Q_CONST_IQCAL_P         = 0x50;
uint32_t TX_Q_CONST_IQCAL_P_4_PHASE = 57;

#define XVR_ANALOG_REG_BAK  XVR_analog_reg_save
//如果外部没有定义XVR_ANALOG_REG_BAK变量，则使用下面语句
extern uint32_t XVR_ANALOG_REG_BAK[16];
//static uint32_t XVR_ANALOG_REG_BAK[16];

uint8_t  calib_dbg_en       = 0;
uint8_t  calib_charger_vlcf = 0;
uint8_t  calib_charger_icp  = 0;
uint8_t  calib_charger_vcv  = 0;

uint32_t calib_sar_adc_dc   = 0;
uint32_t calib_sar_adc_4p2  = 0;

uint8_t  calib_tx_i_doff;        //@XER_0x0C<5:9>
uint8_t  calib_tx_q_doff;        //@XER_0x0C<0:4>
uint8_t  calib_tx_i_dc_offset;   //@XER_0x32<8:15>
uint8_t  calib_tx_q_dc_offset;   //@XER_0x32<0:7>
uint8_t  calib_tx_i_gain_comp;   //@XER_0x32<24:31>
uint8_t  calib_tx_q_gain_comp;   //@XER_0x32<16:23>
uint8_t  calib_tx_phase_comp;    //@XER_0x33<8:15>
uint8_t  calib_tx_output_power;  //@XER_0x0B<4:8>

uint32_t xvr_reg_0x09_bak;
uint32_t xvr_reg_0x0b_bak;
uint32_t xvr_reg_0x0c_bak;
uint32_t xvr_reg_0x24_bak;
uint32_t xvr_reg_0x25_bak;
uint32_t xvr_reg_0x31_bak;

__inline static int32_t abs(int32_t x)
{
    int32_t y = x - (x < 0);

    y ^= (y >> 31);

    return y;
}

__inline static void delay_cycle(uint32_t n)
{
    while(n--) __asm("b.nop 5;");
}

//__inline static void delay_us(uint32_t us)
//{
//    delay_cycle(30 * us); //120
//}

__inline static void delay_ms(uint32_t ms)
{
    delay_cycle(15000 * ms); //120000
}

static void reg_bit_set(uint32_t reg, uint32_t pos, uint32_t value)
{
    volatile uint32_t* r = (volatile uint32_t*)reg;

    if(value)
    {
        *r |= (1 << pos);
    }
    else
    {
        *r &= ~(1 << pos);
    }
}

__inline static uint32_t reg_bit_get(uint32_t reg, uint32_t pos)
{
    return ((*(volatile uint32_t*)reg) >> pos) & 1;
}

__inline static void reg_bits_set(uint32_t reg, uint32_t pos1, uint32_t pos2, uint32_t value)
{
    volatile uint32_t* r = (volatile uint32_t*)reg;

    uint32_t mask = (1 << (pos2 - pos1 + 1)) - 1;

    *r &= ~(mask << pos1);
    *r |=  (value & mask) << pos1;
}

__inline static uint32_t reg_bits_get(uint32_t reg, uint32_t pos1, uint32_t pos2)
{
    uint32_t mask = (1 << (pos2 - pos1 + 1)) - 1;

    return ((*(volatile uint32_t*)reg) >> pos1) & mask;
}

__inline static void xvr_reg_set(uint32_t reg, uint32_t value)
{
    *(volatile uint32_t*)reg = XVR_ANALOG_REG_BAK[(reg - ADDR_XVR_BASE) / 4] = value;
}

__inline static uint32_t xvr_reg_get(uint32_t reg)
{
    return XVR_ANALOG_REG_BAK[(reg - ADDR_XVR_BASE) / 4];
}

__inline static void xvr_reg_bit_set(uint32_t reg, uint32_t pos, uint32_t value)
{
    uint32_t* r = (uint32_t*)&XVR_ANALOG_REG_BAK[(reg - ADDR_XVR_BASE) / 4];

    if(value)
    {
        *r |= (1 << pos);
    }
    else
    {
        *r &= ~(1 << pos);
    }

     *(volatile uint32_t*)reg = *r;
}

__inline static void xvr_reg_bits_set(uint32_t reg, uint32_t pos1, uint32_t pos2, uint32_t value)
{
    uint32_t* r = (uint32_t*)&XVR_ANALOG_REG_BAK[(reg - ADDR_XVR_BASE) / 4];

    uint32_t mask = (1 << (pos2 - pos1 + 1)) - 1;

    *r &= ~(mask << pos1);
    *r |=  (value & mask) << pos1;

    *(volatile uint32_t*)reg = *r;
}

__inline static uint32_t xvr_reg_bit_get(uint32_t reg, uint32_t pos)
{
    return (XVR_ANALOG_REG_BAK[(reg - ADDR_XVR_BASE) / 4] >> pos) & 1;
}

__inline static uint32_t xvr_reg_bits_get(uint32_t reg, uint32_t pos1, uint32_t pos2)
{
    uint32_t mask = (1 << (pos2 - pos1 + 1)) - 1;

    return (XVR_ANALOG_REG_BAK[(reg - ADDR_XVR_BASE) / 4] >> pos1) & mask;
}

void calib_dbg_enable(uint32_t enable)
{
    calib_dbg_en = !!enable;
}

static void audio_dac_init(uint32_t type)
{
    //1. 打开audio DAC的数字电路部分
    BK3000_PMU_PERI_PWDS  &= ~bit_PMU_AUD_PWD;
    BK3000_AUD_AUDIO_CONF  = (0x1 << sft_AUD_SAMP_ADC_RATE) | (0x1 << sft_AUD_SAMP_DAC_RATE);
    BK3000_AUD_FIFO_CONF  &= ~( 0x3FF << sft_AUD_DAC_DACR_RD_THRE );
    BK3000_AUD_FIFO_CONF  |= ((8 << sft_AUD_DAC_DACR_RD_THRE ) | (8 << sft_AUD_DAC_DACL_RD_THRE));
    BK3000_AUD_DTMF_CONF0  = 0x2;
    BK3000_AUD_DTMF_CONF1  = 0x102023;
    BK3000_AUD_DAC_CONF0  &= ~0x00FF0000;
    BK3000_AUD_DAC_CONF0  |= ((1 << 16) | (1 << 17) | (1 << 18));
    BK3000_AUD_AUDIO_CONF |= (1 << sft_AUD_DAC_ENABLE) | (1 << sft_AUD_DTMF_ENABLE);

    while(!(BK3000_AUD_FIFO_STATUS & (bit_DACL_FIFO_EMPTY | bit_DACR_FIFO_EMPTY)));
    while(!(BK3000_AUD_FIFO_STATUS & (bit_DACL_FIFO_FULL  | bit_DACR_FIFO_FULL))) BK3000_AUD_DAC_PORT = 0;

    //2. 打开PGA
    reg_bit_set(ADDR_GPIO_0x36, 27, 1);     //dacckinv_sel=0x1, 表示时钟反向
    reg_bit_set(ADDR_GPIO_0x36, 12, 1);     //enb_audref_autocal=0, 表示参考电压配置用自动校准的值
    reg_bits_set(ADDR_GPIO_0x36, 7, 11, 16);
    reg_bits_set(ADDR_GPIO_0x36, 2, 6, 0);  //audiodac_G<4:0>到相应的增益，比如0X1F，数值越大表示增益越大
    reg_bit_set(ADDR_GPIO_0x36, 1, 1);      //dac_dwasd=0x0, 表示打开dwa算法，打开后可以降低THD，（需要确保校准时也是dwasd也是0）
    reg_bit_set(ADDR_GPIO_0x36, 0, 0);      //dac_mute=0x0, 表示将数字输入输入到audio dac中

    //3. 然后打开校准DAC
    //如果是单端模式，则需要将reg35<8:7> enpaonl和enpaonr中的一个关闭，(需要和校准单端模式时关闭的选择相同), 0是关闭，1是打开。
    //如果是音箱的单端模式，只有audio输出的正端被输出给片外PA的，则需要将这两个enpaonl和enpaonr都置成0
    //如果是差分模式，则保持enpaonl和enpaonr都为1
    //(type & 0x1) == 0 ? Differential : Single-End
    reg_bit_set(ADDR_GPIO_0x35,  7, (type & 0x1) == 0);
    reg_bit_set(ADDR_GPIO_0x35,  8, 1);

    //将校准时对应模式和对应增益(模拟增益)的校准值分别填入reg35<31:24>dcocdr<7:0> 和reg35<23:16>dcocdl<7:0>, dcocdr表示右声道校准值，dcocdl表示左声道的校准值。
    //TODO

    reg_bits_set(ADDR_GPIO_0x35, 14, 15, 0x3);          //dcoc trimming
    reg_bit_set(ADDR_GPIO_0x35, 13, 1);                 //dcoc dac output control
    reg_bit_set(ADDR_GPIO_0x35, 12, 0);                 //power ON dcoc dac
    reg_bits_set(ADDR_GPIO_0x35, 10, 11, 0x0);
    reg_bit_set(ADDR_GPIO_0x35, 6, 0);                  //enbchswvdd

    reg_bit_set(ADDR_GPIO_0x37, 30, 0);                 //auddacl=1;
    reg_bit_set(ADDR_GPIO_0x37, 29, 0);                 //auddacr=1;

    //设置gpio_reg37<31> audiodac_diffen, 1表示differential, 0表示single end。根据需求设置该寄存器（确保和校准时的配置一样）
    reg_bit_set(ADDR_GPIO_0x37, 31, (type & 0x1) == 0);
    reg_bit_set(ADDR_GPIO_0x37, 23, 0);
	reg_bit_set(ADDR_GPIO_0x37, 24, 0);
    reg_bit_set(ADDR_GPIO_0x37, 14, 1);                 //aud_l_vcmsel=1,表示将输出共模设置在0.9V，
    reg_bit_set(ADDR_GPIO_0x37, 13, 1);                 //en_audrefcal=1, 表示开启audio 参考电压温度校准电路模拟电路部分，
    reg_bit_set(ADDR_GPIO_0x37, 11, 0);                 //ckchposel=0, 表示chopper时钟沿选择；
    reg_bits_set(ADDR_GPIO_0x37, 9, 10, 0x1);           //ckchsel<1:0>=0x1, 选择chopper时钟频率。
    reg_bit_set(ADDR_GPIO_0x37, 8, 0);                  //enchpa=0, disenable pa chop.
    reg_bit_set(ADDR_GPIO_0x37, 7, (type >> 1) & 0x1);  //enchpga=1, enable pga chop.
    reg_bit_set(ADDR_GPIO_0x37, 6, 1);                  //enpga=1, enable pga.
    reg_bit_set(ADDR_GPIO_0x37, 3, 0);                  //vcminc=0,
    reg_bit_set(ADDR_GPIO_0x37, 2, 1);                  //bpswvdd=1,
    reg_bit_set(ADDR_GPIO_0x37, 0, 1);                  //pbsel=1;
}

static void audio_dac_enable(uint32_t enable)
{
    if(enable)
    {
        BK3000_PMU_PERI_PWDS  &= ~bit_PMU_AUD_PWD;
        BK3000_AUD_AUDIO_CONF |= ((1 << sft_AUD_DAC_ENABLE) | (1 << sft_AUD_DTMF_ENABLE));

        reg_bit_set(ADDR_GPIO_0x35, 12, 0); //dcocpwd=0
        reg_bit_set(ADDR_GPIO_0x37, 30, 1); //auddacl=0;
        reg_bit_set(ADDR_GPIO_0x37, 29, 1); //auddacr=0
        delay_ms(1);
        reg_bit_set(ADDR_GPIO_0x37, 24, 1); //enaudpacore=0;
        delay_ms(1);
        reg_bit_set(ADDR_GPIO_0x37, 23, 1); //enaudpadrive=0;
    }
    else
    {
        reg_bit_set(ADDR_GPIO_0x37, 23, 0); //enaudpadrive=0;
        reg_bit_set(ADDR_GPIO_0x37, 24, 0); //enaudpacore=0;
        reg_bit_set(ADDR_GPIO_0x37, 30, 0); //auddacl=0;
        reg_bit_set(ADDR_GPIO_0x37, 29, 0); //auddacr=0
        reg_bit_set(ADDR_GPIO_0x35, 12, 1); //dcocpwd=1

        BK3000_AUD_AUDIO_CONF &= ~((1 << sft_AUD_DAC_ENABLE) | (1 << sft_AUD_DTMF_ENABLE));
        BK3000_PMU_PERI_PWDS  |= bit_PMU_AUD_PWD;
    }
}

void audio_dac_dc_offset_calibration(uint32_t type)
{
    int32_t  i;
    int32_t  gain;
    uint32_t dc_offset_l;
    uint32_t dc_offset_r;
    uint32_t gpio_cfg_l;
    uint32_t gpio_cfg_r;

    CALIB_DBG("%s(%d)\r\n", __func__, type);

    audio_dac_init(type);
    audio_dac_enable(1);

    reg_bit_set(ADDR_GPIO_0x30, 31, 1);
    reg_bit_set(ADDR_GPIO_0x32, 11, 1);

    gpio_cfg_l = CFG_DAC_DC_OFFSET_CALIB_IO_L;
    gpio_cfg_r = CFG_DAC_DC_OFFSET_CALIB_IO_R;

    CFG_DAC_DC_OFFSET_CALIB_IO_L = 0x0C;
    CFG_DAC_DC_OFFSET_CALIB_IO_R = 0x0C;

    reg_bit_set(ADDR_GPIO_0x35, 12, 0);

    for(gain = 0x00; gain <= 0x1F; gain++)
    {
        reg_bits_set(ADDR_GPIO_0x36, 2, 6, gain);

        #if 0 //左右分开校准

        dc_offset_l = 0;
        for(i = 7; i >= 0; i--)
        {
            dc_offset_l |= 1 << i;
            reg_bits_set(ADDR_GPIO_0x35, 16, 23, dc_offset_l);
            delay_cycle(1000);
            if(!(CFG_DAC_DC_OFFSET_CALIB_IO_L & (1 << sft_GPIO_INPUT))) dc_offset_l &= ~(1 << i);
        }

        dc_offset_r = 0;
        for(i = 7; i >= 0; i--)
        {
            dc_offset_r |= 1 << i;
            reg_bits_set(ADDR_GPIO_0x35, 24, 31, dc_offset_r);
            delay_cycle(1000);
            if(!(CFG_DAC_DC_OFFSET_CALIB_IO_R & (1 << sft_GPIO_INPUT))) dc_offset_r &= ~(1 << i);
        }

        #else //左右一起校准

        dc_offset_l = 0;
        dc_offset_r = 0;
        for(i = 7; i >= 0; i--)
        {
            dc_offset_l |= 1 << i;
            dc_offset_r |= 1 << i;
            reg_bits_set(ADDR_GPIO_0x35, 16, 23, dc_offset_l);
            reg_bits_set(ADDR_GPIO_0x35, 24, 31, dc_offset_r);
            delay_cycle(1000);
            if(!(CFG_DAC_DC_OFFSET_CALIB_IO_L & (1 << sft_GPIO_INPUT))) dc_offset_l &= ~(1 << i);
            if(!(CFG_DAC_DC_OFFSET_CALIB_IO_R & (1 << sft_GPIO_INPUT))) dc_offset_r &= ~(1 << i);
        }

        #endif

        //CALIB_DBG("DAC DC offset calibration @ gain = 0x%02X, result: [%3d, %3d]\r\n", gain, dc_offset_l, dc_offset_r);
        CALIB_DBG("s_env_audio_dc_offset_1.dac_l_dc_offset[%2d] = %3d; s_env_audio_dc_offset_1.dac_r_dc_offset[%2d] = %3d;\r\n", gain, dc_offset_l, gain, dc_offset_r);
    }

    CFG_DAC_DC_OFFSET_CALIB_IO_L = gpio_cfg_l;
    CFG_DAC_DC_OFFSET_CALIB_IO_R = gpio_cfg_r;

    audio_dac_enable(0);
}

void charger_calibration_enable(uint32_t enable)
{
    reg_bit_set(ADDR_GPIO_0x34, 12, !!enable);
}

void charger_vlcf_calibration(uint32_t type)
{
    CALIB_DBG("%s(%d)\r\n", __func__, type);

    //Internal hardware calibration
    if(type == 0)
    {
        //vlcf calibration
        //>>> Added 4.2V voltage on vbattery
        //>>> Set pwd=0
        reg_bit_set(ADDR_GPIO_0x34, 21, 1);                         //calEn
        reg_bit_set(ADDR_GPIO_0x34, 20, 0);                         //softCalen
        reg_bit_set(ADDR_GPIO_0x33, 18, 0);                         //vlcfSel
        reg_bit_set(ADDR_GPIO_0x33, 17, 0);                         //IcalSel
        reg_bit_set(ADDR_GPIO_0x33, 16, 0);                         //vcvSel
        reg_bit_set(ADDR_GPIO_0x33, 21, 0);                         //vlcf_caltrig
        reg_bit_set(ADDR_GPIO_0x33, 21, 1);                         //vlcf_caltrig
        delay_cycle(100);                                           //Wait for at least 4 clock cycles
        reg_bit_set(ADDR_GPIO_0x33, 21, 0);                         //vlcf_caltrig
        delay_ms(1);                                                //Waiting for 1ms, calibration finished
        calib_charger_vlcf = reg_bits_get(ADDR_GPIO_0x3C, 18, 23);  //Read the value vcal<5:0>, Recorded

        CALIB_DBG("calib_charger_vlcf = 0x%02x\r\n", calib_charger_vlcf);
    }
    //External software calibration
    else
    {
        //TODO
    }
}

void charger_icp_calibration(uint32_t type)
{
    CALIB_DBG("%s(%d)\r\n", __func__, type);

    //Internal hardware calibration
    if(type == 0)
    {
        //Icp calibration
        //>>> Added parallel 60ohm resistor and 100nF capacitor from vbattery to ground.(Removed the external 4.2V)
        //>>> Set pwd=0
        //>>> Porb=0
        reg_bits_set(ADDR_GPIO_0x33, 8, 12, 4);                     //Icp=60mA
        reg_bit_set(ADDR_GPIO_0x34, 21, 1);                         //calEn
        reg_bit_set(ADDR_GPIO_0x34, 20, 0);                         //softCalen
        reg_bit_set(ADDR_GPIO_0x33, 18, 0);                         //vlcfSel
        reg_bits_set(ADDR_GPIO_0x33, 0, 5, calib_charger_vlcf);     //vcal<5:0>=previous vlcf calibration value
        reg_bit_set(ADDR_GPIO_0x33, 17, 0);                         //IcalSel
        reg_bit_set(ADDR_GPIO_0x33, 16, 0);                         //vcvSel
        reg_bit_set(ADDR_GPIO_0x33, 20, 0);                         //Ical_trig
        reg_bit_set(ADDR_GPIO_0x33, 20, 1);                         //Ical_trig
        delay_cycle(100);                                           //Wait for at least 4 clock cycles
        reg_bit_set(ADDR_GPIO_0x33, 20, 0);                         //Ical_trig
        delay_ms(1);                                                //Waiting for 1ms, calibration finished
        calib_charger_icp = reg_bits_get(ADDR_GPIO_0x3C, 12, 16);   //Read the value Ical<4:0>, Recorded

        CALIB_DBG("calib_charger_icp = 0x%02x\r\n", calib_charger_icp);
    }
    //External software calibration
    else
    {
        //TODO
    }
}

void charger_vcv_calibration(uint32_t type)
{
    CALIB_DBG("%s(%d)\r\n", __func__, type);

    //Internal hardware calibration
    if(type == 0)
    {
        //vcv calibration
        //>>> Added 4.2V voltage on vbattery
        //>>> Set pwd=0
        //>>> Porb=0
        reg_bits_set(ADDR_GPIO_0x33, 8, 12, 4);                     //Icp=60mA
        reg_bit_set(ADDR_GPIO_0x34, 21, 1);                         //calEn
        reg_bit_set(ADDR_GPIO_0x34, 20, 0);                         //softCalen
        reg_bit_set(ADDR_GPIO_0x33, 18, 1);                         //vlcfSel
        reg_bits_set(ADDR_GPIO_0x33, 0, 5, calib_charger_vlcf);     //vcal<5:0>=previous vlcf calibration value
        reg_bit_set(ADDR_GPIO_0x33, 17, 1);                         //IcalSel
        reg_bits_set(ADDR_GPIO_0x34, 27, 31, calib_charger_icp);    //Ical<4:0>=previous Ical calibration value
        reg_bit_set(ADDR_GPIO_0x33, 16, 0);                         //vcvSel
        reg_bit_set(ADDR_GPIO_0x33, 19, 0);                         //vcv_caltrig
        reg_bit_set(ADDR_GPIO_0x33, 19, 1);                         //vcv_caltrig
        delay_cycle(100);                                           //Wait for at least 4 clock cycles
        reg_bit_set(ADDR_GPIO_0x33, 19, 0);                         //vcv_caltrig
        delay_ms(1);                                                //Waiting for 1ms, calibration finished
        calib_charger_vcv = reg_bits_get(ADDR_GPIO_0x3C, 24, 28);   //Read the value vcvcal<4:0>, Recorded

        CALIB_DBG("calib_charger_vlcf = 0x%02x\r\n", calib_charger_vcv);
    }
    //External software calibration
    else
    {
        //TODO
    }
}

void sar_adc_enable(uint32_t channel, uint32_t enable)
{
    if(enable)
    {
        uint32_t i = 20, t;

        //SAR-ADC configurations
        reg_bit_set((uint32_t)&BK3000_PMU_PERI_PWDS, 12, 0);            //Power ON SAR-ADC
        reg_bit_set((uint32_t)&BK3000_PMU_PERI_PWDS, 18, 0);            //Select clock source for SAR-ADC, 0: XTAL, 1:RSOC
        reg_bit_set((uint32_t)&BK3000_SARADC_ADC_CONF, 2, 1);           //Digital SAR-ADC enable
        reg_bits_set((uint32_t)&BK3000_SARADC_ADC_CONF, 3, 5, channel); //SAR-ADC channel set
        reg_bits_set((uint32_t)&BK3000_SARADC_ADC_CONF, 11, 12, 3);     //drop

        while(i--)
        {
            t = SAR_ADC_WAIT_BUSY_COUNT;
            reg_bits_set((uint32_t)&BK3000_SARADC_ADC_CONF, 0, 1, 1);
            //while((BK3000_SARADC_ADC_CONF & bit_SARADC_CONF_BUSY));
            while(t-- && (BK3000_SARADC_ADC_CONF & bit_SARADC_CONF_BUSY));
            t = BK3000_SARADC_ADC_DATA; (void)t;
        }

        t  = BK3000_SARADC_ADC_DATA;
        t += BK3000_SARADC_ADC_DATA;
        t += BK3000_SARADC_ADC_DATA;
        t += BK3000_SARADC_ADC_DATA;
        (void)t;
    }
    else
    {
        reg_bit_set((uint32_t)&BK3000_PMU_PERI_PWDS, 12, 1);            //Power DOWN SAR-ADC
        reg_bit_set((uint32_t)&BK3000_SARADC_ADC_CONF, 15, 1);			//SAR-ADC INT clear
        reg_bit_set((uint32_t)&BK3000_SARADC_ADC_CONF, 2, 0);           //Digital SAR-ADC disable
    }
}

uint32_t sar_adc_read(void)
{
    uint32_t i = SAR_ADC_READ_AVG_COUNT;
    uint32_t adc_data;

    //Read SAR-ADC data
    //while(!(BK3000_SARADC_ADC_CONF & bit_SARADC_CONF_FIFO_EMPTY)) adc_data = BK3000_SARADC_ADC_DATA;
    adc_data = BK3000_SARADC_ADC_DATA;
    adc_data = BK3000_SARADC_ADC_DATA;
    adc_data = BK3000_SARADC_ADC_DATA;
    adc_data = BK3000_SARADC_ADC_DATA;
    adc_data = 0;
    while(i--)
    {
        uint32_t t = SAR_ADC_WAIT_BUSY_COUNT;
        reg_bits_set((uint32_t)&BK3000_SARADC_ADC_CONF, 0, 1, 1);
        //while((BK3000_SARADC_ADC_CONF & bit_SARADC_CONF_BUSY));
        while(t-- && (BK3000_SARADC_ADC_CONF & bit_SARADC_CONF_BUSY));
        adc_data += BK3000_SARADC_ADC_DATA;
    }

    adc_data /= SAR_ADC_READ_AVG_COUNT;

    //CALIB_DBG("%s() = 0x%X (%d)\r\n", __func__, adc_data, adc_data);

    return adc_data;
}

void sar_adc_dout_2_vin(uint32_t dout)
{
    if(calib_sar_adc_dc && calib_sar_adc_4p2)
    {
        uint32_t vin = ((dout - (calib_sar_adc_dc - 512)) * 420 + calib_sar_adc_4p2 / 2) / calib_sar_adc_4p2;

        (void)vin;

        CALIB_DBG("%s(%d) = %d.%02d\r\n", __func__, dout, vin / 100, vin % 100);
    }
}

/**
 * ADC输出Dout与模拟输入Vin的对应关系为：
 *
 *   Dout = (Y - (X - 512)) / (4.2 * 0.65) * Vin + (X - 512)
 *   Vin  = [Dout - (X - 512)] * (4.2 * 0.65) / (Y - (X - 512))
 *
 *   注：当选择ADC通道0时，系数0.65不需要，改为1.00。
 *
 * 最终校准会有+/- 0.5个LSB（大概+/- 1.5mV）的误差
 */
void sar_adc_calibration(void)
{
    uint32_t X, Y;

    CALIB_DBG("%s()\r\n", __func__);

    sar_adc_enable(0, 1);

    //SAR-ADC calibration settings
    reg_bit_set(ADDR_GPIO_0x30, 15, 1); //ABB CB enable
    reg_bit_set(ADDR_GPIO_0x30, 13, 1); //SAR-ADC reference selection, 0: VDD/2, 1:Vbg1p2
    reg_bit_set(ADDR_GPIO_0x30,  6, 1); //SAR-ADC calibration enable

    //Read SAR-ADC data
    X = sar_adc_read();

    //SAR-ADC calibration settings
    reg_bit_set(ADDR_GPIO_0x30, 15, 1); //ABB CB enable
    reg_bit_set(ADDR_GPIO_0x30, 13, 1); //SAR-ADC reference selection, 0: VDD/2, 1:Vbg1p2
    reg_bit_set(ADDR_GPIO_0x30,  6, 0); //SAR-ADC calibration disable

    //Read SAR-ADC data
    Y = sar_adc_read();

    sar_adc_enable(0, 0);

    calib_sar_adc_dc  = X;
    calib_sar_adc_4p2 = Y;

    CALIB_DBG("X = 0x%X (%d), Y = 0x%X (%d)\r\n", X, X, Y, Y);
}

void tx_calibration_init(void)
{
    CALIB_DBG("%s()\r\n", __func__);

    set_spr(SPR_VICMR(0), get_spr(SPR_VICMR(0)) & ~(1 << VIC_SADC_ISR_INDEX));
    sar_adc_enable(6, 1);

    //No need to do it
    //memset(XVR_ANALOG_REG_BAK, 0, sizeof(XVR_ANALOG_REG_BAK));

    //BK3000_A0_CONFIG = 0x81061621;
    //BK3000_A0_CONFIG = 0x81169721;//0x81161621;

    reg_bit_set(ADDR_GPIO_0x30, 8, 1);                              //16. set GADC CH6 to TSSI path
    reg_bit_set(ADDR_GPIO_0x30, 13, 1);                             //SAR-ADC source selection, 0:VDD/2, 1:Vbg1p2
    reg_bit_set(ADDR_GPIO_0x30, 25, 0);
    reg_bit_set(ADDR_GPIO_0x30, 26, 0);
    reg_bit_set(ADDR_GPIO_0x30, 30, 0);

    //以下两步操作前面已设置
    //reg_bit_set(ADDR_GPIO_0x30, 13, 1);                             //SAR-ADC source selection, 0:VDD/2, 1:Vbg1p2
    //reg_bit_set(ADDR_GPIO_0x30, 15, 1);                             //17. ABB central bias enable

    //以下两步操作前面已设置
    //reg_bit_set(ADDR_GPIO_0x30, 20, 1); //BT LDO enable
    //reg_bit_set(ADDR_GPIO_0x30, 31, 1); //BT digital LDO power ON

    //以下两步操作已在MAIN初始化中设置
    //reg_bit_set(ADDR_GPIO_0x32, 10, 1); //enable 1.8V power source
    //reg_bit_set(ADDR_GPIO_0x32, 11, 1); //enable 1.8V LDO

    xvr_reg_0x09_bak = xvr_reg_get(ADDR_XVR_0x09);
    xvr_reg_0x0b_bak = xvr_reg_get(ADDR_XVR_0x0B);
    xvr_reg_0x0c_bak = xvr_reg_get(ADDR_XVR_0x0C);
    xvr_reg_0x24_bak = *(volatile uint32_t*)ADDR_XVR_0x24;
    xvr_reg_0x25_bak = *(volatile uint32_t*)ADDR_XVR_0x25;
    xvr_reg_0x31_bak = *(volatile uint32_t*)ADDR_XVR_0x31;
    
    //xvr_reg_set(ADDR_XVR_0x00, 0xA15F2012); //A15F2012
    //xvr_reg_set(ADDR_XVR_0x01, 0x7C805A26); //7C805A26
    //xvr_reg_set(ADDR_XVR_0x02, 0x2C924005); //2C924005
    //xvr_reg_set(ADDR_XVR_0x03, 0x00272830); //CK52M
    //xvr_reg_set(ADDR_XVR_0x07, 0xFEE27820); //LNA+RX Gain
    //xvr_reg_set(ADDR_XVR_0x08, 0xD01C5000); //ADC+Test Buffer
    xvr_reg_set(ADDR_XVR_0x09, 0x2ADF82A0); //RF power down
    //xvr_reg_set(ADDR_XVR_0x0A, 0x428CDF77); //RSSI+LDO ctrl
    xvr_reg_set(ADDR_XVR_0x0B, 0x80F4E64F); //TX
    xvr_reg_set(ADDR_XVR_0x0C, 0x000F0339); //TX
    //xvr_reg_set(ADDR_XVR_0x0D, 0x5481011F); //XTAL26M+RC100K
    //xvr_reg_set(ADDR_XVR_0x0E, 0x00B09350); //DPLL
    //xvr_reg_set(ADDR_XVR_0x0F, 0x3B13B13B); //DPLL

    *(volatile uint32_t*)ADDR_XVR_0x24 = 0x0029;
    *(volatile uint32_t*)ADDR_XVR_0x25 = 0x7800;

    //*(volatile uint32_t*)ADDR_XVR_0x24 = 0x0000;
    //*(volatile uint32_t*)ADDR_XVR_0x25 = 0x0000;
    //*(volatile uint32_t*)ADDR_XVR_0x25 = 0x7800;
}

void tx_calibration_deinit(void)
{
    CALIB_DBG("%s()\r\n", __func__);

    reg_bit_set(ADDR_GPIO_0x30, 8, 0);
    reg_bit_set(ADDR_GPIO_0x30, 25, 1);
    reg_bit_set(ADDR_GPIO_0x30, 30, 1);

    xvr_reg_set(ADDR_XVR_0x09, xvr_reg_0x09_bak);
    xvr_reg_set(ADDR_XVR_0x0B, xvr_reg_0x0b_bak);
    xvr_reg_set(ADDR_XVR_0x0C, xvr_reg_0x0c_bak);

    *(volatile uint32_t*)ADDR_XVR_0x24 = xvr_reg_0x24_bak;
    *(volatile uint32_t*)ADDR_XVR_0x25 = xvr_reg_0x25_bak;
    *(volatile uint32_t*)ADDR_XVR_0x31 = xvr_reg_0x31_bak;

    xvr_reg_bit_set(ADDR_XVR_0x0B, 15, 0);
    xvr_reg_bit_set(ADDR_XVR_0x0C, 16, 0);
    xvr_reg_bit_set(ADDR_XVR_0x0C, 21, 0);

    sar_adc_enable(6, 0);
    set_spr(SPR_VICMR(0), get_spr(SPR_VICMR(0)) | (1 << VIC_SADC_ISR_INDEX));
}

void tx_dc_calibration(void)
{
    int8_t start;
    int8_t stop;

    CALIB_DBG("%s()\r\n", __func__);

    /******************************** 第一步校准 ********************************/

    //>>>校准准备：
    //1. 确认BK3266寄存器初始化正确，即功率能正常输出(进入TX PN9 Mode)。
    //@see tx_calibration_init
    xvr_reg_bit_set(ADDR_XVR_0x0C, 16, 1);              //2. TSSIen=1, enable TX TSSI test mode
    xvr_reg_bit_set(ADDR_XVR_0x0C, 21, 0);              //3. enPcal=0, enable TSSI for output power calibration
    xvr_reg_bit_set(ADDR_XVR_0x0C, 20, 0);              //4. enIQcal=0, disable TSSI for IQ calibration path
    xvr_reg_bit_set(ADDR_XVR_0x0C, 19, 1);              //5. enDCcal=1, enable TSSI for DC calibration
    xvr_reg_bits_set(ADDR_XVR_0x0C, 0, 4, 0x10);        //6. set IDoff to the result of TX DC calibration
    xvr_reg_bits_set(ADDR_XVR_0x0C, 5, 9, 0x10);        //7. set QDoff to the result of TX DC calibration
    xvr_reg_bits_set(ADDR_XVR_0x0B, 4, 8, 0x04);        //8. PA output power -28dBm, to be confirm

    *(volatile uint32_t*)ADDR_XVR_0x31 = 0x07000000;    //9. enable TX LO leakage power output
    *(volatile uint32_t*)ADDR_XVR_0x32 = 0xFFFF0000;    //10. set DC and gain compensation as default value
    *(volatile uint32_t*)ADDR_XVR_0x33 = 0x00000000;    //11. set phase compensation as default value

    //>>>I路校准：
    start = 0xF0;
    stop  = 0x10;

    do
    {
        uint32_t tssi_start, tssi_stop;

        reg_bits_set(ADDR_XVR_0x32, 8, 15, start);
        delay_us(100);
        tssi_start = sar_adc_read();

        reg_bits_set(ADDR_XVR_0x32, 8, 15, stop);
        delay_us(100);
        tssi_stop = sar_adc_read();

        //CALIB_DBG("start = %d, stop = %d, tssi_start = %d, high = %d\n", start, stop, tssi_start, tssi_stop);

        if(start + 1 == stop) break;

        if(tssi_start < tssi_stop)
        {
            stop = (start + stop) / 2;
            calib_tx_i_dc_offset = start;
        }
        else
        {
            start = (start + stop) / 2;
            calib_tx_i_dc_offset = stop;
        }

    }while(start < stop);

    reg_bits_set(ADDR_XVR_0x32, 8, 15, calib_tx_i_dc_offset);

    CALIB_DBG("calib_tx_i_dc_offset = 0x%02X\r\n", calib_tx_i_dc_offset);

    //>>>Q路校准：
    start = 0xF0;
    stop  = 0x10;

    do
    {
        uint32_t tssi_start, tssi_stop;

        reg_bits_set(ADDR_XVR_0x32, 0, 7, start);
        delay_us(100);
        tssi_start = sar_adc_read();

        reg_bits_set(ADDR_XVR_0x32, 0, 7, stop);
        delay_us(100);
        tssi_stop = sar_adc_read();

        //CALIB_DBG("start = %d, stop = %d, tssi_start = %d, high = %d\n", start, stop, tssi_start, tssi_stop);

        if(start + 1 == stop) break;

        if(tssi_start < tssi_stop)
        {
            stop = (start + stop) / 2;
            calib_tx_q_dc_offset = start;
        }
        else
        {
            start = (start + stop) / 2;
            calib_tx_q_dc_offset = stop;
        }

    }while(start < stop);

    reg_bits_set(ADDR_XVR_0x32, 0, 7, calib_tx_q_dc_offset);

    CALIB_DBG("calib_tx_q_dc_offset = 0x%02X\r\n", calib_tx_q_dc_offset);

    /******************************** 第二步校准 ********************************/

    //>>>校准准备：
    //在第一步校准的准备工作基础上，进行如下操作。
    xvr_reg_bits_set(ADDR_XVR_0x0B, 4, 8, 0xA); //PA output power -28dBm, to be confirm

    //>>>I路校准：
    start = 0x01;
    stop  = 0x1F;

    do
    {
        uint32_t tssi_start, tssi_stop;

        xvr_reg_bits_set(ADDR_XVR_0x0C, 5, 9, start);
        delay_us(100);
        tssi_start = sar_adc_read();

        xvr_reg_bits_set(ADDR_XVR_0x0C, 5, 9, stop);
        delay_us(100);
        tssi_stop = sar_adc_read();

        //CALIB_DBG("start = %d, stop = %d, low = %d, high = %d\n", start, stop, tssi_start, tssi_stop);

        if(start + 1 == stop) break;

        if(tssi_start < tssi_stop)
        {
            stop = (start + stop) / 2;
            calib_tx_i_doff = start;
        }
        else
        {
            start = (start + stop) / 2;
            calib_tx_i_doff = stop;
        }

    }while(start < stop);

    xvr_reg_bits_set(ADDR_XVR_0x0C, 5, 9, calib_tx_i_doff);

    CALIB_DBG("calib_tx_i_doff = 0x%02X\r\n", calib_tx_i_doff);

    //>>>Q路校准：
    start = 0x01;
    stop  = 0x1F;

    do
    {
        uint32_t tssi_start, tssi_stop;

        xvr_reg_bits_set(ADDR_XVR_0x0C, 0, 4, start);
        delay_us(100);
        tssi_start = sar_adc_read();

        xvr_reg_bits_set(ADDR_XVR_0x0C, 0, 4, stop);
        delay_us(100);
        tssi_stop = sar_adc_read();

        //CALIB_DBG("start = %d, stop = %d, tssi_start = %d, high = %d\n", start, stop, tssi_start, tssi_stop);

        if(start + 1 == stop) break;

        if(tssi_start < tssi_stop)
        {
            stop = (start + stop) / 2;
            calib_tx_q_doff = start;
        }
        else
        {
            start = (start + stop) / 2;
            calib_tx_q_doff = stop;
        }

    }while(start < stop);

    xvr_reg_bits_set(ADDR_XVR_0x0C, 0, 4, calib_tx_q_doff);

    CALIB_DBG("calib_tx_q_doff = 0x%02X\r\n", calib_tx_q_doff);
}

void tx_q_const_iqcal_p_calibration(void)
{
    int32_t  gain0, gain;

    CALIB_DBG("%s()\r\n", __func__);

    #define Q_CONST_GAIN_THRESHOLD    (390)

    xvr_reg_bits_set(ADDR_XVR_0x0B, 4, 8, 0x00);

    xvr_reg_bit_set(ADDR_XVR_0x0C, 21, 0); //enable  TSSI for output power calibration
    xvr_reg_bit_set(ADDR_XVR_0x0C, 20, 1); //enable  TSSI for IQ calibration path
    xvr_reg_bit_set(ADDR_XVR_0x0C, 19, 0); //disable TSSI for DC calibration

    (*(uint32_t*)ADDR_XVR_0x32) |= 0xFFFF0000;

    reg_bits_set(ADDR_XVR_0x31,  8, 15, 0);
    reg_bits_set(ADDR_XVR_0x31, 16, 23, 0);
    delay_us(100);
    gain0 = sar_adc_read() * 2;

    xvr_reg_bits_set(ADDR_XVR_0x0B, 4, 8, 0x02);

    #if (CFG_TX_CALIB_ALGORITHM ==  TX_CALIB_ALGORITHM_ERGODIC) //遍历搜索

    uint32_t q_const;
    int32_t  diff, diff_min = 0x7FFFFFFF;

    for(q_const = 36; q_const <= 144; q_const += 4)
    {
        reg_bits_set(ADDR_XVR_0x31, 16, 23, q_const);
        delay_us(100);
        gain = sar_adc_read();

        reg_bits_set(ADDR_XVR_0x31, 16, 23, 256 - q_const);
        delay_us(100);
        gain += sar_adc_read();

        diff = abs(gain - gain0 - Q_CONST_GAIN_THRESHOLD);

        if(diff < diff_min)
        {
            diff_min = diff;
            TX_Q_CONST_IQCAL_P = q_const;
        }

        CALIB_DBG("q_const = %d, diff = %d\r\n", q_const, diff);
    }

    #elif  (CFG_TX_CALIB_ALGORITHM == TX_CALIB_ALGORITHM_DICHOTOMY) //二分搜索

    uint32_t start = 36;
    uint32_t stop  = 144;

    do
    {
        int32_t diff_start;
        int32_t diff_stop;

        reg_bits_set(ADDR_XVR_0x31, 16, 23, start);
        delay_us(100);
        gain = sar_adc_read();

        reg_bits_set(ADDR_XVR_0x31, 16, 23, 256 - start);
        delay_us(100);
        gain += sar_adc_read();

        diff_start = abs(gain - gain0 - Q_CONST_GAIN_THRESHOLD);

        reg_bits_set(ADDR_XVR_0x31, 16, 23, stop);
        delay_us(100);
        gain = sar_adc_read();

        reg_bits_set(ADDR_XVR_0x31, 16, 23, 256 - stop);
        delay_us(100);
        gain += sar_adc_read();

        diff_stop = abs(gain - gain0 - Q_CONST_GAIN_THRESHOLD);

        CALIB_DBG("start = %d, stop = %d, diff_start = %d, diff_stop = %d\r\n", start, stop, diff_start, diff_stop);

        if(start + 1 == stop) break;

        if(diff_start < diff_stop)
        {
            stop = (start + stop) / 2;
            TX_Q_CONST_IQCAL_P = start;
        }
        else
        {
            start = (start + stop) / 2;
            TX_Q_CONST_IQCAL_P = stop;
        }

    }while(start < stop);

    #endif

    TX_Q_CONST_IQCAL_P_4_PHASE = (uint32_t)(TX_Q_CONST_IQCAL_P / 1.4142 + 0.5);

    CALIB_DBG("TX_Q_CONST_IQCAL_P         = %d\r\n", TX_Q_CONST_IQCAL_P);
    CALIB_DBG("TX_Q_CONST_IQCAL_P_4_PHASE = %d\r\n", TX_Q_CONST_IQCAL_P_4_PHASE);
}

void tx_iq_gain_imbalance_calibration(void)
{
    int32_t i;
    int32_t gain_i;
    int32_t gain_q;

    CALIB_DBG("%s()\r\n", __func__);

    calib_tx_i_gain_comp = 0xFF;
    calib_tx_q_gain_comp = 0xFF;

    xvr_reg_bits_set(ADDR_XVR_0x0B, 4, 8, 0x02); //FIXME

    xvr_reg_bit_set(ADDR_XVR_0x0C, 21, 0); //enable  TSSI for output power calibration
    xvr_reg_bit_set(ADDR_XVR_0x0C, 20, 1); //enable  TSSI for IQ calibration path
    xvr_reg_bit_set(ADDR_XVR_0x0C, 19, 0); //disable TSSI for DC calibration

    (*(uint32_t*)ADDR_XVR_0x32) |= 0xFFFF0000;

    reg_bits_set(ADDR_XVR_0x31,  8, 15, 0);
    reg_bits_set(ADDR_XVR_0x31, 16, 23, TX_Q_CONST_IQCAL_P);
    delay_us(100);
    gain_i = sar_adc_read();

    reg_bits_set(ADDR_XVR_0x31, 16, 23, 256 - TX_Q_CONST_IQCAL_P);
    delay_us(100);
    gain_i += sar_adc_read();

    reg_bits_set(ADDR_XVR_0x31,  8, 15, TX_Q_CONST_IQCAL_P);
    reg_bits_set(ADDR_XVR_0x31, 16, 23, 0);
    delay_us(100);
    gain_q = sar_adc_read();

    reg_bits_set(ADDR_XVR_0x31,  8, 15, 256 - TX_Q_CONST_IQCAL_P);
    delay_us(100);
    gain_q += sar_adc_read();

    if(abs(gain_i - gain_q) > 1)
    {
        if(gain_i > gain_q)
        {
            #if (CFG_TX_CALIB_ALGORITHM ==  TX_CALIB_ALGORITHM_ERGODIC) //遍历搜索

            int32_t diff;
            int32_t diff_min = abs(gain_i - gain_q);

            for(i = 205; i < 255; i++)
            {
                reg_bits_set(ADDR_XVR_0x32, 24, 31, i);

                reg_bits_set(ADDR_XVR_0x31,  8, 15, 0);
                reg_bits_set(ADDR_XVR_0x31, 16, 23, TX_Q_CONST_IQCAL_P);
                delay_us(100);
                gain_i = sar_adc_read();

                reg_bits_set(ADDR_XVR_0x31, 16, 23, 256 - TX_Q_CONST_IQCAL_P);
                delay_us(100);
                gain_i += sar_adc_read();

                diff = abs(gain_i - gain_q);
                if(diff < diff_min)
                {
                    diff_min = diff;
                    calib_tx_i_gain_comp = i;
                }

                CALIB_DBG("i_gain_comp = %d, q_gain_comp = 255, gain_i = %d, gain_q = %d, diff = %d\r\n", i, gain_i, gain_q, diff);
            }

            #elif  (CFG_TX_CALIB_ALGORITHM == TX_CALIB_ALGORITHM_DICHOTOMY) //二分搜索

            calib_tx_i_gain_comp = 0;

            for(i = 7; i >= 0; i--)
            {
                calib_tx_i_gain_comp |= 1 << i;
                reg_bits_set(ADDR_XVR_0x32, 24, 31, calib_tx_i_gain_comp);

                reg_bits_set(ADDR_XVR_0x31,  8, 15, 0);
                reg_bits_set(ADDR_XVR_0x31, 16, 23, TX_Q_CONST_IQCAL_P);
                delay_us(100);
                gain_i = sar_adc_read();

                reg_bits_set(ADDR_XVR_0x31, 16, 23, 256 - TX_Q_CONST_IQCAL_P);
                delay_us(100);
                gain_i += sar_adc_read();

                if(abs(gain_i - gain_q) < 1) break;
                if(gain_i > gain_q) calib_tx_i_gain_comp &= ~(1 << i);
            }

            #endif
        }
        else
        {
            #if (CFG_TX_CALIB_ALGORITHM ==  TX_CALIB_ALGORITHM_ERGODIC) //遍历搜索

            int32_t diff;
            int32_t diff_min = abs(gain_i - gain_q);

            for(i = 205; i < 255; i++)
            {
                reg_bits_set(ADDR_XVR_0x32, 16, 23, i);

                reg_bits_set(ADDR_XVR_0x31,  8, 15, TX_Q_CONST_IQCAL_P);
                reg_bits_set(ADDR_XVR_0x31, 16, 23, 0);
                delay_us(100);
                gain_q = sar_adc_read();

                reg_bits_set(ADDR_XVR_0x31,  8, 15, 256 - TX_Q_CONST_IQCAL_P);
                delay_us(100);
                gain_q += sar_adc_read();

                diff = abs(gain_i - gain_q);
                if(diff < diff_min)
                {
                    diff_min = diff;
                    calib_tx_q_gain_comp = i;
                }

                CALIB_DBG("i_gain_comp = 255, q_gain_comp = %d, gain_i = %d, gain_q = %d, diff = %d\r\n", i, gain_i, gain_q, diff);
            }

            #elif  (CFG_TX_CALIB_ALGORITHM == TX_CALIB_ALGORITHM_DICHOTOMY) //二分搜索

            calib_tx_q_gain_comp = 0;

            for(i = 7; i >= 0; i--)
            {
                calib_tx_q_gain_comp |= 1 << i;
                reg_bits_set(ADDR_XVR_0x32, 16, 23, calib_tx_q_gain_comp);

                reg_bits_set(ADDR_XVR_0x31,  8, 15, TX_Q_CONST_IQCAL_P);
                reg_bits_set(ADDR_XVR_0x31, 16, 23, 0);
                delay_us(100);
                gain_q = sar_adc_read();

                reg_bits_set(ADDR_XVR_0x31,  8, 15, 256 - TX_Q_CONST_IQCAL_P);
                delay_us(100);
                gain_q += sar_adc_read();

                if(abs(gain_i - gain_q) < 1) break;
                if(gain_q > gain_i) calib_tx_q_gain_comp &= ~(1 << i);
            }

            #endif
        }
    }

    xvr_reg_bits_set(ADDR_XVR_0x0B, 4, 8, 0x04);//FIXME

    reg_bits_set(ADDR_XVR_0x32, 24, 31, calib_tx_i_gain_comp);
    reg_bits_set(ADDR_XVR_0x32, 16, 23, calib_tx_q_gain_comp);

    CALIB_DBG("calib_tx_i_gain_comp = 0x%02X\r\n", calib_tx_i_gain_comp);
    CALIB_DBG("calib_tx_q_gain_comp = 0x%02X\r\n", calib_tx_q_gain_comp);
}

uint32_t tx_iq_phase_imbalance_calibration(void)
{
    uint32_t tx_ty2 = 0;
    int32_t  phase_i;
    int32_t  phase_q;

    CALIB_DBG("%s()\r\n", __func__);

    xvr_reg_bits_set(ADDR_XVR_0x0B, 4, 8, 0x02);//FIXME

    #if (CFG_TX_CALIB_ALGORITHM ==  TX_CALIB_ALGORITHM_ERGODIC) //遍历搜索

    int8_t  tx_phase_comp;
    int32_t diff, diff_min = 0x7FFFFFFF;

    for(tx_phase_comp = 0xE0; tx_phase_comp <= 0x20; tx_phase_comp++)
    {
        int32_t t = tx_phase_comp;

        t = ((t * t << 18) + (t * t * t * t) * 3 + (1 << 26)) >> 27;

        reg_bits_set(ADDR_XVR_0x33, 8, 15, tx_phase_comp);
        reg_bits_set(ADDR_XVR_0x33, 0,  7, t);

        reg_bits_set(ADDR_XVR_0x31,  8, 15, 256 - TX_Q_CONST_IQCAL_P_4_PHASE);
        reg_bits_set(ADDR_XVR_0x31, 16, 23, TX_Q_CONST_IQCAL_P_4_PHASE);
        delay_us(100);
        phase_i = sar_adc_read();

        reg_bits_set(ADDR_XVR_0x31,  8, 15, TX_Q_CONST_IQCAL_P_4_PHASE);
        reg_bits_set(ADDR_XVR_0x31, 16, 23, 256 - TX_Q_CONST_IQCAL_P_4_PHASE);
        delay_us(100);
        phase_i += sar_adc_read();

        reg_bits_set(ADDR_XVR_0x31,  8, 15, TX_Q_CONST_IQCAL_P_4_PHASE);
        reg_bits_set(ADDR_XVR_0x31, 16, 23, TX_Q_CONST_IQCAL_P_4_PHASE);
        delay_us(100);
        phase_q = sar_adc_read();

        reg_bits_set(ADDR_XVR_0x31,  8, 15, 256 - TX_Q_CONST_IQCAL_P_4_PHASE);
        reg_bits_set(ADDR_XVR_0x31, 16, 23, 256 - TX_Q_CONST_IQCAL_P_4_PHASE);
        delay_us(100);
        phase_q += sar_adc_read();

        diff = abs(phase_i - phase_q);
        if(diff_min > diff)
        {
            diff_min = diff;
            tx_ty2   = t;
            calib_tx_phase_comp = tx_phase_comp;
        }

        CALIB_DBG("@tx_phase_comp = %d (0x%0X), tx_ty2 = %d, diff = %d\r\n", tx_phase_comp, tx_phase_comp & 0xFF, t, diff);
    }

    #elif  (CFG_TX_CALIB_ALGORITHM == TX_CALIB_ALGORITHM_DICHOTOMY) //二分搜索

    int8_t   start = 0xE0;
    int8_t   stop  = 0x20;
    uint32_t tx_ty2_start;
    uint32_t tx_ty2_stop;

    do
    {
        uint32_t diff_start;
        uint32_t diff_stop;

        int32_t t = start;

        tx_ty2_start = ((t * t << 18) + (t * t * t * t) * 3 + (1 << 26)) >> 27;

        reg_bits_set(ADDR_XVR_0x33, 8, 15, start);
        reg_bits_set(ADDR_XVR_0x33, 0,  7, tx_ty2_start);

        reg_bits_set(ADDR_XVR_0x31,  8, 15, 256 - TX_Q_CONST_IQCAL_P_4_PHASE);
        reg_bits_set(ADDR_XVR_0x31, 16, 23, TX_Q_CONST_IQCAL_P_4_PHASE);
        delay_us(100);
        phase_i = sar_adc_read();

        reg_bits_set(ADDR_XVR_0x31,  8, 15, TX_Q_CONST_IQCAL_P_4_PHASE);
        reg_bits_set(ADDR_XVR_0x31, 16, 23, 256 - TX_Q_CONST_IQCAL_P_4_PHASE);
        delay_us(100);
        phase_i += sar_adc_read();

        reg_bits_set(ADDR_XVR_0x31,  8, 15, TX_Q_CONST_IQCAL_P_4_PHASE);
        reg_bits_set(ADDR_XVR_0x31, 16, 23, TX_Q_CONST_IQCAL_P_4_PHASE);
        delay_us(100);
        phase_q = sar_adc_read();

        reg_bits_set(ADDR_XVR_0x31,  8, 15, 256 - TX_Q_CONST_IQCAL_P_4_PHASE);
        reg_bits_set(ADDR_XVR_0x31, 16, 23, 256 - TX_Q_CONST_IQCAL_P_4_PHASE);
        delay_us(100);
        phase_q += sar_adc_read();

        diff_start = abs(phase_i - phase_q);

        t = stop;

        tx_ty2_stop = ((t * t << 18) + (t * t * t * t) * 3 + (1 << 26)) >> 27;

        reg_bits_set(ADDR_XVR_0x33, 8, 15, stop);
        reg_bits_set(ADDR_XVR_0x33, 0,  7, tx_ty2_stop);

        reg_bits_set(ADDR_XVR_0x31,  8, 15, 256 - TX_Q_CONST_IQCAL_P_4_PHASE);
        reg_bits_set(ADDR_XVR_0x31, 16, 23, TX_Q_CONST_IQCAL_P_4_PHASE);
        delay_us(100);
        phase_i = sar_adc_read();

        reg_bits_set(ADDR_XVR_0x31,  8, 15, TX_Q_CONST_IQCAL_P_4_PHASE);
        reg_bits_set(ADDR_XVR_0x31, 16, 23, 256 - TX_Q_CONST_IQCAL_P_4_PHASE);
        delay_us(100);
        phase_i += sar_adc_read();

        reg_bits_set(ADDR_XVR_0x31,  8, 15, TX_Q_CONST_IQCAL_P_4_PHASE);
        reg_bits_set(ADDR_XVR_0x31, 16, 23, TX_Q_CONST_IQCAL_P_4_PHASE);
        delay_us(100);
        phase_q = sar_adc_read();

        reg_bits_set(ADDR_XVR_0x31,  8, 15, 256 - TX_Q_CONST_IQCAL_P_4_PHASE);
        reg_bits_set(ADDR_XVR_0x31, 16, 23, 256 - TX_Q_CONST_IQCAL_P_4_PHASE);
        delay_us(100);
        phase_q += sar_adc_read();

        diff_stop = abs(phase_i - phase_q);

        CALIB_DBG("start = %d, stop = %d, diff_start = %d, diff_stop = %d\r\n", start, stop, diff_start, diff_stop);

        if(start + 1 == stop) break;

        if(diff_start < diff_stop)
        {
            stop   = (start + stop) / 2;
            tx_ty2 = tx_ty2_start;
            calib_tx_phase_comp = start;
        }
        else
        {
            start  = (start + stop) / 2;
            tx_ty2 = tx_ty2_stop;
            calib_tx_phase_comp = stop;
        }

    }while(start < stop);

    #endif

    reg_bits_set(ADDR_XVR_0x33, 8, 15, calib_tx_phase_comp);
    reg_bits_set(ADDR_XVR_0x33, 0,  7, tx_ty2);

    xvr_reg_bits_set(ADDR_XVR_0x0B, 4, 8, 0x04); //FIXME

    CALIB_DBG("calib_tx_phase_comp = 0x%02X\r\n", calib_tx_phase_comp);

    return tx_ty2;
}

void tx_output_power_calibration(void)
{
    #define TSSI_POUT_TH    (0x83)

    int32_t tssi_leakage;

    CALIB_DBG("%s()\r\n", __func__);

    xvr_reg_bit_set(ADDR_XVR_0x0C, 21, 1);          //2. enPcal=0, enable TSSI for output power calibration
    xvr_reg_bit_set(ADDR_XVR_0x0C, 20, 0);          //3. enIQcal=0, disable TSSI for IQ calibration path
    xvr_reg_bit_set(ADDR_XVR_0x0C, 19, 0);          //4. enDCcal=1, enable TSSI for DC calibration
    xvr_reg_bits_set(ADDR_XVR_0x0B, 4, 8, 0x00);    //5. PA output power -28dBm to be confirm

    reg_bits_set(ADDR_XVR_0x31,  8, 15, 0xE); //FIXME
    reg_bits_set(ADDR_XVR_0x31, 16, 23, 0xE); //FIXME

    xvr_reg_bits_set(ADDR_XVR_0x0B, 0, 3, 0);
    delay_us(100);
    tssi_leakage = sar_adc_read();
    xvr_reg_bits_set(ADDR_XVR_0x0B, 0, 3, 0xF);

    CALIB_DBG("tssi_leakage = 0x%X\n", tssi_leakage);

    calib_tx_output_power = 0;

    #if (CFG_TX_CALIB_ALGORITHM ==  TX_CALIB_ALGORITHM_ERGODIC) //遍历搜索

    uint32_t tx_output_power;
    uint32_t diff_min = 0x7FFFFFFF;

    for(tx_output_power = 0x10; tx_output_power <= 0x1F; tx_output_power++)
    {
        int32_t  tssi_power;
        uint32_t diff;

        xvr_reg_bits_set(ADDR_XVR_0x0B, 4, 8, tx_output_power);

        delay_us(100);
        tssi_power = sar_adc_read() - tssi_leakage;

        CALIB_DBG("calib_tx_output_power = 0x%X, tssi_power = 0x%0X\n", tx_output_power, tssi_power);

        diff = abs(tssi_power - TSSI_POUT_TH);

        if(diff < diff_min)
        {
            diff_min = diff;
            calib_tx_output_power = tx_output_power;
        }
    }

    #elif  (CFG_TX_CALIB_ALGORITHM == TX_CALIB_ALGORITHM_DICHOTOMY) //二分搜索

    #if 1  //二分搜索模式1

    int32_t i;

    calib_tx_output_power = 0x10;

    for(i = 3; i >= 0; i--)
    {
        int32_t tssi_power;

        calib_tx_output_power |= 1 << i;
        xvr_reg_bits_set(ADDR_XVR_0x0B, 4, 8, calib_tx_output_power);

        delay_us(100);
        tssi_power = sar_adc_read() - tssi_leakage;

        CALIB_DBG("calib_tx_output_power = 0x%X, tssi_power = 0x%0X\n", calib_tx_output_power, tssi_power);

        //if(tssi_power > TSSI_POUT_TH)
        if(tssi_power > TSSI_POUT_TH + 3)
        {
            calib_tx_output_power &= ~(1 << i);
        }
    }

    #else //二分搜索模式2

    uint32_t start = 0x10;
    uint32_t stop  = 0x1F;

    do
    {
        uint32_t tssi_start, tssi_stop;

        xvr_reg_bits_set(ADDR_XVR_0x0B, 4, 8, start);
        delay_us(100);
        tssi_start = sar_adc_read();

        xvr_reg_bits_set(ADDR_XVR_0x0B, 4, 8, stop);
        delay_us(100);
        tssi_stop = sar_adc_read();

        CALIB_DBG("start = 0x%X, stop = 0x%X, tssi_start = 0x%X, tssi_stop = 0x%X\n", start, stop, tssi_start, tssi_stop);

        tssi_start = abs(tssi_start - tssi_leakage - TSSI_POUT_TH);
        tssi_stop  = abs(tssi_stop  - tssi_leakage - TSSI_POUT_TH);

        if(start + 1 == stop) break;

        if(tssi_start < tssi_stop)
        {
            stop = (start + stop) / 2;
            calib_tx_output_power = start;
        }
        else
        {
            start = (start + stop) / 2;
            calib_tx_output_power = stop;
        }

    }while(start < stop);

    #endif

    #endif

    xvr_reg_bits_set(ADDR_XVR_0x0B, 4, 8, calib_tx_output_power);

    CALIB_DBG("calib_tx_output_power = 0x%X\r\n", calib_tx_output_power);
}

void rx_rssi_calibration_old(void)
{
    uint32_t rssi = xvr_reg_bits_get(ADDR_XVR_0x0A, 25, 27);
    uint32_t rssi_bak = rssi;

    uint32_t xvr_reg_0x07_bak = xvr_reg_get(ADDR_XVR_0x07);
    uint32_t xvr_reg_0x08_bak = xvr_reg_get(ADDR_XVR_0x08);
    uint32_t xvr_reg_0x25     = xvr_reg_get(ADDR_XVR_0x25);

    //1.	进入接收模式
    *(volatile uint32_t*)ADDR_XVR_0x25 = 0x2400;

    //2.	接收链路设置为固定增益（最大增益）
    xvr_reg_bit_set(ADDR_XVR_0x07, 17, 0);
    xvr_reg_bits_set(ADDR_XVR_0x07, 14, 16, 7);
    xvr_reg_bits_set(ADDR_XVR_0x08, 18, 20, 0);

    do
    {
        #define TEST_FREQ_COUNT     3
        const uint8_t freq[TEST_FREQ_COUNT] = { 0, 28, 80 };

        uint32_t i;
        uint32_t t1[TEST_FREQ_COUNT];
        uint32_t t2[TEST_FREQ_COUNT];

        for(i = 0; i < TEST_FREQ_COUNT; i++)
        {
            //设置频率
            reg_bits_set(ADDR_XVR_0x24, 0, 6, freq[i]);
            //toggle
            *(volatile uint32_t*)ADDR_XVR_0x25 = 0x0000;
            delay_us(1000);
            *(volatile uint32_t*)ADDR_XVR_0x25 = 0x2400;
            delay_us(1000);
            //读取RSSI
            t1[i] = reg_bits_get(ADDR_XVR_0x12, 13, 14);
            t2[i] = reg_bits_get(ADDR_XVR_0x12, 8,  12);

            CALIB_DBG("[RSSI-%d][%d]: %d, %d\n", rssi, i, t1[i], t2[i]);
        }

        if((t1[0] == 0 && t2[0] == 0) ||
           (t1[1] == 0 && t2[1] == 0) ||
           (t1[2] == 0 && t2[2] == 0))
        {
            break;
        }

        #if 1
        if(++rssi > 4)
        {
            rssi--;
            break;
        }
        #else
        rssi = 4;
        #endif

        xvr_reg_bits_set(ADDR_XVR_0x0A, 25, 27, rssi);

    }while(1);

    if(rssi != rssi_bak)
    {
        if(++rssi > 4) rssi = 4;
        xvr_reg_bits_set(ADDR_XVR_0x0A, 25, 27, rssi);
    }

    *(volatile uint32_t*)ADDR_XVR_0x25 = xvr_reg_0x25;
    *(volatile uint32_t*)ADDR_XVR_0x07 = xvr_reg_0x07_bak;
    *(volatile uint32_t*)ADDR_XVR_0x08 = xvr_reg_0x08_bak;

    CALIB_DBG("calib_rx_rssi = %d\r\n", rssi);
}

void rx_rssi_calibration(void)
{
    cpu_set_interrupts_enabled(0);

    uint32_t rssi = xvr_reg_bits_get(ADDR_XVR_0x0A, 25, 27);
    uint32_t rssi_bak = rssi;

    uint32_t xvr_reg_0x07_bak = xvr_reg_get(ADDR_XVR_0x07);
    uint32_t xvr_reg_0x08_bak = xvr_reg_get(ADDR_XVR_0x08);
    uint32_t xvr_reg_0x24     = xvr_reg_get(ADDR_XVR_0x24);
    uint32_t xvr_reg_0x25     = xvr_reg_get(ADDR_XVR_0x25);

    //1.	进入接收模式
    *(volatile uint32_t*)ADDR_XVR_0x25 = 0x2400;

    //2.	接收链路设置为固定增益（最大增益）
    xvr_reg_bit_set(ADDR_XVR_0x07, 17, 0);
    xvr_reg_bits_set(ADDR_XVR_0x07, 14, 16, 7);
    xvr_reg_bits_set(ADDR_XVR_0x08, 18, 20, 0);

    do
    {
        int cnt = 19;
        int err = 0;
        do
        {
            uint32_t rx_gain;

            #define RX_RSSI_CALIB_FREQ  (0x5A)

            //设置频率
            reg_bits_set(ADDR_XVR_0x24, 0, 6, RX_RSSI_CALIB_FREQ);
            reg_bits_set(ADDR_XVR_0x24, 16, 20, 0);
            //toggle
            *(volatile uint32_t*)ADDR_XVR_0x25 = 0x0000;
            delay_us(1000);
            *(volatile uint32_t*)ADDR_XVR_0x25 = 0x2400;
            //BK3000_GPIO_18_CONFIG = 2;
            delay_us(2000);
            //BK3000_GPIO_18_CONFIG = 0;
            //读取RSSI
            rx_gain = reg_bits_get(ADDR_XVR_0x18, 0, 9);

            //CALIB_DBG("[RSSI-%d][%d]: %d, %d\n", rssi, i, t1[i], t2[i]);
            //CALIB_DBG("[%s]: %d, %0x\r\n", __func__, rx_gain != 0x3FF, rx_gain);
            err += rx_gain != 0x3FF;

            //TODO

        }while(cnt--);

        if(err >= 5)
        {
            CALIB_DBG("[%s]: 1 (%d)\r\n", __func__, err);
        }
        else
        {
            CALIB_DBG("[%s]: 0 (%d)\r\n", __func__, err);
            break;
        }

        if(++rssi > 4)
        {
            rssi--;
            break;
        }

        xvr_reg_bits_set(ADDR_XVR_0x0A, 25, 27, rssi);

    }while(1);

    if(rssi != rssi_bak)
    {
        if(++rssi > 4) rssi = 4;
        xvr_reg_bits_set(ADDR_XVR_0x0A, 25, 27, rssi);
    }

    *(volatile uint32_t*)ADDR_XVR_0x24 = xvr_reg_0x24;
    *(volatile uint32_t*)ADDR_XVR_0x25 = xvr_reg_0x25;
    *(volatile uint32_t*)ADDR_XVR_0x07 = xvr_reg_0x07_bak;
    *(volatile uint32_t*)ADDR_XVR_0x08 = xvr_reg_0x08_bak;

    cpu_set_interrupts_enabled(1);

    CALIB_DBG("[%s]: calib_rx_rssi = %d\r\n", __func__, rssi);
}
