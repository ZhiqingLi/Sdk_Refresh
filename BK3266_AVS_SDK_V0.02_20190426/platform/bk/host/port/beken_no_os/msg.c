#include <string.h>
#include <stdlib.h>

#include "port_mem.h"
#include "msg_pub.h"
#include "os_port.h"
#include "msg.h"
#include "driver_icu.h"
#include "timer.h"

MESSAGE_ENTITY_T msg_entity;

static int msg_check(uint32 id)
{
    int i;
    MSG_T *element_ptr;
  
    for (i = msg_entity.out_id; i < msg_entity.in_id; i++) 
    {
        element_ptr = &(msg_entity.pool[i & (MSG_COUNT - 1)]);

        if (element_ptr->id == id)
            return 1;
    }

    return 0;
}

void msg_init(void)
{
    uint8 *temp;
    uint32 *magic_hdr;
    uint32 *magic_tail;

    MSG_PRT("msg_init\r\n");
    memset(&msg_entity, sizeof(MESSAGE_ENTITY_T), 0);
    
    temp = (uint8 *)jmalloc((sizeof(MSG_T) * MSG_COUNT + MSG_SECURITY_BOUNDARY_NUM + MSG_SECURITY_BOUNDARY_NUM), M_ZERO);
    msg_entity.borderline = (MSG_T *)temp;
    
    magic_hdr = (uint32 *)temp;
    msg_entity.pool = (MSG_T *)((uint32)temp + MSG_SECURITY_BOUNDARY_NUM);
    magic_tail = (uint32 *)((uint32)temp + sizeof(MSG_T) * MSG_COUNT + MSG_SECURITY_BOUNDARY_NUM);
    
    msg_entity.count  = MSG_COUNT;
    msg_entity.out_id = 0;
    msg_entity.in_id  = 0;
    msg_entity.status = MSG_POOL_INIT;
    
    *magic_hdr  = MSG_HDR_MAGIC_WORD;
    *magic_tail = MSG_TAIL_MAGIC_WORD;

    #ifdef DELAY_MSG_PUT
    jtask_init(&msg_entity.msg_task, J_TASK_TIMEOUT);
    if(UWE_NOMEM == (result_t)msg_entity.msg_task)
    {
        FATAL_PRT("msg_task_init_exception\r\n");
    }
    #endif
}

void msg_uninit(void)
{
    msg_clear_all();
    
    MSG_PRT("msg_uninit\r\n");
    jfree(msg_entity.borderline);
    msg_entity.borderline = 0;
    msg_entity.pool       = 0;
    
    msg_entity.status     = MSG_POOL_UNINIT;
    
    #ifdef DELAY_MSG_PUT
    jtask_stop(msg_entity.msg_task);
    jtask_uninit(msg_entity.msg_task);
    msg_entity.msg_task = 0;
    #endif
}

void msg_put(uint32 id)
{
    MSG_T msg;

    msg.id  = id;
    msg.arg = 0;
    msg.hdl = 0;

    msg_lush_put(&msg);
}

int msg_lush_put(MSG_T *msg_ptr)
{
    MSG_T *element_ptr;
	uint32 interrupts_info, mask;

    if(0 == msg_ptr)
    {
        MSG_PRT("msg_lush_put_null_ptr\r\n");
    }

    //If MSG_ENV_WRITE_ACTION is already in, skip put
    if (msg_check(MSG_ENV_WRITE_ACTION) && (msg_ptr->id == MSG_ENV_WRITE_ACTION))
        return MSG_SUCCESS_RET;
    
    //MSG_PRT("msg:%p\r\n", msg_ptr->id);
    VICMR_disable_interrupts(&interrupts_info, &mask);
    /* step 0: check if the pool is full*/
    if(MSG_POOL_FULL == msg_entity.status)
    {
        VICMR_restore_interrupts(interrupts_info, mask);
        static uint8 cnt=0;
        if(cnt < 10)
        {
            cnt++;
            MSG_PRT("f\r\n"); 
        }
		
        return MSG_FAILURE_RET;
    }
    
    /* step 1: put the msg into container*/
    element_ptr = &(msg_entity.pool[msg_entity.in_id & (MSG_COUNT - 1)]);
    memcpy(element_ptr, msg_ptr, sizeof(MSG_T));
    
    /* step 3: update the variable*/
    msg_entity.in_id += 1;

    if((msg_entity.in_id & (MSG_COUNT - 1)) == (msg_entity.out_id & (MSG_COUNT - 1)))
    {
        msg_entity.status = MSG_POOL_FULL;
    }
    else
    {
        msg_entity.status = MSG_POOL_ADSUM;
    }
    
    VICMR_restore_interrupts(interrupts_info, mask);

    return MSG_SUCCESS_RET;
}

#ifdef DELAY_MSG_PUT
void msg_put_func(void *id) {
    msg_put(*(uint32 *)id);
}

int msg_delay_put(uint32 os_tick_count, MSG_T *msg_ptr)
{
    uint32 msec;
    
    if(jtask_is_pending(msg_entity.msg_task))
    {
        return MSG_FAILURE_RET;
    }

    msec = os_tick_count * OS_TICK_MSEC;
    memcpy(&msg_entity.arg, msg_ptr, sizeof(MSG_T));
    jtask_schedule(msg_entity.msg_task, 
    		        msec,
                    msg_put_func, 
                    (void *)&msg_entity.arg);

    return MSG_SUCCESS_RET;
}

#endif

int msg_priority_put(MSG_T *msg_ptr)
{
    MSG_T *element_ptr;
	uint32 interrupts_info, mask;
    
    VICMR_disable_interrupts(&interrupts_info, &mask);
    
    /* step 0: check if the pool is full*/
    if(MSG_POOL_FULL == msg_entity.status)
    {
        VICMR_restore_interrupts(interrupts_info, mask);
        
        MSG_PRT("pmsg_pool_full\r\n");
        
        return MSG_FAILURE_RET;
    }

    /* step 1: add msg into pool*/
    msg_entity.out_id -= 1;    
    element_ptr = &(msg_entity.pool[msg_entity.out_id & (MSG_COUNT - 1)]);
    memcpy(element_ptr, msg_ptr, sizeof(MSG_T));
    
    /* step 2: update varaible*/
    if((msg_entity.in_id & (MSG_COUNT - 1)) == (msg_entity.out_id & (MSG_COUNT - 1)))
    {
        msg_entity.status = MSG_POOL_FULL;
    }
    else
    {
        msg_entity.status = MSG_POOL_ADSUM;
    }
    
    VICMR_restore_interrupts(interrupts_info, mask);

    return MSG_SUCCESS_RET;
}

RAM_CODE int msg_get(MSG_T *msg_ptr)
{
    MSG_T *element_ptr;
	uint32 interrupts_info, mask;
    
    VICMR_disable_interrupts(&interrupts_info, &mask);
    /* step 0: check if the pool is empty*/
    if(MSG_POOL_EMPTY == msg_entity.status)
    {
        VICMR_restore_interrupts(interrupts_info, mask);

        return MSG_FAILURE_RET;
    }

    /* step 1: put the msg into container*/
    element_ptr = &(msg_entity.pool[msg_entity.out_id & (MSG_COUNT - 1)]);
    memcpy(msg_ptr, element_ptr, sizeof(MSG_T));
    
    /* step 3: update the variable*/
    msg_entity.out_id += 1;

    if((msg_entity.in_id & (MSG_COUNT - 1)) == (msg_entity.out_id & (MSG_COUNT - 1)))
    {
        msg_entity.status = MSG_POOL_EMPTY;
    }
    else
    {
        msg_entity.status = MSG_POOL_ADSUM;
    }
    
    VICMR_restore_interrupts(interrupts_info, mask);

    return MSG_SUCCESS_RET;
}

uint32 msg_get_pool_status(void)
{
    return msg_entity.status;
}

void msg_clear_all(void)
{
	uint32 interrupts_info, mask;

    MSG_PRT("msg_clear_all\r\n");
    VICMR_disable_interrupts(&interrupts_info, &mask);
    msg_entity.out_id = 0;
    msg_entity.in_id  = 0;
    msg_entity.status = MSG_POOL_EMPTY;
    VICMR_restore_interrupts(interrupts_info, mask);
}

void msg_dump(void)
{
    uint32 i;
    MSG_T *element_ptr;    

    element_ptr = msg_entity.pool;
    for(i = 0; i < MSG_COUNT; i ++)
    {
        MSG_PRT("msg:%d, %p:%p:%p\r\n", i, element_ptr[i].id, element_ptr[i].hdl, element_ptr[i].arg);
    }

    MSG_PRT("msg-in:%d:%d out:%d:%d\r\n", msg_entity.in_id
                                        , msg_entity.out_id
                                        , msg_entity.in_id & (MSG_COUNT - 1)
                                        , msg_entity.out_id & (MSG_COUNT - 1));
    
    MSG_PRT("msg-border:%p pool:%p\r\n", msg_entity.borderline, msg_entity.pool);
}

uint32 msg_pool_is_polluted(void)
{
    uint8 *temp;
    uint32 *magic_hdr;
    uint32 *magic_tail;

    temp = (uint8 *)msg_entity.borderline;
    
    magic_hdr = (uint32 *)temp;
    magic_tail = (uint32 *)((uint32)temp + sizeof(MSG_T) * MSG_COUNT + MSG_SECURITY_BOUNDARY_NUM);
    
    return ((MSG_HDR_MAGIC_WORD != *magic_hdr) 
            || (MSG_TAIL_MAGIC_WORD != *magic_tail));
}
// EOF
