#ifndef __HIDALLOC_H__
#define __HIDALLOC_H__


typedef struct _BtHidContext {

    /* List of registered channels */ 
    ListEntry         channelList;


    /* Device Manager Variables */ 
    CmgrHandler       cmgrHandler;

    L2capPsm        hidcpsm;
    L2capPsm        hidipsm;

    SdpAttribute      hidSdpAttribute[13];
    SdpRecord         hidSdpRecord;


} BtHidContext;



extern BtHidContext hidContext;
#define HID(s) (hidContext.s)

BOOL HidAlloc(void);

#endif
