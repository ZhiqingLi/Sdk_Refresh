#ifndef __HAL_LOCATION_H__
#define __HAL_LOCATION_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) && !(defined(ROM_BUILD) || defined(PROGRAMMER))

#define HAL_SEC_DEF_A(s, d, n)          __attribute__((section(#s #d #n))) n
#define HAL_SEC_DEF(section, name)      HAL_SEC_DEF_A(section, ., name)
#define HAL_SEC_LOC(section)            HAL_SEC_DEF_A(section, , )

#define BOOT_TEXT_SRAM_LOC              HAL_SEC_LOC(.boot_text_sram)
#define BOOT_TEXT_SRAM_DEF(n)           HAL_SEC_DEF(.boot_text_sram, n)
#define BOOT_TEXT_FLASH_LOC             HAL_SEC_LOC(.boot_text_flash)
#define BOOT_TEXT_FLASH_DEF(n)          HAL_SEC_DEF(.boot_text_flash, n)
#define BOOT_RODATA_LOC                 HAL_SEC_LOC(.boot_rodata)
#define BOOT_RODATA_DEF(n)              HAL_SEC_DEF(.boot_rodata, n)
#define BOOT_DATA_LOC                   HAL_SEC_LOC(.boot_data)
#define BOOT_DATA_DEF(n)                HAL_SEC_DEF(.boot_data, n)
#define BOOT_BSS_LOC                    HAL_SEC_LOC(.boot_bss)
#define BOOT_BSS_DEF(n)                 HAL_SEC_DEF(.boot_bss, n)

#define SRAM_TEXT_LOC                   HAL_SEC_LOC(.sram_text)
#define SRAM_TEXT_DEF(n)                HAL_SEC_DEF(.sram_text, n)
#define SRAM_DATA_LOC                   HAL_SEC_LOC(.sram_data)
#define SRAM_DATA_DEF(n)                HAL_SEC_DEF(.sram_data, n)
#define SRAM_STACK_LOC                  ALIGNED(8) HAL_SEC_LOC(.sram_data)
#define SRAM_STACK_DEF(n)               ALIGNED(8) HAL_SEC_LOC(.sram_data, n)
#define SRAM_BSS_LOC                    HAL_SEC_LOC(.sram_bss)
#define SRAM_BSS_DEF(n)                 HAL_SEC_DEF(.sram_bss, n)

#define FRAM_TEXT_LOC                   HAL_SEC_LOC(.fast_text_sram)
#define FRAM_TEXT_DEF(n)                HAL_SEC_DEF(.fast_text_sram, n)

#ifdef SYSTEM_USE_PSRAM
#define PSRAM_TEXT_LOC                   HAL_SEC_LOC(.psram_text)
#define PSRAM_TEXT_DEF(n)                HAL_SEC_DEF(.psram_text, n)
#define PSRAM_DATA_LOC                   HAL_SEC_LOC(.psram_data)
#define PSRAM_DATA_DEF(n)                HAL_SEC_DEF(.psram_data, n)
#define PSRAM_STACK_LOC                  ALIGNED(8) HAL_SEC_LOC(.psram_data)
#define PSRAM_STACK_DEF(n)               ALIGNED(8) HAL_SEC_LOC(.psram_data, n)
#define PSRAM_BSS_LOC                    HAL_SEC_LOC(.psram_bss)
#define PSRAM_BSS_DEF(n)                 HAL_SEC_DEF(.psram_bss, n)
#endif


#define FLASH_TEXT_LOC                  HAL_SEC_LOC(.flash_text)
#define FLASH_TEXT_DEF(n)               HAL_SEC_DEF(.flash_text, n)
#define FLASH_RODATA_LOC                HAL_SEC_LOC(.flash_rodata)
#define FLASH_RODATA_DEF(n)             HAL_SEC_DEF(.flash_rodata, n)

#else // !__GNUC__ || ROM_BUILD || PROGRAMMER

#define BOOT_TEXT_SRAM_LOC
#define BOOT_TEXT_SRAM_DEF(n)           n
#define BOOT_TEXT_FLASH_LOC
#define BOOT_TEXT_FLASH_DEF(n)          n
#define BOOT_RODATA_LOC
#define BOOT_RODATA_DEF(n)              n
#define BOOT_DATA_LOC
#define BOOT_DATA_DEF(n)                n
#define BOOT_BSS_LOC
#define BOOT_BSS_DEF(n)                 n

#define SRAM_TEXT_LOC
#define SRAM_TEXT_DEF(n)                n
#define SRAM_DATA_LOC
#define SRAM_DATA_DEF(n)                n
#define SRAM_STACK_LOC
#define SRAM_STACK_DEF(n)               n
#define SRAM_BSS_LOC
#define SRAM_BSS_DEF(n)                 n

#define FRAM_TEXT_LOC
#define FRAM_TEXT_DEF(n)                n

#define FLASH_TEXT_LOC
#define FLASH_TEXT_DEF(n)               n
#define FLASH_RODATA_LOC
#define FLASH_RODATA_DEF(n)             n

#endif // !__GNUC__ || ROM_BUILD || PROGRAMMER

#ifdef __cplusplus
}
#endif

#endif
