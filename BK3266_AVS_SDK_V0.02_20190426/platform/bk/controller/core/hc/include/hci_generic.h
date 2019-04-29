/***********************************************************************
 *
 * MODULE NAME:     hci_generic.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:     Generic HCI for lib embedded with 3rd party hosts.
 * MAINTAINER:      Tom Kerwick
 * CREATION DATE:   1 December 2001
 *
 * LICENSE:
 *     This source code is copyright (c) 2001-2004 Ceva Inc.
 *     All rights reserved.
 *
 * DESCRIPTION
 * This module presents an API to the generic HCI Transport. It can be used
 * to access the queues in the Host Controller without a physical transport
 * mechanism (i.e. UART,USB).
 *
 * The General Mechanism using the system is as follows :
 *
 * INITIALISATION
 * OPTION A (min priority model):
 * 1/ Call HCI_Generic_Initialise to setup the transport.
 * 2/ Call HCI_Generic_Register_Tx_Callback passing the function to be called 
 *    when data is to being transmited FROM the host controller.
 * 3/ Call HCI_Generic_Register_Exit_HC_Scheduler_Callback passing the function 
 *    to be called when when there is no event/activity in the HC stack in order 
 *    for the CPU process to leave from the HC scheduler space.
 * OPTION B (event driven model):
 * 1/ Call HCI_Generic_Initialise to setup the transport.
 * 2/ Call HCI_Generic_Register_Tx_Callback passing the function to be called 
 *    when data is to being transmited FROM the host controller.
 * 3/ Call HCI_Generic_Register_HC_Event_Handlers to register the OS wrappers
 *    and enable event driven Host Controller.
 * 
 * DATA TRANSFER TO HOST CONTROLLER
 * 1/ Call HCI_Generic_Get_Rx_Buf to get a buffer from the Host Controller.
 * 2/ Write the payload into the buffer.
 * 3/ Call HCI_Generic_Commit_Rx_Buf to complete delivery of the Data to the host controller
 *
 * DATA TRANSFER FROM HOST CONTROLLER
 * 1/ HC uses previously registered callback deliver data to the host.
 * 2/ Delivered data is processed by host layers.
 * 3/ Call HCI_Generic_Acknowledge_Complete_Tx when processing of the data is complete
 *    ( This allows the HC to free the memory associated with the data).
 *
 * SYSTEM SHUTDOWN
 * 1/ Call HCI_Generic_Shutdown to permanently disable the transport mechanism.
 ***********************************************************************/
#include "types.h"

#define HCI_COMMAND 0x1
#define HCI_ACLDATA 0x2
#define HCI_SCODATA 0x3
#define HCI_EVENT   0x4

/*****************************************************************************
 * HCI_Generic_Initialise
 * 
 * Description :
 * Used to initialise the Transport in the Host Controller Stack.
 * This function is not re-entrant.
 *
 * Paramaters:
 *              a) queue depth for incoming ACL data (packets) RECOMMEND: 12.
 *              b) queue depth for outgoing ACL data (packets) RECOMMEND: 08.
 *              c) queue depth for incoming SCO data (packets) RECOMMEND: 20.
 *              d) queue depth for outgoing SCO data (packets) RECOMMEND: 20.
 *              e) bottom address of HC configuration data (usually in FLASH).
 *              f) hardware configuration information
 *                  bit31..bit16: reserved
 *                  bit15..bit12: transmitter class type
 *                                 0: external power amplifier unavailable (class2/3 tx)
 *                                 1: external power amplifier available (class1 tx)
 *                                 others: reserved
 *                  bit11..bit8 : voice codec via voice control interface
 *                                 0: Motorola MC145483
 *                                 1: ST Micro STLC7550
 *                                 others: reserved
 *                  bit7..bit4  : system clock scheme for low power mode operation
 *                                 0: radio clocks cpu
 *                                 1: an external oscillator clocks cpu
 *                                 others: reserved
 *                  bit3..bit0  : low power oscillator type
 *                                 0: low power oscillator unavailable
 *                                 1: 32.768KHz low power oscillator available
 *                                 2: 32.000KHz low power oscillator available
 *                                 others: reserved
 *
 * Returns:     0   initialisation completed successfully
 *              1   transport system already active
 *             -1   initialisation failed on memory allocation
 *             -2   hardware(RF) self diagnosis failure
 *
 * Notes:
 *              If not linking with stdlib standard library then malloc/free
 *              substitute functions should be created by application for the
 *              HC library to link against, for example:
 *
 *              void *malloc(size_t size)
 *              {
 *                  VP p_blk;
 *                  ER ercd = get_mpl(PRH_HEAP_RAM, (INT)size, &p_blk);
 *                  return (ercd == E_OK ? p_blk : (VP)0);
 *              }
 *
 *              void free(void *blk)
 *              {
 *                  rel_mpl(PRH_HEAP_RAM, blk);
 *              }
 *
 *              If stdlib prototypes unsuitable, the following global pointers
 *              may be overridden before HCI_Generic_Initialise is called:
 *
 *              extern void* (*HC_MemAlloc)(u_int32); // defaults = malloc
 *              extern void (*HC_MemFree)(void*); // defaults = free
 *
 ****************************************************************************/

int HCI_Generic_Initialise( u_int16 num_in_acl_packets,
                            u_int16 num_out_acl_packets,
                            u_int16 num_in_sco_packets,
                            u_int16 num_out_sco_packets,
                            u_int32* flash_config_addr,
                            u_int32 hw_config_info);

/*****************************************************************************
 * HCI_Generic_Register_HC_Event_Handlers
 *
 * Generic HCI registration of HC pend/post event handlers.
 *
 * Default prototypes with suggested OS wrapper implementations:
 *
 * void HC_Event_Post(u_int32 mask)
 * {
 *     if(get_imask())
 *         iset_flg(HC_EVENT_FLAGS, mask);
 *     else
 *         set_flg(HC_EVENT_FLAGS, mask);
 * }
 *
 * u_int32 HC_Event_Pend(u_int32 mask, u_int16 timout)
 * {
 *     FLGPTN hc_flgptn = 0;
 *     twai_flg(HC_EVENT_FLAGS, mask, TWF_ORW, &hc_flgptn, timeout);
 *     return (u_int32)hc_flgptn;
 * }
 *
 * The event flags should be initialised to the following specificaiton:
 *
 * Clears bits when released from wait state: YES
 * Initial bit pattern: 0x00000000
 * Enables multiple tasks to wait: NO
 * Waiting queue fifo/priority order: N/A
 *
 * The pend operation should return when any event specified by the OR mask
 * is posted, or when the specified timeout (in milliseconds) expires.
 *
 * Extended prototype also allows override of HC 10ms default wakeup timeout.
 * Warning: Increasing this value would reduce responsiveness of some lower
 * priority host controller actions such as peer power control monitor. Do
 * not increase this value greater than 100ms.
 *
 ****************************************************************************/
#define HCI_Generic_Register_HC_Event_Handlers() \
        HCI_Generic_Register_HC_Event_Handlers_Ex(HC_Event_Post,HC_Event_Pend,10)
        
void HCI_Generic_Register_HC_Event_Handlers_Ex(void (*hc_post)(u_int32),
                  u_int32 (*hc_pend)(u_int32, u_int16), u_int16 hc_timeout);

/*****************************************************************************
 * HCI_Generic__Shutdown
 * Used to shutdown the Transport in the Host Controller Stack.
 * This function is not re-entrant 
 *
 * Returns:     0   if shutdown completed successfully
 *              1   if transport system not active
 ****************************************************************************/
int HCI_Generic_Shutdown(void);

/*****************************************************************************
 * HCI_Generic_Get_Rx_Buf
 *
 * Gets a buffer from the queuing system in the Host Controller. Once the host
 * has written the pdu into this buffer it should commit it using "HCI_Generic_Commit_Rx_Buf"
 *
 * buf_type            HCI_COMMAND | HCI_ACLDATA | HCI_SCODATA
 *
 *                        HCI_COMMAND 0x01
 *                        HCI_ACLDATA 0x02
 *                        HCI_SCODATA 0x03
 *
 * size             Length of command, or of acl data, or of sco data
 * hci_header       Header of HCI packet
 * 
 * DESCRIPTION
 *  The first parameter indicates if the data 
 *  contains a HCI Command, HCI ACL Data or HCI SCO Data.
 *
 *           buf_type == 0x01  -- HCI_COMMAND
 *           buf_type == 0x02  -- HCI_ACL_DATA
 *           buf_type == 0x03  -- HCI_SCO_DATA
 *      
 *  HCI COMMAND
 *----------------
 *     buf_type == HCI_COMMAND (0x01)
 *     hci_header          - A pointer to 3 Bytes containing the Command Opcode 
 *                           and the total length of the command parameters.
 *                           See section 4.4.1 of Bluetooth Spec.
 *     size                - Length of the overall command INCLUDING the bytes for the header.
 *
 *     Note : The returned pointer is a buffer pointer into which the HCI Command
 *            parameters are to be written
 *         
 *
 *  HCI ACL DATA
 *  --------------
 *     buf_type == HCI_ACL_DATA (0x02)
 *     hci_header          - A pointer to 2 Bytes containing the ACL Handle, PB_Flags & BC Flags.
 *                           encoded as described in section 4.4.3 of Bluetooth Spec.
 *
 *       For HCI ACL Data  header[0 .. 1] =
 *        _________________________________________
 *       0 ...................12 .......14........16
 *       | Connecton Handle    | PB Flag | BC Flag |
 *       |_____________________|_________|_________|
 *
 *     size  - Length of the data section of the packet. Equivalent to the "data total length"
 *             field of HCI Data packets described in section 4.4.1 of Bluetooth spec.
 *
 *     Note : The returned pointer is a Buffer pointer into the Data bytes are written.
 *
 *
 ***************************************************************************************/

u_int8* HCI_Generic_Get_Rx_Buf(u_int8 buf_type, int size, u_int8 *hci_header);


/*****************************************************************************
 * HCI_Generic_Commit_Rx_Buf
 *
 * Commits a buffer allocated by HC_Generic_Get_Rx_Buf to the queueing system
 *
 * buf_type            HCI_COMMAND | HCI_ACLDATA | HCI_SCODATA
 *
 *                        HCI_COMMAND 0x01
 *                        HCI_ACLDATA 0x02
 *                        HCI_SCODATA 0x03
 ****************************************************************************/

void HCI_Generic_Commit_Rx_Buf(u_int8 buf_type);

/*****************************************************************************
 * HCI_Generic_Register_Tx_Callback
 *
 * Registers the function to be called back to dispatch Events or Incoming data
 * to the Host.
 *
 * Parameters of the Callback Function
 *-------------------------------------
 * data     - The data payload of the packet to be delivered to the host
 * pdu_len  - The length of the payload section being delivered to the host.
 * header   - The header associated with the packet.
 * head_len - The length of the header associated with the packet.
 * 
 * DESCRIPTION
 *-------------
 * The below description is how to interpret the values delivered to the callback.
 *
 *  The first Byte of the Header (header[0]) indicates if the data 
 *  contains a HCI Command, HCI ACL Data or HCI SCO Data.
 *
 *           header[0] == 0x01  -- HCI_COMMAND
 *           header[0] == 0x02  -- HCI_ACL_DATA
 *           header[0] == 0x03  -- HCI_SCO_DATA
 *      
 *  HCI EVENT
 *----------------
 *     head_len ==  0x01   - There is only a single byte header for HCI Events
 *     header[0] == 0x04   - Indicates that this is a HCI Event               
 *    
 *     pdu_len            - Overall length of the HCI Event PDU ( length of the data field).
 *     data               - pointer to the bytes of the HCI Event as described in section
 *                          4.4.1 of Bluetooth Specification.
 *
 *     data[0.....N] == 
 *   _________________________________________
 *  0 .........8.........16 ........24........31
 *  |    Event |Para Tot |  Event Para 0      |
 *  |    code  | Length  |                    |
 *   -----------------------------------------
 *  | Para 1             |  Para 2  | Para 3  |
 *   -----------------------------------------
 *  |         :              :                |
 *   -----------------------------------------
 *  | Para N-1             |  Para N          |	
 *   -----------------------------------------
 *
 *  HCI ACL DATA
 *  --------------
 *     head_len ==  0x05   - There is only a single byte header for HCI Events
 *     header[0] == 0x02   - Indicates that this is a HCI ACL Data.
 *     header[1..4]        - Contain the header portion of the ACL Data packet.
 *                           encompassing Handle, PB & BC flags, Packet Length.
 *                           See Section 4.4.3 of Bluetooth Spec.
 *
 *       For HCI ACL Data  header[1 .. 4] =
 *        ____________________________________________________________
 *       0 ...................12 .......14........16.................31
 *       | Connecton Handle    | PB Flag | BC Flag |   Packet Length  |
 *       |_____________________|_________|_________|__________________|
 *
 *
 *     data               - pointer to the bytes of the HCI ACL Data EXCLUDING the header
 *                          information. 
 *
 *     pdu_len	          - The number of bytes in the data field.
 *
 ***************************************************************************************/

void HCI_Generic_Register_Tx_Callback(void (*callback_function)(u_int8 *data, u_int32 pdu_len, u_int8 *header, 
                            u_int8 head_len));



/*****************************************************************************
 * HCI_Generic_Acknowledge_Complete_Tx
 *
 * Acknowledge the completion of tranmission - this allows the next item 
 * on the queue to be transmitted.
 * This function should be called once a PDU has been successfully delivered to
 * the Host. Always occurs as a result of the invokation of the host callback function
 * registered in "HCI_Generic_Register_Tx_Callback"
 *   
 * buf_type    Identifies the type of the packet being acknowledged.
 *                    HCI_ACLDATA - 0x02
 *                    HCI_EVENT   - 0x04
 *
 * buff_len     The length of the buffer being acknowledged                                 
 ****************************************************************************/

void HCI_Generic_Acknowledge_Complete_Tx(u_int8 buf_type, u_int32 buf_len);

/*****************************************************************************
 * HCI_Generic_Register_Exit_HC_Scheduler_Callback
 * 
 * Registers the function to be called back to exit Host Controller task.
 *
 * Descriptions:
 * This function is for the registration of the function to be called back 
 * when there is no event/activity in the HC stack in order for the CPU process 
 * to leave from the HC scheduler. The HC scheduler and the Host scheduler are 
 * mapped by RTOS (Real Time Operating System) respectively so that this function
 * allows the CPU process to switch to the Host task. This function can avoid 
 * that the HC scheduler consumes all of the CPU bandwidth in case of system idle. 
 * This function helps the Host maintain and control the state of the processor 
 * power after CPU process switches to the Host task. Thus the function registered 
 * to this API may be the RTOS service call.
 ****************************************************************************/

void HCI_Generic_Register_Exit_HC_Scheduler_Callback(void (*callback)(void));


/*****************************************************************************
 * HCI_Generic_HC_Monitor
 * 
 * Descriptions:
 * This function allows the Host to check for events/activities in the HC 
 * from the Host side. If the Host recognizes that the HC has no event/activity 
 * by using this API for checking and there is no event/activity in the Host 
 * itself, the Host can make the processor sleep.
 *
 * Returns:     0   if there are events/activities in the Host Controller.
 *              1   if there is no event/activity in the Host Controller.
 ****************************************************************************/

u_int8 HCI_Generic_HC_Monitor(void);


/*****************************************************************************
 * HCI_Generic_LC_Monitor
 * 
 * Descriptions:
 * This function allows the Host to check whether the CPU Halt is available or 
 * not in the HC. If the Host has no required CPU process (for example 
 * system standby, the status of saving the power by sniff/hold/park mode) and 
 * this API returns the value Åe1Åf, the Host can make the processor stand by. 
 * 
 * Returns:     0   if the Host Controller does not allow to halt system.
 *              1   if system halt is avalable in the Host Controller.
 ****************************************************************************/

u_int8 HCI_Generic_LC_Monitor(void);

