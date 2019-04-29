#ifndef _DRIVER_FLASH_H
#define _DRIVER_FLASH_H

#ifndef BEKEN_UP_OTA_3266
extern uint32 FLASH_ENVDATA_DEF_ADDR;
#endif
//#define MEMORIZE_INTO_FLASH

#define FLASH_SPEED_QUAD_READ                   1
#define FLASH_SPEED_STANDARD_READ               0
#define FLASH_SR_SEL_2BYTE                      0
#define FLASH_SR_SEL_1BYTE                      1
#define FLASH_FCKDIV_PLL_90M                    ( 0x0 << 2 )
#define FLASH_FCKDIV_PLL_120M                   ( 0x1 << 2 )
#define FLASH_FCKDIV_XTAL                       ( 0x2 << 2 )
#define FLASH_FCKDIV_DIV1                       0
#define FLASH_FCKDIV_DIV2                       1
#define FLASH_FCKDIV_DIV4                       2
#define FLASH_FCKDIV_DIV8                       3
#if (CONFIG_APP_MP3PLAYER == 1)
//default vol is 8,  but 12 for linein becasue 8 is lower
#define  PLAYER_VOL_MEDIUM          8
#define  PLAYER_VOL_MEDIUM_LINEIN   12
#define  PLAYER_VOL_MEDIUM_HFP      13
#endif

/// flash operation command type(decimal)
enum {
	FLASH_OPCODE_WREN    = 1,
	FLASH_OPCODE_WRDI    = 2,
	FLASH_OPCODE_RDSR    = 3,
	FLASH_OPCODE_WRSR    = 4,
	FLASH_OPCODE_READ    = 5,
	FLASH_OPCODE_RDSR2   = 6,
	FLASH_OPCODE_WRSR2   = 7,
	FLASH_OPCODE_PP      = 12,
	FLASH_OPCODE_SE      = 13,
	FLASH_OPCODE_BE1     = 14,
	FLASH_OPCODE_BE2     = 15,
	FLASH_OPCODE_CE      = 16,
	FLASH_OPCODE_DP      = 17,
	FLASH_OPCODE_RFDP    = 18,
	FLASH_OPCODE_RDID    = 20,
	FLASH_OPCODE_HPM     = 22,
	FLASH_OPCODE_CRMR    = 22,
	FLASH_OPCODE_CRMR2   = 23,
} FLASH_OPCODE;

#ifdef MEMORIZE_INTO_FLASH
/*suppose the size of backup info is 127 Bytes or less*/

#define FLASH_LAST_SECTOR_ADDRESS              (FLASH_LAST_BLOCK_NUM*0x10000-0x1000)
#define END_OF_INFO_SECTOR                     (4096/128)
#define MAX_STATION                            30
#define FILE_BYTE_NUM                          16   //(20+(MAX_DIR_DEPTH+1)*4)//断点保存字节数  48byte

#define FLASH_4MBIT     					  0

//#define FLASH_TYPE_ZETTA					  0xF2	 // zetta flash
//#define FLASH_TYPE_BOYA_GD					  0xF1   // boya  flash

//#define FLASH_TYPE_DEF						  FLASH_TYPE_ZETTA	
#if (CONFIG_APP_MP3PLAYER == 1)
/*BYTE ADDRESS*/
#define BT_VOLUME_POS                         1
#define HFP_VOLUME_POS                        2
#define MP3_VOLUME_POS                        3
#define FM_VOLUME_POS                         4
#define LINEIN_VOLUME_POS                     5

//16byte
#define SONG_INDEX_POS                        8

//保留8个字节
#define CUR_STATION_POS                       (SONG_INDEX_POS+FILE_BYTE_NUM+8) // 8+16+8 = 32
#define TOTAL_STATION_POS                     (CUR_STATION_POS+1)// 33
#define STATION_ARRAY_POS                     (TOTAL_STATION_POS+1)// 34

typedef enum{
    VOL_INFO_BT,
    VOL_INFO_HFP,
    VOL_INFO_MUSIC,
    VOL_INFO_LINEIN,
    SONGIDX_INFO,
    CUR_STATION_INFO,
    ALL_STATION_INFO
} flash_info_t;
#endif

#endif

void flash_init(void);
uint32 flash_read_mID(void);
void save_volume_task(void *arg);
void set_flash_ctrl_config(void);
void set_flash_clk(unsigned char clk_conf);
void set_flash_qe(void);
uint8 flash_get_4M_flag(void);
void flash_set_line_mode(uint8 mode);
uint8 flash_get_line_mode(void);
void flash_memcpy(uint8 *dst,uint8 *src,uint32 len,boolean first);
void flash_read_data (uint8 *buffer, uint32 address, uint32 len);
void flash_write_data (uint8 *buffer, uint32 address, uint32 len);
void flash_crc_remove(uint32 address);
void flash_erase_sector(uint32 address);
void flash_config(void);
#ifdef BEKEN_UP_OTA_3266
void flash_erase_half(uint8 half);
#endif
//void app_write_info(flash_info_t type,void * ptr1,void *ptr2,void *ptr3);
//uint8 app_read_info(flash_info_t type,void * ptr1,void *ptr2,void *ptr3);
#endif
