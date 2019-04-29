#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"

CONST struct GPIO_Ax_TypeDef GPIO_Ax = {
    (volatile GPIO_A0_TypeDef *)(BK3000_GPIO_BASE_ADDR+48*4),
    (volatile GPIO_A1_TypeDef *)(BK3000_GPIO_BASE_ADDR+49*4),
    (volatile GPIO_A2_TypeDef *)(BK3000_GPIO_BASE_ADDR+50*4),
    (volatile GPIO_A3_TypeDef *)(BK3000_GPIO_BASE_ADDR+51*4),
    (volatile GPIO_A4_TypeDef *)(BK3000_GPIO_BASE_ADDR+52*4),
    (volatile GPIO_A5_TypeDef *)(BK3000_GPIO_BASE_ADDR+53*4),
    (volatile GPIO_A6_TypeDef *)(BK3000_GPIO_BASE_ADDR+54*4),
    (volatile GPIO_A7_TypeDef *)(BK3000_GPIO_BASE_ADDR+55*4),
};

void gpio_enable_second_function( int gpio_function )
{
    int i, start_index=-1, end_index=-1;

    assert(gpio_function < 0 || gpio_function > 15);

    switch( gpio_function )
    {
        case GPIO_FUNCTION_UART2:
            break;
        case GPIO_FUNCTION_I2C2:
            break;
        case GPIO_FUNCTION_PCM2:
            break;
        case GPIO_FUNCTION_UART1_FLOW_CTL:
            break;
        case GPIO_FUNCTION_UART1_MONITOR:
            break;
        case GPIO_FUNCTION_SPI:
            start_index = 12;
            end_index = 14;
            break;
    #ifdef ENABLE_PWM
        case GPIO_FUNCTION_PWM0:
            start_index = end_index = 10; // 6
            break;
        case GPIO_FUNCTION_PWM1:
            start_index = end_index = 11; // 7
            break;
        case GPIO_FUNCTION_PWM2:
            start_index = end_index = 18; // 11
            break;
        case GPIO_FUNCTION_PWM3:
            start_index = end_index = 19; // 12
            break;
        case GPIO_FUNCTION_PWM4:
            start_index = end_index = 20; // 13
            break;
        case GPIO_FUNCTION_PWM5:
            start_index = end_index = 21; // 14
            break;
    #endif
        case GPIO_FUNCTION_SDIO:
            start_index = 12;
            #ifdef CONFIG_APP_SDCARD_4_LINE
            /* BK3000_GPIO_GPIOCON |= (0x3f<<12); */
            end_index = 17;
            #else
            /* BK3000_GPIO_GPIOCON |= (0x7<<12); */
            end_index = 14;
            #endif
            break;
        case GPIO_FUNCTION_I2C_FM:
            break;

        case GPIO_FUNCTION_JTAG:
            break;

        default:
            return;
     }

     for(i = start_index; i <= end_index; i++)
        gpio_config(i, 2);
}

void gpio_int_enable( int index, int level )
{
    assert(index > GPIO_NUM);

    BK3000_GPIO_INTSTA |= ( 1 << index );
    BK3000_GPIO_INTLV &= ~( 1 << index );
    BK3000_GPIO_INTLV |= ( level << index );
    BK3000_GPIO_WKUPEN |= ( 1 << index );
    BK3000_GPIO_INTEN |= ( 1 << index );
}

void gpio_button_wakeup_enable(void)
{
    app_handle_t app_h = app_get_sys_handler();
    BK3000_GPIO_INTEN = 0;
    BK3000_GPIO_INTSTA |= app_h->button_mask; // clear interrupt status;
    BK3000_GPIO_INTLV &= ~app_h->button_mask; // Low level detect
    BK3000_GPIO_INTLV |= (1<<22);             // High level detect for GPIO22 shutdown PIN;
//    BK3000_GPIO_WKUPEN |= app_h->button_mask;
    BK3000_GPIO_INTEN |= app_h->button_mask;
#if(CONFIG_UART_IN_SNIFF == 1)
    BK3000_GPIO_INTSTA |= (1 << 1);
    BK3000_GPIO_INTLV &= ~(1 << 1);  // Low level detect
    BK3000_GPIO_INTEN |= (1 << 1); 
    BK3000_GPIO_1_CONFIG = 0x3c;     // Uart Rx set 'input & pull up'
#endif

    
}


// 0 -input & pulldown, 1-output, 2--second function, 3-input & pullup , 4 -input , 5 -open-drain ¿ªÂ·ÉèÖÃ:bit[5]:0    bit[3]:1   bit[2]:0 
void gpio_config( int index, int dir )
{
    volatile unsigned long *ptr;

    assert(dir < 0 || dir > 5 || index > GPIO_NUM);

    if( dir < 0 || dir > 5 || index > GPIO_NUM || index <0)
        return;

    ptr = (volatile unsigned long *)(BK3000_GPIO_BASE_ADDR + index*4);

    if(dir  == 1)
        *ptr = 0;
    else if(dir == 0)
        *ptr = 0x2C;
    else if(dir == 2)
        *ptr = 0x70;
    else if(dir == 3)
        *ptr = 0x3C;
    else if(dir == 4)
        *ptr = 0x0C;
    else if(dir == 5)
        *ptr = 0x08;
}

void gpio_output( int index, uint32 val )
{
    volatile unsigned long *ptr;
    uint32 data;
	//#ifdef BEKEN_DEBUG
//	os_printf("gpio_output(%d,%d)\r\n",index,val);
	//#endif

    if(index > GPIO_NUM || index <0)
        return;
	
    ptr = (volatile unsigned long *)(BK3000_GPIO_BASE_ADDR + index*4);

    data = *ptr;
    data &= ~(1 << sft_GPIO_OUTPUT);
    data |= (val&0x01) << sft_GPIO_OUTPUT;
    *ptr = data;
}

uint32 gpio_input( int index )
{
    volatile unsigned long *ptr;

    if(index > GPIO_NUM || index <0)
        return 0;

    ptr = (volatile unsigned long *)(BK3000_GPIO_BASE_ADDR + index*4);

    return (*ptr)&( 1 << sft_GPIO_INPUT );
}

void gpio_output_reverse( int index )
{
    volatile unsigned long *ptr;
    uint32 data;

    if(index > GPIO_NUM || index <0)
        return;

    ptr = (volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+index*4);
    data = *ptr;
    data ^= ( 1 << sft_GPIO_OUTPUT );
    *ptr = data;
}

void gpio_isr( void )
{
    uint32 gpiosts = BK3000_GPIO_INTSTA;
    uint32 old_mask = get_spr(SPR_VICMR(0));
    app_handle_t app_h = app_get_sys_handler();
#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
    app_exit_sniff_mode();
#if (CONFIG_UART_IN_SNIFF == 1)
    if(gpiosts & (1<<1))		//GPIO01 = Uart Rx
    {
        BK3000_GPIO_INTEN &= ~(1 << 1);
        sniffmode_wakeup_dly = 1000;
        uart_initialise(UART_BAUDRATE_115200);
        os_printf("===uart wake-up!!!\r\n");  // The first rx datas only wake up CPU from halting state;
    }
#endif
#endif
    old_mask &= ~(1<<VIC_GPIO_ISR_INDEX);
    set_spr(SPR_VICMR(0), old_mask);

    BK3000_GPIO_INTEN &= ~app_h->button_mask;
    BK3000_GPIO_INTSTA = gpiosts;
//    cpu_set_interrupts_enabled(0);
//    set_spr( SPR_VICMR(0), old_mask );
}
// EOF
