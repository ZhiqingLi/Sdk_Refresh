//#include "mbed.h"
#include <stdio.h>
#include "cmsis_os.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "audioflinger.h"
#include "lockcqueue.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "analog.h"

extern "C" {
#include "eventmgr.h"
#include "me.h"
#include "mei.h"
#include "sec.h"
#include "a2dp.h"
#include "avdtp.h"
#include "avctp.h"
#include "avrcp.h"
#include "hf.h"
}

#include "rtos.h"
#include "besbt.h"

#include "cqueue.h"
#include "btapp.h"

#include "apps.h"

/* pair */
BtHandler pair_handler;
void pair_handler_func(const BtEvent* evt)
{
//    TRACE("!!!pair_handler_func  evt->eType:%d\n",evt->eType);
    switch(evt->eType) {
    case BTEVENT_DISPLAY_NUMERIC_IND:
        break;
    case BTEVENT_CONFIRM_NUMERIC_REQ:
        SEC_UserConfirm(evt->p.userIoReq.remDev, BUC_YES, BPT_SAVE_TRUSTED);
        break;
    case BTEVENT_PASS_KEY_REQ:
        SEC_SetPassKey(evt->p.userIoReq.remDev, (const unsigned char*)"0000", 4, BPT_SAVE_TRUSTED);
        break;
    case BTEVENT_PIN_REQ:
        //CMGR_ScoSetEDR(EDR_ENABLED);
        SEC_SetPin(evt->p.userIoReq.remDev, (const unsigned char*)"0000", 4, BPT_SAVE_TRUSTED);
        break;
    case BTEVENT_PAIRING_COMPLETE:
        TRACE("::BTEVENT_PAIRING_COMPLETE  keyType:%x\n",evt->p.pairingInfo.keyType);
#ifndef FPGA
#ifdef MEDIA_PLAYER_SUPPORT
        if(evt->errCode == BEC_NO_ERROR){
            app_voice_report(APP_STATUS_INDICATION_PAIRSUCCEED,0);
        }else{
            app_voice_report(APP_STATUS_INDICATION_PAIRFAIL,0);
        }
#endif
#endif
        break;
    }
    return;
}

#if BT_SECURITY == XA_ENABLED
BtHandler auth_handler;
void auth_handler_func(const BtEvent* evt)
{
        TRACE("auth_handler_func %x \n", evt->eType);
       if(evt->eType == BTEVENT_AUTHORIZATION_REQ){
            TRACE(" auth_handler_func level 4 \n");
            TRACE("\n you can reject the authoriztion use any info from this evt you wanted");
            TRACE("\n the default handler is accept");    
            evt->p.remDev->secAccessState |= BAS_AUTHORIZE_COMPLETE;
            evt->p.remDev->authorizeState  =  BAS_AUTHORIZED_SERVICE;
            SecProcessAllSecurityTokens(evt->p.remDev);
       }
    return;
}
#endif

