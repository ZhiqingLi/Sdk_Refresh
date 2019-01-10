#ifndef __APP_BT_FUNC_H__
#define __APP_BT_FUNC_H__

#include "cmsis_os.h"
#include "hal_trace.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "me.h"
#include "a2dp.h"
#include "hf.h"
#include "hs.h"
#include "dip.h"
#include "sys/mei.h"
#include "avrcp.h"
typedef enum _bt_fn_req {
        Me_switch_sco_req                           = 0,
        ME_SwitchRole_req                           = 1, 
        ME_SetConnectionRole_req                    = 2, 
        MeDisconnectLink_req                        = 3, 
        ME_StopSniff_req                            = 4, 
        ME_SetAccessibleMode_req                    = 5, 
        Me_SetLinkPolicy_req                        = 6, 
        CMGR_SetSniffTimer_req                      = 7, 
        CMGR_SetSniffInofToAllHandlerByRemDev_req   = 8, 
        A2DP_OpenStream_req                         = 9, 
        A2DP_CloseStream_req                        = 10,
        A2DP_SetMasterRole_req                      = 11,
        HF_CreateServiceLink_req                    = 12,
        HF_DisconnectServiceLink_req                = 13,
        HF_CreateAudioLink_req                      = 14,
        HF_DisconnectAudioLink_req                  = 15,
        HF_EnableSniffMode_req                      = 16,
        HF_SetMasterRole_req                        = 17,
#if defined (__HSP_ENABLE__)                        
        HS_CreateServiceLink_req                    = 18,
        HS_CreateAudioLink_req                      = 19,
        HS_DisconnectAudioLink_req                  = 20,
        HS_EnableSniffMode_req                      = 21,
        HS_DisconnectServiceLink_req                = 22,
#endif
		BT_Custom_Func_req							= 23,
		BT_avrcp_dis_req                             = 24,

		DIP_QuryService_req 						= 26,
}bt_fn_req;

typedef void (*APP_BT_REQ_CUSTOMER_CALl_CB_T)(void *, void *);

typedef union _bt_fn_param {
    // BtStatus Me_switch_sco(uint16_t  scohandle)
    struct {
        uint16_t  scohandle;
    } Me_switch_sco_param;

    // BtStatus ME_SwitchRole(BtRemoteDevice *remDev)
    struct {
        BtRemoteDevice* remDev;
    } ME_SwitchRole_param;

    //BtConnectionRole ME_SetConnectionRole(BtConnectionRole role)
    struct {
        BtConnectionRole role;
    } BtConnectionRole_param;

    // void MeDisconnectLink(BtRemoteDevice* remDev)
    struct {
        BtRemoteDevice* remDev;
    } MeDisconnectLink_param;
    
    //BtStatus ME_StopSniff(BtRemoteDevice *remDev)
    struct {
        BtRemoteDevice* remDev;
    } ME_StopSniff_param;

    //BtStatus ME_SetAccessibleMode(BtAccessibleMode mode, const BtAccessModeInfo *info)
    struct {
        BtAccessibleMode mode;
        BtAccessModeInfo info;
    } ME_SetAccessibleMode_param;

    //BtStatus Me_SetLinkPolicy(BtRemoteDevice *remDev, BtLinkPolicy policy)
    struct {
        BtRemoteDevice *remDev;
        BtLinkPolicy policy;
    } Me_SetLinkPolicy_param;

    /*BtStatus CMGR_SetSniffTimer(CmgrHandler *Handler, 
                                BtSniffInfo* SniffInfo, 
                                TimeT Time)
       */
    struct {
        CmgrHandler *Handler;
        BtSniffInfo SniffInfo;
        TimeT Time;
    } CMGR_SetSniffTimer_param;

    /*BtStatus CMGR_SetSniffInofToAllHandlerByRemDev(BtSniffInfo* SniffInfo, 
                                                                BtRemoteDevice *RemDev)
       */
    struct {
        BtSniffInfo SniffInfo;
        BtRemoteDevice *RemDev;
    } CMGR_SetSniffInofToAllHandlerByRemDev_param;

    //BtStatus A2DP_OpenStream(A2dpStream *Stream, BT_BD_ADDR *Addr)
    struct {
        A2dpStream *Stream;
        BT_BD_ADDR *Addr;
    } A2DP_OpenStream_param;

    //BtStatus A2DP_CloseStream(A2dpStream *Stream);
    struct {
        A2dpStream *Stream;
    } A2DP_CloseStream_param;

    //BtStatus A2DP_SetMasterRole(A2dpStream *Stream, BOOL Flag);    
    struct {
        A2dpStream *Stream;
        BOOL Flag;
    } A2DP_SetMasterRole_param;

    //BtStatus HF_CreateServiceLink(HfChannel *Chan, BT_BD_ADDR *Addr)
    struct {
        HfChannel *Chan;
        BT_BD_ADDR *Addr;
    } HF_CreateServiceLink_param;

    //BtStatus HF_DisconnectServiceLink(HfChannel *Chan)
    struct {
        HfChannel *Chan;
    } HF_DisconnectServiceLink_param;

    //BtStatus HF_CreateAudioLink(HfChannel *Chan)
    struct {
        HfChannel *Chan;
    } HF_CreateAudioLink_param;

    //BtStatus HF_DisconnectAudioLink(HfChannel *Chan)
    struct {
        HfChannel *Chan;
    } HF_DisconnectAudioLink_param;

    //BtStatus HF_EnableSniffMode(HfChannel *Chan, BOOL Enable)
    struct {
        HfChannel *Chan;
        BOOL Enable;
    } HF_EnableSniffMode_param;

    //BtStatus HF_SetMasterRole(HfChannel *Chan, BOOL Flag);
    struct {
        HfChannel *Chan;
        BOOL Flag;
    } HF_SetMasterRole_param;

	struct {
		BtRemoteDevice* remDev;
		DipClient *dip_dev;
	} DIP_QuryService_param;

	struct {
		AvrcpChannel *Chan;
	} AVRCP_DisconnectServiceLink_param;

#if defined (__HSP_ENABLE__)
    //BtStatus HS_CreateServiceLink(HsChannel *Chan, BT_BD_ADDR *Addr)
    struct {
        HsChannel *Chan;
        BT_BD_ADDR *Addr;
    } HS_CreateServiceLink_param;

    //BtStatus HS_CreateAudioLink(HsChannel *Chan)
    struct {
        HsChannel *Chan;
    } HS_CreateAudioLink_param;

    //BtStatus HS_DisconnectAudioLink(HsChannel *Chan)
    struct {
        HsChannel *Chan;
    } HS_DisconnectAudioLink_param;

    //BtStatus HS_DisconnectServiceLink(HsChannel *Chan)
    struct {
        HsChannel *Chan;
    } HS_DisconnectServiceLink_param;
	
    //BtStatus HS_EnableSniffMode(HsChannel *Chan, BOOL Enable)    
    struct {
        HsChannel *Chan;
        BOOL Enable;
    } HS_EnableSniffMode_param;
#endif

	struct {
		uint32_t func_ptr;
		uint32_t param0;
		uint32_t param1;
	} CustomFunc_param;

} bt_fn_param;

typedef struct {
    uint32_t src_thread;
    uint32_t request_id;
    bt_fn_param param;
} APP_BT_MAIL;

int app_bt_mail_init(void);

int app_bt_Me_switch_sco(uint16_t  scohandle);

int app_bt_ME_SwitchRole(BtRemoteDevice* remDev);

int app_bt_ME_SetConnectionRole(BtConnectionRole role);

int app_bt_MeDisconnectLink(BtRemoteDevice* remDev);

int app_bt_ME_StopSniff(BtRemoteDevice *remDev);

int app_bt_ME_SetAccessibleMode(BtAccessibleMode mode, const BtAccessModeInfo *info);

int app_bt_Me_SetLinkPolicy(BtRemoteDevice *remDev, BtLinkPolicy policy);

int app_bt_CMGR_SetSniffTimer(CmgrHandler *Handler, 
                                        BtSniffInfo* SniffInfo, 
                                        TimeT Time);

int app_bt_CMGR_SetSniffInofToAllHandlerByRemDev(BtSniffInfo* SniffInfo, 
                            								     BtRemoteDevice *RemDev);

int app_bt_A2DP_OpenStream(A2dpStream *Stream, BT_BD_ADDR *Addr);

int app_bt_A2DP_CloseStream(A2dpStream *Stream);

int app_bt_A2DP_SetMasterRole(A2dpStream *Stream, BOOL Flag);

int app_bt_HF_CreateServiceLink(HfChannel *Chan, BT_BD_ADDR *Addr);

int app_bt_HF_DisconnectServiceLink(HfChannel *Chan);

int app_bt_HF_CreateAudioLink(HfChannel *Chan);

int app_bt_HF_DisconnectAudioLink(HfChannel *Chan);

int app_bt_HF_EnableSniffMode(HfChannel *Chan, BOOL Enable);

int app_bt_HF_SetMasterRole(HfChannel *Chan, BOOL Flag);

int app_bt_AVRCP_DisconnectServiceLink(AvrcpChannel *Chan);


void app_bt_accessible_manager_process(const BtEvent *Event);
void app_bt_role_manager_process(const BtEvent *Event);
void app_bt_sniff_manager_process(const BtEvent *Event);
void app_bt_golbal_handle_hook(const BtEvent *Event);


int app_bt_dip_QuryService(DipClient *dev, BtRemoteDevice* rem);

#if defined (__HSP_ENABLE__)
int app_bt_HS_CreateServiceLink(HsChannel *Chan, BT_BD_ADDR *Addr);

int app_bt_HS_CreateAudioLink(HsChannel *Chan);

int app_bt_HS_DisconnectAudioLink(HsChannel *Chan);

int app_bt_HS_DisconnectServiceLink(HsChannel *Chan);

int app_bt_HS_EnableSniffMode(HsChannel *Chan, BOOL Enable);

#endif

int app_bt_start_custom_function_in_bt_thread(uint32_t param0, uint32_t param1, uint32_t funcPtr);
    
#ifdef __cplusplus
}
#endif
#endif /* __APP_BT_FUNC_H__ */

