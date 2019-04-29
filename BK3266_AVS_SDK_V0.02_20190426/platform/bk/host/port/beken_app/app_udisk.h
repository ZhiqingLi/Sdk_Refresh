#ifndef __APP_UDISK_H__
#define __APP_UDISK_H__

//#define DIRECT_COPY_BTWN_SD_EP_FIFO

//#define UDISK_DG
//#define USE_DMA_

#include "usb.h"
#include "doubly_list.h"

#ifdef UDISK_DG
	#define UDISK_PRT                    
#else
	#define UDISK_PRT
#endif

#define  USB_DCBWSIGNATURE              (0x43425355)
#define  USB_DCSWSIGNATURE              (0x53425355)

#define  UDISK_CBW_LEN                  (31)
#define  UDISK_CSW_LEN                  (13)

#define  USB_CBW_DIRECTION_BIT          (0x80)

/*bk3260: all usb memory space for endpoints hardware cache is 1024 Bytes
		  pay attention to the total size and each endpoint's memory 
		  boundary. No overlay, No overflow, thx
*/
#define  BULK_OUT_FIFO_OFFSET           (0x100)
#define  BULK_IN_FIFO_OFFSET            (0x100)

#define BULK_OUT_CELL_COUNT             (8)     //Shall be the power of 2   
#define BULK_IN_CELL_COUNT              (8)     // the power of 2

#define UDISK_BUFFER_SIZE               (512)

#define  APP_CONTROL_MAX_PKT_SIZE       (64)
#define  DEV_DESC_MAX_PACKET_SIZE       (7)
#define  CFG_DESC_EP1TX_MAX_PACKET_SIZE (22)
#define  CFG_DESC_EP1RX_MAX_PACKET_SIZE (29)
 
#define  TOTAL_LOGICAL_ADDRESS_BLOCKS   (400)
#define  LENGTH_OF_EACH_LAB             (512)

#define USB_MEM32_ALIGN(n)              ((n) + (-(n) & 31))

typedef uint8 * uint8_ptr;
typedef unsigned char UCHAR, *uchar_ptr;  

/* USB Command Block Wrapper */
typedef struct cbw_struct {
   uint32   DCBWSIGNATURE;
   uint32   DCBWTAG;
   uint32   DCBWDATALENGTH;
   UCHAR    BMCBWFLAGS;
   /* 4 MSBs bits reserved */
   UCHAR    BCBWCBLUN;
   /* 3 MSB reserved */
   UCHAR    BCBWCBLENGTH;
   UCHAR    CBWCB[16];
} CBW_STRUCT, * CBW_STRUCT_PTR;

/* USB Command Status Wrapper */
typedef  struct csw_struct {
   uint32  DCSWSIGNATURE;
   uint32  DCSWTAG;
   uint32  DCSWDATARESIDUE;
   UCHAR   BCSWSTATUS;
} CSW_STRUCT, * CSW_STRUCT_PTR;

/* USB Mass storage Inquiry Command */
typedef struct mass_storage_inquiry {
   UCHAR    OPCODE;
   UCHAR    LUN;
   UCHAR    PAGE_CODE;
   UCHAR    RESERVED1;
   UCHAR    ALLOCATION_LENGTH;
   UCHAR    RESERVED2[7];
} MASS_STORAGE_INQUIRY_STRUCT, * MSC_INQUIRY_PTR;

/* USB Mass storage READ CAPACITY Data */
typedef struct mass_storage_read_capacity {
   UCHAR    LAST_LOGICAL_BLOCK_ADDRESS[4];
   UCHAR    BLOCK_LENGTH_IN_BYTES[4];
} MSC_READ_CAPACITY_STRUCT, * MSC_READ_CAPACITY_PTR;

/* USB Mass storage Device information */
typedef struct mass_storage_device_info {
   UCHAR    PERIPHERAL_DEVICE_TYPE;    /* Bits 0-4. All other bits reserved */
   UCHAR    RMB;                       /* Bit 7. All other bits reserved */
   UCHAR    ANSI_ECMA_ISO_VERSION;     /* ANSI: bits 0-2, ECMA: bits 3-5, 
                                       ** ISO: bits 6-7 
                                       */
   UCHAR    RESPONSE_DATA_FORMAT;      /* bits 0-3. All other bits reserved */
   UCHAR    ADDITIONAL_LENGTH;         /* For UFI device: always set to 0x1F */
   UCHAR    RESERVED1[3];
   UCHAR    VENDOR_INFORMATION[8];
   UCHAR    PRODUCT_ID[16];
   UCHAR    PRODUCT_REVISION_LEVEL[4];
} MSC_DEVICE_INFO_STRUCT, * MASS_STORAGE_DEVICE_INFO_PTR;

typedef struct _udisk_dev_
{
	uint32 media_exist_flag;	
	uint32 cbw_processed;
	uint32 zero_terminate;
	
	MSC_READ_CAPACITY_STRUCT read_capacity;
	MSC_DEVICE_INFO_STRUCT device_info_data;
	
	CSW_STRUCT csw;
	CBW_STRUCT cbw;
	
	UDEV_PTR udev_pointer;

	uint32 block_size;
	uint32 block_count;	
}UDISK_T, *UDISK_PTR;

/*backup
		BK3000_GPIO_OTDET bit1-bit0 == 1: usb plug detection
		BK3000_PMU_PERI_PWDS bit_PMU_USB_PWD: usb clock enable control
*/
extern void udisk_cbw_swi(void *param, uint32 param2);
extern void udisk_tx_callback(void *param, uint32 count);
extern void udisk_rx_callback(void *param, uint32 count);
extern void app_udisk_swi_handler(void);
extern void app_udisk_install(void *p);
extern uint32 udisk_get_pseudo_swi(void);
extern void msc_ep0_mass_storage_reset(EP_ENTITY_PTR ctrl_ep_ptr, setup_buffer *setup_ptr);
extern void msc_ep0_get_max_lun (EP_ENTITY_PTR ctrl_ep_ptr, setup_buffer *setup_ptr);


#ifndef DIRECT_COPY_BTWN_SD_EP_FIFO
#define NODE_COUNT                   (32)

#define DMA_USB_CHANNEL_ID           (0)
#define DMA_SD_CHANNEL_ID            (0)

#define OCCUPY_NODE_STATUS           (1)
#define AVAILABLE_NODE_STATUS        (0)

typedef struct _node_buf_
{
	uint8 *buf_ptr;
#ifdef 	UDISK_DG
	volatile uint32 node_id;
#endif
	
	LIST_HEADER_T node_list;
}NODE_BUF_T;

typedef struct _use_head_
{
	volatile uint32 count;
	
	LIST_HEADER_T use_list;
}USE_LIST_HEAD_T;

#define ACCESS_CMD_NULL                     (0x00)
#define ACCESS_CMD_READ                     (0x21)
#define ACCESS_CMD_WRITE                    (0x32)

typedef struct _request_info_
{
	volatile uint32 access_cmd_type;	
	
	uint32 start_blk;
	uint32 blk_num;
	
	volatile uint32 current_blk_id;
	volatile uint32 produce_blk_num;
	volatile uint32 consume_blk_num;
	
	NODE_BUF_T *produce_node_ptr;
	NODE_BUF_T *consume_node_ptr;
}REQ_INFO_T;

typedef void (*PRODUCE_FUNC)(void);
typedef uint32 (*CONSUME_FUNC)(void);

#endif

#endif
/* EOF */
