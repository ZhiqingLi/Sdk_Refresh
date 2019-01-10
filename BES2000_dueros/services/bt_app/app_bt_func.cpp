#include "cmsis_os.h"
#include "string.h"
#include "hal_trace.h"

#include "besbt.h"
#include "app_bt_func.h"
#include "avrcp.h"
extern "C" void OS_NotifyEvm(void);

#define APP_BT_MAILBOX_MAX (15)
osMailQDef (app_bt_mailbox, APP_BT_MAILBOX_MAX, APP_BT_MAIL);
static osMailQId app_bt_mailbox = NULL;

extern BtStatus AVRCP_Disconnect(AvrcpChannel *chnl);

inline int app_bt_mail_process(APP_BT_MAIL* mail_p)
{
    BtStatus status = BT_STATUS_LAST_CODE;
    if (mail_p->request_id != CMGR_SetSniffTimer_req){
        TRACE("%s src_thread:0x%08x request_id:%d enter",__func__, mail_p->src_thread, mail_p->request_id);
    }
    switch (mail_p->request_id) {     
        case Me_switch_sco_req:
            status = Me_switch_sco(mail_p->param.Me_switch_sco_param.scohandle);
            break;
        case ME_SwitchRole_req:
            status = ME_SwitchRole(mail_p->param.ME_SwitchRole_param.remDev);
            break;
        case ME_SetConnectionRole_req:
            ME_SetConnectionRole(mail_p->param.BtConnectionRole_param.role);
            status = BT_STATUS_SUCCESS;
            break;            
        case MeDisconnectLink_req:
            MeDisconnectLink(mail_p->param.MeDisconnectLink_param.remDev);
            status = BT_STATUS_SUCCESS;
            break;
        case ME_StopSniff_req:
            status = ME_StopSniff(mail_p->param.ME_StopSniff_param.remDev);
            break;
        case ME_SetAccessibleMode_req:
            status = ME_SetAccessibleMode(mail_p->param.ME_SetAccessibleMode_param.mode,
                                          &mail_p->param.ME_SetAccessibleMode_param.info);
            break;
        case Me_SetLinkPolicy_req:
            status = Me_SetLinkPolicy(mail_p->param.Me_SetLinkPolicy_param.remDev,
                                      mail_p->param.Me_SetLinkPolicy_param.policy);
            break;
        case CMGR_SetSniffTimer_req:
            if (mail_p->param.CMGR_SetSniffTimer_param.SniffInfo.maxInterval == 0){
                status = CMGR_SetSniffTimer(mail_p->param.CMGR_SetSniffTimer_param.Handler,
                                            NULL,
                                            mail_p->param.CMGR_SetSniffTimer_param.Time);
            }else{
                status = CMGR_SetSniffTimer(mail_p->param.CMGR_SetSniffTimer_param.Handler,
                                            &mail_p->param.CMGR_SetSniffTimer_param.SniffInfo,
                                            mail_p->param.CMGR_SetSniffTimer_param.Time);
            }
            break;
        case CMGR_SetSniffInofToAllHandlerByRemDev_req:
            status = CMGR_SetSniffInofToAllHandlerByRemDev(&mail_p->param.CMGR_SetSniffInofToAllHandlerByRemDev_param.SniffInfo,
                                                            mail_p->param.CMGR_SetSniffInofToAllHandlerByRemDev_param.RemDev);
            break;
        case A2DP_OpenStream_req:
            status = A2DP_OpenStream(mail_p->param.A2DP_OpenStream_param.Stream,
                                     mail_p->param.A2DP_OpenStream_param.Addr);
            break;
        case A2DP_CloseStream_req:
            status = A2DP_CloseStream(mail_p->param.A2DP_CloseStream_param.Stream);
            break;
        case A2DP_SetMasterRole_req:
            status = A2DP_SetMasterRole(mail_p->param.A2DP_SetMasterRole_param.Stream,
                                        mail_p->param.A2DP_SetMasterRole_param.Flag);
            break;
        case HF_CreateServiceLink_req:
            status = HF_CreateServiceLink(mail_p->param.HF_CreateServiceLink_param.Chan,
                                          mail_p->param.HF_CreateServiceLink_param.Addr);
            break;
        case HF_DisconnectServiceLink_req:
            status = HF_DisconnectServiceLink(mail_p->param.HF_DisconnectServiceLink_param.Chan);
            break;
        case HF_CreateAudioLink_req:
            status = HF_CreateAudioLink(mail_p->param.HF_CreateAudioLink_param.Chan);
            break;
        case HF_DisconnectAudioLink_req:
            status = HF_DisconnectAudioLink(mail_p->param.HF_DisconnectAudioLink_param.Chan);
            break;
        case HF_EnableSniffMode_req:
            status = HF_EnableSniffMode(mail_p->param.HF_EnableSniffMode_param.Chan,
                                        mail_p->param.HF_EnableSniffMode_param.Enable);
            break;
        case HF_SetMasterRole_req:
            status = HF_SetMasterRole(mail_p->param.HF_SetMasterRole_param.Chan,
                                      mail_p->param.HF_SetMasterRole_param.Flag);
            break;
		case DIP_QuryService_req:
			status = DipQueryForService(mail_p->param.DIP_QuryService_param.dip_dev,
							  mail_p->param.DIP_QuryService_param.remDev);
			break;	
#if defined (__HSP_ENABLE__)
        case HS_CreateServiceLink_req:
            status = HS_CreateServiceLink(mail_p->param.HS_CreateServiceLink_param.Chan,
                                          mail_p->param.HS_CreateServiceLink_param.Addr);
            break;
        case HS_CreateAudioLink_req:
            status = HS_CreateAudioLink(mail_p->param.HS_CreateAudioLink_param.Chan);
            break;
        case HS_DisconnectAudioLink_req:
            status = HS_DisconnectAudioLink(mail_p->param.HS_DisconnectAudioLink_param.Chan);
            break;
		case HS_DisconnectServiceLink_req:
			status = HS_DisconnectServiceLink(mail_p->param.HS_DisconnectServiceLink_param.Chan);
			break;
        case HS_EnableSniffMode_req:
            status = HS_EnableSniffMode(mail_p->param.HS_EnableSniffMode_param.Chan,
                                        mail_p->param.HS_EnableSniffMode_param.Enable);
            break;
#endif
		case BT_Custom_Func_req:
//			  TRACE("%s:%d\n", __func__, __LINE__);
			if (mail_p->param.CustomFunc_param.func_ptr){
 // 			  TRACE("%s:%d\n", __func__, __LINE__);
				((APP_BT_REQ_CUSTOMER_CALl_CB_T)(mail_p->param.CustomFunc_param.func_ptr))(
					(void *)mail_p->param.CustomFunc_param.param0, 
					(void *)mail_p->param.CustomFunc_param.param1);
			}
			break;

		 case BT_avrcp_dis_req:
            status = AVRCP_Disconnect(mail_p->param.AVRCP_DisconnectServiceLink_param.Chan);
            break;
    }
    
    if (mail_p->request_id != CMGR_SetSniffTimer_req){
        TRACE("%s request_id:%d :status:%d exit",__func__, mail_p->request_id, status);
    }
    return 0;
}

inline int app_bt_mail_alloc(APP_BT_MAIL** mail)
{
	*mail = (APP_BT_MAIL*)osMailAlloc(app_bt_mailbox, 0);
	ASSERT(*mail, "app_bt_mail_alloc error");
    return 0;
}

inline int app_bt_mail_send(APP_BT_MAIL* mail)
{
	osStatus status;

	ASSERT(mail, "osMailAlloc NULL");	
	status = osMailPut(app_bt_mailbox, mail);
    ASSERT(osOK == status, "osMailAlloc Put failed");

    OS_NotifyEvm();

	return (int)status;
}

inline int app_bt_mail_free(APP_BT_MAIL* mail_p)
{
	osStatus status;
	
	status = osMailFree(app_bt_mailbox, mail_p);
    ASSERT(osOK == status, "osMailAlloc Put failed");
	
	return (int)status;
}

inline int app_bt_mail_get(APP_BT_MAIL** mail_p)
{
	osEvent evt;
	evt = osMailGet(app_bt_mailbox, 0);
	if (evt.status == osEventMail) {
		*mail_p = (APP_BT_MAIL *)evt.value.p;
		return 0;
	}
	return -1;
}

static void app_bt_mail_poll(void)
{
	APP_BT_MAIL *mail_p = NULL;
	if (!app_bt_mail_get(&mail_p)){
        app_bt_mail_process(mail_p);
		app_bt_mail_free(mail_p);
	}        
}

int app_bt_mail_init(void)
{
	app_bt_mailbox = osMailCreate(osMailQ(app_bt_mailbox), NULL);
	if (app_bt_mailbox == NULL)  {
        TRACE("Failed to Create app_mailbox\n");
		return -1;
	}
    Besbt_hook_handler_set(BESBT_HOOK_USER_1, app_bt_mail_poll);

	return 0;
}

int app_bt_Me_switch_sco(uint16_t  scohandle)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = Me_switch_sco_req;
    mail->param.Me_switch_sco_param.scohandle = scohandle;
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_ME_SwitchRole(BtRemoteDevice* remDev)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = ME_SwitchRole_req;
    mail->param.ME_SwitchRole_param.remDev = remDev;
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_ME_SetConnectionRole(BtConnectionRole role)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = ME_SetConnectionRole_req;
    mail->param.BtConnectionRole_param.role = role;
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_MeDisconnectLink(BtRemoteDevice* remDev)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = MeDisconnectLink_req;
    mail->param.MeDisconnectLink_param.remDev = remDev;
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_ME_StopSniff(BtRemoteDevice *remDev)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = ME_StopSniff_req;
    mail->param.ME_StopSniff_param.remDev = remDev;
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_ME_SetAccessibleMode(BtAccessibleMode mode, const BtAccessModeInfo *info)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = ME_SetAccessibleMode_req;
    mail->param.ME_SetAccessibleMode_param.mode = mode;    
    memcpy(&mail->param.ME_SetAccessibleMode_param.info, info, sizeof(BtAccessModeInfo));
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_Me_SetLinkPolicy(BtRemoteDevice *remDev, BtLinkPolicy policy)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = Me_SetLinkPolicy_req;
    mail->param.Me_SetLinkPolicy_param.remDev = remDev;    
    mail->param.Me_SetLinkPolicy_param.policy = policy;    
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_CMGR_SetSniffTimer(   CmgrHandler *Handler, 
                                                BtSniffInfo* SniffInfo, 
                                                TimeT Time)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = CMGR_SetSniffTimer_req;
    mail->param.CMGR_SetSniffTimer_param.Handler = Handler;
    if (SniffInfo){
        memcpy(&mail->param.CMGR_SetSniffTimer_param.SniffInfo, SniffInfo, sizeof(BtSniffInfo));
    }else{
        memset(&mail->param.CMGR_SetSniffTimer_param.SniffInfo, 0, sizeof(BtSniffInfo));
    }
    mail->param.CMGR_SetSniffTimer_param.Time = Time;    
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_CMGR_SetSniffInofToAllHandlerByRemDev(BtSniffInfo* SniffInfo, 
                            								     BtRemoteDevice *RemDev)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = CMGR_SetSniffInofToAllHandlerByRemDev_req;
    memcpy(&mail->param.CMGR_SetSniffInofToAllHandlerByRemDev_param.SniffInfo, SniffInfo, sizeof(BtSniffInfo));    
    mail->param.CMGR_SetSniffInofToAllHandlerByRemDev_param.RemDev = RemDev;    
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_A2DP_OpenStream(A2dpStream *Stream, BT_BD_ADDR *Addr)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = A2DP_OpenStream_req;
    mail->param.A2DP_OpenStream_param.Stream = Stream;    
    mail->param.A2DP_OpenStream_param.Addr = Addr;    
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_A2DP_CloseStream(A2dpStream *Stream)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = A2DP_CloseStream_req;
    mail->param.A2DP_CloseStream_param.Stream = Stream;    
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_A2DP_SetMasterRole(A2dpStream *Stream, BOOL Flag)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = A2DP_SetMasterRole_req;
    mail->param.A2DP_SetMasterRole_param.Stream = Stream;    
    mail->param.A2DP_SetMasterRole_param.Flag = Flag;    
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_HF_CreateServiceLink(HfChannel *Chan, BT_BD_ADDR *Addr)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = HF_CreateServiceLink_req;
    mail->param.HF_CreateServiceLink_param.Chan = Chan;    
    mail->param.HF_CreateServiceLink_param.Addr = Addr;    
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_HF_DisconnectServiceLink(HfChannel *Chan)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = HF_DisconnectServiceLink_req;
    mail->param.HF_DisconnectServiceLink_param.Chan = Chan;    
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_HF_CreateAudioLink(HfChannel *Chan)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = HF_CreateAudioLink_req;
    mail->param.HF_CreateAudioLink_param.Chan = Chan;    
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_HF_DisconnectAudioLink(HfChannel *Chan)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = HF_DisconnectAudioLink_req;
    mail->param.HF_DisconnectAudioLink_param.Chan = Chan;    
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_HF_EnableSniffMode(HfChannel *Chan, BOOL Enable)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = HF_EnableSniffMode_req;
    mail->param.HF_EnableSniffMode_param.Chan = Chan;    
    mail->param.HF_EnableSniffMode_param.Enable = Enable;    
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_HF_SetMasterRole(HfChannel *Chan, BOOL Flag)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = HF_SetMasterRole_req;
    mail->param.HF_SetMasterRole_param.Chan = Chan;    
    mail->param.HF_SetMasterRole_param.Flag = Flag;    
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_dip_QuryService(DipClient *dev, BtRemoteDevice* rem)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = DIP_QuryService_req;
    mail->param.DIP_QuryService_param.remDev = rem;    
    mail->param.DIP_QuryService_param.dip_dev = dev;    
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_AVRCP_DisconnectServiceLink(AvrcpChannel *Chan)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = BT_avrcp_dis_req;
    mail->param.AVRCP_DisconnectServiceLink_param.Chan = Chan;    
    app_bt_mail_send(mail);
    return 0;
}



#if defined (__HSP_ENABLE__)
int app_bt_HS_CreateServiceLink(HsChannel *Chan, BT_BD_ADDR *Addr)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = HS_CreateServiceLink_req;
    mail->param.HS_CreateServiceLink_param.Chan = Chan;    
    mail->param.HS_CreateServiceLink_param.Addr = Addr;    
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_HS_CreateAudioLink(HsChannel *Chan)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = HS_CreateAudioLink_req;
    mail->param.HS_CreateAudioLink_param.Chan = Chan;    
    app_bt_mail_send(mail);
    return 0;
}


int app_bt_HS_DisconnectAudioLink(HsChannel *Chan)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = HS_DisconnectAudioLink_req;
    mail->param.HS_DisconnectAudioLink_param.Chan = Chan;    
    app_bt_mail_send(mail);
    return 0;
}

int app_bt_HS_DisconnectServiceLink(HsChannel *Chan)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = HS_DisconnectServiceLink_req;
    mail->param.HS_DisconnectServiceLink_param.Chan = Chan;    
    app_bt_mail_send(mail);
    return 0;
}


int app_bt_HS_EnableSniffMode(HsChannel *Chan, BOOL Enable)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = HS_EnableSniffMode_req;
    mail->param.HS_EnableSniffMode_param.Chan = Chan;    
    mail->param.HS_EnableSniffMode_param.Enable = Enable;    
    app_bt_mail_send(mail);
    return 0;
}

#endif

int app_bt_start_custom_function_in_bt_thread(
    uint32_t param0, uint32_t param1, uint32_t funcPtr)
{
    APP_BT_MAIL* mail;
    app_bt_mail_alloc(&mail);
    mail->src_thread = (uint32_t)osThreadGetId();
    mail->request_id = BT_Custom_Func_req;
    mail->param.CustomFunc_param.func_ptr = funcPtr;    
    mail->param.CustomFunc_param.param0 = param0;    
    mail->param.CustomFunc_param.param1 = param1;    
    app_bt_mail_send(mail);
    return 0;
}

