/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                             All Rights Reserved.
 *      Description:i2c触摸按键的处理
 *      <author>       <time>
 *       Wekan       2015-3-26pm
 *******************************************************************************/

#include "key_inner.h"

#if (SUPPORT_TK_DRIVER == TK_DRIVER_OUTER)

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
// mode    prev         play         next        V-            V+          call
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

void key_i2c_tk_init(void)
{
    /* TWI 操作延时*/
    uint8 tk_delay_twi = TK_DELAY_TWI;

    TWI_Init((gpio_init_cfg_t*)tk_gpio_twi, &tk_delay_twi);

    g_i2s_tk_enable = TRUE;
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
            TWI_Start((gpio_init_cfg_t*)tk_gpio_twi, &tk_delay_twi);
            TWI_WriteByte(TK_SLAVE_ADDR, (gpio_init_cfg_t*)tk_gpio_twi, &tk_delay_twi);
            i2c_tk_fsm = I2C_TK_FSM_WRITE_ADDR;
            break;

            case I2C_TK_FSM_WRITE_ADDR:
            if (TWI_GetAck((gpio_init_cfg_t*)tk_gpio_twi, &tk_delay_twi) == FALSE)
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
            i2c_tk_adc_data[i2c_tk_read_index] = TWI_ReadByte((gpio_init_cfg_t*)tk_gpio_twi, &tk_delay_twi);
            if (i2c_tk_read_length > 1)
            {
                TWI_SendAck(0, (gpio_init_cfg_t*)tk_gpio_twi, &tk_delay_twi);

                i2c_tk_read_length--;
                i2c_tk_read_index++;
            }
            else
            {
                TWI_SendAck(1, (gpio_init_cfg_t*)tk_gpio_twi, &tk_delay_twi);
                //DEBUG_PRINT_IRQ("tk succ!", 0, 0);
                i2c_tk_fsm = I2C_TK_FSM_SUCC;
            }
            break;

            case I2C_TK_FSM_SUCC:
            case I2C_TK_FSM_FAIL:
            default:
            TWI_Stop((gpio_init_cfg_t*)tk_gpio_twi, &tk_delay_twi);
            i2c_tk_fsm = I2C_TK_FSM_IDLE;
            break;
        }

        if ((i2c_tk_fsm != I2C_TK_FSM_IDLE) && (g_key_scan_isr_cbk != NULL))
        {
            g_key_scan_isr_cbk();
        }

    } while (i2c_tk_fsm != I2C_TK_FSM_IDLE);

    *tk_adc_value = ((uint16) i2c_tk_adc_data[0] << 4) | (i2c_tk_adc_data[1] >> 4);
    //if (*tk_adc_value != 0xfff)
    //{
    //    DEBUG_PRINT_IRQ("tk adc:", *tk_adc_value, 2);
    //}

    return ret;
}
/******************************************************************************
 * \par  Description:   查询i2c触摸按键状态
 * \param[in]      none
 * \param[out]     none
 * \return         uint8 按键结果
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-26pm
 *******************************************************************************/
uint8 key_i2c_tk_check(void)
{
    uint16 tk_adc_value;
    uint8 tk_key_val, i;
    if (g_i2s_tk_enable == FALSE)
    {
        return NO_KEY;
    }

    tk_key_val = NO_KEY;

    if (read_tk_adc_data(&tk_adc_value) == FALSE)
    {
        return NO_KEY;
    }
    for (i = 0; i < TK_ADC_KEY_NUM; i++)
    {
        if (tk_adc_value == tk_adc_data[i])
        {
            tk_key_val = i;
            break;
        }
    }
    return tk_key_val;
}

uint8 key_inner_extern_tk_op(void* null0, void *null1, tk_op_e op)
{
    uint8 ret_val = 0;

    switch (op)
    {
    case TK_OP_ON:
        //给触摸屏供电
        act_writel(act_readl(EXTERN_TK_PLANE_GPIOOUT) | EXTERN_TK_PLANE_PIN, EXTERN_TK_PLANE_GPIOOUT);
        act_writel(act_readl(EXTERN_TK_PLANE_GPIODAT) | EXTERN_TK_PLANE_PIN, EXTERN_TK_PLANE_GPIODAT);

        key_i2c_tk_init();
        break;

    case TK_OP_OFF:
        //触摸屏断电
        act_writel(act_readl(EXTERN_TK_PLANE_GPIOOUT) | EXTERN_TK_PLANE_PIN, EXTERN_TK_PLANE_GPIOOUT);
        act_writel(act_readl(EXTERN_TK_PLANE_GPIODAT) & (~EXTERN_TK_PLANE_PIN), EXTERN_TK_PLANE_GPIODAT);
        break;

    case TK_OP_CHECK:
        ret_val = key_i2c_tk_check();
        if (ret_val == NO_KEY)
        {
            ret_val = 0;
        }
        else
        {
            ret_val = 1;
        }
        break;

    default:
        break;
    }

    return ret_val;
}

#endif

