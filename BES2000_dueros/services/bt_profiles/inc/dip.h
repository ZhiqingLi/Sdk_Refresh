#ifndef _DIP_H_

#define _DIP_H_
#include "overide.h"
#include "sdp.h"
#if DIP_DEVICE == XA_ENABLED





#define DIP_VENDOR_ID  (0x02B0) //BesTechnic(Shanghai),Ltd
#define DIP_PRODUCT_ID (0x0000) //
#define DIP_PRODUCT_VERSION (0x001F)  //0.1.F

#ifndef SDP_DEFS_H

/* Device Identification (DI)
*/
#define ATTR_ID_SPECIFICATION_ID                0x0200
#define ATTR_ID_VENDOR_ID                       0x0201
#define ATTR_ID_PRODUCT_ID                      0x0202
#define ATTR_ID_PRODUCT_VERSION                 0x0203
#define ATTR_ID_PRIMARY_RECORD                  0x0204
#define ATTR_ID_VENDOR_ID_SOURCE                0x0205

#define BLUETOOTH_DI_SPECIFICATION              0x0103  /* 1.3 */
#define DI_VENDOR_ID_DEFAULT                    0xFFFF
#define DI_VENDOR_ID_SOURCE_BTSIG               0x0001
#define DI_VENDOR_ID_SOURCE_USBIF               0x0002
#endif
typedef struct _BtDipContext {

    SdpAttribute      dipSdpAttribute[9];
    SdpRecord         dipSdpRecord;


} BtDipContext;


typedef struct _DipPnpInfo {
    BT_BD_ADDR          bdAddr;
    /* === Internal use only === */
    U16 DevieIDSpecVersion;
    U16 VID;
    U16 PID;
    U16 ProductVersion;
    U16 PrimaryRecord;
    U16 VIDSource;
} DipPnpInfo;


typedef void (*DipCallBack)(const DipPnpInfo*,void * ptr);

typedef struct _DipClient {
    /* === Internal use only === */
    BtRemoteDevice      *remDev;
    U8                  info_index;
    DipPnpInfo          PnpInfo;
    U8                  serverId;
    SdpQueryToken       sdpToken;
    //notify the A2DP update the info of dip eg whitlist info
    //I haven't thought about how to do it for the time being. 
    //It is mainly considered that the parallel modules should
    //not be coupled to each other, or the APP layer of A2DP HF
    //should actively query this part of the information, 
    // reserve the interface...
    DipCallBack         A2dpCallback;
    //notify the HF update the info of dip eg whitlist info    
    DipCallBack         HfCallback;

	DipCallBack         callback;
} DipClient;



extern BtDipContext dipContext;

#define DIP(s) (dipContext.s)



void DIP_init(void (*cb)(const DipPnpInfo*,void * ptr));




void DipGetRemotePnpInfo(BtRemoteDevice *btDevice);
BtStatus DipQueryForService(DipClient *dev,BtRemoteDevice *btDevice);

#endif


#endif
