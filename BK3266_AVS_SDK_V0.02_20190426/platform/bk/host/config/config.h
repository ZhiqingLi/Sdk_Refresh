/* Jungo Confidential, Copyright (c) 2012 Jungo Ltd.  http://www.jungo.com */
#ifndef __CONFIG_H__
#define __CONFIG_H__
#include "config_debug.h"
#include "version.h"

#define CONFIG_ARM_COMPILER                         1
#define CONFIG_BYTE_ORDER                       CPU_LITTLE_ENDIAN
#define CONFIG_PORT                             beken_no_os
#define CONFIG_SINGLE_THREADED                   1
#define CONFIG_NATIVE_UINT64                        1
#define CONFIG_JOS_MBUF                                 1
#define CONFIG_JOS_BUS                          1
#define CONFIG_JOS_UTILS                        1
#define CONFIG_JOS_SECURE_PTR                   1
#define CONFIG_COMMON_STR_UTILS                 1
#define CONFIG_MEMPOOL                          1
//#define CONFIG_MEMPOOL_SIZE                    (38 * 1024)//(33 * 1024)
#define CONFIG_MEMPOOL_DMABLE                   1
#define CONFIG_BLUETOOTH                        1
#define CONFIG_BLUETOOTH_HCI_UART               1
#define CONFIG_BLUETOOTH_SDP_SERVER             1
#define CONFIG_BLUETOOTH_A2DP                   1
#define CONFIG_BLUETOOTH_A2DP_SINK              1
#define CONFIG_BLUETOOTH_AVRCP                  1
#define CONFIG_BLUETOOTH_AVRCP_CT               1
#define CONFIG_BLUETOOTH_AVRCP_TG              1
#define CONFIG_BLUETOOTH_AVDTP                  1
#define CONFIG_BLUETOOTH_AVDTP_SCMS_T     1
#define CONFIG_BLUETOOTH_AVCTP                  1
#define CONFIG_BLUETOOTH_HFP                    1
#define CONFIG_BLUETOOTH_SDP_HFP                1
#define CONFIG_BLUETOOTH_HSP                    1
#define CONFIG_BLUETOOTH_SDP_HSP                1
#define CONFIG_BLUETOOTH_SPP                    1
#define CONFIG_BLUETOOTH_SCO                    1
#define CONFIG_BLUETOOTH_APP                    1
#define CONFIG_BLUETOOTH_AUDIO_APP_STANDALONE   1
#define CONFIG_AUDIO_OUT_INTERFACE              1
#define CONFIG_PKG                              1
#define CONFIG_PKG_SBC                          1
#define CONFIG_FILE_LIST                        file_list_beken
#define UWVER_STR                               "4.0.33.5"
#define CONFIG_APP_MP3PLAYER                    0  //change by cai.zhong 20190308 for smartvoice 
#define CONFIG_AUDIO_TRANSFER_DMA               1
#define LOWBAT_DET_ENABLE						 1

#define CONFIG_DEBUG_PCM_TO_UART                   0
#define CONFIG_CPU_CLK_OPTIMIZATION               1
#define CONFIG_PROMPT_WAVE_AS_ALAW              0
#define CONFIG_BLUETOOTH_SSP
//#define CONFIG_BLUETOOTH_HID      //change by cai.zhong 20190308 for smartvoice 
/* #define CONFIG_DRIVER_I2S */
#define CONFIG_DRIVER_DAC
#define CONFIG_DRIVER_ADC
#define CONFIG_CHARGE_EN						1

#define CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE      1
#define CONFIG_UART_IN_SNIFF					0

//注意： 音箱方案配置时请将以下宏CONFIG_AUDIO_MIC_DIFF_EN/CONFIG_EXT_AUDIO_PA_EN打开， SYS_CFG_BUCK_ON关闭

#define CONFIG_TX_CALIBRATION_EN                1

#define CHIP_PACKAGE_TSSOP_28                   0
#if (CHIP_PACKAGE_TSSOP_28 == 1)
#define CONFIG_AUDIO_DAC_ALWAYSOPEN			1
#define CONFIG_AUDIO_DAC_RAMP_EN				0
#define CONFIG_AUDIO_MIC_DIFF_EN				1
#define CONFIG_EXT_AUDIO_PA_EN   				1
#define CONFIG_EXT_PA_DIFF_EN                              1
#define SYS_CFG_BUCK_ON                                         0
#define BT_ONE2MULTIPLE_AS_SCATTERNET		1
#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
    #undef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
#endif
#else
#define BT_ONE2MULTIPLE_AS_SCATTERNET		0
#define CONFIG_AUDIO_DAC_ALWAYSOPEN			0
#define CONFIG_AUDIO_DAC_RAMP_EN				1
#define CONFIG_AUDIO_MIC_DIFF_EN				0
#define CONFIG_EXT_AUDIO_PA_EN   				0
#define CONFIG_EXT_PA_DIFF_EN                              0
#define SYS_CFG_BUCK_ON                                         1
#endif//CHIP_PACKAGE_TSSOP_28

#if (CONFIG_CHARGE_EN == 1)
#define CHARGE_HARDWARE         0
#define CHARGE_SOFTWARE         1
#define CHARGE_EXTERNAL         2
#define CONFIG_CHARGE_MODE   CHARGE_HARDWARE
#endif

/* #define CONFIG_APP_HALFDUPLEX */
#define CONFIG_APP_AEC
#define CONFIG_APP_EQUANLIZER 
#define CONFIG_DAC_DELAY_OPERATION              0
#define CONFIG_ANA_DAC_CLOSE_IN_IDLE			1

//#define CONFIG_IRDA 
/* #define WROK_AROUND_DCACHE_BUG */

// PWM not sleep when no connection
//#define CONFIG_PWM_NOT_SLEEP

//#define CONFIG_ACTIVE_SSP

#define BT_MODE_1V1           (1<<0)
#define BT_MODE_1V2           (1<<1)
#define BT_MODE_TWS          (1<<2)
#define BT_MODE_BLE           (1<<3)
#define BT_MODE_DM_1V1    (1<<4)
#define BT_MODE_DM_TWS   (1<<5)

#define BT_MODE		           BT_MODE_DM_1V1



#if( BT_MODE==BT_MODE_1V1)
    #ifndef BT_ONE_TO_MULTIPLE
    #define BT_ONE_TO_MULTIPLE
    #endif
    #ifndef BT_ONLY_ONE_BT
    #define BT_ONLY_ONE_BT
    #endif
    #ifdef CONFIG_TWS
    #undef CONFIG_TWS
    #endif
    #ifndef CONFIG_EN_AFC
    #define CONFIG_EN_AFC
    #endif
    //#ifndef CONFIG_BLUETOOTH_HID
    //#define CONFIG_BLUETOOTH_HID
    //#endif
#elif( BT_MODE==BT_MODE_1V2)
    #ifndef BT_ONE_TO_MULTIPLE
    #define BT_ONE_TO_MULTIPLE
    #endif
    #ifdef BT_ONLY_ONE_BT
    #undef BT_ONLY_ONE_BT
    #endif
    #ifdef CONFIG_TWS
    #undef CONFIG_TWS
    #endif
    #define CONFIG_AUTO_CONN_AB  // 上电回连 A ,B 手机交叉循环连接
#elif( BT_MODE==BT_MODE_TWS)
    #ifdef BT_ONE_TO_MULTIPLE
    #undef BT_ONE_TO_MULTIPLE
    #endif
    #ifdef BT_ONLY_ONE_BT
    #undef BT_ONLY_ONE_BT
    #endif
    #ifndef CONFIG_TWS
    #define CONFIG_TWS
    #endif
#elif((BT_MODE==BT_MODE_DM_1V1) || (BT_MODE==BT_MODE_BLE))
    #ifndef BT_ONE_TO_MULTIPLE
    #define BT_ONE_TO_MULTIPLE
    #endif
    #ifndef BT_ONLY_ONE_BT
    #define BT_ONLY_ONE_BT
    #endif
    #ifdef CONFIG_TWS
    #undef CONFIG_TWS
    #endif
    #ifndef BT_DUALMODE
    #define BT_DUALMODE
    #endif
    #ifndef CONFIG_EN_AFC
    #define CONFIG_EN_AFC
    #endif
#elif( BT_MODE==BT_MODE_DM_TWS)
    #ifdef BT_ONE_TO_MULTIPLE
    #undef BT_ONE_TO_MULTIPLE
    #endif
    #ifdef BT_ONLY_ONE_BT
    #undef BT_ONLY_ONE_BT
    #endif
    #ifndef CONFIG_TWS
    #define CONFIG_TWS
    #endif
    #ifndef BT_DUALMODE
    #define BT_DUALMODE
    #endif
#endif


#ifdef CONFIG_TWS
//	#ifdef CONFIG_BLUETOOTH_AVRCP_TG
//	#undef CONFIG_BLUETOOTH_AVRCP_TG
//	#endif
//	#ifdef CONFIG_APP_AEC
//	#undef CONFIG_APP_AEC
//	#endif
    #ifdef BT_ONE_TO_MULTIPLE
    #undef BT_ONE_TO_MULTIPLE
    #endif
    #ifdef BT_ONLY_ONE_BT
    #undef BT_ONLY_ONE_BT
    #endif
    #ifdef BQB_TEST
    #undef BQB_TEST
    #define BQB_TEST 0
    #endif
    #ifdef CONFIG_DAC_DELAY_OPERATION
    #undef CONFIG_DAC_DELAY_OPERATION
    #define CONFIG_DAC_DELAY_OPERATION 1
    #endif
    #ifdef INQUIRY_ALWAYS
    #undef INQUIRY_ALWAYS
    #endif
    #ifdef CONFIG_AUDIO_TRANSFER_DMA
    #undef CONFIG_AUDIO_TRANSFER_DMA
    #define CONFIG_AUDIO_TRANSFER_DMA 0
    #endif
    #ifdef CONFIG_CPU_CLK_OPTIMIZATION
    #undef CONFIG_CPU_CLK_OPTIMIZATION
    #define CONFIG_CPU_CLK_OPTIMIZATION 0
    #endif

    #define TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
    #define LINE_TX_DEBUG

    #ifdef LINE_TX_DEBUG
        #define LTX_PRT      os_printf
    #else
        #define LTX_PRT      os_null_printf
    #endif


#endif

//#define CONFIG_PRODUCT_TEST_INF
#if ( BT_MODE==BT_MODE_1V2)
#define CONFIG_MEMPOOL_SIZE                         (40 * 1024)
#else 
#define CONFIG_MEMPOOL_SIZE                         (36 * 1024)
#endif

#ifdef BT_ONE_TO_MULTIPLE
#ifdef CONFIG_MEMPOOL_SIZE
    #undef CONFIG_MEMPOOL_SIZE
   #ifdef CONFIG_APP_AEC
    #if ( BT_MODE==BT_MODE_1V2)
    #define CONFIG_MEMPOOL_SIZE                     (40 * 1024)
    #else
    #define CONFIG_MEMPOOL_SIZE                     (36 * 1024)
    #endif
   #else
    #define CONFIG_MEMPOOL_SIZE                     (27 * 1024)
   #endif 
#endif
//    #ifdef CONFIG_CPU_CLK_OPTIMIZATION
//        #undef CONFIG_CPU_CLK_OPTIMIZATION
//        #define CONFIG_CPU_CLK_OPTIMIZATION 		0
//    #endif
    #define A2DP_PATCH_FOR_AVRCP
    #define OTT_STRETIGG_LINK_COEXIST
#ifdef BT_ONLY_ONE_BT
    #define BT_MAX_AG_COUNT                                             1
    #define CONFIG_SELECT_PREV_A2DP_PLAY		         0
    #define CONFIG_A2DP_PLAYING_SET_AG_FLOW		  0
    #define INQUIRY_ALWAYS
    //#define CONN_WITH_MUSIC		
    #define NEED_SNIFF_DEVICE_COUNT                              1
    //#define IPHONE_MUSIC_STATE_COME_LATE_REPAIR     //for iphone PLAYING come before MA having comed,we need stream_start manually on avrcp notify coming
#else
    #define BT_MAX_AG_COUNT                                             2
    #define CONFIG_A2DP_PLAYING_SET_AG_FLOW		  1
    #define CONFIG_SELECT_PREV_A2DP_PLAY		         1
    //#define INQUIRY_ALWAYS
    #define CONN_WITH_MUSIC
    #define NEED_SNIFF_DEVICE_COUNT                              2
    #if(CONFIG_SELECT_PREV_A2DP_PLAY == 0)
    	#define IPHONE_MUSIC_STATE_COME_LATE_REPAIR     //for iphone PLAYING come before MA having comed,we need stream_start manually on avrcp notify coming
    #endif
#endif
//    #define NO_SCAN_WHEN_WORKING
#else
    #define A2DP_PATCH_FOR_AVRCP
    #define BT_MAX_AG_COUNT                                             1
    #define CONFIG_A2DP_PLAYING_SET_AG_FLOW		  0
    #define CONFIG_SELECT_PREV_A2DP_PLAY		         0
    #ifdef CONFIG_TWS
        #define NEED_SNIFF_DEVICE_COUNT                          2
    #else
        #define NEED_SNIFF_DEVICE_COUNT                          1
    #endif
    #define INQUIRY_ALWAYS
#endif
#if (CONFIG_DEBUG_PCM_TO_UART == 1)
    #ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
        #undef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
    #endif
#endif

#define CONFIG_HFP17_MSBC_SUPPORTED                         1

#define CONFIG_APP_MSBC_RESAMPLE                0 // 16bit 8k     turn  the macro on or off in thd SDK
#define CONFIG_APP_ADC_VAD                            0 //    turn  the macro on or off in thd SDK
#define CONFIG_APP_DAC_VAD                            0 //    turn  the macro on or off in thd SDK

/* #define INCOMINGCALL_HF_TRANSFER_SCO */
//#define INCOMING_CALL_RING //play remote number wave first, and then internal ring whatever remote support inband ring


/*Memorize System Information into Flash, The Address based on Flash Size
    --8,   If 4Mbit Flash  4*1024*1024/8/0x10000
    --32,  If 8Mbit Flash
*/
#define FLASH_LAST_BLOCK_NUM		          (8)

#define IPHONE_BAT_DISPLAY
#ifdef CONFIG_APP_AEC
#define ADJUST_AEC_DELAY 1
//#define CONFIG_APP_DATA_CAPTURE 1
#endif

#define CONFIG_AUD_FADE_IN_OUT              1

//#define UPDATE_LOCAL_AFH_ENGINE_AFTER_ACK   

#define CONFIG_SBC_DECODE_BITS_EXTEND		1
#define CONFIG_SBC_PROMPT      1
#define A2DP_SBC_DUMP_SHOW
#define CONFIG_SW_SWITCH_KEY_POWER						1
/********************************************BT_DUALMODE BEGIN*******************************************
 *BT_DUALMODE macro contorl,function config
 *added by yangyang, updated 2018/02/02 
 */
#ifdef BT_DUALMODE 
    //#define LMP_LINK_L2CAL_TIMEOUT_ENABLE
    //#define LE_DEBUG_ENABLE                                   /* le_debug message print enable or not */
    //#define LE_MINI_INTERV_WITH_MUSIC_SOLUTION_ENBALE        /* music play when LE update with 7.5ms inteval,we stay in BLE until update to big interval*/
    #define LE_FIRST_FULL_RX_TIMEOUT_ENBALE                     /* first_full_rx_timeout avoid BT disconn among mini LE_CONNECTION_INTERVAL */
    #define LE_SLEEP_ENABLE                                   /* DM sleep enable */
    //#define LE_CLOSE_WITH_SCO_ENBALE                          /* close BLE when seco connected */
    //#define LE_PROTECT_MUSIC_FLUENCY_ENABLE                   /* discrad BLE anchor point to ensure music palying fluency(Andriod 6.0 particularly)*/
    #define LE_ADV_ENABLE_WITHOUT_SCO_ENABLE                    /* diaable LE_adv when sco connection, enable LE_adv when sco disable*/
    #define SUPPORT_BLE_MD                                      /* BLE more data enable */
    //#define SUPPORT_BAIDU_OPUS                                  /*some changes for Baidu case*/
    #ifdef SUPPORT_BAIDU_OPUS
    #define SUPPORT_128BITS_UUID                               /*supported 128 UUID ble/spp*/
    #endif
#ifdef LE_SLEEP_ENABLE
    #ifndef CONFIG_CPU_CLK_OPTIMIZATION
        #define CONFIG_CPU_CLK_OPTIMIZATION         1
    #endif
    #ifndef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
        #define CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE      1
    #endif
    #if 0//def CONFIG_LINE_SD_SNIFF
        #undef CONFIG_LINE_SD_SNIFF
    #endif 
#else
    #ifdef CONFIG_CPU_CLK_OPTIMIZATION
        #undef CONFIG_CPU_CLK_OPTIMIZATION
        #define CONFIG_CPU_CLK_OPTIMIZATION 0
    #endif
    #ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
        #undef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
    #endif 
#endif
#endif

/*******************************************BT_DUALMODE END***********************************************/
 
// for BQB Test
#define CONFIG_CTRL_BQB_TEST_SUPPORT    0

#define BEKEN_UP_OTA_3266	 // OTA Updating								
#ifdef BEKEN_UP_OTA_3266
#ifdef BT_DUALMODE 
#define BEKEN_BLE_OTA_3266									/* OTA supported by BLE */
#define BEKEN_BLE_OTA_3266_POWER_FAIL
#endif
#define BEKEN_SPP_OTA_3266									/* OTA supported by SPP */
#endif

/*************************************LP_MDD_PDD_SUPPORT BEGIN*****************************************/
#define LP_MDD_PDD_SUPPORT
/*************************************LP_MDD_PDD_SUPPORT END*****************************************/


/*************************************************************************  
* control version upgraded.
*************************************************************************/
#define UPGRADED_VERSION  1
/*************************************************************************  
* just only for pts testing, it should be closed in release version.
*************************************************************************/
#define PTS_TESTING 0


#if (CONFIG_APP_MP3PLAYER == 1)
#ifdef CONFIG_APP_SDCARD
    #undef CONFIG_APP_SDCARD
#endif
    // #define CONFIG_APP_SDCARD    //change by cai.zhong 20190308 for smartvoice 
    // #define CONFIG_APP_USB       // U盘模式时，不支持SNIFF功能 //change by cai.zhong 20190308 for smartvoice 
    
#ifdef CONFIG_APP_SDCARD  
//	#define CONFIG_APP_SDCARD_4_LINE
#ifdef CONFIG_APP_SDCARD_4_LINE
    #undef CONFIG_APP_SDCARD_4_LINE
#endif
#ifndef CONFIG_APP_SDCARD_4_LINE
    #define CONFIG_SDCARD_BY_MULTIBLOCK_WAY
#endif
#endif    
    
    #define  SYS_VOL_SAVE		0 // save vol
    #define CALC_PLAY_TIME     0
#ifdef BT_DUALMODE   
    //#define SUPPORT_SPP_IN_MP3PLAYER            1 // allow spp conn in mp3player/linein mode
#endif

#if 0//ndef LE_SLEEP_ENABLE
#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
    #undef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE 
#endif
#ifdef CONFIG_LINE_SD_SNIFF
    #undef CONFIG_LINE_SD_SNIFF //全功能版本LINEIN/SD，无双模
#endif
#endif

//#define FM_ENABLE   
#ifdef FM_ENABLE
#define FM_IN_LINEIN
#define FM_BK1080H
#define DATA_PIN  	11
#define CLK_PIN   10
#endif
#define CONFIG_NOUSE_RESAMPLE 		1

//#define CONFIG_BK_QFN56_DEMO    1

#endif

#define ADC_CHANNEL_1		                1 // Virtual GPIO2(Physical GPIO6)
#define ADC_CHANNEL_2  	                       2 // Virtual GPIO3(Physical GPIO7)
#define ADC_CHANNEL_4		                4 // Virtual GPIO11(Physical GPIO18)
#define ADC_CHANNEL_6  	                       6 // Virtual GPIO13(Physical GPIO20)
#define ADC_CHANNEL_7  	                       7 // Virtual GPIO14(Physical GPIO21)
#define ADC_CHANNEL_NULL                    0xff

//#define ADC_KEY_CHANNEL 	                ADC_CHANNEL_1
//#define ADC_CHARGE_CHANNEL 	         3

//#define CONFIG_BLUETOOTH_COEXIST      // bluetooth coexist, can connect bluetooth in other modes.   应用层可直接开关
#ifdef CONFIG_BLUETOOTH_COEXIST
#ifdef SUPPORT_SPP_IN_MP3PLAYER
    #undef SUPPORT_SPP_IN_MP3PLAYER
#endif
#ifdef CONFIG_MEMPOOL_SIZE
    #undef CONFIG_MEMPOOL_SIZE
    #define CONFIG_MEMPOOL_SIZE                     (40 * 1024)
#endif

#endif

#define CONFIG_PRE_EQ 				1
#define CONFIG_DRC	 				1
#define CONFIG_APP_ESD				0
#define CONFIG_CHG_DEEPSLEEP		0
#define CINFIG_CHARGER_VLCF_OFFSET	5
#define CONFIG_APP_RECONNECT_MATCH	0//回连间隔必须>=15s
 
#define CONFIG_APP_TOOLKIT_5    1
#if (CONFIG_APP_TOOLKIT_5 == 1)
#define POWERKEY_5S_PARING		1
#ifdef BT_MAX_AG_COUNT
    #undef BT_MAX_AG_COUNT
    #define BT_MAX_AG_COUNT     1 // (app_check_bt_mode(BT_MODE_1V2)?2:1)
#endif
#endif


#endif

