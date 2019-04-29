
#include "app_update_image_com.h"

#ifdef BEKEN_UP_OTA_3266

#include "app_Oad_target.h"
#include "app_Oad.h"
#include "app_update_image_3266.h"

#include "sys_features.h"
#include "hc_const.h"

#include "hw_lc.h"
#include <stdio.h>

#include "hc_const.h"
#include "hc_event_gen.h"
#include "tra_queue.h"
#include "bt_timer.h"
#include "sys_mmi.h"
#include "config.h"
#include "lmp_utils.h"
#include "bk3000_reg.h"
#include "app_beken_includes.h"

#ifdef BEKEN_BLE_OTA_3266
#include "hw_le_lc_impl.h"

#include "hw_le_lc.h"
#include "le_connection.h"

#if(PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)
#include "le_att.h"
#include "le_gatt_server.h"
#endif

#if(PRH_BS_CFG_SYS_LE_GAP_INCLUDED==1)
#include "le_gap.h"
#include "le_gap_const.h"
#endif

#include "le_const.h"
#include "le_link_layer.h"

#if (PRH_BS_CFG_SYS_LE_SMP_INCLUDED==1)
#include "le_smp.h"
#endif

#if (PRH_BS_CFG_SYS_LE_GAP_INCLUDED==1)
#include "le_gap.h"
#endif

#if (PRH_BS_CFG_SYS_LE_L2CAP_INCLUDED==1)
#include "le_l2cap.h"
#endif


#include "le_config.h"
#include "le_cts_app.h"

//MTU长度
#include "lp_pdd_main.h" 


#endif

/*********************************************************************
 * CONSTANTS
 */
#ifdef BEKEN_BLE_OTA_3266
extern void SYSirq_Disable_Interrupts_Save_Flags(u_int32 *flags, u_int32 *mask);
extern void SYSirq_Interrupts_Restore_Flags(u_int32 flags, u_int32 mask);
#endif
#ifdef BEKEN_SPP_OTA_3266
extern __INLINE__ unsigned char spp_is_connected(void);
extern result_t spp_send( char *buff, uint8_t len );
#endif
extern void set_flash_protect(uint8 all);

//#define	FEATURE_OAD_BIM
#define	HAL_IMAGE_A
#define	FEATURE_OAD


#define OAD_FLASH_PAGE_MULT  ((uint16)(HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE))


#if !defined (OAD_IMAGE_VERSION)
#define OAD_IMAGE_VERSION    0x0000
#endif

#if !defined (OAD_IMAGE_A_USER_ID)
#define OAD_IMAGE_A_USER_ID  {'A', 'A', 'A', 'A'}
#endif

#if !defined (OAD_IMAGE_B_USER_ID)
#define OAD_IMAGE_B_USER_ID  {'B', 'B', 'B', 'B'}
#endif


/*********************************************************************
 * MACROS
 */
#define ATT_UUID_SIZE                    16
#define OAD_CHAR_CNT										 2
#define	GATT_MAX_NUM_CONN								 2


// Size of 128-bit UUID
// TI Base 128-bit UUID: F000XXXX-0451-4000-B000-000000000000
#define TI_BASE_UUID_128( uuid )  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, \
                                  0x00, 0x40, 0x51, 0x04, LO_UINT16( uuid ), HI_UINT16( uuid ), 0x00, 0xF0

/*********************************************************************
 * GLOBAL VARIABLES
 */
u_int8  ota_data_tmp[512] = {0}, ota_data_count = 0;
u_int8  ota_update_flag = 0, ota_end_flag = 0;
volatile uint8 flag_ota_beginning = 0;
#ifdef BEKEN_BLE_OTA_3266
// OAD Service UUID
static  uint8 oadServUUID[ATT_UUID_SIZE] =
{
    TI_BASE_UUID_128( OAD_SERVICE_UUID )
};

static  uint8 oadImgIdentifyCharUUID[ATT_UUID_SIZE] =
{
    // OAD Image Identify UUID
    TI_BASE_UUID_128( OAD_IMG_IDENTIFY_UUID ),
};

static  uint8 oadImgBlockCharUUID[ATT_UUID_SIZE] =
{
    // OAD Image Block Request/Response UUID
    TI_BASE_UUID_128( OAD_IMG_BLOCK_UUID )
};
// Place holders for the GATT Server App to be able to lookup handles.
static uint8 oadImgIdentifyCharVals[DEFAULT_LP_PDD_BLE_DATA_SEC_SIZE];
static uint8 oadImgBlockCharVals[DEFAULT_LP_PDD_BLE_DATA_SEC_SIZE];
// OAD Client Characteristic Configs
static uint8 oadImgIdentifyConfig[2] = {0x00, 0x00};
static uint8 oadImgBlockConfig[2] = {0x00, 0x00};
// OAD Characteristic user descriptions
static  uint8 oadImgIdentifyDesc[] = "Img Identify";
static  uint8 oadImgBlockDesc[] = "Img Block";
#endif

/*********************************************************************
 * LOCAL VARIABLES
 */
uint16 oadBlkNum = 0, oadBlkTot = 0xFFFF;

//start add by ps
#ifdef BEKEN_BLE_OTA_3266_POWER_FAIL
uint16 oadPreBlkFlag = 0;
uint32 oadPreCrc = 0;
uint16 oadPreBlkNum = 0;
extern u_int32 udi_updating_BK3266_CRC(u_int8 *buf);
extern uint16 oadsize_4K;

#endif
//end add by ps


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void oadImgBlockReq(uint16 connHandle, uint16 blkNum);
static void oadImgIdentifyReq(uint16 connHandle, img_hdr_t *pImgHdr);
#ifdef BEKEN_BLE_OTA_3266
extern t_LE_Config LE_config;
// OAD Characteristic Properties
static uint8 oadCharProps = GATT_PROPERTIES_WRITE_WITHOUT_RESPONSE | GATT_PROPERTIES_WRITE | GATT_PROPERTIES_NOTIFY;

/*******************************************************
 * OAD PROFILE DEFINITION
 * Handle 0x1000 to 0x1010
 *
 ******************************************************/
/*
_GATTserver_Add_Service(u_int8 service_type,u_int16 handle,u_int8* p_service_UUID,u_int16 service_UUID)

 _GATTserver_Add_Characteristic_Declaration(u_int16 handle,u_int16 char_UUID,u_int8 properties)
 u_int8 _GATTserver_Add_16ByteUUID_Characteristic_Declaration(u_int16 handle, u_int8* p_UUID,u_int8 properties)


_GATTserver_Add_Characteristic_Value(u_int16 handle,u_int16 char_UUID,u_int8 local_notify,u_int8 permissions,u_int8 val_len,u_int8* p_value)
_GATTserver_Add_16ByteUUID_Characteristic_Value(u_int16 handle,u_int8* char_UUID,u_int8 local_notify,u_int8 permissions,u_int8 val_len,u_int8* p_value)


u_int8 _GATTserver_Add_Char_Descriptor(u_int16 handle,u_int16 uuid,u_int8 permission,u_int8 val_len,u_int8* p_value)
*/

u_int16 App_Setup_OAD_GATT_DB(void)
{
    u_int16 handle = 0x1000;

    // OAD Service
    _GATTserver_Add_Service(PRIMARY_SERVICE_128BIT, handle, oadServUUID, OAD_SERVICE_UUID);
    handle++;

    // OAD Image Identify Characteristic Declaration
    _GATTserver_Add_16ByteUUID_Characteristic_Declaration(handle, oadImgIdentifyCharUUID, oadCharProps);
    handle++;

    // OAD Image Identify Characteristic Value
    _GATTserver_Add_16ByteUUID_Characteristic_Value(handle, oadImgIdentifyCharUUID, USE_LOCAL_NOTIFY, GATT_PERMISSION_WRITE, DEFAULT_LP_PDD_BLE_DATA_SEC_SIZE, oadImgIdentifyCharVals);
    handle++;


    // Characteristic configuration
    _GATTserver_Add_Char_Descriptor(handle, CLIENT_CHARACTERISTIC_CONFIGURATION, GATT_PERMISSION_READ | GATT_PERMISSION_WRITE, 0x02, oadImgIdentifyConfig);
    handle++;

    // OAD Image Identify User Description
    _GATTserver_Add_Char_Descriptor(handle, CHARACTERISTIC_USER_DESCRIPTION, GATT_PERMISSION_READ, 12, oadImgIdentifyDesc);
    handle++;



    // OAD Image Block Request/Response Characteristic Declaration
    _GATTserver_Add_16ByteUUID_Characteristic_Declaration(handle, oadImgBlockCharUUID, oadCharProps);
    handle++;


    // OAD Image Block Request/Response Characteristic Value
    _GATTserver_Add_16ByteUUID_Characteristic_Value(handle, oadImgBlockCharUUID, USE_LOCAL_NOTIFY, GATT_PERMISSION_WRITE, DEFAULT_LP_PDD_BLE_DATA_SEC_SIZE, oadImgBlockCharVals);
    handle++;


    // Characteristic configuration
    _GATTserver_Add_Char_Descriptor(handle, CLIENT_CHARACTERISTIC_CONFIGURATION, GATT_PERMISSION_READ | GATT_PERMISSION_WRITE, 0x02, oadImgBlockConfig);
    handle++;



    // OAD Image Block Request/Response User Description
    _GATTserver_Add_Char_Descriptor(handle, CHARACTERISTIC_USER_DESCRIPTION, GATT_PERMISSION_READ, 9, oadImgBlockDesc);
    handle++;

    return 0;
}
#endif
/*********************************************************************
 * @fn      oadImgIdentifyWrite
 *
 * @brief   Process the Image Identify Write.
 *
 * @param   connHandle - connection message was received on
 * @param   pValue - pointer to data to be written
 *
 * @return  status
 */ 
#define FLASH_ADDR  0x00001024
#define FLASH_SHFT  0
#define FLASH_MASK  0XFFFFFFFF
extern u_int8 get_active_sw_section(void);
extern u_int16 get_software_version(u_int8 half);




/****************	0x1001		********************/


/*********************************************************************
 * @fn      oadImgIdentifyReq
 *
 * @brief   Process the Image Identify Request.
 *
 * @param   connHandle - connection message was received on
 * @param   pImgHdr - Pointer to the img_hdr_t data to send.
 *
 * @return  None
 */
static void oadImgIdentifyReq(uint16 connHandle, img_hdr_t *pImgHdr)
{
//    os_printf("oadImgIdentifyReq()\r\n");
#ifdef BEKEN_SPP_OTA_3266
    if(spp_is_connected()) //SPP
    {
        uint8 noti[18];

        noti[0] = LO_UINT16(OADHEADER_SPP);    //header
        noti[1] = HI_UINT16(OADHEADER_SPP);
		noti[2] = 0x0e;    //len
        noti[3] = 0x00;
        
        noti[4] = LO_UINT16(HANDLE_BLE_T1);    //handle
        noti[5] = HI_UINT16(HANDLE_BLE_T1);

        noti[6] = 0x0A;  //length
        noti[7] = 0x00;
        
        noti[8] = LO_UINT16(pImgHdr->ver);  //ver
        noti[9] = HI_UINT16(pImgHdr->ver);
		

        noti[10] = LO_UINT16(pImgHdr->len);  //len
        noti[11] = HI_UINT16(pImgHdr->len);

        noti[12] = 'A';   //uuid
        noti[13] = 'A';
        noti[14] = 'A';
        noti[15] = 'A';

        noti[16] = 0x66;  //0x66 rom_ver
        noti[17] = 0x66; //0x66
        //GATTserver_Characteristic_Write_Local_Value(OADIMGIDENTIFY_HANDLE, OAD_IMG_HDR_SIZE, noti);
		//os_printf("spp_send((char *)noti, 18)\r\n");
		spp_send((char *)noti, 18);
    }
    else
#endif
    {
    #ifdef BEKEN_BLE_OTA_3266
        uint16 value = LMutils_Get_Uint16(oadImgBlockConfig);
        // If notifications enabled
        if ( value & 0x0001 ) //BLE
        {
//add begin by	ps for OTA
			uint8 noti[14];

			noti[0] = LO_UINT16(HANDLE_BLE_T1);    //handle
	        noti[1] = HI_UINT16(HANDLE_BLE_T1);

	        noti[2] = 0x0A;  //length
	        noti[3] = 0x00;

            noti[4] = LO_UINT16(pImgHdr->ver);
            noti[5] = HI_UINT16(pImgHdr->ver);

            noti[6] = LO_UINT16(pImgHdr->len);
            noti[7] = HI_UINT16(pImgHdr->len);

            noti[8] = 'A';
            noti[9] = 'A';

            noti[10] = 'A';
            noti[11] = 'A';

            noti[12] = 0x66;  //0x66 rom_ver
	        noti[13] = 0x66; //0x66
        	GATTserver_Characteristic_Write_Local_Value(OADIMGBLOCK_BLE_HANDLE, 14, noti);
//add end by	ps for OTA
        }
    #endif
    }
}




/****************	0x1002		********************/


/*********************************************************************
 * @fn      oadImgIdentifyReq
 *
 * @brief   Process the Image Identify Request.
 *
 * @param   connHandle - connection message was received on
 * @param   pImgHdr - Pointer to the img_hdr_t data to send.
 *
 * @return  None
 */
static void oadImgBlockReq(uint16 connHandle, uint16 blkNum)
{
//    os_printf("oadImgBlockReq()\r\n");

#ifdef BEKEN_SPP_OTA_3266
    if(spp_is_connected()) //SPP
    {
        uint8 noti[10];
        
        noti[0] = LO_UINT16(OADHEADER_SPP);    //header
        noti[1] = HI_UINT16(OADHEADER_SPP);
		noti[2] = 0x06;    //len
        noti[3] = 0x00;
        
        noti[4] = LO_UINT16(HANDLE_BLE_T2);    //handle
        noti[5] = HI_UINT16(HANDLE_BLE_T2);

        noti[6] = 0x02;
        noti[7] = 0x00;

        noti[8] = LO_UINT16(blkNum);
        noti[9] = HI_UINT16(blkNum);

        //GATTserver_Characteristic_Write_Local_Value(OADIMGBLOCK_HANDLE, 2, noti);
        spp_send((char *)noti, 10);
    }
    else
#endif
    {
    #ifdef BEKEN_BLE_OTA_3266
        uint16 value = LMutils_Get_Uint16(oadImgBlockConfig);
        // If notifications enabled
        //     0x0001 //!< The Characteristic Value shall be notified
        //     0x0002 //!< The Characteristic Value shall be indicated

        if ( value & 0x0001 )//BLE
        {
//add begin by	ps for OTA
			uint8 noti[6];
        
		    noti[0] = LO_UINT16(HANDLE_BLE_T2);    //header
		    noti[1] = HI_UINT16(HANDLE_BLE_T2);

			noti[2] = 0x02;	//len	2字节
        	noti[3] = 0x00;

			noti[4] = LO_UINT16(blkNum);
			noti[5] = HI_UINT16(blkNum);
			GATTserver_Characteristic_Write_Local_Value(OADIMGBLOCK_BLE_HANDLE, 6, noti);
//add end by	ps for OTA
        }
    #endif
    }
}



uint8 oadImgIdentifyWrite( uint16 connHandle, uint8 length ,uint8 *pValue )
{
    img_hdr_t rxHdr;
    img_hdr_t ImgHdr;
   
    uint8 len[2] = {0x00, 0x7c};
#ifdef OAD_IMAGE_B
    uint8 ver[2] = {0x01, 0x00};
    uint8 uid[4] = {0x42, 0x42, 0x42, 0x42};//Image B  ver bit0 = 1
#else
    uint8 ver[2] = {0x11, 0x12};
    uint8 uid[4] = {0x41, 0x41, 0x41, 0x41};//Image A  ver bit0 = 0
#endif
#ifdef BEKEN_BLE_OTA_3266
    uint8 buf[10];
    uint8 rom_ver[2] = {0x06,0x00};
#endif
//    os_printf("oadImgIdentifyWrite()\r\n");
	
    uid[3] = uid[2] = uid[1] = uid[0] = get_active_sw_section();

    if(uid[0] == 0x41) // A section
    {
        ver[0] = (get_software_version(0) & 0x00ff);
        ver[1] = (get_software_version(0) & 0xff00)>>8;
    }
    else
    {
        ver[0] = (get_software_version(1) & 0x00ff);
        ver[1] = (get_software_version(1) & 0xff00)>>8;
    }

    rxHdr.ver = LMutils_Get_Uint16( pValue + 4);
    rxHdr.len = LMutils_Get_Uint16( pValue + 6);

    ImgHdr.uid = LMutils_Get_Uint32(uid);
    ImgHdr.ver = LMutils_Get_Uint16(ver);
    ImgHdr.len = LMutils_Get_Uint16(len);

#if(LEN_SIZE_UINT == 16)
    oadBlkTot = rxHdr.len;
#else
    oadBlkTot = rxHdr.len / (OAD_BLOCK_SIZE / HAL_FLASH_WORD_SIZE);
#endif
    OTA_PRINT("oadBlkTot = %x\r\n", oadBlkTot);
    OTA_PRINT("ImgHdr.ver = %x, rxHdr.ver = %x\r\n", OAD_IMG_ID( ImgHdr.ver ), OAD_IMG_ID( rxHdr.ver ));

    if(length == 0x10)
    {
    	uint8 *data = pValue;
		os_printf("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\n",
                data[0],data[1],data[2],data[3],data[4],data[5],
                data[6],data[7],data[8],data[9],data[10],
                data[11],data[12],data[13],data[14],data[15]);
	
        if ((OAD_IMG_ID( ImgHdr.ver ) != OAD_IMG_ID( rxHdr.ver )) && // TBD: add customer criteria for initiating OAD here.
             (oadBlkTot <= OAD_BLOCK_MAX) && (oadBlkTot != 0))
        {
            oadBlkNum = 0;
            oadImgBlockReq(connHandle, 0);
            OTA_PRINT("BlockReq num =%x \r\n", oadBlkNum);
        }
        else
        {
#if 0
        #ifdef BEKEN_SPP_OTA_3266    
            if(spp_is_connected()) //SPP
            {
            
            }
            else
        #endif
            {
            #ifdef BEKEN_BLE_OTA_3266
                extern t_App_Control LEapp_Config;
                LE_L2CAP_Connection_Update(0x10/*use internal valid handle*/,0x0020,0x0018,0,0x0258);
                LEapp_Config.TAPP_L2CAP_PARA_UPDATE_Timer =  BT_TIMER_OFF;
            #endif
        	}
#endif
            oadImgIdentifyReq(connHandle, &ImgHdr);
            OTA_PRINT("Error OTA verion!\r\n");
        }

    }
    else
    {	
		oadImgIdentifyReq(connHandle, &ImgHdr);
    }
    
    return ( 0x00 );//SUCCESS
}





/****************	0x1003		********************/
void oad_state_to_app(uint16 blknum,uint16 status)
{
#ifdef BEKEN_SPP_OTA_3266
    if(spp_is_connected()) //SPP
    {
        uint8 noti[12];
        
        noti[0] = LO_UINT16(OADHEADER_SPP);    //header
        noti[1] = HI_UINT16(OADHEADER_SPP);
		noti[2] = 0x08;    //len
        noti[3] = 0x00;
        
        noti[4] = LO_UINT16(HANDLE_BLE_T3);    //handle
		noti[5] = HI_UINT16(HANDLE_BLE_T3);

		noti[6] = 0x04;  //length
		noti[7] = 0x00;

		noti[8] = LO_UINT16(blknum);
		noti[9] = HI_UINT16(blknum);

		noti[10] = LO_UINT16(status);
		noti[11] = HI_UINT16(status);

        //GATTserver_Characteristic_Write_Local_Value(OADIMGBLOCK_HANDLE, 2, noti);
        spp_send((char *)noti, 12);
    }
    else
#endif
    {
    #ifdef BEKEN_BLE_OTA_3266
		uint16 value = LMutils_Get_Uint16(oadImgBlockConfig);
		if ( value & 0x0001 )
		{
			uint8 noti[8];
			noti[0] = LO_UINT16(HANDLE_BLE_T3);    //handle
			noti[1] = HI_UINT16(HANDLE_BLE_T3);

			noti[2] = 0x04;  //length
			noti[3] = 0x00;

			noti[4] = LO_UINT16(blknum);
			noti[5] = HI_UINT16(blknum);

			noti[6] = LO_UINT16(status);
			noti[7] = HI_UINT16(status);

			GATTserver_Characteristic_Write_Local_Value(OADIMGBLOCK_BLE_HANDLE, 8, noti);
		}
    #endif
    }
}


u_int8  Oad_save_reciveData(u_int8 *pValue )
{
    hw_memcpy8(ota_data_tmp + (ota_data_count*OTA_PACKET_SIZE), pValue + 2, OTA_PACKET_SIZE);
    ota_data_count++;
#ifdef BEKEN_SPP_OTA_3266
    if(spp_is_connected()) //SPP
    {
        if(ota_data_count >= 12)
        {    	    
            udi_updating_BK3266_section(ota_data_tmp, ota_data_count * OTA_PACKET_SIZE);
            ota_data_count = 0;
            os_printf("Oad save data overflow!!!!\r\n");
        }
    }
	else
#endif	
	{
#ifdef BEKEN_BLE_OTA_3266
		if(ota_data_count >= 12)
        {    	    
            udi_updating_BK3266_section(ota_data_tmp, ota_data_count * OTA_PACKET_SIZE);
            ota_data_count = 0;
            os_printf("Oad save data overflow!!!!\r\n");
        }
#endif
	}
    return 0;
}



uint8 oadImgBlockWrite( uint16 connHandle, uint8 len,uint8 *pValue )
{
    uint16 blkNum = LMutils_Get_Uint16( pValue);
    // make sure this is the image we're expecting
    if ( blkNum == 0 )
    {
		uint8 *data = pValue;
		os_printf("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\n",
                data[0],data[1],data[2],data[3],data[4],data[5],
                data[6],data[7],data[8],data[9]);

	
        uint16 ver = LMutils_Get_Uint16( pValue + 6);
        uint32 crc = LMutils_Get_Uint32(pValue + 2);
    #if(LEN_SIZE_UINT == 16)
        uint16 blkTot = LMutils_Get_Uint16( pValue + 8 );
    #else
        uint16 blkTot = LMutils_Get_Uint16( pValue + 8 ) / (OAD_BLOCK_SIZE / HAL_FLASH_WORD_SIZE);
    #endif
        set_flash_protect(0);
    
        OTA_PRINT("start blkNum = %x, ver = %x, blkTot = %x\r\n", blkNum, ver, blkTot);
        if((oadBlkNum != blkNum) || (oadBlkTot != blkTot))
        {
            OTA_PRINT("ERROR: oadBlkNum = %x, oadBlkTot = %x, ver = %x\r\n",oadBlkNum, oadBlkTot, ver);
        #ifdef BEKEN_SPP_OTA_3266
            if(spp_is_connected()) //SPP
            {
                return 0;
            }
            else
        #endif
            {
            #ifdef BEKEN_BLE_OTA_3266
                return ( GATT_ERROR_WRITE_NOT_PERMITTED );
            #endif
            }
        }
        
        ota_data_count = 0;
        ota_end_flag = 0;
        flag_ota_beginning = 1;
        udi_updating_BK3266_begin(crc , blkTot);//save ImgHdr info

        /* disconnect BT connnection when OTA procedure */
        /*if( bt_flag1_is_set(APP_FLAG_ACL_CONNECTION))
        {
            app_handle_t app_h = app_get_sys_handler();
            bt_unit_acl_disconnect(app_h->unit, &app_h->remote_btaddr);
        }*/
    }
    
    if (oadBlkNum == blkNum)
    {

        if (oadBlkNum != 0)
        {
#if 0

			uint8 *test = pValue+2;
		
        	uint8 data[OTA_PACKET_SIZE];
			memset(data,0,OTA_PACKET_SIZE);
			
			data[0] = test[0];data[1] = test[1];data[2] = test[2];data[3] = test[3];
			data[4] = test[4];data[5] = test[5];data[6] = test[6];data[7] = test[7];
			data[8] = test[8];data[9] = test[9];data[10] = test[10];
			data[11] = test[11];data[12] = test[12];data[13] = test[13];

/*			
			os_printf("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\n",
                data[0],data[1],data[2],data[3],data[4],data[5],
                data[6],data[7],data[8],data[9],data[10],data[11],
                data[12],data[13],data[14],data[15],data[16],data[17],
                data[18],data[19],data[20],data[21],data[22],data[23],
                data[24],data[25],data[26],data[27],data[28],data[29],
                data[30],data[31]);
*/
			

            Oad_save_reciveData(data-2);
#else
			Oad_save_reciveData(pValue);
#endif

			
			oad_state_to_app(oadBlkNum,0);
        }
		else
		{
		
#ifdef BEKEN_BLE_OTA_3266_POWER_FAIL
#ifdef  OAD_IMAGE_A
			UDI_SECTION_PTR sec_ptr = &bsec_B;
#endif		
			//os_printf("@@@@@@@@@@@@@@@@@	%x	%x\r\n",oadPreCrc,oadPreBlkNum);
			if(oadPreCrc == sec_ptr->left_boundary.sec_crc && oadPreCrc != 0 && oadPreCrc != 0xffffffff){
				oad_state_to_app(oadPreBlkNum*OTA_PACKET_NUM_4K,0);
				sec_ptr->update_offset = oadPreBlkNum*OTA_PACKET_SIZE*OTA_PACKET_NUM_4K;
				oadBlkNum = oadPreBlkNum*OTA_PACKET_NUM_4K;
			}else{
				oadPreBlkNum = 0;
				oadPreCrc = sec_ptr->left_boundary.sec_crc;
				oad_state_to_app(oadBlkNum,0);
			}
			udi_updating_BK3266_CRC(&oadPreCrc);
			oadsize_4K = 0;
#else
			oad_state_to_app(oadBlkNum,0);
#endif

		}

		
        oadBlkNum++;

        /* If the OTA Image is complete, set end flag */
        if (oadBlkNum == oadBlkTot) 
        {
            ota_end_flag = 1;	
            OTA_PRINT("OTA update end!\r\n");
        } 
    }
    else
    {     		
        OTA_PRINT( "OTA EXCEPTION:%x,%x\r\n", blkNum, oadBlkNum);
    }
    
    return 0;
}


/****************	0x1004		********************/


void oad_finished(uint8 status)
{
//    os_printf("oad_finished()\r\n");
#ifdef BEKEN_SPP_OTA_3266
    if(spp_is_connected()) //SPP
    {
        uint8 noti[9];
        
        noti[0] = LO_UINT16(OADHEADER_SPP);    //header
        noti[1] = HI_UINT16(OADHEADER_SPP);
		noti[2] = 0x05;    //len
        noti[3] = 0x00;
        
        noti[4] = LO_UINT16(HANDLE_BLE_T4);    //handle
        noti[5] = HI_UINT16(HANDLE_BLE_T4);

        noti[6] = 0x01;  //length
        noti[7] = 0x00;
        
	 	noti[8] = status;

        //GATTserver_Characteristic_Write_Local_Value(OADIMGIDENTIFY_HANDLE, 1, noti);
        spp_send((char *)noti, 9);
    }
    else
#endif
    {
    #ifdef BEKEN_BLE_OTA_3266
        uint16 value = LMutils_Get_Uint16(oadImgBlockConfig);
        // If notifications enabled
        if ( value & 0x0001 )
        {
//add begin by  ps for OTA        
			uint8 noti[5];
			noti[0] = LO_UINT16(HANDLE_BLE_T4);    //handle
			noti[1] = HI_UINT16(HANDLE_BLE_T4);

			noti[2] = 0x01;  //length
			noti[3] = 0x00;

			noti[4] = status;

			GATTserver_Characteristic_Write_Local_Value(OADIMGBLOCK_BLE_HANDLE, 5, noti);
//add end by	ps for OTA

        }
    #endif
    }

	oadBlkNum = 0;
    set_flash_protect(1);
}










/****************	spp and ble handle ota data		********************/
void oad_pdu_decode(uint8 *pValue, uint16_t length)
{

	//os_printf("oad_pdu_decode\r\n");
		
    uint16_t i = 0;
  
    uint16 handle = LMutils_Get_Uint16(pValue);
    uint16 len = LMutils_Get_Uint16(pValue + 2);
    uint8 *payload = (uint8 *)(pValue + 4);
    
    switch(handle)
    {
        case HANDLE_BLE_T1:
            oadImgIdentifyWrite(0, len, payload);
            break;
		
        case HANDLE_BLE_T2:
            oadImgIdentifyWrite(0, len, payload);
            break;

		case HANDLE_BLE_T3:
			if(len == 0){
				if(oadBlkNum == 0)
					oad_state_to_app(0,0);
				else
					oad_state_to_app(oadBlkNum-1,0);
			}
			else{
            	oadImgBlockWrite(0, len, payload);
			}
			
            break;

		case HANDLE_BLE_T4:
            break;

        default :
            break;
    }
}



#endif // BEKEN_UP_OTA_3266
