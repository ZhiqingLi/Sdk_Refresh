
#include "hal_trace.h"
#include "hal_aud.h"
#include "apps.h"
#include "app_thread.h"
#include "app_status_ind.h"

#include "nvrecord.h"
#include "besbt.h"

extern "C" {
#include "besbt_cfg.h"
#include "eventmgr.h"
#include "me.h"
#include "sec.h"
#include "a2dp.h"
#include "avdtp.h"
#include "avctp.h"
#include "avrcp.h"
#include "hf.h"
#include "hs.h"
#include "sys/mei.h"
#include "app.h"
}
#include "btalloc.h"
#include "btapp.h"
#include "app_bt.h"
#include "app_bt_func.h"
#include "bt_drv_interface.h"
#include "app_dip.h"
#ifdef __THIRDPARTY
#include "app_thirdparty.h"
#endif
//add begin by cai.zhong 20190103 for rfcomm
#if defined(__DMA_VOICE__)
#include "app_dma_bt.h"
#endif
//add end by cai.zhong 20190103 for rfcomm

static bool ble_force_stop=true;

#if defined(__EARPHONE_STAY_BCR_SLAVE__) && defined(__BT_ONE_BRING_TWO__)
#error can not defined at the same time.
#endif

extern "C" BOOL ME_IsOpInProgress(BtRemoteDevice *remDev);

extern struct BT_DEVICE_T  app_bt_device;
U16 bt_accessory_feature_feature = HF_CUSTOM_FEATURE_SUPPORT;

#ifdef __BT_ONE_BRING_TWO__
BtDeviceRecord record2_copy;
uint8_t record2_avalible;

#endif

BtAccessibleMode g_bt_access_mode = BAM_NOT_ACCESSIBLE;

#define APP_BT_PROFILE_BOTH_SCAN_MS (11000)
#define APP_BT_PROFILE_PAGE_SCAN_MS (4000)

osTimerId app_bt_accessmode_timer = NULL;
BtAccessibleMode app_bt_accessmode_timer_argument = BAM_NOT_ACCESSIBLE;
static int app_bt_accessmode_timehandler(void const *param);
osTimerDef (APP_BT_ACCESSMODE_TIMER, (void (*)(void const *))app_bt_accessmode_timehandler);                      // define timers

BtRemoteDevice* app_bt_get_remoteDev(uint8_t deviceId);

extern void app_spp_init(void);

#ifdef __AMA_VOICE__
void app_ama_rfcomm_client_init(BtRemoteDevice *dev);
void app_ama_rfcomm_server_init(void);
void app_ama_rfcomm_disconnlink(void);
void app_ama_link_free_after_rfcomm_dis();

#define APP_AMA_CREATE_SPP_CLIENT_TIMEOUT_MS (100)
osTimerId app_ama_create_spp_client_timerId = NULL;
uint8_t  app_ama_create_spp_client_devId = 0;
static void app_ama_create_spp_client_timehandler(void const *param){
    //app_ama_rfcomm_client_init(app_bt_get_remoteDev(app_ama_create_spp_client_devId));
	app_ama_rfcomm_server_init();

	app_spp_init();
}
osTimerDef (AMA_CREATE_SPP_CLIENT_TIMEOUT, app_ama_create_spp_client_timehandler);                      // define timers
#endif

//add begin by cai.zhong 20190103 for rfcomm
#ifdef __DMA_VOICE__
#define APP_DMA_CREATE_SPP_CLIENT_TIMEOUT_MS (100)
osTimerId app_dma_create_spp_client_timerId = NULL;
uint8_t  app_dma_create_spp_client_devId = 0;
static void app_dma_create_spp_client_timehandler(void const *param){
	app_dma_rfcomm_server_init();
}
osTimerDef (DMA_CREATE_SPP_CLIENT_TIMEOUT, app_dma_create_spp_client_timehandler);                      // define timers
#endif
//add begin by cai.zhong 20190103 for rfcomm

typedef struct
{
    BtRemoteDevice  *remDev;
    BtLinkPolicy    policy;
} BT_SET_LINKPOLICY_REQ_T;
#define PENDING_SET_LINKPOLICY_REQ_BUF_CNT  5
static BT_SET_LINKPOLICY_REQ_T pending_set_linkpolicy_req[PENDING_SET_LINKPOLICY_REQ_BUF_CNT];

static uint8_t pending_set_linkpolicy_in_cursor = 0;
static uint8_t pending_set_linkpolicy_out_cursor = 0;

static void app_bt_set_linkpolicy(BtRemoteDevice *remDev, BtLinkPolicy policy);
static BT_SET_LINKPOLICY_REQ_T* app_bt_pop_pending_set_linkpolicy(void);

static int app_bt_accessmode_timehandler(void const *param)
{
    const BtAccessModeInfo info = { BT_DEFAULT_INQ_SCAN_INTERVAL,
                                    BT_DEFAULT_INQ_SCAN_WINDOW,
                                    BT_DEFAULT_PAGE_SCAN_INTERVAL,
                                    BT_DEFAULT_PAGE_SCAN_WINDOW };
    BtAccessibleMode accMode = *(BtAccessibleMode*)(param);

    OS_LockStack();
    if (accMode == BAM_CONNECTABLE_ONLY){
        app_bt_accessmode_timer_argument = BAM_GENERAL_ACCESSIBLE;
        osTimerStart(app_bt_accessmode_timer, APP_BT_PROFILE_PAGE_SCAN_MS);
    }else if(accMode == BAM_GENERAL_ACCESSIBLE){
        app_bt_accessmode_timer_argument = BAM_CONNECTABLE_ONLY;
        osTimerStart(app_bt_accessmode_timer, APP_BT_PROFILE_BOTH_SCAN_MS);
    }
    app_bt_ME_SetAccessibleMode(accMode, &info);
    TRACE("app_bt_accessmode_timehandler accMode=%x",accMode);
    OS_UnlockStack();
    return 0;
}


void app_bt_accessmode_set(BtAccessibleMode mode)
{
    const BtAccessModeInfo info = { BT_DEFAULT_INQ_SCAN_INTERVAL,
                                    BT_DEFAULT_INQ_SCAN_WINDOW,
                                    BT_DEFAULT_PAGE_SCAN_INTERVAL,
                                    BT_DEFAULT_PAGE_SCAN_WINDOW };
    OS_LockStack();
    g_bt_access_mode = mode;
    if (g_bt_access_mode == BAM_GENERAL_ACCESSIBLE){
        app_bt_accessmode_timehandler(&g_bt_access_mode);
    }else{
        osTimerStop(app_bt_accessmode_timer);
        app_bt_ME_SetAccessibleMode(g_bt_access_mode, &info);
        TRACE("app_bt_accessmode_set access_mode=%x",g_bt_access_mode);
    }
    OS_UnlockStack();
}


void PairingTransferToConnectable(void)
{
    int activeCons;
    OS_LockStack();
    activeCons = MEC(activeCons);
    OS_UnlockStack();
    TRACE("%s",__func__);

    if(activeCons == 0){
        TRACE("!!!PairingTransferToConnectable  BAM_CONNECTABLE_ONLY\n");
        app_bt_accessmode_set_req(BAM_CONNECTABLE_ONLY);
    }
}

int app_bt_state_checker(void)
{
    uint8_t i;
    BtRemoteDevice *remDev = NULL;
    CmgrHandler    *cmgrHandler;

    for (i=0; i<BT_DEVICE_NUM; i++){
        remDev = MeEnumerateRemoteDevices(i);
        cmgrHandler = CMGR_GetAclHandler(remDev);
        TRACE("checker: id:%d state:%d mode:%d role:%d cmghdl:%x sniffInterva:%d/%d IsAudioUp:%d", i, remDev->state, remDev->mode,remDev->role, cmgrHandler,
                                                                 cmgrHandler->sniffInterval, cmgrHandler->sniffInfo.maxInterval, CMGR_IsAudioUp(cmgrHandler));
        DUMP8("0x%02x ", remDev->bdAddr.addr, BD_ADDR_SIZE);
        TRACE("remDev:%x a2dp State:%d remDev:%x hf_channel Connected:%d remDev:%x ",
                                remDev,
                                app_bt_device.a2dp_connected_stream[i]?A2DP_GetStreamState(app_bt_device.a2dp_connected_stream[i]):A2DP_STREAM_STATE_CLOSED,
                                 app_bt_device.a2dp_connected_stream[i]?app_bt_device.a2dp_connected_stream[i]->device->cmgrHandler.remDev:NULL,
                                HF_IsACLConnected(&app_bt_device.hf_channel[i]),
                                app_bt_device.hf_channel[i].cmgrHandler.remDev);

#if defined (__HSP_ENABLE__)
        TRACE("hs_channel Connected:%d remDev:%x ",
                        HS_IsACLConnected(&app_bt_device.hs_channel[i]),
                        app_bt_device.hs_channel[i].cmgrHandler.remDev);
#endif

    }
    return 0;
}

void app_bt_accessible_manager_process(const BtEvent *Event)
{
#ifdef __BT_ONE_BRING_TWO__
    static uint8_t opening_reconnect_cnf_cnt = 0;
    if (app_bt_profile_connect_openreconnecting(NULL)){
        if (Event->eType == BTEVENT_LINK_CONNECT_CNF){
            opening_reconnect_cnf_cnt++;
        }
        if (record2_avalible){
            if (opening_reconnect_cnf_cnt<2){
                return;
            }
        }
    }
#endif
    switch (Event->eType) {
        case BTEVENT_LINK_CONNECT_CNF:
        case BTEVENT_LINK_CONNECT_IND:
            TRACE("BTEVENT_LINK_CONNECT_IND/CNF activeCons:%d",MEC(activeCons));
#if defined(__EARPHONE__)   && !defined(FPGA)
            app_stop_10_second_timer(APP_PAIR_TIMER_ID);
#endif
#ifdef __BT_ONE_BRING_TWO__
             if(MEC(activeCons) == 0){
#ifdef __EARPHONE_STAY_BOTH_SCAN__
                app_bt_accessmode_set_req(BT_DEFAULT_ACCESS_MODE_PAIR);
#else
                app_bt_accessmode_set_req(BAM_CONNECTABLE_ONLY);
#endif
            }else if(MEC(activeCons) == 1){
                app_bt_accessmode_set_req(BAM_CONNECTABLE_ONLY);
            }else if(MEC(activeCons) >= 2){
                app_bt_accessmode_set_req(BAM_NOT_ACCESSIBLE);
            }
#else
            if(MEC(activeCons) == 0){
#ifdef __EARPHONE_STAY_BOTH_SCAN__
                app_bt_accessmode_set_req(BT_DEFAULT_ACCESS_MODE_PAIR);
#else
                app_bt_accessmode_set_req(BAM_CONNECTABLE_ONLY);
#endif
            }else if(MEC(activeCons) >= 1){
                app_bt_accessmode_set_req(BAM_NOT_ACCESSIBLE);
            }
#endif
            break;
        case BTEVENT_LINK_DISCONNECT:
            TRACE("DISCONNECT activeCons:%d",MEC(activeCons));
#ifdef __EARPHONE_STAY_BOTH_SCAN__
#ifdef __BT_ONE_BRING_TWO__
            if(MEC(activeCons) == 0){
                app_bt_accessmode_set_req(BT_DEFAULT_ACCESS_MODE_PAIR);
            }else if(MEC(activeCons) == 1){
                app_bt_accessmode_set_req(BAM_CONNECTABLE_ONLY);
            }else if(MEC(activeCons) >= 2){
                app_bt_accessmode_set_req(BAM_NOT_ACCESSIBLE);
            }
#else
            app_bt_accessmode_set_req(BT_DEFAULT_ACCESS_MODE_PAIR);
#endif
#else
            app_bt_accessmode_set_req(BAM_CONNECTABLE_ONLY);
#endif
            break;
#ifdef __BT_ONE_BRING_TWO__
        case BTEVENT_SCO_CONNECT_IND:
        case BTEVENT_SCO_CONNECT_CNF:
            if(MEC(activeCons) == 1){
                app_bt_accessmode_set_req(BAM_NOT_ACCESSIBLE);
            }
            break;
        case BTEVENT_SCO_DISCONNECT:
            if(MEC(activeCons) == 1){
                app_bt_accessmode_set_req(BAM_CONNECTABLE_ONLY);
            }
            break;
#endif
        default:
            break;
    }
}

#define APP_BT_SWITCHROLE_LIMIT (2)
void app_bt_role_manager_process(const BtEvent *Event)
{
    static BtRemoteDevice *opRemDev = NULL;
    static uint8_t switchrole_cnt = 0;
    BtRemoteDevice *remDev = NULL;

    //on phone connecting
    switch (Event->eType) {
        case BTEVENT_LINK_CONNECT_IND:
            if(Event->errCode == BEC_NO_ERROR){
                if (MEC(activeCons) == 1){
                    switch (Event->p.remDev->role) {
                        case BCR_MASTER:
                        case BCR_PMASTER:
                            TRACE("CONNECT_IND try to role %x\n", Event->p.remDev);
                            //curr connectrot try to role
                            opRemDev = Event->p.remDev;
                            switchrole_cnt = 0;
                            app_bt_Me_SetLinkPolicy(Event->p.remDev, BLP_MASTER_SLAVE_SWITCH|BLP_SNIFF_MODE);
                            break;
                        case BCR_SLAVE:
                        case BCR_PSLAVE:
                        case BCR_ANY:
                        case BCR_UNKNOWN:
                        default:
                            TRACE("CONNECT_IND disable role %x\n", Event->p.remDev);
                            //disable roleswitch when 1 connect
                            app_bt_Me_SetLinkPolicy(Event->p.remDev, BLP_SNIFF_MODE);
                            break;
                    }
                    //set next connector to master
                    app_bt_ME_SetConnectionRole(BCR_MASTER);
                }else if (MEC(activeCons) > 1){
                    switch (Event->p.remDev->role) {
                        case BCR_MASTER:
                        case BCR_PMASTER:
                            TRACE("CONNECT_IND disable role %x\n", Event->p.remDev);
                            //disable roleswitch
                            app_bt_Me_SetLinkPolicy(Event->p.remDev, BLP_SNIFF_MODE);
                            break;
                        case BCR_SLAVE:
                        case BCR_PSLAVE:
                        case BCR_ANY:
                        case BCR_UNKNOWN:
                        default:
                            //disconnect slave
                            TRACE("CONNECT_IND disconnect slave %x\n", Event->p.remDev);
                            app_bt_MeDisconnectLink(Event->p.remDev);
                            break;
                    }
                    //set next connector to master
                    app_bt_ME_SetConnectionRole(BCR_MASTER);
                }
            }
            break;
        case BTEVENT_LINK_CONNECT_CNF:
            if (MEC(activeCons) == 1){
                switch (Event->p.remDev->role) {
                    case BCR_MASTER:
                    case BCR_PMASTER:
                        TRACE("CONNECT_CNF try to role %x\n", Event->p.remDev);
                        //curr connectrot try to role
                        opRemDev = Event->p.remDev;
                        switchrole_cnt = 0;
                        app_bt_Me_SetLinkPolicy(Event->p.remDev, BLP_MASTER_SLAVE_SWITCH|BLP_SNIFF_MODE);
                        app_bt_ME_SwitchRole(Event->p.remDev);
                        break;
                    case BCR_SLAVE:
                    case BCR_PSLAVE:
                    case BCR_ANY:
                    case BCR_UNKNOWN:
                    default:
                        TRACE("CONNECT_CNF disable role %x\n", Event->p.remDev);
                        //disable roleswitch
                        app_bt_Me_SetLinkPolicy(Event->p.remDev, BLP_SNIFF_MODE);
                        break;
                }
                //set next connector to master
                app_bt_ME_SetConnectionRole(BCR_MASTER);
            }else if (MEC(activeCons) > 1){
                switch (Event->p.remDev->role) {
                    case BCR_MASTER:
                    case BCR_PMASTER :
                        TRACE("CONNECT_CNF disable role %x\n", Event->p.remDev);
                        //disable roleswitch
                        app_bt_Me_SetLinkPolicy(Event->p.remDev, BLP_SNIFF_MODE);
                        break;
                    case BCR_SLAVE:
                    case BCR_ANY:
                    case BCR_UNKNOWN:
                    default:
                        //disconnect slave
                        TRACE("CONNECT_CNF disconnect slave %x\n", Event->p.remDev);
                        app_bt_MeDisconnectLink(Event->p.remDev);
                        break;
                }
                //set next connector to master
                app_bt_ME_SetConnectionRole(BCR_MASTER);
            }
            break;
        case BTEVENT_LINK_DISCONNECT:
            if (opRemDev == Event->p.remDev){
                opRemDev = NULL;
                switchrole_cnt = 0;
            }
            if (MEC(activeCons) == 0){
                for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                    if(app_bt_device.a2dp_connected_stream[i])
                        app_bt_A2DP_SetMasterRole(app_bt_device.a2dp_connected_stream[i], FALSE);
                    app_bt_HF_SetMasterRole(&app_bt_device.hf_channel[i], FALSE);
                }
                app_bt_ME_SetConnectionRole(BCR_ANY);
            }else if (MEC(activeCons) == 1){
                //set next connector to master
                app_bt_ME_SetConnectionRole(BCR_MASTER);
            }
            break;
        case BTEVENT_ROLE_CHANGE:
        if (opRemDev == Event->p.remDev){
                switch (Event->p.roleChange.newRole) {
                    case BCR_MASTER:
                        if (++switchrole_cnt<=APP_BT_SWITCHROLE_LIMIT){
                            app_bt_ME_SwitchRole(Event->p.remDev);
                        }else{
                            TRACE("ROLE TO SLAVE FAILED remDev%x cnt:%d\n", Event->p.remDev, switchrole_cnt);
                            opRemDev = NULL;
                            switchrole_cnt = 0;
                        }
                        break;
                    case BCR_SLAVE:
                        TRACE("ROLE TO SLAVE SUCCESS remDev%x cnt:%d\n", Event->p.remDev, switchrole_cnt);
                        opRemDev = NULL;
                        switchrole_cnt = 0;
                        app_bt_Me_SetLinkPolicy(Event->p.remDev,BLP_SNIFF_MODE);
                        break;
                    case BCR_ANY:
                        break;
                    case BCR_UNKNOWN:
                        break;
                    default:
                        break;
                }
            }

            if (MEC(activeCons) > 1){
                uint8_t slave_cnt = 0;
                for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                    remDev = MeEnumerateRemoteDevices(i);
                    if (ME_GetCurrentRole(remDev) == BCR_SLAVE){
                        slave_cnt++;
                    }
                }
                if (slave_cnt>1){
                    TRACE("ROLE_CHANGE disconnect slave %x\n", Event->p.remDev);
                    app_bt_MeDisconnectLink(Event->p.remDev);
                }
            }
            break;
        default:
           break;
        }
}

//#define MULTIPOINT_DUAL_SLAVE

void app_bt_role_manager_process_dual_slave(const BtEvent *Event)
{
    static BtRemoteDevice *opRemDev = NULL;
    static uint8_t switchrole_cnt = 0;
    //BtRemoteDevice *remDev = NULL;

    //on phone connecting
    switch (Event->eType) {
        case BTEVENT_LINK_CONNECT_IND:
        case BTEVENT_LINK_CONNECT_CNF:
            if(Event->errCode == BEC_NO_ERROR){
                switch (Event->p.remDev->role) {
                    case BCR_MASTER:
                    case BCR_PMASTER:
                        TRACE("CONNECT_IND/CNF try to role %x\n", Event->p.remDev);
                        opRemDev = Event->p.remDev;
                        switchrole_cnt = 0;
                        app_bt_Me_SetLinkPolicy(Event->p.remDev, BLP_MASTER_SLAVE_SWITCH|BLP_SNIFF_MODE);
                        app_bt_ME_SwitchRole(Event->p.remDev);
                        break;
                    case BCR_SLAVE:
                    case BCR_PSLAVE:
                    case BCR_ANY:
                    case BCR_UNKNOWN:
                    default:
                        TRACE("CONNECT_IND disable role %x\n", Event->p.remDev);
                        app_bt_Me_SetLinkPolicy(Event->p.remDev, BLP_SNIFF_MODE);
                        break;
                }
                app_bt_ME_SetConnectionRole(BCR_SLAVE);
            }
            break;
        case BTEVENT_LINK_DISCONNECT:
            if (opRemDev == Event->p.remDev){
                opRemDev = NULL;
                switchrole_cnt = 0;
            }
            if (MEC(activeCons) == 0){
                for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                    if(app_bt_device.a2dp_connected_stream[i])
                        app_bt_A2DP_SetMasterRole(app_bt_device.a2dp_connected_stream[i], FALSE);
                    app_bt_HF_SetMasterRole(&app_bt_device.hf_channel[i], FALSE);
                }
                app_bt_ME_SetConnectionRole(BCR_ANY);
            }else if (MEC(activeCons) == 1){
                app_bt_ME_SetConnectionRole(BCR_SLAVE);
            }
            break;
        case BTEVENT_ROLE_CHANGE:
            if (opRemDev == Event->p.remDev){
                switch (Event->p.roleChange.newRole) {
                    case BCR_MASTER:
                        if (++switchrole_cnt<=APP_BT_SWITCHROLE_LIMIT){
                            TRACE("ROLE_CHANGE try to role again: %d", switchrole_cnt);
                            app_bt_Me_SetLinkPolicy(Event->p.remDev, BLP_MASTER_SLAVE_SWITCH|BLP_SNIFF_MODE);
                            app_bt_ME_SwitchRole(Event->p.remDev);
                        }else{
                            TRACE("ROLE TO SLAVE FAILED remDev%x cnt:%d\n", Event->p.remDev, switchrole_cnt);
                            opRemDev = NULL;
                            switchrole_cnt = 0;
                        }
                        break;
                    case BCR_SLAVE:
                        TRACE("ROLE TO SLAVE SUCCESS remDev%x cnt:%d\n", Event->p.remDev, switchrole_cnt);
                        opRemDev = NULL;
                        switchrole_cnt = 0;
                        //workaround for power reset opening reconnect sometime unsuccessfully in sniff mode, 
                        //only after authentication completes, enable sniff mode.
                        opRemDev = Event->p.remDev;
                        if (opRemDev->authState == BAS_AUTHENTICATED)
                        {
                            app_bt_Me_SetLinkPolicy(opRemDev,BLP_SNIFF_MODE);
                        }
                        else
                        {
                            app_bt_Me_SetLinkPolicy(opRemDev,BLP_DISABLE_ALL);
                        }
                        break;
                    case BCR_ANY:
                        break;
                    case BCR_UNKNOWN:
                        break;
                    default:
                        break;
                }
            }
            break;
    }
}

static int app_bt_sniff_manager_init(void)
{
    BtSniffInfo sniffInfo;
    BtRemoteDevice *remDev = NULL;

    for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
        remDev = MeEnumerateRemoteDevices(i);
        sniffInfo.maxInterval = CMGR_SNIFF_MAX_INTERVAL;
        sniffInfo.minInterval = CMGR_SNIFF_MIN_INTERVAL;
        sniffInfo.attempt = CMGR_SNIFF_ATTEMPT;
        sniffInfo.timeout = CMGR_SNIFF_TIMEOUT;
        app_bt_CMGR_SetSniffInofToAllHandlerByRemDev(&sniffInfo, remDev);
        app_bt_HF_EnableSniffMode(&app_bt_device.hf_channel[i], FALSE);
#if defined (__HSP_ENABLE__)
        app_bt_HS_EnableSniffMode(&app_bt_device.hs_channel[i], FALSE);
#endif
    }

    return 0;
}

void app_bt_sniff_manager_process(const BtEvent *Event)
{
    static BtRemoteDevice *opRemDev = NULL;
    BtRemoteDevice *remDev = NULL;
    CmgrHandler    *currCmgrHandler = NULL;
    CmgrHandler    *otherCmgrHandler = NULL;
    bool need_reconfig = false;
    BtSniffInfo sniffInfo;

    if (!besbt_cfg.sniff)
        return;

    switch (Event->eType) {
        case BTEVENT_LINK_CONNECT_IND:
            if(Event->errCode == BEC_NO_ERROR){
                TRACE("CONNECT_IND sniff info %x\n", Event->p.remDev);
                sniffInfo.maxInterval = CMGR_SNIFF_MAX_INTERVAL;
                sniffInfo.minInterval = CMGR_SNIFF_MIN_INTERVAL;
                sniffInfo.attempt = CMGR_SNIFF_ATTEMPT;
                sniffInfo.timeout = CMGR_SNIFF_TIMEOUT;
                app_bt_CMGR_SetSniffInofToAllHandlerByRemDev(&sniffInfo, Event->p.remDev);
                if (MEC(activeCons) > 1){
                    currCmgrHandler = CMGR_GetConnIndHandler(Event->p.remDev);
                    for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                        remDev = MeEnumerateRemoteDevices(i);
                        if (Event->p.remDev != remDev){
                            otherCmgrHandler = CMGR_GetAclHandler(remDev);
                            if (otherCmgrHandler && currCmgrHandler){
                                if (otherCmgrHandler->sniffInfo.maxInterval == currCmgrHandler->sniffInfo.maxInterval){
                                    sniffInfo.maxInterval = otherCmgrHandler->sniffInfo.maxInterval -20;
                                    sniffInfo.minInterval = otherCmgrHandler->sniffInfo.minInterval - 20;
                                    sniffInfo.attempt = CMGR_SNIFF_ATTEMPT;
                                    sniffInfo.timeout = CMGR_SNIFF_TIMEOUT;
                                    app_bt_CMGR_SetSniffInofToAllHandlerByRemDev(&sniffInfo, Event->p.remDev);
                                    TRACE("CONNECT_IND reinit sniff info\n");
                                }
                            }
                            break;
                        }
                    }
                }
            }
            break;
        case BTEVENT_LINK_CONNECT_CNF:
            if(Event->errCode == BEC_NO_ERROR){
                TRACE("CONNECT_CNF init %x\n", Event->p.remDev);
                sniffInfo.maxInterval = CMGR_SNIFF_MAX_INTERVAL;
                sniffInfo.minInterval = CMGR_SNIFF_MIN_INTERVAL;
                sniffInfo.attempt = CMGR_SNIFF_ATTEMPT;
                sniffInfo.timeout = CMGR_SNIFF_TIMEOUT;
                app_bt_CMGR_SetSniffInofToAllHandlerByRemDev(&sniffInfo, Event->p.remDev);
                if (MEC(activeCons) > 1){
                    currCmgrHandler = CMGR_GetAclHandler(Event->p.remDev);
                    for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                        remDev = MeEnumerateRemoteDevices(i);
                        if (Event->p.remDev != remDev){
                            otherCmgrHandler = CMGR_GetAclHandler(remDev);
                            if (otherCmgrHandler && currCmgrHandler){
                                if (otherCmgrHandler->sniffInfo.maxInterval == currCmgrHandler->sniffInfo.maxInterval){
                                    sniffInfo.maxInterval = otherCmgrHandler->sniffInfo.maxInterval -20;
                                    sniffInfo.minInterval = otherCmgrHandler->sniffInfo.minInterval - 20;
                                    sniffInfo.attempt = CMGR_SNIFF_ATTEMPT;
                                    sniffInfo.timeout = CMGR_SNIFF_TIMEOUT;
                                    app_bt_CMGR_SetSniffInofToAllHandlerByRemDev(&sniffInfo, Event->p.remDev);
                                    TRACE("CONNECT_CNF reinit sniff info\n");
                                }
                            }
                            break;
                        }
                    }
                }
            }
            break;
        case BTEVENT_LINK_DISCONNECT:
             if (opRemDev == Event->p.remDev){
                opRemDev = NULL;
            }
            sniffInfo.maxInterval = CMGR_SNIFF_MAX_INTERVAL;
            sniffInfo.minInterval = CMGR_SNIFF_MIN_INTERVAL;
            sniffInfo.attempt = CMGR_SNIFF_ATTEMPT;
            sniffInfo.timeout = CMGR_SNIFF_TIMEOUT;
            app_bt_CMGR_SetSniffInofToAllHandlerByRemDev(&sniffInfo, Event->p.remDev);
            break;
        case BTEVENT_MODE_CHANGE:
            if(Event->p.modeChange.curMode == BLM_SNIFF_MODE){
                currCmgrHandler = CMGR_GetAclHandler(Event->p.remDev);
                if (Event->p.modeChange.interval > CMGR_SNIFF_MAX_INTERVAL){
                        if (!opRemDev){
                            opRemDev = currCmgrHandler->remDev;
                        }
                        currCmgrHandler->sniffInfo.maxInterval = CMGR_SNIFF_MAX_INTERVAL;
                        currCmgrHandler->sniffInfo.minInterval = CMGR_SNIFF_MIN_INTERVAL;
                        currCmgrHandler->sniffInfo.attempt = CMGR_SNIFF_ATTEMPT;
                        currCmgrHandler->sniffInfo.timeout = CMGR_SNIFF_TIMEOUT;
                        app_bt_CMGR_SetSniffInofToAllHandlerByRemDev(&currCmgrHandler->sniffInfo, Event->p.remDev);
                        app_bt_ME_StopSniff(currCmgrHandler->remDev);
                }else{
                    if (currCmgrHandler){
                        currCmgrHandler->sniffInfo.maxInterval = Event->p.modeChange.interval;
                        currCmgrHandler->sniffInfo.minInterval = Event->p.modeChange.interval;
                        currCmgrHandler->sniffInfo.attempt = CMGR_SNIFF_ATTEMPT;
                        currCmgrHandler->sniffInfo.timeout = CMGR_SNIFF_TIMEOUT;
                        app_bt_CMGR_SetSniffInofToAllHandlerByRemDev(&currCmgrHandler->sniffInfo, Event->p.remDev);
                    }
                    if (MEC(activeCons) > 1){
                        for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                            remDev = MeEnumerateRemoteDevices(i);
                            if (Event->p.remDev != remDev){
                                otherCmgrHandler = CMGR_GetAclHandler(remDev);
                                if (otherCmgrHandler){
                                    if (otherCmgrHandler->sniffInfo.maxInterval == currCmgrHandler->sniffInfo.maxInterval){
                                        if (ME_GetCurrentMode(remDev) == BLM_ACTIVE_MODE){
                                            otherCmgrHandler->sniffInfo.maxInterval -= 20;
                                            otherCmgrHandler->sniffInfo.minInterval -= 20;
                                            otherCmgrHandler->sniffInfo.attempt = CMGR_SNIFF_ATTEMPT;
                                            otherCmgrHandler->sniffInfo.timeout = CMGR_SNIFF_TIMEOUT;
                                            app_bt_CMGR_SetSniffInofToAllHandlerByRemDev(&otherCmgrHandler->sniffInfo, remDev);
                                            TRACE("reconfig sniff other RemDev:%x\n", remDev);
                                        }else if (ME_GetCurrentMode(remDev) == BLM_SNIFF_MODE){
                                            need_reconfig = true;
                                        }
                                    }
                                }
                                break;
                            }
                        }
                    }
                    if (need_reconfig){
                        opRemDev = remDev;
                        if (currCmgrHandler){
                            currCmgrHandler->sniffInfo.maxInterval -= 20;
                            currCmgrHandler->sniffInfo.minInterval -= 20;
                            currCmgrHandler->sniffInfo.attempt = CMGR_SNIFF_ATTEMPT;
                            currCmgrHandler->sniffInfo.timeout = CMGR_SNIFF_TIMEOUT;
                            app_bt_CMGR_SetSniffInofToAllHandlerByRemDev(&currCmgrHandler->sniffInfo, currCmgrHandler->remDev);
                        }
                        app_bt_ME_StopSniff(currCmgrHandler->remDev);
                        TRACE("reconfig sniff setup op opRemDev:%x\n", opRemDev);
                    }
                }
            }
            if (Event->p.modeChange.curMode == BLM_ACTIVE_MODE){
                if (opRemDev == Event->p.remDev){
                    TRACE("reconfig sniff op opRemDev:%x\n", opRemDev);
                    opRemDev = NULL;
                    currCmgrHandler = CMGR_GetAclHandler(Event->p.remDev);
                    if (currCmgrHandler){
                        app_bt_CMGR_SetSniffTimer(currCmgrHandler, NULL, 1000);
                    }
                }
            }
            break;
        case BTEVENT_ACL_DATA_ACTIVE:
            CmgrHandler    *cmgrHandler;
            /* Start the sniff timer */
            cmgrHandler = CMGR_GetAclHandler(Event->p.remDev);
            if (cmgrHandler)
                app_bt_CMGR_SetSniffTimer(cmgrHandler, NULL, CMGR_SNIFF_TIMER);
            break;
        case BTEVENT_SCO_CONNECT_IND:
        case BTEVENT_SCO_CONNECT_CNF:
            app_bt_Me_SetLinkPolicy(Event->p.scoConnect.remDev, BLP_DISABLE_ALL);
            if (MEC(activeCons) > 1){
                for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                    remDev = MeEnumerateRemoteDevices(i);
                    if (Event->p.remDev == remDev){
                        continue;
                    }
                    otherCmgrHandler = CMGR_GetConnIndHandler(remDev);
                    if (otherCmgrHandler){
                        if (CMGR_IsLinkUp(otherCmgrHandler)){
                            if (ME_GetCurrentMode(remDev) == BLM_ACTIVE_MODE){
                                TRACE(" ohter dev disable sniff");
                                app_bt_Me_SetLinkPolicy(remDev, BLP_DISABLE_ALL);
                            }else if (ME_GetCurrentMode(remDev) == BLM_SNIFF_MODE){
                                TRACE(" ohter dev exit & disable sniff");
                                app_bt_ME_StopSniff(remDev);
                                app_bt_Me_SetLinkPolicy(remDev, BLP_DISABLE_ALL);
                            }
                        }
                    }
                }
            }
            break;
        case BTEVENT_SCO_DISCONNECT:
            if (MEC(activeCons) == 1){
                app_bt_Me_SetLinkPolicy(Event->p.scoConnect.remDev, BLP_SNIFF_MODE);
            }else{
                for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                    remDev = MeEnumerateRemoteDevices(i);
                    if (Event->p.remDev == remDev){
                        continue;
                    }
                    otherCmgrHandler = CMGR_GetConnIndHandler(remDev);
                }
                TRACE("SCO_DISCONNECT:%d%/%d %x/%x\n", CMGR_IsAudioUp(currCmgrHandler), CMGR_IsAudioUp(otherCmgrHandler),
                                                 CMGR_GetRemDev(currCmgrHandler), Event->p.remDev);
                if (otherCmgrHandler){
                    if (!CMGR_IsAudioUp(otherCmgrHandler)){
                        TRACE("enable sniff to all\n");
                        app_bt_Me_SetLinkPolicy(Event->p.scoConnect.remDev, BLP_SNIFF_MODE);
                        app_bt_Me_SetLinkPolicy(CMGR_GetRemDev(otherCmgrHandler), BLP_SNIFF_MODE);
                    }
                }else{
                    app_bt_Me_SetLinkPolicy(Event->p.scoConnect.remDev, BLP_SNIFF_MODE);
                }
            }
            break;
        default:
            break;
    }
}

APP_BT_GOLBAL_HANDLE_HOOK_HANDLER app_bt_golbal_handle_hook_handler[APP_BT_GOLBAL_HANDLE_HOOK_USER_QTY] = {0};
void app_bt_golbal_handle_hook(const BtEvent *Event)
{
    uint8_t i;
    for (i=0; i<APP_BT_GOLBAL_HANDLE_HOOK_USER_QTY; i++){
        if (app_bt_golbal_handle_hook_handler[i])
            app_bt_golbal_handle_hook_handler[i](Event);
    }
}

int app_bt_golbal_handle_hook_set(enum APP_BT_GOLBAL_HANDLE_HOOK_USER_T user, APP_BT_GOLBAL_HANDLE_HOOK_HANDLER handler)
{
    app_bt_golbal_handle_hook_handler[user] = handler;
    return 0;
}

APP_BT_GOLBAL_HANDLE_HOOK_HANDLER app_bt_golbal_handle_hook_get(enum APP_BT_GOLBAL_HANDLE_HOOK_USER_T user)
{
    return app_bt_golbal_handle_hook_handler[user];
}

/////There is a device connected, so stop PAIR_TIMER and POWEROFF_TIMER of earphone.
BtHandler  app_bt_handler;
static void app_bt_golbal_handle(const BtEvent *Event)
{
    switch (Event->eType) {
        case BTEVENT_HCI_COMMAND_SENT:
		case BTEVENT_ACL_DATA_NOT_ACTIVE:
            return;
        case BTEVENT_ACL_DATA_ACTIVE:
            CmgrHandler    *cmgrHandler;
            /* Start the sniff timer */
            cmgrHandler = CMGR_GetAclHandler(Event->p.remDev);
            if (cmgrHandler)
                app_bt_CMGR_SetSniffTimer(cmgrHandler, NULL, CMGR_SNIFF_TIMER);
            return;
		case BTEVENT_AUTHENTICATED:
            TRACE("GLOBAL HANDER AUTH error=%x", Event->errCode);
            //after authentication completes, re-enable sniff mode.
            if(Event->errCode == BEC_NO_ERROR)
            {
                app_bt_Me_SetLinkPolicy(Event->p.remDev,BLP_SNIFF_MODE);
            }
            break;	
    }

    TRACE("app_bt_golbal_handle evt = %d",Event->eType);

    switch (Event->eType) {
        case BTEVENT_LINK_CONNECT_IND:
        case BTEVENT_LINK_CONNECT_CNF:
			app_dip_get_remote_info(Event->p.remDev);
            TRACE("CONNECT_IND/CNF evt:%d errCode:0x%0x newRole:%d activeCons:%d",Event->eType, Event->errCode, Event->p.remDev->role, MEC(activeCons));
#if defined(__EARPHONE__) && defined(__AUTOPOWEROFF__)  && !defined(FPGA)
            if (MEC(activeCons) == 0){
                app_start_10_second_timer(APP_POWEROFF_TIMER_ID);
            }else{
                app_stop_10_second_timer(APP_POWEROFF_TIMER_ID);
            }
#endif
#ifdef __BT_ONE_BRING_TWO__
            if (MEC(activeCons) > 2){
                app_bt_MeDisconnectLink(Event->p.remDev);
            }
#else
            if (MEC(activeCons) > 1){
                app_bt_MeDisconnectLink(Event->p.remDev);
            }
#endif
            break;
        case BTEVENT_LINK_DISCONNECT:
            TRACE("DISCONNECT evt = %d encryptState:%d",Event->eType, Event->p.remDev->encryptState);
#if defined(__EARPHONE__) && defined(__AUTOPOWEROFF__) && !defined(FPGA)
            if (MEC(activeCons) == 0){
                app_start_10_second_timer(APP_POWEROFF_TIMER_ID);
            }
#endif
            break;
        case BTEVENT_ROLE_CHANGE:
            TRACE("ROLE_CHANGE eType:0x%x errCode:0x%x newRole:%d activeCons:%d", Event->eType, Event->errCode, Event->p.roleChange.newRole, MEC(activeCons));
            break;
        case BTEVENT_MODE_CHANGE:
            TRACE("MODE_CHANGE evt:%d errCode:0x%0x curMode=0x%0x, interval=%d ",Event->eType, Event->errCode,Event->p.modeChange.curMode,Event->p.modeChange.interval);
            break;
        case BTEVENT_ACCESSIBLE_CHANGE:
            TRACE("ACCESSIBLE_CHANGE evt:%d errCode:0x%0x aMode=0x%0x",Event->eType, Event->errCode,Event->p.aMode);
            break;
        case BTEVENT_LINK_POLICY_CHANGED:
        {
            BT_SET_LINKPOLICY_REQ_T* pReq = app_bt_pop_pending_set_linkpolicy();
            if (NULL != pReq)
            {
                app_bt_set_linkpolicy(pReq->remDev, pReq->policy);
            }
            break;
        }
        default:
            break;
    }

#ifdef MULTIPOINT_DUAL_SLAVE
    app_bt_role_manager_process_dual_slave(Event);
#else
    app_bt_role_manager_process(Event);
#endif
    app_bt_accessible_manager_process(Event);
    app_bt_sniff_manager_process(Event);
    app_bt_golbal_handle_hook(Event);
}

void app_bt_golbal_handle_init(void)
{
    BtEventMask mask = BEM_NO_EVENTS;
    ME_InitHandler(&app_bt_handler);
    app_bt_handler.callback = app_bt_golbal_handle;
    ME_RegisterGlobalHandler(&app_bt_handler);
    mask |= BEM_ROLE_CHANGE | BEM_SCO_CONNECT_CNF | BEM_SCO_DISCONNECT | BEM_SCO_CONNECT_IND;
	mask |= BEM_AUTHENTICATED;
    mask |= BEM_LINK_CONNECT_IND;
    mask |= BEM_LINK_DISCONNECT;
    mask |= BEM_LINK_CONNECT_CNF;
    mask |= BEM_ACCESSIBLE_CHANGE;
#ifdef __BT_ONE_BRING_TWO__
    mask |= BEM_MODE_CHANGE;
#endif
    app_bt_ME_SetConnectionRole(BCR_ANY);
    for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
        app_bt_A2DP_SetMasterRole(&app_bt_device.a2dp_stream[i], FALSE);
#if defined(A2DP_AAC_ON)
        app_bt_A2DP_SetMasterRole(&app_bt_device.a2dp_aac_stream[i], FALSE);
#endif
#if defined(A2DP_SCALABLE_ON)
        app_bt_A2DP_SetMasterRole(&app_bt_device.a2dp_scalable_stream[i], FALSE);
#endif
        app_bt_HF_SetMasterRole(&app_bt_device.hf_channel[i], FALSE);
#if defined (__HSP_ENABLE__)
        HS_SetMasterRole(&app_bt_device.hs_channel[i], FALSE);
#endif
    }
    ME_SetEventMask(&app_bt_handler, mask);
    app_bt_sniff_manager_init();
    app_bt_accessmode_timer = osTimerCreate (osTimer(APP_BT_ACCESSMODE_TIMER), osTimerOnce, &app_bt_accessmode_timer_argument);
#ifdef __AMA_VOICE__
    //-add app ama create spp client timer    
    app_ama_create_spp_client_devId = 0;
	app_ama_create_spp_client_timerId = osTimerCreate(osTimer(AMA_CREATE_SPP_CLIENT_TIMEOUT), osTimerOnce, NULL);
#endif
//add begin by cai.zhong 20190103 for rfcomm
#ifdef __DMA_VOICE__
	app_dma_create_spp_client_timerId = osTimerCreate(osTimer(DMA_CREATE_SPP_CLIENT_TIMEOUT), osTimerOnce, NULL);
#endif
//add end by cai.zhong 20190103 for rfcomm
}

void app_bt_send_request(uint32_t message_id, uint32_t param0, uint32_t param1, uint32_t ptr)
{
    APP_MESSAGE_BLOCK msg;

    TRACE("app_bt_send_request: %d\n", message_id);
    msg.mod_id = APP_MODUAL_BT;
    msg.msg_body.message_id = message_id;
    msg.msg_body.message_Param0 = param0;
    msg.msg_body.message_Param1 = param1;
    msg.msg_body.message_ptr = ptr;
    app_mailbox_put(&msg);
}


static int app_bt_handle_process(APP_MESSAGE_BODY *msg_body)
{
    BtAccessibleMode old_access_mode;
    TRACE("app_bt_handle_process: %d\n", msg_body->message_id);

    switch (msg_body->message_id) {
        case APP_BT_REQ_ACCESS_MODE_SET:
            old_access_mode = g_bt_access_mode;
            app_bt_accessmode_set(msg_body->message_Param0);
            if (msg_body->message_Param0 == BAM_GENERAL_ACCESSIBLE &&
                old_access_mode != BAM_GENERAL_ACCESSIBLE){
#if FPGA == 0
                app_status_indication_set(APP_STATUS_INDICATION_BOTHSCAN);
                app_voice_report(APP_STATUS_INDICATION_BOTHSCAN, 0);
                app_start_10_second_timer(APP_PAIR_TIMER_ID);
#endif
            }else{
                app_status_indication_set(APP_STATUS_INDICATION_PAGESCAN);
            }
            break;
        default:
            break;
    }

    return 0;
}

void *app_bt_profile_active_store_ptr_get(uint8_t *bdAddr)
{
    static btdevice_profile device_profile = {false, false, false,0};
    btdevice_profile *ptr;
    nvrec_btdevicerecord *record = NULL;

#ifndef FPGA
    if (!nv_record_btdevicerecord_find((BT_BD_ADDR *)bdAddr,&record)){
        ptr = &(record->device_plf);
        DUMP8("0x%02x ", bdAddr, BD_ADDR_SIZE);
        TRACE("%s hfp_act:%d hsp_act:%d a2dp_act:0x%x codec_type=%x", __func__, ptr->hfp_act, ptr->hsp_act, ptr->a2dp_act,ptr->a2dp_codectype);
    }else
#endif
    {
        ptr = &device_profile;
        TRACE("%s default", __func__);
    }
    return (void *)ptr;
}

enum bt_profile_reconnect_mode{
    bt_profile_reconnect_null,
    bt_profile_reconnect_openreconnecting,
    bt_profile_reconnect_reconnecting,
};

enum bt_profile_connect_status{
    bt_profile_connect_status_unknow,
    bt_profile_connect_status_success,
    bt_profile_connect_status_failure,
};

struct app_bt_profile_manager{
    bool has_connected;
    enum bt_profile_connect_status hfp_connect;
    enum bt_profile_connect_status hsp_connect;
    enum bt_profile_connect_status a2dp_connect;
    BT_BD_ADDR rmt_addr;
    bt_profile_reconnect_mode reconnect_mode;
    A2dpStream *stream;
    HfChannel *chan;
#if defined (__HSP_ENABLE__)
    HsChannel * hs_chan;
#endif
    uint16_t reconnect_cnt;
    osTimerId connect_timer;
    void (* connect_timer_cb)(void const *);
};

//reconnect = (INTERVAL+PAGETO)*CNT = (3000ms+5000ms)*15 = 120s
#define APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS (3000)
#define APP_BT_PROFILE_RECONNECT_RETRY_LIMIT_CNT (15)
#define APP_BT_PROFILE_CONNECT_RETRY_MS (10000)

static struct app_bt_profile_manager bt_profile_manager[BT_DEVICE_NUM];
static void app_bt_profile_reconnect_timehandler(void const *param);

osTimerDef (BT_PROFILE_CONNECT_TIMER0, app_bt_profile_reconnect_timehandler);                      // define timers
#ifdef __BT_ONE_BRING_TWO__
osTimerDef (BT_PROFILE_CONNECT_TIMER1, app_bt_profile_reconnect_timehandler);
#endif

#ifdef __AUTO_CONNECT_OTHER_PROFILE__
static void app_bt_profile_connect_hf_retry_timehandler(void const *param)
{
    struct app_bt_profile_manager *bt_profile_manager_p = (struct app_bt_profile_manager *)param;
    app_bt_HF_CreateServiceLink(bt_profile_manager_p->chan, &bt_profile_manager_p->rmt_addr);
}

#if defined (__HSP_ENABLE__)
static void app_bt_profile_connect_hs_retry_timehandler(void const *param)
{
    struct app_bt_profile_manager *bt_profile_manager_p = (struct app_bt_profile_manager *)param;
    app_bt_HS_CreateServiceLink(bt_profile_manager_p->hs_chan, &bt_profile_manager_p->rmt_addr);
}
#endif

static void app_bt_profile_connect_a2dp_retry_timehandler(void const *param)
{
    struct app_bt_profile_manager *bt_profile_manager_p = (struct app_bt_profile_manager *)param;
    app_bt_A2DP_OpenStream(bt_profile_manager_p->stream, &bt_profile_manager_p->rmt_addr);
}
#endif

static void app_bt_profile_reconnect_timehandler(void const *param)
{
    struct app_bt_profile_manager *bt_profile_manager_p = (struct app_bt_profile_manager *)param;
    btdevice_profile *btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get(bt_profile_manager_p->rmt_addr.addr);
    if (bt_profile_manager_p->connect_timer_cb){
        bt_profile_manager_p->connect_timer_cb(param);
        bt_profile_manager_p->connect_timer_cb = NULL;
    }else{
        if (btdevice_plf_p->hfp_act){
            TRACE("try connect hf");
            app_bt_HF_CreateServiceLink(bt_profile_manager_p->chan, &bt_profile_manager_p->rmt_addr);
        }
#if defined (__HSP_ENABLE__)
        else if(btdevice_plf_p->hsp_act){
            TRACE("try connect hs");
            app_bt_HS_CreateServiceLink(bt_profile_manager_p->hs_chan, &bt_profile_manager_p->rmt_addr);
        }
#endif
        else if(btdevice_plf_p->a2dp_act){
            TRACE("try connect a2dp");
            app_bt_A2DP_OpenStream(bt_profile_manager_p->stream, &bt_profile_manager_p->rmt_addr);
        }
    }
}

void app_bt_profile_connect_manager_open(void)
{
    uint8_t i=0;
    for (i=0;i<BT_DEVICE_NUM;i++){
        bt_profile_manager[i].has_connected = false;
        bt_profile_manager[i].hfp_connect = bt_profile_connect_status_unknow;
        bt_profile_manager[i].hsp_connect = bt_profile_connect_status_unknow;
        bt_profile_manager[i].a2dp_connect = bt_profile_connect_status_unknow;
        memset(bt_profile_manager[i].rmt_addr.addr,0 ,0);
        bt_profile_manager[i].reconnect_mode = bt_profile_reconnect_null;
        bt_profile_manager[i].stream = NULL;
        bt_profile_manager[i].chan = NULL;
#if defined (__HSP_ENABLE__)
        bt_profile_manager[i].hs_chan = NULL;
#endif
        bt_profile_manager[i].reconnect_cnt = 0;
        bt_profile_manager[i].connect_timer_cb = NULL;
    }

    bt_profile_manager[BT_DEVICE_ID_1].connect_timer = osTimerCreate (osTimer(BT_PROFILE_CONNECT_TIMER0), osTimerOnce, &bt_profile_manager[BT_DEVICE_ID_1]);
#ifdef __BT_ONE_BRING_TWO__
    bt_profile_manager[BT_DEVICE_ID_2].connect_timer = osTimerCreate (osTimer(BT_PROFILE_CONNECT_TIMER1), osTimerOnce, &bt_profile_manager[BT_DEVICE_ID_2]);
#endif
}

BOOL app_bt_profile_connect_openreconnecting(void *ptr)
{
    bool nRet = false;
    uint8_t i;

    for (i=0;i<BT_DEVICE_NUM;i++){
        nRet |= bt_profile_manager[i].reconnect_mode == bt_profile_reconnect_openreconnecting ? true : false;
    }

    return nRet;
}

void app_bt_profile_connect_manager_opening_reconnect(void)
{
    int ret;
    BtDeviceRecord record1;
    BtDeviceRecord record2;
    btdevice_profile *btdevice_plf_p;
    int find_invalid_record_cnt;
#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
	if(app_bt_device.src_or_snk==BT_DEVICE_SRC)
	{
		return ;
	}
#endif
    OS_LockStack();
    do{
        find_invalid_record_cnt = 0;
        ret = nv_record_enum_latest_two_paired_dev(&record1,&record2);
        if(ret == 1){
            btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get(record1.bdAddr.addr);
            if (!(btdevice_plf_p->hfp_act)&&!(btdevice_plf_p->a2dp_act)){
                nv_record_ddbrec_delete(&record1.bdAddr);
                find_invalid_record_cnt++;
            }
        }else if(ret == 2){
            btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get(record1.bdAddr.addr);
            if (!(btdevice_plf_p->hfp_act)&&!(btdevice_plf_p->a2dp_act)){
                nv_record_ddbrec_delete(&record1.bdAddr);
                find_invalid_record_cnt++;
            }
            btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get(record2.bdAddr.addr);
            if (!(btdevice_plf_p->hfp_act)&&!(btdevice_plf_p->a2dp_act)){
                nv_record_ddbrec_delete(&record2.bdAddr);
                find_invalid_record_cnt++;
            }
        }
    }while(find_invalid_record_cnt);

    TRACE("!!!app_bt_opening_reconnect:\n");
    DUMP8("%02x ", &record1.bdAddr, 6);
    DUMP8("%02x ", &record2.bdAddr, 6);


    if(ret > 0){
        TRACE("!!!start reconnect first device\n");

        if(MEC(pendCons) == 0){
            bt_profile_manager[BT_DEVICE_ID_1].reconnect_mode = bt_profile_reconnect_openreconnecting;
            memcpy(bt_profile_manager[BT_DEVICE_ID_1].rmt_addr.addr, record1.bdAddr.addr, BD_ADDR_SIZE);
            btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get(bt_profile_manager[BT_DEVICE_ID_1].rmt_addr.addr);
#if defined(A2DP_AAC_ON)
            if(btdevice_plf_p->a2dp_codectype == AVDTP_CODEC_TYPE_MPEG2_4_AAC)
                bt_profile_manager[BT_DEVICE_ID_1].stream = &app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_1];
            else
#endif
#if defined(A2DP_SCALABLE_ON)
            if(btdevice_plf_p->a2dp_codectype == AVDTP_CODEC_TYPE_NON_A2DP)
                bt_profile_manager[BT_DEVICE_ID_1].stream = &app_bt_device.a2dp_scalable_stream[BT_DEVICE_ID_1];
            else
#endif
            {
                bt_profile_manager[BT_DEVICE_ID_1].stream = &app_bt_device.a2dp_stream[BT_DEVICE_ID_1];
            }

            bt_profile_manager[BT_DEVICE_ID_1].chan = &app_bt_device.hf_channel[BT_DEVICE_ID_1];
#if defined (__HSP_ENABLE__)
            bt_profile_manager[BT_DEVICE_ID_1].hs_chan = &app_bt_device.hs_channel[BT_DEVICE_ID_1];
#endif
            if (btdevice_plf_p->hfp_act){
                TRACE("try connect hf");
                app_bt_HF_CreateServiceLink(bt_profile_manager[BT_DEVICE_ID_1].chan, &bt_profile_manager[BT_DEVICE_ID_1].rmt_addr);
            }
#if defined (__HSP_ENABLE__)
			else if (btdevice_plf_p->hsp_act){
                TRACE("try connect hs");
                app_bt_HS_CreateServiceLink(bt_profile_manager[BT_DEVICE_ID_1].hs_chan, &bt_profile_manager[BT_DEVICE_ID_1].rmt_addr);
            }
#endif
			else if(btdevice_plf_p->a2dp_act) {
                TRACE("try connect a2dp");
                app_bt_A2DP_OpenStream(bt_profile_manager[BT_DEVICE_ID_1].stream, &bt_profile_manager[BT_DEVICE_ID_1].rmt_addr);
            }
        }
#ifdef __BT_ONE_BRING_TWO__
        if(ret > 1){
            TRACE("!!!need reconnect second device\n");
            bt_profile_manager[BT_DEVICE_ID_2].reconnect_mode = bt_profile_reconnect_openreconnecting;
            memcpy(bt_profile_manager[BT_DEVICE_ID_2].rmt_addr.addr, record2.bdAddr.addr, BD_ADDR_SIZE);
            btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get(bt_profile_manager[BT_DEVICE_ID_2].rmt_addr.addr);
#if defined(A2DP_AAC_ON)
            if(btdevice_plf_p->a2dp_codectype == AVDTP_CODEC_TYPE_MPEG2_4_AAC)
                bt_profile_manager[BT_DEVICE_ID_2].stream = &app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_2];
            else
#endif
#if defined(A2DP_SCALABLE_ON)
            if(btdevice_plf_p->a2dp_codectype == AVDTP_CODEC_TYPE_NON_A2DP)
                bt_profile_manager[BT_DEVICE_ID_2].stream = &app_bt_device.a2dp_scalable_stream[BT_DEVICE_ID_2];
            else
#endif
            bt_profile_manager[BT_DEVICE_ID_2].stream = &app_bt_device.a2dp_stream[BT_DEVICE_ID_2];
            bt_profile_manager[BT_DEVICE_ID_2].chan = &app_bt_device.hf_channel[BT_DEVICE_ID_2];
#if defined (__HSP_ENABLE__)
            bt_profile_manager[BT_DEVICE_ID_2].hs_chan = &app_bt_device.hs_channel[BT_DEVICE_ID_2];
#endif
        }
#endif
    }

    else
    {
        TRACE("!!!go to pairing\n");
#ifdef __EARPHONE_STAY_BOTH_SCAN__
        app_bt_accessmode_set_req(BT_DEFAULT_ACCESS_MODE_PAIR);
#else
        app_bt_accessmode_set_req(BAM_CONNECTABLE_ONLY);
#endif

    }
    OS_UnlockStack();
}

#if defined (__HSP_ENABLE__)
void app_bt_profile_connect_manager_hs(enum BT_DEVICE_ID_T id, HsChannel *Chan, HsCallbackParms *Info)
{
    btdevice_profile *btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get((uint8_t *)Info->p.remDev->bdAddr.addr);

    osTimerStop(bt_profile_manager[id].connect_timer);
    bt_profile_manager[id].connect_timer_cb = NULL;
    if (Chan&&Info){
        switch(Info->event)
        {
            case HF_EVENT_SERVICE_CONNECTED:
                TRACE("%s HS_EVENT_SERVICE_CONNECTED",__func__);
                btdevice_plf_p->hsp_act = true;
#ifndef FPGA
                nv_record_touch_cause_flush();
#endif
                bt_profile_manager[id].hsp_connect = bt_profile_connect_status_success;
                bt_profile_manager[id].reconnect_cnt = 0;
                bt_profile_manager[id].hs_chan = &app_bt_device.hs_channel[id];
                memcpy(bt_profile_manager[id].rmt_addr.addr, Info->p.remDev->bdAddr.addr, BD_ADDR_SIZE);
                if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_openreconnecting){
                    //do nothing
                }else if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_reconnecting){
                    if (btdevice_plf_p->a2dp_act && bt_profile_manager[id].a2dp_connect != bt_profile_connect_status_success){
                        TRACE("!!!continue connect a2dp\n");
                        app_bt_A2DP_OpenStream(bt_profile_manager[id].stream, &bt_profile_manager[id].rmt_addr);
                    }
                }
#ifdef __AUTO_CONNECT_OTHER_PROFILE__
                else{
                    bt_profile_manager[id].connect_timer_cb = app_bt_profile_connect_a2dp_retry_timehandler;
                    osTimerStart(bt_profile_manager[id].connect_timer, APP_BT_PROFILE_CONNECT_RETRY_MS);
                }
#endif
                break;
            case HF_EVENT_SERVICE_DISCONNECTED:
                TRACE("%s HS_EVENT_SERVICE_DISCONNECTED discReason:%d",__func__, Info->p.remDev->discReason);
                bt_profile_manager[id].hsp_connect = bt_profile_connect_status_failure;
                if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_openreconnecting){
                    //do nothing
                }else if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_reconnecting){
                    if (++bt_profile_manager[id].reconnect_cnt < APP_BT_PROFILE_RECONNECT_RETRY_LIMIT_CNT){
                        osTimerStart(bt_profile_manager[id].connect_timer, APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS);
                    }else{
                        bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_null;
                    }
                    TRACE("%s try to reconnect cnt:%d",__func__, bt_profile_manager[id].reconnect_cnt);
                }else if(Info->p.remDev->discReason == 0x8){
                    bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_reconnecting;
                    TRACE("%s try to reconnect",__func__);
                    osTimerStart(bt_profile_manager[id].connect_timer, APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS);
                }else{
                    bt_profile_manager[id].hsp_connect = bt_profile_connect_status_unknow;
                }
                break;
            default:
                break;
        }
    }

    if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_reconnecting){
        bool reconnect_hsp_proc_final = true;
        bool reconnect_a2dp_proc_final = true;
        if (bt_profile_manager[id].hsp_connect == bt_profile_connect_status_failure){
            reconnect_hsp_proc_final = false;
        }
        if (bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_failure){
            reconnect_a2dp_proc_final = false;
        }
        if (reconnect_hsp_proc_final && reconnect_a2dp_proc_final){
            TRACE("!!!reconnect success %d/%d/%d\n", bt_profile_manager[id].hfp_connect, bt_profile_manager[id].hsp_connect, bt_profile_manager[id].a2dp_connect);
            bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_null;
        }
    }else if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_openreconnecting){
        bool opening_hsp_proc_final = false;
        bool opening_a2dp_proc_final = false;

        if (btdevice_plf_p->hsp_act && bt_profile_manager[id].hsp_connect == bt_profile_connect_status_unknow){
            opening_hsp_proc_final = false;
        }else{
            opening_hsp_proc_final = true;
        }

        if (btdevice_plf_p->a2dp_act && bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_unknow){
            opening_a2dp_proc_final = false;
        }else{
            opening_a2dp_proc_final = true;
        }

        if ((opening_hsp_proc_final && opening_a2dp_proc_final) ||
            (bt_profile_manager[id].hsp_connect == bt_profile_connect_status_failure)){
            TRACE("!!!reconnect success %d/%d/%d\n", bt_profile_manager[id].hfp_connect, bt_profile_manager[id].hsp_connect, bt_profile_manager[id].a2dp_connect);
            bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_null;
        }

        if (btdevice_plf_p->hsp_act && bt_profile_manager[id].hsp_connect == bt_profile_connect_status_success){
            if (btdevice_plf_p->a2dp_act && !opening_a2dp_proc_final){
                TRACE("!!!continue connect a2dp\n");
                app_bt_A2DP_OpenStream(bt_profile_manager[id].stream, &bt_profile_manager[id].rmt_addr);
            }
        }

        if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_null){
            for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                if (bt_profile_manager[i].reconnect_mode == bt_profile_reconnect_openreconnecting){
                    TRACE("!!!hs->start reconnect second device\n");
                    if (btdevice_plf_p->hfp_act){
                        TRACE("try connect hf");
                        app_bt_HF_CreateServiceLink(bt_profile_manager[i].chan, &bt_profile_manager[i].rmt_addr);
                    }
		else if (btdevice_plf_p->hsp_act){
		     TRACE("try connect hs");
                        app_bt_HS_CreateServiceLink(bt_profile_manager[i].hs_chan, &bt_profile_manager[i].rmt_addr);

		} else if(btdevice_plf_p->a2dp_act) {
                        TRACE("try connect a2dp");
                        app_bt_A2DP_OpenStream(bt_profile_manager[i].stream, &bt_profile_manager[i].rmt_addr);
                    }
                    break;
                }
            }
        }
    }

    if (!bt_profile_manager[id].has_connected &&
        (bt_profile_manager[id].hfp_connect == bt_profile_connect_status_success ||
         bt_profile_manager[id].hsp_connect == bt_profile_connect_status_success||
         bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_success)){

        bt_profile_manager[id].has_connected = true;
#ifdef MEDIA_PLAYER_SUPPORT
        app_voice_report(APP_STATUS_INDICATION_CONNECTED, id);
#endif
    }

    if (bt_profile_manager[id].has_connected &&
        (bt_profile_manager[id].hfp_connect != bt_profile_connect_status_success &&
         bt_profile_manager[id].hsp_connect != bt_profile_connect_status_success &&
         bt_profile_manager[id].a2dp_connect != bt_profile_connect_status_success)){

        bt_profile_manager[id].has_connected = false;
#ifdef MEDIA_PLAYER_SUPPORT
        app_voice_report(APP_STATUS_INDICATION_DISCONNECTED, id);
#endif
    }
}
#endif

bool app_is_ios(HfChannel *Chan)
{
	BtRemoteDevice* remDev = CMGR_GetRemoteDevice(&Chan->cmgrHandler);
	return app_dip_is_support_msbc(remDev);
}


void app_bt_profile_connect_manager_hf(enum BT_DEVICE_ID_T id, HfChannel *Chan, HfCallbackParms *Info)
{
    btdevice_profile *btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get((uint8_t *)Info->p.remDev->bdAddr.addr);

    osTimerStop(bt_profile_manager[id].connect_timer);
    bt_profile_manager[id].connect_timer_cb = NULL;
    if (Chan&&Info){
        switch(Info->event)
        {
            case HF_EVENT_SERVICE_CONNECTED:
                TRACE("%s HF_EVENT_SERVICE_CONNECTED",__func__);
                btdevice_plf_p->hfp_act = true;
#ifndef FPGA
                nv_record_touch_cause_flush();
#endif
                bt_profile_manager[id].hfp_connect = bt_profile_connect_status_success;
                bt_profile_manager[id].reconnect_cnt = 0;
                bt_profile_manager[id].chan = &app_bt_device.hf_channel[id];
                memcpy(bt_profile_manager[id].rmt_addr.addr, Info->p.remDev->bdAddr.addr, BD_ADDR_SIZE);
                if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_openreconnecting){
                    //do nothing
                }else if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_reconnecting){
                    if (btdevice_plf_p->a2dp_act && bt_profile_manager[id].a2dp_connect != bt_profile_connect_status_success){	
                        TRACE("!!!continue connect a2dp\n");
                         app_bt_A2DP_OpenStream(bt_profile_manager[id].stream, &bt_profile_manager[id].rmt_addr);
                    }
                }
#ifdef __AUTO_CONNECT_OTHER_PROFILE__
                else{
                    bt_profile_manager[id].connect_timer_cb = app_bt_profile_connect_a2dp_retry_timehandler;
                    osTimerStart(bt_profile_manager[id].connect_timer, APP_BT_PROFILE_CONNECT_RETRY_MS);
                }
#endif
                break;
            case HF_EVENT_SERVICE_DISCONNECTED:
                TRACE("%s HF_EVENT_SERVICE_DISCONNECTED discReason:%d/%d",__func__, Info->p.remDev->discReason, Info->p.remDev->discReason_saved);
                TRACE("%s reconnect_mode %d",__func__, bt_profile_manager[id].reconnect_mode);
                bt_profile_manager[id].hfp_connect = bt_profile_connect_status_failure;
                if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_openreconnecting){
                    bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_openreconnecting;
                    TRACE("%s try to reconnect",__func__);
                    osTimerStart(bt_profile_manager[id].connect_timer, APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS);
                    //do nothing
                }else if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_reconnecting){
                    if (++bt_profile_manager[id].reconnect_cnt < APP_BT_PROFILE_RECONNECT_RETRY_LIMIT_CNT){
                        osTimerStart(bt_profile_manager[id].connect_timer, APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS);
                    }else{
                        bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_null;
                    }
                    TRACE("%s try to reconnect cnt:%d",__func__, bt_profile_manager[id].reconnect_cnt);
                }else if ((Info->p.remDev->discReason == 0x8)||
                          (Info->p.remDev->discReason_saved == 0x8)){
                    bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_reconnecting;
                    TRACE("%s try to reconnect",__func__);
					if(ble_force_stop==true){
						ble_force_stop=false;
#if IAG_BLE_INCLUDE == XA_ENABLED
						bes_ble_stop_advertising();
#endif
					}
                    osTimerStart(bt_profile_manager[id].connect_timer, APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS);
                }else{
                    bt_profile_manager[id].hfp_connect = bt_profile_connect_status_unknow;
					if(ble_force_stop==false){
#if IAG_BLE_INCLUDE == XA_ENABLED
						appm_restart_advertising();//reconnect timeout restart ble adv
#endif
						ble_force_stop=true;
					}
                }
                break;
            default:
                break;
        }
    }

    if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_reconnecting){
        bool reconnect_hfp_proc_final = true;
        bool reconnect_a2dp_proc_final = true;
        if (bt_profile_manager[id].hfp_connect == bt_profile_connect_status_failure){
            reconnect_hfp_proc_final = false;
        }
        if (bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_failure){
            reconnect_a2dp_proc_final = false;
        }
        if (reconnect_hfp_proc_final && reconnect_a2dp_proc_final){
            TRACE("!!!reconnect success %d/%d/%d\n", bt_profile_manager[id].hfp_connect, bt_profile_manager[id].hsp_connect, bt_profile_manager[id].a2dp_connect);
            bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_null;
        }
    }else if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_openreconnecting){
        bool opening_hfp_proc_final = false;
        bool opening_a2dp_proc_final = false;

        if (btdevice_plf_p->hfp_act && bt_profile_manager[id].hfp_connect == bt_profile_connect_status_unknow){
            opening_hfp_proc_final = false;
        }else{
            opening_hfp_proc_final = true;
        }

        if (btdevice_plf_p->a2dp_act && bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_unknow){
            opening_a2dp_proc_final = false;
        }else{
            opening_a2dp_proc_final = true;
        }

        if ((opening_hfp_proc_final && opening_a2dp_proc_final) ||
            (bt_profile_manager[id].hfp_connect == bt_profile_connect_status_failure)){
            TRACE("!!!reconnect success %d/%d/%d\n", bt_profile_manager[id].hfp_connect, bt_profile_manager[id].hsp_connect, bt_profile_manager[id].a2dp_connect);
            bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_null;
        }

        if (btdevice_plf_p->hfp_act && bt_profile_manager[id].hfp_connect == bt_profile_connect_status_success){
            if (btdevice_plf_p->a2dp_act && !opening_a2dp_proc_final){
                TRACE("!!!continue connect a2dp\n");
                 app_bt_A2DP_OpenStream(bt_profile_manager[id].stream, &bt_profile_manager[id].rmt_addr);
            }
        }

        if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_null){
            for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                if (bt_profile_manager[i].reconnect_mode == bt_profile_reconnect_openreconnecting){
                    TRACE("!!!hf->start reconnect second device\n");
                    if (btdevice_plf_p->hfp_act){
                        TRACE("try connect hf");
                        app_bt_HF_CreateServiceLink(bt_profile_manager[i].chan, &bt_profile_manager[i].rmt_addr);
                    }
#if defined (__HSP_ENABLE__)
					else if(btdevice_plf_p->hfp_act) {
                        TRACE("try connect hs");
                        app_bt_HS_CreateServiceLink(bt_profile_manager[i].hs_chan, &bt_profile_manager[i].rmt_addr);
					}
#endif
					else if(btdevice_plf_p->a2dp_act) {
                        TRACE("try connect a2dp");
                        app_bt_A2DP_OpenStream(bt_profile_manager[i].stream, &bt_profile_manager[i].rmt_addr);
                    }
                    break;
                }
            }
        }
    }

    if (!bt_profile_manager[id].has_connected &&
        (bt_profile_manager[id].hfp_connect == bt_profile_connect_status_success ||
         bt_profile_manager[id].hsp_connect == bt_profile_connect_status_success||
         bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_success)){

        bt_profile_manager[id].has_connected = true;
		if(ble_force_stop==false){
#if IAG_BLE_INCLUDE == XA_ENABLED
			appm_restart_advertising();
#endif
			ble_force_stop=true;
		}
#ifdef MEDIA_PLAYER_SUPPORT
        app_voice_report(APP_STATUS_INDICATION_CONNECTED, id);
#endif
#ifdef __THIRDPARTY
		//app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO2,THIRDPARTY_BT_CONNECTED,0);
#endif

    }
	if((bt_profile_manager[id].hfp_connect == bt_profile_connect_status_success)
		&&(bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_success)){
#if defined(__AMA_VOICE__) || defined(__DMA_VOICE__) //change begin by cai.zhong 20190103 for rfcomm
		//if app is ios then start	app_ama_create_spp_client_timer
		if(app_is_ios(Chan) == true){
			TRACE("The connected is: iPhone");
			#if defined(__AMA_VOICE__)
			app_ama_create_spp_client_devId = id;
			//osTimerStop(app_ama_create_spp_client_timerId); 
			//osTimerStart(app_ama_create_spp_client_timerId, APP_AMA_CREATE_SPP_CLIENT_TIMEOUT_MS);
			#endif
		}else{
            TRACE("The connected is: Android");

			#if defined(__AMA_VOICE__)
			bes_ble_stop_advertising();
			osTimerStop(app_ama_create_spp_client_timerId); 
			osTimerStart(app_ama_create_spp_client_timerId, APP_AMA_CREATE_SPP_CLIENT_TIMEOUT_MS);
			#elif defined(__DMA_VOICE__)
			osTimerStop(app_dma_create_spp_client_timerId); 
			osTimerStart(app_dma_create_spp_client_timerId, APP_DMA_CREATE_SPP_CLIENT_TIMEOUT_MS);
			#endif
		}
#endif //defined(__AMA_VOICE__) || defined(__DMA_VOICE__) //change end by cai.zhong 20190103 for rfcomm
	}
    if (bt_profile_manager[id].has_connected &&
        (bt_profile_manager[id].hfp_connect != bt_profile_connect_status_success &&
         bt_profile_manager[id].hsp_connect != bt_profile_connect_status_success &&
         bt_profile_manager[id].a2dp_connect != bt_profile_connect_status_success)){

        bt_profile_manager[id].has_connected = false;
		
        //change begin by cai.zhong 20190103 for rfcomm
		if(app_is_ios(Chan) == true){
			TRACE("The disconnected is: iPhone");
		}else{
		    TRACE("The disconnected is: Android");
			#ifdef __AMA_VOICE__
			app_ama_link_free_after_rfcomm_dis();
			#endif
			#ifdef __DMA_VOICE__
			app_dma_link_free_after_rfcomm_dis();
			#endif
		}
        //change end by cai.zhong 20190103 for rfcomm
#ifdef MEDIA_PLAYER_SUPPORT
        app_voice_report(APP_STATUS_INDICATION_DISCONNECTED, id);
#endif
    }
}

void app_bt_profile_connect_manager_a2dp(enum BT_DEVICE_ID_T id, A2dpStream *Stream, const A2dpCallbackParms *Info)
{
    btdevice_profile *btdevice_plf_p = (btdevice_profile *)app_bt_profile_active_store_ptr_get((uint8_t *)Stream->stream.conn.remDev->bdAddr.addr);

    osTimerStop(bt_profile_manager[id].connect_timer);
    bt_profile_manager[id].connect_timer_cb = NULL;
    if (Stream&&Info){
        switch(Info->event)
        {
            case A2DP_EVENT_STREAM_OPEN:
                TRACE("%s A2DP_EVENT_STREAM_OPEN,codec type=%x",__func__,Info->p.configReq->codec.codecType);
                btdevice_plf_p->a2dp_act = true;
                btdevice_plf_p->a2dp_codectype = Info->p.configReq->codec.codecType;
#ifndef FPGA
                nv_record_touch_cause_flush();
#endif
                bt_profile_manager[id].a2dp_connect = bt_profile_connect_status_success;
                bt_profile_manager[id].reconnect_cnt = 0;
                bt_profile_manager[id].stream = app_bt_device.a2dp_connected_stream[id];
                memcpy(bt_profile_manager[id].rmt_addr.addr, Stream->stream.conn.remDev->bdAddr.addr, BD_ADDR_SIZE);
                if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_openreconnecting){
                    //do nothing
                }else if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_reconnecting){
                    if (btdevice_plf_p->hfp_act&& bt_profile_manager[id].hfp_connect != bt_profile_connect_status_success){
                        TRACE("!!!continue connect hfp\n");
                        app_bt_HF_CreateServiceLink(bt_profile_manager[id].chan, &bt_profile_manager[id].rmt_addr);
                    }
#if defined (__HSP_ENABLE__)
                    else if(btdevice_plf_p->hsp_act&& bt_profile_manager[id].hsp_connect != bt_profile_connect_status_success){
                        TRACE("!!!continue connect hsp\n");
                        app_bt_HS_CreateServiceLink(bt_profile_manager[id].hs_chan, &bt_profile_manager[id].rmt_addr);
                    }
#endif
                }
#ifdef __AUTO_CONNECT_OTHER_PROFILE__
                else{
                    if(btdevice_plf_p->hfp_act)
                        bt_profile_manager[id].connect_timer_cb = app_bt_profile_connect_hf_retry_timehandler;
#if defined (__HSP_ENABLE__)
                    else if(btdevice_plf_p->hsp_act)
                        bt_profile_manager[id].connect_timer_cb = app_bt_profile_connect_hs_retry_timehandler;
#endif
				    osTimerStart(bt_profile_manager[id].connect_timer, APP_BT_PROFILE_CONNECT_RETRY_MS);
                }
#endif
                break;
            case A2DP_EVENT_STREAM_CLOSED:
                TRACE("%s A2DP_EVENT_STREAM_CLOSED discReason:%d/%d",__func__, Info->discReason, A2DP_GetRemoteDevice(Stream)->discReason_saved);
                bt_profile_manager[id].a2dp_connect = bt_profile_connect_status_failure;

                if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_openreconnecting){
                   //do nothing
                }else if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_reconnecting){
                   if (++bt_profile_manager[id].reconnect_cnt < APP_BT_PROFILE_RECONNECT_RETRY_LIMIT_CNT){
                       osTimerStart(bt_profile_manager[id].connect_timer, APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS);
                   }else{
                       bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_null;
                   }
                   TRACE("%s try to reconnect cnt:%d",__func__, bt_profile_manager[id].reconnect_cnt);
                }else if(((Info->discReason == 0x8)||
                           (A2DP_GetRemoteDevice(Stream)->discReason_saved == 0x8)) &&
                          (btdevice_plf_p->a2dp_act)&&
                          (!btdevice_plf_p->hfp_act) &&
                          (!btdevice_plf_p->hsp_act)){
                    bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_reconnecting;
                    TRACE("%s try to reconnect cnt:%d",__func__, bt_profile_manager[id].reconnect_cnt);
					if(ble_force_stop==true){
						ble_force_stop=false;
#if IAG_BLE_INCLUDE == XA_ENABLED
						bes_ble_stop_advertising();
#endif
					}
                    osTimerStart(bt_profile_manager[id].connect_timer, APP_BT_PROFILE_RECONNECT_RETRY_INTERVAL_MS);
               }else{
                    bt_profile_manager[id].a2dp_connect = bt_profile_connect_status_unknow;
					if(ble_force_stop==false){
#if IAG_BLE_INCLUDE == XA_ENABLED						
						appm_restart_advertising();//reconnect timeout restart ble adv
#endif
						ble_force_stop=true;
					}
               }
               break;
            default:
                break;
        }
    }

    if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_reconnecting){
        bool reconnect_hfp_proc_final = true;
        bool reconnect_a2dp_proc_final = true;
        if (bt_profile_manager[id].hfp_connect == bt_profile_connect_status_failure){
            reconnect_hfp_proc_final = false;
        }
#if defined (__HSP_ENABLE__)
        if(btdevice_plf_p->hsp_act !=0){		//has HSP
            reconnect_hfp_proc_final = true;
            if (bt_profile_manager[id].hsp_connect == bt_profile_connect_status_failure){
	            reconnect_hfp_proc_final = false;
	        }
        }
#endif
        if (bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_failure){
            reconnect_a2dp_proc_final = false;
        }
        if (reconnect_hfp_proc_final && reconnect_a2dp_proc_final){
            TRACE("!!!reconnect success %d/%d/%d\n", bt_profile_manager[id].hfp_connect, bt_profile_manager[id].hsp_connect, bt_profile_manager[id].a2dp_connect);
            bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_null;
        }
    }else if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_openreconnecting){
        bool opening_hfp_proc_final = false;
        bool opening_a2dp_proc_final = false;

        if (btdevice_plf_p->hfp_act && bt_profile_manager[id].hfp_connect == bt_profile_connect_status_unknow){
            opening_hfp_proc_final = false;
        }else{
            opening_hfp_proc_final = true;
        }

        if (btdevice_plf_p->a2dp_act && bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_unknow){
            opening_a2dp_proc_final = false;
        }else{
            opening_a2dp_proc_final = true;
        }

        if ((opening_hfp_proc_final && opening_a2dp_proc_final) ||
            (bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_failure)){
            TRACE("!!!reconnect success %d/%d/%d\n", bt_profile_manager[id].hfp_connect, bt_profile_manager[id].hsp_connect, bt_profile_manager[id].a2dp_connect);
            bt_profile_manager[id].reconnect_mode = bt_profile_reconnect_null;
        }

        if (btdevice_plf_p->a2dp_act && bt_profile_manager[id].a2dp_connect== bt_profile_connect_status_success){
            if (btdevice_plf_p->hfp_act && !opening_hfp_proc_final){
                TRACE("!!!continue connect hf\n");
                app_bt_HF_CreateServiceLink(bt_profile_manager[id].chan, &bt_profile_manager[id].rmt_addr);
            }
#if defined (__HSP_ENABLE)
            else if(btdevice_plf_p->hsp_act && !opening_hfp_hsp_proc_final){
                TRACE("!!!continue connect hs\n");
                app_bt_HS_CreateServiceLink(bt_profile_manager[id].hs_chan, &bt_profile_manager[id].rmt_addr);
            }
#endif
        }

        if (bt_profile_manager[id].reconnect_mode == bt_profile_reconnect_null){
            for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                if (bt_profile_manager[i].reconnect_mode == bt_profile_reconnect_openreconnecting){
                    TRACE("!!!a2dp->start reconnect second device\n");
                    if (btdevice_plf_p->hfp_act){
                        TRACE("try connect hf");
                        app_bt_HF_CreateServiceLink(bt_profile_manager[i].chan, &bt_profile_manager[i].rmt_addr);
                    }
#if defined (__HSP_ENABLE__)
                    else if(btdevice_plf_p->hsp_act) {
                        TRACE("try connect hs");
                        app_bt_HS_CreateServiceLink(bt_profile_manager[i].hs_chan, &bt_profile_manager[i].rmt_addr);
                    }
#endif
					else if(btdevice_plf_p->a2dp_act) {
                        TRACE("try connect a2dp");
                         app_bt_A2DP_OpenStream(bt_profile_manager[i].stream, &bt_profile_manager[i].rmt_addr);
                     }
                    break;
                }
            }
        }
    }

    if (!bt_profile_manager[id].has_connected &&
        (bt_profile_manager[id].hfp_connect == bt_profile_connect_status_success ||
         bt_profile_manager[id].hsp_connect == bt_profile_connect_status_success||
         bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_success)){

        bt_profile_manager[id].has_connected = true;
		if(ble_force_stop==false){
#if IAG_BLE_INCLUDE == XA_ENABLED			
			appm_restart_advertising();
#endif
			ble_force_stop=true;
		}
#ifdef MEDIA_PLAYER_SUPPORT
        app_voice_report(APP_STATUS_INDICATION_CONNECTED, id);
#endif
#ifdef __THIRDPARTY
		//app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO2,THIRDPARTY_BT_CONNECTED,0);
#endif

    }
	if((bt_profile_manager[id].hfp_connect == bt_profile_connect_status_success)
		&&(bt_profile_manager[id].a2dp_connect == bt_profile_connect_status_success)){
#if defined(__AMA_VOICE__) || defined(__DMA_VOICE__) //change begin by cai.zhong 20190103 for rfcomm
		//if app is ios then start	app_ama_create_spp_client_timer
		if(app_dip_is_support_msbc(Stream->stream.conn.remDev) == true){
			TRACE("The connected is: iPhone");
			#if defined(__AMA_VOICE__)
			app_ama_create_spp_client_devId = id;
			//osTimerStop(app_ama_create_spp_client_timerId); 
			//osTimerStart(app_ama_create_spp_client_timerId, APP_AMA_CREATE_SPP_CLIENT_TIMEOUT_MS);
			#endif
		}else{
            TRACE("The connected is: Android");

			#if defined(__AMA_VOICE__)
			bes_ble_stop_advertising();
			osTimerStop(app_ama_create_spp_client_timerId); 
			osTimerStart(app_ama_create_spp_client_timerId, APP_AMA_CREATE_SPP_CLIENT_TIMEOUT_MS);
			#elif defined(__DMA_VOICE__)
			osTimerStop(app_dma_create_spp_client_timerId); 
			osTimerStart(app_dma_create_spp_client_timerId, APP_DMA_CREATE_SPP_CLIENT_TIMEOUT_MS);
			#endif
		}
#endif //defined(__AMA_VOICE__) || defined(__DMA_VOICE__) //change end by cai.zhong 20190103 for rfcomm
	}
    if (bt_profile_manager[id].has_connected &&
        (bt_profile_manager[id].hfp_connect != bt_profile_connect_status_success &&
         bt_profile_manager[id].hsp_connect != bt_profile_connect_status_success &&
         bt_profile_manager[id].a2dp_connect != bt_profile_connect_status_success)){

        bt_profile_manager[id].has_connected = false;

        //change begin by cai.zhong 20190103 for rfcomm
		if(app_dip_is_support_msbc(Stream->stream.conn.remDev) == true){
			TRACE("The disconnected is: iPhone");
		}else{
		    TRACE("The disconnected is: Android");
			#ifdef __AMA_VOICE__
			app_ama_link_free_after_rfcomm_dis();
			#endif
			#ifdef __DMA_VOICE__
			app_dma_link_free_after_rfcomm_dis();
			#endif
		}
        //change end by cai.zhong 20190103 for rfcomm
#ifdef MEDIA_PLAYER_SUPPORT
        app_voice_report(APP_STATUS_INDICATION_DISCONNECTED, id);
#endif
    }

}

#if HID_DEVICE == XA_ENABLED
void hid_exit_shutter_mode(void);
#endif

extern "C" U8 wait_detach_finished;

BtStatus AVRCP_Disconnect(AvrcpChannel *chnl);

static void app_link_dis_timeout_timer_cb(void const *n)
{
	TRACE("------>link monitor TIME OUT");
	if(wait_detach_finished==0){
		//AVRCP_Disconnect(&app_bt_device.avrcp_channel[BT_DEVICE_ID_1]);
		app_bt_AVRCP_DisconnectServiceLink(&app_bt_device.avrcp_channel[BT_DEVICE_ID_1]);
	}
}


osTimerDef (APP_LINK_DIS_TIMEOUT, app_link_dis_timeout_timer_cb); 
osTimerId	app_link_dis_timeout_timer_id = NULL;



BtStatus LinkDisconnectDirectly(void)
{
    OS_LockStack();
	wait_detach_finished = 0;
	if(app_link_dis_timeout_timer_id==NULL)
		app_link_dis_timeout_timer_id = osTimerCreate(osTimer(APP_LINK_DIS_TIMEOUT), osTimerOnce, NULL); 
	bool device_active = false;

    TRACE("%s id1 hf:%d a2dp:%d",__func__, app_bt_device.hf_channel[BT_DEVICE_ID_1].state, app_bt_device.a2dp_stream[BT_DEVICE_ID_1].stream.state);
#if HID_DEVICE == XA_ENABLED
    hid_exit_shutter_mode();
#endif
    if(app_bt_device.hf_channel[BT_DEVICE_ID_1].state == HF_STATE_OPEN){
		device_active=true;
        app_bt_HF_DisconnectServiceLink(&app_bt_device.hf_channel[BT_DEVICE_ID_1]);
    }
#if defined (__HSP_ENABLE__)
    if(app_bt_device.hs_channel[BT_DEVICE_ID_1].state == HS_STATE_OPEN){
		device_active=true;
        app_bt_HS_DisconnectServiceLink(&app_bt_device.hs_channel[BT_DEVICE_ID_1]);
    }
#endif
    if(app_bt_device.a2dp_stream[BT_DEVICE_ID_1].stream.state == AVDTP_STRM_STATE_STREAMING ||
        app_bt_device.a2dp_stream[BT_DEVICE_ID_1].stream.state == AVDTP_STRM_STATE_OPEN){
        device_active=true;
        app_bt_A2DP_CloseStream(&app_bt_device.a2dp_stream[BT_DEVICE_ID_1]);
    }
#if defined(A2DP_AAC_ON)
    if(app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_1].stream.state == AVDTP_STRM_STATE_STREAMING ||
        app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_1].stream.state == AVDTP_STRM_STATE_OPEN){
        device_active=true;
        app_bt_A2DP_CloseStream(&app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_1]);
    }
#endif
#if defined(A2DP_SCALABLE_ON)
    if(app_bt_device.a2dp_scalable_stream[BT_DEVICE_ID_1].stream.state == AVDTP_STRM_STATE_STREAMING ||
        app_bt_device.a2dp_scalable_stream[BT_DEVICE_ID_1].stream.state == AVDTP_STRM_STATE_OPEN){
        device_active=true;
        app_bt_A2DP_CloseStream(&app_bt_device.a2dp_scalable_stream[BT_DEVICE_ID_1]);
    }
#endif

#ifdef __BT_ONE_BRING_TWO__
    TRACE("%s id2 hf:%d a2dp:%d",__func__, app_bt_device.hf_channel[BT_DEVICE_ID_2].state, app_bt_device.a2dp_stream[BT_DEVICE_ID_2].stream.state);
    if(app_bt_device.hf_channel[BT_DEVICE_ID_2].state == HF_STATE_OPEN){
        app_bt_HF_DisconnectServiceLink(&app_bt_device.hf_channel[BT_DEVICE_ID_2]);
    }

#if defined (__HSP_ENABLE__)
		if(app_bt_device.hs_channel[BT_DEVICE_ID_2].state == HS_STATE_OPEN){
			app_bt_HS_DisconnectServiceLink(&app_bt_device.hs_channel[BT_DEVICE_ID_2]);
		}
#endif

    if(app_bt_device.a2dp_stream[BT_DEVICE_ID_2].stream.state == AVDTP_STRM_STATE_STREAMING ||
        app_bt_device.a2dp_stream[BT_DEVICE_ID_2].stream.state == AVDTP_STRM_STATE_OPEN){
        app_bt_A2DP_CloseStream(&app_bt_device.a2dp_stream[BT_DEVICE_ID_2]);
    }
#if defined(A2DP_AAC_ON)
    if(app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_2].stream.state == AVDTP_STRM_STATE_STREAMING ||
        app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_2].stream.state == AVDTP_STRM_STATE_OPEN){
        app_bt_A2DP_CloseStream(&app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_2]);
    }
#endif
#if defined(A2DP_SCALABLE_ON)
    if(app_bt_device.a2dp_scalable_stream[BT_DEVICE_ID_2].stream.state == AVDTP_STRM_STATE_STREAMING ||
        app_bt_device.a2dp_scalable_stream[BT_DEVICE_ID_2].stream.state == AVDTP_STRM_STATE_OPEN){
        app_bt_A2DP_CloseStream(&app_bt_device.a2dp_scalable_stream[BT_DEVICE_ID_2]);
    }
#endif
#endif
#ifdef __AMA_VOICE__
    //if rfcomm connect ,then disconnlink
    app_ama_rfcomm_disconnlink();
#endif
//change begin by cai.zhong 20190103 for rfcomm
#ifdef __DMA_VOICE__
    //if rfcomm connect ,then disconnlink
    app_dma_rfcomm_disconnlink();
#endif
//change end by cai.zhong 20190103 for rfcomm

	if(device_active){
		osTimerStop(app_link_dis_timeout_timer_id); 
		osTimerStart(app_link_dis_timeout_timer_id, 3000);	
	}

    OS_UnlockStack();
    return BT_STATUS_SUCCESS;
}

void app_bt_init(void)
{
    app_bt_mail_init();
    app_set_threadhandle(APP_MODUAL_BT, app_bt_handle_process);
//    SecSetIoCapRspRejectExt(app_bt_profile_connect_openreconnecting);
}

#ifdef __TWS_CHARGER_BOX__

extern "C" {
    BtStatus ME_Ble_Clear_Whitelist(void);
    BtStatus ME_Ble_Set_Private_Address(BT_BD_ADDR *addr);
    BtStatus ME_Ble_Add_Dev_To_Whitelist(U8 addr_type,BT_BD_ADDR *addr);
    BtStatus ME_Ble_SetAdv_data(U8 len, U8 *data);
    BtStatus ME_Ble_SetAdv_parameters(adv_para_struct *para);
    BtStatus ME_Ble_SetAdv_en(U8 en);
    BtStatus ME_Ble_Setscan_parameter(scan_para_struct *para);
    BtStatus ME_Ble_Setscan_en(U8 scan_en,  U8 filter_duplicate);
}


int8_t power_level=0;
#define TWS_BOX_OPEN 1
#define TWS_BOX_CLOSE 0
void app_tws_box_set_slave_adv_data(uint8_t power_level,uint8_t box_status)
{
    uint8_t adv_data[] = {
        0x02,0xfe, 0x00,
        0x02, 0xfd, 0x00  // manufacturer data
    };

    adv_data[2] = power_level;

    adv_data[5] = box_status;
    ME_Ble_SetAdv_data(sizeof(adv_data), adv_data);

}


void app_tws_box_set_slave_adv_para(void)
{
    uint8_t  peer_addr[BD_ADDR_SIZE] = {0};
    adv_para_struct para;


    para.interval_min =             0x0040; // 20ms
    para.interval_max =             0x0040; // 20ms
    para.adv_type =                 0x03;
    para.own_addr_type =            0x01;
    para.peer_addr_type =           0x01;
    para.adv_chanmap =              0x07;
    para.adv_filter_policy =        0x00;
    memcpy(para.bd_addr.addr, peer_addr, BD_ADDR_SIZE);

    ME_Ble_SetAdv_parameters(&para);

}


extern uint8_t bt_addr[6];
void app_tws_start_chargerbox_adv(void)
{
    app_tws_box_set_slave_adv_data(power_level,TWS_BOX_OPEN);
    ME_Ble_Set_Private_Address((BT_BD_ADDR *)bt_addr);
    app_tws_box_set_slave_adv_para();
    ME_Ble_SetAdv_en(1);

}



#endif

bool app_bluetooth_is_connected(void){
	return bt_profile_manager[BT_DEVICE_ID_1].has_connected;
}

bool a2dp_service_is_connected(void){
	return (bt_profile_manager[BT_DEVICE_ID_1].a2dp_connect == bt_profile_connect_status_success);
}


bool app_is_hfp_service_connected(void)
{
    return (bt_profile_manager[BT_DEVICE_ID_1].hfp_connect == bt_profile_connect_status_success);
}

bool app_is_link_connected()
{
	if(MEC(activeCons) == 0)
		return false;
	else
		return true;
}

#define COUNT_OF_PENDING_REMOTE_DEV_TO_EXIT_SNIFF_MODE  8
static BtRemoteDevice* pendingRemoteDevToExitSniffMode[COUNT_OF_PENDING_REMOTE_DEV_TO_EXIT_SNIFF_MODE];
static uint8_t  maskOfRemoteDevPendingForExitingSniffMode = 0;
void app_check_pending_stop_sniff_op(void)
{
    if (maskOfRemoteDevPendingForExitingSniffMode > 0)
    {
        for (uint8_t index = 0;index < COUNT_OF_PENDING_REMOTE_DEV_TO_EXIT_SNIFF_MODE;index++)
        {
            if (maskOfRemoteDevPendingForExitingSniffMode & (1 << index))
            {
                BtRemoteDevice* remDev = pendingRemoteDevToExitSniffMode[index];
                if (!ME_IsOpInProgress(remDev))
                {
                    if (ME_GetRemDevState(remDev) == BDS_CONNECTED){
                        if (ME_GetCurrentMode(remDev) == BLM_SNIFF_MODE){
                            TRACE("!!! stop sniff currmode:%d\n", ME_GetCurrentMode(remDev));
                            BtStatus ret =  ME_StopSniff(remDev);
                            TRACE("Return status %d", ret);
                            if (BT_STATUS_IN_PROGRESS != ret)
                            {
                                maskOfRemoteDevPendingForExitingSniffMode &= (~(1<<index));
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (maskOfRemoteDevPendingForExitingSniffMode > 0)
        {
            OS_NotifyEvm();
        }
    }
}

void app_add_pending_stop_sniff_op(BtRemoteDevice* remDev)
{
    for (uint8_t index = 0;index < COUNT_OF_PENDING_REMOTE_DEV_TO_EXIT_SNIFF_MODE;index++)
    {
        if (maskOfRemoteDevPendingForExitingSniffMode & (1 << index))
        {
            if (pendingRemoteDevToExitSniffMode[index] == remDev)
            {
                return;
            }
        }
    }

    for (uint8_t index = 0;index < COUNT_OF_PENDING_REMOTE_DEV_TO_EXIT_SNIFF_MODE;index++)
    {
        if (0 == (maskOfRemoteDevPendingForExitingSniffMode & (1 << index)))
        {
            pendingRemoteDevToExitSniffMode[index] = remDev;
            maskOfRemoteDevPendingForExitingSniffMode |= (1 << index);
        }
    }
}

static void app_bt_print_pending_set_linkpolicy_req(void)
{
    TRACE("Pending set link policy requests:");
    uint8_t index = pending_set_linkpolicy_out_cursor;
    while (index != pending_set_linkpolicy_in_cursor)
    {
        TRACE("index %d RemDev 0x%x LinkPolicy %d", index,
            pending_set_linkpolicy_req[index].remDev,
            pending_set_linkpolicy_req[index].policy);
        index++;
        if (PENDING_SET_LINKPOLICY_REQ_BUF_CNT == index)
        {
            index = 0;
        }
    }
}

static void app_bt_push_pending_set_linkpolicy(BtRemoteDevice *remDev, BtLinkPolicy policy)
{
    // go through the existing pending list to see if the remDev is already in
    uint8_t index = pending_set_linkpolicy_out_cursor;
    while (index != pending_set_linkpolicy_in_cursor)
    {
        if (remDev == pending_set_linkpolicy_req[index].remDev)
        {
            pending_set_linkpolicy_req[index].policy = policy;
            return;
        }
        index++;
        if (PENDING_SET_LINKPOLICY_REQ_BUF_CNT == index)
        {
            index = 0;
        }
    }

    pending_set_linkpolicy_req[pending_set_linkpolicy_in_cursor].remDev = remDev;
    pending_set_linkpolicy_req[pending_set_linkpolicy_in_cursor].policy = policy;
    pending_set_linkpolicy_in_cursor++;
    if (PENDING_SET_LINKPOLICY_REQ_BUF_CNT == pending_set_linkpolicy_in_cursor)
    {
        pending_set_linkpolicy_in_cursor = 0;
    }

    app_bt_print_pending_set_linkpolicy_req();
}

static BT_SET_LINKPOLICY_REQ_T* app_bt_pop_pending_set_linkpolicy(void)
{
    if (pending_set_linkpolicy_out_cursor == pending_set_linkpolicy_in_cursor)
    {
        return NULL;
    }

    BT_SET_LINKPOLICY_REQ_T* ptReq = &pending_set_linkpolicy_req[pending_set_linkpolicy_out_cursor];
    pending_set_linkpolicy_out_cursor++;
    if (PENDING_SET_LINKPOLICY_REQ_BUF_CNT == pending_set_linkpolicy_out_cursor)
    {
        pending_set_linkpolicy_out_cursor = 0;
    }

    app_bt_print_pending_set_linkpolicy_req();
    return ptReq;
}

static void app_bt_set_linkpolicy(BtRemoteDevice *remDev, BtLinkPolicy policy)
{
    BtStatus ret = Me_SetLinkPolicy(remDev, policy);
    TRACE("%s policy %d returns %d", __FUNCTION__, policy, ret);

    OS_LockStack();
    if (BT_STATUS_IN_PROGRESS == ret)
    {
        app_bt_push_pending_set_linkpolicy(remDev, policy);
    }
    OS_UnlockStack();
}

BtRemoteDevice* app_bt_get_remoteDev(uint8_t deviceId)
{
    BtRemoteDevice* currentRemDev = NULL;

    if(app_bt_device.a2dp_stream[app_bt_device.curr_a2dp_stream_id].stream.state
        == AVDTP_STRM_STATE_STREAMING ||
        app_bt_device.a2dp_stream[app_bt_device.curr_a2dp_stream_id].stream.state
        == AVDTP_STRM_STATE_OPEN)
    {
        currentRemDev =
            app_bt_device.a2dp_stream[app_bt_device.curr_a2dp_stream_id].stream.conn.remDev;
    }
    else if (app_bt_device.hf_channel[app_bt_device.curr_hf_channel_id].state
        == HF_STATE_OPEN)
    {
        currentRemDev =
            app_bt_device.hf_channel[app_bt_device.curr_hf_channel_id].cmgrHandler.remDev;
    }

    TRACE("%s get current Remdev 0x%x", __FUNCTION__, currentRemDev);

    return currentRemDev;
}

void app_bt_stay_active(uint8_t deviceId)
{
    BtRemoteDevice* currentRemDev = app_bt_get_remoteDev(deviceId);
    if (currentRemDev)
    {
        CmgrHandler    *cmgrHandler;
        /* Clear the sniff timer */
        cmgrHandler = CMGR_GetAclHandler(currentRemDev);
        CMGR_ClearSniffTimer(cmgrHandler);
        CMGR_DisableSniffTimer(cmgrHandler);
        app_bt_set_linkpolicy(currentRemDev, BLP_MASTER_SLAVE_SWITCH);
    }
}

void app_bt_allow_sniff(uint8_t deviceId)
{
    BtRemoteDevice* currentRemDev = app_bt_get_remoteDev(deviceId);
    if (currentRemDev)
    {
        CmgrHandler    *cmgrHandler;
        /* Enable the sniff timer */
        cmgrHandler = CMGR_GetAclHandler(currentRemDev);

        /* Start the sniff timer */
        BtSniffInfo sniffInfo;
        sniffInfo.minInterval = CMGR_SNIFF_MIN_INTERVAL;
        sniffInfo.maxInterval = CMGR_SNIFF_MAX_INTERVAL;
        sniffInfo.attempt = CMGR_SNIFF_ATTEMPT;
        sniffInfo.timeout = CMGR_SNIFF_TIMEOUT;
        if (cmgrHandler){
            CMGR_SetSniffTimer(cmgrHandler, &sniffInfo, CMGR_SNIFF_TIMER);
        }
        app_bt_set_linkpolicy(currentRemDev, BLP_MASTER_SLAVE_SWITCH | BLP_SNIFF_MODE);
    }
}

void app_bt_stop_sniff(uint8_t deviceId)
{
    BtRemoteDevice* currentRemDev = app_bt_get_remoteDev(deviceId);

    if (currentRemDev && (ME_GetRemDevState(currentRemDev) == BDS_CONNECTED)){
        if (ME_GetCurrentMode(currentRemDev) == BLM_SNIFF_MODE){
            TRACE("!!! stop sniff currmode:%d\n", ME_GetCurrentMode(currentRemDev));
            BtStatus ret =  ME_StopSniff(currentRemDev);
            TRACE("Return status %d", ret);
            if (BT_STATUS_IN_PROGRESS == ret)
            {
                app_add_pending_stop_sniff_op(currentRemDev);
            }
        }
    }
}


