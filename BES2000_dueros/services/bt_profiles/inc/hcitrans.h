/****************************************************************************
 *
 * File:
 *     $Id: hcitrans.h 827 2009-02-12 23:35:42Z dliechty $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 827 $
 *
 * Description:
 *     Types, defines, and prototypes used for creating an HCI
 *     transport driver.
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

#ifndef __HCITRANS_H
#define __HCITRANS_H

#include "bttypes.h"
#include "sys/hci.h"

/*---------------------------------------------------------------------------
 * HCI Transport Driver API layer
 *
 *     The Bluetooth specifications identify a number of acceptable
 *     transport mechanisms for communicating between Bluetooth radio
 *     firmware and software such as UART, RS232, and USB. While the HCI
 *     specification defines the kinds of commands and events that can be
 *     exchanged, the HCI transport layer determines how those commands
 *     and events are transmitted. The transport layer then makes calls
 *     to a hardware-driver layer to communicate with the actual hardware.
 *
 *     In a typical scenario, a higher layer requests an HCI command, such as
 *     "Inquiry". The HCI layer packages this command into an HciPacket
 *     structure and sends it to the HCI transport driver using
 *     tranEntry.sendData(). Then, the HCI transport driver converts the
 *     HciPacket structure into a stream of bytes, which are transmitted
 *     using the HCI hardware driver.
 *
 *     If you are using a common Bluetooth transport, such as UART, you can
 *     use the HCI transport code provided with the Blue SDK. If you are
 *     using a hardware-specific transport, such as a flash memory or
 *     PCMCIA interface, you will need to provide your own transport driver
 *     as defined by this interface.
 *
 *     Communication with the radio module may not be complete until
 *     vendor specific HCI commands can be sent to the radio module to
 *     perform module specific initialization.  This transport API also
 *     includes an API for creating module specific initialization code.
 */


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BT_STATUS_SUCCESS_NO_CONFIG constant
 *
 *     This return code can be used upon completion of Radio Module Specific
 *     Initialization.  It tells the Radio Manager that no flow control
 *     configuration should be performed by the HCI.
 *
 *     If this return code is used, the Radio Module Specific Initialization
 *     is responsible for setting up HCI flow control.  This is accomplished by
 *     using the HCC_READ_BUFFER_SIZE and HCC_HOST_BUFFER_SIZE HCI commands.
 *     In the case of transmit flow control, this information must be
 *     communicated to the HCI by calling HCI_SetFlowParms().
 *
 *     AT the very minimum, the Radio Module Specific Initialization must
 *     read the buffer sizes of the Host Controller by sending the
 *     HCC_READ_BUFFER_SIZE command, parsing the results, and sending that
 *     information to the HCI via the HCI_SetFlowParms() function.
 *
 *     If the HCC_HOST_BUFFER_SIZE command is not sent to the Host Controller,
 *     then it is important that the following definitions are configured to
 *     match the buffer requirements of Host Controller.
 *
 *     HCI_NUM_SCO_BUFFERS
 *     HCI_SCO_DATA_SIZE
 *     HCI_NUM_ACL_BUFFERS
 *     HCI_ACL_DATA_SIZE
 */
#define BT_STATUS_SUCCESS_NO_CONFIG (BT_STATUS_LAST_CODE+1)

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * RxBuffHandle type
 *
 *     Represents a handle to a receive buffer.  Receive buffers are
 *     allocated by the HCI driver.  HCI Transport drivers must make requests
 *     for buffers from the HCI driver and return them with data to the HCI
 *     driver for processing.
 */
typedef HciBuffer *RxBuffHandle;

/* End of RxBuffHandle */

/*---------------------------------------------------------------------------
 * HciConnHandle type
 *
 *     Represents a connection handle between the host and the host
 *     controller.  The value for the handle must be parsed out of the
 *     first two bytes of ACL and SCO data and passed to HCI_GetAclBuffer()
 *     and HCI_GetScoBuffer().
 */
typedef HciHandle HciConnHandle;

/* End of HciConnHandle */

/*---------------------------------------------------------------------------
 * HciBufFlags type
 *
 *     Represents the type of data exchanged between the HCI layer and the
 *     HCI transport driver. These flags are stored in the "flags" field
 *     of HciPacket to indicate the type of packet being sent.
 */
typedef U8 HciBufFlags;

#define HCI_BUFTYPE_COMMAND      0x01
#define HCI_BUFTYPE_EVENT        0x02 /* Never sent as an HciPacket */
#define HCI_BUFTYPE_ACL_DATA     0x04
#define HCI_BUFTYPE_SCO_DATA     0x08
#define HCI_BUFTYPE_INTERMEDIATE 0x10 /* Intermediate part of an L2CAP packet */
#define HCI_BUFTYPE_L2CAP        0x20 /* L2CAP owns this buffer */
#if IAG_BLE_INCLUDE == XA_ENABLED
#define HCI_BUFTYPE_BLE          0x40
#endif

/* End of HciBufFlags */

/*---------------------------------------------------------------------------
 * HciGetRxBufferFunc type
 *     Type for function provided by the HCI API for use by the transport
 *     driver. See HCI_GetAclBuffer, HCI_GetScoBuffer, and HCI_GetEventBuffer.
 */
typedef BtStatus (*HciGetRxBufferFunc)(RxBuffHandle *rxBuffHandle,
                                       HciConnHandle hciConnHandle,
                                       U16 len, U8 type);
/* End of HciGetRxBufferFunc */

/*---------------------------------------------------------------------------
 * HciSetRxBufferLenFunc type
 *     Type for function provided by the HCI API for use by the transport
 *     driver. See HCI_SetBufferLen
 */
typedef BtStatus (*HciSetRxBufferLenFunc)(RxBuffHandle rxBuffHandle, U16 len);
/* End of HciSetRxBufferLenFunc */

/*---------------------------------------------------------------------------
 * HciRxBufferIndFunc type
 *     Type for function provided by the HCI API for use by the transport
 *     driver. See HCI_RxBufferInd.
 */
typedef void (*HciRxBufferIndFunc)(RxBuffHandle rxBuffhandle, BtStatus status);

/* End of HciRxBufferIndFunc */

/*---------------------------------------------------------------------------
 * HciPacketSentFunc type
 *     Type for function provided by the HCI API for use by the transport
 *     driver. See HCI_PacketSent.
 */
typedef void (*HciPacketSentFunc)(HciPacket *packet);

/* End of HciPacketSentFunc */

/*---------------------------------------------------------------------------
 * HciTransportErrorFunc type
 *     Type for function provided by the HCI API for use by the transport
 *     driver. See HCI_TransportError.
 */
typedef void (*HciTransportErrorFunc)(HciTransError error);

/* End of HciTransportErrorFunc */

/*---------------------------------------------------------------------------
 * TranSendDataFunc type
 *
 *     Called by the HCI driver to provide commands and data to be sent to
 *     the host controller.
 */
typedef void (*TranSendDataFunc)(HciPacket *packet);

/* End of TranSendDataFunc */

/*---------------------------------------------------------------------------
 * TranBufferAvailFunc type
 *
 *     Called by the HCI driver to indicate that a receive buffer has been
 *     freed.
 */
typedef void (*TranBufferAvailFunc)(HciBufFlags buffType);

/* End of TranBufferAvailFunc */

/*---------------------------------------------------------------------------
 * TranSetSpeedFunc type
 *
 *     Called by the HCI driver to set the speed of the transport.
 */
typedef void (*TranSetSpeedFunc)(U16 speed);

/* End of TranSetSpeedFunc */

/*---------------------------------------------------------------------------
 * TranNumScoConsFunc type
 *
 *     Called by the HCI driver to indicate the number of active SCO
 *     connections.
 */
typedef void (*TranNumScoConsFunc)(U16 numScoCons);

/* End of TranNumScoConsFunc */

/*---------------------------------------------------------------------------
 * TranEvent type
 *
 *     When a transport is initialized or shut down, it can be done
 *     synchronously or asynchronously.  If completed asynchronously,
 *     the status of the transport is sent through a callback function as an
 *     event.  The following events describe this status.
 */
typedef U8 TranEvent;

/** The transport has completed initialization.  If initialization was
 * successful, then the "Status" parameter of the callback function should
 * be set to BT_STATUS_SUCCESS.  If the initialization failed, then the
 * "Status" parameter should be set to BT_STATUS_FAILED.
 */
#define TRAN_INIT_STATUS    1

/** The transport has completed shutdown.  If shutdown was successful, then
 * the "Status" parameter of the callback function should be set to
 * BT_STATUS_SUCCESS.  If the shutdown failed, then the "Status"
 * parameter should be set to BT_STATUS_FAILED.
 */
#define TRAN_SHUTDOWN_STATUS  2

/* End of TranEvent */

/*---------------------------------------------------------------------------
 * TranCallback type
 *
 *     Called by the transport initialization and shutdown code to indicate
 *     it's initialization or shutdown state.  The "Event" parameter contains
 *     the appropriate event (see TranEvent), and the "Status" parameter
 *     contains the status of the event.
 */
typedef void (*TranCallback)(TranEvent Event, BtStatus Status);

/* End of TranCallback */

/*---------------------------------------------------------------------------
 * RadioEvent type
 *
 *     When a radio module is initialized or shut down, it can be done
 *     synchronously or asynchronously.  If completed asynchronously, the
 *     status of the process is sent through a callback function as an event.
 *     The following events describe this status.
 */
typedef U8 RadioEvent;

/** The radio module has completed initialization.  If initialization was
 * successful, then the "Status" parameter of the callback function should
 * be set to BT_STATUS_SUCCESS.  If the initialization failed, then the
 * "Status" parameter should be set to BT_STATUS_FAILED.
 */
#define RADIO_DEFAULT_STATUS  0

/** The radio module has completed initialization.  If initialization was
 * successful, then the "Status" parameter of the callback function should
 * be set to BT_STATUS_SUCCESS.  If the initialization failed, then the
 * "Status" parameter should be set to BT_STATUS_FAILED.
 */
#define RADIO_INIT_STATUS    1

/** The radio module has completed shutdown.  If shutdown was successful, then
 * the "Status" parameter of the callback function should be set to
 * BT_STATUS_SUCCESS.  If the initialization failed, then the "Status"
 * parameter should be set to BT_STATUS_FAILED.
 */
#define RADIO_SHUTDOWN_STATUS  2

/* End of RadioEvent */

/*---------------------------------------------------------------------------
 * RadioCallback type
 *
 *     Called by the radio module initialization and shutdown code to indicate
 *     it's initialization or shutdown state.  The "Event" parameter contains
 *     the appropriate event (see RadioEvent), and the "Status" parameter
 *     contains the status of the event.
 */
typedef void (*RadioCallback)(RadioEvent Event, BtStatus Status);

/* End of RadioCallback */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HciFragment structure
 *
 *     Represents a fragment of data that may appear in an HciPacket.
 */
typedef struct _HciFragment {
   const U8     *buffer;
   U16           len;
} HciFragment;

/* End of HciFragment */

/*---------------------------------------------------------------------------
 * HciPacket structure
 *
 *     Represents a complete HCI packet to be transmitted.  All data passed
 *     from the HCI driver to the HCI transport driver are passed in an
 *     HciPacket structure.  The actual packet data consists of the four
 *     HCI header bytes, followed by any number of data fragments. These
 *     fragments must be reassembled by the transport layer when sending data
 *     to the hardware driver.
 *
 *     There is no length field for header information, but the length can be
 *     determined by the type of data in the packet (stored in "flags").
 *     ACL data headers always have a length of 4.  SCO data and command
 *     packets have a header length of 3.
 */
struct _HciPacket {
    ListEntry    node;       /* Node element for linked list management. */

    U8           header[4];  /* HCI packet header. Transmit these bytes
                              * first. This array contains three valid bytes
                              * for commands and SCO data packets. For ACL
                              * data it contains four valid bytes.
                              */

    U8           fCount;     /* The number of fragments present. Zero to
                              * three fragments may be specified. */
#if L2CAP_NUM_ENHANCED_CHANNELS > 0
    HciFragment f[4];       /* Array of fragments. */
#else
    HciFragment f[3];       /* Array of fragments. */
#endif /* L2CAP_NUM_ENHANCED_CHANNELS > 0 */

    HciBufFlags flags;      /* Flags that identify the type of the packet */
};

/* End of HciPacket */

#if IAG_BLE_INCLUDE == XA_ENABLED
/*---------------------------------------------------------------------------
 * TranSendBufferFunc type
 *
 *     Called by the HCI driver to provide commands and data to be sent to
 *     the host controller.
 */
typedef int (*TranSendBufferFunc)(U8 packet_type, U8 *packet, int size);
#endif

/*---------------------------------------------------------------------------
 * TranEntry structure
 *
 *     Function table containing the entry points into the HCI transport driver.
 *     All callbacks from the HCI driver are made through this table.
 */
typedef struct _TranEntry {
    TranSendDataFunc     sendData;         /* Entry point for sending data
                                            * to the transport driver.
                                            */

    TranBufferAvailFunc  buffAvail;        /* Entry point for receiving
                                            * indication of a free buffer.
                                            */

    TranSetSpeedFunc     setSpeed;         /* Entry point for setting the
                                            * speed of the transport driver.
                                            */

    TranNumScoConsFunc   numScoCons;       /* Entry point that indicates the
                                            * number of SCO connections
                                            * currently active.
                                            */
#if IAG_BLE_INCLUDE == XA_ENABLED
    TranSendBufferFunc  sendBuffer;
#endif
} TranEntry;

/* End of TranEntry */

/****************************************************************************
 *
 * Section: HCI Functions Called by Transports
 *
 ****************************************************************************/

#include "sys/host.h"

/*---------------------------------------------------------------------------
 * HCI_RegisterTransport()
 *
 *     Called by the transport driver to register its entry points.  These
 *     entry points must be registered before the stack can communicated with
 *     the Bluetooth radio module.  This function should be called after the
 *     radio is available (i.e. inserted, powered up, and initialized).
 *
 * Parameters:
 *     tranEntry - Transport entry point.  Each function pointer in this
 *         structure must be initialized before calling this function (see
 *         the TranEntry structure).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The transport entry points were successfully
 *         registered with HCI.
 *
 *     BT_STATUS_INVALID_PARM - The tranEntry parameter was not properly
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HCI_RegisterTransport(TranEntry *tranEntry);

/*---------------------------------------------------------------------------
 * HCI_DeregisterTransport()
 *
 *     Called by the transport driver to deregister its entry points.  This
 *     function should be called be called when the radio is no longer
 *     available (i.e. powered down or physically removed).
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     void
 */
void HCI_DeregisterTransport(void);
#define HCI_DeregisterTransport() HCI_RegisterTransport(0)

/*---------------------------------------------------------------------------
 * HCI_GetAclBuffer()
 *
 *     Called by the transport driver to get an available ACL receive buffer.
 *     Once received data is placed in the buffer, it should be returned
 *     to the HCI for processing by calling HCI_RxBufferInd().
 *     This call may be issued at interrupt time or during execution of
 *     the high-priority driver thread (see OS_StopHardware).
 *
 * Parameters:
 *     rxBuffHandle - A pointer to receive the handle of an initialized
 *                    HCI buffer.
 *
 *     len - The desired length of the receive buffer.  The length must
 *           be the exact length of the data that will be received.
 *
 *     hciConnHandle - Specifies the HCI handler for which this buffer will be
 *           used.
 *
 * Returns:
 *           BT_STATUS_SUCCESS - A buffer is available and can be accessed
 *               by "rxBuffHandle".
 *
 *           BT_STATUS_NO_RESOURCES - No buffer is currently available.
 *
 *           BT_STATUS_INVALID_PARM - The requested length was too long, or
 *               the specified type was invalid.
 */
BtStatus HCI_GetAclBuffer(RxBuffHandle *rxBuffHandle,
                          HciConnHandle hciConnHandle, U16 len);
#define HCI_GetAclBuffer(r, h, l) RXBUFF_Alloc(r, h, l, HCI_BUFTYPE_ACL_DATA)

/*---------------------------------------------------------------------------
 * HCI_GetScoBuffer()
 *
 *     Called by the transport driver to get an available SCO receive buffer.
 *     Once received data is placed in the buffer, it should be returned
 *     to the HCI for processing by calling HCI_RxBufferInd().
 *     This call may be issued at interrupt time or during execution of
 *     the high-priority driver thread (see OS_StopHardware).
 *
 * Parameters:
 *     rxBuffHandle - A pointer to receive the handle of an initialized
 *                    HCI buffer.
 *
 *     len - The desired length of the receive buffer.  The length must
 *           be the exact length of the data that will be received.
 *
 *     hciConnHandle - Specifies the HCI handler for which this buffer will be
 *           used.
 *
 * Returns:
 *           BT_STATUS_SUCCESS - A buffer is available and can be accessed
 *               by "rxBuffHandle".
 *
 *           BT_STATUS_NO_RESOURCES - No buffer is currently available.
 *
 *           BT_STATUS_INVALID_PARM - The requested length was too long, or
 *               the specified type was invalid.
 */
BtStatus HCI_GetScoBuffer(RxBuffHandle *rxBuffHandle,
                          HciConnHandle hciConnHandle, U16 len);
#define HCI_GetScoBuffer(r, h, l) RXBUFF_Alloc(r, h, l, HCI_BUFTYPE_SCO_DATA)

/*---------------------------------------------------------------------------
 * HCI_GetEventBuffer()
 *
 *     Called by the transport driver to get an available Event receive buffer.
 *     Once received data is placed in the buffer, it should be returned
 *     to the HCI for processing by calling HCI_RxBufferInd().
 *     This call may be issued at interrupt time or during execution of
 *     the high-priority driver thread (see OS_StopHardware).
 *
 * Parameters:
 *     rxBuffHandle - A pointer to receive the handle of an initialized
 *                    HCI buffer.
 *
 *     len - The desired length of the receive buffer.  The length must
 *           be the exact length of the data that will be received.
 *
 * Returns:
 *           BT_STATUS_SUCCESS - A buffer is available and can be accessed
 *               by "rxBuffHandle".
 *
 *           BT_STATUS_NO_RESOURCES - No buffer is currently available.
 *
 *           BT_STATUS_INVALID_PARM - The requested length was too long, or
 *               the specified type was invalid.
 */
BtStatus HCI_GetEventBuffer(RxBuffHandle *rxBuffHandle, U16 len);
#define HCI_GetEventBuffer(r, l) RXBUFF_Alloc(r, 0, l, HCI_BUFTYPE_EVENT)

/*---------------------------------------------------------------------------
 * HCI_SetBufferLen()
 *
 *     Adjusts the length of the HCI receive buffer.  Receive buffer sizes
 *     are normally set at the time the buffer is allocated.  In some
 *     circumstances, however, the length of the received data is not known
 *     at the time the buffer is allocated.  In these instances, a buffer can
 *     be allocated with any length and then adjusted (reallocated) when the
 *     length is known.  The new length must not exceed the maximum receive
 *     buffer size (TRANS_MAX_ACL_BUFF_SIZE, TRANS_MAX_SCO_BUFF_SIZE, or
 *     TRANS_EVENT_BUFF_SIZE, depending on the buffer type).
 *
 * Parameters:
 *     rxBuffHandle - An initialized receive buffer pointer.
 *
 *     length - The new length of the packet.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The length was changed successfully.
 *
 *     BT_STATUS_FAILED - The function is not available, because the HCI
 *         is not initialized.
 *
 *     BT_STATUS_INVALID_PARM - The length is greater than the maximum
 *         receive buffer size.
 */
BtStatus HCI_SetBufferLen(RxBuffHandle rxBuffHandle, U16 len);
#define HCI_SetBufferLen(r, l) RXBUFF_SetLength(r, l)

/*---------------------------------------------------------------------------
 * HCI_GetRxPtr()
 *
 *    Returns the memory pointer associated with the RxBuffHandle.
 *
 * Parameters:
 *     rxBuffHandle - An initialized receive buffer pointer.
 *
 * Returns:
 *     A pointer to memory.
 */
U8 * HCI_GetRxPtr(RxBuffHandle rxBuffHandle);
#define HCI_GetRxPtr(r) ((r)->buffer)

/*---------------------------------------------------------------------------
 * HCI_RxBufferInd()
 *
 *     Called by the transport driver to indicate that data was
 *     received. This call may be issued at interrupt time or during
 *     execution of the high-priority driver thread (see OS_StopHardware).
 *     This function also frees the buffer, so it may not be used
 *     again.
 *
 * Parameters:
 *     buffer - A handle to the receive buffer that was acquired by calling
 *              HCI_Get[Acl/Sco/Event]Buffer().
 *
 *     status - A status to indicate whether data was successfully copied
 *         into the receive buffer.  Possible values are:
 *
 *         BT_STATUS_SUCCESS - Data was successfully received and placed
 *             into the receive buffer.  The data will be passed up to the
 *             appropriate destination.
 *
 *         BT_STATUS_FAILED - If a transmission error occurs after
 *             a buffer has been acquired, then a failed status should be
 *             indicated.  The buffer will be freed, but it is assumed that
 *             the data portion of the RxBuffHandle is invalid.
 */
void HCI_RxBufferInd(RxBuffHandle rxBuffHandle, BtStatus status);
#define HCI_RxBufferInd(r, s) HciBufferInd(r, s)


/*---------------------------------------------------------------------------
 * HCI_PacketSent()
 *
 *     Called by the transport driver to indicate that an HCI packet
 *     was successfully transmitted to the host controller. This call may
 *     be issued at interrupt time or during execution of the high-priority
 *     driver thread (see OS_StopHardware).
 *
 * Parameters:
 *     packet - An HCI packet previously provided by tranEntry.sendData().
 *         After this call, the fields and data in "packet" must not be
 *         modified by the transport driver.
 */
void HCI_PacketSent(HciPacket *packet);
#define HCI_PacketSent(p) HciPacketSent(p)


/*---------------------------------------------------------------------------
 * HCI_TransportError()
 *
 *     Indicates that the transport layer has detected an error. The
 *     stack will track only the last error received.  This call may
 *     be issued at interrupt time or during execution of the high-priority
 *     driver thread (see OS_StopHardware).
 *
 * Parameters:
 *     void
 */
void HCI_TransportError(HciTransError error);
#define HCI_TransportError(e) HciTransportError(e)

/****************************************************************************
 *
 * Section: Functions Implemented by HCI Transports
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * TRAN_Init()
 *
 *     Called by the HCI driver to initialize the transport.  This
 *     initialization can happen synchronously or asynchronously.  This
 *     function must be implemented by the transport driver.
 *
 * Parameters:
 *     tranCallback - the callback function that must be called to inform
 *                    the HCI of the status of the initialization or
 *                    shutdown if it will complete asynchronously. (See
 *                    TranCallback and TranEvent).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - the initialization completed synchronously (the
 *                         callback function will not be called.
 *
 *     BT_STATUS_FAILED - the initialization could not be performed (the
 *                        callback function will not be called).
 *
 *     BT_STATUS_PENDING - the initialization will complete asynchronously
 *                         and the callback function will be called when
 *                         initialization is finished.
 */
BtStatus TRAN_Init(TranCallback tranCallback);

/*---------------------------------------------------------------------------
 * TRAN_Shutdown()
 *
 *     Called by the HCI driver to shutdown the transport.  This
 *     shutdown can happen synchronously or asynchronously.  This function
 *     must be implemented by the transport driver.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     BT_STATUS_SUCCESS - the shutdown completed synchronously.  No callback
 *                         will be made.
 *
 *     BT_STATUS_FAILED - the shutdown could not be performed (the
 *                        callback function will NOT be called).
 *
 *     BT_STATUS_PENDING - the shutdown will complete asynchronously
 *                         and the callback function will be called when
 *                         shutdown is finished.
 */
BtStatus TRAN_Shutdown(void);

/*---------------------------------------------------------------------------
 * TRAN_SendData()
 *
 *     Called by the HCI driver to provide commands and data to be sent to
 *     the host controller.  The function entry point is passed to the HCI
 *     through the HCI_RegisterTransport() function.  Therefore, it is not
 *     necessary to name this function "TRAN_SendData."  In a case where
 *     mutliple transports are compiled into a single image with the
 *     stack, each "TRAN_SendData" function must have a unique name.
 *
 * Parameters:
 *     packet - contains a pointer to the HCI packet that must be sent.
 *
 * Returns:
 *     void - if transmission fails for some reason, the error is indicated
 *            to the HCI by calling HCI_TransportError().
 */
void TRAN_SendData(HciPacket *packet);

/*---------------------------------------------------------------------------
 * TRAN_BufferAvailable()
 *
 *     Called by the HCI driver to indicate that received data has been
 *     consumed and a buffer is available.  This function can be used to
 *     restart the read state machine when data is available, but no buffer
 *     was available to receive it.  The 'buffType' flag contains the type
 *     of buffer available.  One of the following flags will be set in the
 *     'bufType' parameter:  HCI_BUFTYPE_EVENT, HCI_BUFTYPE_ACL_DATA, or
 *     HCI_BUFTYPE_SCO_DATA.
 *
 * Parameters:
 *     buffType - indicates the type of buffer available.
 *
 * Returns:
 *     void
 */
void TRAN_BufferAvailable(HciBufFlags buffType);

/*---------------------------------------------------------------------------
 * TRAN_SetSpeed()
 *
 *     Called by the HCI driver to set the speed of the transport.  The
 *     function entry point is passed to the HCI through the
 *     HCI_RegisterTransport() function.  Therefore, it is not
 *     necessary to name this function "TRAN_SendData."  In a case where
 *     mutliple transports are compiled into a single image with the
 *     stack, each "TRAN_SetSpeed" function must have a unique name.
 *
 * Parameters:
 *     speed - this value is implementation specific, but generally means
 *             baud rate.
 *
 * Returns:
 *     void
 */
void TRAN_SetSpeed(U16 speed);

/*---------------------------------------------------------------------------
 * TRAN_NumScoCons()
 *
 *     Called by the HCI driver to indicate the number of active SCO
 *     connections.  The function entry point is passed to the
 *     HCI through the HCI_RegisterTransport() function.  Therefore, it is
 *     not necessary to name this function "TRAN_SendData."  In a case where
 *     mutliple transports are compiled into a single image with the stack,
 *     each "TRAN_NumScoCons" function must have a unique name.
 *
 * Parameters:
 *     speed - this value is implementation specific, but generally means
 *             baud rate.
 *
 * Returns:
 *     void
 */
void TRAN_NumScoCons(U16 numScoCons);

/*---------------------------------------------------------------------------
 * TRAN_StartTimer()
 *
 *     Called by transport layers to start a timer. After the specified
 *     amount of time elapses, the timer fires by calling the timer's
 *     callback function. The timer fires only once for each call to
 *     TRAN_StartTimer().
 *
 *     If the timer was already running, this function resets it with a
 *     new time period.
 *
 *     Internally, timers are managed by calling OS_GetSystemTime during
 *     stack execution. When XA_MULTITASKING is enabled, this function
 *     uses OS_StartTimer, which maps to a single system timer.
 *
 * Parameters:
 *     timer - Pointer to a timer structure. The caller must
 *         initialize the func field, and may also initialize the context
 *         field if desired. When the timer fires, timer.func is called
 *         during stack execution. NOTE: The timer.func will typically check
 *         for Hold, Park or Sniff mode with ME_GetCurrentMode when called
 *         and handle these modes differently (usually restarting the timer
 *         is sufficient.)
 *
 *     ms - Number of milliseconds to wait before firing. This value
 *         is converted to system ticks using the BT_MS_TO_TICKS macro.
 */
void TRAN_StartTimer(EvmTimer* timer, TimeT ms);
#define TRAN_StartTimer(timer, ms) EvmStartTimer(timer, BT_MS_TO_TICKS(ms), FALSE)

/*---------------------------------------------------------------------------
 * TRAN_CancelTimer()
 *
 *     Called by the transport to cancel a timer. This prevents the timer
 *     from firing.
 *
 * Parameters:
 *     timer - Pointer to a timer structure. The timer must have been
 *         started by TRAN_StartTimer and must not have fired or
 *         been cancelled already.
 */
void TRAN_CancelTimer(EvmTimer* timer);
#define TRAN_CancelTimer(timer) EvmCancelTimer(timer, FALSE)

/****************************************************************************
 *
 * Section: HCI Functions Called by Radio Module Initialization Code
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HCI_SetSpeed()
 *
 *     Called by the Radio Module Initialization code to set the speed of
 *     the transport.
 *
 * Parameters:
 *     speed - this value is implementation specific, but generally means
 *             baud rate.  This function is called by radio specific
 *             code in the case the baud rate must be changed.
 *
 * Returns:
 *     void
 */
void HCI_SetSpeed(U16 Speed);

/*---------------------------------------------------------------------------
 * HCI_SendRadioCommand()
 *
 *     Starts sending a command to the Radio Module. After the command
 *     is completely sent, HCI calls RADIO_EventHandler with the
 *     HCI_COMMAND_HANDLED event type (see sys/hci.h>. The callback
 *     parameters contain the command that was sent (in "ptr.cmd") and
 *     the success of the operation (in "status").
 *
 *     In response to the command, the Radio Module will send one
 *     or more events. These events are also passed to the RADIO_EventHandler
 *     function as described by the HciCallbackType HCI_CONTROLLER_EVENT
 *     type (see sys/hci.h>.
 *
 *     Before calling this function, the command's "parms" field must
 *     be filled with the appropriate command-specific parameters as
 *     defined in the Bluetooth specification. Filling the "header"
 *     field is not required.
 *
 * Parameters:
 *    opCode - The type of HCI command to send.
 *
 *    parmLen - The length of the command parameters.  The command
 *        parameter block is located in the HciCommand structure.
 *
 *    cmd - the command structure. The "cmd->parm" field must be filled
 *        with command-specific parameters.
 *
 * Returns:
 *    BT_STATUS_PENDING - the command was accepted by the HCI. Notification
 *        will be sent to RADIO_EventHandler at a later time.
 *
 *    BT_STATUS_INVALID_PARM - the "cmd" parameter is not valid
 *        (XA_ERROR_CHECK only).
 */
BtStatus HCI_SendRadioCommand(HciCommandType opCode, U8 parmLen, HciCommand *cmd);
#define HCI_SendRadioCommand(o, p, c) HciSendCommand(o, p, c, TRUE)

/*---------------------------------------------------------------------------
 * HCI_SetFlowParms()
 *
 *     Called to set flow control parameters (transmit only) of the
 *     HCI transport.
 *
 * Parameters:
 *     Parms - A pointer to a memory structure that contains the flow
 *         control information.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The request was successful
 *
 *     BT_STATUS_INVALID_PARMS - Invalid parameters.
 */
BtStatus HCI_SetFlowParms(HciFlowParms *Parms);

/****************************************************************************
 *
 * Section: Functions Implemented by Radio Module Initialization Code
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * RADIO_Init()
 *
 *     Called by the HCI to begin Radio Module
 *     initialization.  Radio module initialization is accomplished
 *     by sending vendor specific or other standard HCI commands.  This
 *     function is called after the transport initialized.
 *
 *     HCI commands are sent one at a time to the radio module.  Once
 *     a command has completed (COMMAND_COMPLETE or the equivalent is
 *     received), a new command can be sent.  Standard HCI commands and
 *     events are documented in the Bluetooth specification.  Vendor
 *     specific commands and events are defined by the module manufacturer.
 *
 *     The RADIO_Init code is responsible for doing an HCI Reset for HCI
 *     initialization. Usually this is the first command to be sent in
 *     RADIO_Init, but each radio module may have different constraints
 *     and requirements.
 *
 *     The "Callback" parameter contains a pointer to a callback function
 *     for reporting initialization progress (see RadioCallback).  If no
 *     initialization is necessary for the Radio Module, or if
 *     initialization is completed during the call to RADIO_Init, RADIO_Init
 *     must return BT_STATUS_SUCCESS, and the callback function must not
 *     be called.  In most cases initialization is required and RADIO_Init
 *     must return BT_STATUS_PENDING, and the callback function must be
 *     called when initialization is complete.  If initialization cannot be
 *     started or failed during the call to RADIO_Init, then RADIO_Init must
 *     return BT_STATUS_FAILED. When initialization is complete the RadioCallback
 *     must be called to indicate to the stack that operation may proceed.
 *
 * Parameters:
 *     Callback - the callback function that must be called to inform
 *                the HCI of the status of the initialization or
 *                shutdown if it will complete asynchronously. (See
 *                RadioCallback and RadioEvent).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - the initialization completed synchronously (the
 *                         callback function will not be called.
 *
 *     BT_STATUS_FAILED - the initialization could not be performed (the
 *                        callback function will not be called).
 *
 *     BT_STATUS_PENDING - the initialization will complete asynchronously
 *                         and the callback function will be called when
 *                         initialization is finished.
 */
BtStatus RADIO_Init(RadioCallback Callback);

/*---------------------------------------------------------------------------
 * RADIO_Shutdown()
 *
 *     Called by the HCI to begin shutdown of the Radio Module.
 *     Radio module shutdown is accomplished by sending vendor specific
 *     or other standard HCI commands.  This function is called before
 *     the transports and the HCI are shut down.
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     BT_STATUS_SUCCESS - the initialization completed synchronously (the
 *                         callback function will not be called.
 *
 *     BT_STATUS_FAILED - the initialization could not be performed (the
 *                        callback function will not be called).
 *
 *     BT_STATUS_PENDING - the initialization will complete asynchronously
 *                         and the callback function will be called when
 *                         initialization is finished.
 */
BtStatus RADIO_Shutdown(void);

/*---------------------------------------------------------------------------
 * RADIO_EventHandler()
 *
 *     Called by the HCI to indicate HCI events to the Radio
 *     Module initialization or shutdown code.  These events are
 *     generated in response to commands sent from the initialization
 *     or shutdown code.  The commands and events used conform to the
 *     HCI API interface defined in the HCI_SendCommand function (see
 *     sys/hci.h).
 *
 * Parameters:
 *     Event - contains the event generated by the HCI (see HciEventtype in
 *             sys/hci.h)
 *
 *     Parms - contains the callback parameters of the vent (see
 *             HciCallbackParms in sys/hci.h)
 *
 * Returns:
 *     void
 */
void RADIO_EventHandler(HciEventType Event, HciCallbackParms *Parms);

#endif /* __HCITRANS_H */


