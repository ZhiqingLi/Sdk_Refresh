#ifndef __BES_HAL_FLASH_H__
#define __BES_HAL_FLASH_H__

#include "plat_types.h"
#ifdef __cplusplus
extern "C" {
#endif

#define PARTITION_NOERR                 0
#define PARTITION_ERR                   -1
#define PARTITION_CRC_CHECK_ERROR       -2



/* Norflash paration area 
    paration name           size            start addr
    OTA firmware            96KB            0x00000000
    BES Firmware            828KB           0x00018000
    User Data               2048KB          0x000E7000
    CX20921 Bootloader A    50KB            0x002E7000
    CX20921 Bootloader B    50KB            0x002F3800
    CX20921 Firmware A      500KB           0x00300000
    CX20921 Firmware B      500KB           0x0037D000
    resovle                 12KB            0x003FA000
    audio param info        4KB             0x003FD000
    user data info          4KB             0x0037E000
    BT NV Info              4KB             0x0037F000
*/
#define VOICE_INPUT_BOOTLOADER_PARTITION_SIZE           (50*1024) // 50KB
#define VOICE_INPUT_BOOTLOADER_PARTITION_HEAD_SIZE      (4*1024)   // 4KB
#define VOICE_INPUT_BOOTLOADER_PARTITION_A_ADDR         (0x002E7000)
#define VOICE_INPUT_BOOTLOADER_PARTITION_B_ADDR         (0x002F3800)

#define VOICE_INPUT_FIRMWARE_PARTITION_SIZE             (500*1024) // 500KB
#define VOICE_INPUT_FIRMWARE_PARTITION_HEAD_SIZE        (4*1024)   // 4KB
#define VOICE_INPUT_FIRMWARE_PARTITION_A_ADDR           (0x00300000)
#define VOICE_INPUT_FIRMWARE_PARTITION_B_ADDR           (0x0037D000)

#define VOICE_INPUT_MAGIC_NUM_LEN                   (4)
#define VOICE_INPUT_PAYLOAD_LEN                     (4)
#define VOICE_INPUT_BINFIlE_VER_LEN                 (4)
#define VOICE_INPUT_MD5_INFO_LEN                    (16)

enum VOICE_INPUT_CODE_PARTITION_T {
    BOOTLOADER_PARTITION_A = 0,
    BOOTLOADER_PARTITION_B,
    FIRMWARE_PARTITION_A,
    FIRMWARE_PARTITION_B,
    FIRMWARE_PARTITION_MAX
};

typedef struct
{
    uint8_t magicNum[VOICE_INPUT_MAGIC_NUM_LEN];
    uint8_t payloadLen[VOICE_INPUT_PAYLOAD_LEN];
    uint32_t version[VOICE_INPUT_BINFIlE_VER_LEN];
    uint8_t md5Info[VOICE_INPUT_MD5_INFO_LEN];
}voice_input_partition_head_t;

uint8_t bes_hal_norflash_init(void);
uint8_t bes_hal_norflash_deinit(void);
int32_t voice_input_erase_partition(enum VOICE_INPUT_CODE_PARTITION_T partitionIndex);
int32_t voice_input_write_head_partition(enum VOICE_INPUT_CODE_PARTITION_T partitionIndex, void* pHeader);
int32_t voice_input_read_head_partition(enum VOICE_INPUT_CODE_PARTITION_T partitionIndex, void* pHeader);
int32_t voice_input_write_partition(enum VOICE_INPUT_CODE_PARTITION_T partitionIndex, uint32_t* pWriteAddr, uint8_t* pbuf, uint32_t len);
int32_t voice_input_read_partition(enum VOICE_INPUT_CODE_PARTITION_T partitionIndex, uint32_t* pReadAddr, uint8_t* pbuf, uint32_t len);
int32_t voice_input_md5_check(enum VOICE_INPUT_CODE_PARTITION_T partitionIndex);

#ifdef __cplusplus
}
#endif
#endif