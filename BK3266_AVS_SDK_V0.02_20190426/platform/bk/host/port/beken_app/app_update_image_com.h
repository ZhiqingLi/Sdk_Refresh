#ifndef _BACKUP_IMAGE_H_
#define _BACKUP_IMAGE_H_

#include "config.h"
#ifdef BEKEN_UP_OTA_3266

#include "types.h"
#include <stddef.h>
#include "string.h"
#include "sys_config.h"


#define OAD_IMAGE_A
#define LEN_SIZE_UINT  16

#define UART_PRINTF                          1
#define BOOT_UART_DEBUG											 0

#define CFG_UDI_ERASE_ALL_FIRST                            1

#define FLASH_PAGE                                        (256)
#define FLASH_PAGE_MASK                                   (FLASH_PAGE - 1)
#define FLASH_ERASED_CODE                                 (0xFFFFFFFF)

#define FLASH_ERASE_SECTOR_SIZE_RSL_BIT_CNT               (12)
#define FLASH_ERASE_SECTOR_SIZE                           (4096)
#define FLASH_ERASE_SECTOR_SIZE_MASK                      (FLASH_ERASE_SECTOR_SIZE - 1)

#define UPDATE_CHUNK_SIZE                                 (32)

typedef void (*FUNCPTR)(void);

#define MAX(x, y)                  (((x) > (y)) ? (x) : (y))
#define MIN(x, y)                  (((x) < (y)) ? (x) : (y))
#define max(x, y)                  (((x) > (y)) ? (x) : (y))
#define min(x, y)                  (((x) < (y)) ? (x) : (y))

#define UDI_DEFAULT_VERSION                 (0x11110000)
#define UDI_SIGNATURE_CODE                  (0x88776655)

#define ENCRYPT_FRAME_SIZE             (0x20)
#define ENCRYPT_CRC_SIZE               (0x02)
#define crc_size(len)                  ((len >> 5) * (ENCRYPT_FRAME_SIZE + ENCRYPT_CRC_SIZE))
#define crc_address(len)               (((unsigned int)(len >> 5)) * (ENCRYPT_FRAME_SIZE + ENCRYPT_CRC_SIZE) + (len & (ENCRYPT_FRAME_SIZE - 1)))

typedef struct
{
    unsigned int signature;          /* section signature*/
    unsigned int version;            /* section version*/
    unsigned int sec_size;
    unsigned int sec_crc;
    unsigned int imgback_addr;
    unsigned int rsv[3];             /* for scatter file adds the attribute word: ALIGNALL 32*/
} SECTION_BOUNDARY_T, *SECTION_BOUNDARY_PTR;

typedef struct
{
    SECTION_BOUNDARY_T left_boundary;
    SECTION_BOUNDARY_T right_boundary;
    u_int32 sect_status;

    u_int32 erase_offset;
    u_int32 update_offset;
} UDI_SECTION_T, *UDI_SECTION_PTR;

extern UDI_SECTION_T psec_A;
extern UDI_SECTION_T psec_B;

#ifdef OAD_IMAGE_A
extern UDI_SECTION_T bsec_B;
#endif //OAD_IMAGE_A

#ifdef OAD_IMAGE_B
extern UDI_SECTION_T bsec_A;
#endif //OAD_IMAGE_B

extern uint32_t calc_backup_sec_crc(void);


#endif // BEKEN_UP_OTA_3266

#endif // _BACKUP_IMAGE_H_
// eof

