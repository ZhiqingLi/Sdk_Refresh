#include "driver_beken_includes.h"
#include "app_beken_includes.h"

#define SARADC_DUMP		5				//每次切换ADC通道后的去抖次数
static uint8 saradcDebounce;
static uint16 saradc_value = 0;
static uint16 saradcKeyValue = 0xffff;
//static uint32 saradcBatteryValueSum = 0;
//static uint8 saradcValueCnt = 0;
static uint16 saradc_chn = 0;

//static uint16 saradc_threshold = SARADC_BELOW_THRESHOLD + 100;
//static uint16 saradc_threshold_pd = SARADC_BELOW_THRESHOLD;
static uint16 saradc_threshold = 688;
static uint16 saradc_threshold_pd = 628;
static uint16 saradc_below_count = SARADC_BELOW_COUNT;
static boolean s_saradc_1st_calibration = FALSE;
static uint16 s_saradc_1st_read_times = 0;
static uint16 low_bat_cnt = 0;
static uint16 low_bat_pd_cnt = 0;

uint8  saradc_start = 0;
uint16 bat_adc_temp = 0;
uint16 batChannelCnt=0;
uint16 keyChannelCnt=0;

void saradc_calibration_first(void)
{
    uint32 reg = 0,i;
    BK3000_A0_CONFIG   |= (0x01<<13);   
    //BK3000_A0_CONFIG   |= (0x01<<6);    // saradc calibration enable;

    BK3000_PMU_PERI_PWDS &= ~bit_PMU_ADC_PWD;
    BK3000_PMU_PERI_PWDS &= ~( 1 << 18 ); // xtal;
    //BK3000_PMU_PERI_PWDS |= PMU_ADC_CLK_SEL_ROSC; // warning!!!
    for(i=0;i<16;i++)
    {
        BK3000_SARADC_ADC_CONF = 0;  /* Try to reset SARADC ??? */
        saradc_chn = 0;
        reg = SARADC_MODE_SINGLESTEP |(saradc_chn << sft_SARADC_CHANNEL)|(3 << bit_SARADC_CLKDIV);
        BK3000_SARADC_ADC_CONF = reg;
        delay_us(1000);
        saradc_chn = BK3000_SARADC_ADC_DATA & 0x3ff;
        saradc_chn = BK3000_SARADC_ADC_DATA & 0x3ff;
        saradc_chn = BK3000_SARADC_ADC_DATA & 0x3ff;
        saradc_chn = BK3000_SARADC_ADC_DATA & 0x3ff;
        saradc_chn = BK3000_SARADC_ADC_DATA & 0x3ff;
        saradc_chn = BK3000_SARADC_ADC_DATA & 0x3ff;
    }
	saradc_start = 1;
}

uint16 saradc_get_value(void)
{
    return bat_adc_temp;
}

uint16 saradc_get_key_value(void)
{
	return saradcKeyValue;
}
void saradc_reset( void )
{

    SADC_PRT("saradc_reset\r\n");
}

void saradc_refer_select(uint8 mode)
{
    if(mode)
        BK3000_A0_CONFIG |= (0x01 << 13);   //saradc reference select: vbg1p2
    else
        BK3000_A0_CONFIG &= ~(0x01 << 13);  //saradc reference select: vdd/2
}

void saradc_init( int mode, int channel, int div )
{
    uint32 reg;
    if(!s_saradc_1st_calibration)
        return;

    saradc_chn = channel;
    saradcDebounce = SARADC_DUMP+5;
    batChannelCnt=0;
    keyChannelCnt=0;
    BK3000_A0_CONFIG   |= (0x01 << 13); 
    BK3000_A0_CONFIG   &= ~(0x01 << 6);
    BK3000_PMU_PERI_PWDS &= ~bit_PMU_ADC_PWD;
    BK3000_PMU_PERI_PWDS &= ~( 1 << 18 );  // xtal 
    //BK3000_PMU_PERI_PWDS |= PMU_ADC_CLK_SEL_ROSC; // warning!!!
   /* Try to reset SARADC ??? */
    BK3000_SARADC_ADC_CONF = 0;
    

    if(ADC_CHANNEL_1 == channel)
        BK3000_GPIO_6_CONFIG = 0x08; //0x0C
    else if(ADC_CHANNEL_2 == channel)
        BK3000_GPIO_7_CONFIG = 0x08; //0x0C
    else if(ADC_CHANNEL_4 == channel)
        BK3000_GPIO_18_CONFIG = 0x08; //0x0C
    else if(ADC_CHANNEL_6 == channel)
        BK3000_GPIO_20_CONFIG = 0x08; //0x0C
    else if(ADC_CHANNEL_7 == channel)
        BK3000_GPIO_21_CONFIG = 0x08; //0x0C 

    reg =  mode
            |(channel << sft_SARADC_CHANNEL)
            |(div << bit_SARADC_CLKDIV);

    BK3000_SARADC_ADC_CONF = reg;
    saradc_start = 1;
}

int saradc_set_lowpower_para( int interval, int threshold_lowpower, int threshold_powerdown )
{
    //SADC_PRT("set_lowpower_para:%d:%d:%d\r\n", interval, threshold_lowpower, threshold_powerdown);
    os_printf("set_lowpower_para:%d:%d:%d\r\n", interval, threshold_lowpower, threshold_powerdown);
    saradc_threshold = threshold_lowpower;
    saradc_threshold_pd = threshold_powerdown;

    return 0;
}
static uint16 value_buf[5] ={0};
static uint16 bat_adc_filter(uint16 data)
{
    unsigned char i = 0;
    uint16 sum = 0;
    uint16 value_avr_adc =0;
    uint16 value_adc_max = 0;
    uint16 value_adc_min = 0;
    value_buf[4] = data;
    for(i = 0; i < 4; i++)
        value_buf[i] = value_buf[i+1];
    value_adc_max = value_adc_min = value_buf[0];
    for(i = 0; i < 4; i++)
    {    
        value_adc_max =MAX(value_buf[i],value_adc_max);
        value_adc_min = MIN(value_buf[i],value_adc_min);
	    sum += value_buf[i];
    }
    //os_printf("max:%d,min:%d\r\n",value_adc_max,value_adc_min);
    value_avr_adc = (sum-(value_adc_max + value_adc_min))>> 1;
	
	
    if((value_avr_adc < 3000)||(value_avr_adc > 9000))
    {
        for(i = 0; i < 4; i++)
            value_buf[i] = value_buf[4];	
	//    os_printf("++adc:%d(mV),adc_avr:%d(mV)\r\n",value_buf[4],value_avr_adc);
	    return value_buf[4];
    }
    //os_printf("+++adc:%d(mV),adc_avr:%d(mV)\r\n",value_buf[4],value_avr_adc);
    return value_avr_adc;
}

uint16 sdradc_transf_adc_to_vol(uint16 data)
{
    uint16 v_data;
    env_saradc_cali_data_t *saradc_cali = app_get_env_saradc_cali_data();
    v_data = 4200 * (data-(saradc_cali->sar_adc_dc-511)) / (saradc_cali->sar_adc_4p2 - (saradc_cali->sar_adc_dc-511));
	return v_data;
}

int saradc_lowpower_status(void)
{
     app_env_handle_t  env_h = app_env_get_handle();
    static uint8 bat_adc_filter_cnt = 0;
    env_saradc_cali_data_t *saradc_cali = app_get_env_saradc_cali_data();
    uint16 data;
    uint16 v_data;
    static uint8 saradc_count=0;
    v_data = 4200 * (bat_adc_temp-(saradc_cali->sar_adc_dc-511)) / (saradc_cali->sar_adc_4p2 - (saradc_cali->sar_adc_dc-511));
    data = bat_adc_filter(v_data);

    saradc_start = 0;
    /* ADC上电稳定的去抖 */
    if (bat_adc_filter_cnt < 3)
    {
        bat_adc_filter_cnt++;
        return 0;       
    }
 //   os_printf("bat_adc_temp=%d,data=%d,saradc_threshold_pd=%d,saradc_threshold=%d\r\n",bat_adc_temp,data,saradc_threshold_pd,saradc_threshold);

    if (data < saradc_threshold_pd)
    {
            low_bat_pd_cnt++;
             low_bat_cnt++;
    }   
    else if ((data < saradc_threshold) && (data >= saradc_threshold_pd))
    {
            low_bat_cnt++;
         if (low_bat_pd_cnt)
            low_bat_pd_cnt--;
    }
    else
    {
        if (low_bat_cnt)
            low_bat_cnt--;
        if (low_bat_pd_cnt)
            low_bat_pd_cnt--;
    }
    /**********************************/
    ++saradc_count;
    if(saradc_count==saradc_below_count)
    {
        //os_printf("+++bat_avr:%d(mV)\r\n",data);
        saradc_count=0;
        if (low_bat_pd_cnt >= saradc_below_count)
        {
            saradc_below_count = env_h->env_cfg.system_para.lp_interval/200;	// 2s检测一次的倍数
            return 2;
        }
        else if(low_bat_cnt >= saradc_below_count)
        {
            saradc_below_count = env_h->env_cfg.system_para.lp_interval/200;	// 2s检测一次的倍数
            return 1;
        }
        else 
        {
            saradc_below_count = SARADC_BELOW_COUNT;
            return 0;
        }
        low_bat_cnt=0;
        low_bat_pd_cnt=0;
    }
    else
    {
        return 3;
    }
 
}

void saradc_chnl_scanning(uint32 step)
{
    app_handle_t app_h = app_get_sys_handler();
    uint16 data,data2;
    static uint16 saradcBatAverage=0;
    static uint16 saradc_det_cnt=0;
    static uint8 saradc_bat_cnt=0;
    
    if(!s_saradc_1st_calibration)
        return;

    if(!(BK3000_SARADC_ADC_CONF&SARADC_FIFO_EMPTY))
    {
        data = BK3000_SARADC_ADC_DATA & 0x3ff; 
        data2 = BK3000_SARADC_ADC_DATA & 0x3ff; 
        if(app_h->button_ad_channel == saradc_chn)
        {
            saradcKeyValue = data;
           
        }
        else if((app_h->button_ad_channel==ADC_CHANNEL_NULL)||( app_h->low_detect_channel == saradc_chn))  
        {
        
           // if(data > 512) //battery det other channel,not channel0 may < 512. xugaojing 20180106
            {
                saradcBatAverage += data;
                saradc_bat_cnt++;
                if(saradc_bat_cnt >= 4)
                {
                    saradc_bat_cnt=0;
                    saradcBatAverage >>= 2; 
                    bat_adc_temp = saradcBatAverage;
                    saradcBatAverage = 0;
                }
            }          
        }
        saradc_start = 0;
    }
    saradc_det_cnt += step;
    if(saradc_det_cnt > 100) // timer:1s
    {// BAT DET
        saradc_det_cnt = 0;
        saradc_init(SARADC_MODE_SINGLESTEP, app_h->low_detect_channel,4);
    }
    else if(ADC_CHANNEL_NULL != app_h->button_ad_channel) //timer:10ms
    {//ADC KEY DET
        saradc_init(SARADC_MODE_SINGLESTEP,app_h->button_ad_channel,4);
        saradc_refer_select(0);
    }
}

void saradc_isr( void )
{
    uint16 data;
    //uint32 old_mask = get_spr(SPR_VICMR(0));

    //set_spr( SPR_VICMR(0), old_mask & (1 << VIC_AUD_ISR_INDEX));
    //cpu_set_interrupts_enabled(1);

#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
    Open_26M_Clock();
#endif
    
    BK3000_SARADC_ADC_CONF |= ( 1 << 15 ); /*  clear interrupt; */

    if(!s_saradc_1st_calibration)
    {
    	data = BK3000_SARADC_ADC_DATA & 0x3ff; /*  dummy read for cleaning up FIFO data; */
    	data = BK3000_SARADC_ADC_DATA & 0x3ff; 
        s_saradc_1st_read_times++;
        if(s_saradc_1st_read_times > 12)
        {
            s_saradc_1st_calibration = TRUE;
            saradc_value = data;
            bat_adc_temp = data;
            os_printf("===saradc calibration finished:%d!\r\n",data);
            /*  dummy read for cleaning up FIFO data; */
            while(!(BK3000_SARADC_ADC_CONF & (1<<6)))
            {
                data = BK3000_SARADC_ADC_DATA & 0x3ff;
            }
            /* disable saradc calibration */
            BK3000_SARADC_ADC_CONF = 0;
            BK3000_A0_CONFIG &= ~(1<<13);
            BK3000_A0_CONFIG &= ~(1<<6);
            BK3000_PMU_PERI_PWDS |= bit_PMU_ADC_PWD;
        }
        else
        {
            saradc_calibration_first();
        }
    }
    else
    {
        if(saradcDebounce)
        {
           saradcDebounce--;       // ignore 4 times saradc value,after changed saradc channel. 
        }
        else
        {
            /* power down SARADC */
            BK3000_SARADC_ADC_CONF = 0;
            BK3000_A0_CONFIG &= ~(1<<13);
            BK3000_PMU_PERI_PWDS |= bit_PMU_ADC_PWD;           
        }
    }    
    //cpu_set_interrupts_enabled(0);
    //set_spr( SPR_VICMR(0), old_mask );
    (void)data;
}
