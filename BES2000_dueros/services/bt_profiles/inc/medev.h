/****************************************************************************
 *
 * File:
 *     $Id: medev.h 1346 2009-04-14 18:34:05Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 1346 $
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the Bluetooth
 *     Device Selection functionality of the Management Entity.
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

#ifndef __MEDEV_H
#define __MEDEV_H

#include "me.h"
#include "sdp.h"

/*---------------------------------------------------------------------------
 * Device Selection (DS) layer
 *
 *     The Bluetooth Device Selector API is a part of the
 *     Management Entity. It provides the functionality necessary to
 *     keep track of all devices within the operating range
 *     of the local radio (the "known device list").
 * 
 *     The known device list is a dynamic list of devices found during
 *     inquiry and from the security database. Information about devices
 *     in the known device list is found in BtDeviceContext structures which
 *     are accessible via the DS_ APIs. The Device Selector API also provides
 *     functions for enumerating the list of known devices, and requesting
 *     selection of the most appropriate device via an external "device
 *     selection monitor".
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtDeviceStatus type
 *
 *     The Bluetooth Device Status tracks the status of each device.
 */
typedef U16 BtDeviceStatus;

#define BDS_IN_RANGE        0x01
#define BDS_PAIRED          0x02
#define BDS_TRUSTED         0x04

/* End of BtDeviceStatus */


/*---------------------------------------------------------------------------
 * BtDeviceQualityMask type
 *
 *     The Bluetooth Device Quality Mask is a set of flags used with the
 *     BtDeviceQuality structure to indicate which fields in the structure
 *     have been set.
 */
typedef U8 BtDeviceQualityMask;

#define BDQM_NONE                0x00
#define BDQM_SERVICE_CLASS       0x01
#define BDQM_DEVICE_CLASS        0x02
#define BDQM_SERVICE_UUID        0x04
#define BDQM_DEVICE_STATUS       0x08

/* End of BtDeviceQualityMask */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtDeviceContext structure
 *
 *      This Bluetooth Device context structure is used to keep track of
 *      each known device. The structure also contains information about
 *      the device that is used when determining the best device to connect
 *      to and what parameters to use when establishing an ACL connection.
 *
 *      All fields in this structure are public but should be treated as
 *      read-only! The single exception is the "context" field which can be
 *      modified by the device selection manager.
 */
typedef struct _BtDeviceContext {
    ListEntry           node;       /* Internal, do not use.*/

    /* Group: The following fields are valid for all known devices */

    /* The status of this device. */
    BtDeviceStatus      state;

    /* The 48-bit Bluetooth device address of this device. */
    BT_BD_ADDR             addr;

    /* The Page Scan Information received during the most recent inquiry
     * result from this device.
     */
    BtPageScanInfo      psi;

    /* The Class of Device setting received during the most recent inquiry
     * result or connection request from this device.
     */
    BtClassOfDevice     classOfDevice;

#if DS_NUM_SERVICES >  0
    /* The list of Service Class UUIDs known to exist on this device. */
    SdpServiceClassUuid services[DS_NUM_SERVICES];
#endif

    /* If there is an active ACL connection to this device, this field 
     * contains a pointer to the matching remote device structure.
     */
    BtRemoteDevice     *link;

    /* Reserved for use by the device selection monitor handler to
     * store extended information about this device. The ME
     * does not access or modify this field. This is the only field in
     * this structure that may be modified outside of the ME.
     */
    void               *context;

    /* === Internal Use Only === */
    ListEntry           active;

} BtDeviceContext;


/*---------------------------------------------------------------------------
 * BtDeviceQuality structure
 *
 *     The Bluetooth Device Quality structure is used to provide a set of 
 *     criteria for selecting a matching device. This structure is used with
 *     the DS_SelectDevice and DS_EnumDevicesByQuality functions. This
 *     structure is used to score each device; with the device receiving the
 *     highest score best matches the criteria.
 */
typedef struct _BtDeviceQuality {
    /* Bitwise OR'd flags that indicate which fields of the structure
     * have been supplied.
     */
    BtDeviceQualityMask  mask;

    /* Bluetooth Service Class list. Multiple class' can be OR'd together. 
     * If at least one service class is found in both the query and the 
     * device, 2 points are scored.
     */
    BtClassOfDevice      serviceClass;

    /* Bluetooth Major Device Class. Only one can be provided, minor class of
     * device is ignored. If the class of device matches, 1 point is scored.
     */
    BtClassOfDevice      deviceClass;

    /* The preferred status of the device. For each matching flag in the 
     * device status, 1 point is scored, for a maximum of 3 points.
     */
    BtDeviceStatus       status;

    /* The SDP Service Class UUID to search for in the known services list.
     * If the service is found, 4 points are scored.
     */
    SdpServiceClassUuid  serviceUuid;

} BtDeviceQuality;


/*---------------------------------------------------------------------------
 * BtSelectDeviceToken structure
 *
 *     The Select Device Token is used when requesting the Bluetooth
 *     device selection manager to select an appropriate device based
 *     on the device quality criteria.
 */
typedef struct _BtSelectDeviceToken {
    ListEntry           node;    /* === Internal Use Only === */

    /* The caller must set the callback function. It is called
     * when the command is complete.
     */
    BtCallBack          callback;

    /* The device quality criteria to use when selecting a device. The
     * caller must set this.
     */
    BtDeviceQuality     quality;

    /* The result field is available when the request is complete. If the
     * errCode of the request indicated success, a pointer to the selected
     * device is provided here.
     */
    BtDeviceContext    *result;

    /* === Internal Use Only === */
    U8                  state;

} BtSelectDeviceToken;


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * DS_AddDevice()
 *
 *     Add a device to the dynamic list of known devices. A successful call
 *     to this function will generate a BTEVENT_DEVICE_ADDED event to the
 *     device selection monitor handler.
 *
 * Parameters:
 *     tmplt - A BtDeviceContext structure which provides all the known
 *         information about the device to add. Unknown fields must be
 *         initialized to zero. At a minimum, the "addr" field must be set.
 *
 *     newDevice - A pointer to a BtDeviceContext pointer that receives the
 *         newly added device.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The device has been added to the list and a
 *         reference returned in "newDevice".
 *
 *     BT_STATUS_FAILED - The device is already in the list.
 *
 *     BT_STATUS_NO_RESOURCES - The device list is full.
 *
 *     BT_STATUS_INVALID_PARM - The template parameter was invalid.
 */
BtStatus DS_AddDevice(const BtDeviceContext *tmplt, BtDeviceContext **newDevice);


/*---------------------------------------------------------------------------
 * DS_DeleteDevice()
 *
 *     Remove a device from the known device list. If the device is also in
 *     the security database, this function will not remove it from the
 *     security database. A successful call to this function will generate
 *     a BTEVENT_DEVICE_DELETED event to the device selection monitor handler.
 *
 * Parameters:
 *     addr - The 48-bit Bluetooth address of the device to remove.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The device was found and removed.
 *
 *     BT_STATUS_NOT_FOUND - The device was not found.
 *
 *     BT_STATUS_INVALID_PARM - The "addr" parameter was invalid.
 */
BtStatus DS_DeleteDevice(const BT_BD_ADDR *addr);


/*---------------------------------------------------------------------------
 * DS_EnumDeviceList()
 *
 *     Enumerates all known devices.
 *
 * Parameters:
 *     lastDevice - A pointer to a BtDeviceContext pointer. The first time
 *         this function is called, "lastDevice" points to NULL. Upon return,
 *         "lastDevice" will point to the first element in the list of
 *         known devices.
 *
 *         On subsequent calls, "lastDevice" should point to the element
 *         provided by the prior DS_EnumDeviceList call.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The next device has been stored in "lastDevice".
 *
 *     BT_STATUS_FAILED - The device pointed to by lastDevice is not on the
 *         device list.
 *
 *     BT_STATUS_NOT_FOUND - There are no more devices on the list.
 *
 *     BT_STATUS_INVALID_PARM - The lastDevice parameter was invalid.
 */
BtStatus DS_EnumDeviceList(BtDeviceContext **lastDevice);


/*---------------------------------------------------------------------------
 * DS_EnumDevicesByQuality()
 *
 *     Enumerates all known devices in sorted order based on the device
 *     quality setting. The device that best matches the device quality is
 *     returned first, followed by the next best match, and ending with the
 *     least matching device.
 *
 * Parameters:
 *     lastDevice - Similar to DS_EnumDeviceList.
 *
 *     quality - A pointer to a device quality structure that defines the
 *         attributes of the preferred device.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The next device has been returned.
 *
 *     BT_STATUS_FAILED - The device pointed to by lastDevice is not on the
 *         device list.
 *
 *     BT_STATUS_NOT_FOUND - There are no more devices on the list.
 *
 *     BT_STATUS_INVALID_PARM - The lastDevice or Quality parameters were
 *         invalid.
 */
BtStatus DS_EnumDevicesByQuality(BtDeviceContext **lastDevice, 
                                 const BtDeviceQuality *quality);


/*---------------------------------------------------------------------------
 * DS_FindDevice()
 *
 *     Finds the device in the device list with a matching Bluetooth address.
 *
 * Parameters:
 *     addr - The 48-bit Bluetooth address of the device to find.
 *
 * Returns:
 *     A pointer to the matching device, or zero if no device was found.
 */
BtDeviceContext *DS_FindDevice(const BT_BD_ADDR *addr);


/*---------------------------------------------------------------------------
 * DS_RegisterMonitorHandler()
 *
 *     Registers a handler for receiving device selection monitor events from
 *     the device manager. This handler is required to process select device
 *     events. In addition, the non-maskable HCI status events are indicated 
 *     to the monitor handler. The handler is required to complete any select
 *     device requests that it is holding during a BTEVENT_HCI_FATAL_ERROR or
 *     BTEVENT_HCI_FAILED event indication. If this handler is also a global
 *     handler it will not receive duplicate non-maskable event indications.
 *
 * Parameters:
 *     handler - BtHandler to register. This can be a registered global
 *         handler but does not have to be.
 *
 * Returns:
 *     The previously registered handler, or zero if none was registered.
 */
BtHandler *DS_RegisterMonitorHandler(BtHandler *handler);


/*---------------------------------------------------------------------------
 * DS_SelectDevice()
 *
 *     Select a device which best matches the device quality criteria. If
 *     registered, the Bluetooth device selection monitor is responsible
 *     for selecting the final result.
 *
 * Parameters:
 *     token - An initialized select device token that describes the request.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The function has completed successfully, the 
 *         selected device is returned in token-> result.
 *
 *     BT_STATUS_PENDING - The request was accepted, completion will be 
 *         indicated by a BTEVENT_DEVICE_SELECTED event to the specified
 *         callback function. If the status of this event is BEC_NO_ERROR,
 *         a pointer to the selected device is provided in token-> result.
 *         The ME maintains ownership of the token until this command
 *         completes.
 *
 *     BT_STATUS_NOT_FOUND - There are no devices on the list.
 *
 *     BT_STATUS_INVALID_PARM - The token parameter was invalid.
 */
BtStatus DS_SelectDevice(BtSelectDeviceToken *token);


/*---------------------------------------------------------------------------
 * DS_SelectDeviceResult()
 *
 *     The device selection monitor calls this function to complete
 *     a BTEVENT_SELECT_DEVICE_REQ event. The monitor uses this function
 *     to provide the results of the select device request back to the
 *     requestor. Only the monitor handler uses this function.
 *
 * Parameters:
 *     token - The select device token that was received during a 
 *          BTEVENT_SELECT_DEVICE_REQ in the "p.select" field.
 *
 *     device - A pointer to the selected device.
 *
 *     reason - The result of the request, taken from the BEC_ list.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The function completed successfully.
 *
 *     BT_STATUS_INVALID_PARM - The token or device pointer was invalid.
 */
BtStatus DS_SelectDeviceResult(BtSelectDeviceToken *token, 
                               BtDeviceContext     *device, 
                               BtErrorCode          reason);

#endif /* __MEDEV_H */

