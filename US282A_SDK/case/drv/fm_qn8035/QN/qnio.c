#include "fm_drv.h"
#include "fm_qn8035.h"

void TWI_SDA_Output_Dis(gpio_init_cfg_t* gpio);
void TWI_SDA_Input_Dis(gpio_init_cfg_t* gpio);
void TWI_SCL_Output_Dis(gpio_init_cfg_t* gpio);
void TWI_SCL_Input_Dis(gpio_init_cfg_t* gpio);

void SwitchGPIO(void)
{
    TWI_SDA_Output_Dis(twi_gpio);
    TWI_SDA_Input_Dis(twi_gpio);
    TWI_SCL_Output_Dis(twi_gpio);
    TWI_SCL_Input_Dis(twi_gpio);
}

//返回值：失败->-1; 成功->0
UINT8 QND_WriteReg(UINT8 Regis_Addr, UINT8 Data)
{
    uint8 i;
    int result = 0;
    uint8 writebuffer[2];

    uint32 flags;
    //因为A31被用做I2C_SCL, 但同时有可以被用做SPI_MO/SPI_MI
    //如果I2C初始化完成了，被打断，去读写SPI, 就有问题

    //关中断
    flags = sys_local_irq_save();

    writebuffer[0] = Regis_Addr;
    //register data
    writebuffer[1] = Data;

    for (i = 0; i < 10; i++)
    {
        //send 3 byte to fm module
        if (TWI_Trans_Bytes(writebuffer, TWI_WRITE_ADDR,
                sizeof(writebuffer), twi_gpio, &delay_twi) != 0)
        {
            break;
        }
    }

    SwitchGPIO();
    //开中断
    sys_local_irq_restore(flags);
    if (i == 10)
    {
        result = -1;
    }
    return result;
}

UINT8 QND_ReadReg(UINT8 Regis_Addr)
{
    uint8 Data = 0;
    uint32 flags;
    uint8 init_cnt = 0;
    uint8 ret = 0;

    //因为A31被用做I2C_SCL, 但同时有可以被用做SPI_MO/SPI_MI
    //如果I2C初始化完成了，被打断，去读写SPI, 就有问题

    //关中断
    flags = sys_local_irq_save();
    //TWI init
    for (init_cnt = 0;init_cnt<10; init_cnt++)
    {
        TWI_Init(twi_gpio, &delay_twi);

        TWI_Start(twi_gpio, &delay_twi);

        //send TWI write addr
        TWI_WriteByte(TWI_WRITE_ADDR, twi_gpio, &delay_twi);

        ret = TWI_GetAck(twi_gpio, &delay_twi);
        if (ret != 0)
        {
            //读寄存器地址,地址在高位，需左移1位，低位为1表示读，从0x0a寄存器读数据
            TWI_WriteByte(Regis_Addr, twi_gpio, &delay_twi);

            ret = TWI_GetAck(twi_gpio, &delay_twi);
            if (ret != 0)
            {
                TWI_Start(twi_gpio, &delay_twi);
                //send TWI read addr
                TWI_WriteByte(TWI_WRITE_ADDR | 1, twi_gpio, &delay_twi);

                ret = TWI_GetAck(twi_gpio, &delay_twi);
                if (ret != 0)
                {
                    //receive data
                    Data = TWI_ReadByte(twi_gpio, &delay_twi);
                    //最后一字节发送NACK
                    TWI_SendAck(1, twi_gpio, &delay_twi);
                }
            }
            TWI_Stop(twi_gpio, &delay_twi);
        }

        if (ret != 0)
        {
            break;
        }
    }
    SwitchGPIO();
    //开中断
    sys_local_irq_restore(flags);
    return Data;
}

/*************************************************************************************************************
 end file
 ************************************************************************************************************/
