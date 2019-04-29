#ifndef _DRIVER_ICU_H_
#define _DRIVER_ICU_H_

#define CLEAR_BIT(reg,count,...) clear_bit(&(reg),count,##__VA_ARGS__)
#define SET_BIT(reg,count,...) set_bit(&(reg),count,##__VA_ARGS__)

typedef enum
{
    POWERDOWN_SELECT             = 0,
    POWERDOWN_CHG_DEEPSLEEP      = 1,
    POWERDOWN_SHUTDOWN           = 2
}t_powerdown_mode;


RAM_CODE void BK3000_Ana_Decrease_Current(u_int8 enable);
RAM_CODE void Close_26M_Clock(void);
RAM_CODE void Open_26M_Clock(void);
RAM_CODE uint8 bk3000_set_afc_disable(void);

void BK3000_ICU_Initial(void);
void BK3000_icu_sw_powerdown(uint8  wakup_pin,t_powerdown_mode pwrdown_mode);
void BK3000_start_wdt(uint32 val);
void BK3000_wdt_power_on(void);
void BK3000_stop_wdt(void);
void BK3000_wdt_reset(void);
void BK3000_Dig_Buck_open(char enable );
void BK3000_ana_buck_enable(char enable );
void BK3000_set_ana_dig_voltage(uint8 ana,uint8 dig);

int  BK3000_wdt_flag( void );
void BK3000_Ana_Dac_enable(uint8 enable );
void BK3000_Ana_PLL_enable( uint16 freq );

#ifdef CONFIG_BLUETOOTH_HFP
void BK3000_Ana_Adc_enable( uint8 enable );
#endif

void BK3000_Ana_Line_enable( uint8 enable );
//void BK3000_Ana_Dac_clk_adjust( int mode );
void VICMR_usb_chief_intr_enable(void);
void VICMR_usb_chief_intr_disable(void);
void _audio_isr_dispatch(void);
void ba22_disable_intr_exception(void);
void ba22_enable_intr_exception(void);

DRAM_CODE void VICMR_disable_interrupts(uint32 *interrupts_info_ptr, uint32 *);
DRAM_CODE void VICMR_restore_interrupts(uint32 interrupts_info, uint32);
void enable_audio_ldo(void);
void clear_sco_connection(void);
void clear_music_play(void);
//void ldo_enable(void);
//void ldo_disable(void);
void enable_audio_ldo_for_music_files(void);
void clear_wave_playing(void);
//RAM_CODE unsigned char BK3000_hfp_set_powercontrol(void);
#ifdef	WROK_AROUND_DCACHE_BUG
void app_Dcache_disable(void);
void app_Dcache_enable(void);
void app_Dcache_initial(void);
#endif

#ifdef CONFIG_PRODUCT_TEST_INF
extern uint8 aver_rssi;
extern int16 aver_offset;
inline void get_freqoffset_rssi(void);
void average_freqoffset_rssi(void);
#endif
RAM_CODE uint8 bk3000_set_afc_disable(void);

#endif
