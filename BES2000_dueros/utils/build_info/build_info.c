#include "plat_addr_map.h"

#define BUILD_INFO_LOCATION             __attribute__((section(".build_info")))

#define TO_STR_A(s)                     # s
#define TO_STR(s)                       TO_STR_A(s)

const char BUILD_INFO_LOCATION sys_build_info[] =
#if defined(ROM_BUILD) || defined(PROGRAMMER)
    __DATE__   " "   __TIME__ " " TO_STR(REVISION_INFO)
#else
    "CHIP=" TO_STR(CHIP) "\n"
#ifdef SOFTWARE_VERSION
    "SW_VER=" TO_STR(SOFTWARE_VERSION) "\n"
#endif
#ifdef OTA_BUILD
    "SW_TYPE=OTA" "\n"
    "OTA_SEC_SIZE=" TO_STR(OTA_SEC_SIZE) "\n"
#else
    "AUD_SEC_SIZE=" TO_STR(AUD_SECTION_SIZE) "\n"
    "USER_SEC_SIZE=" TO_STR(USERDATA_SECTION_SIZE) "\n"
    "FACT_SEC_SIZE=" TO_STR(FACTORY_SECTION_SIZE) "\n"
#endif
    "FLASH_BASE=" TO_STR(FLASH_NC_BASE) "\n"
    "FLASH_SIZE=" TO_STR(FLASH_SIZE) "\n"
    "NV_REC_DEV_VER=" TO_STR(NV_REC_DEV_VER) "\n"

    //--------------------
    // Add new items above
    //--------------------
    "BUILD_DATE=" __DATE__   " "   __TIME__ "\n"
    "REV_INFO=" TO_STR(REVISION_INFO)
#endif
    ;

