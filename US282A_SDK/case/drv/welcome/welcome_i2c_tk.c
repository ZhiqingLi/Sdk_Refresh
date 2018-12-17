/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                             All Rights Reserved.
 *      Description:i2c触摸按键的处理
 *      <author>       <time>
 *       Wekan       2015-3-26pm
 *******************************************************************************/

#include "case_independent.h"

#if (SUPPORT_TK_DRIVER == TK_DRIVER_OUTER)

#include ".\..\Key_boombox\key_inner.h"

typedef enum
{
    I2C_TK_FSM_IDLE,
    I2C_TK_FSM_WRITE_ADDR,
    I2C_TK_FSM_GET_ACK,
    I2C_TK_FSM_READ_OVER,
    I2C_TK_FSM_SUCC,
    I2C_TK_FSM_FAIL,
} i2c_tk_fsm_e;

#define TK_ADC_KEY_NUM  (7)
//ADC data
const uint16 tk_adc_data[TK_ADC_KEY_NUM] =
{ 0x0020, 0x0200, 0x0080, 0x0040, 0x0100, 0x0400, 0x0800 };

#define TK_GPIO_SCL_BIT   (1 << 9) //GPIOB9
#define TK_GPIO_SDA_BIT   (1 << 8) //GPIOB8

const gpio_init_cfg_t tk_gpio_twi[2] =
{
    { GPIOBINEN, GPIOBOUTEN, GPIOBDAT, TK_GPIO_SCL_BIT },
    { GPIOBINEN, GPIOBOUTEN, GPIOBDAT, TK_GPIO_SDA_BIT }
};

#define TK_DELAY_TWI  200
#define TK_SLAVE_ADDR 0x13 //READ ADDRESS

void welcome_i2c_tk_init(void)
{
    /* TWI 操作延时*/
    uint8 tk_delay_twi = TK_DELAY_TWI;

    TWI_Init(tk_gpio_twi, &tk_delay_twi);
}

/******************************************************************************
 * \par  Description:   读取i2c触摸按键的adc结果
 * \param[in]      none
 * \param[out]   none
 * \return           adc 结果
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-26pm
 *******************************************************************************/
bool read_tk_adc_data(uint16 *tk_adc_value)
{
    bool ret = TRUE;
    /* 读状态机*/
    i2c_tk_fsm_e i2c_tk_fsm = I2C_TK_FSM_IDLE;
    uint8 i2c_tk_adc_data[2];
    uint8 i2c_tk_read_length = 0;
    uint8 i2c_tk_read_index = 0;
    /* TWI 操作延时*/
    uint8 tk_delay_twi = TK_DELAY_TWI;

    do
    {
        switch (i2c_tk_fsm)
        {
            case I2C_TK_FSM_IDLE:
            TWI_Start(tk_gpio_twi, &tk_delay_twi);
            TWI_WriteByte(TK_SLAVE_ADDR, tk_gpio_twi, &tk_delay_twi);
            i2c_tk_fsm = I2C_TK_FSM_WRITE_ADDR;
            break;

            case I2C_TK_FSM_WRITE_ADDR:
            if (TWI_GetAck(tk_gpio_twi, &tk_delay_twi) == FALSE)
            {
                i2c_tk_adc_data[0] = 0xff;
                i2c_tk_adc_data[1] = 0xff;
                i2c_tk_fsm = I2C_TK_FSM_FAIL;
                ret = FALSE;
                //DEBUG_PRINT_IRQ("tk fail!", 0, 0);
            }
            else
            {
                i2c_tk_fsm = I2C_TK_FSM_GET_ACK;
                i2c_tk_read_length = 2;
                i2c_tk_read_index = 0;
            }
            break;

            case I2C_TK_FSM_GET_ACK:
            i2c_tk_adc_data[i2c_tk_read_index] = TWI_ReadByte(tk_gpio_twi, &tk_delay_twi);
            if (i2c_tk_read_length > 1)
            {
                TWI_SendAck(0, tk_gpio_twi, &tk_delay_twi);

                i2c_tk_read_length--;
                i2c_tk_read_index++;
            }
            else
            {
                TWI_SendAck(1, tk_gpio_twi, &tk_delay_twi);
                //DEBUG_PRINT_IRQ("tk succ!", 0, 0);
                i2c_tk_fsm = I2C_TK_FSM_SUCC;
            }
            break;

            case I2C_TK_FSM_SUCC:
            case I2C_TK_FSM_FAIL:
            default:
            TWI_Stop(tk_gpio_twi, &tk_delay_twi);
            i2c_tk_fsm = I2C_TK_FSM_IDLE;
            break;
        }

    } while (i2c_tk_fsm != I2C_TK_FSM_IDLE);

    *tk_adc_value = ((uint16) i2c_tk_adc_data[0] << 4) | (i2c_tk_adc_data[1] >> 4);
    //if (*tk_adc_value != 0xfff)
    //{
    //    DEBUG_PRINT_IRQ("tk adc:", *tk_adc_value, 2);
    //}

    return ret;
}

extern void welcome_delay_ms(uint8 dly_ms);

/******************************************************************************
 * \par  Description:   查询i2c触摸按键状态
 * \param[in]      none
 * \param[out]     none
 * \return         uint8 按键结果
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-26pm
 *******************************************************************************/
bool welcome_i2c_tk_check(void)
{
    bool ret_val = FALSE;
    uint16 tk_adc_value;
    uint8 i, try_time;

    //过滤开头不稳定部分
    for (i = 0; i < 20; i++)
    {
        welcome_delay_ms(5);
        read_tk_adc_data(&tk_adc_value);
    }

    for (try_time = 0; try_time < 20; try_time++)
    {
        read_tk_adc_data(&tk_adc_value);

        welcome_delay_ms(10);

        for (i = 0; i < TK_ADC_KEY_NUM; i++)
        {
            if (tk_adc_value == tk_adc_data[i])
            {
                ret_val = TRUE;
                break;
            }
        }
    }

    return ret_val;
}

#endif

