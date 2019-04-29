#include "includes.h"
#include "usb.h"

#ifdef CONFIG_APP_USB_DISK
UDEV usb_dev = {
	DEV_DEFAULT,
	{
		UDEV_DMEM_SPACE_FOR_HW_LEN,
		UDEV_DMEM_SPACE_FOR_HW_LEN,
	},
};

extern setup_buffer setup; 

void udev_ep_put_on_halt(EP_ENTITY_PTR ep_ptr)
{
	REG_USB_EP_HALT |= (1 << ep_ptr->index);
	
	ep_ptr->status = EP_STALL;
}

void udev_ep_put_off_halt(EP_ENTITY_PTR ep_ptr)
{
	REG_USB_EP_HALT &= ~(1 << ep_ptr->index);
	
	ep_ptr->status = EP_IDLE;
}

void udev_clear_hw_address(void)
{
	udev_update_hw_address(0);
}

void udev_update_hw_address(uint32 addr)
{
	REG_USB_FUNCT_ADR = addr;
}

void udev_put_on_remote_wakeup(void)
{
	REG_USB_PWR_CN |= (USB_POWER_REMOTE_WAKEUP << USB_POWER_REMOTE_WAKEUP_POSITION); 
}

void udev_put_off_remote_wakeup(void)
{
	REG_USB_PWR_CN &= (~(USB_POWER_REMOTE_WAKEUP << USB_POWER_REMOTE_WAKEUP_POSITION)); 
}
void udev_put_on_suspend(void)
{
	REG_USB_PWR_CN |= (USB_POWER_SUSPEND << USB_POWER_SUSPEND_POSITION); 
}

void udev_put_off_suspend(void)
{
	REG_USB_PWR_CN &= (~(USB_POWER_SUSPEND << USB_POWER_SUSPEND_POSITION)); 
}

void udev_put_on_reset(void)
{
	REG_USB_PWR_CN |= (USB_POWER_RESET << USB_POWER_RESET_POSITION); 
}

void udev_put_off_reset(void)
{
	REG_USB_PWR_CN &= (~(USB_POWER_RESET << USB_POWER_RESET_POSITION)); 
}

void udev_pullup_enable(void)
{
	REG_USB_PWR_CN |= (USB_POWER_PULLUP_ENABLE << USB_POWER_PULLUP_ENABLE_POSITION); 
}

void udev_pullup_disable(void)
{
	REG_USB_PWR_CN &= (~(USB_POWER_PULLUP_ENABLE << USB_POWER_PULLUP_ENABLE_POSITION)); 
}

void udev_peripheral_clock_enable(void)
{
	BK3000_PMU_PERI_PWDS &= ~bit_PMU_USB_PWD;
}

void udev_peripheral_clock_disable(void)
{
	BK3000_PMU_PERI_PWDS |= bit_PMU_USB_PWD;
}

void udev_enable_usb_isr_branches(void)
{
	REG_USB_EN0 = (USB_ENABLE_RESUME << USB_ENABLE_RESUME_POSITION)
					/*+ (USB_ENABLE_SOF << USB_ENABLE_SOF_POSITION)*/
					+ (USB_ENABLE_SUSPEND << USB_ENABLE_SUSPEND_POSITION)
					+ (USB_ENABLE_RESET << USB_ENABLE_RESET_POSITION)
					+ (USB_ENABLE_TX_READY << USB_ENABLE_TX_READY_POSITION)
					+ (USB_ENABLE_RX_READY << USB_ENABLE_RX_READY_POSITION)
					+ (USB_ENABLE_CTRL_SEND << USB_ENABLE_CTRL_SEND_POSITION)
					+ (USB_ENABLE_CTRL_RECEIVE << USB_ENABLE_CTRL_RECEIVE_POSITION);
	
#ifdef ENABLE_USB_EXCEPTIONAL_ISR 	
	REG_USB_EN1 = (USB_ENABLE_PID_ERR << USB_ENABLE_PID_ERR_POSITION)
					+ (USB_ENABLE_OVERTIME << USB_ENABLE_OVERTIME_POSITION)
					+ (USB_ENABLE_CRC16_ERR << USB_ENABLE_CRC16_ERR_POSITION)
					+ (USB_ENABLE_BAD_TOKEN << USB_ENABLE_BAD_TOKEN_POSITION);
#else
	REG_USB_EN1 = 0;
#endif
}

#ifdef FLASH_SPEEDUP
void udev_hw_speedup_config(void)
{
}
#endif

UDEV_PTR udev_init(void)
{ 
	int32 reg;
	UDEV_PTR udev_ptr;
    
    udev_ptr = &usb_dev;
    
	/*USB entity initialization*/
	os_memset(udev_ptr, 0, sizeof(usb_dev));
	udev_ptr->udev_dmem.total_size = UDEV_DMEM_SPACE_FOR_HW_LEN;
	udev_ptr->udev_dmem.remanent   = UDEV_DMEM_SPACE_FOR_HW_LEN;

	udev_set_status(udev_ptr, DEV_DEFAULT);

	/* clear the usb hardware platform*/
	udev_pullup_disable();
	udev_peripheral_clock_disable();

	/*config analog clock, open digital usb switch*/
	udev_peripheral_clock_enable();
    
#ifdef FLASH_SPEEDUP
	udev_hw_speedup_config();
#endif
	
	udev_clear_hw_address();
	udev_put_off_suspend(); 

	reg = REG_USB_INT0;
	REG_USB_INT0 = reg;

	reg = REG_USB_INT1;
	REG_USB_INT1 = reg;
	
	while (REG_USB_CFG_EP0_1 != 0xc0)       //wait to no setup token
	    REG_USB_CFG_EP0_1 = 0xc0;

	udev_init_endpoint(EP_0, 
						EP_DIR_BIT_IN, 
						EP_MODE_CONTROL, 
						EP_SIZE_64,
						EP_ENABLE_FLAG,
						CTRL_EP_FIFO_OFFSET_ADDR,
						EP_0_CELL_COUNT);
	udev_enable_usb_isr_branches();

	/* step into normal condition*/
	udev_pullup_enable();

	return udev_ptr;
}

void udev_uninit(void)
{
    uint32 i;
    EP_ENTITY_PTR ep_ptr;
    UDEV_PTR usb_ptr = &usb_dev;

    for(i = 0; i <= EP_MAX_INDEX; i ++)
    {
        ep_ptr = usb_ptr->ep_esse[i];
        if(ep_ptr)
        {
            jfree(ep_ptr);
            ep_ptr = NULL;
            
            usb_ptr->ep_esse[i] = 0;
        }
    }
        
	udev_pullup_disable();
}

uint32 udev_update_dmem_for_hw(UDEV_PTR udev_ptr, EP_ENTITY_PTR ep_ptr)
{
	uint32 ep_dmem;

	ep_dmem = ep_ptr->dmem_cell_count * ep_ptr->dmem_cell_size;
	if((udev_ptr->udev_dmem.remanent - ep_dmem) < 0)
	{
		return RET_USB_FAILURE;
	}

	udev_ptr->udev_dmem.remanent -= ep_dmem;
	
	return RET_USB_SUCCESS;
}

void udev_set_status(UDEV_PTR udev_ptr, int status)
{
	int suspend_flag;

	suspend_flag = udev_ptr->state & DEV_SUSPENDED;
	
	udev_ptr->state = status;
	if(suspend_flag)
	{
		udev_ptr->state |= DEV_SUSPENDED;
	}
}

void udev_ep_hw_config(uint8 ep_index, 
	uint8 dir, 
	uint8 mode, 
	uint8 mps, 
	uint8 enable_flag,
	uint16 fifo_addr_offset)
{
	volatile uint32 *cfg_reg0_addr;
	volatile uint32 *cfg_reg1_addr;
	cfg_reg0_addr = (volatile uint32 *)(BK3000_USB_BASE_ADDR 
										+ ((EP00_REG_INDEX + (2 * ep_index)) * 4));
	cfg_reg1_addr = (volatile uint32 *)(BK3000_USB_BASE_ADDR 
										+ ((EP01_REG_INDEX + (2 * ep_index)) * 4));
	if(ep_index)
	{	// non-zero endpoint
		*(cfg_reg1_addr) = ((dir & EP_CFG_REG_DIR_MASK) << EP_CFG_REG_DIR_POSITION)
							+ ((mode & EP_CFG_REG_MODE_MASK) << EP_CFG_REG_MODE_POSITION)
							+ ((mps & EP_CFG_REG_SIZE_MASK) << EP_CFG_REG_SIZE_POSITION)
							+ (((fifo_addr_offset >> 8) & EP_CFG_REG_ADDR_PART0_MASK) << EP_CFG_REG_ADDR_PART0_POSITION);
	}
	else
	{	// control endpoint, ep 0
		*(cfg_reg1_addr) = ((dir & EP_CFG_REG_DIR_MASK) << EP_CFG_REG_DIR_POSITION)
							+ ((enable_flag & EP_CFG_REG_ENABLE_MASK) << EP_CFG_REG_ENABLE_POSITION)
							+ (((fifo_addr_offset >> 8) & EP_CFG_REG_ADDR_PART0_MASK) << EP_CFG_REG_ADDR_PART0_POSITION);
	}
	
	*(cfg_reg0_addr) = (fifo_addr_offset & EP_CFG_REG_ADDR_PART1_MASK) << EP_CFG_REG_ADDR_PART1_POSITION;

	return;
}

void udev_ep_sw_config(EP_ENTITY ** ep_pptr, 
	uint8 dir, 
	uint8 mps, 
	uint16 fifo_addr_offset,
	uint32 cell_count)
{
	uint32 count;
	EP_ENTITY_PTR ep_ptr;
	
	ep_ptr = (EP_ENTITY_PTR)jmalloc(sizeof(EP_ENTITY), M_ZERO);	
	switch(mps)
	{			
		case EP_SIZE_16:
			count = 16;
			break;
			
		case EP_SIZE_32:
			count = 32;
			break;
			
		case EP_SIZE_64:
			count = 64;
			break;
			
		case EP_SIZE_128:
			count = 128;
			break;
			
		case EP_SIZE_256:
			count = 256;
			break;
			
		case EP_SIZE_512:
			count = 512;
			break;
			
		case EP_SIZE_0:
		default:
			count = 0;
			break;
	}
	
	ep_ptr->status          = EP_IDLE;
	ep_ptr->mps             = count;
	ep_ptr->dir             = dir;
	ep_ptr->dmem_offset    = fifo_addr_offset;
	ep_ptr->cell_free_id    = 0;
	ep_ptr->cell_use_id     = 0;
	ep_ptr->dmem_cell_count = cell_count;
	ep_ptr->dmem_cell_size  = ep_ptr->mps;

	*ep_pptr = ep_ptr;

	return;
	
ep_sw_fatal:
	USB_FATAL();
	
	return;		
}

void udev_init_endpoint(uint8 ep_index, 
	uint8 dir, 
	uint8 mode, 
	uint8 mps, 
	uint8 enable_flag,
	uint16 fifo_addr_offset,
	uint32 cell_count)
{
	#ifdef UDEV_MEM_CHECK
	uint32 ret;
	#endif
	UDEV_PTR udev_ptr = &usb_dev;
	EP_ENTITY_PTR ep_ptr;

#ifdef USB_DEBUG	
	if(ep_index > EP_MAX_INDEX)
	{
		goto ep_init_fatal;
	}
#endif
	UDEV_PRT("udev_init_endpoint:%d\r\n", ep_index);
	
	udev_ep_sw_config(&ep_ptr, 
						dir, 
						mps, 
						fifo_addr_offset,
						cell_count);
	
	ep_ptr->index = ep_index;
	
	if(udev_ptr->ep_esse[ep_index])
	{
		UDEV_PRT("ep_init_existing\r\n");
		goto ep_init_fatal;
	}

	udev_ptr->ep_esse[ep_index] = ep_ptr;
	
	udev_ep_hw_config(ep_index, 
						dir, 
						mode, 
						mps, 
						enable_flag,
						fifo_addr_offset);
	#ifdef UDEV_MEM_CHECK
	ret = udev_update_dmem_for_hw(udev_ptr, ep_ptr);
	if(ret)
	{
		goto ep_init_fatal;
	}
	#endif
	
	return;
	
ep_init_fatal:
	USB_FATAL();
	
	return;	
}

void udev_enter_suspend(UDEV_PTR udev_ptr)
{
	udev_ptr->state |= DEV_SUSPENDED;
}

void udev_exit_suspend(UDEV_PTR udev_ptr)
{
	udev_ptr->state &= ~DEV_SUSPENDED;
}
uint32 udev_read_fifo(EP_ENTITY_PTR ep_ptr, 
	u_int16 len, 
	u_int8 *pointer,
	uint32 dmem_offset)
{
	uint32 i;
	uint8 *fifo_p;

	fifo_p = (u_int8 *)(BK3000_USBMEM_BASE + (dmem_offset << 2));

	for(i = 0; i < len; i ++)
	{
		pointer[i] = fifo_p[i * 4];
	}

	return len;
}

uint32 udev_write_fifo(EP_ENTITY_PTR ep_ptr, uint16 len, uint8 *pointer)
{
	uint32 i;
	uint32 count;
	uint8 *fifo_p;

	count = len & 0x3ff;
	fifo_p = (u_int8 *)(BK3000_USBMEM_BASE + (ep_ptr->dmem_offset << 2));
	
	for(i=0; i < count; i++)             
	{
		*(fifo_p) = *(pointer++);
		fifo_p += 4;
	} 

	return count;
}

void udev_enable_receive(EP_ENTITY_PTR ep_ptr)
{
	if(0 == ep_ptr->index)
	{
		REG_USB_CFG_EP0_1 &= (~(EP_DIR_BIT_IN << EP_CFG_REG_DIR_POSITION)); 
		REG_USB_CFG_EP0_1 |= (EP0_ENABLE_BIT << EP_CFG_REG_ENABLE_POSITION); 
	}

	REG_USB_EP_RDY = (1 << ep_ptr->index);
}

inline void udev_set_ready_patch(void)
{
	int32 usb_line_status0;
	int32 usb_line_status1;
	int32 usb_line_status2;
	int32 usb_line_status3;
	int32 usb_line_status4;

	while(1)
	{
		usb_line_status0 = REG_USB_PWR_CN & 0x60; //((USB_POWER_DP << USB_POWER_DP_POSTION) | (USB_POWER_DN << USB_POWER_DN_POSTION));
		usb_line_status1 = REG_USB_PWR_CN & 0x60; // ((USB_POWER_DP << USB_POWER_DP_POSTION) | (USB_POWER_DN << USB_POWER_DN_POSTION));
		usb_line_status2 = REG_USB_PWR_CN & 0x60; // ((USB_POWER_DP << USB_POWER_DP_POSTION) | (USB_POWER_DN << USB_POWER_DN_POSTION));
		usb_line_status3 = REG_USB_PWR_CN & 0x60; // ((USB_POWER_DP << USB_POWER_DP_POSTION) | (USB_POWER_DN << USB_POWER_DN_POSTION));
		usb_line_status4 = REG_USB_PWR_CN & 0x60; // ((USB_POWER_DP << USB_POWER_DP_POSTION) | (USB_POWER_DN << USB_POWER_DN_POSTION));

		/*0x20 is single-ended zero status: SE0*/
		if((0x20 == usb_line_status0) 
			&& (0x20 == usb_line_status1)
			&& (0x20 == usb_line_status2)
			&& (0x20 == usb_line_status3)
			&& (0x20 == usb_line_status4))
		{
			return;
		}
	}
}

void udev_enable_send(EP_ENTITY_PTR ep_ptr)
{
	u_int8 *cnt0_p;
	u_int8 *cnt1_p;
	uint32 count;
	
	cnt0_p = (u_int8 *)(BK3000_USB_BASE_ADDR + 4 * (REG_USB_CNT0_INDEX + ep_ptr->index));
	cnt1_p = (u_int8 *)(BK3000_USB_BASE_ADDR + 4 * (REG_USB_CNT0_HBIT_INDEX + ep_ptr->index));

	count = MIN(ep_ptr->mps, (ep_ptr->data_req_len - ep_ptr->data_actual_len));
	
	*cnt0_p = (count & 0xff);  
	*cnt1_p = (count >> 8) & 0x3;
	
	if(0 == ep_ptr->index)
	{
		REG_USB_CFG_EP0_1 |= ((EP_DIR_BIT_IN << EP_CFG_REG_DIR_POSITION) 
								+ (EP0_ENABLE_BIT << EP_CFG_REG_ENABLE_POSITION)); 
	}

	udev_set_ready_patch();
	
	REG_USB_EP_RDY = (1 << ep_ptr->index);
}

void udev_update_ep_fifo_config(EP_ENTITY_PTR ep_ptr, int32 blk_id)
{	
	volatile uint32 *reg_cfg1_addr;
	volatile uint32 *reg_cfg0_addr;

	reg_cfg1_addr = (volatile uint32 *)(BK3000_USB_BASE_ADDR 
						+ ((EP01_REG_INDEX + (2 * ep_ptr->index)) * 4));
	reg_cfg0_addr = (volatile uint32 *)(BK3000_USB_BASE_ADDR 
						+ ((EP00_REG_INDEX + (2 * ep_ptr->index)) * 4));	
	*(reg_cfg1_addr) &= ~((EP_CFG_REG_ADDR_PART0_MASK) << EP_CFG_REG_ADDR_PART0_POSITION);

	*(reg_cfg1_addr) |= ((((ep_ptr->dmem_offset + (ep_ptr->dmem_cell_size * blk_id)) >> 8) & EP_CFG_REG_ADDR_PART0_MASK) << EP_CFG_REG_ADDR_PART0_POSITION);
	*(reg_cfg0_addr) = ((ep_ptr->dmem_offset + (ep_ptr->dmem_cell_size * blk_id)) & EP_CFG_REG_ADDR_PART1_MASK) << EP_CFG_REG_ADDR_PART1_POSITION;
}

void udev_tx_callback(void *param, uint32 count)
{
	uint32 deta_count;
	EP_ENTITY_PTR ep_ptr = (EP_ENTITY_PTR)param;
	
	/* check this successful sending, and update some variables*/
	deta_count = ep_ptr->data_req_len - ep_ptr->data_actual_len;
	if(deta_count <= ep_ptr->mps)
	{
		ep_ptr->data_actual_len = ep_ptr->data_req_len;
		ep_ptr->urb_status = URB_SEND_OVER;
		
		udev_update_ep_fifo_config(ep_ptr, 0);

		ep_ptr->cell_free_id = 0;
		ep_ptr->cell_use_id  = 0;
	}
	else
	{
		ep_ptr->data_actual_len += ep_ptr->mps;
		
		ep_ptr->cell_free_id += 1;
		udev_update_ep_fifo_config(ep_ptr, ep_ptr->cell_free_id & (ep_ptr->dmem_cell_count - 1));
		udev_enable_send(ep_ptr);		
	}			
}

void udev_rx_callback(void *param, uint32 count)
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
		ep_ptr->urb_status = URB_RECEIVE_OVER;
		udev_update_ep_fifo_config(ep_ptr, 0);
		
		ep_ptr->cell_free_id = 0;
		ep_ptr->cell_use_id  = 0;
	}
}

void set_address_cb(void *param, uint32 count)
{
	UDEV_PTR udev_ptr = (UDEV_PTR)param;
	EP_ENTITY_PTR ctrl_ep_ptr = udev_ptr->ep_esse[EP_0];
	
	udev_update_hw_address(setup.wValue.c[LSB]);

	ctrl_ep_ptr->cb         = 0;
	ctrl_ep_ptr->urb_status = URB_SEND_OVER;		
	ctrl_ep_ptr->status     = EP_IDLE;
}

void _ctrl_ep_tx_zero_packet_set_address(UDEV_PTR udev_ptr)
{
	uint8 zero = 0;
	
	udev_async_send(udev_ptr, EP_0, &zero, 0, set_address_cb);
}

void _ctrl_ep_tx_zero_packet(UDEV_PTR udev_ptr)
{
	uint8 zero = 0;

	udev_async_send(udev_ptr, EP_0, &zero, 0, _ctrl_ep_tx_cb);
}

void _ctrl_ep_rx_zero_packet(UDEV_PTR udev_ptr)
{
	uint8 zero = 0;
	
	udev_async_receive(udev_ptr, EP_0, &zero, 0, _ctrl_ep_rx_cb);
}

void _ctrl_ep_tx_cb(void *param, uint32 param2)
{
	int count;
	UDEV_PTR udev_ptr = (UDEV_PTR)param;
	EP_ENTITY_PTR ctrl_ep_ptr = udev_ptr->ep_esse[EP_0];

	count = ctrl_ep_ptr->data_req_len - ctrl_ep_ptr->data_actual_len;
	if(0 == ctrl_ep_ptr->data_req_len)
	{	
		/* handled zero packet*/
		ctrl_ep_ptr->urb_status = URB_SEND_OVER;		
		ctrl_ep_ptr->status = EP_IDLE;
		
		UDEV_PRT("ep0_txed_0\r\n");
	}
	else if(count > ctrl_ep_ptr->mps)
	{
		/* continue sending data, update the configuration*/
		ctrl_ep_ptr->data_actual_len += ctrl_ep_ptr->mps;
		
		ctrl_ep_ptr->cell_free_id += 1;
		udev_update_ep_fifo_config(ctrl_ep_ptr, ctrl_ep_ptr->cell_free_id & (ctrl_ep_ptr->dmem_cell_count - 1));
		udev_enable_send(ctrl_ep_ptr);	
		UDEV_PRT("ep0_tx_again\r\n");	
	}
	else
	{
		/* send over, rx zero packet at the handshake phase*/
		ctrl_ep_ptr->urb_status = URB_SEND_OVER;
		ctrl_ep_ptr->data_actual_len = ctrl_ep_ptr->data_req_len;
		
		_ctrl_ep_rx_zero_packet(udev_ptr);
		UDEV_PRT("ep0_txcb_rx0\r\n");
	}

	return;
}

void _ctrl_ep_rx_cb(void *param, uint32 param2)
{
	uint32 count;
	UDEV_PTR udev_ptr = (UDEV_PTR)param;
	EP_ENTITY_PTR ctrl_ep_ptr = udev_ptr->ep_esse[EP_0];

	count = ctrl_ep_ptr->data_req_len - ctrl_ep_ptr->data_actual_len;
	if(0 == ctrl_ep_ptr->data_req_len)
	{	
		/* handled zero packet*/
		ctrl_ep_ptr->urb_status = URB_RECEIVE_OVER;
		ctrl_ep_ptr->status = EP_IDLE;
		UDEV_PRT("ep0_rxed_0\r\n");
	}
	else if(count > ctrl_ep_ptr->mps)
	{
		/* continue receiving data, read data, and update the configuration*/
		udev_read_fifo(ctrl_ep_ptr, 
						ctrl_ep_ptr->mps, 
						ctrl_ep_ptr->data_ptr + ctrl_ep_ptr->data_actual_len,
						ctrl_ep_ptr->dmem_offset);
		
		ctrl_ep_ptr->data_actual_len += ctrl_ep_ptr->mps;
		
		udev_enable_receive(ctrl_ep_ptr);
		UDEV_PRT("ep0_rx_again\r\n");
	}
	else
	{
		/* receive over, rx zero packet at the handshake phase*/
		ctrl_ep_ptr->urb_status = URB_RECEIVE_OVER;

		udev_read_fifo(ctrl_ep_ptr, 
						count, 
						ctrl_ep_ptr->data_ptr + ctrl_ep_ptr->data_actual_len,
						ctrl_ep_ptr->dmem_offset);
		
		ctrl_ep_ptr->data_actual_len = ctrl_ep_ptr->data_req_len;
		
		_ctrl_ep_tx_zero_packet(udev_ptr);
		UDEV_PRT("ep0_rxcb_tx0\r\n");
	}

	return;
}


uint32 udev_async_send(
      /* [IN] the USB_USB_dev_initialize state structure */
      UDEV_PTR                  handle,
            
      /* [IN] the Endpoint number */
      uint32                     ep_num,
            
      /* [IN] buffer to send data */
      uint8                     *buff_ptr,
            
      /* [IN] length of the transfer */
      uint32                     size,

      /* [IN] send context*/
      callback_func             cb
   )
{
	uint32 ret;
	EP_ENTITY_PTR ep_ptr;
	
	ep_ptr = handle->ep_esse[ep_num];
	
	if((!handle) 
		|| (ep_num > EP_MAX_INDEX) 
		|| (!buff_ptr)
		|| (URB_SENDING == ep_ptr->urb_status))
	{
		goto tx_fatal;
	}
	
	ret = udev_write_fifo(ep_ptr, size, buff_ptr);

	ep_ptr->urb_status      = URB_SENDING;
	ep_ptr->data_ptr        = buff_ptr;
	ep_ptr->data_req_len    = ret;
	ep_ptr->data_actual_len = 0;
	ep_ptr->cb              = cb;
	ep_ptr->sync_flag       = URB_ASYNC;

	udev_enable_send(ep_ptr);
		
	return RET_USB_SUCCESS;
		
tx_fatal:	
	USB_FATAL();
	
	return RET_USB_FAILURE;
}

uint32 udev_sync_send(
      /* [IN] the USB_USB_dev_initialize state structure */
      UDEV_PTR                  handle,
            
      /* [IN] the Endpoint number */
      uint32                     ep_num,
            
      /* [IN] buffer to send data */
      uint8                     *buff_ptr,
            
      /* [IN] length of the transfer */
      uint32                     size,

      /* [IN] send context*/
      callback_func             cb
   )
{
	uint32 ret;
	EP_ENTITY_PTR ep_ptr;
	
	ep_ptr = handle->ep_esse[ep_num];
	
	if((!handle) 
		|| (ep_num > EP_MAX_INDEX) 
		|| (!buff_ptr)
		|| (URB_SENDING == ep_ptr->urb_status))
	{
		goto tx_fatal;
	}
	
	ret = udev_write_fifo(ep_ptr, size, buff_ptr);

	ep_ptr->urb_status        = URB_SENDING;
	ep_ptr->data_ptr           = buff_ptr;
	ep_ptr->data_req_len    = ret;
	ep_ptr->data_actual_len = 0;
	ep_ptr->cb                   = cb;
	ep_ptr->sync_flag        = URB_SYNC;

	udev_enable_send(ep_ptr);

	/*condition: no os, no task*/
	while(URB_SEND_OVER != ep_ptr->urb_status)
	{
		;
	}
		
	return RET_USB_SUCCESS;
		
tx_fatal:
	USB_FATAL();
	
	return RET_USB_FAILURE;
}

uint32 udev_async_receive(
      /* [IN] the USB_USB_dev_initialize state structure */
      UDEV_PTR                   handle,
            
      /* [IN] the Endpoint number */
      uint32                     ep_num,
            
      /* [IN] buffer to receive data */
      uint8                     *buff_ptr,
            
      /* [IN] length of the transfer */
      uint32                     size,

      /* [IN] receive context*/
      callback_func              cb
   )
{
	EP_ENTITY_PTR ep_ptr;
	
	ep_ptr = handle->ep_esse[ep_num];
	
	if((!handle) 
		|| (ep_num > EP_MAX_INDEX) 
		|| (!buff_ptr)
		|| (URB_RECEIVING == ep_ptr->urb_status))
	{
		goto tx_fatal;
	}
	
	ep_ptr->urb_status      = URB_RECEIVING;
	ep_ptr->data_ptr        = buff_ptr;
	ep_ptr->data_req_len    = size;
	ep_ptr->cb              = cb;
	ep_ptr->data_actual_len = 0;
	ep_ptr->sync_flag       = URB_ASYNC;

	udev_enable_receive(ep_ptr);
		
	return RET_USB_SUCCESS;
		
tx_fatal:
	USB_FATAL();
	
	return RET_USB_FAILURE;
}

uint32 udev_sync_receive(
      /* [IN] the USB_USB_dev_initialize state structure */
      UDEV_PTR                   handle,
            
      /* [IN] the Endpoint number */
      uint32                     ep_num,
            
      /* [IN] buffer to receive data */
      uint8                     *buff_ptr,
            
      /* [IN] length of the transfer */
      uint32                     size,

      /* [IN] receive context*/
      callback_func              cb
   )
{
	uint32 cell_len;
	uint32 total_len;
	uint8 *cpy_ptr;
	uint32 dmem_offset;
	EP_ENTITY_PTR ep_ptr;

	ep_ptr = handle->ep_esse[ep_num];
	
	if((!handle) 
		|| (ep_num > EP_MAX_INDEX) 
		|| (!buff_ptr)
		|| (URB_RECEIVING == ep_ptr->urb_status))
	{
		goto tx_fatal;
	}
	
	UDEV_PRT("udev_sync_receive\r\n");
	ep_ptr->urb_status      = URB_RECEIVING;
	ep_ptr->data_ptr        = buff_ptr;
	ep_ptr->data_req_len    = size;
	ep_ptr->cb              = cb;
	ep_ptr->data_actual_len = 0;
	ep_ptr->sync_flag       = URB_SYNC;

	udev_enable_receive(ep_ptr);

	/* get the data*/
	total_len = size;
	cpy_ptr = buff_ptr;
	while(total_len > 0)
	{
		if(ep_ptr->cell_use_id > ep_ptr->cell_free_id)
		{
			/* read*/
			cell_len = MIN(total_len, ep_ptr->mps);
			dmem_offset = ep_ptr->dmem_offset + ep_ptr->dmem_cell_size * (ep_ptr->cell_free_id * (ep_ptr->dmem_cell_count - 1));
			udev_read_fifo(ep_ptr, cell_len, cpy_ptr, dmem_offset);
			total_len -= cell_len;
			cpy_ptr = (uint8 *)(cell_len + (uint32)cpy_ptr);

			/* update*/
			ep_ptr->cell_free_id += 1;

			/* waiting for mem, rx again*/
			if(URB_RECEIVE_SUSPEND == ep_ptr->urb_status)
			{
				ep_ptr->cell_use_id += 1;
				
				/* update rx dmem*/
				udev_update_ep_fifo_config(ep_ptr, (ep_ptr->cell_use_id & (ep_ptr->dmem_cell_count - 1)));
				
				/* re-enable rx*/
				udev_enable_receive(ep_ptr);
			}
		}
	}
		
	/*condition: no os, no task*/
	while(URB_RECEIVE_OVER != ep_ptr->urb_status)
	{
		;
	}
	
	return RET_USB_SUCCESS;
		
tx_fatal:
	USB_FATAL();
	
	return RET_USB_FAILURE;
}

uint32 udev_get_suspend_status(void)
{
	return usb_dev.state & DEV_SUSPENDED;
}

#ifdef USB_TEST_SPEED
#define TEST_BUF_LEN              128
#define RX_COUNT                  128

uint8 test_buf[TEST_BUF_LEN];

void udev_test_rx_callback(void *param)
{	
	EP_ENTITY_PTR ep_ptr = (EP_ENTITY_PTR)param;
	
	ep_ptr->urb_status = URB_RECEIVE_OVER;
	
	udev_async_receive(&usb_dev, 
		EP_BULK_OUT, 
		test_buf, 
		RX_COUNT, 
		udev_test_rx_callback);
}

int udev_check_set_configuration_status(void)
{
	return (DEV_CONFIGURED & usb_dev.state);
}

void udev_startup_receive(void)
{

	UDEV_PRT("udev_startup_receive\r\n");

	udev_async_receive(&usb_dev, 
		EP_BULK_OUT, 
		test_buf, 
		RX_COUNT, 
		udev_test_rx_callback);
}

#endif
#endif

// end of file
