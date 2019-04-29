#ifndef _USB_DEVICE_H_
#define _USB_DEVICE_H_

#define UDISK_DEBUG
//#define USB_DEBUG

//#define UDEV_MEM_CHECK

#ifdef USB_DEBUG
	#define USB_FATAL()       do{                               \
								    os_printf("USB_FATAL\r\n"); \
									while(1);                   \
								 }while(0)
	#define UDEV_PRT          
#else 
	#define USB_FATAL()              
	#define UDEV_PRT                 
#endif

#define UDEV_DMEM_SPACE_FOR_HW_LEN              (1024)
#define EP_0_CELL_COUNT                         (2)

#define EP_MAX_INDEX                            (7)

#define EP_ENABLE_FLAG                          (1)
#define CTRL_EP_FIFO_OFFSET_ADDR                (0x0000)

#define URB_SENDING                             (0x12)
#define URB_RECEIVING                           (0x23)
#define URB_SEND_OVER                           (0x34)
#define URB_RECEIVE_OVER                        (0x45)
#define URB_EXCEPTIONAL                         (0x56)
#define URB_SEND_SUSPEND                        (0x67)
#define URB_RECEIVE_SUSPEND                     (0x78)

#define URB_ASYNC                               (0)
#define URB_SYNC                                (1)

typedef void (*callback_func)(void *, uint32);

typedef struct _EP_
{
	uint32 status;
	uint32 index;
	uint32 mps;                // MPS: maximum packet size
	uint32 dir;
	
	uint32 dmem_offset;
	uint32 dmem_cell_size;
	uint32 dmem_cell_count;
	volatile uint32 cell_use_id;
	volatile uint32 cell_free_id;
	
	uint8 *data_ptr;
	volatile callback_func cb;
	uint32 data_req_len;
	uint32 data_actual_len;
	
	int32 sync_flag;
	
	volatile int32 urb_status;
}EP_ENTITY, *EP_ENTITY_PTR;

typedef struct _udev_fifo_
{
	uint32 total_size;
	int32 remanent;
}UDEV_DMEM_T;

typedef struct _udev_
{
	volatile int32 state;

	UDEV_DMEM_T udev_dmem;
	
	EP_ENTITY_PTR ep_esse[EP_MAX_INDEX + 1];
	
#ifdef USB_DEBUG
	int32 usb_intr_data_receive_cnt;
	int32 usb_intr_data_send_cnt;
	int32 usb_intr_reset_cnt;
	int32 usb_intr_suspend_cnt;
	int32 usb_intr_resume_cnt;
	int32 usb_intr_unsupported_token_cnt;
	int32 usb_intr_crc_error_cnt;
	int32 usb_intr_overtime_cnt;
	int32 usb_intr_data_pid_error_cnt;

	int32 usb_intr_data_control_timestamp;
	int32 usb_intr_data_receive_timestamp;
	int32 usb_intr_data_send_timestamp;
	int32 usb_intr_reset_timestamp;
	int32 usb_intr_suspend_timestamp;
	int32 usb_intr_resume_timestamp;
	int32 usb_intr_unsupported_token_timestamp;
	int32 usb_intr_crc_error_timestamp;
	int32 usb_intr_overtime_timestamp;
	int32 usb_intr_data_pid_error_timestamp;
#endif
}UDEV, *UDEV_PTR;

extern void udev_ep_put_on_halt(EP_ENTITY_PTR ep_ptr);
extern void udev_ep_put_off_halt(EP_ENTITY_PTR ep_ptr);
extern void udev_clear_hw_address(void);
extern void udev_update_hw_address(uint32 addr);
extern void udev_put_on_remote_wakeup(void);
extern void udev_put_off_remote_wakeup(void);
extern void udev_put_on_suspend(void);
extern void udev_put_off_suspend(void);
extern void udev_put_on_reset(void);
extern void udev_put_off_reset(void);
extern void udev_pullup_enable(void);
extern void udev_pullup_disable(void);
extern void udev_peripheral_clock_enable(void);
extern void udev_peripheral_clock_disable(void);
extern void udev_enable_usb_isr_branches(void);
extern UDEV_PTR udev_init(void);
extern void udev_uninit(void);
extern void udev_set_status(UDEV_PTR udev_ptr, int status);
extern void udev_init_endpoint(uint8 ep_index, 
	uint8 dir, 
	uint8 mode, 
	uint8 mps, 
	uint8 enable_flag,
	uint16 fifo_addr_offset,
	uint32 cell_count);
extern void udev_enter_suspend(UDEV_PTR udev_ptr);
extern void udev_exit_suspend(UDEV_PTR udev_ptr);
extern uint32 udev_read_fifo(EP_ENTITY_PTR ep_ptr, u_int16 len, u_int8 *pointer, uint32 offset);
extern void udev_enable_receive(EP_ENTITY_PTR ep_ptr);
extern uint32 udev_write_fifo(EP_ENTITY_PTR ep_ptr, u_int16 len, u_int8 *pointer);
extern void udev_enable_send(EP_ENTITY_PTR ep_ptr);
extern void udev_update_ep_fifo_config(EP_ENTITY_PTR ep_ptr, int32 blk_id);
extern void udev_tx_callback(void *param, uint32 count);
extern void udev_rx_callback(void *param, uint32 count);
extern void set_address_cb(void *param, uint32 count);
extern void _ctrl_ep_tx_zero_packet_set_address(UDEV_PTR udev_ptr);
extern void _ctrl_ep_tx_zero_packet(UDEV_PTR udev_ptr);
extern void _ctrl_ep_rx_zero_packet(UDEV_PTR udev_ptr);
extern void _ctrl_ep_tx_cb(void *param, uint32 count);
extern void _ctrl_ep_rx_cb(void *param, uint32 count);
extern uint32 udev_async_send(
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
   );
extern uint32 udev_sync_send(
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
   );
extern uint32 udev_async_receive(
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
   );
extern uint32 udev_sync_receive(
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
   );
extern uint32 udev_get_suspend_status(void);
extern UDEV usb_dev;

#endif
