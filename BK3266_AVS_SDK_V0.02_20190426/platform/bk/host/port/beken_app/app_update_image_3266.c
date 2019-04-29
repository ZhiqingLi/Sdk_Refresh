#include "app_update_image_com.h"

#ifdef BEKEN_UP_OTA_3266

#include "app_Oad_target.h"
#include "app_update_image_3266.h"
#include "driver_flash.h"
#include <jos.h>

extern volatile uint8 flag_ota_beginning;
extern u_int8 get_active_sw_section(void);




#ifdef BEKEN_BLE_OTA_3266_POWER_FAIL

#define ADDR_FLAG 0x1fe000		//0x100000 + 64768*16 + 0x1000
#define ADDR_CRC  0x1fe002	
#define ADDR_BLK  0x1fe006	

extern uint16 oadPreBlkNum;
extern uint32 oadPreCrc;
extern uint16 oadPreBlkFlag;

uint16 oadsize_4K = 0;
static uint32 ADDR_BLK_WRITE = ADDR_BLK;

u_int32 udi_updating_BK3266_FLAG(u_int8 *buf) 
{
	flash_write_data (buf, ADDR_FLAG, 2);

	return 0;
}

u_int32 udi_updating_BK3266_CRC(u_int8 *buf) 
{
	flash_write_data (buf, ADDR_CRC, 4);

	return 0;
}

u_int32 udi_updating_BK3266_BLK(u_int8 *buf) 
{
	flash_write_data (buf, ADDR_BLK_WRITE, 2);
	ADDR_BLK_WRITE += 2;
	
	return 0;
}

u_int32 udi_read_BK3266_CRC_BLK() 
{

	flash_read_data (&oadPreBlkFlag, ADDR_FLAG, 2);
	flash_read_data(&oadPreCrc, ADDR_CRC, 4);

	uint32 temp_blk_addr = ADDR_BLK;
	do{
		flash_read_data(&oadPreBlkNum, temp_blk_addr, 2);
		//os_printf("############		oadPreBlkNum : %d\r\n",oadPreBlkNum);
		temp_blk_addr += 2;
	}while(oadPreBlkNum != 0xffff);

	temp_blk_addr -= 4;
	flash_read_data(&oadPreBlkNum, temp_blk_addr, 2);


	os_printf("############		oadPreBlkNum : %d		oadPreCrc : %08x		oadPreBlkFlag : %04x\r\n",
		oadPreBlkNum,oadPreCrc,oadPreBlkFlag);

	//搽除数据
	flash_erase_sector(ADDR_FLAG);
	return 0;
}

#endif





u_int32 udi_updating_BK3266_begin(u_int32 crc, u_int32 total_len) 
{
#ifdef OAD_IMAGE_A
	u_int32 imgback_addr;
	u_int8 sw_type;
	sw_type =  get_active_sw_section();
	if(sw_type == 0x41)
	{
		imgback_addr = PSEC_40_CRC_IMAGE_B_ADDR;
	}else
	{
		imgback_addr = PSEC_40_CRC_IMAGE_A_ADDR;
	}
	bsec_B.left_boundary.sec_size = total_len;
	bsec_B.left_boundary.sec_crc = crc;
	bsec_B.update_offset = 0;
	bsec_B.left_boundary.imgback_addr = imgback_addr;
	return bsec_B.left_boundary.sec_size;
#endif  

#ifdef OAD_IMAGE_B
	bsec_A.left_boundary.sec_size = total_len;
	bsec_A.update_offset = 0;
	return bsec_A.left_boundary.sec_size;
#endif
}

u_int32 udi_updating_BK3266_section(u_int8 *buf, u_int32 len) 
{
	u_int32 wr_addr;
	UDI_SECTION_PTR sec_ptr;
#ifdef OAD_IMAGE_A
	sec_ptr = &bsec_B;
#endif  

#ifdef OAD_IMAGE_B
	sec_ptr = &bsec_A;
#endif

#ifdef OAD_IMAGE_A
	wr_addr = sec_ptr->update_offset + sec_ptr->left_boundary.imgback_addr;
#endif	

#ifdef  OAD_IMAGE_B
	wr_addr = sec_ptr->update_offset + sec_ptr->left_boundary.imgback_addr;
#endif

	flash_write_data (buf, wr_addr, len);

	sec_ptr->update_offset += len;

	return 0;
}

u_int32 udi_updating_BK3266_end(void) 
{
    uint32 crc = 0,i;
    UDI_SECTION_PTR sec_ptr;
    uint8 section_tag[32] = {0xff};
//    uint8 section_data1 = 0;
//    uint8 section_data2 = 0;


#ifdef  OAD_IMAGE_A
	sec_ptr = &bsec_B;
#endif	 
	for(i=0;i<32;i++)
		section_tag[i] = 0xff;
		
	crc = calc_backup_sec_crc();
	OTA_PRINT( "BK3266_OTA_END: crc = %08x, %08x\r\n", crc, sec_ptr->left_boundary.sec_crc);

    if(crc == sec_ptr->left_boundary.sec_crc )
    {
    	os_printf("*********	get_software_mark(0):%04x	get_software_mark(1):%04x\r\n",
		get_software_mark(0),get_software_mark(1));

        if(sec_ptr->left_boundary.imgback_addr == PSEC_40_CRC_IMAGE_B_ADDR)
        {
//            section_data1 = section_tag[0];
//            section_data2 = section_tag[1];
            section_tag[0] = 0x5a;
            section_tag[1] = 0xa5;
			//bin文件中这里是固定0xffa5,这里才能生效才能设置为a55a,要不就不是这个值会出问题
            flash_write_data(section_tag,PSEC_40_CRC_IMAGE_B_ADDR+0x20,32);		
            Delay(100);
            section_tag[0] = 0;
            section_tag[1] = 0;
//            section_tag[8] = section_data1;
//            section_tag[9] = section_data2;
            flash_write_data(section_tag,PSEC_40_CRC_IMAGE_A_ADDR+0x20,32);	
        }
        else
        {
//            section_data1 = section_tag[0];
//            section_data2 = section_tag[1];
            section_tag[0] = 0xa5;
            section_tag[1] = 0x5a;
			//bin文件中这里是固定0xff5a,这里才能生效才能设置为5aa5,要不就不是这个值会出问题
            flash_write_data(section_tag,PSEC_40_CRC_IMAGE_A_ADDR+0x20,32);	
            Delay(100);
            section_tag[0] = 0;
            section_tag[1] = 0;
//            section_tag[0] = section_data1;
//            section_tag[1] = section_data2;
            flash_write_data(section_tag,PSEC_40_CRC_IMAGE_B_ADDR+0x20,32);		
        }

		u_int16 test;
		u_int16 test_lll = 0xa18f;
		//flash_write_data(&test_lll,PSEC_40_CRC_IMAGE_B_ADDR+0x20,2);	
		flash_read_data(&test,PSEC_40_CRC_IMAGE_B_ADDR+0x20,2);	

		os_printf("*********	get_software_mark(0):%04x	get_software_mark(1):%04x   %04x\r\n",
		get_software_mark(0),get_software_mark(1),test);
		
        oad_finished(1); // 1:SUCCESS 0:FAILED
        flag_ota_beginning = 0;
        OTA_PRINT( "+++++++++++++++ OTA SUCCESS +++++++++++++++\r\n");
    }
    else
    {
        if(sec_ptr->left_boundary.imgback_addr == PSEC_40_CRC_IMAGE_B_ADDR)
        {
			section_tag[0] = 0;
			section_tag[1] = 0xff;
            flash_write_data(section_tag,PSEC_40_CRC_IMAGE_B_ADDR+0x20,32);	
        }
        else
        {
            section_tag[0] = 0;
            section_tag[1] = 0xff;
            flash_write_data(section_tag,PSEC_40_CRC_IMAGE_A_ADDR+0x20,32);	
        }

        oad_finished(0); // 1:SUCCESS 0:FAILED
        flag_ota_beginning = 0;
        OTA_PRINT( "--------------- OTA fail ---------------\r\n");
    }


    return 0;
}




//SPP
#ifdef BEKEN_SPP_OTA_3266
extern __INLINE__ unsigned char spp_is_connected(void);
RAM_CODE void BK3266_ota_updating_spp_ms_Sched(void)  //SPP
{
    extern u_int8 ota_data_tmp[], ota_data_count;
    extern u_int8 ota_end_flag;
    extern volatile uint16 flag_ota_wdt_rst;
    u_int32 cpu_flags, mask;

    if(!spp_is_connected())
        return ;
    if((ota_data_count > 5) || (1 == ota_end_flag))
    {
        SYSirq_Disable_Interrupts_Save_Flags(&cpu_flags, &mask);
        udi_updating_BK3266_section(ota_data_tmp, ota_data_count * OTA_PACKET_SIZE);


#ifdef BEKEN_BLE_OTA_3266_POWER_FAIL
		oadsize_4K += ota_data_count;
		if(oadsize_4K > OTA_PACKET_NUM_4K)//128*32 = 4k
		{
			os_printf("#######	  OTA	4k write	#######\r\n");
			oadPreBlkNum++;
			udi_updating_BK3266_BLK(&oadPreBlkNum);
			oadsize_4K -= OTA_PACKET_NUM_4K;
		}
#endif

        ota_data_count = 0;

        if(1 == ota_end_flag)
        {
            udi_updating_BK3266_end();
            ota_end_flag = 0;
            flag_ota_wdt_rst = 500; // 5s
            
#ifdef BEKEN_BLE_OTA_3266_POWER_FAIL
			oadPreBlkNum = 0;
			oadPreCrc = 0;
			oadPreBlkFlag = 0;
			udi_updating_BK3266_FLAG(&oadPreBlkFlag);
#endif
        }
		SYSirq_Interrupts_Restore_Flags(cpu_flags, mask);
    }
}
#endif



//BLE
#ifdef BEKEN_BLE_OTA_3266
void BK3266_ota_updating_ble_Sched(void) // BLE
{
	u_int32 cpu_flags, mask;
    extern u_int8 ota_data_tmp[], ota_data_count;
    extern u_int8 ota_end_flag;
    extern volatile uint16 flag_ota_wdt_rst;

	
    if((ota_data_count > 5) || (1 == ota_end_flag))
    {
    	SYSirq_Disable_Interrupts_Save_Flags(&cpu_flags, &mask);
        udi_updating_BK3266_section(ota_data_tmp, ota_data_count * OTA_PACKET_SIZE);


#ifdef BEKEN_BLE_OTA_3266_POWER_FAIL
		oadsize_4K += ota_data_count;
		if(oadsize_4K > OTA_PACKET_NUM_4K)//128*32 = 4k
		{
			os_printf("#######	  OTA	4k write	#######\r\n");
			oadPreBlkNum++;
			udi_updating_BK3266_BLK(&oadPreBlkNum);
			oadsize_4K -= OTA_PACKET_NUM_4K;
		}
#endif

        ota_data_count = 0;

        if(1 == ota_end_flag)
        {
            udi_updating_BK3266_end();
            ota_end_flag = 0;
            flag_ota_wdt_rst = 500; // 5s
            
#ifdef BEKEN_BLE_OTA_3266_POWER_FAIL
			oadPreBlkNum = 0;
			oadPreCrc = 0;
			oadPreBlkFlag = 0;
			udi_updating_BK3266_FLAG(&oadPreBlkFlag);
#endif
        }
		SYSirq_Interrupts_Restore_Flags(cpu_flags, mask);
    }
}
#endif






#else
void BK3266_ota_updating_ble_Sched(void) // BLE
{

}

#endif // BEKEN_UP_OTA_3266


