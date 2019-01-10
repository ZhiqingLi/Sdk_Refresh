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
#define __SMARTVOICE__
#ifdef __SMARTVOICE__
#include "app_sv_handle.h"
#include "app_sv_cmd_code.h"
#endif
#include "app_spp.h"

#define SPP_ID					0
#define SPP_MAX_PACKET_SIZE     600
#define SPP_MAX_PACKET_NUM      5

#define SPP_MAX_DATA_PACKET_SIZE    700

#ifdef __SMARTVOICE__
extern "C" APP_SV_CMD_RET_STATUS_E app_sv_data_received(uint8_t* ptrData, uint32_t dataLength);
extern "C" APP_SV_CMD_RET_STATUS_E app_sv_cmd_received(uint8_t* ptrData, uint32_t dataLength);

static void spp_read_thread(const void *arg);
osThreadDef(spp_read_thread, osPriorityAboveNormal, 4096);

osThreadId  spp_read_thread_id = NULL;
#endif

static bool isSppConnected = false;
static SppDev   spp_dev;
static BtPacket spp_txPacket[SPP_MAX_PACKET_NUM];

spp_mutex_t spp_mutex;
OsSppDev    osSppDev;

#if SPP_SERVER == XA_ENABLED
SppService  sppService;
SdpRecord   sppSdpRecord;

#define SPP(s) (spp_dev.type.sppService->s)

static app_spp_tx_done_t app_spp_tx_done_func = NULL;

#ifdef __SMARTVOICE__

#define SPP_TX_BUF_SIZE     4096
// in case (offsetToFillSppTxData + dataLen) > SPP_TX_BUF_SIZE
#define SPP_TX_BUF_EXTRA_SIZE   SPP_MAX_DATA_PACKET_SIZE 

static uint8_t sppTxBuf[SPP_TX_BUF_SIZE + SPP_TX_BUF_EXTRA_SIZE];
static uint32_t occupiedSppTxBufSize;
static uint32_t offsetToFillSppTxData;
static uint8_t* ptrSppTxBuf;

uint16_t app_spp_tx_buf_size(void)
{
    return SPP_TX_BUF_SIZE;
}

void app_spp_init_tx_buf(uint8_t* ptr)
{
    ptrSppTxBuf = ptr;
    occupiedSppTxBufSize = 0;
    offsetToFillSppTxData = 0;
}

static void app_spp_free_tx_buf(uint8_t* ptrData, uint32_t dataLen)
{
    if (occupiedSppTxBufSize > 0)
    {
        occupiedSppTxBufSize -= dataLen;
    }
    TRACE("occupiedSppTxBufSize %d", occupiedSppTxBufSize);
}

uint8_t* app_spp_fill_data_into_tx_buf(uint8_t* ptrData, uint32_t dataLen)
{
    ASSERT((occupiedSppTxBufSize + dataLen) < SPP_TX_BUF_SIZE, 
        "Pending SPP tx data has exceeded the tx buffer size !");

    uint8_t* filledPtr = ptrSppTxBuf + offsetToFillSppTxData;
    memcpy(filledPtr, ptrData, dataLen);
        
    if ((offsetToFillSppTxData + dataLen) > SPP_TX_BUF_SIZE)
    {        
        offsetToFillSppTxData = 0;
    }
    else
    {
        offsetToFillSppTxData += dataLen;
    }

    occupiedSppTxBufSize += dataLen;

    TRACE("dataLen %d offsetToFillSppTxData %d occupiedSppTxBufSize %d",
        dataLen, offsetToFillSppTxData, occupiedSppTxBufSize);
    
    return filledPtr;
}

#endif
/****************************************************************************
 * SPP SDP Entries
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * ServiceClassIDList 
 */
static const U8 SppClassId[] = {
    SDP_ATTRIB_HEADER_8BIT(3),        /* Data Element Sequence, 6 bytes */ 
    SDP_UUID_16BIT(SC_SERIAL_PORT),     /* Hands-Free UUID in Big Endian */ 
};

static const U8 SppProtoDescList[] = {
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
    SDP_UINT_8BIT(3)
};

/*
 * BluetoothProfileDescriptorList
 */
static const U8 SppProfileDescList[] = {
    SDP_ATTRIB_HEADER_8BIT(8),        /* Data element sequence, 8 bytes */

    /* Data element sequence for ProfileDescriptor, 6 bytes */
    SDP_ATTRIB_HEADER_8BIT(6),

    SDP_UUID_16BIT(SC_SERIAL_PORT),   /* Uuid16 SPP */
    SDP_UINT_16BIT(0x0102)            /* As per errata 2239 */ 
};

/*
 * * OPTIONAL *  ServiceName
 */
static const U8 SppServiceName1[] = {
    SDP_TEXT_8BIT(5),          /* Null terminated text string */ 
    'S', 'p', 'p', '1', '\0'
};

static const U8 SppServiceName2[] = {
    SDP_TEXT_8BIT(5),          /* Null terminated text string */ 
    'S', 'p', 'p', '2', '\0'
};

/* SPP attributes.
 *
 * This is a ROM template for the RAM structure used to register the
 * SPP SDP record.
 */
static const SdpAttribute sppSdpAttributes1[] = {

    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, SppClassId), 

    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, SppProtoDescList),

    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, SppProfileDescList),

    /* SPP service name*/
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), SppServiceName1),
};

static const SdpAttribute sppSdpAttributes2[] = {

    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, SppClassId), 

    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, SppProtoDescList),

    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, SppProfileDescList),

    /* SPP service name*/
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), SppServiceName2),
};

static BtStatus SppRegisterSdpServices(SppDev *dev, U8 sppId)
{
    BtStatus  status = BT_STATUS_SUCCESS;
    const U8 *cu8p;

    UNUSED_PARAMETER(dev);

    /* Register SPP SDP Attributes */
    cu8p = (const U8 *)(&sppSdpAttributes1[0]);
	OS_MemCopy((U8 *)SPP(sppSdpAttribute), cu8p, sizeof(sppSdpAttributes1));

	SPP(service).serviceId = 3;
	SPP(numPorts) = 0;

    SPP(sdpRecord)->attribs = SPP(sppSdpAttribute);
    SPP(sdpRecord)->num = 4;
    SPP(sdpRecord)->classOfDevice = COD_MAJOR_PERIPHERAL;

    return status;
}
#endif

#ifdef __SMARTVOICE__
static void spp_read_thread(const void *arg)
{
	uint8_t buffer[SPP_MAX_PACKET_SIZE];
	U16 maxBytes;
	
    while (1)
	{		
		maxBytes = SPP_MAX_PACKET_SIZE;
		
		SPP_Read(&spp_dev, (char *)buffer, &maxBytes);
		DUMP8("0x%02x ",buffer,maxBytes);
        // the first two bytes of the data packet is the fixed value 0xFFFF
        if (OP_DATA_XFER == *(uint16_t *)buffer)
        {
            //app_sv_data_received(buffer, (uint32_t)maxBytes);
        }
        // otherwise it will be a command packet
        else    
        {
            //app_sv_cmd_received(buffer, (uint32_t)maxBytes);
        }

        // loop back the received data 	
        // SPP_Write(&spp_dev, (char *)buffer, &maxBytes);
    }
}

static void app_spp_create_read_thread(void)
{
    TRACE("%s %d\n", __func__, __LINE__);
    spp_read_thread_id = osThreadCreate(osThread(spp_read_thread), NULL);
}

static void app_spp_close_read_thread(void)
{
    TRACE("%s %d\n", __func__, __LINE__);
    if(spp_read_thread_id)
    {
        osThreadTerminate(spp_read_thread_id);
        spp_read_thread_id = NULL;
    }
}

static void app_spp_send_data(uint8_t* ptrData, uint16_t length);

uint8_t inbuf[500];
uint8_t index1;
void data_send_test()
{
	for(uint16_t i;i<500;i++){
		inbuf[i]=index1;
	}
	app_spp_send_data(inbuf, 500);
	index1++;
	if(index1==256)
		index1=0;
		
}

static void spp_callback(SppDev *locDev, SppCallbackParms *Info)
{
	if (SPP_EVENT_REMDEV_CONNECTED == Info->event)
	{
		TRACE("::SPP_EVENT_REMDEV_CONNECTED %d\n", Info->event);
        isSppConnected = true;
#ifdef __SMARTVOICE__
        app_spp_create_read_thread();
        //app_smartvoice_connected(APP_SMARTVOICE_SPP_CONNECTED);
#endif
	//data_send_test();
	}
	else if (SPP_EVENT_REMDEV_DISCONNECTED == Info->event)
	{
		TRACE("::SPP_EVENT_REMDEV_DISCONNECTED %d\n", Info->event);
        isSppConnected = false;
        app_spp_close_read_thread();
        //app_smartvoice_disconnected(APP_SMARTVOICE_SPP_DISCONNECTED);
        app_spp_tx_done_func = NULL;
	}
	else if (SPP_EVENT_DATA_SENT == Info->event)
	{
        SppTxDone_t* pTxDone = (SppTxDone_t *)(Info->p.other);
        app_spp_free_tx_buf(pTxDone->pTxBuf, pTxDone->txDataLength);
        if (app_spp_tx_done_func)
        {
            //app_spp_tx_done_func();
            data_send_test();
        }		
	}
    else
    {
        TRACE("::unknown event %d\n", Info->event);
    }
}

static void app_spp_send_data(uint8_t* ptrData, uint16_t length)
{
    if (!isSppConnected)
    {
        return;
    }

    SPP_Write(&spp_dev, (char *)ptrData, &length);
}

void app_sv_send_cmd_via_spp(uint8_t* ptrData, uint32_t length)
{
    uint8_t* ptrBuf = app_spp_fill_data_into_tx_buf(ptrData, length);
    app_spp_send_data(ptrBuf, (uint16_t)length);
}




void app_sv_send_data_via_spp(uint8_t* ptrData, uint32_t length)
{
    uint8_t* ptrBuf = app_spp_fill_data_into_tx_buf(ptrData, length);
    app_spp_send_data(ptrBuf, (uint16_t)length);
}

void app_spp_register_tx_done(app_spp_tx_done_t callback)
{
	app_spp_tx_done_func = callback;
}

void app_spp_init(void)
{
    app_spp_init_tx_buf(sppTxBuf);
	spp_dev.portType = SPP_SERVER_PORT;
	spp_dev.osDev	  = (void *)&osSppDev;
#if SPP_SERVER == XA_ENABLED
	spp_dev.type.sppService = &sppService;
	spp_dev.type.sppService->sdpRecord = &sppSdpRecord;
#endif
	osSppDev.mutex = (void *)&spp_mutex;
	osSppDev.sppDev = &spp_dev;
    InitCQueue(&osSppDev.rx_queue, SPP_RECV_BUFFER_SIZE, osSppDev.sppRxBuffer);
	OS_MemSet(osSppDev.sppRxBuffer, 0, SPP_RECV_BUFFER_SIZE);
        
#if SPP_SERVER == XA_ENABLED
	SppRegisterSdpServices(&spp_dev, 0);
#endif

    SPP_InitDevice(&spp_dev, &spp_txPacket[0], SPP_MAX_PACKET_NUM);
    SPP_Open(&spp_dev, NULL, spp_callback); 
}

#endif

