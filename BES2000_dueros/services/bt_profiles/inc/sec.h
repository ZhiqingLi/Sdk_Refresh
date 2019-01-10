/****************************************************************************
 *
 * File:
 *     $Id: sec.h 790 2009-02-10 16:46:01Z hansensv $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 790 $
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     security manager functionality of the Management Entity.
 * 
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of BES.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __SEC_H
#define __SEC_H

#include "me.h"

/*---------------------------------------------------------------------------
 * Security Manager (SEC) layer
 *
 *     The Security Manager provides access to the security features
 *     offered by Bluetooth radio hardware. This includes encryption,
 *     authentication, and pairing services.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

#define BT_MAX_PIN_LEN 16

/*---------------------------------------------------------------------------
 * BtPairingType type
 *
 *     Bluetooth Pairing type. Indicates the action that should be taken during
 *     pairing. A link key is generated as a result of pairing. What happens to
 *     the link key is determined by the setting of BtPairingType.
 */
typedef U8 BtPairingType;

#define BPT_NOT_SAVED        0x00 /* Do not save the link key */
#define BPT_SAVE_NOT_TRUSTED 0x01 /* Save the link key but make not trusted */
#define BPT_SAVE_TRUSTED     0x02 /* Save the link key and make trusted */

/* End of BtPairingType */


/*---------------------------------------------------------------------------
 * BtSecurityLevel type
 *
 * Security on incoming connections.  Only one security level should be
 * selected for an incoming and one for an outgoing connection (OR'd).  The
 * highest security selected will be used.  If authorization of that service 
 * is required, the BSL_AUTHORIZATION_IN or BSL_AUTHORIZATION_OUT bit can be
 * OR'd together with the security level.
 */
typedef U8 BtSecurityLevel;

/* Security on incoming connections */

/* Requirements:  No "Man In The Middle" protection, No encryption 
 * (legacy devices only), and minimal user Interaction.  
 *
 * If both devices are capable of Secure Simple Pairing, this security level
 * will result in "just works" pairing (unauthenticated link key), and an
 * encrypted connection.  If one of the devices is not capable of Secure
 * Simple Pairing, this level of security will result in no link key and no
 * encryption.
 */
#define BSL_SECURITY_L1_IN      0x01 

/* Requirements: No "Man In The Middle" protection, Encryption.
 *
 * If both devices are capable of Secure Simple Pairing, this security level
 * will result in "just works" pairing (unauthenticated link key) and an
 * encrypted connection.  If one of the devices is not capable of Secure
 * Simple Pairing, this level of security will result in a pin code exchange,
 * which will create a combination link key and an unencrypted connection.
 */
#define BSL_SECURITY_L2_IN      0x02 

/* Requirements: "Man In The Middle" protection, Encryption, User Interaction is
 * acceptable.
 *
 * If both devices are capable of Secure Simple Pairing, this security level
 * will result in an authenticated link key and an encrypted connection.  If
 * one of the devices is not capable of Secure Simple Pairing, this level of
 * security will result in a pin code exchange, which will create a
 * combination link key and an encrypted connection.  It is recommended that
 * a 16 digit PIN code be used. The required pin code length can be provided
 * in a BtSecurityRecord.
 */
#define BSL_SECURITY_L3_IN      0x04 

/* Authorization on incoming required. This value can be OR'd with the 
 * security level to indicate that authorization on this service is
 * required.  The authorize handler will be called if it has been
 * registered.
 */
#define BSL_AUTHORIZATION_IN   0x08 

/* Security on outgoing connections */

/* Requirements:  No "Man In The Middle" protection, No encryption 
 * (legacy devices only), and minimal user Interaction.  
 *
 * If both devices are capable of Secure Simple Pairing, this security level
 * will result in "just works" pairing (unauthenticated link key), and an
 * encrypted connection.  If one of the devices is not capable of Secure
 * Simple Pairing, this level of security will result in no link key and no
 * encryption.
 */
#define BSL_SECURITY_L1_OUT     0x10 

/* Requirements: No "Man In The Middle" protection, Encryption.
 *
 * If both devices are capable of Secure Simple Pairing, this security level
 * will result in "just works" pairing (unauthenticated link key) and an
 * encrypted connection.  If one of the devices is not capable of Secure
 * Simple Pairing, this level of security will result in a pin code exchange,
 * which will create a combination link key and an unencrypted connection.
 */
#define BSL_SECURITY_L2_OUT     0x20 

/* Requirements: "Man In The Middle" protection, Encryption, User Interaction is
 * acceptable.
 *
 * If both devices are capable of Secure Simple Pairing, this security level
 * will result in an authenticated link key and an encrypted connection.  If
 * one of the devices is not capable of Secure Simple Pairing, this level of
 * security will result in a pin code exchange, which will create a
 * combination link key and an encrypted connection.  It is recommended that
 * a 16 digit PIN code be used.
 */
#define BSL_SECURITY_L3_OUT    0x40 

/* Authorization on outgoing required. This value can be OR'd with the 
 * security level to indicate that authorization on this service is
 * required.  The authorize handler will be called if it has been
 * registered.
 */
#define BSL_AUTHORIZATION_OUT  0x80 

/* End of BtSecurityLevel */


/*=================================
 * For Backward Compatibility only. 
 * Do not use these definitions.
 */
#define BSL_NO_SECURITY          0
#define BSL_AUTHENTICATION_IN    BSL_SECURITY_L2_IN
#define BSL_AUTHENTICATION_OUT   BSL_SECURITY_L2_OUT
#define BSL_ENCRYPTION_IN        BSL_SECURITY_L3_IN
#define BSL_ENCRYPTION_OUT       BSL_SECURITY_L3_OUT
#define BSL_DEFAULT             (BSL_SECURITY_L2_IN | BSL_AUTHORIZATION_IN | \
                                 BSL_SECURITY_L2_OUT)

/*---------------------------------------------------------------------------
 * BtEncryptMode type
 */
typedef U8 BtEncryptMode;

#define BECM_ENCRYPT_DISABLE 0x00
#define BECM_ENCRYPT_ENABLE  0x01
#define BECM_ENCRYPT_REFRESH 0x02

/* End of BtEncryptMode */

/*---------------------------------------------------------------------------
 * BtSecurityMode type
 */
typedef U8 BtSecurityMode;

#define BSM_SEC_MODE_1       0x01
#define BSM_SEC_MODE_2       0x02
#define BSM_SEC_MODE_3       0x03
#define BSM_SEC_MODE_4       0x04

/* End of BtSecurityMode */

/*=================================
 * For Backward Compatibility only. 
 * Do not use these definitions.
 */
#define BSM_SEC_DISABLED     0x00
#define BSM_SEC_LEVEL_1      0x01
#define BSM_SEC_LEVEL_2      0x02
#define BSM_SEC_LEVEL_3      0x03
#define BSM_SEC_ENCRYPT      0x04

/*---------------------------------------------------------------------------
 * BtSimplePairing
 */
typedef U8 BtSimplePairingMode;

/* The local controller does not support Secure Simple Pairing */
#define BSP_SIMPLE_PAIRING_NOT_SET  0

/* The local controller supports Secure Simple Pairing */
#define BSP_SIMPLE_PAIRING_ENABLED  1
//wangjianjun
/*---------------------------------------------------------------------------
 * BtUserConfirm
 */
typedef U8 BtUserConfirm;

/* The user rejects the numerical value displayed */
#define BUC_NO   0

/* The user accepts the numerical value displayed */
#define BUC_YES  1

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtOobData structure
 *
 *     Out of Band data used for Secure Simple Pairing.
 */
typedef struct _BtOobData
{
    /* Bluetooth address of the remote device from which Out of Band
     * data was received.
     */
    BT_BD_ADDR bdAddr;

    /* Secure Simple Pairing Hash C received from the remote device */
    U8 hashC[16];

    /* Secure Simple Pairing Randomizer R received from the remote device */
    U8 randR[16];

    /* === Internal use only === */ 

    BtPairingType pairingType;

} BtOobData;

/*---------------------------------------------------------------------------
 * BtSecurityRecord structure
 *
 *     Security records are not stored in non-volatile memory.
 */
typedef struct _BtSecurityRecord
{
    ListEntry  node; /* Used internally by the stack */

    /* Group: The user must fill in the four fields below */
    BtProtocolId    id;
    U32_PTR         channel;
    BtSecurityLevel level;

    /* PinLen specifies the size of pin required by the entity registering the
     * record. A value of 0 means the registering entity does not care while
     * a non-0 value indicates the minimum size of the pin. Keys made with
     * pin codes of shorter length are considered not strong enough. This field 
     * is ignored for Secure Simple Pairing since Pins are not used and ignored
     * for BtSecurityLevel less than BSL_SECURITY_L3_IN or BSL_SECURITY_L3_OUT.
     * The maximum value for pinLen is BT_MAX_PIN_LEN.
     */
    U8              pinLen;

} BtSecurityRecord;


/*---------------------------------------------------------------------------
 * BtSecurityParms structure
 *
 *     This structure is used to pass security parameters to register security record.
 */

typedef struct _BtSecurityParms {

	/*Security on incoming connections.*/
	BtSecurityLevel level;

    /* PinLen specifies the size of pin required by the entity registering the
     * record. A value of 0 means the registering entity does not care while
     * a non-0 value indicates the minimum size of the pin. Keys made with
     * pin codes of shorter length are considered not strong enough. This field 
     * is ignored for Secure Simple Pairing since Pins are not used and ignored
     * for BtSecurityLevel less than BSL_SECURITY_L3_IN or BSL_SECURITY_L3_OUT.
     * The maximum value for pinLen is BT_MAX_PIN_LEN.
     */
    U8              pinLen;
} BtSecurityParms;

/*---------------------------------------------------------------------------
 * BtSecurityToken structure
 *
 *     Security tokens are used to request access by protocol
 *     layers and multiplexors.
 */
typedef struct _BtSecurityToken
{
    ListEntry  node;   /* Used internally by the stack */

    /* Group: The following parameters must be set before calling
     * SEC_AccessRequest.
     */
    BtProtocolId    id;      /* Protocol ID of caller */

    /* Pointer to remote device for which the access is being requested */
    BtRemoteDevice *remDev;  

    /* The multiplexing channel number. For example for L2CAP it is the PSM 
     * and for RFCOMM it is the Server ID (incoming only) or a pointer to an 
     * RfChannel structure (outgoing only).
     */
    U32_PTR         channel;

    /* Incoming must be set to TRUE if access is desired for a connection 
     * request from a remote (incoming). It must be set to FALSE if the 
     * request is for a connection to remote device (outgoing).
     */
    BOOL            incoming; 

    /* === Internal use only === */
    
    BtEventType         result;
    BtSecurityRecord   *record;
    BtSecurityLevel     level;
    EvmTimer            timer;
    BOOL                changeLinkKey;
    BtAuthRequirements  authRequirements;
} BtSecurityToken;

#if BT_SECURITY == XA_ENABLED
/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * SEC_RegisterPairingHandler()
 *
 *     Register the handler that will handle all user interactions for
 *     pairing.  This handler is called when user interaction is required. 
 *     When both devices support Secure Simple Pairing, this handler is called
 *     when "Man In The Middle" protection is required, and an
 *     authenticated link key will be generated.  If "Just Works" pairing
 *     occurs no user interaction is required.  When one of the devices does
 *     not support Secure Simple Pairing, this handler will be called
 *     whenever a PIN code exchanges is required.
 *
 *     If no handler is set (handler == 0) then the pairing
 *     procedure will fail. This function is used to set pairable
 *     mode. The old handler is returned when the function is 
 *     called.
 *
 *     The pairing handler is called with the following events:
 *
 *     BTEVENT_PIN_REQ - If "errCode" is BEC_NO_ERROR, this event
 *     indicates a PIN code is needed. The handler provides a PIN
 *     code by calling SEC_SetPin. If "errCode" is NOT BEC_NO_ERROR,
 *     this event indicates a previous PIN code request has
 *     been cancelled.
 *
 *     BTEVENT_PASS_KEY_REQ - If "errCode" is BEC_NO_ERROR, this event
 *     indicates a Passkey  is needed. The handler provides a Passkey by 
 *     calling SEC_SetPassKey. If "errCode" is NOT BEC_NO_ERROR,
 *     this event indicates a previous Passkey request has
 *     been cancelled.
 *
 *     BTEVENT_CONFIRM_NUMERIC_REQ - If "errCode" is BEC_NO_ERROR, this event
 *     indicates that a user confirmation is needed. The handler provides this
 *     confirmation by calling SEC_UserConfirm.  If "errCode" is NOT BEC_NO_ERROR,
 *     this event indicates a previous confirmation request has been cancelled.
 *
 *     BTEVENT_DISPLAY_NUMERIC_IND - If "errCode" is BEC_NO_ERROR, this event
 *     indicates that a numeric value must be displayed.  No confirmation of 
 *     this event is required by the handler.  If "errCode" is NOT BEC_NO_ERROR,
 *     this event indicates a previous request has been cancelled.
 *
 *     BTEVENT_PAIRING_COMPLETE - This event indicates that pairing
 *     has completed successfully and the link is authenticated.
 *     The "errCode" field of the BtEvent indicates the status of
 *     storing the link key. If pairing fails no event will be 
 *     sent (there is no event sent by the radio module indicating 
 *     failure).
 *
 * Parameters:
 *     handler - pointer to the handler for dealing with pairing.
 *         This handler can be a registered global handler but does
 *         not have to be.
 *
 * Returns:
 *     Pointer to the old handler.
 */
BtHandler *SEC_RegisterPairingHandler(BtHandler *handler);

/*---------------------------------------------------------------------------
 * SEC_RegisterAuthorizeHandler()
 *
 *     Register the handler that will handle all authorization.  
 *     This handler is called when authorization is needed.  
 *
 *     If no handler is set (handler == 0) then the authorization
 *     procedure will fail. 
 *
 *     The authorization handler is called with the following events:
 *
 *     BTEVENT_AUTHORIZATION_REQ - If "errCode" is BEC_NO_ERROR, this event
 *     indicates a request for authorization. The link needing authorization
 *     is passed in the "remDev" field of BtEvent. During the callback, the
 *     handler can call SEC_GetSecurityRecord to obtain more information on
 *     what service or channel is being authorized. If "errCode" is NOT
 *     BEC_NO_ERROR, this event indicates a previous authorization request
 *     has been cancelled.
 *
 * Parameters:
 *     handler - pointer to the handler for dealing with 
 *         authorization.  This handler can be a registered global 
 *         handler or even the same handler that does pairing but it
 *         does not have to be.
 *
 * Returns:
 *     Pointer to the old handler.
 */
BtHandler *SEC_RegisterAuthorizeHandler(BtHandler *handler);

/*---------------------------------------------------------------------------
 * SEC_AccessRequest()
 *
 *     Determine if access is allowed for the device specified
 *     by rm at the protocol specified by id for the given
 *     channel. The incoming parameter indicates if the request
 *     is incoming or outgoing.
 *
 * Parameters:
 *     token - pointer to a Security token. The fields of the
 *         token must be filled in appropriately. If BT_STATUS_PENDING
 *         is returned the token cannot be modified until the protocol's
 *         callback is called.
 *     
 * Returns:
 *     BT_STATUS_SUCCESS - Access is approved
 *
 *     BT_STATUS_PENDING - The operation has started. The results
 *         will be returned via the protocol's callback identified by
 *         id. The callback is called with "eventType"
 *         BTEVENT_ACCESS_APPROVED or BTEVENT_ACCESS_DENIED.
 *
 *     BT_STATUS_FAILED - Access is denied.
 *
 *     BT_STATUS_INVALID_PARM - Access is denied. Token is not
 *         properly initialized, token is 0 (XA_ERROR_CHECK only)
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_AccessRequest(BtSecurityToken *token);

/*---------------------------------------------------------------------------
 * SEC_CancelAccessRequest()
 *
 *     SEC_CancelAccessRequest cancels the security procedure associated with
 *     the "token" BtSecurityToken.
 *
 * Parameters:
 *     token - pointer to a Security token with active access request.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - cancel operation successful
 *
 *     BT_STATUS_FAILED - operation failed. The remote device is
 *         not in a state to cancel access request.
 */
BtStatus SEC_CancelAccessRequest(BtSecurityToken *token);

/*---------------------------------------------------------------------------
 * SEC_Register()
 *
 *     Register the security record with the security manager. The security
 *     record contains a pinLen field. If the field is 0 then the caller
 *     does not care the size of the pin code used in created a link key.
 *     If the value is non-0 the pin code used in creating the key must
 *     be at least as long as pinLen. If the value passed to this function
 *     exceeds BT_MAX_PIN_LEN it will be set to BT_MAX_PIN_LEN.
 *
 * Parameters:
 *     sr - A pointer to the security record.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - security record is registered.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the
 *         security record pointer is 0, or the security level is
 *         not properly formed. (error check only).
 *
 *     BT_STATUS_IN_USE - operation failed because the protocol ID and
 *         channel in security record was already registered. 
 *
 *     BT_STATUS_FAILED - operation failed because record is
 *         already on the list (XA_ERROR_CHECK only).
 */
BtStatus SEC_Register(BtSecurityRecord *sr);


/*---------------------------------------------------------------------------
 * SEC_Unregister()
 *
 *     Unregister the security record with the security manager.
 *
 * Parameters:
 *     sr - A pointer to the security record.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - security record is unregistered.
 *
 *     BT_STATUS_FAILED - operation failed because record is not
 *         on the list. 
 */
BtStatus SEC_Unregister(BtSecurityRecord *sr);

/*---------------------------------------------------------------------------
 * SEC_Authorize()
 *
 *     If "isAuthorized" is set to TRUE then temporary authorization 
 *     for the remote device is granted otherwise it is denied. If
 *     the "futureTrust" parameter is set to TRUE then the device
 *     is marked as a trusted device in the device database. The 
 *     trust mark holds beyond this connection. If "futureTrust" is
 *     FALSE then the device database record is not modified.
 *     The authorization handler calls this function after 
 *     receiving a BTEVENT_AUTHORIZATION_REQ when "errCode" is
 *     BEC_NO_ERROR.
 *
 * Parameters:
 *     rm - Pointer to remote device structure.
 *
 *     isAuthorized - TRUE indications authorization is granted.
 *         FALSE indicates authorization is denied.
 *
 *     futureTrust - If set to TRUE the device is marked as trusted
 *         in the device database. If set to FALSE the device database 
 *         record is not modified.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation successful
 *
 *     BT_STATUS_FAILED - operation failed. The remote device is
 *         not in a state to accept authorization.
 *
 *     BT_STATUS_DEVICE_NOT_FOUND - the authorization was successful
 *         but the device data base write failed because the device was
 *         not found in the database or the write operation to the 
 *         database failed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the remote
 *         device pointer is 0 (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_Authorize(BtRemoteDevice *rm, BOOL isAuthorized,
                       BOOL futureTrust);
#define SEC_Authorize(rm, au, tr) (SEC_AuthorizeServiceB(rm, au, tr, FALSE))

/*---------------------------------------------------------------------------
 * SEC_AuthorizeService()
 *
 *     SEC_AuthorizeService is similar to SEC_Authorize, but temporarily
 *     authorizes a device for a specific service or channel. If
 *     "isAuthorized" is set to TRUE then temporary authorization 
 *     for a service is granted to the remote device, otherwise it
 *     is denied.  There is no "futureTrust" parameter as this authorization
 *     is always temporary for a one time only access of the service. 
 *     The authorization handler calls this function after receiving
 *     a BTEVENT_AUTHORIZATION_REQ event when "errCode" is BEC_NO_ERROR.
 *     The event handler can access the security record registered for the
 *     service with the SEC_GetSecurityRecord API.
 *
 * Parameters:
 *     rm - Pointer to remote device structure.
 *
 *     isAuthorized - TRUE indications authorization is granted.
 *         FALSE indicates authorization is denied.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation successful
 *
 *     BT_STATUS_FAILED - operation failed. The remote device is
 *         not in a state to accept authorization.
 *
 *     BT_STATUS_DEVICE_NOT_FOUND - the authorization was successful
 *         but the device data base write failed because the device was
 *         not found in the database or the write operation to the 
 *         database failed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the remote
 *         device pointer is 0 (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_AuthorizeService(BtRemoteDevice *rm, BOOL isAuthorized);
#define SEC_AuthorizeService(rm, au) (SEC_AuthorizeServiceB(rm, au, FALSE, TRUE))

/* Prototype for actual SEC_Authorize and SEC_AuthorizeService implementation */
BtStatus SEC_AuthorizeServiceB(BtRemoteDevice *rm, BOOL isAuthorized,
                               BOOL futureTrust, BOOL authService);

/*---------------------------------------------------------------------------
 * SEC_GetSecurityRecord()
 *
 *     SEC_GetSecurityRecord retrieves the security record associated with
 *     a BTEVENT_AUTHORIZATION_REQ event. The API must be used only during
 *     the BTEVENT_AUTHORIZATION_REQ event. An authorization handler might
 *     use this call to determine what service or channel is being authorized.
 *
 * Parameters:
 *     rm - Pointer to remote device structure.
 *
 * Returns:
 *     pointer to a BtSecurityRecord structure
 */
BtSecurityRecord *SEC_GetSecurityRecord(BtRemoteDevice *rm);
#define SEC_GetSecurityRecord(rm) ((BtSecurityRecord*)(rm)->secRec)

/*---------------------------------------------------------------------------
 * SEC_UserConfirm()
 *
 *     Provide a user confirmation for the pairing process.  A user 
 *     confirmation is required when "Man In The Middle" protection is 
 *     required.  The IO capabilities of the local device must be
 *     IO_DISPLAY_YESNO to receive a request for user confirmation.
 *
 * Parameters:
 *
 *     remDev - Pointer to remote device structure
 *
 *     confirm - The confirmation action.
 *
 *     type - indicates how the link key should be handled.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation is started successful. The
 *         pairing handler will be called with the BTEVENT_PAIRING_COMPLETE
 *         event when pairing is complete.
 *
 *     BT_STATUS_FAILED - operation failed because the remote
 *         device structure is not in a state where a pin is needed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the remote
 *         device structure is invalid or the pin is not set to 0 but
 *         the len is 0 or greater than 16.
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_UserConfirm(BtRemoteDevice *remDev, BtUserConfirm confirm,
                         BtPairingType type);

/*---------------------------------------------------------------------------
 * SEC_SetPassKey()
 *
 *     Provide a passkey for the pairing process.  A passkey is required when
 *     "Man In The Middle" protection is required.  The IO capabilities of the 
 *     device must be IO_KEYBOARD_ONLY to receive a request for a passkey.
 *
 * Parameters:
 *
 *     remDev - Pointer to remote device structure
 *
 *     passKey - The Passkey.
 *
 *     len - number of bytes in the Passkey (should not exceed 6 digits).
 *
 *     type - indicates how the link key should be handled.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation is started successful. The
 *         pairing handler will be called with the BTEVENT_PAIRING_COMPLETE
 *         event when pairing is complete.
 *
 *     BT_STATUS_FAILED - operation failed because the remote
 *         device structure is not in a state where a pin is needed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the remote
 *         device structure is invalid or the pin is not set to 0 but
 *         the len is 0 or greater than 16.
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_SetPassKey(BtRemoteDevice *remDev, const U8 *passKey, U8 len,
                        BtPairingType type);

/*---------------------------------------------------------------------------
 * SEC_SetPin()
 *
 *     Set the pin value for the device specified by rm. This
 *     function is called as a response to the BTEVENT_PIN_REQ
 *     event when "errCode" is BEC_NO_ERROR. Setting pin to 0
 *     rejects the pairing operation. If p.pinReq.pinLen for the 
 *     BTEVENT_PIN_REQ event is greater than 0 then len must be equal to or
 *     greater than p.pinReq.pinLen.
 *     
 * Parameters:
 *     rm - Pointer to remote device structure
 *
 *     pin - pointer to pin. Pin set to 0 indicates that
 *         the pairing operation should be rejected. 
 *
 *     len - number of bytes in the pin.
 *
 *     type - indicates how the link key should be handled.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation is started successful. The
 *         pairing handler will be called with the BTEVENT_PAIRING_COMPLETE
 *         event when pairing is complete.
 *
 *     BT_STATUS_FAILED - operation failed because the remote
 *         device structure is not in a state where a pin is needed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the remote
 *         device structure is invalid or the pin is not set to 0 but
 *         the len is 0 or greater than 16 or the BTEVENT_PIN_REQ indicated
 *         a specific pin length was required and the len is smaller
 *         that the required length.
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_SetPin(BtRemoteDevice *rm, const U8* pin, U8 len, BtPairingType type);

/*---------------------------------------------------------------------------
 * SEC_OobDataReceived()
 *
 *     Provide Out of Band data received from a remote device.  When Out of
 *     Band data has been received from the remote device, it should be provided
 *     to the stack using this function call.  During the Secure Simple
 *     Pairing process, this data will be provided to the host controller for
 *     the purpose of Secure Simple Pairing.  To clear the oobData, this
 *     function can be called again with a BD_ADDR set to all 0's.
 * 
 * Requires:
 *     Secure Simple Pairing support in the host controller.
 * 
 * Parameters:
 *
 *     OobData - Pointer to a structure containing Out of Band Data received
 *         from the remote device.
 *
 *     type - indicates how the link key should be handled.
 * 
 * Returns:
 *
 *     BT_STATUS_SUCCESS - Out of Band data has been registered with the stack.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the remote
 *         device structure is invalid or the OobData pointer is 0l
 */
BtStatus SEC_OobDataReceived(BtOobData *oobData, BtPairingType type);

/*---------------------------------------------------------------------------
 * SEC_GetLocalOobData()
 *
 *     Retrieves Out of Band data from the local host controller.  This data
 *     should be transmitted to the remote device using an Out of Band mechanism.
 *     The data is used during Simple Pairing.
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller.
 * 
 * Parameters:
 *
 *     none
 *
 * Returns: 
 *
 *     BT_STATUS_PENDING - operation is started successful. The
 *         pairing handler will be called with the BTEVENT_LOCAL_OOB_DATA
 *         event when the out of band data has been read from the 
 *         host controller.
 *
 *     BT_STATUS_BUSY - operation failed because another operation is already
 *         active.  Try again in a little while.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_GetLocalOobData(void);

/*---------------------------------------------------------------------------
 * SEC_SetAuthRequirements
 *
 *     Set the default authentication requirements to be used during Secure 
 *     Simple pairing. If MITM_PROTECT_REQUIRED is set then it will used
 *     regardless of the values in a security record. This setting is ignored 
 *     if Secure Simple Pairing is not supported by the Local Controller. If 
 *     the remote device does not support simple pairing then 
 *     MITM_PROTECT_REQUIRED specifies that authentication and encryption 
 *     (BSL_SECURITY_L3_IN | BSL_SECURITY_L3_OUT) will be used if there is 
 *     no security record.
 *
 * Requires:
 *     Secure Simple Pairing support in the local controller.
 * 
 * Parameters:
 * 
 *     authReq - The new default authentication requirements.
 *
 * Returns:
 *
 *     The previous authentication requirements.
 */
BtAuthRequirements SEC_SetAuthRequirements(BtAuthRequirements authReq);


/*---------------------------------------------------------------------------
 * SEC_GetAuthRequirements
 *
 *     Get the current default authentication requirements.
 *
 * Returns:
 *
 *     The current authentication requirements.
 */
BtAuthRequirements SEC_GetAuthRequirements(void);

/*---------------------------------------------------------------------------
 * SEC_SetAuthorizeRequirements
 *
 *     Set the default authorization requirements.  If no security record
 *     exists for a particular connection, then the provided authorization
 *     requirements will be used. 
 *
 * Requires:
 *     Secure Simple Pairing support in the local controller.
 * 
 * Parameters:
 * 
 *     authorizeReq - The new default authorization requirements.
 *
 * Returns:
 *
 *     The previous authorization requirements.
 */
BtAuthorizeRequirements SEC_SetAuthorizeRequirements(BtAuthorizeRequirements authorizeReq);

/*---------------------------------------------------------------------------
 * SEC_GetAuthorizeRequirements
 *
 *     Get the current default authorization requirements.
 *
 * Returns:
 *
 *     The current authentication requirements.
 */
BtAuthorizeRequirements SEC_GetAuthorizeRequirements(void);

/*---------------------------------------------------------------------------
 * SEC_AuthenticateLink()
 *
 *     Authenticate an existing link with a remote device. If
 *     a link key does not exist then pairing will be attempted.
 *     Pairing events will be sent to the Pairing handler. If
 *     a pairing handler does not exist then pairing will fail 
 *     which in turn will cause authentication to fail. 
 *     BTEVENT_AUTHENTICATE_CNF will be sent to the handler provided.
 *     The "errCode" field of the BtEvent specifies if the operation 
 *     was successful or not.
 *
 *     If the operation is successful then BTEVENT_AUTHENTICATED will
 *     be sent to all registered global handlers and all handlers
 *     bound to the remote device structure. Note that this event does not
 *     occur if the remote device initiated an authentication operation
 *     because authentication is a one-way operation.
 *
 *     The link must be active (not in hold, sniff, park or in the
 *     process of being disconnected). Also it must not already
 *     be authenticated.
 *
 * Parameters:
 *     handler - handler to receive authenticate results. This can
 *         be the registered global handler or a handler bound to
 *         a remote device or 0. Failures are sent to this handler
 *         only. The handler can be 0 if desired, though no handler
 *         will receive failure notifications in this case.
 *
 *     remDev - pointer to remote device structure. 
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *
 *     BT_STATUS_FAILED - the operation failed because the link
 *         is not in a state to perform authentication or is already
 *         authenticated.
 *
 *     BT_STATUS_INVALID_PARM - the operation failed because
 *         the "remDev" is invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_AuthenticateLink(BtHandler *handler, BtRemoteDevice *remDev);


/*---------------------------------------------------------------------------
 * SEC_SetSimplePairingDebugMode()
 *
 *     Enables or disables simple pairing debug mode. When enabled, the
 *     controller uses a fixed private/public key pair which permits
 *     over-the-air sniffers to monitor encrypted traffic.
 *
 * Parameters:
 *     enable - TRUE to enable debug mode pairing. FALSE to restore
 *         ordinary simple pairing security link keys.
 * Returns:
 *     BT_STATUS_SUCCESS - the operation started successfully. No event
 *         will be generated.
 *
 *     BT_STATUS_IN_PROGRESS - an attempt to set the simple pairing
 *         debug mode is already in progress.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_SetSimplePairingDebugMode(BOOL enable);

/*---------------------------------------------------------------------------
 * SEC_SetLinkEncryption()
 *
 *     Enable, disable, or refresh (v2.1 controllers only) link level
 *     encryption for the give link. If the function returns
 *     BT_STATUS_PENDING then BTEVENT_ENCRYPT_COMPLETE will be sent to the
 *     handler. The "errCode" field will indicate the result and the
 *     "p.remDev" field will indicate the remote device.
 * 
 *     The operation is successful then all handlers bound to the
 *     remote device and all registered global handlers will receive
 *     BTEVENT_ENCRYPTION_CHANGE event. The "p.encrypt.remDev" field
 *     indications the link and the "p.encrypt.mode" field indicates the new
 *     encryption mode.
 *      
 *     On a v2.1 controller, setting the link encryption on a link that is
 *     already encrypted will refresh the link and result in a pause and
 *     resume operation, and this function will return BT_STATUS_PENDING. 
 *     When the operation is complete BTEVENT_ENCRYPT_COMPLETE will be 
 *     indicated to the application.  The BTEVENT_ENCRYPTION_CHANGE event
 *     will also be indicated with the mode set to BECM_ENCRYPT_REFRESH.  On
 *     a 2.0 or previous controller, setting the link encryption on a link
 *     that is already encrypted will result an a BT_STATUS_SUCCES return
 *     code.
 *
 *     The link must be active (not in hold, sniff, park or in the
 *     process of being disconnected). Also it must not already
 *     be in the process of performing an encryption operation.
 *
 * Parameters:
 *     handler - handler to receive encryption results. This can
 *         be the registered global handler or a handler bound to
 *         a remote device or 0. Failures are sent to this handler
 *         only. The handler can be 0 if desired, though no handler
 *         will receive failure notifications in this case.
 *
 *     remDev - pointer to remote device structure. 
 *
 *     mode - desired encryption mode (enabled or disabled)
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation was started successfully.
 *         When the operation is complete the handler will be
 *         called with the BTEVENT_ENCRYPT_COMPLETE event.
 *         A BTEVENT_HCI_FATAL_ERROR event indicates a fatal radio
 *         or HCI transport error and all pending operations have failed.
 *
 *     BT_STATUS_SUCCESS - the link is already in the desired mode.
 *         No events will be generated.
 *
 *     BT_STATUS_FAILED - the operation failed because the link
 *         is not in a state to perform encryption. The link must
 *         be authenticated before encryption is allowed.
 *
 *     BT_STATUS_INVALID_PARM - the operation failed because
 *         the handler, "remDev" or mode are invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_SetLinkEncryption(BtHandler *handler, BtRemoteDevice *remDev,
                               BtEncryptMode mode);

/*---------------------------------------------------------------------------
 * SEC_EnableSecurityMode3()
 *
 *     If Secure Simple Pairing is not enabled (i.e. - the local or remote
 *     device does not support Secure Simple Pairing), then enable security
 *     mode 3. That means all connections (incoming and outgoing) must be
 *     authenticated. If any client sets security mode 3 then security mode 3
 *     is in force for all. The security Mode 2 security records are still
 *     checked and authorization may still be required. Existing connections
 *     are not affected.
 *
 *     If the function returns BT_STATUS_PENDING then the handler
 *     is called with BTEVENT_SECURITY3_COMPLETE when the operation
 *     is finished. The "errCode" field indicates the status of the
 *     operation and the "p.secMode.mode" field indicates the new 
 *     security mode. Success of the operation is based on whether
 *     authentication is set or not.
 *
 *     If the operation is successful (as defined above) then all 
 *     registered global handlers will receive BTEVENT_SECURITY_CHANGE 
 *     event. The same fields as used above provide the results of
 *     the change.
 *
 * Parameters:
 *     handler - A pointer to a handler. It can be a registered
 *         handler, the pairing handler, or the authorization handler
 *         but doesn't have to be. Failures are sent only to this
 *         handler.
 *
 *     encryption - If set to TRUE then encryption is required.
 *         Otherwise encryption is not required.  This parameter is ignored
 *         if the local host controller supports v2.1 of the Bluetooth
 *         Specification.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation has started successfully.
 *         When the operation is complete the handler will be
 *         called with the BTEVENT_SECURITY3_COMPLETE event.
 *         A BTEVENT_HCI_FATAL_ERROR event indicates a fatal radio
 *         or HCI transport error and all pending operations have failed.
 *
 *     BT_STATUS_SUCCESS - the operation is successful. The
 *         security mode was already set to 3. No events will be
 *         generated.
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_INVALID_PARM - the operation failed because
 *         the handler is 0 (XA_ERROR_CHECK only)
 *
 *     BT_STATUS_FAILED - operations failed because change of
 *         security mode is already in progress.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_EnableSecurityMode3(BtHandler *handler, BOOL encryption);


/*---------------------------------------------------------------------------
 * SEC_DisableSecurityMode3()
 *
 *     Disable security mode 3. The Security Manager now operates
 *     in security mode 2. Security mode 2 and 3 are only used for
 *     connections to devices that do not support Secure Simple Pairing.
 *     If Secure Simple Pairing is supported, the device is always in
 *     Security Mode 4.  Disabling security mode 3 simply disables this
 *     feature for legacy devices (v2.0 and early)
 *
 *     If the function returns BT_STATUS_PENDING then the handler
 *     is called with BTEVENT_SECURITY3_COMPLETE when the operation
 *     is finished. The "errCode" field indicates the status of the
 *     operation and the "p.secMode.mode" field indicates the new 
 *     security mode.  If the operation is successful then all 
 *     registered global handlers will receive BTEVENT_SECURITY_CHANGE event. 
 *
 * Parameters:
 *     handler - A pointer to a handler.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation has started successfully.
 *         When the operation is complete the handler will be
 *         called with the BTEVENT_SECURITY3_COMPLETE event.
 *         A BTEVENT_HCI_FATAL_ERROR event indicates a fatal radio
 *         or HCI transport error and all pending operations have failed.
 *
 *     BT_STATUS_SUCCESS - the operation is successful. The
 *         security mode was already set to 3. No events will be
 *         generated.
 *
 *     BT_STATUS_NO_RESOURCES - operation failed because there are
 *         too many security operations on the queue. Try again
 *         in a little while.
 *
 *     BT_STATUS_INVALID_PARM - the operation failed because
 *         the handler is 0 (XA_ERROR_CHECK only)
 *
 *     BT_STATUS_FAILED - operations failed because change of
 *         security mode is already in progress.
 *
 *     BT_STATUS_HCI_INIT_ERR - operation failed because the HCI has
 *         an initialization error. Monitor the global events to
 *         be notified when the error has been corrected.
 */
BtStatus SEC_DisableSecurityMode3(BtHandler *handler);

/*---------------------------------------------------------------------------
 * SEC_GetSimplePairingMode()
 *
 *    Get the Secure Simple Pairing mode.  If the local host controller is
 *    capable of Secure Simple Pairing, this value will be set to
 *    BSP_SIMPLE_PAIRING_ENABLED.
 *
 * Returns:
 *    BSP_SIMPLE_PAIRING_NOT_SET - The device does not support Secure Simple
 *        Pairing.
 *
 *    BSP_SIMPLE_PAIRING_ENABLED - The device supports Secure Simple Pairing.
 */
BtSimplePairingMode SEC_GetSimplePairingMode(void);

/*---------------------------------------------------------------------------
 * SEC_SetBondingMode()
 *
 *     Enable the current bonding mode.  DEDICATED_BONDING should be used
 *     when the intent is to connect to a device with the explicit purpose of
 *     bonding, and then disconnect.  Afterward, it should be disabled.
 *     GENERAL_BONDING should be enabled if the intent is to bond while
 *     connecting to a service. Bonding can be disabled by setting the mode
 *     to BONDING_NOT_ALLOWED.  If Secure Simple Pairing is not supported,
 *     this value is ignored. 
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller.
 * 
 * Parameters:
 *     mode - Should be set to the desired bonding mode.
 *
 * Returns:
 *     The previous bonding mode.
 */
BtBondingMode SEC_SetBondingMode(BtBondingMode mode);

/*---------------------------------------------------------------------------
 * SEC_GetBondingMode()
 *
 *     Get the current Bonding mode.
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller.
 * 
 * Parameters:
 *     none
 *
 * Returns:
 *     The current bonding mode.
  */
BtBondingMode SEC_GetBondingMode(void);

/*---------------------------------------------------------------------------
 * SEC_SetIoCapabilities()
 *
 *     Set the IO capabilities of this device.  The IO capabilities determine
 *     how Secure Simple Pairing is performed.  Use the IO capabilities that
 *     best describe the capabilities of the device.  If Secure Simple
 *     Pairing is not supported, this value is ignored.
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller.
 * 
 * Parameters:
 *     ioCap - The IO capabilities of the device.
 *
 * Returns:
 *     The previous IO capabilities.
 */
BtIoCapabilities SEC_SetIoCapabilities(BtIoCapabilities ioCap);

/*---------------------------------------------------------------------------
 * SEC_GetIoCapabilities()
 *
 *     Get the current IO capabilities of this device.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     The current IO capabilities. 
 */
BtIoCapabilities SEC_GetIoCapabilities(void);

/*---------------------------------------------------------------------------
 * SEC_GetRemoteIoCapabilities()
 *
 *     Get the current IO capabilities of the remote device.  Not valid if
 *     Secure Simple Pairing is not supported.
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller.
 * 
 * Parameters:
 *     none
 *
 * Returns:
 *     The remote IO capabilities. 
 */
BtIoCapabilities SEC_GetRemoteIoCapabilities(BtRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * SEC_GetRemoteAuthCapabilities()
 *
 *     Get the Authentication Requirements of the remote device.  Not valid
 *     if Secure Simple Pairing is not supported.
 *
 * Requires:
 *     Secure Simple Pairing support in the host controller.
 * 
 * Parameters:
 *     none
 *
 * Returns:
 *     The remote authentication requirements.
 */
BtAuthRequirements SEC_GetRemoteAuthRequirements(BtRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * SEC_DeleteDeviceRecord()
 *
 *     Delete a record with the given "bdAddr" from the device
 *     database. 
 *
 * Parameters:
 *     bdAddr - pointer to 48-bit address of device record to delete.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation is successful.
 *
 *     BT_STATUS_FAILED - operation failed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because bdAddr
 *         is invalid (XA_ERROR_CHECK only)
 */
BtStatus SEC_DeleteDeviceRecord(const BT_BD_ADDR *bdAddr);


/*---------------------------------------------------------------------------
 * SEC_AddDeviceRecord()
 *
 *     Add a record to the device database. A pointer to a
 *     record is passed to the function . The contents of the
 *     record are copied into the device database. If a record
 *     with the same BD_ADDR already exists then the existing
 *     record is changed. This function can be used to
 *     preload the device database.
 *
 * Parameters:
 *     record - pointer to record which is to be copied into the
 *     database.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - record written successfully.
 *
 *     BT_STATUS_FAILED - record was not written.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because record
 *         is invalid (XA_ERROR_CHECK only)
 */
BtStatus SEC_AddDeviceRecord(const BtDeviceRecord *record);


/*---------------------------------------------------------------------------
 * SEC_FindDeviceRecord()
 *
 *     Find the device record with the given bdAddr. The record
 *     is copied to the location pointed to by the record parameter.
 *
 * Parameters:
 *     bdAddr - pointer to 48-bit address of device record to find.
 *
 *     record - pointer to location to receive record.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation is successful.
 *
 *     BT_STATUS_FAILED - operation failed.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because bdAddr
 *         or record is invalid (XA_ERROR_CHECK only)
 */
BtStatus SEC_FindDeviceRecord(const BT_BD_ADDR *bdAddr, BtDeviceRecord *record);


/*---------------------------------------------------------------------------
 * SEC_EnumDeviceRecords()
 *
 *     Enumerate the device database. The first record is at index 0. The
 *     intent of this function is to enumerate all records in the device
 *     database in a loop. If records are added or deleted in between calls
 *     to this function then all records may not be returned or duplicate
 *     records could be returned.
 *
 * Parameters:
 *     i - index of desired record. The first record is at index 0.
 *
 *     record - pointer to location to receive record.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation is successful. 
 *         Record is returned
 *
 *     BT_STATUS_FAILED - operation failed. No record is
 *         returned. This occurs if the index is greater than
 *         the number of records or an error occurred in reading
 *         the database.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because record
 *         is invalid (XA_ERROR_CHECK only).
 */
BtStatus SEC_EnumDeviceRecords(I16 i, BtDeviceRecord *record);
#endif /* BT_SECURITY */

#endif /* __SEC_H */
