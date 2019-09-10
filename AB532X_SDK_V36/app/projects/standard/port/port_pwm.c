#include "include.h"

#if PWM_RGB_EN

#define PWM_RGB_PR                  255*64

//LPWM0, LPWM2, LPW3对应R, G, B控制
void pwm_init(void)
{
    GPIOEDE |= (BIT(4) | BIT(6) | BIT(7));
    GPIOEDIR &= ~(BIT(4) | BIT(6) | BIT(7));
    GPIOE &= ~(BIT(4) | BIT(6) | BIT(7));
    FUNCMCON1 = LPWM0MAP_PE4 | LPWM2MAP_PE6 | LPWM3MAP_PE7;

    PWMPRCON = PWM_RGB_PR;
    PWMCON = 0;                     //初始化disable pwm
}

void pwm_rgb_write(u8 red, u8 green, u8 blue)
{
    PWM01DUTY = ((u32)red << 6);
    PWM23DUTY = ((u32)green << 6) | ((u32)blue << (6 + 16));
    if ((PWMCON & 0x0D) != 0x0D) {
        FUNCMCON1 = LPWM0MAP_PE4 | LPWM2MAP_PE6 | LPWM3MAP_PE7;
        PWMCON |= 0x0D;             //enbale LPWM0, LPWM2, LPWM3(R,G,B)
    }
}

void pwm_rgb_close(void)
{
    FUNCMCON1 = 0xff0f0000;
    PWMCON = 0;
}


/*
void pwm_test(void)
{
    printf("%s\n", __func__);
    GPIOADE |= 0x0f;
    GPIOADIR &= ~0x0f;              // PA0 ~ PA3 对应 lowpower PWM0~PWM3 Mapping G3
    GPIOA &= ~0x0f;
    FUNCMCON1 = (3 << 16) | (3 << 20) | (3 << 24) | (3 << 28);

    PWMPRCON = 0x6590;              //PWM period = (PWMPR+1)，分辨率26MHz/4    0x6590->4ms
    PWM01DUTY = 0x32c84c2c;         //DUTY: PWMIVN=0:设置高电平时间； PWMIVN=1:设置低电平时间
    PWM23DUTY = 0x19641964;

    PWMSTEP = 0x68686868;           //PWMSTEP有效位7bit，最高位为符号位 （按STEP加减调节到DUTY或0） 0x68->16us
    PWMCYCNUM = 0x01010101;         //CYCNUM当前STEP的PWM输出次数 = PWMCYCNUM + 1

    PWMCON = 0x1f;
}
*/
#endif // PWM_RGB_EN


#if ENERGY_LED_EN    //能量灯软件PWM输出.

#define  ENERGY_LED_NUM 4         //灯的个数
#define  ENERGY_PWM_MAX_DUTY  8   //一个灯分8个亮度.

#define  ENERGY_LED_INIT()     {GPIOBDE |= 0x0F; GPIOBDIR &= ~0x0F;}
#define  ENERGY_LED_0_ON()     GPIOBSET = BIT(0)
#define  ENERGY_LED_0_OFF()    GPIOBCLR = BIT(0)
#define  ENERGY_LED_1_ON()     GPIOBSET = BIT(1)
#define  ENERGY_LED_1_OFF()    GPIOBCLR = BIT(2)
#define  ENERGY_LED_2_ON()     GPIOBSET = BIT(2)
#define  ENERGY_LED_2_OFF()    GPIOBCLR = BIT(2)
#define  ENERGY_LED_3_ON()     GPIOASET = BIT(3)
#define  ENERGY_LED_3_OFF()    GPIOACLR = BIT(3)

u8 pwm_duty_buf[ENERGY_LED_NUM];

AT(.com_text.rgb.tbl)   //非线性量化表.
u8 const energy_qtz_tbl[4 * 8] = {
     1,14,18,22,26,30,35,40,
     41,43,46,48,51,53,56,60,
     61,63,66,68,71,73,76,80,
     81,83,85,88,91,93,96,100,
};

void energy_led_init(void)
{
    ENERGY_LED_INIT();
}

AT(.com_text.rgb)
void energy_led_level_calc(void)    //约5ms调用一次.
{
    u8 level,i;
    u16 energy;
    static u8 disp_level,last_level;
    static u8 time_5ms_cnt = 0;

    time_5ms_cnt++;
    if(time_5ms_cnt >= 4){   //20ms计算一次能量
        time_5ms_cnt  = 0;

        energy = dac_pcm_pow_calc();
        //printf("[%X_%d]",rgb_led_en,energy);
        if (FUNC_MUSIC == func_cb.sta) {   //各模式能量效果可能不一样.
            energy = energy/450;
        } else if (FUNC_FMRX == func_cb.sta) {
            energy = energy/350;
        } else{
            energy = energy/300;
        }
        //非线性量化表
        for (i = 0, last_level = 0; i < 4*8; i++) {
           if (energy < energy_qtz_tbl[i]) {
               break;
           }
           last_level++;
        }
    }
    //能量相同, 不用更新
    if(disp_level == last_level){
        return;
    }

    //能量减少时,慢慢下降
    if (disp_level > last_level) {
        disp_level--;
    } else {
        disp_level = last_level;
    }

    //能量转为占空比,存放到pwm_duty_buf中.
    level = disp_level;
    for (i=0; i<ENERGY_LED_NUM; i++){
        if (level >= ENERGY_PWM_MAX_DUTY) {
            pwm_duty_buf[i] = 0;
            level -= ENERGY_PWM_MAX_DUTY;
        } else {
            pwm_duty_buf[i] = (1<<level) - 1;
            pwm_duty_buf[i] = ~pwm_duty_buf[i];
            level = 0;
        }
    }
}

AT(.com_text.rgb)
void energy_led_scan(void)  //建议1ms扫描一次.
{
    static u8 scan_bit = 0;
    if (pwm_duty_buf[0] & BIT(scan_bit)){
        ENERGY_LED_0_OFF();
    } else {
        ENERGY_LED_0_ON();
    }

    if (pwm_duty_buf[1] & BIT(scan_bit)) {
        ENERGY_LED_1_OFF();
    } else {
        ENERGY_LED_1_ON();
    }

    if (pwm_duty_buf[2] & BIT(scan_bit)) {
        ENERGY_LED_2_OFF();
    } else {
        ENERGY_LED_2_ON();
    }

    if (pwm_duty_buf[3] & BIT(scan_bit)) {
        ENERGY_LED_3_OFF();
    } else {
        ENERGY_LED_3_ON();
    }

    if (++scan_bit >= ENERGY_PWM_MAX_DUTY){ //ENERGY_PWM_MAX_DUTY= 8
        scan_bit = 0;
    }
}
#endif // ENERGY_LED_EN

#if RGB_SERIAL_EN
/**************************************************************************************************
///由于时序要求严格，建议使用120M系统时钟，否则因为系统时间更改导致SPI 波特率变化引起时序错误
***************************************************************************************************/
#define RGB_BAUD    100
u8 rgb_buf[360];   //rgb数据BUF
void rgb_spi1_init(u32 baud)
{
    //SPI1_G1: CLK_PA3, DIO_PA4,   //CS_PA5
//    GPIOAFEN |= BIT(3)|BIT(4);
//    GPIOADE |=  BIT(3)|BIT(4);
//    GPIOADIR &= ~(BIT(3) | BIT(4));  //CLK,DO output
//    GPIOADIR |= BIT(4);  //DI input
//    GPIOAPU |= BIT(4);   //DI pull up 10K
//    FUNCMCON1 = (0x0F<<12);
//    FUNCMCON1 = (0x01<<12);  //G1


    GPIOAFEN |= BIT(4);
    GPIOADE |=  BIT(4);
    GPIOADIR &= ~(BIT(4));  //CLK,DO output
    FUNCMCON1 = (0x0F<<12);
    FUNCMCON1 = (0x01<<12);  //G1

//    //SPI1_G2:  DIO_PA7,  CLK_PA6, //CS_PA5
//    GPIOAFEN |= BIT(6)|BIT(7);
//    GPIOADE |= BIT(6)|BIT(7);
//    GPIOADIR &= ~ BIT(6) ;  //CLK output
//    GPIOADIR |= BIT(7);  //DI input
//    GPIOAPU |= BIT(7);   //DI pull up 10K
//    FUNCMCON1 = (0x0F<<12);
//    FUNCMCON1 = (0x02<<12); //G2

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SPI1BAUD = 24000000/baud - 1;   //sysclk/baud - 1
    SPI1CON =  0x01 | (1<<2);
    memset(rgb_buf,0xa5,sizeof(rgb_buf));
}
/*直接调用此函数更新数据即可*/
AT(.com_text.rgb)
void spi1_senddata(void)
{
    SPI1DMAADR = DMA_ADR(rgb_buf);
    SPI1DMACNT = sizeof(rgb_buf);
//    while(!(SPI1CON & BIT(16))){  //wait send finish
//        WDT_CLR();
//    }
}

void rgb_spi_test(void)
{
    rgb_spi1_init(RGB_BAUD);
//    while(1)
//    {
//        WDT_CLR();
//        spi1_senddata();
//        delay_5ms(100);
//        printf("->");
//    }
}

#endif // RGB_SERIAL_EN

//定时器PWM功能，注意用Timer3时遥控功能不能实现，用Timer4/5时 TWS不能实现
#if PWM_TIMER_EN
//常量宏定义
#define	TIMER_MASK				0xF0			//timer MASK
#define CHANNEL_MASK			0x0F			//CHANNEL MASK
#define SYSTEM_CLK				260000000L		//系统时钟频率
#define TIMER_DUTY_MAX			0xffff			//占空比最大值

typedef struct {
	uint32_t DIR;
	uint32_t DE;
	uint32_t FEN;
	uint32_t IO;
}pwm_cb_t;

const pwm_cb_t timer3_map[4] = {
	//GPIOXDIR			//GPIOXDE			//GPIOXFEN			//BITS
	{(SFR6_BASE+0x03*4),(SFR6_BASE+0x04*4),(SFR6_BASE+0x05*4),(uint32_t)(BIT(2)|(BIT(3)<<8)|(BIT(4)<<16)),},
	{(SFR6_BASE+0x13*4),(SFR6_BASE+0x14*4),(SFR6_BASE+0x15*4),(uint32_t)((0xff)|(BIT(3)<<8)|(BIT(4)<<16)),},
	{(SFR6_BASE+0x13*4),(SFR6_BASE+0x14*4),(SFR6_BASE+0x15*4),(uint32_t)(BIT(0)|(BIT(1)<<8)|(BIT(2)<<16)),},
	{(SFR6_BASE+0x23*4),(SFR6_BASE+0x24*4),(SFR6_BASE+0x25*4),(uint32_t)(BIT(0)|(BIT(1)<<8)|((0xff)<<16)),},
};

const pwm_cb_t timer4_map[3] = {
	//GPIOXDIR			//GPIOXDE			//GPIOXFEN			//BITS
	{(SFR6_BASE+0x03*4),(SFR6_BASE+0x04*4),(SFR6_BASE+0x05*4),(uint32_t)(BIT(0)|(BIT(1)<<8)|((0xff)<<16)),},
	{(SFR6_BASE+0x23*4),(SFR6_BASE+0x24*4),(SFR6_BASE+0x25*4),(uint32_t)(BIT(5)|(BIT(6)<<8)|(BIT(7)<<16)),},
	{(SFR6_BASE+0x33*4),(SFR6_BASE+0x34*4),(SFR6_BASE+0x35*4),(uint32_t)(BIT(1)|(BIT(0)<<8)|(BIT(5)<<16)),},
};

const pwm_cb_t timer5_map[3] = {
	//GPIOXDIR			//GPIOXDE			//GPIOXFEN			//BITS
	{(SFR6_BASE+0x03*4),(SFR6_BASE+0x04*4),(SFR6_BASE+0x05*4),(uint32_t)(BIT(5)|(BIT(6)<<8)|(BIT(7)<<16)),},
	{(SFR6_BASE+0x23*4),(SFR6_BASE+0x24*4),(SFR6_BASE+0x25*4),(uint32_t)(BIT(2)|(BIT(3)<<8)|(BIT(4)<<16)),},
	{(SFR6_BASE+0x13*4),(SFR6_BASE+0x14*4),(SFR6_BASE+0x15*4),(uint32_t)(BIT(5)|(BIT(6)<<8)|(BIT(7)<<16)),},
};

/*****************************************************************************
 函 数 名  : timer_pwm_enable
 功能描述  : 定时器PWM通道配置
 输入参数  : uint8_t PwmChannel		定时器与通道组合，参见config_define.h  
           uint8_t MapSel
           @ TIMER3PWM MAP G1	PWM0(PA2), PWM1(PA3), PWM2(PA4)
           @ TIMER3PWM MAP G2	PWM1(PB3), PWM2(PB4)
           @ TIMER3PWM MAP G3	PWM0(PB0), PWM1(PB1), PWM2(PB2)
           @ TIMER3PWM MAP G4	PWM0(PE0), PWM1(PE1)

           @ TIMER4PWM MAP G1	PWM0(PA0), PWM1(PA1)
           @ TIMER4PWM MAP G2	PWM0(PE5), PWM1(PE6), PWM2(PE7)
           @ TIMER4PWM MAP G3	PWM0(PF1), PWM1(PF0), PWM2(PF5)

           @ TIMER5PWM MAP G1	PWM0(PA5), PWM1(PA6), PWM2(PA7)
           @ TIMER5PWM MAP G2	PWM0(PE2), PWM1(PE3), PWM2(PE4)
           @ TIMER5PWM MAP G3	PWM0(PB5), PWM1(PB6), PWM2(PB7) 
		   bool OutLevel        1:HIGHT    0:LOW
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年5月17日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void timer_pwm_enable(uint8_t PwmChannel, uint8_t MapSel)
{
	uint8_t TimerIndex = PwmChannel&TIMER_MASK;
	uint8_t ChannelID = PwmChannel&CHANNEL_MASK;
	uint8_t io_index = 0xff;
	
	if (PWM_TIMER3 == TimerIndex) {
		io_index = (timer3_map[MapSel-1].IO>>(ChannelID*8))&0xff;
		
		if (0xff == io_index) {
			printf("timer3 is not this channel;\n");
			return;
		}
		
		SFR_RW(timer3_map[MapSel-1].DIR) &= ~io_index;
		SFR_RW(timer3_map[MapSel-1].DE) |= io_index;
		SFR_RW(timer3_map[MapSel-1].FEN) |= io_index;
		
		FUNCMCON2 |= (uint32_t)(0x0f<<8);
		FUNCMCON2 |= (uint32_t)(MapSel<<8);
		TMR3CON	&= ~(uint32_t)(0x0c);
		TMR3CNT = 0;

		if (PWM_CHANNEL0 == ChannelID) {
			TMR3CON	|= BIT(9);
		}
		else if (PWM_CHANNEL1 == ChannelID) {
			TMR3CON	|= BIT(10);
		}
		else if (PWM_CHANNEL2 == ChannelID) {
			TMR3CON	|= BIT(11);
		}	
	}
	else if (PWM_TIMER4 == TimerIndex) {
		io_index = (timer4_map[MapSel-1].IO>>(ChannelID*8))&0xff;
		
		if (0xff == io_index) {
			printf("timer4 is not this channel;\n");
			return;
		}
		
		SFR_RW(timer4_map[MapSel-1].DIR) &= ~io_index;
		SFR_RW(timer4_map[MapSel-1].DE) |= io_index;
		SFR_RW(timer4_map[MapSel-1].FEN) |= io_index;

		FUNCMCON2 |= (uint32_t)(0x0f<<12);
		FUNCMCON2 |= (uint32_t)(MapSel<<12);
		TMR4CON	&= ~(uint32_t)(0x0c);
		TMR4CNT = 0;

		if (PWM_CHANNEL0 == ChannelID) {
			TMR4CON	|= BIT(9);
		}
		else if (PWM_CHANNEL1 == ChannelID) {
			TMR4CON	|= BIT(10);
		}
		else if (PWM_CHANNEL2 == ChannelID) {
			TMR4CON	|= BIT(11);
		}
	}
	else if (PWM_TIMER5 == TimerIndex) {
		io_index = (timer5_map[MapSel-1].IO>>(ChannelID*8))&0xff;
		
		if (0xff == io_index) {
			printf("timer5 is not this channel;\n");
			return;
		}
		
		SFR_RW(timer5_map[MapSel-1].DIR) &= ~io_index;
		SFR_RW(timer5_map[MapSel-1].DE) |= io_index;
		SFR_RW(timer5_map[MapSel-1].FEN) |= io_index;
		
		FUNCMCON2 |= (uint32_t)(0x0f<<16);
		FUNCMCON2 |= (uint32_t)(MapSel<<16);
		TMR5CON	&= ~(uint32_t)(0x0c);
		TMR5CNT = 0;

		if (PWM_CHANNEL0 == ChannelID) {
			TMR5CON	|= BIT(9);
		}
		else if (PWM_CHANNEL1 == ChannelID) {
			TMR5CON	|= BIT(10);
		}
		else if (PWM_CHANNEL2 == ChannelID) {
			TMR5CON	|= BIT(11);
		}
	}
}

/*****************************************************************************
 函 数 名  : timer_pwm_disable
 功能描述  : 关闭定时器PWM通道
 输入参数  : uint8_t PwmChannel  
             uint8_t MapSel      
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年5月18日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void timer_pwm_disable(uint8_t PwmChannel, uint8_t MapSel)
{
	uint8_t TimerIndex = PwmChannel&TIMER_MASK;
	uint8_t ChannelID = PwmChannel&CHANNEL_MASK;
	uint8_t io_index = 0xff;
	
	if (PWM_TIMER3 == TimerIndex) {
		io_index = (timer3_map[MapSel-1].IO>>(ChannelID*8))&0xff;
		
		if (0xff == io_index) {
			printf("timer3 is not this channel;\n");
			return;
		}
		
		SFR_RW(timer3_map[MapSel-1].DIR) &= ~io_index;
		SFR_RW(timer3_map[MapSel-1].DE) |= io_index;
		SFR_RW(timer3_map[MapSel-1].FEN) &= ~io_index;
		
		if (PWM_CHANNEL0 == ChannelID) {
			TMR3CON &= ~BIT(9);
		}
		else if (PWM_CHANNEL1 == ChannelID) {
			TMR3CON &= ~BIT(10);
		}
		else if (PWM_CHANNEL2 == ChannelID) {
			TMR3CON &= ~BIT(11);
		}

		if (!(TMR3CON & (BIT(9)|BIT(10)|BIT(11)))) {
			TMR3CON &= ~BIT(0);
		}
	}
	else if (PWM_TIMER4 == TimerIndex) {
		io_index = (timer4_map[MapSel-1].IO>>(ChannelID*8))&0xff;
		
		if (0xff == io_index) {
			printf("timer4 is not this channel;\n");
			return;
		}
		
		SFR_RW(timer4_map[MapSel-1].DIR) &= ~io_index;
		SFR_RW(timer4_map[MapSel-1].DE) |= io_index;
		SFR_RW(timer4_map[MapSel-1].FEN) &= ~io_index;
		
		if (PWM_CHANNEL0 == ChannelID) {
			TMR4CON &= ~BIT(9);
		}
		else if (PWM_CHANNEL1 == ChannelID) {
			TMR4CON &= ~BIT(10);
		}
		else if (PWM_CHANNEL2 == ChannelID) {
			TMR4CON &= ~BIT(11);
		}

		if (!(TMR4CON & (BIT(9)|BIT(10)|BIT(11)))) {
			TMR4CON &= ~BIT(0);
		}
	}
	else if (PWM_TIMER5 == TimerIndex) {
		io_index = (timer5_map[MapSel-1].IO>>(ChannelID*8))&0xff;
		
		if (0xff == io_index) {
			printf("timer5 is not this channel;\n");
			return;
		}
		
		SFR_RW(timer5_map[MapSel-1].DIR) &= ~io_index;
		SFR_RW(timer5_map[MapSel-1].DE) |= io_index;
		SFR_RW(timer5_map[MapSel-1].FEN) &= ~io_index;
		
		if (PWM_CHANNEL0 == ChannelID) {
			TMR5CON &= ~BIT(9);
		}
		else if (PWM_CHANNEL1 == ChannelID) {
			TMR5CON &= ~BIT(10);
		}
		else if (PWM_CHANNEL2 == ChannelID) {
			TMR5CON &= ~BIT(11);
		}

		if (!(TMR5CON & (BIT(9)|BIT(10)|BIT(11)))) {
			TMR5CON &= ~BIT(0);
		}
	}
}

/*****************************************************************************
 函 数 名  : timer_pwm_config
 功能描述  : PWM通道频率及占空比配置
 输入参数  : uint8_t PwmChannel  
             uint16_t Freq  		步进0.1HZ     
             uint16_t Duty  		步进0.1%     
             bool OutLevel 			1：HIGHT    0：LOW      
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年5月18日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void timer_pwm_config(uint8_t PwmChannel, uint16_t Freq, uint16_t Duty, bool OutLevel)
{
	uint8_t TimerIndex = PwmChannel&TIMER_MASK;
	uint8_t ChannelID = PwmChannel&CHANNEL_MASK;
	
	if (PWM_TIMER3 == TimerIndex) {
		if (TMR3CON & BIT(0)) {
			if ((SYSTEM_CLK/Freq-1) != TMR3PR) {
				printf("timer3 diff channel not set diff freq;\n");
				return;
			}
		}
		else {
			TMR3CON &= ~(uint32_t)(0x0c);
			TMR3PR = SYSTEM_CLK/Freq-1;
			TMR3CON |= BIT(0);
		}

		uint32_t tmr_duty = ((TMR3PR+1)*Duty/1000-1);

		if (OutLevel) {
			tmr_duty = (TMR3PR-tmr_duty+1);
		}
		tmr_duty += 1;
		
		if (tmr_duty > TIMER_DUTY_MAX) {
			printf("timer3 channel duty setting is ERRO!!! %d,%d;\n", Duty,ChannelID);
			tmr_duty = TIMER_DUTY_MAX;
		}
		
		if (PWM_CHANNEL0 == ChannelID) {
			TMR3DUTY0 = tmr_duty;
		}
		else if (PWM_CHANNEL1 == ChannelID) {
			TMR3DUTY1 = tmr_duty;
		}
		else if (PWM_CHANNEL2 == ChannelID) {
			TMR3DUTY2 = tmr_duty;
		}		
	}
	else if (PWM_TIMER4 == TimerIndex) {
		if (TMR4CON & BIT(0)) {
			if ((SYSTEM_CLK/Freq-1) != TMR4PR) {
				printf("timer4 diff channel not set diff freq;\n");
				return;
			}
		}
		else {
			TMR4CON &= ~(uint32_t)(0x0c);
			TMR4PR = SYSTEM_CLK/Freq-1;
			TMR4CON |= BIT(0);
		}

		uint32_t tmr_duty = ((TMR4PR+1)*Duty/1000-1);

		if (OutLevel) {
			tmr_duty = (TMR4PR-tmr_duty);
		}
		tmr_duty += 1;
		//printf ("timer4 channel duty setting is %d,%d;\n", tmr_duty,ChannelID);
		
		if (tmr_duty > TIMER_DUTY_MAX) {
			printf("timer4 channel duty setting is ERRO!!! %d,%d;\n", Duty,ChannelID);
			tmr_duty = TIMER_DUTY_MAX;
		}
		
		if (PWM_CHANNEL0 == ChannelID) {
			TMR4DUTY0 = tmr_duty;
		}
		else if (PWM_CHANNEL1 == ChannelID) {
			TMR4DUTY1 = tmr_duty;
		}
		else if (PWM_CHANNEL2 == ChannelID) {
			TMR4DUTY2 = tmr_duty;
		}
	}
	else if (PWM_TIMER5 == TimerIndex) {
		if (TMR5CON & BIT(0)) {
			if ((SYSTEM_CLK/Freq-1) != TMR5PR) {
				printf("timer5 diff channel not set diff freq;\n");
				return;
			}
		}
		else {
			TMR5CON &= ~(uint32_t)(0x0c);
			TMR5PR = SYSTEM_CLK/Freq-1;
			TMR5CON |= BIT(0);
		}

		uint32_t tmr_duty = ((TMR5PR+1)*Duty/1000-1);

		if (OutLevel) {
			tmr_duty = (TMR5PR-tmr_duty);
		}
		tmr_duty += 1;

		if (tmr_duty > TIMER_DUTY_MAX) {
			printf("timer5 channel duty setting is ERRO!!! %d,%d;\n", Duty,ChannelID);
			tmr_duty = TIMER_DUTY_MAX;
		}
		
		if (PWM_CHANNEL0 == ChannelID) {
			TMR5DUTY0 = tmr_duty;
		}
		else if (PWM_CHANNEL1 == ChannelID) {
			TMR5DUTY1 = tmr_duty;
		}
		else if (PWM_CHANNEL2 == ChannelID) {
			TMR5DUTY2 = tmr_duty;
		}
	}
}

#endif // PWM_TIMER_EN

