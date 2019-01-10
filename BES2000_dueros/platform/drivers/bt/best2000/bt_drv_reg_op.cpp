#include "cmsis.h"
#include "bt_drv_reg_op.h"
#include "bt_drv_internal.h"
#include "bt_drv_interface.h"
#include "bt_drv.h"
#include "hal_chipid.h"
#include "hal_trace.h"


#define BT_DRIVER_GET_U8_REG_VAL(regAddr)		(*(uint8_t *)(regAddr))
#define BT_DRIVER_GET_U16_REG_VAL(regAddr)		(*(uint16_t *)(regAddr))
#define BT_DRIVER_GET_U32_REG_VAL(regAddr)		(*(uint32_t *)(regAddr))

#define BT_DRIVER_PUT_U8_REG_VAL(regAddr, val)		*(uint8_t *)(regAddr) = (val)
#define BT_DRIVER_PUT_U16_REG_VAL(regAddr, val)		*(uint16_t *)(regAddr) = (val)
#define BT_DRIVER_PUT_U32_REG_VAL(regAddr, val)		*(uint32_t *)(regAddr) = (val)

void bt_drv_reg_op_rssi_set(uint16_t rssi)
{
    if(hal_get_chip_metal_id() >= 2){
        *(volatile uint16_t*)(0xc0003bec) = 0;         //rssi low
    }
}

void bt_drv_reg_op_scan_intv_set(uint32_t scan_intv)
{
    *(volatile uint32_t *)0xc000566c = scan_intv;        
}

void bt_drv_reg_op_encryptchange_errcode_reset(uint16_t hci_handle)
{
    uint32_t error_code_address = (0xc000531b +(hci_handle -0x80)*0x210);
    TRACE("EncryptChange error checker code= %x",*(volatile uint8_t *)(error_code_address));
    if(*(volatile uint8_t *)(error_code_address) != 0)
    {
        *(volatile uint8_t *)(error_code_address) = 0;
    }
}

void bt_drv_reg_op_sco_checker(void)
{
    TRACE("MASTER CHANMAP=:");
    DUMP8("%02x",(unsigned char *)0xc0004c10,10);
    TRACE("\n instant=%x,mode=%x,SCO silence REG=%x,%x",*(unsigned int *)0xc0004c1c,*(unsigned int *)0xc0004c20,*(volatile uint16_t *)(0xd0210214),*(volatile uint16_t *)(0xd0210216));
}

void bt_drv_reg_op_trigger_time_checker(void)
{
    TRACE("0xd0224024 = %x,0xd022007c=%x",*(volatile uint32_t *)0xd0224024,*(volatile uint32_t *)0xd022007c);
    TRACE("0xd02201b4 = %x,0xd02201cc=%x",*(volatile uint32_t *)0xd02201b4,*(volatile uint32_t *)0xd02201cc);
}

void bt_drv_reg_op_tws_output_power_fix_separate(uint16_t hci_handle, uint16_t pwr)
{
    *(volatile uint32_t *)(0xd02101a4+btdrv_conhdl_to_linkid(hci_handle)) = pwr;
}


#define SNIFF_IN_SCO    2

bool bt_drv_reg_op_ld_sniffer_env_monitored_dev_state_get(void)
{
    uint8_t val = *(volatile uint8_t *)0xc0004bfe;

    bool nRet = false;
    
    if (val & SNIFF_IN_SCO){
        nRet = true;
    }else{
        nRet = false;
    }
    TRACE("monitored_dev_state %d", nRet);

    return nRet;
}

void bt_drv_reg_op_ld_sniffer_env_monitored_dev_state_set(bool state)
{
    uint8_t val = *(volatile uint8_t *)0xc0004bfe;

    if (state){        
        val |= (uint8_t)(SNIFF_IN_SCO);
    }else{
        val &= (uint8_t)(~SNIFF_IN_SCO);
    }
    *(volatile uint8_t *)0xc0004bfe = val;
}

int bt_drv_reg_op_currentfreeaclbuf_get(void)
{
    return *(volatile uint16_t *)0xC0005D48;
}


static uint16_t app_tws_original_mobile_airpath_info;

void bt_drv_reg_op_save_mobile_airpath_info(uint16_t hciHandle)
{
    uint8_t link_id = btdrv_conhdl_to_linkid(hciHandle);
    app_tws_original_mobile_airpath_info = BT_DRIVER_GET_U16_REG_VAL(0xd0210192 + 96*link_id);
    TRACE("org mobile air info 0x%x ", app_tws_original_mobile_airpath_info);
}

void bt_drv_reg_op_block_xfer_with_mobile(uint16_t hciHandle)
{
    uint8_t link_id = btdrv_conhdl_to_linkid(hciHandle);
    BT_DRIVER_PUT_U16_REG_VAL((0xd0210192 + 96*link_id), app_tws_original_mobile_airpath_info^0x8000);

    TRACE("Set hci handle 0x%x as 0x%x", hciHandle, app_tws_original_mobile_airpath_info^0x8000);
}

void bt_drv_reg_op_resume_xfer_with_mobile(uint16_t hciHandle)
{
    uint8_t link_id = btdrv_conhdl_to_linkid(hciHandle);
    BT_DRIVER_PUT_U16_REG_VAL((0xd0210192 + 96*link_id), app_tws_original_mobile_airpath_info);
}

int bt_drv_reg_op_packet_type_checker(uint16_t hciHandle)
{
    if (hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_2){
#if  IS_ENABLE_BT_DRIVER_REG_DEBUG_READING    
        bt_drv_reg_op_connection_checker();
#endif
        uint8_t link_id = btdrv_conhdl_to_linkid(hciHandle);
        uint32_t table = BT_DRIVER_GET_U32_REG_VAL(0xc0003df8);
        uint32_t len = BT_DRIVER_GET_U32_REG_VAL(0xc0003e04);
        uint16_t table_id = 0;
        
        if(link_id ==0){
            table_id = table & 0xffff;
        }else if(link_id==1){
            table_id = table >>16;
        }
        
        if(table_id == 0x8 || table_id ==0xa){
            TRACE("TX TABLE IS ONLY 1SLOT PACKET !!!");
            if(link_id ==0){
                len &= 0xffff0000;
                len |= 0x2a7;
                table |= 0x110a;
                BT_DRIVER_PUT_U32_REG_VAL(0xc0003e04, len);
                BT_DRIVER_PUT_U32_REG_VAL(0xc0003df8, table);
            }else if(link_id ==1){
                len &= 0xffff;
                len |= 0x2a70000;
                table |= 0x110a0000;
                BT_DRIVER_PUT_U32_REG_VAL(0xc0003e04, len);
                BT_DRIVER_PUT_U32_REG_VAL(0xc0003df8, table);        
            }
        }   
    }

    return 0;
}

void bt_drv_reg_op_max_slot_setting_checker(uint16_t hciHandle)
{
#if IS_ENABLE_BT_DRIVER_REG_DEBUG_READING    
    bt_drv_reg_op_connection_checker();
#endif
    if (hciHandle<0x80 || hciHandle>0x83)
    {
        return;
    }

    uint8_t link_id = btdrv_conhdl_to_linkid(hciHandle); 
    uint32_t table = BT_DRIVER_GET_U32_REG_VAL(0xc0003df8);
    uint32_t len = BT_DRIVER_GET_U32_REG_VAL(0xc0003e04);
    uint16_t table_id = 0xFFFF;
    if(link_id == 0)
    {
        table_id = table &0xffff;
    }
    else if(link_id == 1)
    {
        table_id = table >>16;
    }
    
    if(table_id == 0x8 || table_id ==0xa)
    {
        TRACE("TX TABLE IS ONLY 1 SLOT PACKET");
        if(link_id ==0)
        {
            len &= 0xffff0000;
            len |= 0x2a7;
            table |= 0x110a;
            BT_DRIVER_PUT_U32_REG_VAL(0xc0003e04, len);
            BT_DRIVER_PUT_U32_REG_VAL(0xc0003df8, table);
        }
        else if(link_id ==1)
        {
            len &= 0xffff;
            len |= 0x2a70000;
            table |= 0x110a0000;
            BT_DRIVER_PUT_U32_REG_VAL(0xc0003e04, len);
            BT_DRIVER_PUT_U32_REG_VAL(0xc0003df8, table);       
        }
    }
}

void bt_drv_reg_op_force_task_dbg_idle(void)
{
    BT_DRIVER_PUT_U32_REG_VAL(0xC00060FC, 1);
}

void bt_drv_reg_op_afh_follow_mobile_mobileidx_set(uint16_t hciHandle)
{
    if (hciHandle == 0xff){
        BT_DRIVER_PUT_U32_REG_VAL(0xc00062d4, 0xff);
    }else{
        BT_DRIVER_PUT_U32_REG_VAL(0xc00062d4, btdrv_conhdl_to_linkid(hciHandle));
    }
}

void bt_drv_reg_op_afh_follow_mobile_twsidx_set(uint16_t hciHandle)
{
    if (hciHandle == 0xff){
        BT_DRIVER_PUT_U32_REG_VAL(0xc00062d0, 0xff);
    }else{
        BT_DRIVER_PUT_U32_REG_VAL(0xc00062d0, btdrv_conhdl_to_linkid(hciHandle));
    }
}

void bt_drv_reg_op_connection_checker(void)
{
#if 0
    TRACE("MAX SLOT=%x", BT_DRIVER_GET_U32_REG_VAL(0xc0003df8));
    TRACE("MAX LEN=%x", BT_DRIVER_GET_U32_REG_VAL(0xc0003e04));
    
    TRACE("hdr=%x", BT_DRIVER_GET_U32_REG_VAL(0xd02101b4));
    TRACE("hdr1=%x", BT_DRIVER_GET_U32_REG_VAL(0xd02101b4+96));
#endif
}

extern "C" uint32_t hci_current_left_tx_packets_left(void);
extern "C" uint32_t hci_current_left_rx_packets_left(void);

void bt_drv_reg_op_bt_info_checker(void)
{
    uint32_t *buf_ptr;
    uint8_t buf_count=0;

    bt_drv_reg_op_connection_checker();

#if 1
    TRACE("tx flow=%x",*(uint32_t *)0xc0003f9c);
    TRACE("remote_tx flow=%x",*(uint32_t *)0xc0003fa2);
#endif
    
    
    TRACE("tx buff = %x %x",*(uint16_t *)0xc0005d48,hci_current_left_tx_packets_left());
    buf_ptr = (uint32_t *)0xc0003af0;
    buf_count = 0;
    while(*buf_ptr)
    {
        buf_count++;
        buf_ptr = (uint32_t *)(*buf_ptr);
    }
    
    TRACE("rxbuff = %x %x",buf_count,hci_current_left_rx_packets_left());
    
    
    buf_ptr = (uint32_t *)0xc0003af4;
    buf_count = 0;
    while(*buf_ptr)
    {
        buf_count++;
        buf_ptr = (uint32_t *)(*buf_ptr);
    }
    TRACE("air rxbuff = %x",buf_count);

}

uint32_t *rxbuff;
uint16_t *send_count;
uint16_t *free_count;
uint16_t *bt_send_count;

void bt_drv_reg_op_ble_buffer_cleanup(void)
{
    uint32_t lock;
#if 1
    uint8_t i;
    //if(param->reason == 8)
    {
        for(i=0;i<6;i++)
        {
             rxbuff = (uint32_t *)(0xd0211b64+270*i);
             TRACE("rxbuff=%x,rxd=%x",rxbuff,*rxbuff);
             if(*rxbuff & 0x8000)
             {
                *rxbuff &= (~0x8000);
                 TRACE("rxbuff=%x,rxd=%x",rxbuff,*rxbuff);
                
             }
        }
    }

#if 1 ///def best2000
    send_count = (uint16_t *)0xc0005d52;
    free_count = (uint16_t *)0xc0005d48;
    bt_send_count = (uint16_t *)0xC0005d4a;
#else
    send_count = (uint16_t *)0xc00059ba;
    free_count = (uint16_t *)0xc00059b0;
#endif
    TRACE("send_count = %x %x %x %x %x",*send_count,*(send_count+1),*(send_count+2),*(send_count+3),*(send_count+4));
//    TRACE("bt_send_count = %x %x %x %x",*bt_send_count,*(bt_send_count+1),*(bt_send_count+2),*(bt_send_count+3),*(bt_send_count+4));
     lock = int_lock();
    *free_count += *send_count;
    *free_count += *(send_count+1);
    *free_count += *(send_count+2);
    *free_count += *(send_count+3);
    *free_count += *(send_count+4);

    *send_count = 0;
    *(send_count+1) = 0;
    *(send_count+2) = 0;
    *(send_count+3) = 0;
    *(send_count+4) = 0;
    int_unlock(lock);
    if(*free_count > 6)
    {
        TRACE("ERROR!! FREE COUNT");
    }
    
    TRACE("free count= %x",*free_count);
#endif
}

#define BT_CONTROLLER_CRASH_DUMP_ADDR_BASE  (0xc00064cc)

struct ke_timer
{
    /// next ke timer
    struct ke_timer *next;
    /// message identifier
    uint16_t     id;
    /// task identifier
    uint16_t    task;
    /// time value
    uint32_t    time;
};

struct co_list_hdr
{
    /// Pointer to next co_list_hdr
    struct co_list_hdr *next;
};

/// structure of a list
struct co_list_con
{
    /// pointer to first element of the list
    struct co_list_hdr *first;
    /// pointer to the last element
    struct co_list_hdr *last;

    /// number of element in the list
    uint32_t cnt;
    /// max number of element in the list
    uint32_t maxcnt;
    /// min number of element in the list
    uint32_t mincnt;
};

struct mblock_free
{
    /// Next free block pointer
    struct mblock_free* next;
    /// Previous free block pointer
    struct mblock_free* previous;
    /// Size of the current free block (including delimiter)
    uint16_t free_size;
    /// Used to check if memory block has been corrupted or not
    uint16_t corrupt_check;
};

void bt_drv_reg_op_crash_dump(void)
{
    TRACE("BT controller BusFault_Handler:\nREG:[LR] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE));
    TRACE("REG:[R4] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +4));
    TRACE("REG:[R5] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +8));
    TRACE("REG:[R6] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +12));
    TRACE("REG:[R7] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +16));
    TRACE("REG:[R8] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +20));
    TRACE("REG:[R9] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +24));
    TRACE("REG:[sl] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +28));
    TRACE("REG:[fp] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +32));
    TRACE("REG:[ip] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +36));
    TRACE("REG:[SP,#0] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +40));
    TRACE("REG:[SP,#4] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +44));
    TRACE("REG:[SP,#8] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +48));
    TRACE("REG:[SP,#12] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +52));
    TRACE("REG:[SP,#16] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +56));
    TRACE("REG:[SP,#20] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +60));
    TRACE("REG:[SP,#24] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +64));
    TRACE("REG:[SP,#28] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +68));
    TRACE("REG:[SP,#32] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +72));
    TRACE("REG:[SP,#36] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +76));
    TRACE("REG:[SP,#40] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +80));
    TRACE("REG:[SP,#44] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +84));
    TRACE("REG:[SP,#48] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +88));
    TRACE("REG:[SP,#52] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +92));
    TRACE("REG:[SP,#56] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +96));
    TRACE("REG:[SP,#60] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +100));
    TRACE("REG:[SP,#64] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +104));
    TRACE("REG:[cfsr] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +108));
    TRACE("REG:[bfar] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +112));
    TRACE("REG:[hfsr] = 0x%08x", BTDIGITAL_REG(BT_CONTROLLER_CRASH_DUMP_ADDR_BASE +116));

    struct co_list_con *queue_sent = (struct co_list_con*)(0xc0003844);
    struct co_list_con *queue_saved = (struct co_list_con*)(0xc0003844 + 1*sizeof(struct co_list_con));
    struct co_list_con *queue_timer = (struct co_list_con*)(0xc0003844 + 2*sizeof(struct co_list_con));
    
    //TRACE("Plantform error =0x%x,defualt :0xabcd0000",*(volatile uint32_t *)(0xc000623c));

    //queue_sent
    TRACE("queue_sent _addr=0x%x,fist=0x%x,cnt=%x",(0xc0003844 ),queue_sent->first,queue_sent->cnt);    
    if(queue_sent->first != NULL)
    {
        while(queue_sent->first != NULL)
        {
            struct ke_timer *timer = (struct ke_timer*) queue_sent->first;
            TRACE("queue_sent = 0x%08x,Next_ptr:0x%08x, id:0x%04x time:0x%08x",timer, timer->next,timer->id,timer->task,timer->time);
            queue_sent->first = queue_sent->first->next;
        }
    }
    //queue_saved
    TRACE("queue_saved _addr=0x%x,fist=0x%x,cnt=%x",(0xc0003844 + 1*sizeof(struct co_list_con)),queue_saved->first,queue_saved->cnt);
    if(queue_saved->first != NULL)
    {
        while(queue_saved->first != NULL)
        {
            struct ke_timer *timer = (struct ke_timer*) queue_saved->first;
            TRACE("queue timer_ptr= 0x%08x,Next_ptr:0x%08x, id:0x%04x time:0x%08x",timer, timer->next,timer->id,timer->task,timer->time);
            queue_saved->first = queue_saved->first->next;
        }
    }
    //queue_timer
    TRACE("queue_timer _addr=0x%x,fist=0x%x,cnt=%x",(0xc0003844 + 2*sizeof(struct co_list_con)),queue_timer->first,queue_timer->cnt);
    if(queue_timer->first != NULL)
    {
        while(queue_timer->first != NULL)
        {
            struct ke_timer *timer = (struct ke_timer*) queue_timer->first;
            struct ke_timer *last_timer = (struct ke_timer*) queue_timer->last;

            TRACE("timer first_ptr= 0x%08x,val:0x%x,0x%x ,Next:0x%08x, id:0x%04x time:0x%08x",timer, 
               *(uint32_t*)((uint8_t*)timer - 4),*(uint32_t*)((uint8_t*)timer - 8),
                timer->next,timer->id,timer->task,timer->time);
            TRACE("timer last_ptr= 0x%08x,val:0x%x,0x%x ,Next:0x%08x, id:0x%04x time:0x%08x",last_timer,
               *(uint32_t*)((uint8_t*)last_timer - 4),*(uint32_t*)((uint8_t*)last_timer - 8),
                last_timer->next,last_timer->id,last_timer->task,last_timer->time);
               
            queue_timer->first = queue_timer->first->next;
        }
    }
    
    struct mblock_free * heap0 = ( struct mblock_free *)0xc00014cc;
    struct mblock_free * heap1 = ( struct mblock_free *)0xc0001f00;
    struct mblock_free * heap2 = ( struct mblock_free *)0xc0003438;
       
    uint16_t * heap_size = (uint16_t*)(0xc0003844 + 3*sizeof(struct co_list_con) + 3*sizeof(struct mblock_free*));
    uint16_t * heap_used = (uint16_t*)(0xc0003844 + 3*sizeof(struct co_list_con) + 3*sizeof(struct mblock_free*) + 3*sizeof(uint16_t));
    uint32_t * max_heap_used = (uint32_t*)(0xc0003844 + 3*sizeof(struct co_list_con) + 6*sizeof(struct mblock_free*));
    TRACE("heap0 next =0x%08x, free size=%x,corrupt_check=%x",heap0->next,heap0->free_size,heap0->corrupt_check);
    TRACE("heap1 next =0x%08x, free size=%x,corrupt_check=%x",heap1->next,heap1->free_size,heap1->corrupt_check);
    TRACE("heap2 next =0x%08x, free size=%x,corrupt_check=%x.\nmax_heap_used=0x%x ",heap2->next,heap2->free_size,heap2->corrupt_check,*max_heap_used);
    
    if(heap1 != NULL)
    {
        while(heap1->next!= NULL){
                struct mblock_free *node = (struct mblock_free*) heap1->next;
                TRACE("heap1->next_ptr= 0x%08x, free_size:0x%04x corrupt_check:0x%04x", node->next,node->free_size,node->corrupt_check);
                heap1->next = heap1->next->next;
       }
        
        while(heap1->previous!= NULL){
                struct mblock_free *node = (struct mblock_free*) heap1->previous;
                TRACE("heap1->previous_ptr= 0x%08x, free_size:0x%04x corrupt_check:0x%04x", node->next,node->free_size,node->corrupt_check);
                heap1->previous = heap1->previous->previous;
        }
    }

    for(int i=0;i<3;i++)
    {
        TRACE("heap[%d] size=0x%x, used=0x%x",i,heap_size[i],heap_used[i]);
    }
}

