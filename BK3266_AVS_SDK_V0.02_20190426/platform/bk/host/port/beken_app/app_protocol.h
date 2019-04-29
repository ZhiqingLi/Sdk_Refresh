#ifndef _APP_PROTOCOL_H_
#define _APP_PROTOCOL_H_

#include "app_button.h"

enum
{
    LED_NO_BLINK = 0,
    LED_BLINK_FAST  = 1,
    LED_BLINK_SLOW  = 2,
    LED_BLINK_VERY_FAST = 3,
    LED_BLINK_VERY_SLOW = 4
};

typedef uint8_t APP_LED_MODE;

#define WAKEUP_GPIO                   (15)
#define LED_NUM                        3

#define LED0_INDEX                     16
#define LED1_INDEX                     15
#define LED_BT                         16
#define LED_AUX                        15

#define BTN_CODE(action, gpio, type) ((1 << (gpio-GPIO0)) + (action) + (type))

enum
{
	BUTTON_NONE = 0,
	BUTTON_BT_PLAY_PAUSE,
	BUTTON_BT_NEXT,
	BUTTON_BT_PREV,
	BUTTON_BT_REWIND,
	BUTTON_BT_FORWARD,
	BUTTON_BT_VOL_P,
	BUTTON_BT_VOL_M,
	BUTTON_BT_VOL_MUTE,
	BUTTON_BT_MATCH,
	BUTTON_BT_POWERDOWN, // soft power down
	BUTTON_BT_HFP_ACK,
	BUTTON_BT_HFP_NACK,
	BUTTON_BT_HFP_DIAL,
	BUTTON_BT_MIC_P,
	BUTTON_BT_MIC_M,
	BUTTON_BT_MIC_MUTE,
	BUTTON_BT_VOICE_DIAL_INIT,
	BUTTON_BT_TRANSFER_TOGGLE,
	BUTTON_BT_CONN_DISCONN,
	BUTTON_BT_STOP,
	BUTTON_BT_CLEAR_MEMORY,
	BUTTON_BT_TEST_MODE,
	BUTTON_BT_LANG_CHANGE,
	BUTTON_BT_TWC_HOLD_ACCEPT,
	BUTTON_BT_TWC_HUNG_ACCEPT,
	BUTTON_BT_TWC_REJECT_HOLD,

	BUTTON_BT_TWC_SWTCH_WAY,
	BUTTON_BT_2PHONES_SWTCH,
	BUTTON_MODE,
	BUTTON_RESERVED1,
	BUTTON_RESERVED2,
	BUTTON_RESERVED3,
	BUTTON_BT_END
};
typedef enum
{
    BATTERY_CHARGE_INACTIVE = 0,
    BATTERY_CHARGING,
	BATTERY_CHARGE_FULL,
    BATTERY_CHARGE_FINISHED,
    BATTERY_CHARGE_ABORTED
}t_battery_charge_state;
typedef uint32_t APP_BUTTON_MODE;

#define LED_BLINK_FAST_COUNT_THRE         30 // 300ms
#define LED_BLINK_SLOW_COUNT_THRE         100 // 1s
#define LED_BLINK_VERY_SLOW_COUNT_THRE    200
#define LED_BLINK_VERY_FAST_COUNT_THRE    15

#define BUTTON_PRESS_COUNT_THRE              5
#define BUTTON_PRESS_REPEAT_THRE             30
#define BUTTON_LONG_PRESS_COUNT_THRE         100
#define BUTTON_LONG_PRESS_COUNT_THRE2        500
#define BUTTON_DOUBLE_CLICK_WAIT_COUNT_THRE  40

#define APP_LOWPOWER_DETECT_INTERVAL         1000   // 30s

#define LED_ON                               1
#define LED_OFF                              0

#define LED_INVALID_INDEX                    0xFF

#define AUTO_RE_CONNECT_SCHED_DELAY_HFP				311
#define AUTO_RE_CONNECT_SCHED_DELAY_A2DP_SWITCH		112
#define AUTO_RE_CONNECT_SCHED_DELAY_A2DP_NOSWITCH	102
#define AUTO_RE_CONNECT_SCHED_DELAY_AVRCP			103
#define AUTO_RE_CONNECT_SCHED_DELAY_AVDTP			104


enum
{
    LED_EVENT_AUTO_CONNECTION = 0,
    LED_EVENT_MATCH_STATE,
    LED_EVENT_NO_MATCH_NO_CONN_STATE,
    LED_EVENT_CONN,
    LED_EVENT_BT_PAUSE,
    LED_EVENT_BT_PLAY,
    LED_EVENT_INCOMING_RINGING,
    LED_EVENT_OUTGOING_RINGING,
    LED_EVENT_CALL_ESTABLISHMENT,
    LED_EVENT_LOW_BATTERY,
    LED_EVENT_CHARGING,
    LED_EVENT_BATTERY_FULL,
    LED_EVENT_POWER_ON,
    LED_EVENT_POWER_OFF,
    LED_EVENT_LINEIN_PLAY,
    LED_EVENT_LINEIN_PAUSE,
    LED_EVENT_MUSIC_PLAY,
    LED_EVENT_MUSIC_PAUSE,
    LED_EVENT_FM_PLAY,
    LED_EVENT_FM_PAUSE,
    LED_EVENT_FM_SCAN,
    LED_EVENT_TEST_MODE,
    LED_EVENT_STEREO_CONN_MODE,
    LED_EVENT_STEREO_MATCH_MODE,
    LED_EVENT_STEREO_RECON_MODE,
    LED_EVENT_RESERVED1,
    LED_EVENT_RESERVED2,
    LED_EVENT_RESERVED3,
    LED_EVENT_END
};
#define LED_EVENT_PAUSE LED_EVENT_BT_PAUSE
#define LED_EVENT_PLAY LED_EVENT_BT_PLAY
#define LED_EVENT_LINEIN LED_EVENT_LINEIN_PLAY
typedef uint8_t APP_LED_EVENT;


typedef   struct _app_led_info_s
{
    uint8 index;
    uint16 led_ontime;
    uint16 led_offtime;
    uint8 repeat;
    uint8 number_flash;
    int32 timeout;
}__PACKED_POST__ app_led_info_t;

#define APP_BUTTON_FLAG_BT_ENABLE           0x00000001
#define APP_FLAG_MUSIC_PLAY                         0x00000002
#define APP_BUTTON_FLAG_PLAY_PAUSE          0x00000004
#define APP_FLAG_AUTO_CONNECTION                0x00000008

#define APP_FLAG_AVCRP_CONNECTION           0x00000010
#define APP_FLAG_AVCRP_PROCESSING           0x00000020
#define APP_FLAG_A2DP_CONNECTION            0x00000040
#define APP_FLAG_ACL_CONNECTION             0x00000080

#define APP_FLAG_RECONNCTION		        0x00000100
#define APP_FLAG_MATCH_ENABLE               0x00000200
#define APP_FLAG_DUT_MODE_ENABLE            0x00000400
#define APP_FLAG_HFP_CONNECTION             0x00000800

#define APP_FLAG_HSP_CONNECTION             0x00001000
#define APP_FLAG_POWERDOWN                  0x00002000
#define APP_FLAG_HFP_CALLSETUP              0x00004000 // 电话建立中
#define APP_FLAG_SCO_CONNECTION             0x00008000

#define APP_FLAG_FIRST_ENCRYP               0x00010000
#define APP_FLAG_HFP_OUTGOING               0x00020000
#define APP_FLAG_ROLE_SWITCH                0x00040000
#define APP_FLAG_WAVE_PLAYING               0x00080000

#define APP_FLAG_MUSIC_PLAY_SCHEDULE        0x00100000
#define APP_FLAG_HID_CONNECTION                0x00200000
#define APP_FLAG_LINEIN                                    0x00400000
#define APP_FLAG_LOWPOWER                             0x00800000

#define APP_FLAG_HFP_BAT_LEVEL                     0x01000000
#define APP_FLAG_SNIFF_MODE_CHANGE           0x02000000
#define APP_FLAG_MODE_CHANGE_SETTING      0x04000000
#define APP_FLAG_FCC_MODE_ENABLE		       0x08000000

#define FLAG_HFP_AUTOCONN_DISCONNED 	       0x10000000 // hfp deny
#define APP_FLAG_CALL_ESTABLISHED      	       0x20000000  // 电话已建立
#define APP_FLAG_DISCONN_RECONN        	       0x40000000
#define APP_FLAG_AVRCP_FLAG         	              0x80000000  // old name:APP_FLAG_DEBUG_FLAG

#define APP_FLAG2_LINEIN_SD_PLAYING		0x00000001
#define APP_FLAG2_VOL_MUTE                            0x00000002
#define APP_FLAG2_HFP_INCOMING      		0x00000004    //an incoming call exist(call not exist)
#define APP_FLAG2_MUTE_FUNC_MUTE    		0x00000008

#define APP_FLAG2_LED_LOWPOWER      		0x00000010
//#define APP_FLAG2_DAC_OPEN          		       0x00000020
#define APP_FLAG2_5S_PRESS			              0x00000020
#define APP_FLAG2_VOL_SYNC_OK                      0x00000040
#define APP_FLAG2_BATTERY_FULL			0x00000080

//#define APP_FLAG2_CHARGING	             	        0x00000100
#define APP_FLAG2_CHARGE_POWERDOWN	        0x00000100
#define APP_FLAG2_CALL_PROCESS           	        0x00000200
#define APP_FLAG2_RECONN_AFTER_CALL      	 0x00000400
#define APP_FLAG2_RECONN_AFTER_DISCONNEC 	0x00000800

#define APP_FLAG2_HFP_SECOND_CALLSETUP   	0x00001000
#define APP_FLAG2_MATCH_BUTTON           	       0x00002000
#define APP_FLAG2_WAKEUP_DLY			 	0x00004000
#define APP_FLAG2_RECONN_AFTER_PLAY     	0x00008000

#define APP_FLAG2_VOL_SYNC			 		0x00010000

//#ifdef CONN_WITH_MUSIC
#define APP_FLAG2_SW_MUTE			 		0x00020000
//#endif

#if 1/*(CONFIG_APP_TOOLKIT_5 == 1)*/||defined(CONFIG_TWS)
#define AP_FLAG2_LINEIN_MUTE				0x00040000
#define APP_FLAG2_STEREO_STREAMING        	0x00800000 //0x00040000
#define APP_FLAG2_STEREO_AUTOCONN         	0x00080000
//#define APP_FLAG2_AVRCP_OPCODE_PLAY         0x00800000

#define APP_FLAG2_STEREO_AUTOCONN_RETRY   	0x00100000
#define APP_FLAG2_STEREO_BUTTON_PRESS     	  0x00200000
#define APP_FLAG2_STEREO_MATCH_BUTTON       0x00400000
//for APP_FLAG2_TWC_HOLD_CALLID				0x00800000

#define APP_FLAG2_SYNC   				 	0x01000000
#define APP_FLAG2_STEREO_INQUIRY_RES    	0x02000000
#define APP_FLAG2_STEREO_ROLE_MASTER    	0x04000000
#define APP_FLAG2_STEREO_WORK_MODE      	0x08000000

#define APP_FLAG2_STEREO_CONNECTTING    	    0x10000000
//#define APP_FLAG2_STEREO_SLAVE_ADJ_CLK   	    0x20000000
#define APP_FLAG2_STEREO_PLAYING   		    0x40000000
#define APP_FLAG2_STEREO_SLAVE_GET_TIME      0x80000000
#define APP_FLAG2_STEREO_LINEIN_TX   	           0x20000000



#define APP_STEREO_FLAG_SET    (APP_FLAG2_STEREO_WORK_MODE \
            |APP_FLAG2_STEREO_CONNECTTING|APP_FLAG2_STEREO_INQUIRY_RES|APP_FLAG2_STEREO_STREAMING)
//|APP_FLAG2_STEREO_ROLE_MASTER
#define APP_BT_INQUIRY_TICKS			5
#define APP_BT_INQUIRY_DURATION 	6
#endif


//--------------------profile private flag--------------------------
//---a2dp flag----
#define APP_A2DP_FLAG_STREAM_START              0x01
#define APP_A2DP_FLAG_MUSIC_PLAY                APP_FLAG_MUSIC_PLAY          //0x02
#define APP_A2DP_FLAG_PLAY_PAUSE                APP_BUTTON_FLAG_PLAY_PAUSE   //0x04

#define APP_A2DP_FLAG_A2DP_CONNECTION           APP_FLAG_A2DP_CONNECTION     //0x40
#define APP_A2DP_PRIVATE_FLAG_FLOW_CTRL			0x08
//hfp flag
#define APP_HFP_PRIVATE_FLAG_AG_IN_BAND_RING    0x00000100
#define APP_HFP_PRIVATE_FLAG_AT_CMD_PENDING     0x00400000
#define APP_HFP_PRIVATE_FLAG_TWC_MODE           0x00800000
#define APP_HFP_PRIVATE_FLAG_TWC_WAIT_STATE     0x02000000
#define APP_HFP_PRIVATE_FLAG_CODEC_NEGOTIATION  0x04000000
//#define APP_HFP_PRIVATE_FLAG_ALERTING_STATE   0x0040000
#define APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE1     0x00000010
#define APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE2     0x00000020

//avrcp flag
//sco flag
#define APP_SCO_PRIVATE_FLAG_CONNECTION				APP_FLAG_SCO_CONNECTION		
#define APP_SCO_PRIVATE_FLAG_CONNECTION_ACTIVE		0x08

//--------------------profile private flag--------------------------
#define APP_AUDIO_FLAG_SET      (APP_FLAG_MUSIC_PLAY\
                                   |APP_FLAG_AVCRP_CONNECTION\
                                   |APP_FLAG_AVCRP_PROCESSING\
                                   |APP_FLAG_A2DP_CONNECTION\
                                   |APP_FLAG_ACL_CONNECTION\
                                   |APP_FLAG_HFP_CONNECTION\
                                   |APP_FLAG_HSP_CONNECTION\
                                   |APP_FLAG_HFP_CALLSETUP\
                                   |APP_FLAG_SCO_CONNECTION\
                                   |APP_FLAG_FIRST_ENCRYP\
                                   |APP_FLAG_ROLE_SWITCH)
#define APP_AUDIO_WORKING_FLAG_SET  (APP_FLAG_MUSIC_PLAY \
                                    |APP_BUTTON_FLAG_PLAY_PAUSE\
                                    |APP_FLAG_SCO_CONNECTION\
                                    |APP_FLAG_WAVE_PLAYING\
                                    |APP_FLAG_AUTO_CONNECTION\
                                    |APP_FLAG_LINEIN\
                                    |APP_FLAG_RECONNCTION)

#define APP_HFP_AT_CMD_FLAG1_SET    (APP_FLAG_HFP_CALLSETUP \
                                    |APP_FLAG_HFP_OUTGOING\
                                    |APP_FLAG_CALL_ESTABLISHED)

#define APP_HFP_AT_CMD_FLAG2_SET    (APP_FLAG2_HFP_INCOMING \
                                    |APP_FLAG2_CALL_PROCESS\
                                    |APP_FLAG2_HFP_SECOND_CALLSETUP)

#define APP_HFP_PRIVATE_FLAG_SET    (APP_HFP_PRIVATE_FLAG_AT_CMD_PENDING \
                                    |APP_HFP_PRIVATE_FLAG_AG_IN_BAND_RING \
                                    |APP_HFP_PRIVATE_FLAG_CODEC_NEGOTIATION \
                                    |APP_HFP_PRIVATE_FLAG_TWC_WAIT_STATE \
                                    |APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE1 \
                                    |APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE2 \
                                    |APP_HFP_PRIVATE_FLAG_TWC_MODE)

#define APP_SDP_DISABLE_FLAG_SET  (APP_FLAG_MUSIC_PLAY|APP_FLAG_A2DP_CONNECTION|APP_FLAG_HFP_CONNECTION)

#define APP_MATCH_FLAG_SET    (APP_FLAG_MUSIC_PLAY|APP_FLAG_AVCRP_CONNECTION\
                                   |APP_FLAG_AVCRP_PROCESSING|APP_FLAG_ACL_CONNECTION\
                                   |APP_FLAG_HFP_CALLSETUP|APP_FLAG_SCO_CONNECTION|\
                                   APP_FLAG_FIRST_ENCRYP|APP_FLAG_ROLE_SWITCH)

#define APP_CPU_NOT_HALT_FLAG_SET ( APP_FLAG_FCC_MODE_ENABLE\
                                    |APP_FLAG_AUTO_CONNECTION\
                                    |APP_FLAG_RECONNCTION\
                                    |APP_FLAG_DUT_MODE_ENABLE\
                                    |APP_FLAG_POWERDOWN\
                                    |APP_FLAG_LINEIN\
                                    |APP_FLAG_WAVE_PLAYING\
                                    |APP_FLAG_MUSIC_PLAY)
enum
{
    PROFILE_ID_A2DP  = 0,
    PROFILE_ID_AVRCP = 1,
    PROFILE_ID_HFP   = 2,
    PROFILE_ID_HSP   = 3,
    PROFILE_ID_HID   = 4,
#ifdef CONFIG_TWS
    PROFILE_ID_A2DP_SRC = 5,
    PROFILE_ID_AVRCP_CT = 6
#endif
};

#define APP_BUTTON_ACTION_CLICK              0x80000000
#define APP_BUTTON_ACTION_DOUBLE_CLICK       0x40000000
#define APP_BUTTON_ACTION_LONG_PRESS         0x02000000
#define APP_BUTTON_ACTION_WAIT_DOUBLE_CLICK  0x10000000
#define APP_BUTTON_ACTION_CONTINUE           0x08000000
#define APP_BUTTON_ACTION_LONG_LONG_PRESS    0x04000000
#define APP_BUTTON_ACTION_LONG_PRESS_NO_UP   0x20000000
#define APP_BUTTON_ACTION_MASK               0xfE000000
#if 0
#define APP_BUTTON_TYPE_A2DP                0x00400000
#define APP_BUTTON_TYPE_HFP                 0x00800000
#define APP_BUTTON_TYPE_TWC                 0x01000000
#define APP_BUTTON_TYPE_MASK                0x01C00000
#else
#define APP_BUTTON_TYPE_A2DP                0x00000004
#define APP_BUTTON_TYPE_HFP                 0x00000008
#define APP_BUTTON_TYPE_TWC                 0x00000010
#define APP_BUTTON_TYPE_MASK                0x0000001c
#endif

#define APP_BUTTON_MODE_MASK                 0x01FFFFE0 //0x003FFFFF

typedef void (*wav_cb_func)(void) ;

#define APP_RETRY_FOREVER                                 -1
#define APP_AUTO_RECONN_RETRY_TIMES            3

#define APP_AUTO_CONN_RETRY_TIMES                3

#define APP_A2DP_CONN_ACTIVE_DELAY              20000  // 20s

#define APP_DISCONN_ACTION_RETRY_TIMES      APP_RETRY_FOREVER
#define APP_DISCONN_ACTION_FIRST_DELAY	  200 //6000
#define APP_DISCONN_ACTION_TIMEOUT	 	  6000 //5120
#define APP_BT_DISCONN_TIMEOUT                      5000

#define APP_WAVE_FILE_ID_NULL                              (-1)
#define APP_WAVE_FILE_ID_START                            0
#define APP_WAVE_FILE_ID_CONN                              1
#define APP_WAVE_FILE_ID_DISCONN                        2
#define APP_WAVE_FILE_ID_POWEROFF                     3
#define APP_WAVE_FILE_ID_ENTER_PARING              4
#define APP_WAVE_FILE_ID_LOW_BATTERY               5
#define APP_WAVE_FILE_ID_HFP_RING                      6
#define APP_WAVE_FILE_ID_HFP_ACK                       7
#define APP_WAVE_FILE_ID_HFP_REJECT                  8
#define APP_WAVE_FILE_ID_HFP_CANCEL                 9
#define APP_WAVE_FILE_ID_HFP_VOICE_DIAL          10
#define APP_WAVE_FILE_ID_MUTE_MIC                     11
#define APP_WAVE_FILE_ID_UNMUTE_MIC                 12
#define APP_WAVE_FILE_ID_VOL_MAX                       13
#define APP_WAVE_FILE_ID_VOL_MIN                        14
#define APP_WAVE_FILE_ID_HFP_TRANSFER             15
#define APP_WAVE_FILE_ID_REDIAL                          16
#define APP_WAVE_FILE_ID_CLEAR_MEMORY             17
#define APP_WAVE_FILE_ID_VOICE_NUM_0               18
#define APP_WAVE_FILE_ID_VOICE_NUM_1               19
#define APP_WAVE_FILE_ID_VOICE_NUM_2               20
#define APP_WAVE_FILE_ID_VOICE_NUM_3               21
#define APP_WAVE_FILE_ID_VOICE_NUM_4               22
#define APP_WAVE_FILE_ID_VOICE_NUM_5               23
#define APP_WAVE_FILE_ID_VOICE_NUM_6               24
#define APP_WAVE_FILE_ID_VOICE_NUM_7               25
#define APP_WAVE_FILE_ID_VOICE_NUM_8               26
#define APP_WAVE_FILE_ID_VOICE_NUM_9               27
#define APP_WAVE_FILE_ID_VOICE_CONTINUE         0x8000

#define APP_WAVE_FILE_ID_BT_MODE                       28
#define APP_WAVE_FILE_ID_UDISK_MODE                 29
#define APP_WAVE_FILE_ID_FM_MODE                      30
#define APP_WAVE_FILE_ID_LINEIN_MODE                31
#define APP_WAVE_FILE_ID_SDCARD_MODE              32

#define	APP_WAVE_FILE_ID_RESERVED1			33
#define	APP_WAVE_FILE_ID_RESERVED2			34
#define	APP_WAVE_FILE_ID_RESERVED3			35

#ifdef CONFIG_TWS
#define APP_WAVE_FILE_ID_STEREO_CONN		       APP_WAVE_FILE_ID_RESERVED1
#define APP_WAVE_FILE_ID_STEREO_DISCONN		APP_WAVE_FILE_ID_RESERVED2
#define APP_WAVE_FILE_ID_STEREO_MATCH		APP_WAVE_FILE_ID_RESERVED3
#else
#define APP_WAVE_FILE_ID_BLE_CONN			APP_WAVE_FILE_ID_RESERVED1
#define APP_WAVE_FILE_ID_BLE_DISCONN		       APP_WAVE_FILE_ID_RESERVED2
#endif
#define	APP_WAVE_FILE_ID_RESERVED4			36
#define	APP_WAVE_FILE_ID_RESERVED5			37
#define	APP_WAVE_FILE_ID_RESERVED6			38
#define	APP_WAVE_FILE_ID_RESERVED7			39
#define	APP_WAVE_FILE_ID_RESERVED8			40
#define	APP_WAVE_FILE_ID_RESERVED9			41

#define WAVE_EVENT                                                  42

#define INVALID_WAVE_EVENT                  WAVE_EVENT

#define IS_MODE_CHANGE_FILE_ID(id)      (((id >= APP_WAVE_FILE_ID_BT_MODE) && (id <= APP_WAVE_FILE_ID_LINEIN_MODE))? 1:0)

//add by zjw for button control
enum
{
    SYS_WM_BT_MODE = 0,
    SYS_WM_UDISK_MUSIC_MODE,
    SYS_WM_SD_MUSIC_MODE,
    SYS_WM_FM_MODE,
    SYS_WM_LINEIN_MODE,
    SYS_WM_NULL,
};

enum
{
    APP_SDP_SRV_RFCOMM_CHANNEL = 0,
    APP_SDP_SRV_A2DP_HANDLE,
    APP_SDP_SRV_END
};

enum
{
    BT_DISCONNECTED = 0,
    BT_DISCONNECTING,
};

#define INITIATIVE_CONNECT_START        1
#define INITIATIVE_CONNECT_STOP         0
#define INITIATIVE_CONNECT_HID         2

typedef struct _system_s
{
    /*led part */
    uint8_t        led_onoff[LED_NUM];
    uint16_t       led_repeat_count[LED_NUM];
    uint16_t       led_repeat[LED_NUM];
    uint8_t        led_num_flash_count[LED_NUM];
    uint8_t        led_num_flash[LED_NUM];
    uint16_t       led_timeout[LED_NUM];
    uint16_t       led_ontime[LED_NUM];
    uint16_t       led_offtime[LED_NUM];
    uint8_t        led_map[LED_NUM];
    uint16_t       led_blink_count[LED_NUM];
    app_led_info_t led_info[LED_EVENT_END];
    uint8_t        led_event_save;
    uint8_t        last_led_event;

    //button part */
    APP_BUTTON_MODE button_mode;
    //int     button_press_count;
    //int     button_long_press_count;
    APP_BUTTON_STATE button_state;
    //APP_BUTTON_MODE button_commit;
    uint32   button_type;
    //uint32   button_count;
    //const uint32 *button_code;
    //const button_action_func *button_handler;
    uint32   button_code[BUTTON_BT_END];
    button_action_func  button_handler[BUTTON_BT_END];

    uint32  button_mask;
    //uint32  btn_init_map;

    int8     volume_store;
    int8     mic_volume_store;

    jtask_h   app_auto_con_task;
    jtask_h   app_reset_task;
    jtask_h   app_match_task;
    jtask_h   app_audio_task;
    jtask_h   app_a2dp_task;
    //#ifdef APP_ENV_RESTART_MUSIC_RECOVER
    //jtask_h	  app_a2dp_music_rescover_task;
    //#endif
    jtask_h   app_save_env_task; // save env info to flash
    uint32   low_detect_count;
    int32   powerdown_count;
    uint16   linein_count;
    int8     low_detect_channel;
#if 1//(CONFIG_APP_TOOLKIT_5 == 1)
    uint32	 charg_timeout_cnt;
#endif

    uint16   press_thre;
    uint16   double_thre;
    uint16   long_thre;
    uint16   vlong_thre;
    uint16   repeat_thre;

    hci_unit_t *unit;
    uint16_t   link_handle;
    btaddr_t   remote_btaddr;

    btaddr_t   reconn_btaddr;
    btaddr_t   reconn_btaddr2;
    uint8_t     reconn_num_flag;

    uint8_t   auto_conn_id;
    uint8_t   auto_conn_id1;
    uint8_t   auto_conn_id2;
    uint8_t   auto_conn_status;

    uint8_t    hfp_rfcomm_channel;

    int8_t     linein_vol;
#ifdef CONFIG_TWS
    btaddr_t   stereo_btaddr;
    uint8_t 	inquiry_ticks;
    uint8_t   num_stereo_cnt;
    jtask_h 	app_stereo_task;
    jtask_h 	app_sync_task;
#endif

    uint32_t   flag_sm1;
    uint32_t   flag_sm2;

    //uint32 adc_threshold_cal_count;
//#if (CONFIG_CHARGE_EN == 1)
    uint8  flag_charge;
//#endif
    //add by zjw for cov common&protocol
    //jtask_h   app_common_task; //add by zjw for sdcard test
    uint8 HF_establish_call;
    uint32 sys_work_mode;
    uint32 bt_mode;
    int32 pause_powerdown_count;
    uint8 button_ad_channel; 
    jtask_h app_save_volume_task; // save volume info to flash
#ifdef CONFIG_BLUETOOTH_COEXIST
    jtask_h app_coexist_task;
    uint8 flag_exit_bt;
#endif
}APP_SYSTEM_T, *app_handle_t;

__inline app_handle_t app_get_sys_handler(void);
jtask_h app_get_audio_task_handle( void );
void app_init( void );
void app_post_init( void );

//void app_bt_enable_dut_mode( int8 enable);
//void app_bt_enable_fcc_mode( int8 enable,uint8 chnl);

#if (CONFIG_CHARGE_EN == 1) 
void app_charge_powerdown( void );
#endif

void app_button_led_action( uint32 step );
void app_set_led_event_action( APP_LED_EVENT event );
void app_button_type_set( uint32 type );
int app_button_sw_action( uint8_t button_event );
void app_sleep_func( int enable );
void app_bt_write_sniff_link_policy(uint16_t link_handle,uint8 set_link_policy);
void app_bt_enter_sniff_mode(uint16_t link_handle,uint8_t enable);
void app_bt_enable_complete_wrap( hci_unit_t *unit );

void app_bt_disconn_wrap(uint8 reason, btaddr_t *raddr);
void app_bt_acl_time_out_wrap( void );
void app_bt_key_store_wrap( btaddr_t *raddr, uint8 *key, uint8 key_type );

void app_bt_shedule_task( jthread_func func, void *arg, uint32 delay_ms );
void app_bt_get_remote_addr( btaddr_t *addr);
void *app_bt_get_handle( uint8_t handle_type );
void app_bt_set_match_timeout( void );
int app_bt_get_disconn_event( void );

void app_bt_profile_conn_wrap(uint8 profileId,btaddr_t* rtaddr);

void app_bt_auto_conn_ok( void );
void app_bt_allpro_conn_start(int delay,btaddr_t* rtaddr);

result_t app_bt_active_conn_profile( uint8 profileId, void *arg );

void app_bt_status_show( void );
#ifdef BEKEN_DEBUG
void app_bt_debug_show( void );
#endif
#ifdef BT_ONLY_ONE_BT
void app_bt_set_role_conning(void);
#endif
void app_wave_file_volume_init( uint8_t vol );
void app_wave_file_aud_notify( uint32 freq, uint16 channel, uint8 vol );
void app_wave_file_voice_num_set(char * num);
uint8 app_wave_file_play_start( int file_id );
void app_wave_file_play_stop( void );
void app_wave_file_play( void );
int app_wave_playing( void );
void app_powerdown_action( void );
/*static*/ int app_button_powerdown(void);

#ifdef CONFIG_BLUETOOTH_HFP
int app_button_clear_at_pending(uint16 step);
int app_button_hfack_action( void );
int app_button_reject_action(void);
/*static*/  int app_button_redial_last_number( void );
/*static*/ int app_button_hf_transfer_toggle( void );
int app_button_lang_change( void );
#endif
//jtask_h app_common_get_task_handle( void );
//int app_button_playpause_action_caller( int play_pause );
//int app_button_conn_disconn_caller( void );
int system_work_mode_change_button(void);
unsigned int get_app_mode(void);
unsigned int get_bt_mode(void);
//void set_bt_mode(unsigned int);
uint8 start_wave_and_action(uint32 ,wav_cb_func );
uint8 enter_mode_wave_and_action(uint32 ,wav_cb_func );

uint32 app_is_mp3_mode(void);
uint32 app_is_bt_mode(void);
uint32 app_is_linein_mode(void);
#ifdef FM_ENABLE
uint32 app_is_fm_mode(void);
#endif
void enter_work_mode(void);
void app_reset(void);

uint8 system_mode_shift_for_sd_change(uint8 attatch_flag);
#ifdef CONFIG_APP_USB
uint8 system_mode_shift_for_Udisk_change(uint8 attatch_flag);
#endif
uint8 system_mode_shift_for_linein_change(uint8 attatch_flag);

void app_low_power_detect(void );
uint32 Media_Fs_Init(uint8 type);
uint8 system_work_mode_direct_change(uint32 new_mode);
void exit_work_mode(uint32 mode);
void app_low_power_scanning(uint32 step );
void app_audio_restore(void);

void a2dp_patch_get_raddr(btaddr_t *addr);
void a2dp_try_avrcp_timer_handler(void *arg);
uint32 a2dp_startup_try_avrcp(void);
void a2dp_terminate_try_avrcp(void);
void a2dp_finish_try_avrcp(void);
void app_led_action(uint32 step);
void transceiver_disable_role_switch(void);
void app_player_unload_for_incoming_call(void);
void app_player_load_for_incoming_call(void);

void g_stream_start_handler(void);

int app_is_not_auto_reconnection(void);
void app_exit_sniff_mode(void);
#ifdef BT_DUALMODE
RAM_CODE uint8 app_ble_sniff_is_enable(void);
RAM_CODE int app_is_enable_set_LE_Advertise(void);
void app_bt_set_role_conning(void);
#endif

#ifdef CONFIG_BLUETOOTH_AVRCP_TG
void AVRCP_TG_VOL_adjust_callback(uint8 opera);
void AVRCP_TG_VOL_set_callback(uint8_t volume_dat, btaddr_t *raddr);
uint8_t AVRCP_TG_VOL_get_callback(btaddr_t *raddr);
uint8_t get_tg_volume_value(void);
uint32 avrcp_current_support_vol_syn(void);
void current_send_volume_change_response(uint8_t volume_value);
#else
void AVRCP_TG_VOL_adjust_callback(uint8 opera);
void AVRCP_TG_VOL_set_callback(uint8_t volume_dat, btaddr_t *raddr);
uint8_t AVRCP_TG_VOL_get_callback(btaddr_t *raddr);
#endif

#ifdef CONFIG_TWS
void app_bt_inquiry( uint8 seconds, uint8 max_responses, uint8 oper );
void app_bt_stereo_auto_conn_stop(void);
void app_bt_inquiry_active_conn( void *arg );
uint32_t app_bt_read_CLKN( void );
void app_tws_debug(uint8 *para);
void debug_show_flag(void);
void debug_set_trace_val(uint8 val);
int8 debug_show_trace(uint8 val);
void app_tws_set_stereo_role(void);
int app_button_stereo( void );
void app_bt_stereo_judge_role_again(void);
void app_bt_stereo_pairing_master(void);
void app_bt_stereo_pairing_slave(void);
void app_bt_stereo_pairing_exit(void);
#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
void app_sniff_debug(void);
#endif
void app_bt_trig_dac(uint32_t timer);
void app_bt_sync_send_timer_stop( void );
//int app_button_play_action( void );
#endif

#ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
void app_start_encode(void);
void app_start_aux_tx(void *arg);
#endif

uint8 get_bt_dev_priv_work_flag(void);
void enter_match_state(void);
uint8_t app_set_role_accept_con_req(hci_unit_t *unit, const btaddr_t *btaddr);
uint8_t app_set_role_creat_con_req(hci_unit_t *unit, const btaddr_t *btaddr);
uint8_t app_check_bt_mode(uint8 mode);
t_battery_charge_state get_Charge_state(void);
void app_env_save_task(void *arg,uint8 type);
void app_restore_volume(uint8 voltype);
#if (SYS_VOL_SAVE == 1)
void app_save_volume(uint8 voltype);
#endif
void bt_mode_msg_handler(void);

#if defined(CONFIG_BLUETOOTH_SPP) //&&defined(SUPPORT_BLUETOOTH_SPP)
uint8 app_mp3player_ble_spp_is_enable(void); // 1:support spp enable    0:disable
#endif
#ifdef BEKEN_UP_OTA_3266
int app_button_ota_enable_action( void );
#endif
uint8 app_ota_is_updating(void);// OTA  正在升级

uint8 app_coexist_is_not_bt_mode(void);
#ifdef CONFIG_BLUETOOTH_COEXIST
void app_change_mode_flag_set(void);
void app_change_mode_flag_clear(void);
uint8 app_change_mode_flow_ctrl_en(void);
void app_coexist_play_pause(uint8 flag);
#endif

#endif
