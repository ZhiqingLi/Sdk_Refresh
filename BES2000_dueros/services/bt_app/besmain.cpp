//#include "mbed.h"
#include <stdio.h>
#include "cmsis_os.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "audioflinger.h"
#include "lockcqueue.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "hal_chipid.h"
#include "analog.h"
#include "apps.h"
#include "app_status_ind.h"
#include "app_utils.h"
#include "app_bt_stream.h"
#include "nvrecord_dev.h"
#include "tgt_hardware.h"
#include "besbt_cfg.h"
extern "C" {
#include "eventmgr.h"
#include "me.h"
#include "sec.h"
#include "a2dp.h"
#include "avdtp.h"
#include "avctp.h"
#include "avrcp.h"
#include "hf.h"
#include "hs.h"
#include "btalloc.h"
#include "hid.h"

#if IAG_BLE_INCLUDE == XA_ENABLED
#include "besble.h"
#endif

#ifdef __AMA_VOICE__
#include "app_ama_bt.h"
#endif


void BESHCI_Open(void);
void BESHCI_Poll(void);
void BESHCI_SCO_Data_Start(void);
void BESHCI_SCO_Data_Stop(void);
void BESHCI_LockBuffer(void);
void BESHCI_UNLockBuffer(void);
}
#include "rtos.h"
#include "besbt.h"

#include "cqueue.h"
#include "btapp.h"
#include "app_bt.h"
#include "app_dip.h"

struct besbt_cfg_t besbt_cfg = {
#ifdef __BT_SNIFF__
    .sniff = true,
#else
    .sniff = false,
#endif
#ifdef __BT_ONE_BRING_TWO__
    .one_bring_two = true,
#else
    .one_bring_two = false,
#endif
};

osMessageQDef(evm_queue, 128, message_t);
osMessageQId  evm_queue_id;

/* besbt thread */
#if IAG_BLE_INCLUDE == XA_ENABLED
#define BESBT_STACK_SIZE 1024*5
#else
#define BESBT_STACK_SIZE (1024*2+256)
#endif

static osThreadId besbt_tid;
uint32_t os_thread_def_stack_Besbt[BESBT_STACK_SIZE / sizeof(uint32_t)];
osThreadDef_t os_thread_def_BesbtThread = { (BesbtThread), (osPriorityAboveNormal), (BESBT_STACK_SIZE), (os_thread_def_stack_Besbt),{0,}, ("BT")};



static unsigned char BT_INQ_EXT_RSP[INQ_EXT_RSP_LEN];
static BESBT_HOOK_HANDLER bt_hook_handler[BESBT_HOOK_USER_QTY] = {0};

int Besbt_hook_handler_set(enum BESBT_HOOK_USER_T user, BESBT_HOOK_HANDLER handler)
{
    bt_hook_handler[user]= handler;
    return 0;
}

static void Besbt_hook_proc(void)
{
    uint8_t i;
    for (i=0; i<BESBT_HOOK_USER_QTY; i++){
        if (bt_hook_handler[i]){
            bt_hook_handler[i]();
        }
    }
}

void bes_refresh_device_name(void)
{
    ME_SetLocalDeviceName((const unsigned char*)BT_LOCAL_NAME, strlen(BT_LOCAL_NAME) + 1);
}


extern struct BT_DEVICE_T  app_bt_device;

extern void a2dp_init(void);
extern void app_hfp_init(void);

extern struct BT_DEVICE_T  app_bt_device;

unsigned char *randaddrgen_get_bt_addr(void)
{
    return bt_addr;
}

unsigned char *randaddrgen_get_ble_addr(void)
{
    return ble_addr;
}

const char *randaddrgen_get_btd_localname(void)
{
    return BT_LOCAL_NAME;
}

const char *randaddrgen_get_btd_blename(void)
{
    return BLE_DEFAULT_NAME;
}

#if HID_DEVICE == XA_ENABLED
static bool shutter_mode = false;

void hid_callback(HidChannel *Chan, HidCallbackParms *Info)
{
    TRACE("hid_callback Info->event=%x\n",Info->event);

    if (Info->event == HID_EVENT_INTERRUPT_CONNECTED)
    {
        TRACE("HID_EVENT_INTERRUPT_CONNECTED \n");
    }
    else if (Info->event == HID_EVENT_CONTROL_CONNECTED)
    {
        TRACE("HID_EVENT_CONTROL_CONNECTED \n");
    }
    else if (Info->event == HID_EVENT_INTERRUPT_DISCONNECTED)
    {
        TRACE("HID_EVENT_INTERRUPT_DISCONNECTED \n");        
        shutter_mode = false;
    }
    else if (Info->event == HID_EVENT_CONTROL_DISCONNECTED)
    {
        TRACE("HID_EVENT_CONTROL_DISCONNECTED \n");        
        shutter_mode = false;
    }
}

void hid_enter_shutter_mode(void)
{
    BtRemoteDevice *RemDev = app_bt_device.hf_channel[BT_DEVICE_ID_1].cmgrHandler.remDev;

    TRACE("hid_enter_shutter_mode \n");
    OS_LockStack();
	if (!shutter_mode){
        if (RemDev){
            Hid_Connect(&app_bt_device.hid_channel[BT_DEVICE_ID_1], RemDev);
            shutter_mode = true;
        }
	}    
    OS_UnlockStack();
}

void hid_exit_shutter_mode(void)
{
    BtRemoteDevice *RemDev = app_bt_device.hf_channel[BT_DEVICE_ID_1].cmgrHandler.remDev;
    
    TRACE("hid_exit_shutter_mode \n");    
    OS_LockStack();
    if (shutter_mode){
        if (RemDev){
            Hid_Disconnect(&app_bt_device.hid_channel[BT_DEVICE_ID_1]);
        }
        shutter_mode = false;
    }    
    OS_UnlockStack();
}
#endif

//typedef void (*DipCallBack)(const DipPnpInfo*,void * ptr);

void app_aac_is_support_callback(const DipPnpInfo* info,void *ptr)
{
	switch (info->VIDSource)
		{
		case SRC_BT:
		{
			switch (info->VID)
				{
				case SRC_BT_SANSUMG:
					app_bt_device.app_vid_of_phone=1;
					TRACE("GET sunsumg phone");
					break;
				case SRC_BT_APPLE:
					TRACE("get apple phone");
					app_bt_device.app_vid_of_phone=2;
				break;
				default:
				   TRACE("don't sure phone");					
				   app_bt_device.app_vid_of_phone=0;
				break;
				}
		}
		break;

		case SRC_USB:
		{
			switch (info->VID)
				{
				case SRC_USB_APPLE:
					app_bt_device.app_vid_of_phone=2;	
					TRACE("get apple phone");
				break;
				default:
					app_bt_device.app_vid_of_phone=0;					 
					TRACE("don't sure phone");
				break;
				}
		}
		break;
		default:
		   app_bt_device.app_vid_of_phone=0;
		   TRACE("not test this case yet.it may get PNP error"); 
		break;
		}

	if(app_bt_device.app_vid_of_phone==2){
		TRACE("#####I AM APPLE PHONE,I CAN SUPPORT AAC!!!!!");
#if defined(A2DP_AAC_ON) 
#ifdef __A2DP_AVDTP_CP__
		A2DP_Register(&app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_1], &a2dp_aac_avdtpcodec, &a2dp_avdtpCp, a2dp_callback);
#else
		A2DP_Register(&app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_1], &a2dp_aac_avdtpcodec, NULL, a2dp_callback);
#endif
#endif
	}
}


//extern void app_ama_send_voice_stream();
//#include "app_ama_handle.h"
//#include "app_ama_bt.h"

extern void app_ama_rfcomm_server_init(void);

uint8_t sco_num=0;
int besmain(void)
{
#ifdef RANDOM_BTADDR_GEN
    dev_addr_name devinfo;
#endif

    OS_Init();

    BESHCI_Open();

    for(uint8_t i=0;i<BT_DEVICE_NUM;i++)
    {
        memset(&(app_bt_device.a2dp_stream[i]), 0, sizeof(app_bt_device.a2dp_stream[i]));
#if defined(A2DP_AAC_ON)         
        memset(&(app_bt_device.a2dp_aac_stream[i]), 0, sizeof(app_bt_device.a2dp_aac_stream[i]));
#endif
#if defined(A2DP_SCALABLE_ON)
        memset(&(app_bt_device.a2dp_scalable_stream[i]), 0, sizeof(app_bt_device.a2dp_scalable_stream[i]));
#endif
        memset(&(app_bt_device.avrcp_channel[i]), 0, sizeof(app_bt_device.avrcp_channel[i]));
        memset(&(app_bt_device.hf_channel[i]), 0, sizeof(app_bt_device.hf_channel[i]));
#if defined (__HSP_ENABLE__)
        memset(&(app_bt_device.hs_channel[i]), 0, sizeof(app_bt_device.hs_channel[i]));
#endif
    }
#ifdef CHIP_BEST1000
    if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_2)
#endif
    {
        sco_num = 2;
    }
#ifdef CHIP_BEST1000
    else
    {
        sco_num = 1;
    }
#endif
#if IAG_BLE_INCLUDE == XA_ENABLED
    bes_ble_init();
#endif
    /* bes stack init */
    EVM_Init();

#if DIP_DEVICE ==XA_ENABLED
	app_dip_init(app_bt_device.dip_client,app_aac_is_support_callback);
	//app_dip_init(app_bt_device.dip_client,NULL);
#endif
    a2dp_init();
    app_hfp_init();
#if defined(__HSP_ENABLE_)    
    app_hsp_init();
#endif
    /* pair callback init */
    pair_handler.callback = pair_handler_func;
    SEC_RegisterPairingHandler(&pair_handler);
#if BT_SECURITY == XA_ENABLED
    auth_handler.callback = auth_handler_func;
    SEC_RegisterAuthorizeHandler(&auth_handler);
#endif

    /* a2dp register */
    a2dp_avdtpcodec.codecType = AVDTP_CODEC_TYPE_SBC;
    a2dp_avdtpcodec.discoverable = 1;
    a2dp_avdtpcodec.elements = (U8 *)&a2dp_codec_elements;
    a2dp_avdtpcodec.elemLen  = 4;

#if defined(A2DP_AAC_ON)
    a2dp_aac_avdtpcodec.codecType = AVDTP_CODEC_TYPE_MPEG2_4_AAC;
    a2dp_aac_avdtpcodec.discoverable = 1;
    a2dp_aac_avdtpcodec.elements = (U8 *)&a2dp_codec_aac_elements;
    a2dp_aac_avdtpcodec.elemLen  = sizeof(a2dp_codec_aac_elements);
    avdtp_get_aacenable_callback = avdtp_Get_aacEnable_Flag;    
#endif
#if defined(A2DP_SCALABLE_ON)
    a2dp_scalable_avdtpcodec.codecType = AVDTP_CODEC_TYPE_NON_A2DP;
    a2dp_scalable_avdtpcodec.discoverable = 1;
    a2dp_scalable_avdtpcodec.elements = (U8 *)&a2dp_codec_scalable_elements;
    a2dp_scalable_avdtpcodec.elemLen  = sizeof(a2dp_codec_scalable_elements);
    TRACE("a2dp_scalable_stream = 0x%x \n", &app_bt_device.a2dp_scalable_stream);
#endif
#ifdef __A2DP_AVDTP_CP__
    a2dp_avdtpCp.cpType = AVDTP_CP_TYPE_SCMS_T;
    a2dp_avdtpCp.data = (U8 *)&a2dp_avdtpCp_securityData;
    a2dp_avdtpCp.dataLen = 1;
#endif

#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
if(app_bt_device.src_or_snk==BT_DEVICE_SRC)
{
	app_bt_device.a2dp_stream[BT_DEVICE_ID_1].type = A2DP_STREAM_TYPE_SOURCE;
	A2DP_Register(&app_bt_device.a2dp_stream[BT_DEVICE_ID_1], &a2dp_avdtpcodec,NULL, a2dp_callback);
	AVRCP_Register(&app_bt_device.avrcp_channel[BT_DEVICE_ID_1], avrcp_callback, AVRCP_TG_CATEGORY_1 | AVRCP_CT_CATEGORY_2 | AVRCP_TG_CATEGORY_2);
	//AVRCP_Register(&app_bt_device.avrcp_channel[0], avrcp_source_callback_TG, AVRCP_TG_CATEGORY_1 | AVRCP_CT_CATEGORY_2);
}
else
#endif
{
	for(uint8_t i=0;i<BT_DEVICE_NUM;i++)
	{
		//sbc
		app_bt_device.a2dp_stream[i].type = A2DP_STREAM_TYPE_SINK;		  
#ifdef __A2DP_AVDTP_CP__
		A2DP_Register(&app_bt_device.a2dp_stream[i], &a2dp_avdtpcodec, &a2dp_avdtpCp, a2dp_callback);
		
		A2DP_AddContentProtection(&app_bt_device.a2dp_stream[i], &a2dp_avdtpCp);
#else
		A2DP_Register(&app_bt_device.a2dp_stream[i], &a2dp_avdtpcodec, NULL, a2dp_callback);
#endif

#if defined(A2DP_AAC_ON)        
		//aac
		app_bt_device.a2dp_aac_stream[i].type = A2DP_STREAM_TYPE_SINK;
#if 0
#ifdef __A2DP_AVDTP_CP__
		A2DP_Register(&app_bt_device.a2dp_aac_stream[i], &a2dp_aac_avdtpcodec, &a2dp_avdtpCp, a2dp_callback);
#else
		A2DP_Register(&app_bt_device.a2dp_aac_stream[i], &a2dp_aac_avdtpcodec, NULL, a2dp_callback);
#endif
#endif
#endif
#if defined(A2DP_SCALABLE_ON)
        //ss
        app_bt_device.a2dp_scalable_stream[i].type = A2DP_STREAM_TYPE_SINK;
        A2DP_Register(&app_bt_device.a2dp_scalable_stream[i], &a2dp_scalable_avdtpcodec,NULL, a2dp_callback);
#endif
		
		/* avrcp register */
		AVRCP_Register(&app_bt_device.avrcp_channel[i], avrcp_callback, AVRCP_CT_CATEGORY_1 | AVRCP_CT_CATEGORY_2 | AVRCP_TG_CATEGORY_2);

		/* hfp register */
		HF_Register(&app_bt_device.hf_channel[i], hfp_callback);
#if defined (__HSP_ENABLE__)
		HS_Register(&app_bt_device.hs_channel[i], hsp_callback);
#endif
#if HF_VERSION_1_6 == XA_ENABLED
		app_bt_device.hf_channel[i].hf_msbc_callback = btapp_hfp_check_msbc_status;
#endif

#if HID_DEVICE == XA_ENABLED
		Hid_Register(&app_bt_device.hid_channel[i],hid_callback);
#endif
	}
}

	//app_ama_rfcomm_server_init();

    /* bt local name */
#ifdef RANDOM_BTADDR_GEN
    devinfo.btd_addr = randaddrgen_get_bt_addr();
    devinfo.ble_addr = randaddrgen_get_ble_addr();
    devinfo.localname = randaddrgen_get_btd_localname();
    devinfo.ble_name = randaddrgen_get_btd_blename();
    nvrec_dev_localname_addr_init(&devinfo);
    ME_SetLocalDeviceName((const unsigned char*)devinfo.localname, strlen(devinfo.localname) + 1);
#else
    ME_SetLocalDeviceName((const unsigned char*)BT_LOCAL_NAME, strlen(BT_LOCAL_NAME) + 1);
#endif

#if BT_BEST_SYNC_CONFIG == XA_ENABLED
    /* sync config */
    ME_SetSyncConfig(SYNC_CONFIG_PATH, SYNC_CONFIG_MAX_BUFFER, SYNC_CONFIG_CVSD_BYPASS);
#endif

    /* err report enable */
    CMGR_ScoSetEDR(EDR_ENABLED);

#if HID_DEVICE == XA_ENABLED
//            ME_SetClassOfDevice(COD_MAJOR_PERIPHERAL|COD_MINOR_PERIPH_KEYBOARD);
    ME_SetClassOfDevice(COD_MAJOR_AUDIO|COD_MAJOR_PERIPHERAL|COD_MINOR_AUDIO_HEADSET|COD_MINOR_PERIPH_KEYBOARD |COD_AUDIO|COD_RENDERING);
#else
    ME_SetClassOfDevice(COD_MAJOR_AUDIO|COD_MINOR_AUDIO_HEADSET|COD_AUDIO|COD_RENDERING);
#endif
#if (FPGA ==1) || defined(__EARPHONE_STAY_BOTH_SCAN__)
    MEC(accModeNCON) = BT_DEFAULT_ACCESS_MODE_PAIR;
#else
    MEC(accModeNCON) = BT_DEFAULT_ACCESS_MODE_NCON;
#endif

#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
	if(app_bt_device.src_or_snk==BT_DEVICE_SRC)
	{
		app_a2dp_source_init();
	}
	else
	{
		app_bt_golbal_handle_init();
	}	
#else
	app_bt_golbal_handle_init();
#endif
	app_bt_profile_connect_manager_open();
    /* ext inquiry response */
    ME_AutoCreateExtInquiryRsp(BT_INQ_EXT_RSP, INQ_EXT_RSP_LEN);
    ME_SetExtInquiryRsp(1,BT_INQ_EXT_RSP, INQ_EXT_RSP_LEN);
#if defined( __EARPHONE__)
#if (FPGA==1) || defined(APP_TEST_MODE)
    app_bt_accessmode_set(BT_DEFAULT_ACCESS_MODE_PAIR);
#endif
#endif
    SEC_SetIoCapabilities(IO_NO_IO);
    ///init bt key
    bt_key_init();

    OS_NotifyEvm();
    while(1){
#ifdef __WATCHER_DOG_RESET__
            app_wdt_ping();
#endif
        app_sysfreq_req(APP_SYSFREQ_USER_APP_1, APP_SYSFREQ_32K);
        osMessageGet(evm_queue_id, osWaitForever);
        app_sysfreq_req(APP_SYSFREQ_USER_APP_1, APP_SYSFREQ_52M);
    //    BESHCI_LockBuffer();
#ifdef __LOCK_AUDIO_THREAD__
        bool stream_a2dp_sbc_isrun = app_bt_stream_isrun(APP_BT_STREAM_A2DP_SBC);
        if (stream_a2dp_sbc_isrun)
        {
            af_lock_thread();
        }
#endif

        EVM_Process();
        Besbt_hook_proc();
#if IAG_BLE_INCLUDE == XA_ENABLED
        bes_ble_schedule();
#endif

#ifdef __LOCK_AUDIO_THREAD__
        if (stream_a2dp_sbc_isrun)
        {
            af_unlock_thread();
        }
#endif
    //    BESHCI_UNLockBuffer();
        BESHCI_Poll();

        app_check_pending_stop_sniff_op();

    }

    return 0;
}

int Besbt_get_sco_num(void)
{
    return sco_num;
}

void BesbtThread(void const *argument)
{
    besmain();
}

void BesbtInit(void)
{
    evm_queue_id = osMessageCreate(osMessageQ(evm_queue), NULL);
    /* bt */
    besbt_tid = osThreadCreate(osThread(BesbtThread), NULL);
    TRACE("BesbtThread: %x\n", besbt_tid);
}
