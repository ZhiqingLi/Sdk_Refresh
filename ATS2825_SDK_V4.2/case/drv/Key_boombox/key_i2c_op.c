#include "key_inner.h"

#define I2C_DEBUG_ERR(str, data, mode) libc_print(str, data, mode)

#define I2C_USE_HARDWARE            0
#define I2C_USE_SOFTWARE            1
#define I2C_FUNCTION_SELECT         I2C_USE_SOFTWARE

#if (I2C_FUNCTION_SELECT == I2C_USE_HARDWARE)
#define I2C_SCL_PIN_MAP_TO_A0
#define I2C_SDA_PIN_MAP_TO_A1

//#define I2C_SCL_PIN_MAP_TO_B11
//#define I2C_SDA_PIN_MAP_TO_B12

//#define I2C_SCL_PIN_MAP_TO_A13
//#define I2C_SDA_PIN_MAP_TO_A14
#endif

#if (I2C_FUNCTION_SELECT == I2C_USE_SOFTWARE)

#define TK_GPIO_SCL_BIT   (1 << 0) //GPIOA0
#define TK_GPIO_SDA_BIT   (1 << 1) //GPIOA1
#define TK_DELAY_TWI  30

const gpio_init_cfg_t __section__(".rodata.ram") tk_gpio_twi[2] = {
    { GPIOAINEN, GPIOAOUTEN, GPIOADAT, TK_GPIO_SCL_BIT },
    { GPIOAINEN, GPIOAOUTEN, GPIOADAT, TK_GPIO_SDA_BIT }
};

extern uint8 TWI_Trans_Bytes(uint8 *buf, uint8 address, uint8 length, gpio_init_cfg_t *gpio, uint8 *delay);
extern uint8 TWI_Recev_Bytes(uint8 *buf, uint8 address, uint8 length, gpio_init_cfg_t *gpio, uint8 *delay);

int32 __section__(".text.ram") key_inner_i2c_write(char address, char *buf, int size)
{
    uint8 tk_delay_twi = TK_DELAY_TWI;
    return TWI_Trans_Bytes(buf, address, size, (gpio_init_cfg_t *)tk_gpio_twi, &tk_delay_twi);
}

int32 __section__(".text.ram") key_inner_i2c_read(char address, char *buf, int size)
{
    uint8 tk_delay_twi = TK_DELAY_TWI;
    return TWI_Recev_Bytes(buf, address | 0x01, size, (gpio_init_cfg_t *)tk_gpio_twi, &tk_delay_twi);
}

int32 __section__(".text.ram") key_inner_i2c_read_write(uint8 address, uint8 *buf, uint32 size)
{
    uint8 tk_delay_twi = TK_DELAY_TWI;
    uint16 wlen = size & 0xFFFF;
    uint16 rlen = (size >> 16) & 0xFFFF;

    if(!TWI_Trans_Bytes(buf, address, wlen, (gpio_init_cfg_t *)tk_gpio_twi, &tk_delay_twi))
        return 0;

    return TWI_Recev_Bytes(buf, address | 0x01, rlen, (gpio_init_cfg_t *)tk_gpio_twi, &tk_delay_twi);
}


void __section__(".bank0") i2c_init(void)
{

}

#else
//I2C bus command
#define     I2C_MODE                    0xF0
#define     START                       I2C_MODE|0x4
#define     STOP                        I2C_MODE|0x8
#define     RESTA                       I2C_MODE|0xc
#define     REBUS                       I2C_MODE|0x2
#define     ACK                         I2C_MODE|0x0
#define     NACK                        I2C_MODE|0x1

//I2C stat
#define     TCB                         8
#define     STPD                        7
#define     STAD                        6
#define     RWST                        5
#define     LBST                        4
#define     IRQP                        3
#define     BBB                         2
#define     BEB                         1
#define     RACK                        0

#define bus_busy() (act_readl(I2C_STAT) & (1 << BBB))
#define is_bus_start() (act_readl(I2C_STAT) & (1 << STAD))
#define get_i2c_ack() (act_readl(I2C_STAT) & (1 << RACK))
#define is_transfer_complete() (act_readl(I2C_STAT) & (1 << TCB))
#define clear_transfer_flag() (act_writel((1 << TCB), I2C_STAT))
#define is_bus_stop() (act_readl(I2C_STAT) & (1 << STPD))

int32 key_inner_i2c_write(char address, char *buf, int size)
{
    int32 ret = 0;

    do {
        act_writel(address, I2C_DAT);                               //Slave addr
        act_writel(START | REBUS, I2C_CTL);                         //start bit and relese bus

        while(!is_transfer_complete());
        clear_transfer_flag();

        if(!(ret = get_i2c_ack())) {
            I2C_DEBUG_ERR("addr(w) no ack", 0, 0);
            break;
        }

        while((size--) > 0) {
            act_writel(*(buf++), I2C_DAT);
            act_writel(REBUS, I2C_CTL);

            while(!is_transfer_complete());
            clear_transfer_flag();

            if(!(ret = get_i2c_ack())) {
                I2C_DEBUG_ERR("no ack: ", size, 2);
                break;
            }
        }
    } while(0);

    act_writel(STOP | REBUS, I2C_CTL);          //generate stop bit
    while(!is_bus_stop());

    return ret;
}

int32 key_inner_i2c_read(char address, char *buf, int size)
{
    int32 ret = 0;

    do {
        act_writel((uint8)address | 0x01, I2C_DAT);                       //Slave read addr
        act_writel(START | REBUS, I2C_CTL);                         //start bit and relese bus

        while(!is_transfer_complete());
        clear_transfer_flag();

        if(!(ret = get_i2c_ack())) {
            I2C_DEBUG_ERR("addr(r) no ack", 0, 0);
            break;
        }

        while((size--) > 0) {
            if(size == 0)
                act_writel(REBUS | NACK, I2C_CTL);
            else
                act_writel(REBUS, I2C_CTL);

            while(!is_transfer_complete());
            clear_transfer_flag();

            *(buf++) = act_readl(I2C_DAT);
        }
    } while(0);

    act_writel(STOP | REBUS, I2C_CTL);          //generate stop bit
    while(!is_bus_stop());

    return ret;
}

int32 key_inner_i2c_read_write(uint8 address, uint8 *buf, uint32 size)
{
    int32 ret = 0;

    uint16 wlen = size & 0xFFFF;
    uint16 rlen = (size >> 16) & 0xFFFF;
    uint8 *rbuf = buf;

    do {
        act_writel(address, I2C_DAT);                               //Slave addr
        act_writel(START | REBUS, I2C_CTL);                         //start bit and relese bus

        while(!is_transfer_complete());
        clear_transfer_flag();

        if(!(ret = get_i2c_ack())) {
            I2C_DEBUG_ERR("addr(w) no ack", 0, 0);
            break;
        }

        while((wlen--) > 0) {
            act_writel(*(buf++), I2C_DAT);
            act_writel(REBUS, I2C_CTL);

            while(!is_transfer_complete());
            clear_transfer_flag();
            if(!(ret = get_i2c_ack())) {
                I2C_DEBUG_ERR("no ack: ", wlen, 2);
                break;
            }
        }

        act_writel(address | 0x01, I2C_DAT);                        //read Slave addr
        act_writel(RESTA | REBUS, I2C_CTL);

        while(!is_transfer_complete());
        clear_transfer_flag();

        if(!(ret = get_i2c_ack())) {
            I2C_DEBUG_ERR("addr(r) no ack", 0, 0);
            break;
        }

        while((rlen--) > 0) {
            if(rlen == 0)
                act_writel(REBUS | NACK, I2C_CTL);
            else
                act_writel(REBUS, I2C_CTL);

            while(!is_transfer_complete());
            clear_transfer_flag();

            *(rbuf++) = act_readl(I2C_DAT);
        }
    } while(0);

    act_writel(STOP | REBUS, I2C_CTL);          //generate stop bit
    while(!is_bus_stop());

    return ret;
}


void __section__(".bank0") i2c_init(void)
{
    act_writel((act_readl(CMU_SYSCLK) | 3), CMU_SYSCLK);
    //I2C Controller Reset normal
    act_writel((act_readl(MRCR) & (~(1 << 5))), MRCR);
    act_writel(act_readl(MRCR) | (1 << 5), MRCR);

    //使能I2C时钟
    act_writel(act_readl(CMU_DEVCLKEN) | (1 << 5), CMU_DEVCLKEN);

#if defined(I2C_SCL_PIN_MAP_TO_A0)
    //DISABLE A0的GPIO功能
    act_writel(act_readl(GPIOAOUTEN) & (~(1 << 0)), GPIOAOUTEN);
    act_writel(act_readl(GPIOAINEN) & (~(1 << 0)), GPIOAINEN);
    //配置 A0 为I2C功能
    act_writel(act_readl(MFP_CTL1) & (~(0x07 << 0)), MFP_CTL1);
    act_writel(act_readl(MFP_CTL1) | (0x4 << 0), MFP_CTL1);
    //配置 A0 由MFP_CTL1控制
    act_writel(act_readl(MFP_CTL3) & (~(1 << 24)), MFP_CTL3);
    //配置 A0 PAD LEVEL
    act_writel(act_readl(PADDRV0) & (~(0x07 << 0)), PADDRV0);
    act_writel(act_readl(PADDRV0) | (0x3 << 0), PADDRV0);
    //SMIT ENABLE
    //act_writel(act_readl(PAD_SMIT) | (0x1 << 8), PAD_SMIT);
#elif defined(I2C_SCL_PIN_MAP_TO_A13)
    //DISABLE A13的GPIO功能
    act_writel(act_readl(GPIOAOUTEN) & (~(1 << 13)), GPIOAOUTEN);
    act_writel(act_readl(GPIOAINEN) & (~(1 << 13)), GPIOAINEN);
    //配置 A13 为I2C功能
    act_writel((act_readl(MFP_CTL3) & (~(0x1 << 29))), MFP_CTL3);
    act_writel(act_readl(MFP_CTL3) | (0x1 << 29), MFP_CTL3);
    //配置 A13 PAD LEVEL
    act_writel((act_readl(PADDRV1) & (~(0x07 << 9))), PADDRV1);
    act_writel(act_readl(PADDRV1) | (0x3 << 9)), PADDRV1);
    //配置 A13 为Digital function
    act_writel((act_readl(AD_Select) & (~(0x3 << 26))), AD_Select);
#elif defined(I2C_SCL_PIN_MAP_TO_B11)
    //DISABLE B11的GPIO功能
    act_writel(act_readl(GPIOBOUTEN) & (~(1 << 11)), GPIOBOUTEN);
    act_writel(act_readl(GPIOBINEN) & (~(1 << 11)), GPIOBINEN);
    //配置 B11 为I2C功能
    act_writel((act_readl(MFP_CTL2) & (~(0x7 << 16))), MFP_CTL2);
    act_writel(act_readl(MFP_CTL2) | (0x1 << 16), MFP_CTL2);
    //配置 B11 PAD LEVEL
    act_writel((act_readl(PADDRV4) & (~(0x07 << 12))), PADDRV4);
    act_writel(act_readl(PADDRV4) | (0x3 << 12)), PADDRV4);
#endif

#if defined(I2C_SDA_PIN_MAP_TO_A1)
    //DISABLE A1的GPIO功能
    act_writel(act_readl(GPIOAOUTEN) & (~(1 << 1)), GPIOAOUTEN);
    act_writel(act_readl(GPIOAINEN) & (~(1 << 1)), GPIOAINEN);
    //配置 A1 为I2C功能
    act_writel(act_readl(MFP_CTL1) & (~(0x0f << 3)), MFP_CTL1);
    act_writel(act_readl(MFP_CTL1) | (0x4 << 3), MFP_CTL1);
    //配置 A1 由MFP_CTL1控制
    act_writel((act_readl(MFP_CTL3) & (~(1 << 25))), MFP_CTL3);
    //配置 A1 PAD LEVEL
    act_writel((act_readl(PADDRV0) & (~(0x07 << 3))), PADDRV0);
    act_writel(act_readl(PADDRV0) | (0x3 << 3), PADDRV0);
    //SMIT ENABLE
    //act_writel(act_readl(PAD_SMIT) | (0x1<<9), PAD_SMIT);
#elif defined(I2C_SDA_PIN_MAP_TO_A14)
    //DISABLE A14的GPIO功能
    act_writel(act_readl(GPIOAOUTEN) & (~(1 << 14)), GPIOAOUTEN);
    act_writel(act_readl(GPIOAINEN) & (~(1 << 14)), GPIOAINEN);
    //配置 A14 为I2C功能
    act_writel((act_readl(MFP_CTL3) & (~(0x1 << 30))), MFP_CTL3);
    act_writel(act_readl(MFP_CTL3) | (0x1 << 30), MFP_CTL3);
    //配置 A14 PAD LEVEL
    act_writel((act_readl(PADDRV1) & (~(0x07 << 12))), PADDRV1);
    act_writel(act_readl(PADDRV1) | (0x3 << 12)), PADDRV1);
    //配置 A14 为Digital function
    act_writel((act_readl(AD_Select) & (~(0x3 << 28))), AD_Select);
#elif defined(I2C_SDA_PIN_MAP_TO_B12)
    //DISABLE B12的GPIO功能
    act_writel(act_readl(GPIOBOUTEN) & (~(1 << 12)), GPIOBOUTEN);
    act_writel(act_readl(GPIOBINEN) & (~(1 << 12)), GPIOBINEN);
    //配置 B12 为I2C功能
    act_writel(act_readl(MFP_CTL2) | (0x1 << 19), MFP_CTL2);
#endif

    //使能10k上拉
    act_writel(act_readl(PADPUPD) | (1 << 1), PADPUPD);
    //I2C ENABLE /MASTER/CLKSEL 400K/IRQE
    act_writel(act_readl(I2C_CTL) | (0xf0), I2C_CTL);
}

#endif
