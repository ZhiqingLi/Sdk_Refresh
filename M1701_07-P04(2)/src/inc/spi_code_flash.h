/**
 *****************************************************************************
 * @file     spi_code_flash.h
 * @author   Richard
 * @version  V1.0.6
 * @date     30-May-2013
 * @brief    spi code flash module driver interface
 *****************************************************************************
 * @attention 
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */
 
#ifndef __SPI_CODE_FLASH_H__
#define __SPI_CODE_FLASH_H__

//#include "OrioleReg.h"

#define     POSEDGE_SEL          (0)       /**<sampling at clk posedge */
#define     NEGEDGE_SEL          (1)       /**<sampling at clk negedge */
#define     DELAY_CHAIN_SEL      (4)       /**<sampling at the n clks */

#define     FLASH_GD             (0xC8)    /**<MID */
#define     FLASH_MX             (0xC2)    /**<MID */
#define     FLASH_WINBOUND       (0xEF)    /**<MID */
#define     FLASH_PCT            (0xBF)    /**<MID */
#define     FLASH_EON            (0x1C)    /**<MID */
#define     FLASH_BG             (0xE0)    /**<MID */
#define     FLASH_ESMT           (0x8C)    /**<MID */
#define			FLASH_FT			 (0x0E)    /**<MID */

#define     FLASH_HALF_PROTECT              (1)
#define     FLASH_THREE_QUARTER_PROTECT     (2)
#define     FLASH_SEVEN_EIGHTH_PROTECT      (3)
#define     FLASH_ALL_PROTECT               (4)

#define 		FLASH_GD_VERSION_A		(0)
#define 		FLASH_GD_VERSION_B		(1)
#define 		FLASH_GD_VERSION_C		(2)

/**
 * err code definition
 */

#pragma pack(1)

typedef enum {
	CHIP_ERASE,
	SECTOR_ERASE,
	BLOCK_ERASE
}ERASE_TYPE_ENUM;

/**
 * flash structure define 
 * each byte definition like:    A B CMP SEC/BP4 TB/BP3 BP2 BP1 BP0 <A:area protection support flag,B:CMP bit exist flag>
 */
typedef struct _SPI_CODE_FLASH_INFO
{
    __IO  uint8_t      Mid;             /**<Manufacturer's ID*/    
    __IO  uint16_t      Did;             /**<Device ID*/
    __IO  uint8_t      SqiModeFlag;     /**<bit0:0:spi mode, 1:sqi mode. r/w*/   
    __IO  uint8_t      SqiWriteFlag;    /**<bit1:1 support 4bit page programm, 0 not support. read only*/ 
    __IO  uint32_t     Capacity;        /**<flash capacity*/
    __IO  uint8_t      NoneAreaProtect;
    __IO  uint8_t      HalfAreaProtect;
    __IO  uint8_t      ThreeQuarterAreaProtect; 
    __IO  uint8_t      SevenEighthsAreaProtect;
    __IO  uint8_t      AllAreaProtect; 
//		__IO  uint8_t			GD_version;

}SPI_CODE_FLASH_INFO;

#pragma pack()


#endif  //__SPI_CODE_FLASH_H__

