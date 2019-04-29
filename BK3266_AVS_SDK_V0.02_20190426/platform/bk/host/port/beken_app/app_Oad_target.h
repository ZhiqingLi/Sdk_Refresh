#ifndef _APP_OAD_TARGET_H_
#define _APP_OAD_TARGET_H_

#include "app_update_image_com.h"

#ifdef BEKEN_UP_OTA_3266

#include "types.h"
#include "app_Oad.h"
/*********************************************************************
 * CONSTANTS
 */

#if !defined OAD_IMG_A_PAGE
#define OAD_IMG_A_PAGE        1
#define OAD_IMG_A_AREA        62
#endif

#if !defined OAD_IMG_B_PAGE
// Image-A/B can be very differently sized areas when implementing BIM vice OAD boot loader.
#if defined FEATURE_OAD_BIM
#define OAD_IMG_B_PAGE        8
#else
#define OAD_IMG_B_PAGE        63
#endif
#define OAD_IMG_B_AREA       (124 - OAD_IMG_A_AREA)
#endif

#if defined HAL_IMAGE_B
#define OAD_IMG_D_PAGE        OAD_IMG_A_PAGE
#define OAD_IMG_D_AREA        OAD_IMG_A_AREA
#define OAD_IMG_R_PAGE        OAD_IMG_B_PAGE
#define OAD_IMG_R_AREA        OAD_IMG_B_AREA
#else   //#elif defined HAL_IMAGE_A or a non-BIM-enabled OAD Image-A w/ constants in Bank 1 vice 5.
#define OAD_IMG_D_PAGE        OAD_IMG_B_PAGE
#define OAD_IMG_D_AREA        OAD_IMG_B_AREA
#define OAD_IMG_R_PAGE        OAD_IMG_A_PAGE
#define OAD_IMG_R_AREA        OAD_IMG_A_AREA
#endif

#ifdef BEKEN_SPP_OTA_3266
#define OADHEADER_SPP                                  0x80A1
#endif

#ifdef BEKEN_BLE_OTA_3266
#define OADIMGIDENTIFY_BLE_HANDLE	   0x1002
#define OADIMGBLOCK_BLE_HANDLE	          0x1006
#endif


#define	HANDLE_BLE_T1			0x1001
#define	HANDLE_BLE_T2			0x1002
#define	HANDLE_BLE_T3			0x1003
#define	HANDLE_BLE_T4			0x1004


#define OTA_PACKET_SIZE 		32
#define OTA_PACKET_NUM_4K		128	//4096/32

/*********************************************************************
 * MACROS
 */
#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

//#define NEED_UPDATA_IMAGE_PRINT  

#ifdef NEED_UPDATA_IMAGE_PRINT  
#define OTA_PRINT   bprintf
#else
#define OTA_PRINT   os_printf
#endif

/*********************************************************************
 * GLOBAL VARIABLES
 */
/**
 * GATT Structure for Client Characteristic Configuration.
 */
//typedef struct
//{
//    uint16 connHandle; //!< Client connection handle
//    uint8  value;      //!< Characteristic configuration value for this client
//} gattCharCfg_t;
// OAD Image Header
extern const img_hdr_t _imgHdr;

u_int16 App_Setup_OAD_GATT_DB(void);
uint8 oadImgIdentifyWrite( uint16 connHandle,uint8 length, uint8 *pValue );
uint8 oadImgBlockWrite( uint16 connHandle, uint8 len,uint8 *pValue );
extern u_int8  Oad_save_reciveData(u_int8 *pValue );
extern u_int8 Oad_write_Flash(void);
extern void oad_finished(uint8 status);

void oad_pdu_decode(uint8 *pValue, uint16_t length);
#endif//#ifdef BEKEN_UP_OTA_3266

#endif
