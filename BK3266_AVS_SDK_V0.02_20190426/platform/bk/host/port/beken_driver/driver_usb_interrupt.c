#include "driver_beken_includes.h"
#include "usb.h"

#ifdef CONFIG_APP_USB_DISK
extern UDEV usb_dev;
extern setup_buffer setup; 

/*! \fn void _usb_isr_data_control(void)
    \brief Process the control data from usb endpoint0.

    \param void
    \return void
*/
static void _usb_parsing_setup_content(EP_ENTITY_PTR ctrl_ep_ptr)
{
    switch (setup.bmRequestType & 0x60)
    {
    	case REQUESTTYPE_STANDARD:						
            switch(setup.bRequest)
            {                             
                case GET_STATUS:
                   driver_usb_get_status(ctrl_ep_ptr);
                   break;
                case CLEAR_FEATURE:
                   driver_usb_clear_feature(ctrl_ep_ptr);
                   break;
                case SET_FEATURE:
                   driver_usb_set_feature(ctrl_ep_ptr);
                   break;
                case SET_ADDRESS:
                   driver_usb_set_address(ctrl_ep_ptr);
                   break;
                case GET_DESCRIPTOR:               
                   driver_usb_get_descriptor(ctrl_ep_ptr);
                   break;
                case GET_CONFIGURATION:
                   driver_usb_get_configuration(ctrl_ep_ptr);
                   break;
                case SET_CONFIGURATION:
                   driver_usb_set_configuration(ctrl_ep_ptr);
                   break;
                case GET_INTERFACE:
                   driver_usb_get_interface(ctrl_ep_ptr);
                   break;
                case SET_INTERFACE:
                   driver_usb_set_interface(ctrl_ep_ptr);
                   break;
                default:
                   udev_ep_put_on_halt(ctrl_ep_ptr);   
                   break;
            }
            break;
			
    	case REQUESTTYPE_CLASS:
            switch(setup.bRequest) {		
				case MASS_STORAGE_RESET:
					msc_ep0_mass_storage_reset(ctrl_ep_ptr, &setup);
					break;
					
				case GET_MAX_LUN:
					msc_ep0_get_max_lun(ctrl_ep_ptr, &setup);
					break;		
					
				#ifdef HID_CLASS
                case GET_REPORT:
                    driver_usb_get_report();
                    break;
                case SET_REPORT:
                    driver_usb_set_report();          
                    break;
                case GET_IDLE:
                    driver_usb_get_idle();
                    break;
                case SET_IDLE:
                    driver_usb_set_idle();
                    break;
                case GET_PROTOCOL:
                    driver_usb_get_protocol();
                    break;
                case SET_PROTOCOL:
                    driver_usb_set_protocol();
                    break;
                case GET_CURRENT:
                    driver_usb_get_interface_current();
                    break;
                case GET_MINIUM:
                    driver_usb_get_interface_min();
                    break;
                case GET_MAXIUM:
                    driver_usb_get_interface_max();
                    break;
                case GET_RES:
                    driver_usb_get_interface_resolution();
                    break;	
				#endif // HID_CLASS
				
                default:
                    udev_ep_put_on_halt(ctrl_ep_ptr);                                   // Send stall to host if invalid request
                    break;                  
            }
            break;
			
    	case REQUESTTYPE_VENDOR:
			udev_ep_put_on_halt(ctrl_ep_ptr);
			break;
			
    	default:
			udev_ep_put_on_halt(ctrl_ep_ptr);
			break;
    }
}

static void _usb_get_setup_content(void)
{   
	/* Get setup Packet from Fifo, it is currently Big-Endian*/
	setup.bmRequestType  = EP0_ADDR_DEF;
	setup.bRequest       = EP0_ADDR_DEF_1;
	setup.wValue.c[MSB]  = EP0_ADDR_DEF_3; 
	setup.wValue.c[LSB]  = EP0_ADDR_DEF_2; 
	setup.wIndex.c[MSB]  = EP0_ADDR_DEF_4;
	setup.wIndex.c[LSB]  = EP0_ADDR_DEF_5;
	setup.wLength.c[MSB] = EP0_ADDR_DEF_6;
	setup.wLength.c[LSB] = EP0_ADDR_DEF_7;
}

static void _usb_isr_data_control(UDEV_PTR udev_ptr, uint32 status)
{
	EP_ENTITY_PTR ctrl_ep_ptr;

	ctrl_ep_ptr = udev_ptr->ep_esse[EP_0];

	if(REG_USB_EP_STATUS_OUT & EP_0_BIT)
	{
	    REG_USB_EP_STATUS_IN  = EP_0_BIT;  
	    REG_USB_EP_STATUS_OUT = EP_0_BIT;   
		REG_USB_INT0 = 0x80;
		
		if((EP_STALL == ctrl_ep_ptr->status))
		{
			ctrl_ep_ptr->status = EP_IDLE;              // usb controller must accept setup packet, and clear stall status automatically
		}

		if((EP_IDLE == ctrl_ep_ptr->status) 
				&& (SETUP_PKT_LEN == REG_USB_CNT0))     // sizeof(setup_buffer)
		{
			ctrl_ep_ptr->urb_status = 0;
			_usb_get_setup_content();
			_usb_parsing_setup_content(ctrl_ep_ptr);			
		}		
	}
	else
	{  
		REG_USB_INT0 = 0xc0;           // trip
		
		if((ctrl_ep_ptr) && (ctrl_ep_ptr->cb))
		{
			(*(ctrl_ep_ptr->cb))((void *)udev_ptr, 0);
		}		
	}

	return;
}

/*! \fn void _usb_isr_data_receive(void)
    \brief Process the interrupt data from usb endpoint1-7.
*/
static void _usb_isr_data_receive(UDEV_PTR udev_ptr)
{ 
	uint32 count;
	uint32 ep;
	uint32 ep_id;
	uint32 ep_status;
	EP_ENTITY_PTR ep_ptr;
	volatile uint32 *cnt_reg0_addr;
	volatile uint32 *cnt_reg1_addr;

	ep_status = REG_USB_EP_STATUS_OUT;
	 __asm volatile( "b.ff1 %0, %1;"
					 :"=r"(ep_id)
					 :"r"(ep_status)
					 :"r26"
					 );
	ep = ep_id - 1;

	if(ep)
	{
		/* clear isr status, pay attention pls: clear ep status first, 
		   and clear usb interrupt status, next*/		
		ep_ptr = udev_ptr->ep_esse[ep];	                       
		cnt_reg0_addr = (volatile uint32 *)(BK3000_USB_BASE_ADDR 
							+ ((REG_USB_CNT0_INDEX + ep_ptr->index) * 4));
		cnt_reg1_addr = (volatile uint32 *)(BK3000_USB_BASE_ADDR 
							+ ((REG_USB_CNT0_HBIT_INDEX + ep_ptr->index) * 4));
		count = ((*cnt_reg1_addr & 0x3) << 8) + (*cnt_reg0_addr & 0xff);
		
		REG_USB_EP_STATUS_OUT |= (1 << ep);
		REG_USB_INT0 = 0x20;
	
		if((ep_ptr) && (ep_ptr->cb))
		{
			(*((ep_ptr->cb)))((void *)ep_ptr, count);
		}
	}
	
    return;
}

/*! \fn void _usb_isr_data_send(void)
    \brief Send the input data through usb endpoint1-7 to PC.

    \param void
    \return void
*/
inline void _usb_isr_data_send(UDEV_PTR udev_ptr)
{ 
	uint32 ep;
	uint32 ep_id;
	uint32 ep_status;
	EP_ENTITY_PTR ep_ptr;

	ep_status = REG_USB_EP_STATUS_IN;

	 __asm volatile( "b.ff1 %0, %1;"
					 :"=r"(ep_id)
					 :"r"(ep_status)
					 :"r26"
					 );
	ep = ep_id - 1;

	if(ep)
	{
		/* clear isr status, pay attention pls: clear ep status first,
		   and clear usb interrupt status, next*/
		REG_USB_EP_STATUS_IN |= (1 << ep);
		REG_USB_INT0 = 0x10;

		ep_ptr = udev_ptr->ep_esse[ep];		
		if((ep_ptr) && (ep_ptr->cb))
		{
			(*((ep_ptr->cb)))((void *)ep_ptr, 0);
		}
	}
	
    return;
}

/*! \fn void _usb_isr_reset(void)
    \brief Reset usb after receiving the command from usb interrupt.

    \param void
    \return void
*/
static void _usb_isr_reset(UDEV_PTR udev_ptr)
{
	int i;
	EP_ENTITY_PTR ep_ptr;

	UDEV_PRT("_usb_isr_reset\r\n");
	if(DEV_CONFIGURED == usb_dev.state)
	{
	#if 1
		udev_peripheral_clock_disable();
		REG_USB_INT0 = 0xC8;
		return;
	#else
		USB_FATAL();
	#endif
	}

	udev_peripheral_clock_enable();	

	udev_put_off_suspend();            

	udev_put_on_reset();

	udev_clear_hw_address();

	udev_set_status(udev_ptr, DEV_DEFAULT);

	usb_dev.ep_esse[EP_0]->status = EP_IDLE;
	usb_dev.ep_esse[EP_0]->urb_status = 0;
	
	for(i = EP_1; i < EP_MAX_INDEX; i ++)
	{
		ep_ptr = usb_dev.ep_esse[i];
		if(ep_ptr)
		{
			ep_ptr->status =     EP_STALL;
			ep_ptr->cb     =     0;
			ep_ptr->urb_status = 0;
		}
	}

	REG_USB_INT0 = 0xC8;

	udev_exit_suspend(udev_ptr);

#ifdef USB_DEBUG
	udev_ptr->usb_intr_reset_cnt ++;
	udev_ptr->usb_intr_reset_timestamp = os_get_tick_counter();
#endif
}

/*! \fn void _usb_isr_suspend(void)
    \brief Suspend usb after receiving the command from usb interrupt.

    \param void
    \return void
*/
static void _usb_isr_suspend(UDEV_PTR udev_ptr)
{  
	udev_put_on_reset();         

	REG_USB_INT0 = 0x04;             

	udev_enter_suspend(udev_ptr);
	UDEV_PRT("pmu_cfg:0x%x\r\n", BK3000_PMU_CONFIG);
#ifdef USB_DEBUG
	udev_ptr->usb_intr_suspend_cnt ++;
	udev_ptr->usb_intr_suspend_timestamp = os_get_tick_counter();
#endif
}

/*! \fn void _usb_isr_resume(void)
    \brief Resume usb after receiving the command from usb interrupt.

    \param void
    \return void
*/
static void _usb_isr_resume(UDEV_PTR udev_ptr)
{
	udev_peripheral_clock_enable();	

	udev_put_off_suspend();       

	REG_USB_INT0 = 0x02;                    //clear resume interrupt flag

	udev_exit_suspend(udev_ptr);

#ifdef USB_DEBUG
	udev_ptr->usb_intr_resume_cnt ++;
	udev_ptr->usb_intr_resume_timestamp = os_get_tick_counter();
#endif
}

/*! \fn void _usb_isr_sof(void)
    \brief Process sof event of usb.

    \param void
    \return void
*/
static void _usb_isr_sof(UDEV_PTR udev_ptr)
{
    REG_USB_INT0 = 0x01;
}

/*! \fn void _usb_isr_unsupported_token(void)
    \brief Process unsupported token event of usb.

    \param void
    \return void
*/
static void _usb_isr_unsupported_token(UDEV_PTR udev_ptr)
{
    REG_USB_INT1 = 0x80;
	
	#ifdef USB_DEBUG
	udev_ptr->usb_intr_unsupported_token_cnt ++;
	udev_ptr->usb_intr_unsupported_token_timestamp = os_get_tick_counter();
	#endif
}

/*! \fn void _usb_isr_crc_error(void)
    \brief Process crc error event of usb.

    \param void
    \return void
*/
static void _usb_isr_crc_error(UDEV_PTR udev_ptr)
{
	REG_USB_INT1 = 0x40;

#ifdef USB_DEBUG
	udev_ptr->usb_intr_crc_error_cnt ++;
	udev_ptr->usb_intr_crc_error_timestamp = os_get_tick_counter();
#endif
}

/*! \fn void _usb_isr_overtime(void)
    \brief Process overtime error event of usb.

    \param void
    \return void
*/
static void _usb_isr_overtime(UDEV_PTR udev_ptr)
{
	REG_USB_INT1 = 0x20;

#ifdef USB_DEBUG
	udev_ptr->usb_intr_overtime_cnt ++;
	udev_ptr->usb_intr_overtime_timestamp = os_get_tick_counter();
#endif
}

/*! \fn void _usb_isr_data_pid_error(void)
    \brief Process data pid error event of usb.

    \param void
    \return void
*/
static void _usb_isr_data_pid_error(UDEV_PTR udev_ptr)
{
	REG_USB_INT1 = 0x10;

#ifdef USB_DEBUG
	udev_ptr->usb_intr_data_pid_error_cnt ++;
	udev_ptr->usb_intr_data_pid_error_timestamp = os_get_tick_counter();
#endif
}

/*! \fn void usb_isr(void)
    \brief The interrupt service routine of usb module.

    Call \a _usb_isr_data_control to process the data from endpoint0.\n
    Call \a _usb_isr_data_receive to process the data from endpoint1-7.\n
    Call \a _usb_isr_data_send to send the data to PC from endpoint1-7.\n
    Call \a _usb_isr_reset to reset usb.\n
    Call \a _usb_isr_suspend to suspend usb.\n
    Call \a _usb_isr_resume to restore usb by \a
    Call \a _usb_isr_sof to process sof event.\n
    Call \a _usb_isr_unsupported_token to process the unsupported token.\n
    Call \a _usb_isr_crc_error to process crc error event.\n
    Call \a _usb_isr_overtime to process overtime event.\n
    Call \a _usb_isr_data_pid_error to process pid error event.
    \param void
    \return void
*/
void usb_isr(void)
{ 
	UDEV_PTR udev_ptr = &usb_dev;
	uint32 oldmask = get_spr(SPR_VICMR(0)); 
	uint32 usb_status0 = REG_USB_EN0 & REG_USB_INT0;
	uint32 usb_status1 = REG_USB_EN1 & REG_USB_INT1;

    set_spr(SPR_VICMR(0), oldmask & (1 << VIC_AUD_ISR_INDEX));
    cpu_set_interrupts_enabled(1);

    if(usb_status0 & 0x20)            // Handle ep1-7 receive_data interrupt
    {	
        _usb_isr_data_receive(udev_ptr); 
    }  	         
    
    if(usb_status0 & 0x10)            // Handle ep1-7 send_data interrupt
    {	
		//gpio_output(7, 1);
        _usb_isr_data_send(udev_ptr);
    }
	
    if(usb_status0 & 0x01)            // Handle SOF interrupt
    {	
		UDEV_PRT("_usb_isr_sof\r\n");
		_usb_isr_sof(udev_ptr);
    }

    if(usb_status0 & 0xc0)             // Handle ep0 receive_data and send_data interrupt
    {	
        _usb_isr_data_control(udev_ptr, usb_status0);
    }  

    if(usb_status0 & 0x08)            // USB_rst interrupt
    {	
		UDEV_PRT("_usb_isr_reset\r\n");
        _usb_isr_reset(udev_ptr);   
    }                            

    if(usb_status0 & 0x04)            // Handle Suspend interrupt
    {	
		UDEV_PRT("_usb_isr_suspend\r\n");
        _usb_isr_suspend(udev_ptr);
    }

    if(usb_status0 & 0x02)            // Handle resume interrupt
    {	
		UDEV_PRT("_usb_isr_resume\r\n");
        _usb_isr_resume(udev_ptr);
    }

    if(usb_status1 & 0x80)           
    {	
		UDEV_PRT("_usb_isr_unsupported_token\r\n");
        _usb_isr_unsupported_token(udev_ptr);
    }

    if(usb_status1 & 0x40)       
    {	
		UDEV_PRT("_usb_isr_crc_error\r\n"); 
        _usb_isr_crc_error(udev_ptr);
    }   

    if(usb_status1 & 0x20)           
    {	
		UDEV_PRT("_usb_isr_overtime\r\n");
        _usb_isr_overtime(udev_ptr);
    }

    if(usb_status1 & 0x10)       
    {	
		UDEV_PRT("_usb_isr_data_pid_error\r\n");   
        _usb_isr_data_pid_error(udev_ptr);
    }
	
	/*enable vector interrupt controller isrs*/
    cpu_set_interrupts_enabled(0);
	set_spr(SPR_VICMR(0), oldmask); 
}

#endif
/***********************************************************
						end file
***********************************************************/
