/***************************************************************************
 *
 * File:
 *     $Id: spp.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     Private header file for the Serial Port Profile.  
 *     This file is meant to be included by modules in the portable 
 *     SPP and the OS-specific portion of the SPP (SPPOS).  It defines 
 *     the portable API, data types, and defines presented and required by 
 *     the portable code.  
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

#ifndef __SPP_H
#define __SPP_H

#include "utils.h"
#include "rfcomm.h"
#include "ring.h"
#include "sdp.h"
#include "cmsis_os.h"
#include "cqueue.h"

/*---------------------------------------------------------------------------
 * Serial Port Profile (SPP) layer
 *
 *     The Serial Port Profile (SPP) specification defines procedures
 *     and protocols for Bluetooth devices emulating RS232 or other serial
 *     connections.
 */


/****************************************************************************
 *
 * Section: Configuration Constants
 *
 * The following defines are configuration constants that allow
 * an implementer to include/exclude functionality from SPP.
 *
 ****************************************************************************/
 
/*---------------------------------------------------------------------------
 * SPP_SERVER constant
 *      Configuration constant to enable code for Serial Port Profile
 *      server. If the device is client-only, this should be defined as
 *      XA_DISABLED in overide.h.
 */
#ifndef SPP_SERVER
#define SPP_SERVER      XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * SPP_CLIENT constant
 *      Configuration constant to enable code for Serial Port Profile
 *      client. If the device is server-only, this should be defined as
 *      XA_DISABLED in overide.h.
 */
#ifndef SPP_CLIENT
#define SPP_CLIENT      XA_DISABLED
#endif

#define SPP_RECV_BUFFER_SIZE     3072

typedef struct  {
    uint8_t sem_locked;
    osSemaphoreId _osSemaphoreId;
    osSemaphoreDef_t _osSemaphoreDef;
#ifdef CMSIS_OS_RTX
    uint32_t _semaphore_data[2];
#endif

} spp_lock_t;

typedef struct	{
    osMutexId _osMutexId;
    osMutexDef_t _osMutexDef;
#ifdef CMSIS_OS_RTX
    int32_t _mutex_data[3];
#endif

} spp_mutex_t;

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * SppEvent type
 *
 *     The application is notified of various indications and confirmations 
 *     through a callback function.  Depending on the event, different 
 *     elements of the SppCallbackParms "SppCallbackParms.p" union will be 
 *     valid.
 */
typedef U16 SppEvent;

/** A connection has been established with a remote device.  
 *
 *  When this callback is received, the "SppCallbackParms.p.remDev" field 
 *  contains a pointer to the remote device context.  
 */
#define SPP_EVENT_REMDEV_CONNECTED    	0

/** A connection has been terminated for a remote device.  
 *
 *  When this callback is received, the "SppCallbackParms.p.other" field 
 *  contains a 0.  
 */
#define SPP_EVENT_REMDEV_DISCONNECTED 	1

/** The data has been sent out. At this time the tx buffer can be released.
 *  When this callback is received, the "SppCallbackParms.p.other" field 
 *  is from data structure SppTxDone_t.
 * 
 */
#define SPP_EVENT_DATA_SENT 			2

/** A request to close a channel was received.
 *
 *  When this callback is received, the "SppCallbackParms.p.other" field
 *  contains a 0.
 */
#define SPP_EVENT_REMDEV_DISCONNECTED_IND       3

/** A request to open a channel was received.
 *
 *  When this callback is received, the "SppCallbackParms.p.remDev" field
 *  contains a pointer to the remote device context.
 *
 *  This is an opportunitity for the server to reject the incoming request. To
 *  reject the request modify SppCallbackParms.status to be BT_STATUS_CANCELLED.
 */
#define SPP_EVENT_REMDEV_CONNECTED_IND          4

//min spp number 2
#define SPP_SERVICE_INDEX (0)
#define SPP_CLIENT_INDEX (1)
#define SPP_SERVICE_PORT_NUM (2)
/* End of SppEvent */ 


/*---------------------------------------------------------------------------
 * SppPortType type
 * 
 *     Identifies the serial port as a client (outgoing) or server (incoming). 
 */
typedef U8 SppPortType;

#define SPP_SERVER_PORT      		0x01
#define SPP_CLIENT_PORT      		0x02

typedef struct
{
	U8* 		pTxBuf;
	U16		txDataLength;
} SppTxDone_t;

/* End of SppPortType */

#if SPP_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * SppService structure
 *
 *      Servers need this data type for registering with RFCOMM. A particular
 *      service may have one or more ports available. For example, a device
 *      might provide access to 2 modems. It is providing a single service
 *      (modem) via 2 serial ports.
 */
typedef struct _SppService {
    RfService    service;       /* Required for registering service w/RFCOMM */
    const U8     *name;         /* name of service */
    U16          nameLen;       /* Length of name */
    U16          numPorts;      /* Number of ports in this service */
	SdpAttribute      sppSdpAttribute[4];
    SdpRecord    *sdpRecord;    /* Description of the service */
} SppService;

#endif /* SPP_SERVER == XA_ENABLED */

#if SPP_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * SppClient structure
 *      Contains all fields unique to clients. SppClient is a data type in
 *      device structures (SppDev) for clients.
 */
typedef struct _SppClient {
    /* === Internal use only === */
    BtRemoteDevice      *remDev;
    U8                  serverId;
    SdpQueryToken       sdpToken;
} SppClient;
#endif


typedef struct _SppCallbackParms SppCallbackParms;
typedef struct _SppDev SppDev;


/*---------------------------------------------------------------------------
 * SppCallbackParms structure
 *
 * A pointer to this structure is sent to the application's callback function 
 * notifying the application of state changes or important events.  
 */
struct _SppCallbackParms {
    SppEvent            event;   /* Event associated with the callback */ 

    BtStatus            status;  /* Status of the callback event       */ 

    /* For certain events, a single member of this union will be valid.  
     * See SppEvent documentation for more information.  
     */
    union {
        void           *other;
        BtRemoteDevice *remDev;
    } p;
};


/*---------------------------------------------------------------------------
 * SppCallback type
 *
 * A function of this type is called to indicate events to the application.
 */
typedef void (*SppCallback)(SppDev *locDev, SppCallbackParms *Info);

/* End of SppCallback */ 


typedef struct _SppModeStruct {
    BtRemoteDevice *remDev;
    SppCallback     callback;
} SppModeStruct;


/*---------------------------------------------------------------------------
 * SppDev structure
 *      This structure defines an SPP device. A user of SPP must allocate
 *      one SppDev for each serial device. Members should not be accessed
 *      directly.
 */
struct _SppDev {
    /* Pointer to platform-specific data. This pointer is passed to all
     * platform-specific functions (prefaced SPPOS_).
     */
    void            *osDev;

    SppPortType     portType;       /* SPP_SERVER_PORT or SPP_CLIENT_PORT */

    /* Fields specific to clients and servers */
    union {

#if SPP_CLIENT == XA_ENABLED

        SppClient   client;

#endif

#if SPP_SERVER == XA_ENABLED

        SppService  *sppService;

#endif

    } type;

    /* === Internal use only === */

    SppCallback     callback;       /* application callback function */ 

    /* Server / Client elements */
    U8              state;          /* device state */
    RfChannel       channel;
    RfModemStatus   rModemStatus;   /* remote modem status */
    RfModemStatus   lModemStatus;   /* local modem status */
    RfLineStatus    lineStatus;   
    RfPortSettings  portSettings;

    U8              xonChar;
    U8              xoffChar;
    S16             highWater;      /* when rRing.dataLen >= highWater,
                                       flow off rx */
    U8              credit;         /* rx credits outstanding */
    U8              maxCredits;     /* maximum rx credits to issue */
    U8              msr;            /* Modem Status Register */
    S16             breakLen;

    /* Transmit packet */
    ListEntry       freeTxPacketList;

    ListEntry       pendingTxPacketList;
};

typedef struct _OsSppDev {
	void  *mutex;
	
	spp_lock_t wait_data;
	spp_mutex_t spp_mutex;
	CQueue  rx_queue;
	U8    sppRxBuffer[SPP_RECV_BUFFER_SIZE];
	struct _SppDev *sppDev;
} OsSppDev;

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 *
 * Section: Driver Initialization Functions
 *
 * These APIs are called typically when the driver is initialized or loaded.
 *
 ***************************************************************************/


/*---------------------------------------------------------------------------
 * SPP_InitDevice()
 *
 *      Initializes a Serial Port Profile device structure. Call this API
 *      for each SPP device before using it for any other SPP API. In most
 *      cases this would be during driver initialization. 
 *
 * Parameters:
 *      dev -   pointer to existing device structure
 *      packets - pointer to an array of TX packets used when queuing data to
 *          RFCOMM.
 *      numPackets - number of packets pointed to by 'packets'. This number
 *          can be 1 or more. If too few packets are provided, SPP may
 *          run out and be unable to queue more data to RFCOMM until packets
 *          in use are returned to SPP.
 */
void SPP_InitDevice(SppDev *dev, BtPacket *packets, U16 numPackets);


/****************************************************************************
 *
 * Section: Serial Port Profile I/O Functions
 *
 * These APIs are provided to upper layers. Normally, they are called
 * as the result of a corresponding call to the system API. For example,
 * in a POSIX interface open() would call SPP_Open().
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * SPP_Open()
 *
 *      Opens a serial device for reading and writing. Requires an 
 *      existing ACL connection with the remote device for client devices.  
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *      btDevice - for client port this is a pointer to an existing 
 *               RemoteDevice structure describing the remote device to 
 *               which a service connection will be made.  Server ports 
 *               ignore this field.
 *      callback - The callback function that should be invoked to inform the 
 *               application of events of interest.  
 *
 * Returns:
 *      BT_STATUS_SUCCESS - device opened successfully
 *      BT_STATUS_INVALID_PARM - invalid parameter; usually this is because
 *                               btDevice is not valid or no ACL exists to
 *                               the device
 *      BT_STATUS_FAILED - device is not in a closed state
 */
BtStatus SPP_Open(SppDev         *dev, 
                  BtRemoteDevice *btDevice, 
                  SppCallback     callback);


/*---------------------------------------------------------------------------
 * SPP_Close() 
 *
 *     Close the serial device. Requires the device to have been opened 
 *     previously by SPP_Open(). This function does not verify that the 
 *     calling thread or process is the same one that opened it. The calling
 *     layer may optionally do any such checks.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      BT_STATUS_SUCCESS - closed successfully
 *      BT_STATUS_FAILED - device was not opened
 */
BtStatus SPP_Close(SppDev *dev);


/*---------------------------------------------------------------------------
 * SPP_Read() 
 *      Read from the serial device. Requires the device to have been 
 *      successfully opened by SPP_Open().
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *      buffer - allocated buffer to receive bytes
 *      maxBytes - on input: maximum bytes to read; on successful completion:
 *                 number of bytes actually read
 *
 * Returns:
 *      BT_STATUS_SUCCESS - read was successful
 *      BT_STATUS_FAILED - device is not opened.
 */
BtStatus SPP_Read(SppDev *dev, char *buffer, U16 *maxBytes);


/*---------------------------------------------------------------------------
 * SPP_Write()
 *      Write to the serial device. Requires the device to have been  
 *      successfully opened by SPP_Open().
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *      buffer - buffer containing characters to write
 *      nBytes - on input: pointer to number of bytes in buffer; actual
 *               bytes written are returned in nBytes if it returns success
 *
 * Returns:
 *      BT_STATUS_SUCCESS - write was successful
 *      BT_STATUS_FAILED - device is not open
 */
BtStatus SPP_Write(SppDev *dev, char *buffer, U16 *nBytes);


/*---------------------------------------------------------------------------
 * SPP_Ioctl()
 *      Perform an I/O control function on the serial device.
 *      Requires the device to have been successfully opened by the caller.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *      function - I/O control function as allowed by OS
 *      arg - argument peculiar to function; many ioctl functions alter
 *            the contents of *arg upon success (this is totally platform
 *            dependent and under the control of SPPOS_Ioctl)
 *
 * Returns:
 *      This function returns the status of SPPOS_Ioctl.
 */
BtStatus SPP_Ioctl(SppDev *dev, U16 function, void *arg);



/****************************************************************************
 * Section: SPP Lower-layer API's
 *
 * The following APIs are primarily for I/O control operations. That is,
 * the serial driver could call these functions for assistance in carrying
 * out a particular I/O request. For example, SPPOS_Ioctl() might use
 * these functions to obtain information about the device.
 *
 * Error checking (for example, making sure the SppDev is open and in the
 * right state, protecting from race conditions, etc.) is assumed handled
 * prior to making these calls.
 *
 ***************************************************************************/


/*---------------------------------------------------------------------------
 * SPP_GetDataFormat() 
 *      Retrieve the current data format for the specified device.
 *
 * Parameters:
 *      dev - pointer to existing device structure
 *
 * Returns:
 *      Current data format (data bits, stop bits and parity). The value
 *      returned is a superset of that for each individual setting (data bits,
 *      stop bits and parity - see APIs below).
 */
RfDataFormat SPP_GetDataFormat(SppDev *dev);
#define SPP_GetDataFormat(dev)        ((dev)->portSettings.dataFormat)


/*---------------------------------------------------------------------------
 * SPP_GetDataBits() 
 *      Get the number of data bits in effect for the specified device.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      Current setting for number of data bits as defined by RfDataFormat.
 */
RfDataFormat SPP_GetDataBits(SppDev *dev);
#define SPP_GetDataBits(dev)        (RfDataFormat)((dev)->portSettings.dataFormat \
                                     & RF_DATA_BITS_MASK)


/*---------------------------------------------------------------------------
 * SPP_GetParity()
 *      Get the current parity setting for the specified device. 
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      Current parity setting (see RfDataFormat).
 */
RfDataFormat SPP_GetParity(SppDev *dev);
#define SPP_GetParity(dev)        (RfDataFormat)((dev)->portSettings.dataFormat \
                                   & (RF_PARITY_TYPE_MASK | RF_PARITY_MASK))


/*---------------------------------------------------------------------------
 * SPP_GetStopbits()
 *      Retrieve the number of stop bits for the specified device. 
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      Current setting for number of stop bits as defined by RfDataFormat.
 */
RfDataFormat SPP_GetStopBits(SppDev *dev);
#define SPP_GetStopBits(dev)        (RfDataFormat)((dev)->portSettings.dataFormat \
                                     & RF_STOP_BITS_MASK)


/*---------------------------------------------------------------------------
 * SPP_GetFlowControl() 
 *      Retrieve the current flow control setting for the specified device.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      Flow control setting (see RfFlowControl).
 */
RfFlowControl SPP_GetFlowControl(SppDev *dev);
#define SPP_GetFlowControl(dev)        ((dev)->portSettings.flowControl)



/*---------------------------------------------------------------------------
 * SPP_GetModemStatus()
 *      Retrieve the current modem status signals for the specified device. 
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      Modem status signals (see RfSignals).
 */
RfSignals SPP_GetModemStatus(SppDev *dev);
#define SPP_GetModemStatus(dev)        ((dev)->rModemStatus.signals)


/*---------------------------------------------------------------------------
 * SPP_GetBaud() 
 *      Retrieve the current baud rate setting for the specified device.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      Baud rate (see RfBaudRate).
 */
RfBaudRate SPP_GetBaud(SppDev *dev);
#define SPP_GetBaud(dev)        ((dev)->portSettings.baudRate)


/*---------------------------------------------------------------------------
 * SPP_SendPortSettings()
 *      Transmit the port settings (RfPortSettings) to the remote device.
 *      When a port setting changes on the local system, that setting should
 *      be communicated to the remote device. 
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The processing of sending to the remote device 
 *               has been started or RF_SEND_CONTROL is disabled
 *      BT_STATUS_FAILED
 */
BtStatus SPP_SendPortSettings(SppDev *dev);


/*---------------------------------------------------------------------------
 * SPP_SetModemControl()
 *      Set the modem control signals for the specified device. 
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *      signals - modem control signals to be set
 *
 * Returns:
 *      The modem control signals are set in the device structure. If
 *      RF_SEND_CONTROL is enabled, the settings are sent to the remote
 *      device.
 *
 *      BT_STATUS_SUCCESS - RF_SEND_CONTROL is disabled or
 *               RF_SEND_CONTROL is enabled and the transmission
 *               was begun.
 *
 *      Other returns - See RF_SetModemStatus().
 */
BtStatus SPP_SetModemControl(SppDev *dev, RfSignals signals);


/*---------------------------------------------------------------------------
 * SPP_SetBaud()
 *      Set the baud rate for the specified device. Baud rate for the
 *      device is informational only. 
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *      baud -   current baud rate
 *
 * Returns:
 *      No error checking is performed. The device structure is updated
 *      with 'baud'. Note that SPP_SendPortSettings() should be called
 *      subsequently to send the setting to the remote device. It is not
 *      performed automatically in case other port settings are also to 
 *      be set.
 */
void SPP_SetBaud(SppDev *dev, U8 baud);


/*---------------------------------------------------------------------------
 * SPP_SetDataFormat() 
 *      Set the data format values for the specified device.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *      value -    data format (see RfDataFormat)
 *
 * Returns:
 *      No error checking is performed. The device structure is updated
 *      with 'value'. Note that SPP_SendPortSettings() should be called
 *      subsequently to send the setting to the remote device. It is not
 *      performed automatically in case other port settings are also to 
 *      be set.
 */
void SPP_SetDataFormat(SppDev *dev, U8 value);



/*---------------------------------------------------------------------------
 * SPP_SetFlowControl() 
 *      Set the flow control value for the specified device.
 *
 * Parameters:
 *      dev -    pointer to existing device structure
 *      value -    Flow control value (see RfFlowControl).
 *
 * Returns:
 *      No error checking is performed. The device structure is updated
 *      with 'value'. Note that SPP_SendPortSettings() should be called
 *      subsequently to send the setting to the remote device. It is not
 *      performed automatically in case other port settings are also to 
 *      be set.
 */
void SPP_SetFlowControl(SppDev *dev, U8 value);


/****************************************************************************
 *
 * Section: OS-Provided APIs
 *
 * The following functions are OS-specific in how they are implemented. 
 * The Serial Port Profile calls them in order to carry out its tasks.
 * The implementer needs to port these functions to the platform.
 *
 ***************************************************************************/


/*---------------------------------------------------------------------------
 * SPPOS_LockDev() 
 *      Locks a serial device structure to prevent reentrancy and data
 *      access problems. Called in task context. 
 *
 *      Must allow for nested calls from the same thread. Calling this 
 *      function more than once without an intermediate call to SPPOS_UnlockDev()
 *      should have the same result as calling it once. That is, after
 *      the first call, subsequent calls should be a no-op.
 *
 *      On a non-multitasking platform this could be a no-op.
 *
 * Parameters:
 *      mutex - pointer to an object appropriate for the operating system;
 *              for example, a mutex or binary semaphore
 */
void SPPOS_LockDev(void *dev);


/*---------------------------------------------------------------------------
 * SPPOS_UnlockDev() 
 *      Unlocks a serial device structure. This is the counterpart for 
 *      SPPOS_LockDev. When this function exits, another device pending
 *      on the object is allowed to obtain that object. If more than one
 *      call to SPPOS_LockDev() had been made by the same thread without
 *      an intervening call to SPPOS_UnlockDev(), the first call to this
 *      function should unlock the device.     
 *     
 *      On a non-multitasking platform this could be a no-op.
 *
 * Parameters:
 *      mutex - pointer to an object appropriate for the operating system;
 *              for example, a mutex or binary semaphore
 */
void SPPOS_UnlockDev(void *dev);


/*---------------------------------------------------------------------------
 * SPPOS_Wait()
 *      Causes the caller to block. If another task is ready to run, that
 *      task should be given context by the operating systems scheduler.
 *
 * Parameters:
 *      mutex - pointer to an object appropriate for the operating system;
 *              for example, a mutex or binary semaphore
 *
 *      ms    - number of milliseconds to pend; note that need not be at 
 *              all accurate. Currently, this parameter is used to avoid
 *              a wait-forever condition that might arise from an
 *              implementation bug.
 *
 *      On a non-multitasking platform this could be a no-op.
 *
 * Returns:
 *      BT_STATUS_SUCCESS      SPPOS_Resume was called before timeout
 *      BT_STATUS_FAILED   timeout or other error occurred
 */
BtStatus SPPOS_Wait(void *dev, U16 ms);


/*---------------------------------------------------------------------------
 * SPPOS_Resume()
 *      Allow a blocked task to resume execution. This is the counterpart
 *      of SPPOS_Wait. When this function exits, the task waiting on the 
 *      object is allowed to run. It is operating system dependent as to 
 *      when the task is scheduled. 
 *
 *      This function must obviously be callable by a task other that the
 *      one that called SPPOS_Wait.
 *
 *      On a non-multitasking platform this could be a no-op.
 *
 * Parameters:
 *      mutex - pointer to an object appropriate for the operating system;
 *              for example, a mutex or binary semaphore
 */
void SPPOS_Resume(void *dev);


/*---------------------------------------------------------------------------
 * SPPOS_ReadBuf() 
 *      The SPP Driver calls this function when the caller is 
 *      reading the serial device (RFCOMM).
 *
 *      It should handle any and all special-character handling such as
 *      software flow control, backspace and end-of-line handling etc.
 *      It is entirely up to the implementer to decide how much special-
 *      character handling is to be done. Typically, this function extracts
 *      bytes from a ring buffer. If your OS routine handles only a single
 *      character, then this function should call it iteratively for each
 *      byte returned.
 *
 * Parameters:
 *      dev  - pointer to SppDev
 *      buf  - allocated buffer to receive bytes
 *      len  - pointer to max number of characters to be put in buf; on
 *             successful return this is set to number of bytes actually read
 *
 * Returns:
 *      BT_STATUS_SUCCESS - completed successfully
 *      BT_STATUS_FAILED - error as determined by platform implementation
 */
BtStatus SPPOS_ReadBuf(void *dev, char* buf, U16 *len);


/*---------------------------------------------------------------------------
 * SPPOS_WriteBuf() 
 *      Process characters to be transmitted using the serial device. 
 *      The SPP Driver calls this function when the caller is 
 *      writing bytes to the serial device (RFCOMM).
 *      Typically, this function places bytes in a ring buffer. 
 *      If your OS routine handles only a single character, then this 
 *      function should call it iteratively for each byte in the buffer.
 *
 * Parameters:
 *      dev  - pointer to SppDev
 *      buf  - buffer of characters received from RFCOMM
 *      len  - pointer to number of characters in buf; on successful
 *             return this is set to number of bytes actually written
 *
 * Returns:
 *      BT_STATUS_SUCCESS - len contains number of bytes in buf
 *      BT_STATUS_FAILED
 */
BtStatus SPPOS_WriteBuf(void *dev, char buf[], U16 *len);


/*---------------------------------------------------------------------------
 * SPPOS_DevRx() 
 *      Process characters received from the serial device. 
 *      This function is called by SPP for when characters are received
 *      from RFCOMM. Contrast this to SPPOS_ReadBuf(), which is called 
 *      when the application layer reads from the device itself.
 *
 *      It should handle any and all special-character handling such as
 *      software flow control, backspace and end-of-line handling etc.
 *      It is entirely up to the implementer to decide how much special-
 *      character handling is to be done. Typically, this function places
 *      bytes in a ring buffer. If your OS routine handles only a single
 *      character, then this function should call it iteratively for each
 *      byte in the buffer.
 *
 * Parameters:
 *      dev  - pointer to SppDev
 *      buf  - buffer of characters received from RFCOMM
 *      len  - number of characters in buf; on successful
 *             return this is set to number of bytes in buf
 *
 * Returns:
 *      BT_STATUS_SUCCESS    - *len is the number of bytes written
 *      BT_STATUS_FAILED - 
 */
BtStatus SPPOS_DevRx(void *dev, char buf[], U16 *len);


/*---------------------------------------------------------------------------
 * SPPOS_DevTx() 
 *      Get the next characters to be transmitted via RFCOMM.
 *      This function is called by SPP to retrieve bytes to be sent
 *      via RFCOMM. Typically, this routine removes characters from
 *      a ring buffer. Contrast this function to SPPOS_WriteBuf(), which
 *      queues data to be transmitted. This API retrieves that same data
 *      for actual transmission.
 *
 *      It should handle any and all special-character handling such as
 *      software flow control, backspace and end-of-line handling etc.
 *      It is entirely up to the implementer to decide how much special-
 *      character handling is to be done.
 *
 * Parameters:
 *      dev  - pointer to SppDev
 *
 *      buf  - On output, filled with a char pointer full of data. If "len"
 *             is 0, a NULL pointer may be provided. If non-NULL, this pointer
 *             is owned by the SPP layer until SPPOS_ReturnBuf is called.
 *              
 *      len  - On output, contains the actual number of bytes in "buf".
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Indicates that "buf" contains bytes for transmission.
 *
 *      BT_STATUS_FAILED  - no more characters to send
 */
BtStatus SPPOS_DevTx(void *dev, char **buf, U16 dataLengthInBuf, U16 *len);



/*---------------------------------------------------------------------------
 * SPPOS_Ioctl() 
 *      This routine implements the entire I/O control for the device.
 *      It is highly platform-specific which ioctl functions are supported.
 *      The implementer should consider which functions to support based
 *      on the needs of higher-layer software. 
 *
 * Parameters:
 *      dev  - pointer to SppDev
 *      function - I/O control function code as defined by the platform
 *      arg - a function-specific argument; on successful return
 *            this is set to output of the function if appropriate for
 *            the particular function; implementation specific
 *
 * Returns:
 *      implementation and function specific
 */
BtStatus SPPOS_Ioctl(void *dev, U16 function, void *arg);



/*---------------------------------------------------------------------------
 * SPPOS_RxBytes() 
 *     Determine the number of bytes available for reading. The SPP driver
 *     calls this function. It may optionally be called in response
 *     to an IOCTL call.
 *
 * Parameters:
 *      dev  - pointer to SppDev
 *
 * Returns:
 *      max number of bytes to read
 */
U16 SPPOS_RxBytes(void *dev);



/*---------------------------------------------------------------------------
 * SPPOS_RxFree() 
 *     Determine the number of bytes available for receiving bytes from 
 *     the device. Called by the SPP driver and optionally in response
 *     to an IOCTL call.
 *
 * Parameters:
 *      dev  - pointer to SppDev
 *
 * Returns:
 *      max number of bytes to read
 */
U16 SPPOS_RxFree(void *dev);



/*---------------------------------------------------------------------------
 * SPPOS_FlushRx() 
 *      Flushes all the unread bytes received on the device
 *
 * Parameters:
 *      dev  - pointer to SppDev
 */
void SPPOS_FlushRx(void *dev);



/*---------------------------------------------------------------------------
 * SPPOS_FlushTx() 
 *      Flushes all the bytes that have not been transmitted on the device
 *
 * Parameters:
 *      dev  - pointer to SppDev
 */
void SPPOS_FlushTx(void *dev);



/*---------------------------------------------------------------------------
 * SPPOS_ReturnBuf() 
 *      Notify the OS-specific portion of the driver that a buffer is no
 *      longer needed and may be reused or unallocated. The buffer being 
 *      freed is one that is allocated and/or returned in SPPOS_DevTx.
 *
 * Parameters:
 *      dev  - pointer to SppDev
 *      len  - length of buf
 */
void SPPOS_ReturnBuf(void *dev, U16 len);

#ifdef __cplusplus
	}
#endif


#endif /* __SPP_H */

