/*******************************************************************************
 *                              US280A
 *                            Module: Key driver
 *                 Copyright(c) 2003-2013 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       johnsen     2013-5-18 10:28    1.0             build this file
 *******************************************************************************/

#include "../key_inner.h"

/* 命令带有参数，参数通过param传递进来，会将参数放到堆中 */
bool key_inner_rcp_queue_send_rcode(rmt_ctrl_pkg_t *p_rcp)
{
    rmt_ctrl_pkg_t *p_cur_rcp;

    if(key_rcp_queue_depth_empty == 0) {
        //key_rcp_queue_status = KEY_RCP_QUEUE_IS_EMPTY;
        return FALSE;
    }
    //cur rcp ptr
    p_cur_rcp = (rmt_ctrl_pkg_t *)key_rcp_queue_p + key_rcp_queue_writeptr;
    //data
    p_cur_rcp->rcp_data = p_rcp->rcp_data;
    //lenth
    p_cur_rcp->rcp_len = p_rcp->rcp_len;
    //dep
    if(key_rcp_queue_depth != 0) {
        //write
        key_rcp_queue_writeptr = (key_rcp_queue_writeptr + 1) % key_rcp_queue_depth;
    }
    //depth dec
    key_rcp_queue_depth_empty--;

    return TRUE;
}

/* SPP命令队列是否为空 */
bool key_inner_rcp_queue_is_empty(void *null0, void *null1, void *null2)
{
    //#if (SUPPORT_SPP_COMMAND == 1)
    return (key_rcp_queue_depth_empty >= key_rcp_queue_depth);
    //#else
    //return TRUE;
    //#endif
}


/*从RCP队列堆空间中申请空间*/
void *key_inner_rcp_malloc(uint32 num_bytes)
{
#if (SUPPORT_REMOTE_CONTROL == 1)
    key_rcp_alloc_t *ptr, *newptr;

    /* Round it to a multiple of the pagesize */
    num_bytes = LIBC_MY_BYTE_ALIGN(num_bytes);

    ptr = &head;
    while(ptr && (ptr->next)) {
        if(ptr->next->address >= (ptr->address + ptr->size + 2 * sizeof(key_rcp_alloc_t) + num_bytes)) {
            break; /* enough space */
        }
        ptr = ptr->next;
    }
    /* make sure if goto tail */
    if((ptr == NULL) || (ptr->next == NULL)) {
        return NULL; /* end of list */
    }

    newptr = (key_rcp_alloc_t *)(ptr->address + ptr->size);
    if(newptr == NULL) {
        return NULL;
    }

    /* ok, now stick it after ptr */
    newptr->address = ptr->address + ptr->size + sizeof(key_rcp_alloc_t);
    newptr->size = num_bytes;
    newptr->next = ptr->next;
    if((newptr->address + num_bytes) > tail.address) {
        return NULL;
    }

    ptr->next = newptr;
    return (void *) newptr->address;
#else
    return NULL;
#endif
}

/*释放空间到RCP队列堆空间中*/
bool key_inner_rcp_free(void *alloc_addr)
{
#if (SUPPORT_REMOTE_CONTROL == 1)
    key_rcp_alloc_t *ptr, *prev;

    ptr = &head;
    while(ptr->next != NULL) {
        if(ptr->next->address == alloc_addr) {
            break;
        }
        ptr = ptr->next;
    }

    /* make sure if goto tail */
    if((ptr->address == tail.address) || (ptr->next == NULL)) {
        return FALSE;
    }

    /* the one being freed is ptr->next */
    prev = ptr;
    ptr = ptr->next;
    prev->next = ptr->next;
#endif

    return TRUE;
}


