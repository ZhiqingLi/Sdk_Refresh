#ifndef HFP_EVENT_CODE_H
#define HFP_EVENT_CODE_H

/*---------------------------------------------------------------------------*/
/*                              AT Command /Response Events to APP           */
/*---------------------------------------------------------------------------*/
#pragma __PRQA_IGNORE_START__
enum HFP_APP_EventCodeEnum
{/* HFP_SetState Callback to Application Event Code */

    /* AG APP, HF Request AG to do the action, only use in AG side app */
    HFP_EV_CURRENT_CALLS_REQ = 32, /* 32 AT+CLCC, query the list of current calls in AG. */
    HFP_EV_NETWORK_OPERATOR_FORMAT_REQ, /* 33 AT+COPS=3,0, indicate app the network operator name should be set to long alphanumeric */
    HFP_EV_NETWORK_OPERATOR_REQ, /* 34 AT+COPS?, requests AG to respond with +COPS response indicating the currently selected operator */
    HFP_EV_SUBSCRIBER_NUMBER_REQ, /* 35 AT+CNUM, query the AG subscriber number information. */
    HFP_EV_VOICETAG_PHONE_NUM_REQ, /* 36 AT+BINP, requests AG to input a phone number for the voice-tag at the HF */
    HFP_EV_CUR_INDICATOR_VAL_REQ, /* 37 AT+CIND?, get the current indicator during the service level connection initialization procedure */
    HFP_EV_HF_DIAL_REQ, /* 38 ATD, instructs AG to dial the specific phone number */
    HFP_EV_HF_MEM_DIAL_REQ, /* 39 ATD>, instructs AG to dial the phone number indexed by the specific memory location of SIM card */
    HFP_EV_HF_LASTNUM_REDIAL_REQ, /* 40 AT+BLDN, instructs AG to redial the last dialed phone number */
    HFP_EV_MANUFACTURER_REQ, /* 41 AT+CGMI, requests AG to respond with the Manufacturer ID */
    HFP_EV_MODEL_REQ, /* 42 AT+CGMM, requests AG to respond with the Model ID */
    HFP_EV_NREC_DISABLE_REQ, /* 43 AT+NREC=0, requests AG to disable NREC function */
    HFP_EV_DTMF_REQ, /* 44 AT+VTS, instructs AG to transmit the specific DTMF code */
    HFP_EV_ANSWER_CALL_REQ, /* 45 Inform AG app to answer the call, param is call type, incoming or BTRH hold */
    HFP_EV_CANCEL_CALL_REQ, /* 46 Inform AG app to cancel the call, param is call type, outgoing or incoming or ongoing */
    HFP_EV_HOLD_CALL_REQ, /* 47 Inform AG app to hold the incoming call (AT+BTRH=0) */

    /* AG 3-Way Calling */
    HFP_EV_REJECTWAITINGCALL_REQ, /* 48 AT+CHLD=0, Release all held calls or reject waiting call. */
    HFP_EV_ACCEPTWAITING_RELEASEACTIVECALL_REQ, /* 49 AT+CHLD=1, Accept the held or waiting call and release all avtive calls. */
    HFP_EV_HOLDACTIVECALL_REQ, /* 50 AT+CHLD=2, Held Specified Active Call. */
    HFP_EV_ADDONE_HELDCALL2ACTIVECALL_REQ, /* 51 AT+CHLD=3, Add One CHLD Held Call to Active Call. */
    HFP_EV_LEAVE3WAYCALLING_REQ, /* 52 AT+CHLD=4, Leave The 3-Way Calling. */

    HFP_EV_BTRH_QUERY_REQ = 67 /* 67 HF query the status of the "Response and Hold" state of the AG (AT+BTRH?). UINT8* to return the current state: 0 - No incoming call on hold; 1 - The incoming call is on hold; Other values to respond later with HFP_AG_SetCurIndicatorVal */
};
#pragma __PRQA_IGNORE_END__

#endif
