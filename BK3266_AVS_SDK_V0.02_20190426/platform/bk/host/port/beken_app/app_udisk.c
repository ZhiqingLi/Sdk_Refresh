
#include "includes.h"
#include "driver_beken_includes.h"
#include "app_beken_includes.h"

#ifdef CONFIG_APP_USB_DISK
UDISK_T udisk = {
	0,                               // media_exist_flag
	0,                               // cbw_processed
	0,                               // zero_terminate
		{{0}},
		{
		   0, 0x80, 0, 0x01, 0x1F, 
		   {0, 0, 0}, 
		   /* Vendor information: "BEKEN     " */
		   {'B', 'E', 'K', 'E', 'N', 0x20, }, 
		   /* Product information: "Disk            " */
		   {0x44, 0x69, 0x73, 0x6B, 0x20, 0x20, 0x20, 0x20,
		    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20}, 
		   /* Product Revision level: "Demo" */
		   {0x44, 0x65, 0x6D, 0x6F}
		},
	};

uint8 *data_buf;
uint8 *udisk_data_buf;
volatile uint32 udisk_pseudo_swi = 0;

/**************************************************************************
Global variables and some defines for device.
**************************************************************************/
#ifndef DIRECT_COPY_BTWN_SD_EP_FIFO
REQ_INFO_T rd_req_info = {0};
REQ_INFO_T wr_req_info = {0};

volatile uint32 sd_reading_flag = 0;
volatile uint32 sd_writing_flag = 0;
volatile uint32 dma_write_fifo_over_flag = 0;
USE_LIST_HEAD_T use_list_head;

LIST_HEAD_DEFINE(free_list_head);

void udisk_init_list_head(void)
{
	use_list_head.count = 0;
	
	use_list_head.use_list.prev = &use_list_head.use_list;
	use_list_head.use_list.next = &use_list_head.use_list;
}

void udisk_free_node(NODE_BUF_T **node_buf_pptr)
{
    if(*node_buf_pptr)
    {
   		os_printf("uninit_node:0x%x:0x%x\r\n", (*node_buf_pptr), (*node_buf_pptr)->buf_ptr);

		jfree((*node_buf_pptr)->buf_ptr);
		(*node_buf_pptr)->buf_ptr = 0;
		
		jfree(*node_buf_pptr);
		*node_buf_pptr = 0;
    }
}

void udisk_uninit_node_list(LIST_HEADER_T *head)
{
	LIST_HEADER_T *n;
	LIST_HEADER_T *pos;
	NODE_BUF_T *node_buf_ptr;

    if(list_empty(head))
    {        
    	UDISK_PRT("list_head:%p empty\r\n", head);
        return;
    }
    if((0 == head->next)
        ||(0 == head->prev))
    {        
    	UDISK_PRT("list_head:%p uninit\r\n", head);
        return;
    }
    
	list_for_each_safe(pos, n, head)
	{		
		list_del(pos);
		
        node_buf_ptr = list_entry(pos, NODE_BUF_T, node_list);
        udisk_free_node(&node_buf_ptr);
	}
}

void udisk_uninit_node_lists(void)
{
	udisk_uninit_node_list(&free_list_head);
	udisk_uninit_node_list(&use_list_head.use_list);
}
void udisk_init_access_nodes(void)
{
	int i ;
	uint8 *buf_ptr;
	NODE_BUF_T *node_buf_ptr;

	for(i = 0; i < NODE_COUNT; i ++)
	{
		/*step0: generate node*/
		buf_ptr = (uint8 *)jmalloc(LENGTH_OF_EACH_LAB, M_ZERO);
		node_buf_ptr = (NODE_BUF_T *)jmalloc(sizeof(NODE_BUF_T), M_ZERO);
		node_buf_ptr->buf_ptr = buf_ptr;

		os_printf("init_node:0x%x:0x%x\r\n", node_buf_ptr, node_buf_ptr->buf_ptr);
		/*step1:add to free list*/
		list_add(&node_buf_ptr->node_list, &free_list_head);        
	}

	UDISK_PRT("init_node\r\n");
	return;
	
fail_init:	
	
	udisk_uninit_node_lists();
	return;
}

NODE_BUF_T *udisk_get_node_from_free_list(void)
{
	LIST_HEADER_T *n;
	LIST_HEADER_T *pos;
	LIST_HEADER_T *head;
	NODE_BUF_T *node_buf_ptr;
	uint32 interrupts_info, mask;

	node_buf_ptr = 0;
	head = &free_list_head;

    VICMR_disable_interrupts(&interrupts_info, &mask);
    list_for_each_safe(pos, n, head)
    {				
        list_del(pos);
        node_buf_ptr = list_entry(pos, NODE_BUF_T, node_list);
		
		VICMR_restore_interrupts(interrupts_info, mask);
		
		return node_buf_ptr;
	}

    VICMR_restore_interrupts(interrupts_info, mask);
	return 0;
}

void udisk_put_node_on_free_list(NODE_BUF_T *node_ptr)
{
	uint32 interrupts_info, mask;
	
	VICMR_disable_interrupts(&interrupts_info, &mask);
	
	list_add_tail(&node_ptr->node_list, &free_list_head);
	
	VICMR_restore_interrupts(interrupts_info, mask);
}

NODE_BUF_T *udisk_get_node_from_use_list(void)
{
	LIST_HEADER_T *n;
	LIST_HEADER_T *pos;
	LIST_HEADER_T *head;
	NODE_BUF_T *node_buf_ptr;
	uint32 interrupts_info, mask;

	node_buf_ptr = 0;
	head = &use_list_head.use_list;
	VICMR_disable_interrupts(&interrupts_info, &mask);
	list_for_each_safe(pos, n, head)
	{				
        list_del(pos);		
        node_buf_ptr = list_entry(pos, NODE_BUF_T, node_list);
        use_list_head.count -= 1;		
		
		VICMR_restore_interrupts(interrupts_info, mask);
		
		return node_buf_ptr;
	}

    VICMR_restore_interrupts(interrupts_info, mask);
	return 0;
}

void udisk_put_node_on_use_list(NODE_BUF_T *node_ptr)
{
	uint32 interrupts_info, mask;
	
	VICMR_disable_interrupts(&interrupts_info, &mask);
	
	list_add_tail(&node_ptr->node_list, &use_list_head.use_list);
	
	use_list_head.count += 1;	
	VICMR_restore_interrupts(interrupts_info, mask);
}

uint32 udisk_produce_or_not(void)
{
	uint32 flag = 0;

	if(use_list_head.count < NODE_COUNT)
	{
		flag = 1;
	}
	
	return flag;
}

void read_media_dma_cb(void *param)
{
	NODE_BUF_T *node_buf_ptr = (NODE_BUF_T *)param;	
	
	rd_req_info.current_blk_id   += 1;
	rd_req_info.produce_blk_num  += 1;
	rd_req_info.produce_node_ptr  = 0;	
	sd_reading_flag               = 0;	
	
	/*update the sd controller*/
	BK3000_SDIO_FIFO_THRESHOLD |= SDIO_RX_FIFO_RST_BIT;
	
	udisk_put_node_on_use_list(node_buf_ptr);	
}

void read_media_cb(uint8 *buf, uint32 count)
{
	int i;
	uint32 *ptr;
	uint32 *src;
	NODE_BUF_T *node_buf_ptr;
    static uint32 mini_blk_count = 0;

    src = (uint32 *)buf;
	node_buf_ptr = rd_req_info.produce_node_ptr;
	ptr = (uint32 *)((uint32)node_buf_ptr->buf_ptr + mini_blk_count * 64);
    
    /*copy 64 bytes*/
    for(i = 0; i < (count >> 2); i ++)
	{
		ptr[i] = src[i];
	}	

    /* update mini-block-count*/
    mini_blk_count ++;

    if(8 == mini_blk_count)
    {
        mini_blk_count = 0;
            
    	rd_req_info.current_blk_id   += 1;
    	rd_req_info.produce_blk_num  += 1;
    	rd_req_info.produce_node_ptr  = 0;	
    	sd_reading_flag            = 0;
    	
    	/*update the sd controller*/
    	BK3000_SDIO_FIFO_THRESHOLD |= SDIO_RX_FIFO_RST_BIT;
    	
    	udisk_put_node_on_use_list(node_buf_ptr);	
    }

	
}

void write_media_dma_cb(void *param)
{
	UDISK_PRT("wr_dma_cb\r\n");
	
	dma_write_fifo_over_flag = 1;
}

void udisk_rx_media_cb(void *param, uint32 count)
{
	uint32 dmem_offset;
	EP_ENTITY_PTR ep_ptr = (EP_ENTITY_PTR)param;

	dmem_offset = ep_ptr->dmem_offset + (ep_ptr->cell_use_id & (ep_ptr->dmem_cell_count - 1)) * ep_ptr->dmem_cell_size;
	
	if(URB_ASYNC == ep_ptr->sync_flag)
	{
		/*read rx data*/
		udev_read_fifo(ep_ptr, 
						count, 
						(ep_ptr->data_ptr + ep_ptr->data_actual_len),
						dmem_offset);
	}
    
	if(ep_ptr->data_req_len - ep_ptr->data_actual_len > ep_ptr->mps)
	{
		/* check if dmem of ep has space*/
		if(ep_ptr->cell_use_id - ep_ptr->cell_free_id < (ep_ptr->dmem_cell_count - 1))
		{
			/* mark rxed data*/
			ep_ptr->cell_use_id += 1;
			
			/* update rx dmem*/
			udev_update_ep_fifo_config(ep_ptr, (ep_ptr->cell_use_id & (ep_ptr->dmem_cell_count - 1)));
			
			/* re-enable rx*/
			udev_enable_receive(ep_ptr);
		}
		else
		{
			UDEV_PRT("rx_susp\r\n");
			ep_ptr->urb_status = URB_RECEIVE_SUSPEND;
		}
	}
	
	ep_ptr->data_actual_len += count;
	if(ep_ptr->data_actual_len == ep_ptr->data_req_len)
	{
		udev_update_ep_fifo_config(ep_ptr, 0);
		
		ep_ptr->cell_free_id = 0;
		ep_ptr->cell_use_id  = 0;
        
		udisk_put_node_on_use_list(wr_req_info.produce_node_ptr);
		wr_req_info.produce_node_ptr   = 0;
		wr_req_info.produce_blk_num   += 1;
        
		ep_ptr->urb_status = URB_RECEIVE_OVER;
	}
}

void udisk_rx_media_data(void)
{    
	EP_ENTITY_PTR ep_ptr;
	uint32 interrupts_info, mask;
	NODE_BUF_T *node_buf_ptr;
	UDEV_PTR handle = udisk.udev_pointer;

    ep_ptr = handle->ep_esse[EP_BULK_OUT];
    if((URB_RECEIVING == ep_ptr->urb_status)
        || (0 == udisk_produce_or_not()))
    {
		UDISK_PRT("return\r\n");
		return;
	}	
	
	/*read sd card, first step is getting buffer*/
	VICMR_disable_interrupts(&interrupts_info, &mask);
	node_buf_ptr = udisk_get_node_from_free_list();	
	VICMR_restore_interrupts(interrupts_info, mask);
	
	if(0 == node_buf_ptr)
	{
		UDISK_PRT("no-node\r\n");
		return;
	}	

	/*config reading block*/
	wr_req_info.produce_node_ptr = node_buf_ptr;
	
	udev_async_receive(handle, 
							EP_BULK_OUT, 
							node_buf_ptr->buf_ptr, 
							LENGTH_OF_EACH_LAB, 
							udisk_rx_media_cb);
}

void udisk_tx_media_cb(void *param, uint32 count)
{
	uint32 deta_count;    
	REQ_INFO_T *req_ptr = &rd_req_info;
	EP_ENTITY_PTR ep_ptr = (EP_ENTITY_PTR)param;
	
	/* check this successful sending, and update some variables*/
	deta_count = ep_ptr->data_req_len - ep_ptr->data_actual_len;
	if(deta_count <= ep_ptr->mps)
	{
		ep_ptr->data_actual_len = ep_ptr->data_req_len;
	
		
		udev_update_ep_fifo_config(ep_ptr, 0);

		ep_ptr->cell_free_id = 0;
		ep_ptr->cell_use_id  = 0;
        
		/*step2: update config after sending successfully*/
		udisk_put_node_on_free_list(req_ptr->consume_node_ptr);
        req_ptr->consume_node_ptr = 0;
        
        req_ptr->consume_blk_num ++;
        ep_ptr->urb_status = URB_SEND_OVER;
		
    	/*handle over, and roll the udisk state machine*/
    	if(req_ptr->blk_num == req_ptr->consume_blk_num)
    	{    	    
    		req_ptr->access_cmd_type   = ACCESS_CMD_NULL;
    		req_ptr->blk_num           = 0;
    		udisk_pseudo_swi           = 1; 
    	}
	}
	else
	{
		ep_ptr->data_actual_len += ep_ptr->mps;
		
		ep_ptr->cell_free_id += 1;
		udev_update_ep_fifo_config(ep_ptr, ep_ptr->cell_free_id & (ep_ptr->dmem_cell_count - 1));
		udev_enable_send(ep_ptr);		
	}			
}

uint32 udisk_tx_media_data(void)
{
	UDEV_PTR handle;
	EP_ENTITY_PTR ep_ptr;
	NODE_BUF_T *node_buf_ptr;

	handle = udisk.udev_pointer;
    ep_ptr = handle->ep_esse[EP_BULK_IN];
    if(URB_SENDING == ep_ptr->urb_status)
    {
        return RET_USB_FAILURE;
    }

	/*step0: get node buf*/
	node_buf_ptr = udisk_get_node_from_use_list();
	if(node_buf_ptr)
	{
		/*step1: usb tx*/
        rd_req_info.consume_node_ptr = node_buf_ptr;
		udev_async_send(handle, 
							EP_BULK_IN, 
							node_buf_ptr->buf_ptr,
							LENGTH_OF_EACH_LAB, 
							udisk_tx_media_cb);
		
		return RET_USB_SUCCESS;
	}
    else
    {
    }
	
	return RET_USB_FAILURE;
}

void write_media_cb(uint32 *buf, uint32 count)
{
	int i;
	uint32 *ptr;
	uint32 value;
	NODE_BUF_T *node_buf_ptr;
    #ifdef C_REVERSE_BYTE_ORDER
	uint8 byte0, byte1, byte2, byte3;
    #endif
	
	node_buf_ptr = wr_req_info.consume_node_ptr;
	ptr = (uint32 *)node_buf_ptr->buf_ptr;
	for(i = 0; i < (count >> 2); i ++)
	{
	    /*b.swab rD,rA:
	        Bytes in general-purpose register rA are placed into 
	        general-purpose register rD in a reverse order       */
    #ifndef C_REVERSE_BYTE_ORDER   
    	 __asm volatile( "b.swab %0, %1;"
				 :"=r"(value)
				 :"r"(ptr[i])
				 :"r26"
				 );
        *buf = value;
    #else
        value = ptr[i];
        byte0 = (value >> 0) & 0xff;
        byte1 = (value >> 8) & 0xff;
        byte2 = (value >> 16) & 0xff;
        byte3 = (value >> 24) & 0xff;

        value = (byte0 << 24) 
                + (byte1 << 16) 
                + (byte2 << 8) 
                + (byte3 << 0);
        *buf = value;
    #endif
	}
}

void write_media_over_cb(void)
{
    REQ_INFO_T *req_ptr;

    req_ptr = &wr_req_info;

    UDISK_PRT("sd_wr:%d\r\n", req_ptr->current_blk_id);

    BK3000_SDIO_FIFO_THRESHOLD |= SDIO_TX_FIFO_RST_BIT;

    /*step2: update config after sending successfully*/
    udisk_put_node_on_free_list(req_ptr->consume_node_ptr);

    req_ptr->current_blk_id  += 1;    
    req_ptr->consume_blk_num ++;
    req_ptr->consume_node_ptr = 0;

    /*handle over, and roll the udisk state machine*/
    if(req_ptr->blk_num == req_ptr->consume_blk_num)
    {
        req_ptr->access_cmd_type   = ACCESS_CMD_NULL;
        req_ptr->blk_num           = 0;
        udisk_pseudo_swi           = 1; 
    }
}

void read_media(void)
{
	uint32 blk_id;
	NODE_BUF_T *node_buf_ptr;

	if((sd_reading_flag) 
		|| (0 == udisk_produce_or_not()))
	{
		return;
	}
	
	/*read sd card, first step is getting buffer*/
	sd_reading_flag += 1;
	node_buf_ptr = udisk_get_node_from_free_list();
	
	if(0 == node_buf_ptr)
	{
		UDISK_PRT("rd_no_node\r\n");
		sd_reading_flag -= 1;
		return;
	}	

	/*config reading block*/
	blk_id = rd_req_info.current_blk_id;
	rd_req_info.produce_node_ptr = node_buf_ptr;
	
	sd_rd_blk_sync(blk_id, 1, node_buf_ptr->buf_ptr);
	
	rd_req_info.current_blk_id   += 1;
	rd_req_info.produce_blk_num  += 1;
	rd_req_info.produce_node_ptr  = 0;	
	sd_reading_flag               = 0;
	
	/*update the sd controller*/
	BK3000_SDIO_FIFO_THRESHOLD |= SDIO_RX_FIFO_RST_BIT;
	
	udisk_put_node_on_use_list(node_buf_ptr);
}

uint32 write_media(void)
{
	REQ_INFO_T *req_ptr;
	NODE_BUF_T *node_buf_ptr;

	req_ptr = &wr_req_info;	
	
	if(sd_writing_flag)
	{
	    return;
	}
	
	/*step0: get node buf*/
	sd_writing_flag = 1;
	node_buf_ptr = udisk_get_node_from_use_list();
	if(node_buf_ptr)
	{		
		req_ptr->consume_node_ptr = node_buf_ptr;
		
		/*step1: SD wr*/
		sd_wr_blk_sync(req_ptr->current_blk_id, 1, node_buf_ptr->buf_ptr);
		
		write_media_over_cb();

		sd_writing_flag = 0;

		
		return RET_USB_SUCCESS;
	}

    sd_writing_flag = 0;
    
	return RET_USB_FAILURE;
}

void _read_media_put_data(
	UDISK_PTR ptr_udisk,
	uint32    start_blk,
	uint32    blk_num)
{
    REQ_INFO_T *req_ptr;

    req_ptr = &rd_req_info;	
    req_ptr->consume_blk_num   = 0;
    req_ptr->produce_blk_num   = 0;
    req_ptr->produce_node_ptr  = 0;
    req_ptr->consume_node_ptr  = 0;
    req_ptr->access_cmd_type   = ACCESS_CMD_READ;
    req_ptr->start_blk         = start_blk;
    req_ptr->current_blk_id    = start_blk;
    req_ptr->blk_num           = blk_num;
    
    os_printf("r:%d:%d\r\n", req_ptr->start_blk, req_ptr->blk_num);
}

void _get_data_write_media(
	UDISK_PTR ptr_udisk,
	uint32    start_blk,
	uint32    blk_num)
{
	REQ_INFO_T *req_ptr;

	req_ptr = &wr_req_info;	
	req_ptr->consume_blk_num   = 0;
	req_ptr->produce_blk_num   = 0;
	req_ptr->produce_node_ptr  = 0;
	req_ptr->consume_node_ptr  = 0;
	req_ptr->access_cmd_type   = ACCESS_CMD_WRITE;
	req_ptr->start_blk         = start_blk;
	req_ptr->current_blk_id    = start_blk;
	req_ptr->blk_num           = blk_num;
    UDISK_PRT("w:%d:%d\r\n", req_ptr->start_blk, req_ptr->blk_num);
}

void udisk_produce_nodes(void)
{
	PRODUCE_FUNC func = 0;
    uint32 interrupts_info, mask;

    VICMR_disable_interrupts(&interrupts_info, &mask);
    if((ACCESS_CMD_READ == rd_req_info.access_cmd_type)
        && (rd_req_info.blk_num != rd_req_info.produce_blk_num))
    {
		func = read_media;
    }
    else if((ACCESS_CMD_WRITE == wr_req_info.access_cmd_type)
        && (wr_req_info.blk_num != wr_req_info.produce_blk_num))
    {
		func = udisk_rx_media_data;
    }
    
    if(func)
    {
	    (*func)();
    }
    VICMR_restore_interrupts(interrupts_info, mask);
    
    return;
}

void udisk_consume_nodes(void)
{
	CONSUME_FUNC func = 0;
    uint32 interrupts_info, mask;

    VICMR_disable_interrupts(&interrupts_info, &mask);
    if((ACCESS_CMD_READ == rd_req_info.access_cmd_type)
        && (rd_req_info.blk_num != rd_req_info.consume_blk_num))
    {
		func = udisk_tx_media_data;
    }
    else if((ACCESS_CMD_WRITE == wr_req_info.access_cmd_type)
        && (wr_req_info.blk_num != wr_req_info.consume_blk_num))
    {
		func = write_media;
    }    
    
    if(func)
    {
	    (*func)();
    }
    VICMR_restore_interrupts(interrupts_info, mask);
    
	return;
}
#endif

void msc_ep0_mass_storage_reset(EP_ENTITY_PTR ctrl_ep_ptr, setup_buffer *setup_ptr)
{
	UDISK_PTR udisk_ptr;
	UDEV_PTR udev_ptr;
	EP_ENTITY_PTR bulk_in_ep_ptr;
	EP_ENTITY_PTR bulk_out_ep_ptr;

	udisk_ptr = &udisk;
	udev_ptr = udisk_ptr->udev_pointer;
	
	UDISK_PRT("msc_ep0_mass_storage_reset\r\n");
	/* Bulk-Only Mass Storage Reset: Ready the device for the next 
	** CBW from the host*/
	if ((setup_ptr->wValue.i != 0)
		|| (setup_ptr->wIndex.i != MASS_STORAGE_INTERFACE)
		||(setup_ptr->wLength.i != 0))
	{
		udev_ep_put_on_halt(ctrl_ep_ptr);
	} 
	else 
	{ /* Body */
		udisk_ptr->cbw_processed = FALSE;
		udisk_ptr->zero_terminate = FALSE;
			
		/* unstall bulk endpoint */
		bulk_in_ep_ptr = udev_ptr->ep_esse[EP_BULK_IN];
		bulk_out_ep_ptr = udev_ptr->ep_esse[EP_BULK_OUT];

		if((0 == bulk_in_ep_ptr) || (0 == bulk_out_ep_ptr))
		{
			USB_FATAL();
		}
		
		udev_ep_put_off_halt(bulk_in_ep_ptr);
		udev_ep_put_off_halt(bulk_out_ep_ptr);
		
		_ctrl_ep_tx_zero_packet(&usb_dev);
	} /* Endbody */
}

void msc_ep0_get_max_lun (EP_ENTITY_PTR ctrl_ep_ptr, setup_buffer *setup_ptr)
{
	UDEV_PTR udev_ptr = &usb_dev;
	
	/* For Get Max LUN use any of these responses*/
	if (setup_ptr->wLength.i == 0)
	{ 
		UDISK_PRT("get_max_lun0\r\n");
		udev_ep_put_on_halt(ctrl_ep_ptr);
	} 
	else if ((setup_ptr->wValue.i != 0) 
		|| (setup_ptr->wIndex.i != MASS_STORAGE_INTERFACE) 
		|| (setup_ptr->wLength.i != 1)) 
	{
		UDISK_PRT("get_max_lun1\r\n");
		udev_ep_put_on_halt(ctrl_ep_ptr);
	} 
	else 
	{ /* Body */
		udev_ptr = &usb_dev;
		UDISK_PRT("get_max_lun2\r\n");
		
		/* Send Max LUN = 0 to the the control pipe */
		udisk_data_buf[0] = 0;

    	udev_async_send(udev_ptr, EP_0, udisk_data_buf, 1, _ctrl_ep_tx_cb);
	
		udev_async_receive(udev_ptr, 
			EP_BULK_OUT, 
			udisk_data_buf, 
			UDISK_CBW_LEN,
			udisk_cbw_swi
			);
	} /* Endbody */  
}

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : cbw_inquiry_command
* Returned Value : None
* Comments       :
*     Process a Mass storage class Inquiry command
* 
*END*--------------------------------------------------------------------*/
void cbw_inquiry_command
   (
      /* [IN] Handle of the USB device */
      UDISK_PTR            ptr_udisk,
            
      /* [IN] Endpoint number */
      uint8               ep_num,
      
      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{ /* Body */
	CSW_STRUCT_PTR csw_ptr;
	UDEV_PTR handle = ptr_udisk->udev_pointer;

	csw_ptr = &(ptr_udisk->csw);
	UDISK_PRT("cbw_inquiry_command\r\n");
	if (cbw_ptr->DCBWDATALENGTH) 
	{
		if (cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT) 
		{      
			/* Send the device information */
			udev_async_send(handle, 
							EP_BULK_IN, 
							(uchar_ptr)&(ptr_udisk->device_info_data), 
							36,
							udisk_tx_callback);
		} /* Endif */
	} /* Endif */

	/* The actual length will never exceed the DCBWDATALENGTH */            
	csw_ptr->DCSWDATARESIDUE = (cbw_ptr->DCBWDATALENGTH - 36);
	csw_ptr->BCSWSTATUS = 0;   
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : cbw_unsupported_command
* Returned Value : None
* Comments       :
*     Responds appropriately to unsupported commands
* 
*END*--------------------------------------------------------------------*/
void cbw_unsupported_command
   (
      /* [IN] Handle of the USB device */
      UDISK_PTR           ptr_udisk,
            
      /* [IN] Endpoint number */
      uint8               ep_num,
      
      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{ /* Body */
	CSW_STRUCT_PTR csw_ptr;
	UDEV_PTR handle = ptr_udisk->udev_pointer;

	csw_ptr = &(ptr_udisk->csw);
	
	/* The actual length will never exceed the DCBWDATALENGTH */
	csw_ptr->DCSWDATARESIDUE = 0;
	csw_ptr->BCSWSTATUS = 0;	
	
	if (cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT) 
	{      
		/* Send a zero-length packet */		
		udev_async_send(handle, 
						EP_BULK_IN, 
						(uchar_ptr)&(ptr_udisk->device_info_data), 
						0,
						udisk_tx_callback);
	} 
	else 
	{
		ptr_udisk->cbw_processed = FALSE;

		/* Send the command status information */
		udev_sync_send(handle, 
						EP_BULK_IN, 
						(uchar_ptr)csw_ptr, 
						UDISK_CSW_LEN,
						udev_tx_callback);

		udev_async_receive(handle, 
						EP_BULK_OUT, 
						udisk_data_buf, 
						UDISK_CBW_LEN,
						udisk_cbw_swi
						);
	} /* Endif */

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : cbw_report_capacity
* Returned Value : None
* Comments       :
*     Reports the media capacity as a response to READ CAPACITY Command.
* 
*END*--------------------------------------------------------------------*/
void cbw_report_capacity
   (
      /* [IN] Handle of the USB device */
      UDISK_PTR            ptr_udisk,
            
      /* [IN] Endpoint number */
      uint8               ep_num,
      
      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{ 
	UDEV_PTR handle = ptr_udisk->udev_pointer;
	CSW_STRUCT_PTR csw_ptr;
	
	csw_ptr = &(ptr_udisk->csw);
	
	UDISK_PRT("_capacity\r\n");
	if (cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT) 
	{            
		udev_async_send(handle, 
						EP_BULK_IN, 
						(uchar_ptr)&(ptr_udisk->read_capacity), 
						8,
						udisk_tx_callback);      
	} /* Endif */

	/* The actual length will never exceed the DCBWDATALENGTH */            
	csw_ptr->DCSWDATARESIDUE = 0;
	csw_ptr->BCSWSTATUS = 0;   
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* Function Name  : cbw_read_command
* Comments       :
*     Sends data as a response to READ Command.
*END*--------------------------------------------------------------------*/
void cbw_read_command
   (
      /* [IN] Handle of the USB device */
      UDISK_PTR           ptr_udisk,
            
      /* [IN] Endpoint number */
      uint8               ep_num,
      
      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{ /* Body */
	uint32 index1 = 0;
	uint32 index2 = 0;
	uint32 max_pkt_size;
	UDEV_PTR handle = ptr_udisk->udev_pointer;
	CSW_STRUCT_PTR csw_ptr= &(ptr_udisk->csw);

	if (cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT) 
	{      
		/* Send a zero-length packet */
		index1  = ((uint32)cbw_ptr->CBWCB[2] << 24);
		index1  |= ((uint32)cbw_ptr->CBWCB[3] << 16);
		index1  |= ((uint32)cbw_ptr->CBWCB[4] << 8);
		index1  |= cbw_ptr->CBWCB[5];
        
		index2 = ((uint32)cbw_ptr->CBWCB[7] << 8);
		index2 |= (uint32)cbw_ptr->CBWCB[8];

		index2 *= LENGTH_OF_EACH_LAB;
      
      if (cbw_ptr->DCBWDATALENGTH == 0) 
	  { /* Body */
         csw_ptr->DCSWDATARESIDUE = 0;
         csw_ptr->BCSWSTATUS = 2;
         ptr_udisk->cbw_processed = FALSE;
		 
         /* Send the command status information */
		udev_sync_send(handle, 
						EP_BULK_IN, 
						(uchar_ptr)csw_ptr, 
						UDISK_CSW_LEN,
						udev_tx_callback);
		
		udev_async_receive(handle, 
						EP_BULK_OUT, 
						udisk_data_buf, 
						UDISK_CBW_LEN,
						udisk_cbw_swi
						);
         return;
      } 
	  else 
	  { /* Body */
         csw_ptr->DCSWDATARESIDUE = 0;
         csw_ptr->BCSWSTATUS = 0;         
         if (index2 > cbw_ptr->DCBWDATALENGTH) 
		 { /* Body */
            index2 = cbw_ptr->DCBWDATALENGTH;
            csw_ptr->DCSWDATARESIDUE = cbw_ptr->DCBWDATALENGTH;
            csw_ptr->BCSWSTATUS = 2;
         } 
		 else if (index2 < cbw_ptr->DCBWDATALENGTH) 
		 { /* Body */
            csw_ptr->DCSWDATARESIDUE = cbw_ptr->DCBWDATALENGTH - index2;
            if (index2 > 0) 
			{ /* Body */
                  max_pkt_size = EP_FS_MAX_PACKET_SIZE;
				  
               if (index2 % max_pkt_size == 0) 
			   { /* Body */
                  /* Need send a zero terminate packet to host */
                  ptr_udisk->zero_terminate = TRUE;
               } /* Endbody */
            } /* Endbody */  
         } /* Endbody */
		 
		UDISK_PRT("cbw_read_command:%d:%d\r\n", index1, index2);
		_read_media_put_data(ptr_udisk, index1, (index2 >> 9));
      } /* Endbody */
   } 
   else 
	{ /* Body */
		/* Incorrect but valid CBW */
		if (cbw_ptr->DCBWDATALENGTH > UDISK_BUFFER_SIZE)
		{
			index2 = UDISK_BUFFER_SIZE;
		}
		else
		{
			index2 = cbw_ptr->DCBWDATALENGTH;
		}

		csw_ptr->DCSWDATARESIDUE = cbw_ptr->DCBWDATALENGTH;
		csw_ptr->BCSWSTATUS = 2;

		udev_sync_receive(handle, 
							EP_BULK_OUT, 
							udisk_data_buf, 
							index2,
							udisk_rx_callback); 
	} /* Endbody */  
   
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : cbw_write_command
* Returned Value : None
* Comments       :
*     Sends data as a response to WRITE Command.
* 
*END*--------------------------------------------------------------------*/
void cbw_write_command
   (
      /* [IN] Handle of the USB device */
      UDISK_PTR           ptr_udisk,
            
      /* [IN] Endpoint number */
      uint8               ep_num,
      
      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{ /* Body */
    uint32 index1 = 0;
    uint32 index2 = 0;
    UDEV_PTR handle = ptr_udisk->udev_pointer;
    CSW_STRUCT_PTR csw_ptr= &(ptr_udisk->csw);
    
    UDISK_PRT("cbw_write\r\n");	

   if (!(cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT)) 
   {
        index1  = ((uint32)cbw_ptr->CBWCB[2] << 24);
        index1  |= ((uint32)cbw_ptr->CBWCB[3] << 16);
        index1  |= ((uint32)cbw_ptr->CBWCB[4] << 8);
        index1  |= cbw_ptr->CBWCB[5];

        index2 = ((uint32)cbw_ptr->CBWCB[7] << 8);
        index2 |= (uint32)cbw_ptr->CBWCB[8];
      
      if (cbw_ptr->DCBWDATALENGTH == 0) 
	  { /* Body */
         /* Zero transfer length */
         csw_ptr->DCSWDATARESIDUE = 0;
         csw_ptr->BCSWSTATUS = 2;
         ptr_udisk->cbw_processed = FALSE;
		 
         /* Send the command status information */
		udev_sync_send(handle, 
						EP_BULK_IN, 
						(uchar_ptr)csw_ptr, 
						UDISK_CSW_LEN,
						udev_tx_callback);
		
		udev_async_receive(handle, 
						EP_BULK_OUT, 
						udisk_data_buf, 
						UDISK_CBW_LEN,
						udisk_cbw_swi
						);
         return;
      } 
	  else 
	  { /* Body */
         csw_ptr->DCSWDATARESIDUE = 0;
         csw_ptr->BCSWSTATUS = 0;
         index2 *= LENGTH_OF_EACH_LAB;
         
         if (index2 < cbw_ptr->DCBWDATALENGTH) 
		 { /* Body */
            /* The actual length will never exceed the DCBWDATALENGTH */
            csw_ptr->DCSWDATARESIDUE = cbw_ptr->DCBWDATALENGTH - index2;
            index2 = cbw_ptr->DCBWDATALENGTH;
         } 
		 else if (index2 > cbw_ptr->DCBWDATALENGTH) 
		 { /* Body */
            csw_ptr->DCSWDATARESIDUE = cbw_ptr->DCBWDATALENGTH;
            csw_ptr->BCSWSTATUS = 2;
            index2 = cbw_ptr->DCBWDATALENGTH;
         } /* Endbody */

		UDISK_PRT("cbw_write_command:%d:%d\r\n", index1, index2);	
		_get_data_write_media(ptr_udisk, index1, (index2 >> 9));
      }
   }
   else 
   { /* Body */
      /* Incorrect but valid CBW */
      csw_ptr->DCSWDATARESIDUE = cbw_ptr->DCBWDATALENGTH;
      csw_ptr->BCSWSTATUS = 2;
	  
	  udev_sync_send(handle, 
					EP_BULK_IN, 
					udisk_data_buf, 
					0,
					udisk_tx_callback);  
      return;
   } /* Endbody */
   
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : cbw_test_unit_ready
* Returned Value : None
* Comments       :
*     Responds appropriately to unit ready query
* 
*END*--------------------------------------------------------------------*/
void cbw_test_unit_ready
   (
      /* [IN] Handle of the USB device */
      UDISK_PTR            ptr_udisk,
            
      /* [IN] Endpoint number */
      uint8               ep_num,
      
      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{ /* Body */
	uint32 BufSize;
	UDEV_PTR handle = ptr_udisk->udev_pointer;
	CSW_STRUCT_PTR csw_ptr= &(ptr_udisk->csw);
	
	UDISK_PRT("cbw_test_unit_ready\r\n");
	
	if ((cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT) 
		|| (cbw_ptr->DCBWDATALENGTH == 0)) 
	{
		/* The actual length will never exceed the DCBWDATALENGTH */
		csw_ptr->DCSWDATARESIDUE = 0;
		csw_ptr->BCSWSTATUS = 0;

		ptr_udisk->cbw_processed = FALSE;
		/* Send the command status information */
		udev_sync_send(handle, 
						EP_BULK_IN, 
						(uchar_ptr)csw_ptr, 
						UDISK_CSW_LEN,
						udev_tx_callback);

		udev_async_receive(handle, 
						EP_BULK_OUT, 
						udisk_data_buf, 
						UDISK_CBW_LEN,
						udisk_cbw_swi
						);
	}
	else 
	{ /* Body */	
		/* Incorrect but valid CBW */
		if (cbw_ptr->DCBWDATALENGTH > UDISK_BUFFER_SIZE)
			BufSize = UDISK_BUFFER_SIZE;
		else
			BufSize = cbw_ptr->DCBWDATALENGTH;

		csw_ptr->DCSWDATARESIDUE = cbw_ptr->DCBWDATALENGTH;
		csw_ptr->BCSWSTATUS = 1;

		udev_sync_receive(handle, 
							EP_BULK_OUT, 
							udisk_data_buf, 
							BufSize,
							udev_rx_callback
							);
	} /* Endbody */

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : cbw_prevent_allow_medium_removal
* Returned Value : None
* Comments       :
*     Responds appropriately to unit ready query
* 
*END*--------------------------------------------------------------------*/
void cbw_prevent_allow_medium_removal
   (
      /* [IN] Handle of the USB device */
      UDISK_PTR           ptr_udisk,
            
      /* [IN] Endpoint number */
      uint8               ep_num,
      
      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
	UDEV_PTR handle = ptr_udisk->udev_pointer;
	CSW_STRUCT_PTR csw_ptr= &(ptr_udisk->csw);
	
	/* The actual length will never exceed the DCBWDATALENGTH */
	csw_ptr->DCSWDATARESIDUE = 0;
	csw_ptr->BCSWSTATUS = 0;

	UDISK_PRT("cbw_prevent_allow_medium_removal\r\n");
	ptr_udisk->cbw_processed = FALSE;
	
	/* Send the command status information */
	udev_sync_send(handle, 
					EP_BULK_IN, 
					(uchar_ptr)csw_ptr, 
					UDISK_CSW_LEN,
					udev_tx_callback);

	udev_async_receive(handle, 
					EP_BULK_OUT, 
					udisk_data_buf, 
					UDISK_CBW_LEN,
					udisk_cbw_swi
					);
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : cbw_process_command
* Returned Value : None
* Comments       :
*     Process a Mass storage class command
* 
*END*--------------------------------------------------------------------*/
void cbw_process_command
   (
      /* [IN] Handle of the UDISK device */
      UDISK_PTR           ptr_udisk,
            
      /* [IN] Endpoint number */
      uint8               ep_num,
      
      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{ /* Body */
   switch (cbw_ptr->CBWCB[0]) {
      case 0x00: /* Request the device to report if it is ready */
         cbw_test_unit_ready(ptr_udisk, ep_num, cbw_ptr);
         break;
		 
      case 0x12: /* Inquity command. Get device information */
         cbw_inquiry_command(ptr_udisk, ep_num, cbw_ptr);
         break;
		 
      case 0x1A:
         cbw_unsupported_command(ptr_udisk, ep_num, cbw_ptr);
         break;
		 
      case 0x1E: /* Prevent or allow the removal of media from a removable 
                 ** media device */
         cbw_prevent_allow_medium_removal(ptr_udisk, ep_num, cbw_ptr);
         break;
		 
      case 0x23: /* Read Format Capacities. Report current media capacity and 
                 ** formattable capacities supported by media */
         /* We bahave like already installed medium. No need to send any data */
         cbw_unsupported_command(ptr_udisk, ep_num, cbw_ptr);
         break;
		 
      case 0x25: /* Report current media capacity */
         cbw_report_capacity(ptr_udisk, ep_num, cbw_ptr);
         break;
		 
      case 0x28: /* Read (10) Transfer binary data from media to the host */
         cbw_read_command(ptr_udisk, ep_num, cbw_ptr);
         break;
		 
      case 0x2A: /* Write (10) Transfer binary data from the host to the 
                 ** media */
         cbw_write_command(ptr_udisk, ep_num, cbw_ptr);
         break;
		 
      case 0x01: /* Position a head of the drive to zero track */
      case 0x03: /* Transfer status sense data to the host */
      case 0x04: /* Format unformatted media */
      case 0x1B: /* Request a request a removable-media device to load or 
                 ** unload its media */
      case 0x1D: /* Perform a hard reset and execute diagnostics */
      case 0x2B: /* Seek the device to a specified address */
      case 0x2E: /* Transfer binary data from the host to the media and 
                 ** verify data */
      case 0x2F: /* Verify data on the media */
      case 0x55: /* Allow the host to set parameters in a peripheral */
      case 0x5A: /* Report parameters to the host */
      case 0xA8: /* Read (12) Transfer binary data from the media to the host */
      case 0xAA: /* Write (12) Transfer binary data from the host to the 
                 ** media */
      default:
         cbw_unsupported_command(ptr_udisk, ep_num, cbw_ptr);
         break;
   } /* Endswitch */

} /* EndBody */

void udisk_rx_callback(void *param, uint32 count)
{
	EP_ENTITY_PTR ep_ptr = (EP_ENTITY_PTR)param;
	
	if(ep_ptr->data_req_len - ep_ptr->data_actual_len > ep_ptr->mps)
	{
		/* check if dmem of ep has space*/
		if(ep_ptr->cell_use_id - ep_ptr->cell_free_id < (ep_ptr->dmem_cell_count - 1))
		{
			/* mark rxed data*/
			ep_ptr->cell_use_id += 1;
			
			/* update rx dmem*/
			udev_update_ep_fifo_config(ep_ptr, (ep_ptr->cell_use_id & (ep_ptr->dmem_cell_count - 1)));
			
			/* re-enable rx*/
			udev_enable_receive(ep_ptr);
		}
		else
		{
			UDISK_PRT("usb_rd_no_cell\r\n");
			ep_ptr->urb_status = URB_RECEIVE_SUSPEND;
		}
	}
	
	ep_ptr->data_actual_len += count;
	if(ep_ptr->data_actual_len == ep_ptr->data_req_len)
	{
		ep_ptr->urb_status = URB_RECEIVE_OVER;	
		
		udev_update_ep_fifo_config(ep_ptr, 0);
		ep_ptr->cell_free_id = 0;
		ep_ptr->cell_use_id  = 0;
		
		if(URB_ASYNC == ep_ptr->sync_flag)
		{
			udisk_pseudo_swi = 1;
		}	
	}
}

void udisk_tx_callback(void *param, uint32 count)
{
	uint32 deta_count;
	EP_ENTITY_PTR ep_ptr = (EP_ENTITY_PTR)param;
	
	/* check this successful sending, and update some variables*/
	deta_count = ep_ptr->data_req_len - ep_ptr->data_actual_len;
	if(deta_count <= ep_ptr->mps)
	{
		ep_ptr->data_actual_len = ep_ptr->data_req_len;
		ep_ptr->urb_status = URB_SEND_OVER;
		
		ep_ptr->cell_free_id = 0;
		ep_ptr->cell_use_id  = 0;
		
		udev_update_ep_fifo_config(ep_ptr, 0);

		if(URB_ASYNC == ep_ptr->sync_flag)
		{
			udisk_pseudo_swi = 1; 
		}		
	}
	else
	{
		ep_ptr->data_actual_len += ep_ptr->mps;
		
		ep_ptr->cell_free_id += 1;		
		udev_update_ep_fifo_config(ep_ptr, ep_ptr->cell_free_id & (ep_ptr->dmem_cell_count - 1));

		udev_enable_send(ep_ptr);		
	}			
}

void udisk_cbw_swi(void *param, uint32 count)
{
	EP_ENTITY_PTR ep_ptr = (EP_ENTITY_PTR)param;
	
	udev_read_fifo(ep_ptr, 
				count, 
				(ep_ptr->data_ptr + ep_ptr->data_actual_len),
				ep_ptr->dmem_offset);
	ep_ptr->data_actual_len += count;
	ep_ptr->urb_status = URB_RECEIVE_OVER;
	
	udisk_pseudo_swi = 1;
}

uint32 udisk_get_pseudo_swi(void)
{
	return udisk_pseudo_swi;
}

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : service_ep1
* Returned Value : None
* Comments       :
*     Called upon a completed endpoint transfer
* 
*END*--------------------------------------------------------------------*/
void udisk_service(void)
{ 
	UDISK_PTR udisk_ptr= &udisk;
	CBW_STRUCT_PTR cbw_ptr;     
	CSW_STRUCT_PTR csw_ptr= &(udisk_ptr->csw);
	UDEV_PTR udev_ptr = udisk_ptr->udev_pointer;
	EP_ENTITY_PTR bulk_out_ptr = udev_ptr->ep_esse[EP_BULK_OUT]; 

	udisk_pseudo_swi = 0;
	
	if (!udisk_ptr->cbw_processed) 
	{
		cbw_ptr = (CBW_STRUCT_PTR)(udisk_data_buf);
	} 

	if ((!udisk_ptr->cbw_processed) 
		&& (31 == bulk_out_ptr->data_actual_len) 
		&& (USB_DCBWSIGNATURE == cbw_ptr->DCBWSIGNATURE)) 
	{   /* A valid CBW was received */
		csw_ptr->DCSWSIGNATURE       = USB_DCSWSIGNATURE;
		csw_ptr->DCSWTAG             = cbw_ptr->DCBWTAG;
		udisk_ptr->cbw_processed     = TRUE;

		cbw_process_command(udisk_ptr, 1, cbw_ptr);
	} 
	else 
	{
		/* If a CBW was processed then send the status information and 
		** queue another cbw receive request, else just queue another CBW receive
		** request if we received an invalid CBW 
		*/
		if (udisk_ptr->cbw_processed) 
		{
			if (udisk_ptr->zero_terminate) 
			{ 
				udisk_ptr->zero_terminate = FALSE;
				
				UDISK_PRT("zero_terminate\r\n");
				udev_sync_send(udev_ptr, 
								EP_BULK_IN, 
								udisk_data_buf, 
								0,
								udev_tx_callback);
			} 
			else 
			{ 
				udisk_ptr->cbw_processed = FALSE;
				
				UDISK_PRT("csw_cbw\r\n");
				/* Send the command status information */
				udev_sync_send(udev_ptr, 
								EP_BULK_IN, 
								(uchar_ptr)csw_ptr, 
								UDISK_CSW_LEN,
								udev_tx_callback);
				
				udev_async_receive(udev_ptr, 
					EP_BULK_OUT, 
					udisk_data_buf, 
					UDISK_CBW_LEN,
					udisk_cbw_swi
					);
			}
		} 
	} /* Endif */

	return;
} /* Endbody */

void udisk_update_capacity(UDISK_PTR udisk_ptr)
{
	MSC_READ_CAPACITY_PTR capacity_ptr = &(udisk_ptr->read_capacity);
	
	UDISK_PRT("block info:0x%x,0x%x\r\n", udisk_ptr->block_count, udisk_ptr->block_size);
	
	capacity_ptr->LAST_LOGICAL_BLOCK_ADDRESS[0] = USB_UINT_16_HIGH(((udisk_ptr->block_count - 1) >> 16) & 0xffff);
	capacity_ptr->LAST_LOGICAL_BLOCK_ADDRESS[1] = USB_UINT_16_LOW(((udisk_ptr->block_count - 1) >> 16) & 0xffff);
	capacity_ptr->LAST_LOGICAL_BLOCK_ADDRESS[2] = USB_UINT_16_HIGH((udisk_ptr->block_count - 1) & 0xffff);
	capacity_ptr->LAST_LOGICAL_BLOCK_ADDRESS[3] = USB_UINT_16_LOW((udisk_ptr->block_count - 1) & 0xffff);
	
	capacity_ptr->BLOCK_LENGTH_IN_BYTES[2] = USB_UINT_16_HIGH(udisk_ptr->block_size);
	capacity_ptr->BLOCK_LENGTH_IN_BYTES[3] = USB_UINT_16_LOW(udisk_ptr->block_size);
}

void udisk_uninitialization(void)
{
    if(udisk_data_buf)
    {
        jfree(data_buf);
        data_buf = 0;
        udisk_data_buf = 0;
    }
    
#ifndef DIRECT_COPY_BTWN_SD_EP_FIFO    
    udisk_uninit_node_lists();
#endif

    udev_uninit();
}

void udisk_initialization(void)
{
	UDISK_PTR udisk_ptr;

	udisk_ptr = &udisk;
    
	#ifndef DIRECT_COPY_BTWN_SD_EP_FIFO
    #ifdef USE_DMA_
	dma_init();
    #endif
    
	sd_reading_flag = 0;
    sd_writing_flag = 0;
	udisk_init_list_head();
	udisk_init_access_nodes();
	#endif
	
    udisk_ptr->udev_pointer     = udev_init();	
    
	udisk_ptr->cbw_processed    = FALSE;
	udisk_ptr->zero_terminate   = FALSE;
	udisk_ptr->media_exist_flag = 1;
	udisk_ptr->block_size  = driver_sdcard_get_block_size();
	udisk_ptr->block_count = driver_sdcard_get_block_num();

    data_buf = (uint8*)jmalloc(UDISK_BUFFER_SIZE, M_ZERO);
    udisk_data_buf = (uint8 *)USB_MEM32_ALIGN((uint32)data_buf);
    
	udisk_update_capacity(udisk_ptr);
	
	udev_init_endpoint(EP_BULK_OUT, 
						EP_DIR_BIT_OUT,      // dir, 
						EP_MODE_BULK,        // mode, 
						EP_SIZE_64,          //  mps, 
						EP_ENABLE_FLAG,      // enable_flag, 
						BULK_OUT_FIFO_OFFSET,// fifo_addr_offset
						BULK_OUT_CELL_COUNT
					);
	
	udev_init_endpoint(EP_BULK_IN,              
						EP_DIR_BIT_IN,      // dir, 
						EP_MODE_BULK,       // mode, 
						EP_SIZE_64,         //  mps, 
						EP_ENABLE_FLAG,     // enable_flag, 
						BULK_IN_FIFO_OFFSET,// fifo_addr_offset
						BULK_IN_CELL_COUNT
					);
	
	VICMR_usb_chief_intr_enable();
	UDISK_PRT("VICMR_usb_chief_intr_enable1:%x,%x,%x\r\n"
				, get_spr(SPR_VICMR(0))
				, get_spr(SPR_VICPR(VIC_USB_ISR_INDEX))
				, get_spr(SPR_VICVA(VIC_USB_ISR_INDEX)));
}

void app_udisk_install(void *p)
{	    
	UDISK_PRT("sdio:%x,%x,%x\r\n"
				, get_spr(SPR_VICMR(0))
				, get_spr(SPR_VICPR(VIC_SDIO_ISR_INDEX))
				, get_spr(SPR_VICVA(VIC_SDIO_ISR_INDEX)));
	
#ifdef CONFIG_APP_SDCARD
    driver_sdcard_init(SD_WORK_MODE_USB);

    if(disk_initialize(0))
    {
        os_printf("disk_initialize_exceptional\r\n");
    }
    else
    {
        udisk_initialization();
    }
#endif		

	return;
}

void app_udisk_uninstall(void)
{
    UDISK_PRT("VICMR_usb_chief_intr_disable\r\n");
    VICMR_usb_chief_intr_disable();
    
    udisk_pseudo_swi = 0;
    
    #ifndef DIRECT_COPY_BTWN_SD_EP_FIFO
    rd_req_info.access_cmd_type = ACCESS_CMD_NULL;
    rd_req_info.blk_num = 0;
    udisk_free_node(&(rd_req_info.consume_node_ptr));
    udisk_free_node(&(rd_req_info.produce_node_ptr));
    
    wr_req_info.access_cmd_type = ACCESS_CMD_NULL;
    wr_req_info.blk_num = 0;
    udisk_free_node(&(wr_req_info.consume_node_ptr));
    udisk_free_node(&(wr_req_info.produce_node_ptr));
    #endif
    
    udisk_uninitialization();
    
    UDISK_PRT("udev_uninit\r\n");
    driver_sdcard_uninstall();
}

uint32 app_is_udisk_mode(void)
{
    app_handle_t app_h = app_get_sys_handler();

    return (SYS_WM_UDISK_MODE == app_h->sys_work_mode);
}

void app_udisk_swi_handler(void)
{
	udisk_service();
}

#endif
/* EOF */
