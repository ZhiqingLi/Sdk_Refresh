//亲，请注意：
//这部分代码已经烧录到主控芯片中(不是spiflash中)，是不能改动的!!!
//把代码发布在这里，只是为了方便于理解代码。
#include "TWI.h"
#include "TWI_inner.h"

void TWI_Init(gpio_init_cfg_t* gpio, uint8* delay);
void TWI_Exit(gpio_init_cfg_t* gpio);
void TWI_Delay(uint8* delay);
void TWI_SDA_high_hold(gpio_init_cfg_t* gpio, uint8* delay);
void TWI_SDA_low_hold(gpio_init_cfg_t* gpio, uint8* delay);
void TWI_SCL_high_hold(gpio_init_cfg_t* gpio, uint8* delay);
void TWI_SCL_low_hold(gpio_init_cfg_t* gpio, uint8* delay);

void TWI_SDA_Output_En(gpio_init_cfg_t* gpio);
void TWI_SDA_Output_Dis(gpio_init_cfg_t* gpio);
void TWI_SDA_Input_En(gpio_init_cfg_t* gpio);
void TWI_SDA_Input_Dis(gpio_init_cfg_t* gpio);
void TWI_SCL_Output_En(gpio_init_cfg_t* gpio);
void TWI_SCL_Output_Dis(gpio_init_cfg_t* gpio);
void TWI_SCL_Input_En(gpio_init_cfg_t* gpio);
void TWI_SCL_Input_Dis(gpio_init_cfg_t* gpio);
uint8 TWI_Data_Input(gpio_init_cfg_t* gpio);

void TWI_Start(gpio_init_cfg_t* gpio, uint8* delay);
void TWI_Stop(gpio_init_cfg_t* gpio, uint8* delay);

uint8 TWI_Trans_Bytes(uint8 *buf, uint8 address, uint8 length, gpio_init_cfg_t* gpio, uint8* delay);
uint8 TWI_Recev_Bytes(uint8 *buf, uint8 address, uint8 length, gpio_init_cfg_t* gpio, uint8* delay);
void TWI_WriteByte(uint8 dat, gpio_init_cfg_t* gpio, uint8* delay);
uint8 TWI_ReadByte(gpio_init_cfg_t* gpio, uint8* delay);
void TWI_SendAck(uint8 ack, gpio_init_cfg_t* gpio, uint8* delay);
uint8 TWI_GetAck(gpio_init_cfg_t* gpio, uint8* delay);

//TWI初始化
//gpio: 模拟TWI操作所用的IO
//delay: 操作延时
void TWI_Init(gpio_init_cfg_t* gpio, uint8* delay)
{
    TWI_SDA_high_hold(gpio, delay);
    TWI_SCL_high_hold(gpio, delay);
    TWI_SDA_Output_En(gpio);
    TWI_SCL_Output_En(gpio);
    TWI_SCL_Input_En(gpio);
    TWI_SDA_Input_En(gpio);
}

//TWI退出
void TWI_Exit(gpio_init_cfg_t* gpio)
{
    TWI_SDA_Output_Dis(gpio);
    TWI_SDA_Input_Dis(gpio);
    TWI_SCL_Output_Dis(gpio);
    TWI_SCL_Input_Dis(gpio);
}

//延时函数
void TWI_Delay(uint8* delay)
{
    uint8 j, value;

    value = *delay;
    for (j = 0; j < value; j++)
    {
        //延时函数
        //;
    }
}

//SDA输出高
void TWI_SDA_high_hold(gpio_init_cfg_t* gpio, uint8* delay)
{
    act_writel((act_readl((gpio + 1)->reg_data) | ((gpio + 1)->reg_bit)), (gpio + 1)->reg_data);
    TWI_Delay(delay);
}

//SDA输出低
void TWI_SDA_low_hold(gpio_init_cfg_t* gpio, uint8* delay)
{
    act_writel((act_readl((gpio + 1)->reg_data) & (~((gpio + 1)->reg_bit))), (gpio + 1)->reg_data);
    TWI_Delay(delay);
}

//SCL输出高
void TWI_SCL_high_hold(gpio_init_cfg_t* gpio, uint8* delay)
{
    act_writel((act_readl(gpio->reg_data) | (gpio->reg_bit)), gpio->reg_data);
    TWI_Delay(delay);
}

//SCL输出低
void TWI_SCL_low_hold(gpio_init_cfg_t* gpio, uint8* delay)
{
    act_writel((act_readl(gpio->reg_data) & (~(gpio->reg_bit))), gpio->reg_data);
    TWI_Delay(delay);
}

//SDA输出使能
void TWI_SDA_Output_En(gpio_init_cfg_t* gpio)
{
    act_writel((act_readl((gpio + 1)->reg_out) | ((gpio + 1)->reg_bit)), (gpio + 1)->reg_out);
}

//SDA输出不使能
void TWI_SDA_Output_Dis(gpio_init_cfg_t* gpio)
{
    act_writel((act_readl((gpio + 1)->reg_out) & (~((gpio + 1)->reg_bit))), (gpio + 1)->reg_out);
}

//SDA输入使能
void TWI_SDA_Input_En(gpio_init_cfg_t* gpio)
{
    act_writel((act_readl((gpio + 1)->reg_in) | ((gpio + 1)->reg_bit)), (gpio + 1)->reg_in);
}

//SDA输入不使能
void TWI_SDA_Input_Dis(gpio_init_cfg_t* gpio)
{
    act_writel((act_readl((gpio + 1)->reg_in) & (~((gpio + 1)->reg_bit))), (gpio + 1)->reg_in);
}

//SDA输入使能
void TWI_SCL_Output_En(gpio_init_cfg_t* gpio)
{
    act_writel((act_readl(gpio->reg_out) | (gpio->reg_bit)), gpio->reg_out);
}

//SCL输出不使能
void TWI_SCL_Output_Dis(gpio_init_cfg_t* gpio)
{
    act_writel((act_readl(gpio->reg_out) & (~(gpio->reg_bit))), gpio->reg_out);
}

//SCL输入使能
void TWI_SCL_Input_En(gpio_init_cfg_t* gpio)
{
    act_writel((act_readl(gpio->reg_in) | (gpio->reg_bit)), gpio->reg_in);
}

//SCL输入不使能
void TWI_SCL_Input_Dis(gpio_init_cfg_t* gpio)
{
    act_writel((act_readl(gpio->reg_in) & (~(gpio->reg_bit))), gpio->reg_in);
}

//输入的32 bit 数中哪一个bit 为1
//返回值：所找到的为1的bit位(0~31)(从低为到高位查找)
uint8 TWI_Data_Bit(uint32 value)
{
    uint8 loop;
    uint32 temp;
    for (loop = 0; loop < 32; loop++)
    {
        temp = (value >> loop) & 0x00000001;
        if (temp != 0)
        {
            break;
        }
    }
    return loop;
}

//返回：SDA上的电平
uint8 TWI_Data_Input(gpio_init_cfg_t* gpio)
{
    uint32 value;
    uint8 ret;
    value = act_readl((gpio + 1)->reg_data) & ((gpio + 1)->reg_bit);
    ret = TWI_Data_Bit((gpio + 1)->reg_bit);
    value = (value >> ret) & 0x00000001;
    ret = (uint8) value;
    return ret;
}

//模拟TWI   Start  流程
/*
 ____
 SDA:|_______

 ______
 SCL:  |_____
 */
void TWI_Start(gpio_init_cfg_t* gpio, uint8* delay)
{
    TWI_SDA_high_hold(gpio, delay);
    TWI_SCL_high_hold(gpio, delay);
    TWI_SDA_low_hold(gpio, delay);
    TWI_SCL_low_hold(gpio, delay); /*钳住TWI总线，SCL为低电平，准备发送或接收数据 */
}

//模拟TWI  Stop  流程
/*
 SDA      ________
 ________|
 ___________
 SCL:       |______
 */
void TWI_Stop(gpio_init_cfg_t* gpio, uint8* delay)
{
    TWI_SDA_low_hold(gpio, delay);
    TWI_SCL_high_hold(gpio, delay);
    TWI_SDA_high_hold(gpio, delay);
}

//将存放到*buf地址里长度为length的数据发送出去
//buf: 数据存放地址
//address: 设备地址
//length: 数据长度, 以byte为单位
//返回值:   FALSE: 发送数据接收到nack
//          TRUE: 发送数据接收到ack
uint8 TWI_Trans_Bytes(uint8 *buf, uint8 address, uint8 length, gpio_init_cfg_t* gpio, uint8* delay)
{
    uint8 ret = 0;

    TWI_Init(gpio, delay);

    TWI_Start(gpio, delay);

    /*发送slave 地址*/
    TWI_WriteByte(address, gpio, delay);
    if (TWI_GetAck(gpio, delay) != 0)
    {
        //发送数据
        do
        {
            TWI_WriteByte(*buf, gpio, delay);
            buf++;
            if (!TWI_GetAck(gpio, delay))
            {
                break;
            }
            --length;
        }
        while (length != 0);
        if (length == 0)
        {
            ret = 1;
        }
        else
        {
            ret = 0;
        }
    }

    TWI_Stop(gpio, delay);
    return ret;
}

//读
//buf: 读到的数据放的地方(在收到最后一个字节后，主控会发送NACK)
//address: 设备地址(一般这个参数的BIT0为1)
//length: 读多少个字节
//返回值: FALSE: slave不响应， TRUE: 收到数据
uint8 TWI_Recev_Bytes(uint8 *buf, uint8 address, uint8 length, gpio_init_cfg_t* gpio, uint8* delay)
{
    uint8 ret = 0;
    uint8 *p;

    p = buf;

    TWI_Init(gpio, delay);

    TWI_Start(gpio, delay);

    /*发送slave 地址*/
    TWI_WriteByte(address, gpio, delay);
    if (!TWI_GetAck(gpio, delay))
    {
        //ret = 0;
        goto recev_ret;
    }

    while (length != 0)
    {
        if (length == 1) //the last byte, sent nack
        {
            *p = TWI_ReadByte(gpio, delay);
            TWI_SendAck(1, gpio, delay);
        }
        else
        {
            *p = TWI_ReadByte(gpio, delay);
            TWI_SendAck(0, gpio, delay);
        }
        p++;
        length--;
    }
    ret = 1;
    recev_ret:
    TWI_Stop(gpio, delay);
    return ret;
}

//写1个字节的数据到slave
//本函数执行前SCL必须为低
//本函数结束时，SCL为低
void TWI_WriteByte(uint8 dat, gpio_init_cfg_t* gpio, uint8* delay)
{
    uint8 i;
    for (i = 0; i < 8; i++)
    {
        if (((dat << i) & 0x80) != 0) //对于一个字节，从高bit 到低bit 依次发送
        {
            TWI_SDA_high_hold(gpio, delay);
        }
        else
        {
            TWI_SDA_low_hold(gpio, delay);
        }
        TWI_SCL_high_hold(gpio, delay);
        TWI_SCL_low_hold(gpio, delay);
    }
}

//读取1个字节的数据,并回ack或者nack给slave
//本函数执行前SCL必须为低
//本函数结束时，SCL为低
uint8 TWI_ReadByte(gpio_init_cfg_t* gpio, uint8* delay)
{
    uint8 t = 8, dat = 0;
    uint8 TWI_DATA;

    //TWI_SDA_high_hold(gpio, delay);

    //TWI_Delay(delay);
    TWI_SDA_Output_Dis(gpio);
    //TWI_SDA_Input_En(gpio);
    TWI_Delay(delay);

    do
    {
        TWI_SCL_high_hold(gpio, delay);

        dat <<= 1;
        TWI_DATA = TWI_Data_Input(gpio);
        if (TWI_DATA != 0)
        {
            dat |= 0x01;
        } //从TWI 数据线上采样一个字节的数据，是从高bit 到低bit 采样

        TWI_SCL_low_hold(gpio, delay);
        --t;
    } while (t != 0);

    //TWI_Delay(delay);
    //TWI_SDA_Input_Dis(gpio);
    TWI_SDA_Output_En(gpio);
    TWI_Delay(delay);
    return dat;
}

//发送ack/nack
//ack: 1 noack; 0 ack
//本函数执行前SCL必须为低
//本函数结束时，SCL为低
void TWI_SendAck(uint8 ack, gpio_init_cfg_t* gpio, uint8* delay)
{
    if (ack != 0)
    {
        TWI_SDA_high_hold(gpio, delay); //nack
    }
    else
    {
        TWI_SDA_low_hold(gpio, delay); //ack
    }
    TWI_SCL_high_hold(gpio, delay);
    TWI_SCL_low_hold(gpio, delay);
}

//获取从设备端发送过来的响应信号Ack  or  NAck
//返回值：TRUE->得到了ack; FALSE->得到的是nack
//本函数执行前SCL必须为低
//本函数结束时，SCL为低
uint8 TWI_GetAck(gpio_init_cfg_t* gpio, uint8* delay)
{
    uint8 ack;
    uint8 TWI_DATA;

    // TWI_SDA_high_hold(gpio, delay);

    //TWI_Delay(delay);
    TWI_SDA_Output_Dis(gpio);
    //TWI_SDA_Input_En(gpio);
    TWI_Delay(delay);
    TWI_SCL_high_hold(gpio, delay);
    TWI_DATA = TWI_Data_Input(gpio); //只采样一个bit
    //TWI_SDA_Input_Dis(gpio);
    if (TWI_DATA != 0)
    {
        ack = 0; //nack
        //TWI_SDA_high_hold(gpio, delay);
    }
    else
    {
        ack = 1; //ack
        //TWI_SDA_low_hold(gpio, delay);
    }
    TWI_SDA_Output_En(gpio);
    TWI_Delay(delay);
    TWI_SCL_low_hold(gpio, delay);
    TWI_SDA_low_hold(gpio, delay);
    //TWI_SDA_Input_Dis(gpio);

    return ack;
}

