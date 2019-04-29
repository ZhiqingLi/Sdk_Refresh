#ifndef _MSG_PUB_H_
#define _MSG_PUB_H_

#include "types.h"
#include "config.h"



#define DELAY_MSG_PUT                (1)
#define OS_TICK_MSEC                 (10)

#define MSG_SUCCESS_RET              (0)
#define MSG_FAILURE_RET              (-1)

typedef struct
{
    uint32 id;
    void *hdl;
    uint32 arg;
}MSG_T, *MSG_PTR;

/* public api*/
extern void msg_init(void);
extern void msg_dump(void);
extern void msg_uninit(void);
extern void msg_put(uint32 msg);
extern void msg_clear_all(void);
extern int msg_priority_put(MSG_T *msg);

#ifdef DELAY_MSG_PUT
extern int msg_delay_put(uint32 os_tick, MSG_T *msg);
#endif

/* message table*/
#define KEY_MSG_GP                      (0x00000000)    /* Attention: special msg*/
#define BLUETOOTH_MSG_GP                (0x10000000)
#define SDMUSIC_MSG_GP                  (0x20000000)
#define UDISK_MSG_GP                    (0x30000000)
#define FM_MSG_GP                       (0x40000000)
#define LINEIN_MSG_GP                   (0x50000000)
#define OTHER_MSG_GP                    (0x60000000)
#define ENV_MSG_GP                      (0x70000000)

/* Name format: MSG_module_messageInformation
   assume: message number is less than 65535 at one module
*/
enum
{
    MSG_NULL = 0, 
    /* Attention: special msg for key press, from 0x00000000--0x00000fff*/
	MSG_KEY_0							 = MSG_NULL + 0x1,
	MSG_KEY_1,
	MSG_KEY_2,
	MSG_KEY_3,
	MSG_KEY_4,
	MSG_KEY_5,
	MSG_KEY_6,
	MSG_KEY_7,
	MSG_KEY_8,
	MSG_KEY_9,

	MSG_KEY_PLAY,
	MSG_KEY_NEXT,
	MSG_KEY_PREV,
	MSG_KEY_REWIND,
	MSG_KEY_FORWARD,
	MSG_KEY_VOL_UP,
	MSG_KEY_VOL_DOWN,
	MSG_KEY_MUTE,
	MSG_KEY_MATCH,
	MSG_KEY_POWER_DOWN,
	MSG_KEY_HFP_ACK,
	MSG_KEY_HFP_REJECT,
	MSG_KEY_NUM_REDIAL,
	MSG_KEY_MIC_UP,
	MSG_KEY_MIC_DOWN,
         MSG_KEY_MIC_MUTE,
         MSG_KEY_VOICE_DIAL,
         MSG_KEY_TRANSFER_TOGGLE,
         MSG_KEY_CONN_DISCONN,
    	MSG_KEY_STOP,
    	MSG_CLEAR_BT_MEM,
    	MSG_KEY_TEST_MODE,
         MSG_KEY_LANGE_CHANGE,
         MSG_KEY_TWC_HOLD_ACCEPT,
         MSG_KEY_TWC_HUNG_ACCEPT,
         MSG_KEY_TWC_REJECT_HOLD,
         MSG_KEY_TWC_SWITCH_WAY,
         MSG_KEY_2PHONE_SWITCH,
         MSG_KEY_RESERVED0,
         MSG_KEY_RESERVED1,
         MSG_KEY_RESERVED2,
         MSG_KEY_RESERVED3,

         MSG_KEY_STEREO,
	MSG_KEY_MODE,
	MSG_GOTO_BT_MODE,
	MSG_GOTO_AUX_MOTE,
	MSG_GOTO_FM_MODE,
	MSG_GOTO_SD_MODE,
	MSG_GOTO_UDISK_MODE,
	
	MSG_CHANGE_PLAY_MODE,
	MSG_CHANGE_DIR,
	
  
    /*BLK0: bluetooth msg*/
    MSG_BT_INIT                      = BLUETOOTH_MSG_GP + 0x0000,
    MSG_COEXIST_ENTER_BT,
    MSG_COEXIST_EXIT_BT,
    
    /*BLK1: sd music msg*/
    MSG_SD_INIT                      = SDMUSIC_MSG_GP   + 0x0000,
    MSG_SD_ATTACH,            /* sd attach or detach*/
    MSG_SD_DETACH,

	
    /*BLK2: usb disk msg*/
    MSG_UDISK_INIT                   = UDISK_MSG_GP     + 0x0000,
	MSG_USB_ATTACH,
	MSG_USB_DETATCH,

	
    /*BLK3: fm msg*/
    MSG_FM_INIT                      = FM_MSG_GP        + 0x0000,
    MSG_FM_SEEK_CONTIUE,
    MSG_FM_AUTO_SCAN,
    MSG_FM_SEEK_PREV_CHANNEL,
    MSG_FM_SEEK_NEXT_CHANNEL,
    MSG_FM_AUTO_SEEK_START,
    MSG_FM_CHANNEL_SEEK_START,
    MSG_FM_CHANNEL_TUNE_CONTINUE,
    MSG_FM_CHANNEL_SEEK_CONFIG,
    MSG_FM_CHANNEL_TUNE_OVER,
    MSG_FM_TUNE_SUCCESS_CONTINUE,
    MSG_FM_TUNE_FAILURE_CONTINUE,
    MSG_FM_AUTO_SEEK_OVER,
    MSG_FM_SET_FRE,
    MSG_FM_DISABLE_MUTE,
    MSG_FM_ENABLE_MUTE,
    
    
    /*BLK4: linein msg*/
    MSG_LINEIN_INIT                  = LINEIN_MSG_GP  + 0x0000,
    
    /*BLK5: other msg*/   
    MSG_LED_INIT                     = OTHER_MSG_GP   + 0x0000,
    MSG_DEBUG_UART_RX,               /* debug uart gets datum*/
    MSG_SDADC,                       /* sdadc*/
    MSG_POWER_DOWN,
    MSG_EQ_SELECT,
    MSG_POWER_UP,
    MSG_FLASH_WRITE,
    MSG_IRDA_RX,
    MSG_LOWPOWER_DETECT,
    MSG_MEDIA_READ_ERR,      /* mp3-mode,SD/Udisk read Err */
	
    MSG_LINEIN_ATTACH,
    MSG_LINEIN_DETACH,

    MSG_TIMER_PWM1_PT2_HDL,
    MSG_SAVE_VOLUME,
    MSG_CHANGE_MODE,
    MSG_CHANGE_HID_MODE,
    MSG_INPUT_TIMEOUT,
    MSG_HALF_SECOND,
    MSG_1S_TIMER,
    MSG_ENV_WRITE_ACTION,

    // add begin by cai.zhong 20190219 for smartvoice msg
    MSG_SMARTVOICE_CMD_RECEIVED,
    MSG_SMARTVOICE_MIC_DATA_COME,
    MSG_SMARTVOICE_START_STREAM,
    MSG_SMARTVOICE_STOP_STREAM,
    MSG_SMARTVOICE_SEND_AUDIO_DATA,
    // add begin by cai.zhong 20190219 for smartvoice msg
};


#endif
// EOF
