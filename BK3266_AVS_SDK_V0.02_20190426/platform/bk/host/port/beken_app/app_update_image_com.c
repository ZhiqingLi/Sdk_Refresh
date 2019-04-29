#include "app_update_image_com.h"

#ifdef BEKEN_UP_OTA_3266
#include "app_Oad.h"
#include "driver_flash.h"
#include "app_Oad_target.h"


UDI_SECTION_T psec_A;
UDI_SECTION_T psec_B;


#ifdef OAD_IMAGE_A
UDI_SECTION_T bsec_B;
#endif //OAD_IMAGE_A

#ifdef OAD_IMAGE_B
UDI_SECTION_T bsec_A;
#endif //OAD_IMAGE_B 

/*****************************************************************************************************/
/**************************************CFG_UPDATE_IMAGE***********************************************/
/*****************************************************************************************************/

//uint32_t crc32_table[256];
int make_crc32_table(uint32_t *p_crc32_table)
{
    uint32_t c;
    int i = 0;
    int bit = 0;
    for(i = 0; i < 256; i++)
    {
        c = (uint32_t)i;
        for(bit = 0; bit < 8; bit++)
        {
            if(c&1)
            {
                c = (c>>1)^(0xEDB88320);
            }
            else
            {
                c = c >> 1;
            }
        }
        p_crc32_table[i] = c;
    }
    return 0;
}

uint32_t make_crc32(uint32_t crc,unsigned char *string,uint32_t size, uint32_t *p_crc32_table)
{
    while(size--)
    {
        crc = (crc >> 8)^(p_crc32_table[(crc^*string++)&0xff]);
    }
    return crc;
}

uint32_t calc_backup_sec_crc(void)
{
	uint32_t crc32_table[256];

	UDI_SECTION_PTR sec_ptr;
#ifdef  OAD_IMAGE_A
	sec_ptr = &bsec_B;
#endif
	uint32_t i;
	uint8_t data[OTA_PACKET_SIZE];
	uint32_t block_total;
	uint32_t read_addr;
	uint32_t calcuCrc = 0xffffffff;
	
	make_crc32_table(&crc32_table[0]);
	
#if(LEN_SIZE_UINT == 16)
	block_total = sec_ptr->left_boundary.sec_size - 1;
#else
	block_total = sec_ptr->left_boundary.sec_size / 4 - 1;
#endif
	read_addr = sec_ptr->left_boundary.imgback_addr;
	
	os_printf("block_total= %x\r\n", bsec_B.left_boundary.sec_size);
	
	for (i = 0; i < block_total; i++)
	{
		flash_read_data(data, read_addr, OTA_PACKET_SIZE);


#if 0
			os_printf("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\n",
                data[0],data[1],data[2],data[3],data[4],data[5],
                data[6],data[7],data[8],data[9],data[10],data[11],
                data[12],data[13],data[14],data[15],data[16],data[17],
                data[18],data[19],data[20],data[21],data[22],data[23],
                data[24],data[25],data[26],data[27],data[28],data[29],
                data[30],data[31]);

			//os_printf( "BK3266_OTA_END: crc = %08x\r\n", calcuCrc);
#endif

		calcuCrc = make_crc32(calcuCrc, data, OTA_PACKET_SIZE, &crc32_table[0]);
		read_addr += OTA_PACKET_SIZE;
	}
	
	os_printf("CRC ADDR : start = 0x%x, end = 0x%x\r\n", sec_ptr->left_boundary.imgback_addr, read_addr);

	return calcuCrc;
}


#endif // BEKEN_UP_OTA_3266

