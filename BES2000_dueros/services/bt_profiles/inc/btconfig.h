/****************************************************************************
 *
 * File:
 *     $Id: btconfig.h 1399 2009-04-15 20:01:20Z benbrown $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 1399 $
 *
 * Description:
 *     Default constants for configuring the Bluetooth protocol stack.
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

#ifndef __BTCONFIG_H
#define __BTCONFIG_H

#include "config.h"

#if !defined(BT_STACK) || BT_STACK == XA_DISABLED
#error "Bluetooth Stack must be enabled!"
#endif

/*---------------------------------------------------------------------------
 * Bluetooth Configuration API layer
 *
 *     The constants in this layer configure the layers of the Bluetooth
 *     stack. To change a constant, simply #define it in your overide.h
 *     include file.
 *
 *     Configuration constants here and in the General Configuration API
 *     (config.h) are used to make the stack more appropriate for a
 *     particular environment. Constants can be modified to allow tradeoffs
 *     between code size, RAM usage, functionality, and throughput. 
 *
 *     Some constants are numeric, and others indicate whether a feature
 *     is enabled (defined as XA_ENABLED) or disabled (defined as
 *     XA_DISABLED).
 */

/****************************************************************************
 *
 * Section: General Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BT_STACK_VERSION
 *     Defines the stack's version number.
 */
#define BT_STACK_VERSION 313

/*---------------------------------------------------------------------------
 * NUM_BT_DEVICES constant
 *
 *     The number of devices with which we can connect. This value
 *     represents the maximum size of the piconet in which this device is
 *     the master, plus any master devices for which we are concurrently a
 *     slave. This value includes devices which may be parked, holding,
 *     or sniffing as well any active devices.
 *
 *     The default value is 2.
 */
#ifndef NUM_BT_DEVICES
#define NUM_BT_DEVICES  2
#endif

#if NUM_BT_DEVICES < 1
#error NUM_BT_DEVICES must be greater than 0
#endif

#if NUM_BT_DEVICES > 255
#error NUM_BT_DEVICES must be less than 256
#endif

/*---------------------------------------------------------------------------
 * NUM_SCO_CONNS constant
 * 
 *     Defines the number of SCO connections supported by this device.
 *     When NUM_SCO_CONNS is set to 0, SCO capabilities are disabled
 *     to save code size.
 *
 *     The default value is 0, and the maximum is 3.
 */
#ifndef NUM_SCO_CONNS
#define NUM_SCO_CONNS 2
#endif

#if NUM_SCO_CONNS > 3
#error NUM_SCO_CONNS must be 3 or less
#endif

/****************************************************************************
 *
 * Section: Management Entity Constants
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * BT_ALLOW_SCAN_WHILE_CON constant
 *
 *     Determines whether page and inquiry scans are allowed during a
 *     connection. Performing Page and Inquiry scans while connected takes
 *     bandwidth away from the connections, so it should only be used in
 *     circumstances where scatternets or master/slave switching is required
 *     (for instance, when implementing a LAN access port).
 *
 *     When BT_ALLOW_SCAN_WHILE_CON is XA_ENABLED, the Management Entity
 *     includes code for page and inquiry scans during a connection. When
 *     it is XA_DISABLED, scans are not allowed during a connection.
 *
 *     By default, this constant is disabled.
 */
#ifndef BT_ALLOW_SCAN_WHILE_CON
#define BT_ALLOW_SCAN_WHILE_CON XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * BT_SCO_HCI_DATA constant
 *
 *     Determines whether code to send and receive SCO is included. This 
 *     constant should be defined only when SCO audio data is sent through
 *     the HCI interface. SCO connections are always supported when 
 *     NUM_SCO_CONNS is greater than zero.
 *
 *     By default, this constant is disabled.
 */
#ifndef BT_SCO_HCI_DATA
#define BT_SCO_HCI_DATA XA_DISABLED
#endif

#if BT_SCO_HCI_DATA == XA_ENABLED && NUM_SCO_CONNS == 0 
#error NUM_SCO_CONNS must be > 0 when BT_SCO_HCI_DATA is enabled
#endif

/*---------------------------------------------------------------------------
 * BT_SCO_HCI_NUM_PACKETS constant
 *
 *     Represents the number of HCI packets allocated for sending SCO data to
 *     the Host Controller. These packets are used for SCO data only. If the
 *     HCI layer runs out of SCO packets, no more data will be sent until the
 *     transport layer returns one.
 *
 *     The default number of packets is 20 for each potential SCO connection.
 */
#ifndef BT_SCO_HCI_NUM_PACKETS
#define BT_SCO_HCI_NUM_PACKETS  (20 * NUM_SCO_CONNS)
#endif

/*---------------------------------------------------------------------------
 * BT_SCO_USE_LEGACY_CONNECT constant
 *
 *     Determines whether code to create SCO connections uses the new
 *     HCC_SETUP_SYNC_CONNECTION command or the legacy HCC_ADD_SCO_CONNECTION
 *     command when SCO connections are created.  Some radios have difficulty
 *     negotiating a SCO connection using the HCC_SETUP_SYNC_CONNECTION command.
 *     Enabling this option will force the stack to use the legacy command for
 *     creating a SCO connection.  Certain versions of the CSR firmware have 
 *     this problem.
 *
 *     By default, this constant is disabled.
 */
#ifndef BT_SCO_USE_LEGACY_CONNECT
#define BT_SCO_USE_LEGACY_CONNECT XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * BT_SECURITY constant
 *
 *     Determines whether code to handle BT security features is included. 
 *     This constant is typically enabled, but may be disabled for minimal
 *     applications that don't need security.
 *
 *     By default, this constant is enabled.
 */
#ifndef BT_SECURITY
#define BT_SECURITY XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * BT_SECURITY_TIMEOUT constant
 *
 *     Specifies the number of seconds to elapse before a service-specific
 *     access request (see SEC_AccessRequest) is automatically cancelled.
 *     The stack's security manager will cancel the request regardless of
 *     what is causing the delay. Cancelling the request will cause the
 *     security request to fail, which in most cases will prevent the
 *     requesting service from connecting.
 *
 *     This value does not affect authentication or encryption operations
 *     requested directly through APIs such as SEC_AuthenticateLink or
 *     SEC_SetLinkEncryption; it only affects operations initiated through
 *     SEC_AccessRequest.
 *
 *     Set this value to 0 to disable security timeouts. When disabled, it
 *     may be necessary to drop the ACL connection in order to safely cancel
 *     the security request.
 *
 *     By default, this value is set to 80 seconds. Although any time value
 *     may be used, sufficient time should be allowed for the user of both
 *     the local and remote devices to enter PIN codes and select
 *     authorization settings if required.
 */
#ifndef BT_SECURITY_TIMEOUT
#define BT_SECURITY_TIMEOUT 80
#endif /* BT_SECURITY_TIMEOUT */


/*---------------------------------------------------------------------------
 * BT_DEFAULT_PAGE_SCAN_WINDOW constant
 *
 *     The following are used as the default values for settings in the radio
 *     module and must be 0 or match the radio defaults. When an application
 *     sets specific values for radio settings then returns to the default
 *     settings of the radio it is necessary to set the radio defaults. When
 *     setting the radio defaults these macros are used. Therefore, these
 *     values must be set appropriately for the given radio. If the values
 *     are set to 0 then that implies that these values cannot be changed
 *     for the given radio. If the values are set to 0 the API will prevent
 *     a application from attempting to change the values.
 */
#ifndef BT_DEFAULT_PAGE_SCAN_WINDOW
#define BT_DEFAULT_PAGE_SCAN_WINDOW 0
#endif

/*---------------------------------------------------------------------------
 * BT_DEFAULT_PAGE_SCAN_INTERVAL constant
 *
 *     See BT_DEFAULT_PAGE_SCAN_WINDOW.
 */
#ifndef BT_DEFAULT_PAGE_SCAN_INTERVAL
#define BT_DEFAULT_PAGE_SCAN_INTERVAL 0
#endif

/*---------------------------------------------------------------------------
 * BT_DEFAULT_INQ_SCAN_WINDOW constant
 *
 *     See BT_DEFAULT_PAGE_SCAN_WINDOW.
 */
#ifndef BT_DEFAULT_INQ_SCAN_WINDOW
#define BT_DEFAULT_INQ_SCAN_WINDOW 0
#endif

/*---------------------------------------------------------------------------
 * BT_DEFAULT_INQ_SCAN_INTERVAL constant
 *
 *     See BT_DEFAULT_PAGE_SCAN_WINDOW.
 */
#ifndef BT_DEFAULT_INQ_SCAN_INTERVAL
#define BT_DEFAULT_INQ_SCAN_INTERVAL 0
#endif

/* Verify that the values are in the legal range. Note that these macros
 * cannot be broken over more than one line or some compilers break (gcc) */
#define BT_PSW_TEST ((BT_DEFAULT_PAGE_SCAN_WINDOW != 0) && ((BT_DEFAULT_PAGE_SCAN_WINDOW < 0x0012) || (BT_DEFAULT_PAGE_SCAN_WINDOW > 0x1000)))

#if BT_PSW_TEST
#error BT_DEFAULT_PAGE_SCAN_WINDOW is not legal
#endif

#define BT_PSI_TEST ((BT_DEFAULT_PAGE_SCAN_INTERVAL != 0) && ((BT_DEFAULT_PAGE_SCAN_INTERVAL < 0x0012) || (BT_DEFAULT_PAGE_SCAN_INTERVAL > 0x1000)))

#if BT_PSI_TEST
#error BT_DEFAULT_PAGE_SCAN_INTERVAL is not legal
#endif

#define BT_ISW_TEST ((BT_DEFAULT_INQ_SCAN_WINDOW != 0) && ((BT_DEFAULT_INQ_SCAN_WINDOW < 0x0012) || (BT_DEFAULT_INQ_SCAN_WINDOW > 0x1000)))

#if BT_ISW_TEST
#error BT_DEFAULT_INQ_SCAN_WINDOW is not legal
#endif

#define BT_ISI_TEST ((BT_DEFAULT_INQ_SCAN_INTERVAL != 0) && ((BT_DEFAULT_INQ_SCAN_INTERVAL < 0x0012) || (BT_DEFAULT_INQ_SCAN_INTERVAL > 0x1000)))

#if BT_ISI_TEST
#error BT_DEFAULT_INQ_SCAN_INTERVAL is not legal
#endif

/*---------------------------------------------------------------------------
 * BT_DEFAULT_ACCESS_MODE_NC constant
 *
 *     Specifies the default accessibility mode when not-connected that
 *     the ME should set the radio to during initialization. This value
 *     must be of type BtAccessibleMode, found in me.h.
 */
#ifndef BT_DEFAULT_ACCESS_MODE_NC
#define BT_DEFAULT_ACCESS_MODE_NC  BAM_GENERAL_ACCESSIBLE
#endif /* BT_DEFAULT_ACCESS_MODE_NC */

/*---------------------------------------------------------------------------
 * BT_DEFAULT_ACCESS_MODE_C constant
 *
 *     Specifies the default accessibility mode when connected that the
 *     ME should set the radio to during initialization. This value
 *     must be of type BtAccessibleMode, found in me.h.
 *
 *     This default is applied only when BT_ALLOW_SCAN_WHILE_CON is XA_ENABLED.
 */
#ifndef BT_DEFAULT_ACCESS_MODE_C
#define BT_DEFAULT_ACCESS_MODE_C  BAM_NOT_ACCESSIBLE
#endif /* BT_DEFAULT_ACCESS_MODE_C */

/*---------------------------------------------------------------------------
 * BT_HCI_NUM_INIT_RETRIES constant
 *
 *     Specifies the number of times the Radio Manager should retry the 
 *     initialization of HCI when it fails to initialize.
 */
#ifndef BT_HCI_NUM_INIT_RETRIES
#define BT_HCI_NUM_INIT_RETRIES 0xFF
#endif

/*---------------------------------------------------------------------------
 * BT_DEFAULT_PAGE_TIMEOUT constant
 *
 *     The BT_DEFAULT_PAGE_TIMEOUT constant is the maximum time the
 *     local Link Manager will wait for a response from the remote
 *     device on a connection attempt. The default is 0x2000 = 5.12 sec.
 */
#ifndef BT_DEFAULT_PAGE_TIMEOUT
#define BT_DEFAULT_PAGE_TIMEOUT 0x2000
#endif

/*---------------------------------------------------------------------------
 * BT_PACKET_HEADER_LEN constant
 *
 *     The BT_PACKET_HEADER_LEN constant is the size of the BtPacket header.
 *     The default size is the max(L2CAP) header size (8 bytes), plus the
 *     max(RFCOMM,SDP,TCS) header size (6 bytes). If BNEP is used, you must
 *     increase the BT_PACKET_HEADER_LEN by 5 bytes: (8+6+5) in your config
 *     overide file.
 */
#ifndef BT_PACKET_HEADER_LEN
#define BT_PACKET_HEADER_LEN (8+6)
#endif

#if BT_PACKET_HEADER_LEN < 14 || BT_PACKET_HEADER_LEN > 255
#error BT_PACKET_HEADER_LEN must be greater than 13 and less than 256
#endif

/*---------------------------------------------------------------------------
 * NUM_KNOWN_DEVICES constant
 *
 *     Defines the maximum number of devices that the ME Device Selection
 *     manager can track. If this value is zero, the MEDEV component is
 *     disabled, resulting in a code size savings.
 */
#ifndef NUM_KNOWN_DEVICES
#define NUM_KNOWN_DEVICES       20
#endif

/*---------------------------------------------------------------------------
 * DS_NUM_SERVICES constant
 *
 *     This constant represents the maximum number of services that can
 *     be tracked within a BtDeviceContext. Each time a successful service
 *     search attribute request is performed, the UUID of the found service 
 *     is stored in the device context. This list of services is used for
 *     selecting the best device for a specific service type. If this
 *     constant is defined to 0, service tracking is disabled and the
 *     supporting code is eliminated from the build.
 */
#ifndef DS_NUM_SERVICES
#define DS_NUM_SERVICES         4
#endif


/****************************************************************************
 *
 * Section: HCI Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HCI_RESET_TIMEOUT constant
 *
 *     Specifies the amount of time that the HCI will wait for a response
 *     to the RESET command before indicating an error to the Radio Manager.
 *     When the Radio Manager detects the error, it will attempt to reset
 *     the transports and the HCI again.
 *
 *     The default is 10 seconds.
 */
#ifndef HCI_RESET_TIMEOUT
#define HCI_RESET_TIMEOUT 10000
#endif

/*---------------------------------------------------------------------------
 * HCI_NUM_PACKETS constant
 *
 *     Represents the number of HCI packets allocated for sending ACL data to
 *     the Host Controller.  If the HCI layer runs out of packets, no more data
 *     will be sent until the transport layer returns one.
 *
 *     The default number of packets for ACL data is 2 for each potential
 *     connection.  See HCI_NUM_COMMANDS to define the number packets
 *     allocated for commands, and BT_SCO_HCI_NUM_PACKETS to define the number
 *     of packets allocated for SCO data.
 */
#ifndef HCI_NUM_PACKETS
#define HCI_NUM_PACKETS  (2 * NUM_BT_DEVICES)
#endif

#if HCI_NUM_PACKETS < (2 * NUM_BT_DEVICES)
#error HCI_NUM_PACKETS should be greater than or equal to (2 * NUM_BT_DEVICES)
#endif


/*---------------------------------------------------------------------------
 * HCI_NUM_COMMANDS constant
 *
 *     Represents the number of HCI packets allocated for sending commands
 *     to the host controller.
 *
 *     The default number of packets is 1. 
 */
#ifndef HCI_NUM_COMMANDS
#define HCI_NUM_COMMANDS 1
#endif


/*---------------------------------------------------------------------------
 * HCI_NUM_EVENTS constant
 *
 *     Represents the number of HCI buffers allocated for handling events
 *     from the host controller.
 *
 *     The default number of packets is 2. 
 */
#ifndef HCI_NUM_EVENTS
#define HCI_NUM_EVENTS 2
#endif


/*---------------------------------------------------------------------------
 * HCI_CMD_PARM_LEN constant
 *
 *     Represents the largest size allowed for HCI command parameters.
 *
 *     The default command parameter length is 248.
 */
#ifndef HCI_CMD_PARM_LEN
#define HCI_CMD_PARM_LEN 248
#endif

/* The longest known HCI command is 248 bytes at this time.
 */
#if HCI_CMD_PARM_LEN < 248
#error HCI_CMD_PARM_LEN must be at least 248
#endif


/*---------------------------------------------------------------------------
 * HCI_HOST_FLOW_CONTROL constant
 *
 *     Controls whether HCI applies flow control to data received from the
 *     host controller.
 *
 *     When HCI_HOST_FLOW_CONTROL is enabled, the HCI layer uses
 *     HCC_HOST_NUM_COMPLETED_PACKET commands to periodically tell the
 *     radio hardware how many receive packets it can accept. This is
 *     necessary if the HCI driver's receive buffers could overflow
 *     with incoming data.
 *
 *     When HCI_HOST_FLOW_CONTROL is disabled, the HCI driver assumes
 *     that it can process data faster than the radio hardware can generate
 *     it.
 *
 *     By default, host-side flow control is disabled.
 */
#ifndef HCI_HOST_FLOW_CONTROL
#define HCI_HOST_FLOW_CONTROL XA_DISABLED
#endif

/*-------------------------------------------------------------------------
 * HCI_MAX_COMPLETED_PKTS constant
 *
 *     Defines the maximum number of receive buffers that will be used
 *     before an HCC_HOST_NUM_COMPLETED_PACKET command is sent to the
 *     controller.
 *
 *     By default, the maximum is 8 completed packets.
 */
#ifndef HCI_MAX_COMPLETED_PKTS
#if HCI_HOST_FLOW_CONTROL == XA_ENABLED
#define HCI_MAX_COMPLETED_PKTS 8
#endif
#endif

/*---------------------------------------------------------------------------
 * HCI_SCO_FLOW_CONTROL constant
 *
 *     Controls whether the host controller applies flow control to SCO data 
 *     sent by the host.
 *
 *     When HCI_SCO_FLOW_CONTROL is enabled, flow control is applied to SCO
 *     data transmitted to the host controller.
 * 
 *     When HCI_SCO_FLOW_CONTROL is disabled, no flow control mechanism is
 *     used when transmitting data to the host controller. 
 *
 *     By default, SCO flow control is disabled.
 */
#ifndef HCI_SCO_FLOW_CONTROL
#define HCI_SCO_FLOW_CONTROL XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * HCI_ALLOW_PRESCAN constant
 *
 *     Controls whether prescan handlers can be registered with HCI.  Prescan
 *     handlers allow receive and transmit data to be viewed or modified 
 *     before being sent or received by the stack.
 *
 *     When HCI_ALLOW_PRESCAN is enabled, the functions for registering
 *     prescan handlers are included in the code.
 *
 *     When HCI_ALLOW_PRESCAN is disabled, the functions for registering
 *     prescan handlers are not included in the code.
 */
#ifndef HCI_ALLOW_PRESCAN
#define HCI_ALLOW_PRESCAN XA_ENABLED
#endif

/*-------------------------------------------------------------------------
 * HCI_NUM_ACL_TX_RESERVE constant
 *
 *     Defines the number of TX packets to hold in reserve to prevent
 *     resource depletion by any one HCI channel. A single ACL channel
 *     may use all but HCI_NUM_ACL_TX_RESERVE packets from the TX
 *     packet pool. Use caution when setting this value; as there
 *     must be sufficient transmit packets allocated by HCI_NUM_PACKETS.
 *
 *     By default, 1 ACL TX packet is reserved.
 *     Set this to 0 to disable this option.
 */
#ifndef HCI_NUM_ACL_TX_RESERVE
#define HCI_NUM_ACL_TX_RESERVE  1
#endif

/*-------------------------------------------------------------------------
 * HCI_NUM_UNCONN_RESERVE constant
 *
 *     Defines the number of TX packets to hold in reserve for unconnected
 *     devices. This setting ensures HCI_NUM_UNCONN_RESERVE packets will
 *     always be available for each device that is not yet connected. This
 *     value is calculated from NUM_BT_DEVICES less the number of devices
 *     currently connected. Use caution when setting this value; as there
 *     must be sufficient transmit packets allocated by HCI_NUM_PACKETS.
 *
 *     By default, 1 ACL TX packet is reserved per unconnected device.
 *     Set this to 0 to disable this option.
 */
#ifndef HCI_NUM_UNCONN_RESERVE
#define HCI_NUM_UNCONN_RESERVE  1
#endif

/*-------------------------------------------------------------------------
 * HCI_ACL_DATA_SIZE constant
 *
 *     Defines the maximum data size of a received ACL packet in the HCI
 *     layer. This value affects the size of receive buffers allocated
 *     by the HCI driver.
 *
 *     This value must be at least 255 and no more than 65531. The default
 *     is 800 bytes.
 */
#ifndef HCI_ACL_DATA_SIZE
#define HCI_ACL_DATA_SIZE 800
#endif

#if HCI_ACL_DATA_SIZE > 65531
#error HCI_ACL_DATA_SIZE cannot be greater than 65531
#endif

#if HCI_ACL_DATA_SIZE < 255
#error HCI_ACL_DATA_SIZE cannot be less than 255
#endif


/*-------------------------------------------------------------------------
 * HCI_SCO_DATA_SIZE constant
 *
 *     Defines the maximum data size of a received SCO packet in the HCI
 *     layer. This value affects the size of receive buffers allocated by
 *     the HCI driver. 
 *
 *     The SCO data size must be less than 256 bytes. The default size is
 *     255 bytes.
 */
#ifndef HCI_SCO_DATA_SIZE
#define HCI_SCO_DATA_SIZE 255
#endif

#if HCI_SCO_DATA_SIZE > 255
#error HCI_SCO_DATA_SIZE must be less than 256
#endif


/*-------------------------------------------------------------------------
 * HCI_NUM_ACL_BUFFERS constant
 *
 *     Defines the number of buffers allocated by the HCI driver
 *     for receiving ACL data.
 *
 *     By default, 8 ACL data buffers are allocated.
 */
#ifndef HCI_NUM_ACL_BUFFERS
#if L2CAP_NUM_ENHANCED_CHANNELS > 0
#define HCI_NUM_ACL_BUFFERS      (L2CAP_ERTM_RX_WIN_SIZE * L2CAP_NUM_ENHANCED_CHANNELS)
#else
#define HCI_NUM_ACL_BUFFERS      8
#endif
#endif

/*-------------------------------------------------------------------------
 * HCI_NUM_SCO_BUFFERS constant
 *
 *     Defines the number of buffers allocated by the HCI driver
 *     for receiving SCO data. If the number of SCO connections allowed
 *     (NUM_SCO_CONNS) is zero, the HCI does not need to
 *     allocate any SCO buffers.
 *
 *     By default, 8 SCO data buffers are allocated.
 */
#ifndef HCI_NUM_SCO_BUFFERS
#define HCI_NUM_SCO_BUFFERS      8
#endif

/*---------------------------------------------------------------------------
 * HCI3W_SLIDING_WINDOW constant
 *
 *     For the HCI 3-Wire Transport Layer code, this value indicates how 
 *     many sliding window slots are allowed by the host.  The controller 
 *     has an independent number of sliding window slots that it supports.  
 *     The lower value of the two sliding window slots from the host and 
 *     the controller is the actual number of sliding window slots that 
 *     will be used.  This define should range from 1 to 7.  Larger 
 *     values require more memory for reliable message retransmission 
 *     buffers.  If the controller also supports them, larger values 
 *     will result in higher payload throughput if the connection to the 
 *     controller is reliable and bug free, due to the reduced need for 
 *     as many acknowledgements.  Smaller values reduce how many messages 
 *     may be outstanding before an acknowledgement is required.  
 */
#ifndef HCI3W_SLIDING_WINDOW
#define HCI3W_SLIDING_WINDOW (3)
#endif

/*---------------------------------------------------------------------------
 * HCI3W_MAX_PAYLOAD constant
 *
 *     For the HCI 3-Wire Transport Layer code, this value indicates the 
 *     maximum payload size before framing and before slip encoding.  The 
 *     maximum payload size should range from around 10 up through 4095.  
 *     Smaller values reduce memory use but limit the maximum message size.  
 */
#ifndef HCI3W_MAX_PAYLOAD
#define HCI3W_MAX_PAYLOAD (0x0FFF)
#endif

/*---------------------------------------------------------------------------
 * HCI3W_MEM_POOL_SIZE constant
 *
 *     For the HCI 3-Wire Transport Layer code, this value indicates 
 *     the size of the memory pool.  If you are using an HCI 3-wire 
 *     sliding window size of 3, this value should be roughly 8 to 16 
 *     times larger than HCI3W_MAX_PAYLOAD, depending on how often 
 *     large messages will be transferred.  Larger sliding window sizes 
 *     require more memory.  To reduce memory usage, you can reduce 
 *     the maximum payload size and reduce the sliding window size.  
 */
#ifndef HCI3W_MEM_POOL_SIZE
#define HCI3W_MEM_POOL_SIZE (0x8000)
#endif

/*---------------------------------------------------------------------------
 * HCI3W_UART_MULTIPLIER constant
 *
 *     For the HCI 3-Wire Transport Layer code, this value indicates how 
 *     much UART speeds are multiplied by the internal UART hardware.  
 *     Multiplying the UART clock allows the hardware to achieve higher 
 *     speeds while allowing legacy software to use the older unclocked 
 *     values.  If the UART hardware being used does not have a clock 
 *     multiplier, then this value should be set to the default value of 1.  
 *     Otherwise, it must be overridden in the overide.h file to the correct 
 *     multiplier value or the calculations of Tmax will not be correct.  If 
 *     Tmax is too large, then some devices will time out operations before 
 *     they are acknowledged, making the HCI 3-wire communication unusable.  
 */
#ifndef HCI3W_UART_MULTIPLIER
#define HCI3W_UART_MULTIPLIER (1)
#endif

/*---------------------------------------------------------------------------
 * HCI3W_TXQ_MAX constant
 *
 *     For the HCI 3-Wire Transport Layer code, this value defines the total 
 *     number of simultaneous outstanding reliable transmission requests 
 *     that can exist.  At most, there can only be as many reliable messages 
 *     queued up as the HCI 3-wire sliding window size.  So setting this 
 *     value to a little over the HCI 3-wire sliding limit is sufficient.  
 *     Note that the HCI interface does not specify whether a message should 
 *     be transmitted reliably or unreliably.  However, the HCI 3-wire 
 *     specification requires that HCI Command Packets, HCI ACL Data Packets, 
 *     and HCI Event Packets always be sent as reliable packets.  The HCI 
 *     3-wire specification also requires that HCI Synchronous Data Packets 
 *     be sent as unreliable packets unless HCI Synchronous Flow Control 
 *     is enabled, in which case they are to be sent as reliable packets.  
 */
#ifndef HCI3W_TXQ_MAX
#define HCI3W_TXQ_MAX (8)
#endif

/*---------------------------------------------------------------------------
 * HCI3W_CRC constant
 *
 *     For the HCI 3-Wire Transport Layer code, this value indicates 
 *     whether CRCs are allowed by the host.  Both the host and the 
 *     controller must allow CRCs in order for them to be actually used.  
 */
#ifndef HCI3W_CRC
#define HCI3W_CRC XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * HCI3W_OOF constant
 *
 *     For the HCI 3-Wire Transport Layer code, this value indicates 
 *     whether out-of-frame software flow control is allowed by the 
 *     host.  Both the host and the controller must allow out-of-frame 
 *     software flow control in order for it to be actually used.  
 */
#ifndef HCI3W_OOF
#define HCI3W_OOF XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * HCI3W_DEBUG_TXQ constant
 *
 *     For the HCI 3-Wire Transport Layer code, this value indicates 
 *     whether reliable retransmission queue information should be reported 
 *     for debugging purposes or not.  
 */
#ifndef HCI3W_DEBUG_TXQ
#define HCI3W_DEBUG_TXQ XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * BT_EXPOSE_BCCMD constant
 *
 *     Exposes an interface to the BCCMD interface supported by CSR chips 
 *     using ABCSP connections.
 *
 *     For more information about the BCCMD interface, refer to the 
 *     documentation provided by CSR.
 *
 *     By default, the BCCMD interface is disabled because most applications 
 *     do not need to use the interface.
 */ 
#define BT_EXPOSE_BCCMD XA_DISABLED

/****************************************************************************
 *
 * Section: L2CAP Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * L2CAP_NUM_PROTOCOLS constant
 *
 *     Defines the number of protocol services that may be layered above
 *     L2CAP.
 *
 *     Between 1 and 255 L2CAP protocol services are allowed. The default
 *     is three: one for the SDP client, one for the SDP server,
 *     and one for RFCOMM. 
 */
#ifndef L2CAP_NUM_PROTOCOLS
#define L2CAP_NUM_PROTOCOLS         3
#endif

#if L2CAP_NUM_PROTOCOLS < 1 || L2CAP_NUM_PROTOCOLS > 255
#error L2CAP_NUM_PROTOCOLS must be between 1 and 255
#endif


/*---------------------------------------------------------------------------
 * L2CAP_NUM_GROUPS constant
 *
 *     Defines the number of broadcast groups supported by L2CAP. When
 *     this value is zero, support for groups is removed from the stack,
 *     reducing code size.
 *
 *     Between 0 and 255 L2CAP groups are allowed. The default number of
 *     groups is zero.
 */
#ifndef L2CAP_NUM_GROUPS
#define L2CAP_NUM_GROUPS            0
#endif

#if L2CAP_NUM_GROUPS > 255
#error L2CAP_NUM_GROUPS must be no greater than 255
#endif


/*---------------------------------------------------------------------------
 * L2CAP_PING_SUPPORT constant
 *
 *     Controls whether the Ping operation (L2CAP_Ping) is supported.
 *     When disabled, the L2CAP_Ping() API is not included in the stack,
 *     resulting in code size savings.
 *
 *     By default, Ping support is enabled.
 */
#ifndef L2CAP_PING_SUPPORT
#define L2CAP_PING_SUPPORT          XA_ENABLED
#endif


/*---------------------------------------------------------------------------
 * L2CAP_GET_INFO_SUPPORT constant
 *
 *     Controls whether the GetInfo operation (L2CAP_GetInfo) is supported.
 *     When disabled, the L2CAP_GetInfo() API is not included in the stack,
 *     resulting in code size savings.
 *
 *     By default, GetInfo support is disabled.
 */
#ifndef L2CAP_GET_INFO_SUPPORT
#define L2CAP_GET_INFO_SUPPORT      XA_DISABLED
#endif


/*---------------------------------------------------------------------------
 * L2CAP_FLEXIBLE_CONFIG constant
 *
 *     Controls whether a L2CAP user is allowed to manage channel
 *     configuration options.
 *
 *     When L2CAP_FLEXIBLE_CONFIG is enabled, the application will receive
 *     events have access to functions which enable it to control the
 *     flow of configuration requests and responses.
 *
 *     When L2CAP_FLEXIBLE_CONFIG is disabled, configuration messages are
 *     handled internally by the protocol. The associated APIs are not
 *     included in the stack, resulting in code size savings.
 *
 *     By default, L2CAP flexible configuration is disabled.
 */
#ifndef L2CAP_FLEXIBLE_CONFIG
#define L2CAP_FLEXIBLE_CONFIG       XA_DISABLED
#endif


/*---------------------------------------------------------------------------
 * L2CAP_RTX_TIMEOUT constant
 *
 *     Defines the response timeout value for signaling commands, in
 *     seconds. If the timer elapses without a response from the remote
 *     device, the channel is disconnected.
 *
 *     The timeout value may range from 1 to 60 seconds. The default value
 *     is 30 seconds.
 */
#ifndef L2CAP_RTX_TIMEOUT
#define L2CAP_RTX_TIMEOUT           30
#endif

#if L2CAP_RTX_TIMEOUT < 1 || L2CAP_RTX_TIMEOUT > 60
#error L2CAP_RTX_TIMEOUT must be between 1 and 60 seconds.
#endif


/*---------------------------------------------------------------------------
 * L2CAP_ERTX_TIMEOUT constant
 *
 *     Defines the extended response timeout value for signaling commands,
 *     in seconds. Certain signaling commands (such as a Connect Response
 *     Pending) allow for a longer timeout period than the L2CAP_RTX_TIMEOUT
 *     value. When the ERTX timer elapses without a response, the channel
 *     is disconnected.
 *
 *     The extended timeout value may range from 60 to 300 seconds. The
 *     default value is 150 seconds.
 */
#ifndef L2CAP_ERTX_TIMEOUT
#define L2CAP_ERTX_TIMEOUT          150
#endif

#if L2CAP_ERTX_TIMEOUT < 60 || L2CAP_ERTX_TIMEOUT > 300
#error L2CAP_ERTX_TIMEOUT must be between 60 and 300 seconds.
#endif


/*---------------------------------------------------------------------------
 * L2CAP_NUM_CHANNELS constant
 *
 *     Defines the number of L2CAP Channels, based on number of protocols and
 *     devices. This constant must be large enough to support all the channels
 *     that are needed by the upper layer protocols. There is typically one
 *     channel per protocol per supported device.
 */
#ifndef L2CAP_NUM_CHANNELS
/*
 * First calculate the number of L2CAP Auxiliary Channels. Auxiliary channels
 * are used to support the Echo (Ping) and Info (GetInfo) signalling procedures.
 */
#if (L2CAP_PING_SUPPORT == XA_ENABLED) && (L2CAP_GET_INFO_SUPPORT == XA_ENABLED)
#define AUX_CHANNELS    2
#elif (L2CAP_PING_SUPPORT == XA_ENABLED) || (L2CAP_GET_INFO_SUPPORT == XA_ENABLED)
#define AUX_CHANNELS    1
#else
#define AUX_CHANNELS    0
#endif 

#define L2CAP_NUM_CHANNELS          ((L2CAP_NUM_PROTOCOLS * NUM_BT_DEVICES) + AUX_CHANNELS)
#endif

#if L2CAP_NUM_CHANNELS  < 1
#error L2CAP_NUM_CHANNELS must be at least 1
#endif

/*---------------------------------------------------------------------------
 * L2CAP_NUM_ENHANCED_CHANNELS constant
 *
 *     Enables support for L2CAP Enhanced Retransmission mode. When enabled,
 *     an L2CAP user is able to configure L2CAP connections to include
 *     support for retransmission of lost frames, SDU segmentation and
 *     reassembly and selective rejection of error frames.
 *
 *     By default this option is disabled, which results in a code size 
 *     savings. This option should be enabled if L2CAP based protocols or 
 *     profiles are used which depend on this feature. Each additional
 *     ERTM channel requires allocation of additional buffer space.
 */
#ifndef L2CAP_NUM_ENHANCED_CHANNELS
#define L2CAP_NUM_ENHANCED_CHANNELS 0
#endif

#if L2CAP_NUM_ENHANCED_CHANNELS > L2CAP_NUM_CHANNELS
#error L2CAP_NUM_ENHANCED_CHANNELS must not be greater than L2CAP_NUM_CHANNELS
#endif

/*---------------------------------------------------------------------------
 * L2CAP_ERTM_TX_WIN_SIZE constant
 *
 *     Specifies the maximum send window size for L2CAP Enhanced mode
 *     channels. This setting controls the amount of memory allocated to
 *     send unacknowledged ERTM I-frame segments. This value is indicated in
 *     the Configuration Response as a hint to the remote device of the number
 *     of frames that can be sent before resources are depleted. The remote
 *     device can use this information to efficiently acknowledge I-frames.
 *     A smaller size will save memory. A larger size will increase
 *     throughput.
 *
 *     Note that the TX window may be adjusted lower than this default value
 *     if the remote device specifies an RX window that is smaller than the
 *     local L2CAP_ERTM_TX_WIN_SIZE since we must not overflow the remote
 *     device's RX window.
 *
 *     By default this is set to 5. It should be adjusted as appropriate 
 *     for your implementation.
 */
#ifndef L2CAP_ERTM_TX_WIN_SIZE
#define L2CAP_ERTM_TX_WIN_SIZE      5
#endif

#if L2CAP_ERTM_TX_WIN_SIZE > 63 || L2CAP_ERTM_TX_WIN_SIZE < 1
#error L2CAP_ERTM_TX_WIN_SIZE must be between 1 and 63
#endif

/*---------------------------------------------------------------------------
 * L2CAP_ERTM_RX_WIN_SIZE constant
 *
 *     Specifies the maximum receive window size for L2CAP Enhanced mode
 *     channels. This setting controls the amount of memory allocated to
 *     manage received ERTM I-frames before indication to the higher layer.
 *     A smaller size will save memory. A larger size will increase
 *     throughput.
 *
 *     By default this is set to 5. It should be adjusted as appropriate 
 *     for your implementation.
 */
#ifndef L2CAP_ERTM_RX_WIN_SIZE
#define L2CAP_ERTM_RX_WIN_SIZE      5
#endif

#if L2CAP_ERTM_RX_WIN_SIZE > 63 || L2CAP_ERTM_RX_WIN_SIZE < 1
#error L2CAP_ERTM_RX_WIN_SIZE must be between 1 and 63
#endif

/*---------------------------------------------------------------------------
 * L2CAP_ERTM_MAX_TRANSMIT constant
 *
 *     Specifies the default maximum number of transmissions of an I-frame
 *     (including the first transmission) before the link is disconnected.
 *
 *     The default is 10 transmissions.
 */
#ifndef L2CAP_ERTM_MAX_TRANSMIT
#define L2CAP_ERTM_MAX_TRANSMIT     10
#endif

/*---------------------------------------------------------------------------
 * L2CAP_ERTM_RETRANS_TIMEOUT constant
 *
 *     Specifies the default Retransmission timeout value in milliseconds.
 *     If a flush timeout is defined for the link, this value should be the
 *     greater of 3 times the flush timeout value or 1 second.
 *
 *     The default is 2 seconds.
 */
#ifndef L2CAP_ERTM_RETRANS_TIMEOUT
#define L2CAP_ERTM_RETRANS_TIMEOUT  2000
#endif

/*---------------------------------------------------------------------------
 * L2CAP_ERTM_MONITOR_TIMEOUT constant
 *
 *     Specifies the default Monitor timeout value in milliseconds. If a
 *     flush timeout is defined for the link, this value should be the
 *     greater of 6 times the flush timeout value or 12 seconds.
 *
 *     The default is 12 seconds.
 */
#ifndef L2CAP_ERTM_MONITOR_TIMEOUT
#define L2CAP_ERTM_MONITOR_TIMEOUT  12000
#endif

/*---------------------------------------------------------------------------
 * L2CAP_FCS_OPTION constant
 *
 *     Specifies the default FCS option for Enhanced L2CAP channels. Use
 *     one of the options defined by the L2capFcsOption type in l2cap.h.
 *
 *     The default is 16 bit with the option to turn off FCS.
 */
#ifndef L2CAP_FCS_OPTION
#define L2CAP_FCS_OPTION            L2FCS_16BIT
#endif

#if L2CAP_FCS_OPTION != L2FCS_NONE && L2CAP_FCS_OPTION != L2FCS_16BIT
#error L2CAP_FCS_OPTION must be L2FCS_NONE or L2FCS_16BIT.
#endif

/*---------------------------------------------------------------------------
 * L2CAP_ERTM_ACK_TIMEOUT constant
 *
 *     Specifies the default maximum amount of time to wait before a received
 *     I-frame is acknowledged. Normally, I-frames are acknowledged long
 *     before this timer expires on a busy channel. This timer ensures
 *     acknowledgments are sent in a timely manner on a channel with little
 *     traffic.
 *
 *     Note: this timer will not fire if set larger than
 *     L2CAP_ERTM_RETRANS_TIMEOUT.
 *
 *     The default is 1/2 retransmission timeout.
 */
#ifndef L2CAP_ERTM_ACK_TIMEOUT
#define L2CAP_ERTM_ACK_TIMEOUT      (L2CAP_ERTM_RETRANS_TIMEOUT/2)
#endif

/*---------------------------------------------------------------------------
 * L2CAP_ERTM_SREJ_ENABLE constant
 *
 *     Specifies whether Selective Reject capability is enabled for the
 *     enhanced L2CAP channels. Disabling Selective Reject will save memory,
 *     but will likely cause more retransmitted I-frames. 
 *
 *     Selective Reject is Enabled by default.
 */
#ifndef L2CAP_ERTM_SREJ_ENABLE
#define L2CAP_ERTM_SREJ_ENABLE      XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * L2CAP_ERTM_FRAGMENTS constant
 *
 *     Enables support for fragmented receives in L2CAP Enhanced
 *     Retransmission mode. When enabled, received data will be indicated
 *     with the events L2EVENT_FRAGMENT_IND and L2EVENT_LAST_FRAG_IND.
 *
 *     By default this option is disabled, which ensures compatibility with 
 *     profiles that cannot handle fragmented receives. Enable this option
 *     for more efficient received data indications. Note that a receive
 *     buffer the size of L2CAP_MAXIMUM_MTU will be pre-allocated for each
 *     Enhanced L2CAP channel when this option is disabled. 
 */
#ifndef L2CAP_ERTM_FRAGMENTS
#define L2CAP_ERTM_FRAGMENTS        XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * L2CAP_FLOW_CONTROL constant
 *
 *     This option is deprecated. Use L2CAP_NUM_ENHANCED_CHANNELS.
 */
#ifndef L2CAP_FLOW_CONTROL
#define L2CAP_FLOW_CONTROL          XA_DISABLED
#endif

#if L2CAP_FLOW_CONTROL == XA_ENABLED 
#error The L2CAP_FLOW_CONTROL option is deprecated.
#endif

/*---------------------------------------------------------------------------
 * L2CAP_PRIORITY constant
 *
 *     Enables support for L2CAP BtPacket priority management.
 */
#ifndef L2CAP_PRIORITY
#define L2CAP_PRIORITY              XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * L2CAP_MTU constant
 *
 *     Defines the maximum receivable L2CAP SDU size, in bytes. This
 *     limitation applies only to packets received from the remote device.
 *     When L2CAP_NUM_ENHANCED_CHANNELS is 1 or greater, this constant defines 
 *     the maximum receivable segmented SDU size and can be composed from
 *     multiple PDUs.
 *
 *     This constant also affects the L2CAP Connectionless MTU. Some profiles
 *     require a minimum Connectionless MTU that is greater than the L2CAP
 *     minimum of 48 bytes. When implementing such a profile it is important
 *     that the L2CAP_MTU constant be at least as large as the specified
 *     Connectionless MTU size.
 *
 *     This value may range from 48 to 65529. The default value is 672 bytes.
 */
#ifndef L2CAP_MTU
#define L2CAP_MTU                   672
#endif 

#if L2CAP_MTU < 48 || L2CAP_MTU > 65529
#error L2CAP_MTU must be between 48 and 65529.
#endif

/*---------------------------------------------------------------------------
 * L2CAP_MPS constant
 *
 *     Defines the maximum receivable L2CAP PDU size, in bytes. This constant
 *     is equivalent to L2CAP_MTU when L2CAP_NUM_ENHANCED_CHANNELS is 0. When
 *     L2CAP_NUM_ENHANCED_CHANNELS is 1 or greater, an L2CAP_MTU may be segmented
 *     into multiple L2CAP_MPS sized PDUs.
 *
 *     This value may range from 48 to L2CAP_MTU. The default value is
 *     L2CAP_MTU.
 */
#ifndef L2CAP_MPS
#define L2CAP_MPS                   L2CAP_MTU
#endif 

#if L2CAP_NUM_ENHANCED_CHANNELS == 0 && L2CAP_MPS != L2CAP_MTU
#error L2CAP_MPS must equal L2CAP_MTU when L2CAP_NUM_ENHANCED_CHANNELS is 0.
#endif

#if L2CAP_MPS == 0 || L2CAP_MPS > L2CAP_MTU
#error L2CAP_MPS must be between 1 and L2CAP_MTU.
#endif

/*---------------------------------------------------------------------------
 * L2CAP_DEREGISTER_FUNC constant
 *
 *     Controls whether the L2CAP_DeregisterPsm function is allowed. In
 *     cases where L2CAP services will be dynamically loaded or unloaded,
 *     this constant must be enabled.
 *
 *     By default, L2CAP service deregistration is disabled. Disabling this
 *     option results in code size savings.
 */
#ifndef L2CAP_DEREGISTER_FUNC
#define L2CAP_DEREGISTER_FUNC       XA_DISABLED
#endif 


/*---------------------------------------------------------------------------
 * L2CAP_NUM_SIGNAL_PACKETS constant
 * 
 *     Defines the number of signaling packets allocated by L2CAP.
 *
 *     By default, L2CAP allocates two packets per device.
 */
#ifndef L2CAP_NUM_SIGNAL_PACKETS
#if L2CAP_NUM_ENHANCED_CHANNELS > 0
#define L2CAP_NUM_SIGNAL_PACKETS    (NUM_BT_DEVICES * L2CAP_NUM_ENHANCED_CHANNELS * 4)
#else
#define L2CAP_NUM_SIGNAL_PACKETS    (NUM_BT_DEVICES * 2)
#endif
#endif

#if L2CAP_NUM_SIGNAL_PACKETS < 1 
#error L2CAP_NUM_SIGNAL_PACKETS must be at least 1
#endif

/*---------------------------------------------------------------------------
 * L2CAP_NUM_TX_PACKETS constant
 * 
 *     Defines the number of TX packets allocated per device by L2CAP for
 *     segmenting upper layer packets.
 *
 *     Since this value is based on information obtained at run time, a
 *     best estimate should be calculated from the max expected TX window
 *     and max number of segments per packet. For example: if the max TX
 *     window is expected to be 10 and the max number of segments per Packet
 *     is expected to be 5, then a minimum 10*5=50 TX packets times the
 *     number of channels will be required.
 *
 *     Note that the compile time error checks below are very conservative.
 *     They assume the max window size will be used by each channel
 *     simultaneously. If some channels (e.g. SDP) do not use the max window
 *     size, you may need to tune these checks for run time behavior.
 */
#ifndef L2CAP_NUM_TX_PACKETS
#define L2CAP_NUM_TX_PACKETS        ((L2CAP_ERTM_TX_WIN_SIZE * L2CAP_NUM_ENHANCED_CHANNELS) + 2)
#endif

#if (L2CAP_ERTM_TX_WIN_SIZE * L2CAP_NUM_ENHANCED_CHANNELS) > L2CAP_NUM_TX_PACKETS
#error Insufficient L2CAP_NUM_TX_PACKETS for current configuration
#endif

#if (L2CAP_ERTM_RX_WIN_SIZE * L2CAP_NUM_ENHANCED_CHANNELS) > HCI_NUM_ACL_BUFFERS
#error Insufficient HCI_NUM_ACL_BUFFERS for current configuration
#endif

/*---------------------------------------------------------------------------
 * L2CAP_PRELUDE_SIZE constant
 * 
 *     Defines the number of bytes that are appended to the front of the
 *     L2CAP reassembly buffer. This is only recommended for use by
 *     protocols which are layered directly above L2CAP.
 *
 *     By default, L2CAP allocates zero extra space.
 */
#ifndef L2CAP_PRELUDE_SIZE
#define L2CAP_PRELUDE_SIZE          0
#endif

#if L2CAP_PRELUDE_SIZE+L2CAP_MTU > 65529
#error L2CAP_PRELUDE_SIZE plus L2CAP_MTU must be less than 65529.
#endif

/*---------------------------------------------------------------------------
 * L2CAP_VIOLATE_SPEC_MTU_NEG constant
 * 
 *     Controls whether the code is allowed to violate the L2CAP 
 *     specification to do illegal MTU size negotiation.   Setting this 
 *     define to XA_ENABLED in the overide.h file causes the code to violate 
 *     the L2CAP specification which specifically prohibits negotiating the 
 *     MTU values.  However, some phones such as the Motorola v3m have a bug 
 *     in which they sometimes expect the MTU to be negotiated.  In order to 
 *     work with these phones, this define must be set to XA_ENABLED in the 
 *     overide.h file to cause the code to negotiate the MTU to the value 
 *     taken from failed configuration response messages.  Note that there 
 *     is no valid reason to negotiate these values, as the two MTUs from 
 *     two connected devices are completely independent, do not need to be 
 *     the same, and no device is never obligated to use all of the 
 *     available MTU space on the other device.  
 *
 *     By default, violating the L2CAP specification to negotiate MTU sizes 
 *     is disabled.  
 */
#ifndef L2CAP_VIOLATE_SPEC_MTU_NEG
#define L2CAP_VIOLATE_SPEC_MTU_NEG  XA_DISABLED
#endif

/****************************************************************************
 *
 * Section: SDP Constants
 *
 ****************************************************************************/

/*-------------------------------------------------------------------------
 * SDP_CLIENT_SUPPORT constant
 *
 *     Controls whether the SDP client is included in the SDP protocol.  Set 
 *     this constant to XA_ENABLED when it is necessary to have the SDP client 
 *     compiled into your system.
 *
 *     By default, the SDP client is enabled.
 */
#ifndef SDP_CLIENT_SUPPORT
#define SDP_CLIENT_SUPPORT XA_ENABLED
#endif

/*-------------------------------------------------------------------------
 * SDP_SERVER_SUPPORT constant
 *
 *     Controls whether the SDP server is included in the SDP protocol.  Set 
 *     this constant to XA_ENABLED when it is necessary to have the SDP server 
 *     compiled into your system.
 *
 *     By default, the SDP server is enabled.
 */
#ifndef SDP_SERVER_SUPPORT
#define SDP_SERVER_SUPPORT XA_ENABLED
#endif

#if (SDP_SERVER_SUPPORT == XA_DISABLED) && (SDP_CLIENT_SUPPORT == XA_DISABLED)
#error SDP_CLIENT_SUPPORT and/or SDP_SERVER_SUPPORT must be enabled.
#endif

#if SDP_CLIENT_SUPPORT == XA_ENABLED
/*-------------------------------------------------------------------------
 * SDP_CLIENT_LOCAL_MTU constant
 *
 *     Defines the maximum L2CAP MTU used for the local SDP client.
 *
 *     This value may range from 48 to the L2CAP MTU size (L2CAP_MTU). 
 *     By default, this value is equal to the L2CAP_MTU size.
 */
#ifndef SDP_CLIENT_LOCAL_MTU
#define SDP_CLIENT_LOCAL_MTU L2CAP_MTU
#endif 

#if SDP_CLIENT_LOCAL_MTU < 48 || SDP_CLIENT_LOCAL_MTU > L2CAP_MTU
#error SDP_CLIENT_LOCAL_MTU must be between 48 and L2CAP_MTU.
#endif


/*-------------------------------------------------------------------------
 * SDP_CLIENT_MIN_REMOTE_MTU constant
 *
 *     Defines the minimum remote MTU size acceptable to the local
 *     SDP client. If the remote SDP server cannot support the specified
 *     MTU size, the connection attempt will fail and SDP operations will
 *     not occur.
 *
 *     This value may range from 48 to the L2CAP MTU size (L2CAP_MTU). 
 *     By default, this value is 48.
 */
#ifndef SDP_CLIENT_MIN_REMOTE_MTU
#define SDP_CLIENT_MIN_REMOTE_MTU 48
#endif

#if SDP_CLIENT_MIN_REMOTE_MTU < 48 || SDP_CLIENT_MIN_REMOTE_MTU > L2CAP_MTU
#error SDP_CLIENT_MIN_REMOTE_MTU must be between 48 and L2CAP_MTU.
#endif

/*-------------------------------------------------------------------------
 * SDP_PARSING_FUNCS constant
 *
 *     Controls whether the SDP response parsing is included and made 
 *     available to the application.  Set this constant to XA_ENABLED
 *     when it is necessary to have SDP response parsing available.
 *
 *     By default, SDP parsing is enabled.
 *
 */
#ifndef SDP_PARSING_FUNCS
#define SDP_PARSING_FUNCS XA_ENABLED
#endif

#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */

#if SDP_SERVER_SUPPORT == XA_ENABLED
/*-------------------------------------------------------------------------
 * SDP_NUM_CLIENTS constant
 *
 *     Defines the maximum number of clients that can connect
 *     simultaneously to the SDP server.
 *
 *     This value may range from 1 to the number of possible devices
 *     (NUM_BT_DEVICES). The default is NUM_BT_DEVICES.
 */
#ifndef SDP_NUM_CLIENTS
#define SDP_NUM_CLIENTS NUM_BT_DEVICES
#endif 

#if (SDP_NUM_CLIENTS < 1) || (SDP_NUM_CLIENTS > NUM_BT_DEVICES)
#error SDP_NUM_CLIENTS must be greater than 0 and less than or equal to NUM_BT_DEVICES
#endif

/*-------------------------------------------------------------------------
 * SDP_ACTIVE_CLIENTS constant
 *
 *     Defines the maximum number of active clients that can connect
 *     simultaneously to the SDP server. Active clients are those that
 *     are not in park.
 *
 *     This value may range from 1 to the number of possible active devices
 *     (16). The default value is NUM_BT_DEVICES.
 */
#ifndef SDP_ACTIVE_CLIENTS
#if NUM_BT_DEVICES < 16
#define SDP_ACTIVE_CLIENTS  (NUM_BT_DEVICES)
#else
#define SDP_ACTIVE_CLIENTS  16
#endif
#endif 

#if (SDP_ACTIVE_CLIENTS < 1) || (SDP_ACTIVE_CLIENTS > NUM_BT_DEVICES) || (SDP_ACTIVE_CLIENTS > 16)
#error SDP_ACTIVE_CLIENTS must be greater than 0 and less than or equal to NUM_BT_DEVICES and 16.
#endif

/*-------------------------------------------------------------------------
 * SDP_SERVER_SEND_SIZE constant
 * 
 *     Defines the maximum number of bytes that the server can send in a
 *     single response.
 *
 *     This value may range from 48 to the L2CAP MTU size (L2CAP_MTU). The
 *     default is 128.
 */
#ifndef SDP_SERVER_SEND_SIZE
#define SDP_SERVER_SEND_SIZE 128
#endif

#if (SDP_SERVER_SEND_SIZE < 48) || (SDP_SERVER_SEND_SIZE > L2CAP_MTU)
#error SDP_SERVER_SEND_SIZE must be between 48 and L2CAP_MTU
#endif


/*-------------------------------------------------------------------------
 * SDP_SERVER_MAX_LEVEL constant
 *
 *     Defines the maximum number of nested data element sequences in
 *     an SDP attribute. SDP uses this value internally when examining
 *     attributes. If the number of nested data elements exceeds
 *     SDP_SERVER_MAX_LEVEL, certain SDP functions will return errors.
 *
 *     The default value is four, which allows for typical attributes.
 */
#ifndef SDP_SERVER_MAX_LEVEL
#define SDP_SERVER_MAX_LEVEL  4
#endif

#if SDP_SERVER_MAX_LEVEL < 4
#error SDP_SERVER_MAX_LEVEL must be at least 4
#endif


/*-------------------------------------------------------------------------
 * SDP_SERVER_LOCAL_MTU constant
 *
 *     Defines the maximum L2CAP MTU used for the local SDP server.
 *
 *     This value may range from 48 to the L2CAP MTU size (L2CAP_MTU). 
 *     By default, this value is equal to the L2CAP_MTU size.
 */
#ifndef SDP_SERVER_LOCAL_MTU
#define SDP_SERVER_LOCAL_MTU L2CAP_MTU
#endif 

#if SDP_SERVER_LOCAL_MTU < 48 || SDP_SERVER_LOCAL_MTU > L2CAP_MTU
#error SDP_SERVER_LOCAL_MTU must be between 48 and L2CAP_MTU.
#endif


/*-------------------------------------------------------------------------
 * SDP_SERVER_MIN_REMOTE_MTU constant
 *
 *     Defines the minimum remote MTU size acceptable to the local
 *     SDP server. If the remote SDP client cannot support the specified
 *     MTU size, the connection attempt will fail and SDP operations will
 *     not occur.
 *
 *     This value may range from 48 to the L2CAP MTU size (L2CAP_MTU).
 *     By default, this value is 48.
 */
#ifndef SDP_SERVER_MIN_REMOTE_MTU
#define SDP_SERVER_MIN_REMOTE_MTU 48
#endif 

#if SDP_SERVER_MIN_REMOTE_MTU < 48 || SDP_SERVER_MIN_REMOTE_MTU > L2CAP_MTU
#error SDP_SERVER_MIN_REMOTE_MTU must be between 48 and L2CAP_MTU.
#endif

#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */


/****************************************************************************
 *
 * Section: RFCOMM Constants
 *
 ****************************************************************************/


/*-------------------------------------------------------------------------
 * RFCOMM_PROTOCOL constant
 *
 *     Controls whether the RFCOMM protocol layer will be included.  Set 
 *     this constant to XA_ENABLED when it is necessary to have RFCOMM 
 *     compiled into your system.  This is necessary for all profiles based on 
 *     the serial profile.
 *
 *     By default, RFCOMM is enabled.
 */
#ifndef RFCOMM_PROTOCOL
#define RFCOMM_PROTOCOL XA_ENABLED
#endif

#if RFCOMM_PROTOCOL == XA_ENABLED

/*-------------------------------------------------------------------------
 * RF_SECURITY constant
 *
 *     Controls whether RFCOMM protocol layer security is enabled.
 *     This operates in conjunction with BT_SECURITY.  If BT_SECURITY is
 *     enabled, then an application can enforce different levels of
 *     security from the Bluetooth stack.  Level 2 security includes
 *     the ability to trigger security on an RFCOMM service ID.  If 
 *     RF_SECURITY is disabled, then code to perform RFCOMM security
 *     checks is excluded.  It is possible to enable BT_SECURITY but 
 *     disable RF_SECURITY.  For example, if an application wishes to use 
 *     security (BT_SECURITY), but will not use RFCOMM level security, 
 *     then it should enable BT_SECURITY and disable RF_SECURITY to 
 *     minimize code size.  If BT_SECURITY is disabled, then RF_SECURITY will 
 *     be disabled as well.
 * 
 *     By default, RF_SECURITY inherits the state of BT_SECURITY.
 */
#ifndef RF_SECURITY
#define RF_SECURITY BT_SECURITY
#endif

#if BT_SECURITY != XA_ENABLED
#if RF_SECURITY == XA_ENABLED
#error RF_SECURITY requires BT_SECURITY
#endif /* RF_SECURITY */
#endif /* BT_SECURITY */

/*-------------------------------------------------------------------------
 * RF_SEND_TEST constant
 *
 *     Controls whether the stack offers support for sending an RFCOMM
 *     test. When enabled, the RF_SendTest API can be used to test
 *     the connection with the remote device.
 *
 *     By default, sending RF tests is disabled.
 */
#ifndef RF_SEND_TEST
#define RF_SEND_TEST XA_DISABLED
#endif

/*-------------------------------------------------------------------------
 * RF_SEND_CONTROL constant
 *
 *     Controls whether RFCOMM can be used to send control channel
 *     information. This includes modem status, line status, and negotiation
 *     of remote port settings.
 *
 *     If RF_SEND_CONTROL is enabled, control channel information can
 *     be sent and processed by the application. This includes support for
 *     the following events and APIs.
 *
 *     Events: RFEVENT_MODEM_STATUS_IND, RFEVENT_MODEM_STATUS_CNF,
 *     RFEVENT_LINE_STATUS_IND, RFEVENT_LINE_STATUS_CNF,
 *     RFEVENT_PORT_STATUS_CNF, RFEVENT_PORT_NEG_CNF.
 *
 *     APIs: RF_SetModemStatus, RF_SetLineStatus, RF_RequestPortSettings,
 *     RF_AcceptPortSettings, RF_RequestPortStatus, and RF_SendPortStatus.
 *
 *     If RF_SEND_CONTROL is disabled, RFCOMM will handle control channel
 *     requests received from the remote device, but will not send control
 *     channel requests to the remote device. The APIs and Events listed
 *     above are disabled, resulting in significant code size savings.
 *
 *     By default, sending control channel information is enabled.
 */
#ifndef RF_SEND_CONTROL
#define RF_SEND_CONTROL XA_ENABLED
#endif

/*-------------------------------------------------------------------------
 * NUM_RF_SERVERS constant
 *
 *     Defines the number of RFCOMM services that can be registered with
 *     RFCOMM_RegisterServerChannel.
 *
 *     This value may range from 0 to 30. The default is 4.
 */
#ifndef NUM_RF_SERVERS
#define NUM_RF_SERVERS 4
#endif

#if NUM_RF_SERVERS >= 0
#if NUM_RF_SERVERS > 30
#error NUM_RF_SERVERS must not exceed 30
#endif
#else
#error NUM_RF_SERVERS must be 0 or more
#endif


/*-------------------------------------------------------------------------
 * NUM_RF_CHANNELS constant
 *
 *     Defines the maximum number of RFCOMM channels that can be
 *     established to each device. This includes both server and client 
 *     channels.
 *
 *     The default number of channels is twice the number of local
 *     servers.
 */
#ifdef NUM_RF_CHANNELS

#if (NUM_RF_CHANNELS < NUM_RF_SERVERS) || (NUM_RF_CHANNELS > 60)
#error NUM_RF_CHANNELS value is invalid
#endif

#else

#if NUM_RF_SERVERS > 0
#define NUM_RF_CHANNELS (NUM_RF_SERVERS * 2)
#else
#define NUM_RF_CHANNELS 1
#endif

#endif

#if !(NUM_RF_CHANNELS > 0)
#error NUM_RF_CHANNELS must be greater than 0
#endif


/*-------------------------------------------------------------------------
 * RF_MAX_FRAME_SIZE constant
 *
 *     Defines the maximum frame size for an RFCOMM channel. All registered
 *     RfChannel structures must have a "maxFrameSize" equal to or less
 *     than this value.
 *
 *     This value may range between 127 bytes and 32767 bytes (or the
 *     L2CAP_MTU size minus 5, whichever is lower). The default maximum
 *     frame size is the L2CAP_MTU size, minus five bytes for RFCOMM
 *     protocol information.
 */
#ifndef RF_MAX_FRAME_SIZE
#if L2CAP_MTU > (32767+5)
#define RF_MAX_FRAME_SIZE 32767
#else
#define RF_MAX_FRAME_SIZE (L2CAP_MTU - 5)
#endif
#endif

#if RF_MAX_FRAME_SIZE > (L2CAP_MTU - 5)
#error RF_MAX_FRAME_SIZE cannot exceed the L2CAP_MTU - 5
#endif

/* The value tested here (23) must be the same as RF_MIN_FRAME_SIZE.  */ 
#if RF_MAX_FRAME_SIZE < 23
#error RF_MAX_FRAME_SIZE cannot be less than 23
#endif

#if RF_MAX_FRAME_SIZE > 32767
#error RF_MAX_FRAME_SIZE cannot exceed 32767
#endif


/*-------------------------------------------------------------------------
 * RF_CONNECT_TIMEOUT constant
 *
 *     Defines the amount of time to wait for a response to a connect
 *     request for an RFCOMM channel.  Because it is not possible to know
 *     whether authentication is required, this value must allow time for
 *     user interaction.
 * 
 *     This value may range between 60000 and 300000 (60 sec and 300 sec).
 *     The default time is 60 seconds.
 */
#ifndef RF_CONNECT_TIMEOUT
#define RF_CONNECT_TIMEOUT 60000
#endif

#if RF_CONNECT_TIMEOUT < 60000
#error RF_CONNECT_TIMEOUT cannot be less than 60000
#endif

#if RF_CONNECT_TIMEOUT > 300000
#error RF_CONNECT_TIMEOUT cannot be greater than 300000
#endif

/*-------------------------------------------------------------------------
 * RF_T1_TIMEOUT constant
 *
 *     Defines the amount of time to wait for a response to an RFCOMM command.
 *     If a response is not received within the allotted time, the RFCOMM
 *     multiplexer session will be torn down.
 *
 *     This value may range between 10000 and 60000 (10 sec and 60 sec).
 *     The default time is 20 seconds.
 */
#ifndef RF_T1_TIMEOUT
#define RF_T1_TIMEOUT 20000
#endif

#if RF_T1_TIMEOUT < 10000
#error RF_T1_TIMEOUT cannot be less than 10000
#endif

#if RF_T1_TIMEOUT > 60000
#error RF_T1_TIMEOUT cannot be greater than 60000
#endif

/*-------------------------------------------------------------------------
 * RF_T2_TIMEOUT constant
 *
 *     Defines the amount of time to wait for a response to an RFCOMM
 *     control channel command.  If a response is not received within the
 *     allotted time, the RFCOMM multiplexer session will be torn down.
 *
 *     This value may range between 1000 and 60000 (1 sec and 60 sec).
 *     The default time is 20 seconds.
 */
#ifndef RF_T2_TIMEOUT
#define RF_T2_TIMEOUT 20000
#endif

#if RF_T2_TIMEOUT < 1000
#error RF_T2_TIMEOUT cannot be less than 1000
#endif

#if RF_T2_TIMEOUT > 60000
#error RF_T2_TIMEOUT cannot be greater than 60000
#endif

#endif /* RFCOMM_PROTOCOL == XA_ENABLED */


/* Internal flag for enable/disable of low level tests */
#ifndef JETTEST
#define JETTEST XA_DISABLED
#endif /* JETTEST */

/****************************************************************************
 *
 * Section: UnplugFest Testing
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * UPF_TWEAKS constant
 *
 *     This value controls the inclusion of code which enables certain
 *     behaviors for UnplugFest interoperability testing.  Certain UnplugFest
 *     tests require functionality or behaviors that may not be included in 
 *     the Blue SDK.  Set this value to XA_ENABLED to include this code.
 *
 *     This constant should always be set to XA_DISABLED, except when
 *     performing UnplugFest tests.
 */
#ifndef UPF_TWEAKS
#define UPF_TWEAKS XA_DISABLED
#endif /* UPF_TWEAKS */


#endif /* __BTCONFIG_H */

