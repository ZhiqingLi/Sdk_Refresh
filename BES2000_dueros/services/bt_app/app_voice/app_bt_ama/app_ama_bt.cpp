#include <stdio.h>
#include <string.h>
extern "C" {
#include "cmsis_os.h"
#include "hal_trace.h"


#include "cqueue.h"
#include "cmsis_os.h"
#include "cqueue.h"
#include "me.h"
#include "sdp.h"
#include "spp.h"
#include "osapi.h"
#include "btconfig.h"
}

#ifdef __AMA_VOICE__
#include "app_ama_handle.h"
#include "app_ama_bt.h"

#include "hal_location.h"
#include "app_bt.h"

#define AMA_RFCOMM_MAX_PACKET_SIZE     600//for ama read
#define AMA_RFCOMM_MAX_PACKET_NUM      5


static void ama_rfcomm_read_thread(const void *arg);
osThreadDef(ama_rfcomm_read_thread, osPriorityAboveNormal, 1024*2);
osThreadId  ama_rfcomm_read_thread_id = NULL;



static bool isAma_RfcommConnected = false;
static SppDev   ama_bt_dev;
static BtPacket ama_bt_txPacket[AMA_RFCOMM_MAX_PACKET_NUM];

spp_mutex_t ama_rfcomm_mutex;
OsSppDev    osAmaDev;

#if SPP_SERVER == XA_ENABLED
SppService  ama_bt_Service;
SdpRecord   ama_SdpRecord;
#endif

#define SPP(s) (ama_bt_dev.type.sppService->s)
static app_ama_rfcomm_tx_done_t app_ama_rfcomm_tx_done_func = NULL;

#define AMA_RFCOMM_MAX_DATA_PACKET_SIZE    500

#define AMA_RFCOMM_TX_BUF_SIZE     1024*4
// in case (offsetToFillAmaTxData + dataLen) > AMA_RFCOMM_TX_BUF_SIZE
#define AMA_RFCOMM_TX_BUF_EXTRA_SIZE   AMA_RFCOMM_MAX_DATA_PACKET_SIZE 

static uint8_t ama_rfcomm_TxBuf[AMA_RFCOMM_TX_BUF_SIZE + AMA_RFCOMM_TX_BUF_EXTRA_SIZE];
static uint32_t occupiedAmaTxBufSize;
static uint32_t offsetToFillAmaTxData;
static uint8_t* ptrAma_RfcommTxBuf;

static uint16_t app_ama_rfcomm_tx_buf_size(void)
{
    return AMA_RFCOMM_TX_BUF_SIZE;
}

static void app_ama_rfcomm_init_tx_buf(uint8_t* ptr)
{
    ptrAma_RfcommTxBuf = ptr;
    occupiedAmaTxBufSize = 0;
    offsetToFillAmaTxData = 0;
}

static void app_ama_rfcomm_free_tx_buf(uint8_t* ptrData, uint32_t dataLen)
{
    if (occupiedAmaTxBufSize > 0)
    {
        occupiedAmaTxBufSize -= dataLen;
    }
    TRACE("occupiedAMA_rfcommTxBufSize %d", occupiedAmaTxBufSize);
}

static uint8_t* app_ama_rfcomm_fill_data_into_tx_buf(uint8_t* ptrData, uint32_t dataLen)
{
    ASSERT((occupiedAmaTxBufSize + dataLen) < AMA_RFCOMM_TX_BUF_SIZE, 
        "Pending SPP tx data has exceeded the tx buffer size !");

    //uint8_t* filledPtr = ptrAma_RfcommTxBuf + offsetToFillAmaTxData;
    //memcpy(filledPtr, ptrData, dataLen);

    uint8_t* filledPtr;
    
    if ((offsetToFillAmaTxData + dataLen) > AMA_RFCOMM_TX_BUF_SIZE)
    {        
        offsetToFillAmaTxData = 0;
		filledPtr = ptrAma_RfcommTxBuf + offsetToFillAmaTxData;	
    }
    else
    {
    	filledPtr = ptrAma_RfcommTxBuf + offsetToFillAmaTxData;
        offsetToFillAmaTxData += dataLen;
    }

	memcpy(filledPtr, ptrData, dataLen);

    occupiedAmaTxBufSize += dataLen;

    TRACE("dataLen %d offsetToFillAmaTxData %d occupiedSppTxBufSize %d",
        dataLen, offsetToFillAmaTxData, occupiedAmaTxBufSize);
    
    return filledPtr;
}


/****************************************************************************
 * SPP SDP Entries
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * ServiceClassIDList 
 */


/* Extend UUID */
/*---------------------------------------------------------------------------
 *
 * ServiceClassIDList
 */
//#define IOS_IAP2_PROTOCOL //for test
#ifdef IOS_IAP2_PROTOCOL
//ios uuid
static const U8 AMA_RFCOMM_128UuidClassId[] = {
    SDP_ATTRIB_HEADER_8BIT(17),        /* Data Element Sequence, 17 bytes */
    DETD_UUID + DESD_16BYTES,        /* Type & size index 0x1C */ \
            0xFF, /* Bits[128:121] of AMA UUID */ \
            0xCA, \
            0xCA, \
            0xDE, \  
            0xAF, \ 
            0xDE, \ 
            0xCA, \ 
            0xDE, \ 
            0xDE, \ 
            0xFA, \ 
            0xCA, \
            0xDE, \
            0x00, \
            0x00, \
            0x00, \
            0x00,
};
#else
static const U8 AMA_RFCOMM_128UuidClassId[] = {
    SDP_ATTRIB_HEADER_8BIT(17),        /* Data Element Sequence, 17 bytes */
    DETD_UUID + DESD_16BYTES,        /* Type & size index 0x1C */ \
            0x93, /* Bits[128:121] of AMA UUID */ \
            0x1C, \
            0x7E, \
            0x8A, \  
            0x54, \ 
            0x0F, \ 
            0x46, \ 
            0x86, \ 
            0xB7, \ 
            0x98, \ 
            0xE8, \
            0xDF, \
            0x0A, \
            0x2A, \
            0xD9, \
            0xF7,
};

#endif
static const U8 AMA_RFCOMM_128UuidProtoDescList[] = {
    SDP_ATTRIB_HEADER_8BIT(12),  /* Data element sequence, 12 bytes */ 

    /* Each element of the list is a Protocol descriptor which is a 
     * data element sequence. The first element is L2CAP which only 
     * has a UUID element.  
     */ 
    SDP_ATTRIB_HEADER_8BIT(3),   /* Data element sequence for L2CAP, 3 
                                  * bytes 
                                  */ 

    SDP_UUID_16BIT(PROT_L2CAP),  /* Uuid16 L2CAP */ 

    /* Next protocol descriptor in the list is RFCOMM. It contains two 
     * elements which are the UUID and the channel. Ultimately this 
     * channel will need to filled in with value returned by RFCOMM.  
     */ 

    /* Data element sequence for RFCOMM, 5 bytes */
    SDP_ATTRIB_HEADER_8BIT(5),

    SDP_UUID_16BIT(PROT_RFCOMM), /* Uuid16 RFCOMM */

    /* Uint8 RFCOMM channel number - value can vary */
    SDP_UINT_8BIT(2)
};

/*
 * * OPTIONAL * Language BaseId List (goes with the service name).  
 */ 
static const U8 AMA_RFCOMM_128UuidLangBaseIdList[] = {
    SDP_ATTRIB_HEADER_8BIT(9),  /* Data Element Sequence, 9 bytes */ 
    SDP_UINT_16BIT(0x656E),     /* English Language */ 
    SDP_UINT_16BIT(0x006A),     /* UTF-8 encoding */ 
    SDP_UINT_16BIT(0x0100)      /* Primary language base Id */ 
};

/*
 * * OPTIONAL *  ServiceName
 */
static const U8 AMA_RFCOMM_128UuidServiceName[] = {
    SDP_TEXT_8BIT(8),          /* Null terminated text string */ 
    'U', 'U', 'I', 'D', '1', '2', '8', '\0'
};

/* SPP attributes.
 *
 * This is a ROM template for the RAM structure used to register the
 * SPP SDP record.
 */
static const SdpAttribute AMA_RFCOMM_128UuidSdpAttributes[] = {

    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, AMA_RFCOMM_128UuidClassId),

    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, AMA_RFCOMM_128UuidProtoDescList),

    /* Language base id (Optional: Used with service name) */ 
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, AMA_RFCOMM_128UuidLangBaseIdList),

    /* SPP service name*/
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), AMA_RFCOMM_128UuidServiceName),
};

static BtStatus AMA_RFCOMM_Register128UuidSdpServices(SppDev *dev)
{
    BtStatus  status = BT_STATUS_SUCCESS;
    const U8 *cu8p;

    UNUSED_PARAMETER(dev);

    /* Register 128bit UUID SDP Attributes */
    cu8p = (const U8 *)(&AMA_RFCOMM_128UuidSdpAttributes[0]);
    OS_MemCopy((U8 *)SPP(sppSdpAttribute), cu8p, sizeof(AMA_RFCOMM_128UuidSdpAttributes));

    SPP(service).serviceId = 2;
    SPP(numPorts) = 0;

    SPP(sdpRecord)->attribs = SPP(sppSdpAttribute);
    SPP(sdpRecord)->num = 4;
    SPP(sdpRecord)->classOfDevice = COD_MAJOR_PERIPHERAL;

    return status;
}


static void ama_rfcomm_read_thread(const void *arg)
{
    uint8_t buffer[AMA_RFCOMM_MAX_PACKET_SIZE];
    U16 maxBytes;
    
    while (1)
    {
        maxBytes = AMA_RFCOMM_MAX_PACKET_SIZE;

        SPP_Read(&ama_bt_dev, (char *)buffer, &maxBytes);

        TRACE("RFCOMM:");
        DUMP8("%02x ",buffer,maxBytes);
        app_ama_rcv_data((uint8_t *)buffer, maxBytes);
        //AMA RECEIVEDD
    }
}

static void app_ama_create_rfcomm_read_thread(void)
{
    TRACE("%s %d\n", __func__, __LINE__);
	if(ama_rfcomm_read_thread_id==NULL)
    	ama_rfcomm_read_thread_id = osThreadCreate(osThread(ama_rfcomm_read_thread), NULL);
}

static void app_ama_close_rfcomm_read_thread(void)
{
	osStatus ret;

    TRACE("%s %d\n", __func__, __LINE__);
    if(ama_rfcomm_read_thread_id)
    {
        ret=osThreadTerminate(ama_rfcomm_read_thread_id);
		if(ret==osOK){
        	ama_rfcomm_read_thread_id = NULL;
			TRACE("thread end");
		}
    }
}

static void ama_rfcomm_callback(SppDev *locDev, SppCallbackParms *Info)
{
    if (SPP_EVENT_REMDEV_CONNECTED == Info->event)
    {
        TRACE("::AMA_RFCOMM_CONNECTED %d\n", Info->event);
        isAma_RfcommConnected = true;
        app_ama_create_rfcomm_read_thread();	
#ifdef __AMA_VOICE__
        app_ama_voice_connected(ama_rfcomm_connect);
#endif
        //ama rfcomm connect
        app_bt_stop_sniff(0);
        app_bt_stay_active(0);
    }
    else if (SPP_EVENT_REMDEV_DISCONNECTED == Info->event)
    {
    	BtStatus spp_dis_status=BT_STATUS_FAILED;
        TRACE("::AMA_RFCOMM_DISCONNECTED %d\n", Info->event);
		if(a2dp_service_is_connected()||app_is_hfp_service_connected()){
			//do nothing(ACL IS connected,do close spp close is not required)
		}else{
			spp_dis_status = SPP_Close(&ama_bt_dev);
			TRACE("==AMA SPP DISCONNECT STATUS:%d",spp_dis_status);
			ASSERT(spp_dis_status==BT_STATUS_SUCCESS,"spp close fail");
		}

		isAma_RfcommConnected = false;	
		
		app_ama_close_rfcomm_read_thread();
        //ama rfcomm disconnect
#ifdef __AMA_VOICE__
        app_ama_voice_disconnected(ama_rfcomm_connect);
#endif
    }
    else if (SPP_EVENT_DATA_SENT == Info->event)
    {
        TRACE("::========================-------AMA_RFCOMM_DATA_SENT %d\n", Info->event);
        SppTxDone_t* pTxDone = (SppTxDone_t *)(Info->p.other);
        app_ama_rfcomm_free_tx_buf(pTxDone->pTxBuf, pTxDone->txDataLength);
        if (app_ama_rfcomm_tx_done_func)
        {
            app_ama_rfcomm_tx_done_func();
        }		
    }
    else
    {
        TRACE("::unknown event %d\n", Info->event);
    }
}

void app_ama_rfcomm_send(uint8_t* ptrData, uint32_t length)
{
    uint8_t* ptrBuf = NULL;
    uint16_t write_len=(uint16_t) length;

    if (!isAma_RfcommConnected)
    {
        return;
    }
    ptrBuf = app_ama_rfcomm_fill_data_into_tx_buf(ptrData, length);

    TRACE("RFCOMM SEND!!");
    //DUMP8("%02x ",ptrBuf,length);
    SPP_Write(&ama_bt_dev, (char *)ptrBuf, &write_len);
}

void app_ama_register__rfcomm_tx_done(app_ama_rfcomm_tx_done_t callback)
{
    app_ama_rfcomm_tx_done_func = callback;
}

void app_ama_rfcomm_init(uint8_t portType, BtRemoteDevice *dev)
{	BtStatus spp_creat_status=BT_STATUS_FAILED;
    if (isAma_RfcommConnected)
    {
        TRACE("%s %d rfcomm has connected!!!",__func__,__LINE__);
        return;
    }

    app_ama_rfcomm_init_tx_buf(ama_rfcomm_TxBuf);

    ama_bt_dev.portType = portType;
    ama_bt_dev.osDev      = (void *)&osAmaDev;
    
    if(portType == SPP_SERVER_PORT)
    {
        ama_bt_dev.type.sppService = &ama_bt_Service;
        ama_bt_dev.type.sppService->sdpRecord = &ama_SdpRecord;
    }
    
    osAmaDev.mutex = (void *)&ama_rfcomm_mutex;
    osAmaDev.sppDev = &ama_bt_dev;
    InitCQueue(&osAmaDev.rx_queue, SPP_RECV_BUFFER_SIZE, osAmaDev.sppRxBuffer);
    OS_MemSet(osAmaDev.sppRxBuffer, 0, SPP_RECV_BUFFER_SIZE);
    
    AMA_RFCOMM_Register128UuidSdpServices(&ama_bt_dev);

    SPP_InitDevice(&ama_bt_dev, &ama_bt_txPacket[0], AMA_RFCOMM_MAX_PACKET_NUM);
    spp_creat_status=SPP_Open(&ama_bt_dev, dev, ama_rfcomm_callback);
	ASSERT(spp_creat_status==BT_STATUS_SUCCESS,"spp open fail");
	TRACE("!!!SPP_Open  status:%d",spp_creat_status);
}

void app_ama_rfcomm_client_init(BtRemoteDevice *dev)
{
    TRACE("%s %d ",__func__,__LINE__);
#if SPP_CLIENT == XA_ENABLED
    app_ama_rfcomm_init(SPP_CLIENT_PORT,dev);
#else
    TRACE("!!!SPP_CLIENT is no support ");
#endif
}


void app_ama_rfcomm_server_init(void)
{
    TRACE("%s %d ",__func__,__LINE__);
#if SPP_SERVER == XA_ENABLED
    app_ama_rfcomm_init(SPP_SERVER_PORT,NULL);
#else
    TRACE("!!!SPP_SERVER is no support ");
#endif
}

void app_ama_rfcomm_disconnlink(void)//only used for device force disconnect
{
    TRACE("%s %d ",__func__,__LINE__);
    if(isAma_RfcommConnected == true)
    {
    	SPP_Close(&ama_bt_dev);
    }
    return;
}

void app_ama_link_free_after_rfcomm_dis()
{
	if((ama_bt_dev.portType!=SPP_SERVER_PORT)&&(ama_bt_dev.portType!=SPP_CLIENT_PORT)){
		TRACE("ama rfcomm not open,not required to close");
		return;
	}
	BtStatus link_dis_status=BT_STATUS_FAILED;
	TRACE("app_ama_link_free_after_rfcomm_dis");
    if(isAma_RfcommConnected == false){
    	link_dis_status = SPP_Close(&ama_bt_dev);
		//ASSERT(link_dis_status == BT_STATUS_SUCCESS,"rfcomm free fail");
    }else if((app_is_link_connected())&&(isAma_RfcommConnected==true)){
		link_dis_status = SPP_Close(&ama_bt_dev);//send dis cmd to app
    }
}
#endif

