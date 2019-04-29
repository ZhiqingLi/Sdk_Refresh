#ifndef _DRIVER_SARADC_H_
#define _DRIVER_SARADC_H_

//#define SADC_DEBUG

#ifdef SADC_DEBUG
    #define SADC_PRT      os_printf
#else
    #define SADC_PRT      os_null_printf
#endif

//#define PATCH_DISCARD_FIRST_ADC_VALUE

#define SARADC_CHARGER_THRESHOLD_COUNT        3
#define SARADC_MODE_SLEEP       0
#define SARADC_MODE_SINGLESTEP  1
#define SARADC_MODE_SOFTWARE    2
#define SARADC_MODE_CONTINUE    3


#define SARADC_FIFO_EMPTY    (1 << 6 )
#define SARADC_ADC_BUSY      (1 << 7 )

#define SARADC_ADC_SAMPLE_DIV36   (0 << 8)
#define SARADC_ADC_SAMPLE_DIV18   (1 << 8)

#define SARADC_START_DELAY      (1 << 10)

#define SARADC_CLK_DIV4         0
#define SARADC_CLK_DIV8         1
#define SARADC_CLK_DIV16        2
#define SARADC_CLK_DIV32        3
#define SARADC_CLK_DIV64        4
#define SARADC_CLK_DIV128       5
#define SARADC_CLK_DIV256       6
#define SARADC_CLK_DIV512       7

#define sft_SARADC_CHANNEL             3
#define bit_SARADC_DELAY              10
#define bit_SARADC_CLKDIV           16


#define SARADC_BELOW_COUNT      5
#define SARADC_BELOW_THRESHOLD  3000//0x1d0  // about 2.6V
 
#define SARADC_BIGC_THRESHOLD   4000//0x210 4.0V
#define SARADC_FULL_THRESHOLD   4200//0x2D0 4.2V

#define SARADC_CALI_THRESHOLD   10


#define SARADC_TRK_THRESHOLD   30
#define SARADC_LC_THRESHOLD    38
#define SARADC_CV_THRESHOLD    41
#define SARADC_DELT            2
#define SARADC_END_THRESHOLD   43
#define CHARGE_CURRENT          4

enum
{
    SARADC_CHARGER_MODE_LC = 0,
    SARADC_CHARGER_MODE_BC = 1,
    SARADC_CHARGER_MODE_ALMOST_FULL = 2,
    SARADC_CHARGER_MODE_NONE = 3
};

void saradc_init( int mode, int channel, int div );
int saradc_lowpower_status( void );
int saradc_normalpower_status( void );
int saradc_set_lowpower_para( int interval, int threshold_lowpower, int threshold_powerdown );
void saradc_isr( void );
void saradc_reset( void );
uint16 saradc_get_value(void);//int saradc_get_value(void);
uint16 saradc_get_key_value(void);
void saradc_calibration_first(void);
void saradc_chnl_scanning(uint32 step);
uint16 sdradc_transf_adc_to_vol(uint16 data);

#endif
