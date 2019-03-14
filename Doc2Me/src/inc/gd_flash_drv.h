/**
 *****************************************************************************
 * NAME:
 *	gd_flash_ctrl.h
 *
 * DESCRIPTION:
 *	The functions used only for GD flash
 *****************************************************************************
 */
#include "spi_code_flash.h"
#include "spi_flash.h"

#ifdef FLASH_ERASE_IN_TCM

typedef enum{
	HALF_OF_AREA = 1,			// 1/2
	THREE_QUARTERS_OF_AREA,		// 3/4
	SEVENTH_EIGHT_OF_AREA,		// 7/8
	ALL_OF_AREA
} PROTECT_AREA;

int FlashEraseInTCM(SPI_FLASH_INFO* FlashInfo,uint32_t Offset,uint32_t Size);

__attribute__((section("CACHE.4KBMEM")))
int32_t SpiFlashEraseInTcm(uint32_t Offset, uint32_t Size);

#endif

