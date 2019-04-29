#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"

#ifdef CONFIG_DRIVER_SPI

void BKSpiInit(void)
{
    BK3000_PMU_PERI_PWDS &= ~bit_PMU_SPI_PWD;             //open spi2 clock
    (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+6*4))) = 0x40;
    (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+7*4))) = 0x40;
    (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+8*4))) = 0x40;
    (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+9*4))) = 0x40;	   //SPI 2nd func enable
}

void BKSpiRead(SPI_WR_STRU* input, unsigned char *pData)
{
	unsigned char 	i, DataNum;
    
	BK3000_SPI_SPI_CONF  = 0x00C20f00;
    for(i=0; i < input->headLen; i++)
    {
        BK3000_SPI_SPI_DATA = input->header[i];
    }    
	BK3000_SPI_SPI_CONF |= 0x00000040;	        //open txint_en
	while(!(BK3000_SPI_SPI_STAT & 0x100));       //wait spi transfer finish
    
    for(i=0; i < input->headLen; i++)
	    DataNum = BK3000_SPI_SPI_DATA;			//dummy byte

	while(input->dataLen)
	{
		DataNum = MIN(input->dataLen, 15);
		for(i = 0; i < DataNum; i ++)
		{
			BK3000_SPI_SPI_DATA = 0x0;
		}
		BK3000_SPI_SPI_STAT = 0x100;					//clear tx int
		while(!(BK3000_SPI_SPI_STAT & 0x100));       //wait spi transfer finish
        os_printf("read2 ");
		for(i = 0; i < DataNum; i ++)
		{
			*pData = BK3000_SPI_SPI_DATA & 0xff;
			pData++;
		} 
		input->dataLen -= DataNum;
	}
    BK3000_SPI_SPI_CONF = 0x00430f00;		//nss0 set, close enable
}

void BKSpiWrite(SPI_WR_STRU* input, unsigned char *pData)
{
	unsigned char i=0;
    unsigned char DataNum;	

 	BK3000_SPI_SPI_CONF  = 0x00C20f00;
    for(i=0; i < input->headLen; i++)
    {
        BK3000_SPI_SPI_DATA   = input->header[i];
    }
	BK3000_SPI_SPI_CONF |= 0x00000040;	        //open txint_en
	while(!(BK3000_SPI_SPI_STAT & 0x100));		//wait spi transfer finish
    os_printf("write1 ");
	while(input->dataLen)
	{
		DataNum = MIN(input->dataLen, 15);
		for(i = 0; i < DataNum; i ++)
		{
			BK3000_SPI_SPI_DATA = *pData;
			pData++;
		}
		BK3000_SPI_SPI_STAT = 0x100;					 //clear tx int
		while(!(BK3000_SPI_SPI_STAT & 0x100));        //wait spi transfer finish
        os_printf("write2 ");
        input->dataLen -= DataNum;
	}
    BK3000_SPI_SPI_CONF = 0x00430f00;		        //nss0 set, close enable
}

 void FlashDummy(void)
{
    unsigned char 	status;
    
 	BK3000_SPI_SPI_CONF  = 0x00C20f00;
    BK3000_SPI_SPI_DATA   = FLASH_RDSR_LOW_CMD;
	BK3000_SPI_SPI_CONF |= 0x00000040;	        //open txint_en
	while(!(BK3000_SPI_SPI_STAT & 0x100));       //wait spi transfer finish
    status = BK3000_SPI_SPI_DATA;				 //dummy byte;
    BK3000_SPI_SPI_DATA = 0;
    BK3000_SPI_SPI_STAT = 0x100;
    while(!(BK3000_SPI_SPI_STAT & 0x100));
    status = BK3000_SPI_SPI_DATA & 0xff;
    BK3000_SPI_SPI_CONF = 0x00430f00;		    //nss0 set, close enable    
}

 void FlashInIdle(void)
{
    unsigned char 	status;
    
 	BK3000_SPI_SPI_CONF  = 0x00C20f00;
    BK3000_SPI_SPI_DATA   = FLASH_RDSR_LOW_CMD;
	BK3000_SPI_SPI_CONF |= 0x00000040;	        //open txint_en
	while(!(BK3000_SPI_SPI_STAT & 0x100));       //wait spi transfer finish
    status = BK3000_SPI_SPI_DATA;				 //dummy byte;
    do                       
    {
        BK3000_SPI_SPI_DATA = 0;
        BK3000_SPI_SPI_STAT = 0x100;
        while(!(BK3000_SPI_SPI_STAT & 0x100));
        
        status = BK3000_SPI_SPI_DATA &0xff; 
    }while(status& 0x01);
    BK3000_SPI_SPI_CONF = 0x00430f00;		    //nss0 set, close enable   
}

void BKFlashWriteCtrl(unsigned char cmd)
{
    unsigned char val;
    SPI_WR_STRU input;
    input.headLen = 1;
    input.header[0] = cmd;
    input.dataLen = 0;
    BKSpiWrite(&input, (unsigned char*)&val);  
}

void BKFlashErase(unsigned char cmd,  unsigned int addr)
{
    SPI_WR_STRU input;
    unsigned char* pInput;

    BKFlashWriteCtrl(FLASH_WREN_CMD);
    memset(&input, 0, sizeof(input));
    input.headLen = 1;
    pInput = (unsigned char*)&input;     //dummy input data
        
    if(cmd != FLASH_CE_CMD)
    {
        input.headLen = 4;
        input.header[1]= (addr>>16)&0xff;
        input.header[2] = (addr>>8)&0xff;
        input.header[3] = addr&0xff;            
    }

    input.header[0] = cmd;    
    BKSpiWrite(&input, pInput);         //dummy input data
    FlashInIdle();
}

void BKFlashRead(unsigned int addr, unsigned char* pOutData, unsigned int len)
{
    SPI_WR_STRU input;

    input.headLen = 4;
    input.header[0] = FLASH_READ_CMD;
    input.header[1]= (addr>>16)&0xff;
    input.header[2] = (addr>>8)&0xff;
    input.header[3] = addr&0xff;            
    input.dataLen = len;
    BKSpiRead(&input, pOutData);
}

void BKFlashWrite(unsigned int addr, unsigned char* pInData)
{
    SPI_WR_STRU input;

    memset(&input, 0, sizeof(input));
    BKFlashWriteCtrl(FLASH_WREN_CMD);
    input.headLen = 4;
    input.header[0] = FLASH_PP_CMD;
    input.header[1]= (addr>>16)&0xff;
    input.header[2] = (addr>>8)&0xff;
    input.header[3] = addr&0xff;            
    input.dataLen = WINBOND_PAGE_SIZE;
    BKSpiWrite(&input, pInData);
    FlashInIdle();
}

unsigned char BKFlashReadStatusReg(unsigned char cmd)
{
    unsigned char status;
    SPI_WR_STRU input;
    input.headLen = 1;
    input.header[0] = cmd;
    input.dataLen = 1;
    BKSpiRead(&input, (unsigned char*)&status);
    return status;    
}

void BKFlashWriteStatusReg(unsigned char cmd, unsigned char* val)
{
    unsigned char status;
    SPI_WR_STRU input;
    BKFlashWriteCtrl(FLASH_WREN_CMD);
    input.headLen = 1;
    input.header[0] = cmd;
    input.dataLen = 2;
    BKSpiWrite(&input, (unsigned char*)val);
    FlashInIdle();
}

CONST unsigned char flash_test_data[WINBOND_PAGE_SIZE] =
{
0x00,0x80,0x40,0xc0,0x20,0xa0,0x60,0xe0,0x10,0x90,0x50,0xd0,0x30,0xb0,0x70,0xf0,
0x08,0x88,0x48,0xc8,0x28,0xa8,0x68,0xe8,0x18,0x98,0x58,0xd8,0x38,0xb8,0x78,0xf8,
0x04,0x84,0x44,0xc4,0x24,0xa4,0x64,0xe4,0x14,0x94,0x54,0xd4,0x34,0xb4,0x74,0xf4,
0x0c,0x8c,0x4c,0xcc,0x2c,0xac,0x6c,0xec,0x1c,0x9c,0x5c,0xdc,0x3c,0xbc,0x7c,0xfc,
0x02,0x82,0x42,0xc2,0x22,0xa2,0x62,0xe2,0x12,0x92,0x52,0xd2,0x32,0xb2,0x72,0xf2,
0x0a,0x8a,0x4a,0xca,0x2a,0xaa,0x6a,0xea,0x1a,0x9a,0x5a,0xda,0x3a,0xba,0x7a,0xfa,
0x06,0x86,0x46,0xc6,0x26,0xa6,0x66,0xe6,0x16,0x96,0x56,0xd6,0x36,0xb6,0x76,0xf6,
0x0e,0x8e,0x4e,0xce,0x2e,0xae,0x6e,0xee,0x1e,0x9e,0x5e,0xde,0x3e,0xbe,0x7e,0xfe,
0x01,0x81,0x41,0xc1,0x21,0xa1,0x61,0xe1,0x11,0x91,0x51,0xd1,0x31,0xb1,0x71,0xf1,
0x09,0x89,0x49,0xc9,0x29,0xa9,0x69,0xe9,0x19,0x99,0x59,0xd9,0x39,0xb9,0x79,0xf9,
0x05,0x85,0x45,0xc5,0x25,0xa5,0x65,0xe5,0x15,0x95,0x55,0xd5,0x35,0xb5,0x75,0xf5,
0x0d,0x8d,0x4d,0xcd,0x2d,0xad,0x6d,0xed,0x1d,0x9d,0x5d,0xdd,0x3d,0xbd,0x7d,0xfd,
0x03,0x83,0x43,0xc3,0x23,0xa3,0x63,0xe3,0x13,0x93,0x53,0xd3,0x33,0xb3,0x73,0xf3,
0x0b,0x8b,0x4b,0xcb,0x2b,0xab,0x6b,0xeb,0x1b,0x9b,0x5b,0xdb,0x3b,0xbb,0x7b,0xfb,
0x07,0x87,0x47,0xc7,0x27,0xa7,0x67,0xe7,0x17,0x97,0x57,0xd7,0x37,0xb7,0x77,0xf7,
0x0f,0x8f,0x4f,0xcf,0x2f,0xaf,0x6f,0xef,0x1f,0x9f,0x5f,0xdf,0x3f,0xbf,0x7f,0xff
};

#define  PAGE_NUM_IN_ONE_SECTOR          0x10

void BK_flash_spi_test(void)
{
    unsigned char output[WINBOND_PAGE_SIZE];
    unsigned int i,j;
    unsigned int addr = 0;
    FlashDummy();
    os_printf("set status reg = 0x0000\r\n");
    output[0]= 2;
    output[1]= 0;
    BKFlashWriteStatusReg(FLASH_WRSR_LOW_CMD, output);                       

    os_printf("status reg(low):");
    output[0] = BKFlashReadStatusReg(FLASH_RDSR_LOW_CMD);
    os_printf("%2x\r\n", output[0]);
    os_printf("status reg(high):");
    output[0] = BKFlashReadStatusReg(FLASH_RDSR_HIGH_CMD);
    os_printf("%2x\r\n", output[0]);

    os_printf("sector erase\r\n");
    BKFlashErase(FLASH_SE_CMD,  0);

    for(i=0; i<PAGE_NUM_IN_ONE_SECTOR; i++)
    {
        memset(output, 0, sizeof(output));
        BKFlashRead(i*WINBOND_PAGE_SIZE, output, WINBOND_PAGE_SIZE);
        os_printf("page %d\r\n", i);
        for(j=0; j<WINBOND_PAGE_SIZE; j++)
        {
            os_printf("%02x", output[j]);
            os_printf(" ");
        }
        os_printf("\r\n\r\n\r\n\r\n\r\n ");
    }

    os_printf("write the 1st sector\r\n");
    
    for(i=0; i<PAGE_NUM_IN_ONE_SECTOR; i++)
    {            
        BKFlashWrite(i*WINBOND_PAGE_SIZE, (unsigned char*)flash_test_data);            
    }

    os_printf("read the 1st sector\r\n");
    for(i=0; i<PAGE_NUM_IN_ONE_SECTOR; i++)
    {
        memset(output, 0, sizeof(output));
        BKFlashRead(i*WINBOND_PAGE_SIZE, output, WINBOND_PAGE_SIZE);
        os_printf("page %d\r\n", i);
        for(j=0; j<WINBOND_PAGE_SIZE; j++)
        {
            os_printf("%02x", output[j]);
            os_printf(" ");
        }
        os_printf("\r\n\r\n\r\n\r\n\r\n ");
    } 
}

#endif
