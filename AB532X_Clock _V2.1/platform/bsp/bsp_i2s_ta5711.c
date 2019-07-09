#include "include.h"
#include "bsp_i2c.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if I2S_DEVICE == I2S_DEV_TAS5711 && I2S_EN
/* device registers */
#define TAS5711_CLK_CTRL_REG	0x00
#define TAS5711_DEV_ID_REG		0x01
#define TAS5711_ERR_STATUS_REG	0x02
#define TAS5711_SYS_CTRL_1_REG	0x03
#define TAS5711_SDI_REG			0x04
#define TAS5711_SYS_CTRL_2_REG	0x05
#define TAS5711_SOFT_MUTE_REG	0x06
#define TAS5711_MVOL_REG		0x07
#define TAS5711_OSC_TRIM_REG	0x1b

#if !I2C_EN
#error "TAS5711: please enable i2c!\n"
#endif

//TAS5711CLK设置
#if I2S_MCLK_SEL == 0
#define TAS5711_CLK_SEL         0x60    //64fs
#elif I2S_MCLK_SEL == 1
#define TAS5711_CLK_SEL         0x64    //128fs
#elif I2S_MCLK_SEL == 2
#define TAS5711_CLK_SEL         0x6c    //256fs
#endif

//TAS5711数据对齐设置
#if I2S_DATA_MODE
#define TAS5711_DAT_SEL         0x03
#else
#define TAS5711_DAT_SEL         0x06    //左对齐模式
#endif
typedef struct {
    u8 cmd;
    u8 len;
    u8 parm;
}tas5711_cmd_t;

static tas5711_cmd_t tas5711_cmd_init[] = {
    {TAS5711_CLK_CTRL_REG,      1,      TAS5711_CLK_SEL},
    {TAS5711_SDI_REG,           1,      TAS5711_DAT_SEL},
    {TAS5711_SYS_CTRL_2_REG,    1,      0x00},
    {TAS5711_SOFT_MUTE_REG,     1,      0x00},
    {TAS5711_MVOL_REG,          1,      0x50},
};

AT(.text.bsp.i2s)
static bool bsp_tas5711_detect(void)
{
    bool ret;
    bsp_i2c_start();
    bsp_i2c_tx_byte(0x36);
    ret = bsp_i2c_rx_ack();
    bsp_i2c_stop();
    return ret;
}

AT(.text.bsp.i2s)
static void bsp_tas5711_sfr_write(u8 addr, u8 dat)
{
    bsp_i2c_start();
    bsp_i2c_tx_byte(0x36);
    bsp_i2c_rx_ack();
    bsp_i2c_tx_byte(addr);
    bsp_i2c_rx_ack();
    bsp_i2c_tx_byte(dat);
    bsp_i2c_rx_ack();
    bsp_i2c_stop();
    delay_5ms(2);
}

AT(.text.bsp.i2s)
static u8 bsp_tas5711_sfr_read(u8 addr)
{
    u8 dat;
    bsp_i2c_start();
    bsp_i2c_tx_byte(0x36);
    bsp_i2c_rx_ack();
    bsp_i2c_tx_byte(addr);
    bsp_i2c_rx_ack();

    bsp_i2c_start();
    bsp_i2c_tx_byte(0x37);
    bsp_i2c_rx_ack();
    dat = bsp_i2c_rx_byte();
    bsp_i2c_tx_nack();
    bsp_i2c_stop();
    delay_5ms(2);
    return dat;
}

AT(.text.bsp.i2s)
void bsp_tas5711_init(void)
{
    u8 i;
    bsp_i2c_init();

    //TAS5711 reset
    GPIOFDE |= BIT(5);
    GPIOFDIR &= ~BIT(5);
    GPIOFCLR = BIT(5);
    delay_5ms(1);
    GPIOFSET = BIT(5);
    delay_5ms(4);

    if (!bsp_tas5711_detect()) {
        TRACE("TAS5711 isn't online\n");
        return;
    }
    bsp_tas5711_sfr_write(TAS5711_OSC_TRIM_REG, 0x00);      //tirm Oscillator
    delay_5ms(40);

    for (i=0; i<sizeof(tas5711_cmd_init)/sizeof(tas5711_cmd_t); i++) {
        bsp_tas5711_sfr_write(tas5711_cmd_init[i].cmd, tas5711_cmd_init[i].parm);
        delay_us(10);
    }

    TRACE("[0]:0x%02x\n",bsp_tas5711_sfr_read(TAS5711_CLK_CTRL_REG));
    TRACE("[1]:0x%02x\n",bsp_tas5711_sfr_read(TAS5711_DEV_ID_REG));
    TRACE("[2]:0x%02x\n",bsp_tas5711_sfr_read(TAS5711_ERR_STATUS_REG));
    TRACE("[4]:0x%02x\n",bsp_tas5711_sfr_read(TAS5711_SDI_REG));
    TRACE("[5]:0x%02x\n",bsp_tas5711_sfr_read(TAS5711_SYS_CTRL_2_REG));
    TRACE("[6]:0x%02x\n",bsp_tas5711_sfr_read(TAS5711_SOFT_MUTE_REG));
    TRACE("[7]:0x%02x\n",bsp_tas5711_sfr_read(TAS5711_MVOL_REG));
    TRACE("[1b]:0x%02x\n",bsp_tas5711_sfr_read(TAS5711_OSC_TRIM_REG));
}
#endif
