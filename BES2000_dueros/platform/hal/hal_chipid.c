#include "hal_chipid.h"
#include "hal_location.h"

enum HAL_CHIP_METAL_ID_T BOOT_BSS_LOC metal_id;

uint32_t WEAK BOOT_TEXT_FLASH_LOC read_hw_metal_id(void)
{
    return HAL_CHIP_METAL_ID_0;
}

void BOOT_TEXT_FLASH_LOC hal_chipid_init(void)
{
    metal_id = read_hw_metal_id();
}

enum HAL_CHIP_METAL_ID_T BOOT_TEXT_SRAM_LOC hal_get_chip_metal_id(void)
{
    return metal_id;
}

