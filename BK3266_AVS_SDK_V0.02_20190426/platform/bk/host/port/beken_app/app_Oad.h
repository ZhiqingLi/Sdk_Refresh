#ifndef _APP_OAD_H_
#define _APP_OAD_H_

#include "app_update_image_com.h"

#ifdef BEKEN_UP_OTA_3266

/*********************************************************************
 * CONSTANTS
 */

#define OAD_SERVICE_UUID      0xFFC0
#define OAD_IMG_IDENTIFY_UUID 0xFFC1
#define OAD_IMG_BLOCK_UUID    0xFFC2


#define OAD_IMG_CRC_OSET      0x0000
#define OAD_IMG_HDR_OSET      0x0002

#define OAD_CHAR_CNT          2

// OAD Characteristic Indices
#define OAD_CHAR_IMG_IDENTIFY 0
#define OAD_CHAR_IMG_BLOCK    1

// Image Identification size
#define OAD_IMG_ID_SIZE       4

// Image header size (version + length + image id size)
#define OAD_IMG_HDR_SIZE      ( 2 + 2 + OAD_IMG_ID_SIZE )


// The Image is transporte in 16-byte blocks in order to avoid using blob operations.
//需要根据实际的值修改
#define OAD_BLOCK_SIZE        16
#define HAL_FLASH_PAGE_SIZE   256

#define HAL_FLASH_WORD_SIZE   4

#define OAD_BLOCKS_PER_PAGE  (HAL_FLASH_PAGE_SIZE / OAD_BLOCK_SIZE)

#ifdef BEKEN_UP_OTA_3266
#define OAD_BLOCK_MAX        (0xFFFF)
#else
#define OAD_BLOCK_MAX        (8092) // OAD_BLOCK_MAX = (PRIMARY_24_SECTION_SIZE * 34 / 32) / OAD_BLOCK_SIZE

#endif
/*********************************************************************
 * MACROS
 */

// Macros to get Image ID (LSB) and Version Number
#define OAD_IMG_ID( ver )    (ver)//( (ver) & 0x01 )
#define OAD_VER_NUM( ver )   ( (ver) >> 0x01 )

// Macro to set Image Version
#if defined (HAL_IMAGE_A)
#define OAD_IMG_VER( ver ) ( (uint16)( (ver) << 0x01 ) )            // Clear LSB for Image A
#else
#define OAD_IMG_VER( ver ) ( (uint16)( ( (ver) << 0x01 ) | 0x01 ) ) // Set LSB for Imange B
#endif

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * TYPEDEFS
 */
// The Image Header will not be encrypted, but it will be included in a Signature.
typedef struct
{
    // Secure OAD uses the Signature for image validation instead of calculating a CRC, but the use
    // of CRC==CRC-Shadow for quick boot-up determination of a validated image is still used.
    uint32_t crc;       // CRC must not be 0x0000 or 0xFFFF.
    // User-defined Image Version Number - default logic uses simple a '!=' comparison to start an OAD.
  	uint16_t ver;

	uint16_t len;        // Image length in 4-byte blocks (i.e. HAL_FLASH_WORD_SIZE blocks).

	uint32_t  uid;       // User-defined Image Identification bytes.
	uint8_t  crc_status;     // cur image crc status
	uint8_t  sec_status;     // cur image sec status
	uint16_t  rom_ver;     // Rom ver.
} img_hdr_t;
#endif // BEKEN_UP_OTA_3266

#endif
