#ifndef _BACKUP_IMAGE_3266_H_
#define _BACKUP_IMAGE_3266_H_
#include "app_update_image_com.h"

#ifdef  BEKEN_UP_OTA_3266
/*  memory layout: three sections
 *  1, boot section;
 *  2, primary section A, started/ended with primary_boundary struct(psec);
 *  3, primary section B, started/ended with primary_boundary struct(bsec);
 *  detail information, please refer to associative code and macro.
 */
/*Every Section has two identification structures*/

#define PSEC_40_CRC_IMAGE_A_ADDR          0x1000 //(((PSEC_40_IMAGE_A_ADDR) >> 5) * (ENCRYPT_FRAME_SIZE + ENCRYPT_CRC_SIZE))
#define PSEC_40_CRC_IMAGE_B_ADDR          0x100000// (((PSEC_40_IMAGE_B_ADDR) >> 5) * (ENCRYPT_FRAME_SIZE + ENCRYPT_CRC_SIZE))

extern u_int32 udi_updating_BK3266_begin(u_int32 version, u_int32 total_len);
extern u_int32 udi_updating_BK3266_section(u_int8 *buf, u_int32 len);
extern u_int32 udi_updating_BK3266_end(void);
#ifdef BEKEN_SPP_OTA_3266
RAM_CODE void BK3266_ota_updating_spp_ms_Sched(void); //SPP
#endif

#endif//  BEKEN_UP_OTA_3266
#endif // _BACKUP_IMAGE_3266_H_

