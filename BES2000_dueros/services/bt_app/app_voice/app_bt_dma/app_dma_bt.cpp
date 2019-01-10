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

#ifdef __DMA_VOICE__
#include "app_dma_handle.h"
#include "app_dma_bt.h"

#include "hal_location.h"
#include "app_bt.h"

#define DMA_RFCOMM_MAX_PACKET_SIZE     600//for dma read
#define DMA_RFCOMM_MAX_PACKET_NUM      5


static void dma_rfcomm_read_thread(const void *arg);
osThreadDef(dma_rfcomm_read_thread, osPriorityAboveNormal, 4096);//change cai.zhong 20181224 for stack size
osThreadId  dma_rfcomm_read_thread_id = NULL;



static bool isDma_RfcommConnected = false;
static SppDev   dma_bt_dev;
static BtPacket dma_bt_txPacket[DMA_RFCOMM_MAX_PACKET_NUM];

spp_mutex_t dma_rfcomm_mutex;
OsSppDev    osDmaDev;

#if SPP_SERVER == XA_ENABLED
SppService  dma_bt_Service;
SdpRecord   dma_SdpRecord;
#endif

#define SPP(s) (dma_bt_dev.type.sppService->s)
static app_dma_rfcomm_tx_done_t app_dma_rfcomm_tx_done_func = NULL;

#define DMA_RFCOMM_MAX_DATA_PACKET_SIZE    700

#define DMA_RFCOMM_TX_BUF_SIZE     1024*4
// in case (offsetToFillDmaTxData + dataLen) > DMA_RFCOMM_TX_BUF_SIZE
#define DMA_RFCOMM_TX_BUF_EXTRA_SIZE   DMA_RFCOMM_MAX_DATA_PACKET_SIZE 

static uint8_t dma_rfcomm_TxBuf[DMA_RFCOMM_TX_BUF_SIZE + DMA_RFCOMM_TX_BUF_EXTRA_SIZE];
static uint32_t occupiedDmaTxBufSize;
static uint32_t offsetToFillDmaTxData;
static uint8_t* ptrDma_RfcommTxBuf;

static uint16_t app_dma_rfcomm_tx_buf_size(void)
{
    return DMA_RFCOMM_TX_BUF_SIZE;
}

static void app_dma_rfcomm_init_tx_buf(uint8_t* ptr)
{
    ptrDma_RfcommTxBuf = ptr;
    occupiedDmaTxBufSize = 0;
    offsetToFillDmaTxData = 0;
}

static void app_dma_rfcomm_free_tx_buf(uint8_t* ptrData, uint32_t dataLen)
{
    if (occupiedDmaTxBufSize > 0)
    {
        occupiedDmaTxBufSize -= dataLen;
    }
    TRACE("occupiedDMA_rfcommTxBufSize %d", occupiedDmaTxBufSize);
}

static uint8_t* app_dma_rfcomm_fill_data_into_tx_buf(uint8_t* ptrData, uint32_t dataLen)
{
    ASSERT((occupiedDmaTxBufSize + dataLen) < DMA_RFCOMM_TX_BUF_SIZE, 
        "Pending SPP tx data has exceeded the tx buffer size !");

    //uint8_t* filledPtr = ptrDma_RfcommTxBuf + offsetToFillDmaTxData;
    //memcpy(filledPtr, ptrData, dataLen);

    uint8_t* filledPtr;
    
    if ((offsetToFillDmaTxData + dataLen) > DMA_RFCOMM_TX_BUF_SIZE)
    {        
        offsetToFillDmaTxData = 0;
		filledPtr = ptrDma_RfcommTxBuf + offsetToFillDmaTxData;	
    }
    else
    {
    	filledPtr = ptrDma_RfcommTxBuf + offsetToFillDmaTxData;
        offsetToFillDmaTxData += dataLen;
    }

	memcpy(filledPtr, ptrData, dataLen);

    occupiedDmaTxBufSize += dataLen;

    TRACE("dataLen %d offsetToFillDmaTxData %d occupiedSppTxBufSize %d",
        dataLen, offsetToFillDmaTxData, occupiedDmaTxBufSize);
    
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

static const U8 DMA_RFCOMM_128UuidClassId[] = {
    SDP_ATTRIB_HEADER_8BIT(17),        /* Data Element Sequence, 17 bytes */
    DETD_UUID + DESD_16BYTES,        /* Type & size index 0x1C */ \
            0x51, /* Bits[128:121] of DMA UUID */ \
            0xDB, \
            0xA1, \
            0x09, \  
            0x5B, \ 
            0xA9, \ 
            0x49, \
            0x81, \ 
            0x96, \ 
            0xB7, \ 
            0x6A, \ 
            0xFE, \
            0x13, \
            0x20, \
            0x93, \
            0xDE,
};


static const U8 DMA_RFCOMM_128UuidProtoDescList[] = {
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
static const U8 DMA_RFCOMM_128UuidLangBaseIdList[] = {
    SDP_ATTRIB_HEADER_8BIT(9),  /* Data Element Sequence, 9 bytes */ 
    SDP_UINT_16BIT(0x656E),     /* English Language */ 
    SDP_UINT_16BIT(0x006A),     /* UTF-8 encoding */ 
    SDP_UINT_16BIT(0x0100)      /* Primary language base Id */ 
};

/*
 * * OPTIONAL *  ServiceName
 */
static const U8 DMA_RFCOMM_128UuidServiceName[] = {
    SDP_TEXT_8BIT(8),          /* Null terminated text string */ 
    'U', 'U', 'I', 'D', '1', '2', '8', '\0'
};

/* SPP attributes.
 *
 * This is a ROM template for the RAM structure used to register the
 * SPP SDP record.
 */
static const SdpAttribute DMA_RFCOMM_128UuidSdpAttributes[] = {

    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, DMA_RFCOMM_128UuidClassId),

    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, DMA_RFCOMM_128UuidProtoDescList),

    /* Language base id (Optional: Used with service name) */ 
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, DMA_RFCOMM_128UuidLangBaseIdList),

    /* SPP service name*/
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), DMA_RFCOMM_128UuidServiceName),
};

static BtStatus DMA_RFCOMM_Register128UuidSdpServices(SppDev *dev)
{
    BtStatus  status = BT_STATUS_SUCCESS;
    const U8 *cu8p;

    UNUSED_PARAMETER(dev);

    /* Register 128bit UUID SDP Attributes */
    cu8p = (const U8 *)(&DMA_RFCOMM_128UuidSdpAttributes[0]);
    OS_MemCopy((U8 *)SPP(sppSdpAttribute), cu8p, sizeof(DMA_RFCOMM_128UuidSdpAttributes));

    SPP(service).serviceId = 0;
    SPP(numPorts) = 0;

    SPP(sdpRecord)->attribs = SPP(sppSdpAttribute);
    SPP(sdpRecord)->num = 4;
    SPP(sdpRecord)->classOfDevice = COD_MAJOR_PERIPHERAL;

    return status;
}


static void dma_rfcomm_read_thread(const void *arg)
{
    uint8_t buffer[DMA_RFCOMM_MAX_PACKET_SIZE];
    U16 maxBytes;
    
    while (1)
    {
        maxBytes = DMA_RFCOMM_MAX_PACKET_SIZE;

        SPP_Read(&dma_bt_dev, (char *)buffer, &maxBytes);

        TRACE("RFCOMM:");
        DUMP8("%02x ",buffer,maxBytes);
        dma_data_received((uint8_t *)buffer, maxBytes);
        //DMA RECEIVEDD
    }
}

static void app_dma_create_rfcomm_read_thread(void)
{
    TRACE("%s %d\n", __func__, __LINE__);
	if(dma_rfcomm_read_thread_id==NULL)
    	dma_rfcomm_read_thread_id = osThreadCreate(osThread(dma_rfcomm_read_thread), NULL);
}

static void app_dma_close_rfcomm_read_thread(void)
{
    TRACE("%s %d\n", __func__, __LINE__);
    if(dma_rfcomm_read_thread_id)
    {
        osThreadTerminate(dma_rfcomm_read_thread_id);
        dma_rfcomm_read_thread_id = NULL;
    }
}

static void dma_rfcomm_callback(SppDev *locDev, SppCallbackParms *Info)
{
    if (SPP_EVENT_REMDEV_CONNECTED == Info->event)
    {
        TRACE("::DMA_RFCOMM_CONNECTED %d\n", Info->event);
        isDma_RfcommConnected = true;
        app_dma_create_rfcomm_read_thread();	
#ifdef __DMA_VOICE__
        dma_spp_connected();
#endif
        //dma rfcomm connect
        app_bt_stop_sniff(0);
        app_bt_stay_active(0);
    }
    else if (SPP_EVENT_REMDEV_DISCONNECTED == Info->event)
    {
    	BtStatus spp_dis_status=BT_STATUS_FAILED;
        TRACE("::DMA_RFCOMM_DISCONNECTED %d\n", Info->event);
		if(a2dp_service_is_connected()||app_is_hfp_service_connected()){
			//do nothing(ACL IS connected,do close spp close is not required)
		}else{
			osDelay(100);
			spp_dis_status = SPP_Close(&dma_bt_dev);
			TRACE("==DMA SPP DISCONNECT STATUS:%d",spp_dis_status);
			ASSERT(spp_dis_status==BT_STATUS_SUCCESS,"spp close fail");
		}

		isDma_RfcommConnected = false;	
		
		app_dma_close_rfcomm_read_thread();
        //dma rfcomm disconnect
#ifdef __DMA_VOICE__
        dma_spp_disconnected();
#endif
    }
    else if (SPP_EVENT_DATA_SENT == Info->event)
    {
        TRACE("::DMA_RFCOMM_DATA_SENT %d\n", Info->event);
        SppTxDone_t* pTxDone = (SppTxDone_t *)(Info->p.other);
        app_dma_rfcomm_free_tx_buf(pTxDone->pTxBuf, pTxDone->txDataLength);
        if (app_dma_rfcomm_tx_done_func)
        {
            app_dma_rfcomm_tx_done_func();
        }		
    }
    else
    {
        TRACE("::unknown event %d\n", Info->event);
    }
}

void app_dma_rfcomm_send(uint8_t* ptrData, uint32_t length)
{
    uint8_t* ptrBuf = NULL;
    uint16_t write_len=(uint16_t) length;

    if (!isDma_RfcommConnected)
    {
        return;
    }
    ptrBuf = app_dma_rfcomm_fill_data_into_tx_buf(ptrData, length);

    TRACE("RFCOMM SEND!!");
    //DUMP8("%02x ",ptrBuf,length);
    SPP_Write(&dma_bt_dev, (char *)ptrBuf, &write_len);
}

void app_dma_register__rfcomm_tx_done(app_dma_rfcomm_tx_done_t callback)
{
    app_dma_rfcomm_tx_done_func = callback;
}

void app_dma_rfcomm_init(uint8_t portType, BtRemoteDevice *dev)
{	BtStatus spp_creat_status=BT_STATUS_FAILED;
    if (isDma_RfcommConnected)
    {
        TRACE("%s %d rfcomm has connected!!!",__func__,__LINE__);
        return;
    }

    app_dma_rfcomm_init_tx_buf(dma_rfcomm_TxBuf);

    dma_bt_dev.portType = portType;
    dma_bt_dev.osDev      = (void *)&osDmaDev;
    
    if(portType == SPP_SERVER_PORT)
    {
        dma_bt_dev.type.sppService = &dma_bt_Service;
        dma_bt_dev.type.sppService->sdpRecord = &dma_SdpRecord;
    }
    
    osDmaDev.mutex = (void *)&dma_rfcomm_mutex;
    osDmaDev.sppDev = &dma_bt_dev;
    InitCQueue(&osDmaDev.rx_queue, SPP_RECV_BUFFER_SIZE, osDmaDev.sppRxBuffer);
    OS_MemSet(osDmaDev.sppRxBuffer, 0, SPP_RECV_BUFFER_SIZE);
    
    DMA_RFCOMM_Register128UuidSdpServices(&dma_bt_dev);

    SPP_InitDevice(&dma_bt_dev, &dma_bt_txPacket[0], DMA_RFCOMM_MAX_PACKET_NUM);
    spp_creat_status=SPP_Open(&dma_bt_dev, dev, dma_rfcomm_callback);
	ASSERT(spp_creat_status==BT_STATUS_SUCCESS,"spp open fail");
	TRACE("!!!SPP_Open  status:%d",spp_creat_status);
}

void app_dma_rfcomm_client_init(BtRemoteDevice *dev)
{
    TRACE("%s %d ",__func__,__LINE__);
#if SPP_CLIENT == XA_ENABLED
    app_dma_rfcomm_init(SPP_CLIENT_PORT,dev);
#else
    TRACE("!!!SPP_CLIENT is no support ");
#endif
}


void app_dma_rfcomm_server_init(void)
{
    TRACE("%s %d ",__func__,__LINE__);
#if SPP_SERVER == XA_ENABLED
    app_dma_rfcomm_init(SPP_SERVER_PORT,NULL);
#else
    TRACE("!!!SPP_SERVER is no support ");
#endif
}

void app_dma_rfcomm_disconnlink(void)//only used for device force disconnect
{
    TRACE("%s %d ",__func__,__LINE__);
    if(isDma_RfcommConnected == true)
    {
    	SPP_Close(&dma_bt_dev);
    }
    return;
}

void app_dma_link_free_after_rfcomm_dis()
{
	BtStatus link_dis_status=BT_STATUS_FAILED;
	TRACE("app_dma_link_free_after_rfcomm_dis");
    if(isDma_RfcommConnected == false)
    {
    	link_dis_status = SPP_Close(&dma_bt_dev);
        //change begin by cai.zhong 20190103 for rfcomm
		//ASSERT(link_dis_status == BT_STATUS_SUCCESS,"rfcomm free fail");
        if(link_dis_status != BT_STATUS_SUCCESS)
            TRACE("dma rfcomm free fail!!!");
        //change end by cai.zhong 20190103 for rfcomm
    }else if((app_is_link_connected())&&(isDma_RfcommConnected==true)){
		link_dis_status = SPP_Close(&dma_bt_dev);//send dis cmd to app
    }
}
#endif

