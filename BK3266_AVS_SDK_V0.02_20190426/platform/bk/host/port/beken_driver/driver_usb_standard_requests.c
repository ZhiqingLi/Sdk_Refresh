#include "includes.h"
#include "usb.h"

#ifdef CONFIG_APP_USB_DISK
uint8 USB_IF_ALT[4] = { 0, 0, 0, 0};
volatile int32 flag_usb_remote_wakeup = 1;

u_int8 usb_device_descriptor[DEVICE_DESCRIPTOR_SIZE] =
{
    DEVICE_DESCRIPTOR_SIZE,              // bLength
    0x01,                                // bDescriptorType
    USB_UINT_16_LOW(0x0110),             // bcdUSB
    USB_UINT_16_HIGH(0x0110),            // bcdUSB
    0x00,                                // bDeviceClass
    0x00,                                // bDeviceSubClass
    0x00,                                // bDeviceProtocol
    EP0_FS_MAX_PACKET_SIZE,              // bMaxPacketSize0
    USB_UINT_16_LOW(BEKEN_VID),          // idVendor
    USB_UINT_16_HIGH(BEKEN_VID),         // idVendor
    USB_UINT_16_LOW(BK3260_PID),         // idProduct
    USB_UINT_16_HIGH(BK3260_PID),        // idProduct
    USB_UINT_16_LOW(USB_BCD),            // bcdDevice
    USB_UINT_16_HIGH(USB_BCD),           // bcdDevice
    0x01,                                // iManufacturer
    0x02,                                // iProduct
    0x00,                                // iSerialNumber
    0x01                                 // bNumConfigurations
}; 

u_int8 usb_configuration_descriptor[CONFIG_DESC_SIZE] =
{
   /* Configuration Descriptor - always 9 bytes */
   CONFIG_DESCRIPTOR_SIZE,
   /* "Configuration" type of descriptor */
   2,
   /* Total length of the Configuration descriptor */
   USB_UINT_16_LOW(CONFIG_DESC_SIZE), 
   USB_UINT_16_HIGH(CONFIG_DESC_SIZE),
   /* NumInterfaces */
   1,
   /* Configuration Value */
   1,
   /* Configuration Description String Index*/
   0,
   /* Attributes.  Self-powered */
   0x80,
   /* Current draw from bus */
   0xc8,
   /* Interface 0 Descriptor - always 9 bytes */
   9,
   /* "Interface" type of descriptor */
   4,
   /* Number of this interface */
   MASS_STORAGE_INTERFACE,
   /* Alternate Setting */
   0,
   /* Number of endpoints on this interface */
   2,
#ifdef USB_TEST_SPEED
	0xff, /* vendor specified interface class*/
	0xff, /* vendor specified interface subclass*/
	0xff, /* vendor specified interface protocol*/
#else
   /* Interface Class */
   0x08,
   /* Interface Subclass: SCSI transparent command set */
   0x06,
   /* Interface Protocol: Bulk only protocol */
   0x50,
#endif
   /* Interface Description String Index */
   0,
   /* Endpoint 1 (Bulk In Endpoint), Interface 0 Descriptor - always 7 bytes*/
   7,
   /* "Endpoint" type of descriptor */
   5,
   /*
   ** Endpoint address.  The low nibble contains the endpoint number and the
   ** high bit indicates TX(1) or RX(0).
   */
   (EP_BULK_IN + DIRECTION_EP_IN),
   /* Attributes.  0=Control 1=Isochronous 2=Bulk 3=Interrupt */
   0x02,
   /* Max Packet Size for this endpoint */
   USB_UINT_16_LOW(EP_FS_MAX_PACKET_SIZE), 
   USB_UINT_16_HIGH(EP_FS_MAX_PACKET_SIZE),
   /* Polling Interval (ms) */
   0,
   /* Endpoint 2 (Bulk Out Endpoint), Interface 0 Descriptor - always 7 bytes*/
   7,
   /* "Endpoint" type of descriptor */
   5,
   /*
   ** Endpoint address.  The low nibble contains the endpoint number and the
   ** high bit indicates TX(1) or RX(0).
   */
   EP_BULK_OUT,
   /* Attributes.  0=Control 1=Isochronous 2=Bulk 3=Interrupt */
   0x02,
   /* Max Packet Size for this endpoint */
   USB_UINT_16_LOW(EP_FS_MAX_PACKET_SIZE), 
   USB_UINT_16_HIGH(EP_FS_MAX_PACKET_SIZE),
   /* Polling Interval (ms) */
   0
}
;

CONST u_int8 usb_string_LANGID[LEN_STRING_LANGID] =
{
    LEN_STRING_LANGID, 0x03, 0x09, 0x04
}; 

CONST u_int8 usb_string_manufacturer[LEN_STRING_MANUFACTURER] =
{
   LEN_STRING_MANUFACTURER, 0x03,
   'B', 0,
   'e', 0,
   'k', 0,
   'e', 0,
   'n', 0,
   ' ', 0,
   'C', 0,
   'o', 0,
   'r', 0,
   'p', 0,
   'o', 0,
   'r', 0,
   'a', 0,
   't', 0,
   'i', 0,
   'o', 0,
   'n', 0
}; 

CONST u_int8 usb_string_production[LEN_STRING_PRODUCT] =
{
   LEN_STRING_PRODUCT, 0x03,
   'B', 0,
   'e', 0,
   'k', 0,
   'e', 0,
   'n', 0,
   ' ', 0,
   'U', 0,
   'd', 0,
   'i', 0,
   's', 0,
   'k', 0,
   ' ', 0,
   'D', 0,
   'e', 0,
   'v', 0,
   'i', 0,
   'c', 0,
   'e', 0
};

                             
CONST u_int8 * usb_string_table[LEN_USB_STRING] =
{
   usb_string_LANGID,
   usb_string_manufacturer,
   usb_string_production
};

setup_buffer setup; 

extern UDEV usb_dev;

/*! \fn void driver_usb_get_status(void)
    \brief  This routine returns a two byte status packet to the host.
    \return void
*/
void driver_usb_get_status(EP_ENTITY_PTR ctrl_ep_ptr)
{
	int32 len = 0;
	uint8 ptr[2] = {0, 0};
	UDEV_PTR udev_ptr = &usb_dev;
	
	UDEV_PRT("driver_usb_get_status\r\n");
    /*If non-zero return length or data length not Equal to 2, then send a stall indicating invalid request*/
    if(setup.wValue.c[MSB] 
		|| setup.wValue.c[LSB] 
		|| setup.wLength.c[MSB] 
		|| (2 != setup.wLength.c[LSB]))
    {
        udev_ep_put_on_halt(ctrl_ep_ptr);
    }

    switch(setup.bmRequestType)
    {
        case OUT_DEVICE:
            if(flag_usb_remote_wakeup)
            {
            	ptr[0] = 2;
				
            }
			len = 2;
            break;
			
        case OUT_INTERFACE:
            if((udev_ptr->state != DEV_CONFIGURED))
            {
                udev_ep_put_on_halt(ctrl_ep_ptr);
            }
            else
            {
				len = 2;
            }
            break;
			
        case OUT_ENDPOINT:
            if(usb_dev.state != DEV_CONFIGURED)
            {
                udev_ep_put_on_halt(ctrl_ep_ptr);
            }
            else
            {
				len = 2;
            }
            break;
			
        default:
            udev_ep_put_on_halt(ctrl_ep_ptr);
            break;
    }
    
    if((len) && (EP_STALL != ctrl_ep_ptr->status))
    {
    	udev_async_send(udev_ptr, EP_0, ptr, len, _ctrl_ep_tx_cb);
    }
}

/*! \fn void driver_usb_clear_feature(void)
    \brief  This routine can clear halt endpoint features on endpoint 1.
    \param void
    \return void
*/
void driver_usb_clear_feature(EP_ENTITY_PTR ctrl_ep_ptr)
{
	uint32 ep_index;
	EP_ENTITY_PTR ep_ptr;
	
	UDEV_PRT("driver_usb_clear_feature\r\n");
    // Send procedural stall if device isn't configured request is made to host(remote wakeup not supported) Or data length set to non-zero Or request is made to interface
    if((usb_dev.state != DEV_CONFIGURED) || (setup.bmRequestType == IN_INTERFACE) || setup.wLength.c[MSB] || setup.wLength.c[LSB])
    {
        udev_ep_put_on_halt(ctrl_ep_ptr);
    }
    else
    {
        // Verify that packet was directed at an endpoint // flag_usb_remote_wakeup feature is selected
        if((setup.bmRequestType == IN_DEVICE) && (setup.wValue.c[LSB] == DEVICE_REMOTE_WAKEUP))
        {
            flag_usb_remote_wakeup = 0;
        }
        else if((setup.bmRequestType == IN_ENDPOINT) && (setup.wValue.c[LSB] == ENDPOINT_HALT))     // The feature selected was HALT_ENDPOINT
        {
        	if(setup.wIndex.c[LSB] & 0x80)
        	{
        		ep_index = setup.wIndex.c[LSB] & 0x7f;

				if(ep_index < EP_MAX_INDEX)
				{
					ep_ptr = usb_dev.ep_esse[ep_index];
					if(ep_ptr)
					{
						udev_ep_put_off_halt(ep_ptr);
					}
				}
        	}
        }
        else
        {
            udev_ep_put_on_halt(ctrl_ep_ptr);               // Send procedural stall
        }
    }
	
    if(EP_STALL != ctrl_ep_ptr->status)
    {
    	_ctrl_ep_tx_zero_packet(&usb_dev);
    }
}

/*! \fn void driver_usb_set_feature(void)
    \brief  This routine can set halt endpoint features on endpoint 1.
    \param void
    \return void
*/
void driver_usb_set_feature(EP_ENTITY_PTR ctrl_ep_ptr)
{
	uint32 ep_index;
	EP_ENTITY_PTR ep_ptr;
	
	UDEV_PRT("driver_usb_set_feature\r\n");
    // Make sure device is configured, setup data is all valid and that request is directed at an endpoint
    if((usb_dev.state != DEV_CONFIGURED) 
		|| (setup.bmRequestType == IN_INTERFACE) || setup.wLength.c[MSB] || setup.wLength.c[LSB])
    {
        udev_ep_put_on_halt(ctrl_ep_ptr);
    }
    else
    {
        if((setup.bmRequestType == IN_DEVICE) 
			&& (setup.wValue.c[LSB] == DEVICE_REMOTE_WAKEUP))//flag_usb_remote_wakeup feature is selected
        {
            flag_usb_remote_wakeup = 1;
        }
        else if((setup.bmRequestType == IN_ENDPOINT) 
			&& (setup.wValue.c[LSB] == ENDPOINT_HALT))  //endpoint feature is selected
        {
        	if(setup.wIndex.c[LSB] & 0x80)
        	{
        		ep_index = setup.wIndex.c[LSB] & 0x7f;

				if(ep_index < EP_MAX_INDEX)
				{
					ep_ptr = usb_dev.ep_esse[ep_index];

					if(ep_ptr)
					{
						udev_ep_put_on_halt(ep_ptr);
					}
				}
        	}
        }
        else
        {
            udev_ep_put_on_halt(ctrl_ep_ptr);
        }
    }
	
    if(EP_STALL != ctrl_ep_ptr->status)
    {
        _ctrl_ep_tx_zero_packet(&usb_dev);
    }
}

/*! \fn void driver_usb_set_address(void)
    \brief  This routine can set new function address. Request must be directed to device
    \param void
    \return void
*/
void driver_usb_set_address(EP_ENTITY_PTR ctrl_ep_ptr)
{
	UDEV_PTR udev_ptr = &usb_dev;
	
	UDEV_PRT("driver_usb_set_address\r\n");
    if((setup.bmRequestType != IN_DEVICE) 
		|| setup.wIndex.c[MSB]  
		|| setup.wIndex.c[LSB]
		|| setup.wLength.c[MSB] 
		|| setup.wLength.c[LSB]
		|| setup.wValue.c[MSB]  
		|| (setup.wValue.c[LSB] & 0x80))
    {
        udev_ep_put_on_halt(ctrl_ep_ptr);    // Send stall if setup data invalid
    }
	
    if(setup.wValue.c[LSB] != 0)
    {
    	udev_set_status(udev_ptr, DEV_ADDRESS);// Indicate that device state is now address
    }
    else
    {
    	udev_set_status(udev_ptr, DEV_DEFAULT);// If new address was 0x00, return device to default state
    }
        
    if(EP_STALL != ctrl_ep_ptr->status)
    {
        _ctrl_ep_tx_zero_packet_set_address(udev_ptr);
    }
}

/*! \fn void driver_usb_get_descriptor(void)
    \brief  This routine sets the data pointer and size to correct descriptor, and sets the endpoint state to transmit.
    \param void
    \return void
*/
void driver_usb_get_descriptor(EP_ENTITY_PTR ctrl_ep_ptr)
{
	uint8 *ptr;
	int32 len = 0;
	
	//UDEV_PRT("driver_usb_get_descriptor\r\n");
    switch(setup.wValue.c[MSB])
    {
        case DSC_DEVICE:
            len = 18;
            ptr = usb_device_descriptor;
			UDEV_PRT("DSC_DEVICE\r\n");
            break;
            
        case DSC_CONFIG:
			UDEV_PRT("DSC_CONFIG\r\n");
            ptr =  usb_configuration_descriptor;
            if(setup.wLength.i > CONFIG_DESC_SIZE)
                len = CONFIG_DESC_SIZE;
            else
                len = setup.wLength.i;
            break;
            
        case DSC_INTERFACE:
			UDEV_PRT("DSC_INTERFACE\r\n");
            break;
            
        case DSC_HID:                            // HID Specific (HID class descriptor)
			UDEV_PRT("DSC_HID\r\n");
            break;
            
        case DSC_HID_REPORT:    
			UDEV_PRT("DSC_HID_REPORT\r\n");
            break;
            
        case DSC_STRING:
            if(setup.wValue.c[LSB] <= (sizeof(usb_string_table) / sizeof(usb_string_table[0])))
            {
                ptr = usb_string_table[setup.wValue.c[LSB]];
                len = ptr[0];
            }
            else
            {
                udev_ep_put_on_halt(ctrl_ep_ptr);
            }			
			UDEV_PRT("DSC_STRING:%d\r\n", len);
    	break;
        
    default:
        udev_ep_put_on_halt(ctrl_ep_ptr);               // Send Stall if unsupported request
        break;
    }
    
    if((len) && EP_STALL != ctrl_ep_ptr->status)            // Make sure endpoint not in stall mode
    {
    	udev_async_send(&usb_dev, EP_0, ptr, len, _ctrl_ep_tx_cb);
    }
}

/*! \fn void driver_usb_get_configuration(void)
    \brief  This routine returns current configuration value.
    \param void
    \return void
*/
void driver_usb_get_configuration (EP_ENTITY_PTR ctrl_ep_ptr)
{
	uint8 ptr[2] = {0, 0};
	int32 len = 0;
	
	UDEV_PRT("driver_usb_get_configuration\r\n");
    // This request must be directed to the device // With value word set to zero // And index set to zero // And setup length set to one
    if((setup.bmRequestType != OUT_DEVICE) 
		|| setup.wValue.c[MSB] 
		|| setup.wValue.c[LSB] 
		|| setup.wIndex.c[MSB]  
		|| setup.wIndex.c[LSB]
		|| setup.wLength.c[MSB] 
		|| (setup.wLength.c[LSB] != 1))
    {
        udev_ep_put_on_halt(ctrl_ep_ptr);                        // send a stall to host
    }
    else
    {
        if(usb_dev.state == DEV_CONFIGURED)
        {
        	ptr[0] = 1;
            len = 1;
        }
        if(usb_dev.state == DEV_ADDRESS)
        {
            len = 1;
        }
    }
	
    if((len) && (EP_STALL != ctrl_ep_ptr->status))
    {
    	udev_async_send(&usb_dev, EP_0, ptr, len, _ctrl_ep_tx_cb);
    }
}
/*! \fn void driver_usb_set_configuration(void)
    \brief  This routine allows host to change current device configuration value.
    \param void
    \return void
*/
void driver_usb_set_configuration (EP_ENTITY_PTR ctrl_ep_ptr)
{
	int i;
	UDEV_PTR udev_ptr = &usb_dev;
	
	UDEV_PRT("driver_usb_set_configuration\r\n");
    // Device must be addressed before configured //and request recipient must be the device // the index and length words must be zero// This software only supports config = 0,1
    if((udev_ptr->state == DEV_DEFAULT) 
		|| (setup.bmRequestType != IN_DEVICE) 
		|| setup.wIndex.c[MSB]  
		|| setup.wIndex.c[LSB]
		|| setup.wLength.c[MSB] 
		|| setup.wLength.c[LSB] 
		|| setup.wValue.c[MSB]  
		|| (setup.wValue.c[LSB] > 1))
    {
        udev_ep_put_on_halt(ctrl_ep_ptr);                        // Send stall if setup data is invalid
    }
    else
    {
        if(setup.wValue.c[LSB] > 0)                // Any positive configuration request results in configuration being set to 1
        {
			udev_set_status(udev_ptr, DEV_CONFIGURED);
			for(i = 1; i < EP_MAX_INDEX; i ++)
			{
				if(udev_ptr->ep_esse[i])
				{
					udev_ptr->ep_esse[i]->status = EP_IDLE;
				}
			}
        }
        else
        {
        	// Unconfigured device by setting state to address, and changing endpoint
        	udev_set_status(udev_ptr, DEV_ADDRESS);
			for(i = 1; i < EP_MAX_INDEX; i ++)
			{
				if(udev_ptr->ep_esse[i])
				{
					udev_ptr->ep_esse[i]->status = EP_STALL;
				}
			}
        }
    }
	
    if(EP_STALL != ctrl_ep_ptr->status)
    {
    	_ctrl_ep_tx_zero_packet(udev_ptr);
    }
}

/*! \fn void driver_usb_get_interface(void)
    \brief  This routine returns 0x00, since no alternate interface is supported by this firmware.
    \param void
    \return void
*/
void driver_usb_get_interface(EP_ENTITY_PTR ctrl_ep_ptr)
{
	uint8 *ptr;
	int32 len;
	
	UDEV_PRT("driver_usb_get_interface\r\n");

    if(setup.bmRequestType != OUT_INTERFACE)
    {
        udev_ep_put_on_halt(ctrl_ep_ptr); 
    }
    else
    {
    	ptr = USB_IF_ALT;
    	len = MIN(sizeof(u_int8), setup.wLength.i);
		
		udev_async_send(&usb_dev, EP_0, ptr, len, _ctrl_ep_tx_cb);
    }
}

/*! \fn void driver_usb_set_interface(void)
    \brief  This routine sets the interface. Make sure request is directed at interface and all other packet values are set to zero.
    \param void
    \return void
*/
void driver_usb_set_interface (EP_ENTITY_PTR ctrl_ep_ptr)
{
	UDEV_PRT("driver_usb_set_interface\r\n");
	USB_IF_ALT[setup.wIndex.i & 0x00FF] = (setup.wValue.i & 0x00FF);
	
    if(EP_STALL != ctrl_ep_ptr->status)
	{
    	_ctrl_ep_tx_zero_packet(&usb_dev);
	}
}

#ifdef HID_CLASS
/*! \fn void driver_usb_get_idle(void)
    \brief  This routine checks whether usb is in idle state.
    \param void
    \return void
*/
void driver_usb_get_idle(void)
{
	UDEV_PRT("driver_usb_get_idle\r\n");

}

/*! \fn void driver_usb_set_idle(void)
    \brief  This routine sets the idle feature by interrupt in endpoint.
    \param void
    \return void
*/
void driver_usb_set_idle (void)
{
	UDEV_PRT("driver_usb_set_idle\r\n");

}

/*! \fn void driver_usb_get_protocol(void)
    \brief  This routine returns the usb protocol.
    \param void
    \return void
*/
void driver_usb_get_protocol(void)
{

	UDEV_PRT("driver_usb_get_protocol\r\n");
}

/*! \fn void driver_usb_set_protocol(void)
    \brief  This routine sets the usb protocol.
    \param void
    \return void
*/
void driver_usb_set_protocol (void)
{
	UDEV_PRT("driver_usb_set_protocol\r\n");

}

/*! \fn void driver_usb_get_report(void)
    \brief  This routine sends a given report type to the host.
    \param void
    \return void
*/
void driver_usb_get_report (void)
{
	UDEV_PRT("driver_usb_get_report\r\n");

}

/*! \fn void driver_usb_set_report(void)
    \brief  This routine receives a report sent from the host.
    \param void
    \return void
*/
void driver_usb_set_report (void)
{
	UDEV_PRT("driver_usb_set_report\r\n");
}

/*! \fn void driver_usb_set_interface_current(void)
    \brief
    \param void
    \return void
*/
void driver_usb_set_interface_current(void)
{
	UDEV_PRT("driver_usb_set_interface_current\r\n");
}

/*! \fn void driver_usb_get_interface_current(void)
    \brief
    \param void
    \return void
*/
void driver_usb_get_interface_current(void)
{
	UDEV_PRT("driver_usb_get_interface_current\r\n");
}

/*! \fn void driver_usb_get_interface_min(void)
    \brief
    \param void
    \return void
*/
void driver_usb_get_interface_min(void)
{
	UDEV_PRT("driver_usb_get_interface_min\r\n");
}

/*! \fn void driver_usb_get_interface_max(void)
    \brief
    \param void
    \return void
*/
void driver_usb_get_interface_max(void)
{
	UDEV_PRT("driver_usb_get_interface_max\r\n");
}

/*! \fn void driver_usb_get_interface_resolution(void)
    \brief
    
    \param void
    \return void
*/
void driver_usb_get_interface_resolution(void)
{
	UDEV_PRT("driver_usb_get_interface_resolution\r\n");
}

/*! \fn 
    \brief
    
    \param void
    \return void
*/
void driver_usb_set_endpoint_cur(void)
{
	UDEV_PRT("driver_usb_set_endpoint_cur\r\n");
}

/*! \fn 
    \brief
    
    \param void
    \return void
*/
void driver_usb_get_endpoint_cur(void)
{
	UDEV_PRT("driver_usb_get_endpoint_cur\r\n");
}
#endif // HID_CLASS
#endif
/**end file*/
