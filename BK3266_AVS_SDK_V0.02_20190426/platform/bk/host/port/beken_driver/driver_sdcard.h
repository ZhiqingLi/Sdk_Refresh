#ifndef __DRIVER_SDCARD_H
#define __DRIVER_SDCARD_H

#ifdef CONFIG_BK_QFN56_DEMO
#define SD_DETECT_DEFAULT_GPIO                  12 //18
#else
#define SD_DETECT_DEFAULT_GPIO                  18
#endif
#define SD_DEBOUNCE_COUNT               80

/* #define SD_DEBUG */
#ifdef SD_DEBUG
    #define SD_PRT      os_printf
#else
    #define SD_PRT      os_null_printf
#endif

#define SD_CMD_FLAG_RESPONSE    0x01
#define SD_CMD_FLAG_LONG_CMD    0x02
#define SD_CMD_FLAG_CRC_CHECK   0x04

#define SD_CMD_SHORT    SD_CMD_FLAG_RESPONSE|SD_CMD_FLAG_CRC_CHECK
#define SD_CMD_LONG     SD_CMD_FLAG_RESPONSE|SD_CMD_FLAG_LONG_CMD|SD_CMD_FLAG_CRC_CHECK

#define SD_CMD_FLAG_SHIFT_BIT   1
#define SD_CMD_INDEX_SHIFT_BIT  4

#define  bit_SD_INT_NORSP_END               0x01
#define  bit_SD_INT_RSP_END                 0x02
#define  bit_SD_INT_RSP_TIMEOUT             0x04
#define  bit_SD_INT_DATA_REC_END            0x08
#define  bit_SD_INT_DATA_WR_END             0x10
#define  bit_SD_INT_TIME_OUT_INT            0x20
#define  bit_SD_INT_RX_FIFO_NEED_READ       0x40
#define  bit_SD_INT_TX_FIFO_NEED_WRITE      0x80
#define  bit_SD_INT_RX_OV                   0x100
#define  bit_SD_INT_TX_FIFO_EMPT            0x200
#define  bit_SD_INT_CMD_CRC_OK              0x400
#define  bit_SD_INT_CMD_CRC_FAIL            0x800
#define  bit_SD_INT_DATA_CRC_OK             0x1000
#define  bit_SD_INT_DATA_CRC_FAIL           0x2000
#define  bit_SD_INT_DATA_BUSY               0x1000000

#define bits_SD_CMD_RSP     ( bit_SD_INT_NORSP_END\
							 |bit_SD_INT_RSP_END\
							 |bit_SD_INT_RSP_TIMEOUT )
							 
#define bits_SD_DATA_RSP    ( bit_SD_INT_DATA_REC_END\
							 |bit_SD_INT_DATA_CRC_FAIL\
							 |bit_SD_INT_TIME_OUT_INT )
								 
#define SD_DEFAULT_OCR                     (0xff8000|0x40000000)

#define OCR_MSK_BUSY                        0x80000000 // Busy flag
#define OCR_MSK_HC                          0x40000000 // High Capacity flag
#define OCR_MSK_VOLTAGE_3_2V_3_3V           0x00100000 // Voltage 3.2V to 3.3V flag
#define OCR_MSK_VOLTAGE_ALL                 0x00FF8000 // All Voltage flag

#define DEFAULT_CMD_TIME_OUT                0x300000
#define DEFAULT_DATA_TIME_OUT               0x300000


#define CLK_ENABLE	(BK3000_PMU_PERI_PWDS &= ~bit_PMU_SDIO_PWD)
#define	CLK_DISABLE (BK3000_PMU_PERI_PWDS |=  bit_PMU_SDIO_PWD)

#define	CLK_13M                             0
#define	CLK_6_5M                            1
#define	CLK_3_25M                           2
#define	CLK_1_625M                          3

typedef void (*SD_DETECT_FUN)(void);
enum
{
    SD_ERR_NONE = 0,
    SD_ERR_CMD_TIMEOUT = 1,
    SD_ERR_CMD_CRC_FAIL = 2,
    SD_ERR_DATA_TIMEOUT = 3,
    SD_ERR_DATA_CRC_FAIL = 4,
    SD_ERR_LONG_TIME_NO_RESPONS = 5,
    SD_ERR_CMD41_CNT = 0xfffe
};

typedef struct __driver_sdcard_s
{
    uint32  total_block;
    uint16  block_size;
    uint16  card_rca;
    uint16  init_flag;
	uint16	Addr_shift_bit;
	SD_DETECT_FUN detect_func;
}driver_sdcard_t;

void sdcard_uninit( void );
int sdcard_get_block_size( void );
int sdcard_get_size( void );
int driver_sdcard_get_init_status( void );
int driver_sdcard_set_init_status(uint16 val);

void app_sd_scanning(void);
void app_sd_init(void);
uint8 sd_is_attached(void);
int sd_rd_blk_sync( int first_block, int block_num, uint8 *dest );
int SD_init(void);
void sdcard_idle(char enable);
void clr_sd_noinitial_flag(void);
#endif
