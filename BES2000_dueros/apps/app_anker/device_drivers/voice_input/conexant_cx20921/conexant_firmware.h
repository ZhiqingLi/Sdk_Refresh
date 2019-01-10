#ifndef __CONEXANT_FIRMWARE_H__
#define __CONEXANT_FIRMWARE_H__

#ifdef __cplusplus
extern "C" {
#endif
#ifdef VOICE_INPUT_MODULE_CX20921
#include "plat_types.h"

int32_t wait_for_bootloader(void);
int32_t Download_bootloader(uint8_t* pImg, uint32_t imgSize);
int32_t Download_Firmware(uint8_t* pImg, uint32_t imgSize, uint32_t* pChkVer);
int32_t get_firmware_version(uint32_t* verion);
int32_t verify_firmware_version(uint32_t* pChkVer);

void conexant_ota_init(void);

#endif
#ifdef __cplusplus
}
#endif

#endif