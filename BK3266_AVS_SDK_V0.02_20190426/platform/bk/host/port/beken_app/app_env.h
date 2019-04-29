#ifndef _APP_ENV_H_

#define _APP_ENV_H_

#include <jos.h>
#include "app_hfp.h"
#include "app_aec.h"
#include "app_equ.h"
#include "driver_audio.h"
#if (CONFIG_DRC == 1)
#include "app_drc.h"
#endif


enum
{
    ENV_ERR_PARA = -1,
    ENV_ERR_BUSY = -2
};

#define FLASH_PAGE_LEN                                     256
#define FLASH_PAGE_LEN_CRC                             272
#define ENV_BLOCK_LEN                                       512// 256
#ifndef BEKEN_UP_OTA_3266
#define FLASH_ENVSIZE_DEF_ADDR			 0x114
#define FLASH_ENVSIZE_4M_FLAG			        0x01203636
#define FLASH_ENVSIZE_8M_FLAG			        0x00203636

#define FLASH_ENVDATA2_DEF_ADDR_4M 		(512*1024-0x1000)//calibration data
#define FLASH_ENVDATA2_DEF_ADDR_8M 		(1024*1024-0x1000)

#define FLASH_CHANGED_BT_ADDR_4M		        (FLASH_ENVDATA2_DEF_ADDR_4M-0x1000-0x1000)//name data
#define FLASH_CHANGED_BT_ADDR_8M		        (FLASH_ENVDATA2_DEF_ADDR_8M-0x1000-0x1000)

#define FLASH_ENVCFG_DEF_ADDR_4M		        0x75000 
#define FLASH_ENVCFG_DEF_ADDR_8M		        0xED800 
#endif

enum
{
    ENV_ACTION_DISCONNECT_NONE = 0,
    ENV_ACTION_DISCONNECT_PAGE = 1,
    ENV_ACTION_DISCONNECT_CONN = 2
};

enum
{
    INTER_WAV = 0,
    INTER_SBC,
    EXT_WAV,
    EXT_SBC
};
enum
{
    PROMPT_CLOSED = 0,
    PROMPT_START,
    PROMPT_WORKING,
    PROMPT_FILL_ZERO,
    PROMPT_EMPTY,
    PROMPT_STOP
};
enum {
    TLV_TYPE_CALI_END            = 0,
    TLV_TYPE_CALI_DC_OFFSET_DIFF_DISPGA,
    TLV_TYPE_CALI_DC_OFFSET_SNGL_DISPGA,
    TLV_TYPE_CALI_DC_OFFSET_DIFF_ENPGA,
    TLV_TYPE_CALI_DC_OFFSET_SNGL_ENPGA,
    TLV_TYPE_CALI_SARADC,
    TLV_TYPE_CALI_VOLTAGE,
    TLV_TYPE_CALI_CHARGE
};

typedef struct {
    uint16 type;
    uint16 len;
    uint8  value[];
}__PACKED_POST__ TLV_TYPE;

enum 
{
    ENV_SD_INFO            = 0,
    ENV_UDISK_INFO,
    ENV_MUSIC_VOL_INFO,
    ENV_BT_VOL_INFO,
    ENV_LINE_VOL_INFO,
    ENV_FM_VOL_INF,
};

typedef struct app_mp3_backup_s
{
    uint32          file_index;      // file handle
    uint32          file_size_blks;
    uint32          block_played;
    uint16          dir_index;
}__PACKED_POST__ app_mp3_backup_t;

typedef   struct __app_env_key_pair_s
{
    uint8   used;
    btaddr_t     btaddr;
    uint8    linkkey[16];
    //#ifdef APP_ENV_RESTART_MUSIC_RECOVER
    uint8    crystal_cali_data;//music_paly_pause_flag;
    //#endif
}__PACKED_POST__ app_env_key_pair_t;

//1024*1024-0x1000
typedef   struct _app_program_chg_adc_s
{
    uint16 program_flag;
    uint16 reversed1;
    uint8 chg_flag;
    uint8 chg_val;
    uint8 adc_flag;
    uint16 adc_val;
    uint8 dac_flag;
    int16 dac_L_val;
    int16 dac_R_val;
    uint8   reserved[18];
}__PACKED_POST__ app_program_chg_adc_t;
//app_program_chg_adc_t  env_chg_adc;
typedef struct _env_charge_cali_data_s
{
    uint8 charger_vlcf;
    uint8 charger_icp;
    uint8 charger_vcv;
}__PACKED_POST__  env_charge_cali_data_t;

typedef struct env_saradc_cali_data_s
{
    uint16 sar_adc_dc;
    uint16 sar_adc_4p2;
}__PACKED_POST__  env_saradc_cali_data_t;

typedef struct env_voltage_cali_data_s
{
    uint8   cali_ana_1p8v;
    uint8   cali_dig_1p2v;
}__PACKED_POST__  env_voltage_cali_data_t;

typedef struct env_aud_dc_offset_data_s
{
    uint8   dac_l_dc_offset[32];
    uint8   dac_r_dc_offset[32];
}__PACKED_POST__  env_aud_dc_offset_data_t;

typedef   struct _app_bt_para_s
{
    uint32  bt_flag;

    uint8    device_class;
    int8    device_name[32];
    int8    device_pin[16];
    btaddr_t device_addr;

    int8    disconn_action;
    int32   disconn_start;
    int8    disconn_retry_count;
    int32   disconn_retry_interval;

    int8    auto_conn_count;
    uint16  auto_conn_interval;

    int16  match_timeout;
    uint8   pd_cond;
    uint8   action_disconn;
    uint32   reserved[4];
}__PACKED_POST__ app_bt_para_t;

typedef struct _app_ble_para_s
{
    btaddr_t default_btaddr;
    int8 device_name[32];
    uint16 conn_interval;
    uint32 reserved[2];
}__PACKED_POST__ app_ble_para_t;

#ifdef CONFIG_TWS
#define MAX_KEY_STORE       5
#else
#define MAX_KEY_STORE       6
#endif
//#define MAX_KEY_STORE       6

typedef   struct _app_env_data_s    
{
    uint8 env_tag;
    app_env_key_pair_t  key_pair[MAX_KEY_STORE];
    uint8   key_index;
    btaddr_t default_btaddr;
    uint8 disconnect_reason;//0xff:no disconnect, others defined by hci disconnect reason
    uint8   lang_sel;
    uint8	chg_Vctrl;
    uint32   bt_para_valid_Flag;
//#ifdef APP_ENV_RESTART_MUSIC_RECOVER
    uint8   sw_power_down_flag;
//#endif

#ifdef CONFIG_TWS
    uint8	stereo_role;
    app_env_key_pair_t	 stereo_key;
#endif

    //volume
    uint8 	bt_vol;
    uint8 	hfp_vol;	

    //sd info
    app_mp3_backup_t sd_card;
    //udisk info
    app_mp3_backup_t udisk;
//#ifdef FM_ENABLE
    uint8   channel_valid_flag;
    uint8   fm_vol;
    uint16  freq_deviation_array[50];//MAX_CHANNEL_COUNT  206
    uint16  valid_channel_count;
    uint16  valid_channel_idx;
//#endif
    //uint8   reserved[64]; //size: 254
}__PACKED_POST__ app_env_data_t;    // sizeof(env_data) < 256 *N


typedef   struct _app_wave_info_s
{
    uint8	used;
    uint16	page_index;
}__PACKED_POST__ app_wave_info_t;

#define APP_ENV_SYS_FLAG_UARTDBG    0x00001
#define APP_ENV_SYS_FLAG_IIC        0x00002
#define APP_ENV_SYS_FLAG_SPI        0x00004
#define APP_ENV_SYS_FLAG_LINEIN_ENA 0x00008
#define APP_ENV_SYS_FLAG_LINEIN_HIGH  0x00010
#define APP_ENV_SYS_FLAG_PA_ENABLE	  0x00020
#define APP_ENV_SYS_FLAG_PAMUTE_HIGH  0x00040
#define APP_ENV_SYS_FLAG_DAC_DIFFER   0x00080
#define APP_ENV_SYS_FLAG_L_is_LplusR	0x00100
#define APP_ENV_SYS_FLAG_LED_REVERSE   0x00200
#define APP_ENV_SYS_FLAG_SHADE_OUT      0x00400
#define APP_ENV_SYS_FLAG_BUCK_ENABLE    0x00800
#define APP_ENV_SYS_FLAG_SW_POWERON     0x01000
#define APP_ENV_SYS_FLAG_MIC_BIAS   0x06000
#define APP_ENV_SYS_FLAG_PWRCTRL_HIGH   0x08000

//#define APP_ENV_SYS_FLAG_UART_UPGRATE_ENABLE  	  0x10000
#define APP_ENV_SYS_FLAG_MOS_POWERON		0x20000
#define APP_ENV_SYS_FLAG_SD_DETECT_ENA		0x40000
#define APP_ENV_SYS_FLAG_SD_DET_GPIO_ENA 	0x80000
#define APP_ENV_SYS_FLAG_AD_LINEIN_ENA   	       0x100000
#define APP_ENV_SYS_FLAG_USB_ENA   	              0x200000

#if (SYS_CFG_BUCK_ON == 1)
#define APP_SYSTEM_FLAG_DEFAULT          (APP_ENV_SYS_FLAG_UARTDBG | APP_ENV_SYS_FLAG_DAC_DIFFER) | ((1 << 13) | (1 << 14) | APP_ENV_SYS_FLAG_BUCK_ENABLE)
#else
#define APP_SYSTEM_FLAG_DEFAULT          (APP_ENV_SYS_FLAG_UARTDBG | APP_ENV_SYS_FLAG_DAC_DIFFER) | ((1 << 13) | (1 << 14))
#endif
enum
{
    HW_PWR_KEY = 0,
    SW_PWR_KEY,
    SW_PWR_KEY_SWITCH,
    SW_PWR_KEY_MOS_CTRL,
};

typedef   struct _app_system_para1_s
{
    uint32  system_flag;

    uint8   pamute_pin;
    uint8   linein_pin;
    uint8   wakup_pin;
    uint8	pwrctrl_pin;

    uint8   vol_a2dp;
    uint8   vol_hfp;
    uint8   vol_wave;
    uint8   vol_mic;

    uint8   lp_channel;
    uint32  lp_interval;
    uint16  lp_threshold;
    uint16  lp_pd_threshold;
//#if (CONFIG_CHARGE_EN == 1)
    uint16 charger_trickle_threshold;
    uint16 charger_lc_threshold;
    uint16 charger_cv_threshold;
    uint16 charger_delt;
    uint16 charger_end_threshold;
    uint16 charger_current;
//#endif

    uint16  sleep_timeout;
    int32 powerdown_timeout;
    uint8 upgrade_led;
    uint8 uart_upgrade_enable;
    uint8 dac_adjust_enable;
    uint8 adc_adjust_enable;
    uint8 usbcharge_adjust_enable;
    uint8 dac_adjust_volume;

    uint8 dac_vcom;
    uint8 sdcard_pin;
    uint8 led_reverse_flag;
    uint8 power_valid_flag;
    uint8 mos_soft_power_flag;  // 0. HW switch;  1. Button switch; 2. HW switch; 3. Button switch +GPIO x   MOS
    
    uint8 frq_offset;
    uint8 raw_command[5];
    int32	pause_powerdown_timeout;
    uint32 reserved2[7];

}__PACKED_POST__ app_system_para_t;

#define APP_ENV_BT_FLAG_A2DP     0x00001
#define APP_ENV_BT_FLAG_HFP      0x00002
#define APP_ENV_BT_FLAG_HSP      0x00004
#define APP_ENV_BT_FLAG_AUTO_CONN   0x00010
#define APP_ENV_BT_FLAG_RECOV       0x00020
#define APP_ENV_BT_FLAG_ADDR_POLL        0x00040
#define APP_ENV_BT_FLAG_ADDR_AUDIO_DIAL  0x00080
#define APP_ENV_BT_FLAG_AUTO_CONN_PERIOD  0x00100
#define APP_ENV_BT_FLAG_APP_BAT_DISPLAY   0x00200

#define APP_ENV_BT_PD_FLAG_NOCONN       0x01
#define APP_ENV_BT_PD_FLAG_PAUSE_TO     0x02

#define APP_BT_FLAG_DEFAULT    (APP_ENV_BT_FLAG_AUTO_CONN|APP_ENV_BT_FLAG_A2DP|APP_ENV_BT_FLAG_HFP|APP_ENV_BT_FLAG_ADDR_POLL)

typedef   struct _app_button_para_s
{
    uint16    press;
    uint16    repeat;
    uint16    longp;
    uint16    vlongp;
    uint16    doublep;
}__PACKED_POST__ app_button_para_t;



#if 1

#define APP_ENV_FEATURE_FLAG_INQUIRY_ALWAYS				0x00001
#define APP_ENV_FEATURE_FLAG_DIG_BUCK_ENABLE			0x00002
#define APP_ENV_FEATURE_FLAG_ANA_BUCK_ENABLE			0x00004
#define APP_ENV_FEATURE_FLAG_CHARGE_DET_USE_GPIO		0x00008
#define APP_ENV_FEATURE_FLAG_CHARGE_TIMEOUT_ENABLE		0x00010
#define APP_ENV_FEATURE_FLAG_AUTCONN_TESTER				0x00020
#define APP_ENV_FEATURE_FLAG_CHARGE_MODE_PWR_DOWN		0x00040
#define APP_ENV_FEATURE_FLAG_MIC_SINGLE_ENABLE			0x00080
#define APP_ENV_FEATURE_FLAG_DAC_DC_COMPENSATION		0x00100
#define APP_ENV_FEATURE_FLAG_KEY_LOW_ACTIVE				0x00200
#define APP_ENV_FEATURE_FLAG_DISABLE_IOS_INCOMING_RING	0x00400
#define APP_ENV_FEATURE_FLAG_VOLUME_RESTORE				0x00800
#define APP_ENV_FEATURE_FLAG_VOLUME_SYNC				0x01000
#define APP_ENV_FEATURE_FLAG_PAIRING_AFTER_PWR_ON		0x02000
#define APP_ENV_FEATURE_FLAG_SPP_PROFILE				0x04000
#define APP_ENV_FEATURE_FLAG_HID_PROFILE				0x08000
#define APP_ENV_FEATURE_FLAG_AEC_ENABLE					0x10000
#define APP_ENV_FEATURE_FLAG_DAC_ALWAYS_ON				0x20000
#define APP_ENV_FEATURE_FLAG_VCOM_OUT					0x40000
#define APP_ENV_FEATURE_FLAG_SNIFF_ENABLE				0x80000
#define APP_ENV_FEATURE_FLAG_FAST_MUTE				0x100000
#define APP_ENV_FEATURE_FLAG_AUX_MODE_PWR_DOWN			0x200000
#define APP_ENV_FEATURE_FLAG_AUX_MODE_PAUSE_MUTE		0x400000

typedef   struct _app_volume_s
{
    aud_volume_t vol[17];
//	uint8 dig_vol[16];
//	uint8 ana_vol[16];
}__PACKED_POST__ app_volume_t;

typedef   struct _app_rfpwr_s
{
    uint8 big_adj;//xvr_0x24{bit10:bit7}:
    uint8 small_adj1; //xvr_0x0c{bit26:23}
    uint8 small_adj2; //xvr_0x0c{bit9:6}
}__PACKED_POST__ app_rfpwr_t;

typedef   struct _app_bat_display_s
{
    uint16 bat_level[10];
}__PACKED_POST__ app_bat_display_t;

typedef   struct _app_sw_feature_s
{
    uint32  feature_flag;
    uint16  bt_mode;
    uint8   dig_buck;
    uint8   ana_buck;
    uint8   osc_amp;
    uint8   dac_trimming;
    uint8   charg_det_pin;
    int32   charge_timeout;
    app_volume_t a2dp_vol;
    app_volume_t hfp_vol;
    app_volume_t linein_vol;
    uint8   vol_mic_dig;
    uint8   vol_linein_dig;
    uint8   vol_linein_ana;
    app_rfpwr_t a2dp_rf_pwr;
    app_rfpwr_t hfp_rf_pwr;
    uint16   sw_pwr_on_key_time;
    uint16   sw_pwr_off_key_time;
    uint16   enter_pairing_key_time;
    uint8	pa_mute_delay_time;
    uint8   pa_unmute_delay_time;
    app_bat_display_t bat_display_level;
    btaddr_t tester_bt_addr;
    uint8 reserved[64-6];
}__PACKED_POST__ app_sw_feature_t;
#endif

typedef   struct _app_env_cfg_s
{
    int8 used;
    app_system_para_t system_para;
    app_bt_para_t bt_para;
    app_ble_para_t ble_para;
    uint8 wave_lang_sel;
    uint8_t led_map[LED_NUM];
    app_led_info_t  led_info[LED_EVENT_END];
    app_button_para_t  button_para;
    
    uint32 button_code[BUTTON_BT_END];
    
    int16 eq_gain;
    uint8 eq_flag0;
    app_eq_para_t eq_para[5];
    app_aec_cfg_t env_aec_cfg;
    app_sw_feature_t feature;
    app_wave_info_t wave_info[WAVE_EVENT];
    app_wave_info_t wave_info1[WAVE_EVENT];
    app_wave_info_t wave_info2[WAVE_EVENT];
    app_wave_info_t wave_info3[WAVE_EVENT];
    app_wave_info_t wave_info4[WAVE_EVENT];
	#if (CONFIG_PRE_EQ == 1)
	uint32 pre_eq_gain;
	int8 pre_eq_flag;
	int8 pre_eq_cnt;
	app_eq_para_t pre_eq_para[10];
	#endif
    #if (CONFIG_DRC == 1)
	app_drc_para_t drc;
    #endif
}__PACKED_POST__ app_env_cfg_t;

typedef struct _app_env_s
{
    app_env_data_t  env_data;
    app_env_cfg_t	 env_cfg;
}__PACKED_POST__ app_env_t;

typedef app_env_t * app_env_handle_t;
env_saradc_cali_data_t * app_get_env_saradc_cali_data(void);
env_charge_cali_data_t * app_get_env_charge_cali_data(void);
env_aud_dc_offset_data_t* app_get_env_dc_offset_cali(void);
void app_env_cali_data_init(void);
void app_env_init( void );
void app_env_post_init( void );
app_env_t * app_env_get_handle( void );
int app_env_key_stored( btaddr_t *addr );
int app_env_key_delete( btaddr_t *addr );
int app_env_keypair_used_delete( btaddr_t *addr );
int app_get_env_key_num( void );
int app_get_active_linkey( int Seq );
int app_env_store_autoconn_info( btaddr_t * remote_addr,  uint8 link_key[16] );
btaddr_t* app_env_get_key_free( void );
int app_env_write_action( btaddr_t *raddr,uint8 connected);
void app_env_power_on_check(void);

void app_flash_read_env_data(void);
void app_flash_write_env_data(void);


int app_env_write_flash(uint8 connected);
void app_env_unit_info_init( char *name, uint8 *dev_class, char *pin );
btkey_t * app_env_get_linkkey( btaddr_t *addr );
void app_env_clear_key_info( btaddr_t *addr );
void app_env_clear_all_key_info( void );
int app_env_get_wave_type( int wave_id );
int app_env_get_wave_page_index( int wave_id );
int app_env_get_wave_param( int wave_id, uint32 *param1, uint32 *param2 );
#if (CONFIG_APP_MP3PLAYER == 1)
void app_env_restore(void *arg,uint8 item);
void app_env_backup(void *arg,uint8 item);
void app_evn_wr_flash(void* arg);
void app_env_fm_wr_flash(void *arg);
void app_env_fm_restore(void * buf);

#endif
#ifdef CONFIG_TWS
void app_env_set_stereo_info( btaddr_t *saddr, uint8 link_key[16] );
void app_env_update_stereo_role_info( uint8 role );
int app_get_env_stereo_key_used(void);

#endif

#ifdef BEKEN_DEBUG
void app_env_dump(void);
#endif
//int env_get_auto_conn_info(int start_index, int *id);
//int env_get_auto_conn_info_max_id(void);
#if 1/*(CONFIG_APP_TOOLKIT_5 == 1)*/&&(CONFIG_SW_SWITCH_KEY_POWER == 1)
void app_env_pwrctrl_by_switch(void);
#endif
#if 1//(CONFIG_APP_TOOLKIT_5 == 1)
RAM_CODE int app_env_check_feature_flag(uint32 flag);
RAM_CODE int app_env_check_inquiry_always(void);
RAM_CODE int app_env_check_SPP_profile_enable(void);
RAM_CODE int app_env_check_HID_profile_enable(void);
RAM_CODE void app_env_rf_pwr_set(uint8 type);
RAM_CODE int app_env_check_pwrCtrl_mode(void);
RAM_CODE int app_env_check_DAC_POP_handle_enable(void);
RAM_CODE int app_env_check_Charge_Mode_PwrDown(void);
RAM_CODE int app_env_check_Charge_Det_use_gpio(void);
RAM_CODE int app_env_check_Charge_Time_Out_Enable(void);
RAM_CODE int app_env_check_sniff_mode_Enable(void);
RAM_CODE int app_env_check_Use_ext_PA(void);
RAM_CODE int app_env_check_bat_display(void);

#endif

#endif
