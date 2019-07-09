#include "include.h"
#include "ledseg_common.h"
#include "ledseg_7p3s_HBS7642.h"

#if (GUI_SELECT == GUI_LEDSEG_7P3S_HBS7642)

static unsigned char disp_flush;
static unsigned char disp_buffer[6];
static unsigned char disp_on_off;
//--------------------------------------------------------------------------------
//
AT(.text.ledseg.uart)
void uart_ledseg_init(void)
{
	GPIOBDE  |= BIT(2);
	GPIOBSET |= BIT(2);
	GPIOBDIR &= ~BIT(2);
	GPIOBFEN |= BIT(2);

	//CLKGAT1 |= BIT(11);
	CLKCON1 |= BIT(14);
	FUNCMCON1 &=~(0x0F<<4);
	FUNCMCON1 |= (0x02<<4);

	UART2BAUD = 0x2A4;//0x152;					//19200   26M
	UART2CON  = BIT(0) | BIT(1) | BIT(5);		//1 STOP TX EN
}

//--------------------------------------------------------------------------------
//
AT(.com_text.ledseg)
void uart_ledseg_putchar(unsigned char val)
{
	unsigned int verify = val;

	verify ^= verify >>4;
	verify ^= verify >>2;
	verify ^= verify >>1;
	verify = !(verify&1);
	verify <<= 8;

	while(!(UART2CON & BIT(8)));
	UART2DATA = val | verify;
}

AT(.com_text.ledseg)
void force_flush_ledseg(void)
{
	disp_flush = 1;
}

//--------------------------------------------------------------------------------
//
AT(.text.ledseg.const1)
static const unsigned char HBS7642_bright[3] = {
	HBS7642_DUTY_CMD_2_16 | HBS7642_CURR_CMD_4_8,
	HBS7642_DUTY_CMD_8_16 | HBS7642_CURR_CMD_4_8,
	HBS7642_DUTY_CMD_15_16 | HBS7642_CURR_CMD_8_8,
};

AT(.text.ledseg.init)
void ledseg_7p3s_HBS7642_brightness_loop(void)
{
	if(tm_cb.sleep_mode)
		return;
	
	sys_cb.bright_lv++;
	if(sys_cb.bright_lv >= 3){
		sys_cb.bright_lv = 0;
	}
	printf("blv:%d\r\n", sys_cb.bright_lv);
	param_sys_bright_write();
	disp_flush = 0;
	uart_ledseg_putchar(HBS7642_CMD_ADRESS);
	uart_ledseg_putchar(HBS7642_bright[sys_cb.bright_lv] | HBS7642_TYPE_CMD_7P6S);
	delay_ms(10);
}

//--------------------------------------------------------------------------------
//
AT(.text.ledseg.init)
void ledseg_7p3s_HBS7642_on_off(void)
{
	disp_on_off = !disp_on_off;
	printf("onoff:%d\r\n", disp_on_off);
	disp_flush = 0;
	if(disp_on_off){
		uart_ledseg_putchar(HBS7642_CMD_ADRESS);
		uart_ledseg_putchar(HBS7642_bright[sys_cb.bright_lv] | HBS7642_TYPE_CMD_7P6S);
	}else{
		uart_ledseg_putchar(HBS7642_CMD_ADRESS);
		uart_ledseg_putchar(HBS7642_DISP_CMD_OFF);
	}
	delay_ms(10);
}
//--------------------------------------------------------------------------------
//
AT(.text.ledseg.init)
void ledseg_7p3s_HBS7642_init(void)
{
    memset(ledseg_buf, 0, sizeof(ledseg_buf));
    memset(disp_buffer, 0, sizeof(disp_buffer));

	disp_flush 		= 0;
    ledseg_disp_num = 0xff;
	uart_ledseg_init();

	uart_ledseg_putchar(HBS7642_DISP_ADRESS);
	uart_ledseg_putchar(disp_buffer[0]);
	uart_ledseg_putchar(disp_buffer[1]);
	uart_ledseg_putchar(disp_buffer[2]);
	uart_ledseg_putchar(disp_buffer[3]);
	uart_ledseg_putchar(disp_buffer[4]);
	uart_ledseg_putchar(disp_buffer[5]);
	delay_ms(5);
	uart_ledseg_putchar(HBS7642_CMD_ADRESS);
	uart_ledseg_putchar(HBS7642_bright[sys_cb.bright_lv] | HBS7642_TYPE_CMD_7P6S);
	delay_ms(5);
	disp_on_off = 1;
}

//--------------------------------------------------------------------------------
//
AT(.text.ledseg)
void ledseg_7p3s_HBS7642_off(void)
{
	memset(ledseg_buf, 0, sizeof(ledseg_buf));
	memset(disp_buffer, 0, sizeof(disp_buffer));
	disp_flush = 1;

	uart_ledseg_putchar(HBS7642_CMD_ADRESS);
	uart_ledseg_putchar(HBS7642_DISP_CMD_OFF);
}


AT(.text.ledseg)
void ledseg_7p3s_HBS7642_on(void)
{
	printf("LV:%d\r\n", sys_cb.bright_lv);
	uart_ledseg_putchar(0x00);
	delay_ms(100);
	uart_ledseg_putchar(HBS7642_CMD_ADRESS);
	uart_ledseg_putchar(HBS7642_bright[sys_cb.bright_lv] | HBS7642_TYPE_CMD_7P6S);
	disp_on_off = 1;
}

//--------------------------------------------------------------------------------
//

AT(.text.ledseg)
void ledseg_7p3s_HBS7642_update_dispbuf(void)
{
	disp_flush = 0;
	memcpy(disp_buffer, ledseg_buf, 6);
	disp_flush = 1;
}

//--------------------------------------------------------------------------------
//
//按COM扫描, 5ms扫描间隔
AT(.com_text.ledseg)
void ledseg_7p3s_HBS7642_scan(void)
{
	if(disp_flush){
		uart_ledseg_putchar(HBS7642_DISP_ADRESS);
		uart_ledseg_putchar(disp_buffer[0]);
		uart_ledseg_putchar(disp_buffer[1]);
		uart_ledseg_putchar(disp_buffer[2]);
		uart_ledseg_putchar(disp_buffer[3]);
		uart_ledseg_putchar(disp_buffer[4]);
		uart_ledseg_putchar(disp_buffer[5]);
		disp_flush = 0;
	}

}

#endif


