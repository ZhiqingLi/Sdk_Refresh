#include "ccd_inner.h"

uint8 g_i2s_pa_status = 0; //0表示POWER DOWN,1表示POWER ON
uint8 g_i2s_pa_delay_twi = 0; //delay time
uint8 g_i2s_pa_drc_enable = 0; //DRC使能

// twi 所用模拟GPIO 信息结构
const gpio_init_cfg_t g_i2s_pa_gpio_twi[2] =
{
    { GPIOAINEN, GPIOAOUTEN, GPIOADAT, GPIO_SCL_BIT },
    { GPIOAINEN, GPIOAOUTEN, GPIOADAT, GPIO_SDA_BIT }
};

int pa_set_reg(uint8 reg_addr, uint8 *reg_data, uint8 set_len)
{
    uint8 set_buf[12];
    uint32 flags;
    int ret = 1;

    //关中断
    flags = sys_local_irq_save();

    //set register and value
    set_buf[0] = reg_addr;
    libc_memcpy(set_buf + 1, reg_data, set_len);

    //send out
#ifdef __PACFG339_H__ 
    ret = TWI_Trans_Bytes(set_buf, DEVICE_ADDR1, set_len + 1, g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi);
    ret = TWI_Trans_Bytes(set_buf, DEVICE_ADDR2, set_len + 1, g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi);    
#else
    ret = TWI_Trans_Bytes(set_buf, TI_TAS5731_20_I2C_ADDR, set_len + 1, g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi);
#endif
    TWI_Exit(g_i2s_pa_gpio_twi);

    //开中断
    sys_local_irq_restore(flags);

    return ret;
}

int pa_get_reg(uint8 reg_addr, uint8 *reg_data, uint8 get_len)
{
    uint32 flags;
    uint8 *p_get = reg_data;
    int ret = 1;

    //关中断
    flags = sys_local_irq_save();

    //TWI init
    TWI_Init(g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi);

    TWI_Start(g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi);

    //send TWI write addr
#ifdef __PACFG339_H__ 
    TWI_WriteByte(DEVICE_ADDR1, g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi);
//    TWI_WriteByte(DEVICE_ADDR2, g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi);
#else        
    TWI_WriteByte(TI_TAS5731_20_I2C_ADDR, g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi);
#endif
    if (TWI_GetAck(g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi) != 0)
    {
        //读寄存器地址,地址在高位，需左移1位，低位为1表示读，从0x0a寄存器读数据
        TWI_WriteByte(reg_addr, g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi);

        if (TWI_GetAck(g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi) != 0)
        {
            TWI_Start(g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi);
            //send TWI read addr
#ifdef __PACFG339_H__ 
            TWI_WriteByte((DEVICE_ADDR1 | 0x01), g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi);
//            TWI_WriteByte((DEVICE_ADDR2 | 0x01), g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi);
#else
            TWI_WriteByte((TI_TAS5731_20_I2C_ADDR | 0x01), g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi);
#endif
            if (TWI_GetAck(g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi) != 0)
            {
                while (get_len != 0)
                {
                    //receive data
                    *p_get = TWI_ReadByte(g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi);

                    if (get_len == 1)
                    {
                        //最后一字节发送NACK
                        TWI_SendAck(1, g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi);
                    }
                    else
                    {
                        //前面字节发送ACK
                        TWI_SendAck(0, g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi);
                    }
                    p_get++;
                    get_len--;

                }
            }
            else
            {
                ret = 0;
            }
        }
        else
        {
            ret = 0;
        }
    }
    else
    {
        ret = 0;
    }

    TWI_Stop(g_i2s_pa_gpio_twi, &g_i2s_pa_delay_twi);

    //gpio mode switchh
    TWI_Exit(g_i2s_pa_gpio_twi);
    //开中断
    sys_local_irq_restore(flags);

    return ret;
}

uint16 pa_op_get_volume(void)
{
    //get val from register
#ifdef __PACFG339_H__ 
    uint8 ret_vol;

    uint8 reg_data;

    if((pa_get_reg(MAIN_VOL,&reg_data,1)) == 0)
    {
        return 0;
    }
    ret_vol = reg_data;
#else
    uint32 read_val;

    uint16 ret_vol;

    uint8 reg_data[2];
    if (pa_get_reg(TI_TAS5707_MASTER_VOL, reg_data, 2) == 0)
    {
        return 0;
    }

    read_val = ((uint32) reg_data[0] << 8) | reg_data[1];
    
    ret_vol = read_val;
#endif

    return ret_vol;
}

void pa_op_set_volume(uint16 vol)
{
#ifdef __PACFG339_H__ 
    uint32 reg_vol = vol/4;
    if(reg_vol > 0xff)
    {
        reg_vol = 0xff;
    }
//    pa_set_reg(MAIN_VOL,0xa4,1);   
    if(pa_set_reg(MAIN_VOL,&reg_vol,1) == 0)
    {
        return;
    }
//    pa_set_reg(CHN1_VOL,(reg_vol),1);
//    pa_set_reg(CHN2_VOL,(reg_vol),1);
    //libc_print("set volume:",reg_vol,2);
#else
    uint32 reg_val;
    uint8 reg_data[2];
    reg_val = vol;
    reg_data[0] = (uint8) (reg_val >> 8);
    reg_data[1] = (uint8) (reg_val&0xff);
    //set val to register
    pa_set_reg(TI_TAS5707_MASTER_VOL, reg_data, 2);
#endif    
}

void pa_op_set_clock(uint8 rate)
{
#ifdef __PACFG339_H__ 
    uint32 set_rate = rate;
    uint32 val;
    switch(set_rate)
    {
        case 44: 
        rate = 0x63;
        Set_Device_sfr(CONFIG_A, &rate, 1, (DEVICE_ADDR1|W_MODE));
        val = 0x81;
        //Set_Device_sfr(CONFIG_B, &val, 1, (DEVICE_ADDR1|W_MODE));
        libc_print("set pa clock",set_rate,2);
        break;
        
        case 48:
        rate = 0x63;
        Set_Device_sfr(CONFIG_A, &rate, 1, (DEVICE_ADDR1|W_MODE));
        val = 0x81;
        //Set_Device_sfr(CONFIG_B, &val, 1, (DEVICE_ADDR1|W_MODE));
        libc_print("set pa clock",set_rate,2);
        break;
        
        case 96: 
        rate = 0x69;       
        Set_Device_sfr(CONFIG_A, &rate, 1, (DEVICE_ADDR1|W_MODE));
        val = 0x81;
        //Set_Device_sfr(CONFIG_B, &val, 1, (DEVICE_ADDR1|W_MODE));
        libc_print("set pa clock",set_rate,2);
        break;
        
        default:
        pa_get_reg(CONFIG_A,&rate,1);
        libc_print("get_pa_clock",rate,1);
        break;
    }
#endif
}

static void enable_drc_function(void)
{
#ifdef __PACFG339_H__

#else
    uint8 data_buf[12];

    //DRC setting
    data_buf[0] = 0x00;
    data_buf[1] = 0x00;
    data_buf[2] = 0x00;
    data_buf[3] = 0x23;
    pa_set_reg(0x46, data_buf, 4);

    data_buf[0] = 0x06;
    data_buf[1] = 0x00;
    data_buf[2] = 0x00;
    data_buf[3] = 0x00;
    data_buf[4] = 0x05;
    data_buf[5] = 0xff;
    data_buf[6] = 0xff;
    data_buf[7] = 0xff;
    pa_set_reg(0x40, data_buf, 8);

    data_buf[0] = 0x00;
    data_buf[1] = 0x01;
    data_buf[2] = 0x00;
    data_buf[3] = 0x00;
    data_buf[4] = 0x00;
    data_buf[5] = 0x7f;
    data_buf[6] = 0x00;
    data_buf[7] = 0x00;
    pa_set_reg(0x3b, data_buf, 8);

    data_buf[0] = 0x00;
    data_buf[1] = 0x00;
    data_buf[2] = 0x00;
    data_buf[3] = 0xe0;
    data_buf[4] = 0xff;
    data_buf[5] = 0xff;
    data_buf[6] = 0xff;
    data_buf[7] = 0xf0;
    pa_set_reg(0x3C, data_buf, 8);

    data_buf[0] = 0x06;
    data_buf[1] = 0xbd;
    data_buf[2] = 0x00;
    data_buf[3] = 0x00;
    data_buf[4] = 0x06;
    data_buf[5] = 0xbc;
    data_buf[6] = 0xff;
    data_buf[7] = 0xff;
    pa_set_reg(0x43, data_buf, 8);

    data_buf[0] = 0x00;
    data_buf[1] = 0x00;
    data_buf[2] = 0x01;
    data_buf[3] = 0x00;
    data_buf[4] = 0x00;
    data_buf[5] = 0x7f;
    data_buf[6] = 0xff;
    data_buf[7] = 0x00;
    pa_set_reg(0x3E, data_buf, 8);

    data_buf[0] = 0x00;
    data_buf[1] = 0x00;
    data_buf[2] = 0x00;
    data_buf[3] = 0xe0;
    data_buf[4] = 0xff;
    data_buf[5] = 0xf8;
    data_buf[6] = 0x00;
    data_buf[7] = 0x00;
    pa_set_reg(0x3F, data_buf, 8);
#endif
}

void pa_op_poweron(void)
{
#ifdef __PACFG339_H__
    libc_print("pa init",0,0);
    PAinit();
    pa_op_set_volume(0x3ff);
#else
    uint8 data_buf[4];

    data_buf[0] = 0x0;
    pa_set_reg(TI_TAS5707_OSCILLATOR_TRIM, data_buf, 1);

    sys_mdelay(50);

    data_buf[0] = 0x0;
    pa_set_reg(TI_TAS5707_POWER_CONTROL, data_buf, 1);

    if (g_i2s_pa_drc_enable == 1)
    {
        enable_drc_function();
    }

    g_i2s_pa_status = 1;

    pa_op_set_volume(0x3ff);
#endif    
}

void pa_op_powerdown(void)
{
#ifdef __PACFG339_H__

#else
    uint8 data_buf[2];

    //register value
    data_buf[0] = 0x40; //shut down
    pa_set_reg(TI_TAS5707_POWER_CONTROL, data_buf, 1);

    g_i2s_pa_status = 0;
#endif
}

void Set_Device_sfr(uint8 reg,uint8 *data,uint8 set_len,uint8 addr)
{
    pa_set_reg(reg,data,set_len);   
}



