#ifndef _DRIVER_SPI_H_
#define _DRIVER_SPI_H_

#define     FLASH_WREN_CMD      0x06        //write enable
#define     FLASH_WRDI_CMD      0x04        //write disable
#define     FLASH_RDSR_LOW_CMD  0x05        //read status REG(S7-S0)
#define     FLASH_RDSR_HIGH_CMD 0x35        //read status REG(S15-S8)
#define     FLASH_WRSR_LOW_CMD  0x01        //write status REG(S15-S0)
#define     FLASH_READ_CMD      0x03        //read data bytes
#define     FLASH_PP_CMD        0x02        //page program (256 bytes)
#define     FLASH_SE_CMD        0x20        //sector erase (4K bytes)
#define     FLASH_32K_BE_CMD    0x52        //block erase (32K bytes)
#define     FLASH_64K_BE_CMD    0xD8        //block erase (64K bytes)
#define     FLASH_CE_CMD        0x60        //chip erase
#define     FLASH_DP_CMD        0xB9        //deep power down
#define     FLASH_RDI_CMD       0xAB        //release DP & read device ID
#define     FLASH_REMS_CMD      0x90        //read manufacture & device ID
#define     FLASH_RDID_CMD      0x9F        //read manufacture & memory type & capacity
#define     FLASH_PES_CMD       0x75        //program/erase suspend
#define     FLASH_PER_CMD       0x7A        //program/erase resume
#define     FLASH_ESR_CMD       0x44        //erase security REG
#define     FLASH_PSR_CMD       0x42        //program security REG
#define     FLASH_SSR_CMD       0x48        //read security REG

#define     WINBOND_PAGE_SIZE   0x100       //winbond flash page size(256bytes)
#define     WINBOND_SECTOR_SIZE 0x1000      //winbond flash sector size(4K bytes)

typedef struct __SPI_WR_STRU__
{
    unsigned char header[8];
    unsigned int  headLen;
    unsigned int  dataLen;    
}SPI_WR_STRU;

void BKSpiInit(void);
void BKSpiRead(SPI_WR_STRU* input, unsigned char *pData);
void BKSpiWrite(SPI_WR_STRU* input, unsigned char *pData);
void FlashInIdle(void);
void BKFlashErase(unsigned char cmd,  unsigned int addr);
void BKFlashRead(unsigned int addr, unsigned char* pOutData, unsigned int len);
void BKFlashWrite(unsigned int addr, unsigned char* pInData);
void BKFlashWriteStatusReg(unsigned char cmd, unsigned char* val);
unsigned char BKFlashReadStatusReg(unsigned char cmd);
void BKFlashWriteCtrl(unsigned char cmd);
void BK_flash_spi_test(void);


#endif
