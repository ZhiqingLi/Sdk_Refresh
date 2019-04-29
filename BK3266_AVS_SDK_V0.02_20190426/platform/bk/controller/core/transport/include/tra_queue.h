#ifndef _PARTHUS_TRA_QUEUE_H_
#define _PARTHUS_TRA_QUEUE_H_

/**************************************************************************
 * MODULE NAME:    tra_queue.h
 * PROJECT CODE:   
 * DESCRIPTION:    System Queues
 * MAINTAINER:     John Sheehy
 * DATE:           20 March 2000
 *
 * SOURCE CONTROL: $Id: tra_queue.h,v 1.67 2010/02/11 02:03:11 garyf Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 * SOURCE:
 *
 * ISSUES:
 *
 * NOTES TO USERS:
 **************************************************************************/

/*
 * Queue types and numbers 
 * The "table" is delimeted by TRA_QUEUE_TOTAL_NUM_QUEUES, which has the correct
 * value for dimensioning the queue array due to HCI_COMMAND_Q starting at zero
 */

enum 
{
    HCI_COMMAND_Q = 0,                  
    HCI_EVENT_Q   = HCI_COMMAND_Q + 1,  /* Always 1 */   
    LMP_IN_Q      = HCI_EVENT_Q + 1,    /* Always 2 */
    LMP_OUT_Q     = LMP_IN_Q + 1,       /* Always 3 */

#if (PRH_BS_CFG_SYS_TRACE_VIA_HCI_SUPPORTED==1)
    TCI_DEBUG_Q   = LMP_OUT_Q + MAX_ACTIVE_DEVICE_LINKS,
    L2CAP_IN_Q    = TCI_DEBUG_Q + 1,
#else
    L2CAP_IN_Q    = LMP_OUT_Q + MAX_ACTIVE_DEVICE_LINKS,
#endif


    L2CAP_OUT_Q   = L2CAP_IN_Q + MAX_ACTIVE_DEVICE_LINKS,
    /* 
     * The +1 below is to allow for the piconet broadcast queue (device index 8)       0 is active broadcast 
     */
#if (PRH_BS_CFG_SYS_SYN_VIA_HCI_SUPPORTED==1)
    SCO_IN_Q      = L2CAP_OUT_Q + MAX_ACTIVE_DEVICE_LINKS + 1,

    SCO_OUT_Q     = SCO_IN_Q + PRH_BS_CFG_SYS_MAX_SYN_LINKS_VIA_HCI,

    TRA_QUEUE_TOTAL_NUM_QUEUES = SCO_OUT_Q + PRH_BS_CFG_SYS_MAX_SYN_LINKS_VIA_HCI
#else

    TRA_QUEUE_TOTAL_NUM_QUEUES = L2CAP_OUT_Q + MAX_ACTIVE_DEVICE_LINKS+1
#endif

};

/*****************************************************************************
 * mBTq_Get_Aligned_Data_Packet_Length
 *
 * This macro returns the size of the queue entry aligned to the
 * defined buffer alignment 
 ****************************************************************************/
#define mBTq_Get_Aligned_Data_Packet_Length(data_packet_length) \
    ((((data_packet_length)+PRH_BS_CFG_SYS_ACL_BUFFER_ALIGNMENT-1) /	\
        PRH_BS_CFG_SYS_ACL_BUFFER_ALIGNMENT) * PRH_BS_CFG_SYS_ACL_BUFFER_ALIGNMENT)

/*
 * struct q_desc 
 *
 * Structure definition for array entry on the queues
 *
 * data              pointer to the data (At the start so the structure pointer can 
 *                   be dereferenced for quick checking )
 * callback          Pointer to callback function to indicate successful tx or processing
 *                   complete (ignored if NULL)
 * length            Number of valid bytes pointed to by data
 * packet_type       Packet type for this descriptor
 * message_type      LCH_start, LCH_continue, LMP_message
 * data_start_offset for L2CAP IN/OUT queues for freeing allocated chunk  
 *                   (restoring to free chunk list)
 * next              Linked list next entry for L2CAP
 */

typedef struct q_desc 
{
    u_int8 *data;                   /* pointer to the data                  */
    void (*callback)(t_deviceIndex);/* Optional call back function address  */
    struct q_desc *next;            /* Linked list next entry for L2CAP     */
	u_int32 insert_time;            /* Time an entry was placed on the Queue */
    u_int32 enc_pkt_count;
    u_int16 length;                 /* Length in bytes of data              */ 
    u_int16 data_start_offset;      /* Offset to original data pointer      */
    t_deviceIndex device_index;     /* Main use is on shared LMP In Queue   */
    u_int8 packet_type;             /* Baseband packet type e.g. DM1        */
    u_int8 message_type;            /* LCH_start, LCH_continue, LMP_message */
	u_int8 auto_flushable;          /* Indicates if this packet can be automatically
									   flushed - AUTO_FLUSHABLE / NON_AUTO_FLUSHABLE*/
    u_int8 broadcast_flags;         /* Flags relating to broadcast data     */
    u_int8 encrypt_status;
    u_int8 opcode_pending;
    u_int8 dummy;                   /**< aligned(4) */
} t_q_descr;


/*
 * Actual Queue structure definitions
 *
 * The variable chunk queues are implemented as a circular data buffer
 * with separate entry descriptors (headers) for each entry.
 * The fixed chunk queues have a one to one entry descriptor associated 
 * with each fixed size buffer.
 */
typedef struct s_queue
{
    s_int16   head_offset;              /* Var Q:  Head into circular data  */
    s_int16   tail_offset;              /* Var Q:  Tail of data             */
    s_int16   end_offset;               /* Var Q:  End of data              */ 
    s_int16   data_size;                /* Var Q:  Size of data buffer      */
    u_int8    *q_data;                  /* Var Q:  Where data is located    */

    /*
     * For fixed chunks queues, a linked list of descriptors is used
     * For variable length queues, an array of descriptors is used 
     */
    t_q_descr *headers;                 /* Var Q: Array of descriptors      */
                                        /* DataQ: Head entry for removal    */
    t_q_descr *headers_tail;            /* DataQ: Tail entry for insertion  */
    
    u_int16   original_length;          /* DataQ: Segmented entry original  */
    u_int16   rollback_length;          /* DataQ: Segmentation rollback len */
    u_int16   num_processed_chunks;     /* DataQ: # BB processed chunks     */
    u_int16   num_entries;              /* All Q: # of committed Q Entries  */
	u_int16   total_data_enqueued;      /* SCO Q: Total amount of Q data pending */
    u_int8    pending_commits;          /* All Q: # of pending Q entries    */

    u_int8    desc_head;                /* Var Q: index of head descriptor  */
    u_int8    desc_tail;                /* Var Q: index of tail descriptor  */
    u_int8    num_headers;              /* Var Q: number of headers         */
} t_queue;
    
/*
 * struct s_data_queue 
 *
 * Structure definition for a single in/out ACL or SCO data queue
 * Note: locally used in tra_queue.c, but defined here for test access
 */
typedef struct s_data_queue
{
    u_int16 num_of_headers;             /* HC total number of data packets   */
    u_int16 max_data_packet_length;     /* HC data packet length             */
    u_int16 num_of_consumed_chunks;     /* HC number of chunks processed by  */
    u_int16 num_of_list;                /* HC number of data packets list    */
    struct list_head header_list;
} t_data_queue;

/*
 * Function prototypes
 */
/*****************************************************************************
 * BTq_Initialise
 *
 * Initialise all the queues in the system.
 ****************************************************************************/
void BTq_Initialise(void);

/*****************************************************************************
 * _BTq_Allocate_Data_Buffers
 *
 * Allocates memory from heap for data buffer dimensions specified.
 ****************************************************************************/
#if (PRH_BS_CFG_SYS_FLEXIBLE_DATA_BUFFER_SIZES_SUPPORTED==1)
int _BTq_Allocate_Data_Buffers( void* (*mem_alloc)(u_int32 size),
                                u_int16 num_in_acl_packets,
                                u_int16 num_out_acl_packets,
                                u_int16 num_in_sco_packets,
                                u_int16 num_out_sco_packets);
                                
/*****************************************************************************
 * _BTq_Deallocate_Data_Buffers
 *
 * Deallocate memory from heap for data buffer dimensions specified.
 ****************************************************************************/                                
void _BTq_Deallocate_Data_Buffers(void (*mem_dealloc)(void*));
#endif

/*****************************************************************************
 * BTq_Reset
 *
 * Reset the queues associated with device index
 * Warning: This function should be executed atomically with 
 * respect to any other contexts that might try to enqueue or dequeue 
 * to/from this particular queue.
 *
 * Reset the LMP output Queue
 * Reset the L2CAP input and output queues after returning any entries
 *       and resetting the number of processed chunks
 * Note that LMP input queue is not reset, since shared queue.
 ****************************************************************************/
void BTq_Reset(t_deviceIndex device_index);

/*****************************************************************************
 * BTq_Is_Queue_Empty
 *
 * This function returns 1 if the queue specified is empty
 ****************************************************************************/
RAM_CODE boolean BTq_Is_Queue_Empty(u_int8 q_type, t_deviceIndex device_index);

/*****************************************************************************
 * BTq_Are_Queues_Empty
 *
 * This function returns 1 if the queue set specified is empty
 ****************************************************************************/
boolean BTq_Are_Queues_Empty(u_int8 q_type);


/*****************************************************************************
 * BTq_Is_Queue_Not_Full
 *
 * This function returns one if there is any space at all on the queue 
 * (i.e. non full). Combined with the above, all four cases can be 
 * derived: empty, full, not empty, and not full.
 ****************************************************************************/
RAM_CODE boolean BTq_Is_Queue_Not_Full(u_int8 q_type, t_deviceIndex device_index);


/*****************************************************************************
 * BTq_Enqueue
 *
 * Returns an empty queue descriptor if one is available.
 * NOTE: some of the fields may have residual garbage.
 * It is guaranteed that the length field will be correct
 * and the callback field set to 0x0
 ****************************************************************************/
DRAM_CODE t_q_descr *BTq_Enqueue(u_int8 q_type, t_deviceIndex device_index, t_length);

/*****************************************************************************
 * BTq_Commit
 *
 * This function commits an entry onto a queue by atomically increasing its
 * entry count.
 * Only after this function is executed will the entry become visible on the 
 * queue. This is to prevent another source attempting to remove the entry 
 * before its contents have been fully written.
 ****************************************************************************/
DRAM_CODE void BTq_Commit(u_int8 q_type, t_deviceIndex device_index);

/*****************************************************************************
 * BTq_Dequeue_Next
 *
 * Dequeue an entry from the selected queue
 ****************************************************************************/
RAM_CODE t_q_descr *BTq_Dequeue_Next(u_int8 q_type, t_deviceIndex device_index);

/*****************************************************************************
 * BTq_BB_Dequeue_Next
 *
 * Special function for baseband dequeue: Additional arguement is a bitmap of 
 * packet types.
 * This function will select the most appropriate depending on the types 
 * specified and the state of the queue in question.
 *
 * All baseband queues have an associated device_index
 ****************************************************************************/
RAM_CODE t_q_descr *BTq_BB_Dequeue_Next(
    u_int8 q_type, t_deviceIndex device_index, t_packetTypes in_packet_types);
#ifdef BT_DUALMODE

t_q_descr *BTq_LE_Dequeue_Next(u_int8 q_type, t_deviceIndex device_index, u_int16 length );
#endif
/*****************************************************************************
 * BTq_Dequeue_Next_Data_By_Length
 *
 * Special function for HC to HOST Data dequeuing. 
 * Additional argument a maximum length to segment to.
 *
 * q_type               Normally L2CAP_IN_Q
 * device_index         Associated device index for the q_type
 * max_packet_length    If not 0 Then Segment to presented length
 *
 ****************************************************************************/
DRAM_CODE t_q_descr *BTq_Dequeue_Next_Data_By_Length(
    u_int8 q_type, t_deviceIndex device_index, u_int16 max_packet_length);

/*****************************************************************************
 * BTq_Ack_Last_Dequeued
 *
 * Acknowledge dequeue entry, removing entry from queue.
 ****************************************************************************/
DRAM_CODE void BTq_Ack_Last_Dequeued(
    u_int8 q_type, t_deviceIndex device_index, t_length length);


/*
 * Flow control operations
 */
/*****************************************************************************
 * BTq_Get_Total_Number_Of_Processed_Chunks
 *
 * Get the number of processed chunks
 * For outgoing BB L2CAP queues, processed means transmitted/flushed.
 * For incoming BB L2CAP queues, processed means received into queues
 ****************************************************************************/
u_int16 BTq_Get_Total_Number_Of_Processed_Chunks(u_int8 q_type);

/*****************************************************************************
 * BTq_Get_Queue_Number_Of_Processed_Chunks
 *
 * Get the number of processed chunks on queue.
 ****************************************************************************/
u_int16 BTq_Get_Queue_Number_Of_Processed_Chunks(
    u_int8 q_type, t_deviceIndex device_index);

/*****************************************************************************
 * BTq_Get_and_Reset_Queue_Number_Of_Processed_Chunks
 *
 * Get the number of processed chunks on queue.
 * Reset the number of processed chunks on queue to 0
 * Decrement the associated total of processed chunks.
 ****************************************************************************/
u_int16 BTq_Get_and_Reset_Queue_Number_Of_Processed_Chunks(
    u_int8 q_type, t_deviceIndex device_index);

/*****************************************************************************
 * BTq_Reduce_Queue_Number_Of_Processed_Chunks
 *
 * Reduce the number of queue processed chunks and the associated total
 * of processed chunks for the given queue type by reduction
 *
 * Currently only L2CAP Data queues supported.
 ****************************************************************************/
DRAM_CODE void BTq_Reduce_Queue_Number_Of_Processed_Chunks(
    u_int8 q_type, t_deviceIndex device_index, u_int16 reduction);

/*****************************************************************************
 * BTq_Deallocate_Queue_Data
 *
 * Deallocate data associated with specified queue.
 ****************************************************************************/
void BTq_Deallocate_Queue_Data(u_int8 q_type, t_deviceIndex device_index);
void BTq_Release_All_SYN_Queue_Packets(t_deviceIndex sco_q_index);

/*****************************************************************************
 * BTq_Initialise_Data_Queue
 *
 * Can be used to Initialise any data queue. More specifically, use to reset 
 * SCO and eSCO queues.
 *
 * Currently unused.
 ****************************************************************************/
void BTq_Initialise_Data_Queue(unsigned int q_index);


void BTq_Enhanced_Flush(t_deviceIndex device_index,u_int8 current_entry_flush_type,u_int16* p_current_pdu_remaining);

/*****************************************************************************
 * BTq_Change_Endianness_If_Required
 *
 * Convert data in-situ from little to big endian or vice versa
 *
 * Expected usage is for big endian systems where the data is converted
 * before enqueue or after dequeue (and not in baseband).
 ****************************************************************************/
#if (PRH_BS_DEV_BIG_ENDIAN_AND_DIRECT_BASEBAND_MOVE==1)
void BTq_Change_Endianness_If_Required(u_int8 *p_data, u_int16 length);
#else
/*
 * Function call does nothing
 */
#define BTq_Change_Endianness_If_Required(p_data, length)    
#endif


RAM_CODE u_int16 BTq_BB_Dequeue_Next_Synchronous(u_int8 q_type, u_int8 sco_index, 
                                   u_int16 packet_length, u_int8 *buffer);

RAM_CODE u_int16 BTq_Get_Total_Bytes_Enqueued(u_int8 q_type, t_deviceIndex device_index);

 /*****************************************************************************
 * BTq_Get_Queue_Number_Of_Entries
 *
 * Get the number of entries on queue.
 ****************************************************************************/
#define BTq_Num_Entry(q, di) BTq_Get_Queue_Number_Of_Entries(q, di)
u_int16 BTq_Get_Queue_Number_Of_Entries(
    u_int8 q_type, t_deviceIndex device_index);

#endif /* _PARTHUS_TRA_QUEUE_H_ */
