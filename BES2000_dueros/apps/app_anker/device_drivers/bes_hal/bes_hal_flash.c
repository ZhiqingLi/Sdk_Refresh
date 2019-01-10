#include "hal_norflaship.h"
#include "hal_norflash.h"
#include "string.h"
#include "md5.h"
#include "bes_hal_flash.h"

#include "plat_addr_map.h"
#include "hal_cmu.h"
#include "hal_sysfreq.h"


uint8_t bes_hal_norflash_init(void)
{
#if 1
    uint8_t ret = 0; 
    struct HAL_NORFLASH_CONFIG_T cfg = {
        #ifdef FPGA
        .source_clk = HAL_NORFLASH_SPEED_13M * 2,
        .speed = HAL_NORFLASH_SPEED_13M,
        #else
        .source_clk = HAL_NORFLASH_SPEED_104M * 2,
        .speed = HAL_NORFLASH_SPEED_104M,
        #endif
        .mode  = (enum DRV_NORFLASH_OP_MODE)(DRV_NORFLASH_OP_MODE_QUAD | DRV_NORFLASH_OP_MODE_CONTINUOUS_READ),
        .override_config = 0,
    };

    // Workaround for reboot: controller in SPI mode while FLASH in QUAD mode
    *(volatile uint32_t *)FLASH_NC_BASE;

    hal_cmu_pll_enable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_SYS);
    hal_cmu_flash_select_pll(HAL_CMU_PLL_AUD);
#if defined(CHIP_BEST1000) && defined(AUD_PLL_DOUBLE)
    hal_cmu_flash_set_freq(HAL_CMU_FREQ_104M);
#else
    hal_cmu_flash_set_freq(HAL_CMU_FREQ_208M);
#endif

    ret = hal_norflash_open(HAL_NORFLASH_ID_0, &cfg);

    return ret;
#else
//    hal_cmu_flash_set_freq(HAL_CMU_FREQ_26M);
    return hal_norflash_init();
#endif
}

uint8_t bes_hal_norflash_deinit(void)
{
    return hal_norflash_close(HAL_NORFLASH_ID_0);
}

int32_t voice_input_erase_partition(enum VOICE_INPUT_CODE_PARTITION_T partitionIndex)
{
    uint32_t startAddr = 0;
    uint32_t len = 0;

    switch(partitionIndex)
    {
        case BOOTLOADER_PARTITION_A:
            startAddr = VOICE_INPUT_BOOTLOADER_PARTITION_A_ADDR;
            len = VOICE_INPUT_BOOTLOADER_PARTITION_SIZE;
            break;
        case BOOTLOADER_PARTITION_B:
            startAddr = VOICE_INPUT_BOOTLOADER_PARTITION_B_ADDR;
            len = VOICE_INPUT_BOOTLOADER_PARTITION_SIZE;
            break;
        case FIRMWARE_PARTITION_A:
            startAddr = VOICE_INPUT_FIRMWARE_PARTITION_A_ADDR;
            len = VOICE_INPUT_FIRMWARE_PARTITION_SIZE;
            break;
        case FIRMWARE_PARTITION_B:
            startAddr = VOICE_INPUT_FIRMWARE_PARTITION_B_ADDR;
            len = VOICE_INPUT_FIRMWARE_PARTITION_SIZE;
            break;
        default:
            return PARTITION_ERR;
    }

    hal_norflash_erase(HAL_NORFLASH_ID_0,startAddr,len);
    return PARTITION_NOERR;
}

int32_t voice_input_write_head_partition(enum VOICE_INPUT_CODE_PARTITION_T partitionIndex, void* pHeader)
{
    uint32_t startAddr = 0;
    uint32_t len = 0;

    switch(partitionIndex)
    {
        case BOOTLOADER_PARTITION_A:
            startAddr = VOICE_INPUT_BOOTLOADER_PARTITION_A_ADDR;
            len = sizeof(voice_input_partition_head_t);
            break;
        case BOOTLOADER_PARTITION_B:
            startAddr = VOICE_INPUT_BOOTLOADER_PARTITION_B_ADDR;
            len = sizeof(voice_input_partition_head_t);
            break;
        case FIRMWARE_PARTITION_A:
            startAddr = VOICE_INPUT_FIRMWARE_PARTITION_A_ADDR;
            len = sizeof(voice_input_partition_head_t);
            break;
        case FIRMWARE_PARTITION_B:
            startAddr = VOICE_INPUT_FIRMWARE_PARTITION_B_ADDR;
            len = sizeof(voice_input_partition_head_t);
            break;
        default:
            return PARTITION_ERR;
    }

    // write partition head area for firmware len, md5info, firmware version etc.
    hal_norflash_write(HAL_NORFLASH_ID_0,startAddr,(const uint8_t*)pHeader,len);
    return PARTITION_NOERR;
}

int32_t voice_input_read_head_partition(enum VOICE_INPUT_CODE_PARTITION_T partitionIndex, void* pHeader)
{
    uint32_t startAddr = 0;
    uint32_t len = 0;

    switch(partitionIndex)
    {
        case BOOTLOADER_PARTITION_A:
            startAddr = VOICE_INPUT_BOOTLOADER_PARTITION_A_ADDR;
            len = sizeof(voice_input_partition_head_t);
            break;
        case BOOTLOADER_PARTITION_B:
            startAddr = VOICE_INPUT_BOOTLOADER_PARTITION_B_ADDR;
            len = sizeof(voice_input_partition_head_t);
            break;
        case FIRMWARE_PARTITION_A:
            startAddr = VOICE_INPUT_FIRMWARE_PARTITION_A_ADDR;
            len = sizeof(voice_input_partition_head_t);
            break;
        case FIRMWARE_PARTITION_B:
            startAddr = VOICE_INPUT_FIRMWARE_PARTITION_B_ADDR;
            len = sizeof(voice_input_partition_head_t);
            break;
        default:
            return PARTITION_ERR;
    }

    // write partition head area for firmware len, md5info, firmware version etc.
    hal_norflash_read(HAL_NORFLASH_ID_0,startAddr,(uint8_t*)pHeader,len);
    return PARTITION_NOERR;
}

int32_t voice_input_write_partition(enum VOICE_INPUT_CODE_PARTITION_T partitionIndex, uint32_t* pWriteAddr, uint8_t* pbuf, uint32_t len)
{
    uint32_t startAddr = 0;

    if(partitionIndex >= FIRMWARE_PARTITION_MAX)
        return PARTITION_ERR;

    if(*pWriteAddr == 0)
    {
        switch(partitionIndex)
        {
            case BOOTLOADER_PARTITION_A:
                startAddr = VOICE_INPUT_BOOTLOADER_PARTITION_A_ADDR + VOICE_INPUT_BOOTLOADER_PARTITION_HEAD_SIZE;
                break;
            case BOOTLOADER_PARTITION_B:
                startAddr = VOICE_INPUT_BOOTLOADER_PARTITION_B_ADDR + VOICE_INPUT_BOOTLOADER_PARTITION_HEAD_SIZE;
                break;
            case FIRMWARE_PARTITION_A:
                startAddr = VOICE_INPUT_FIRMWARE_PARTITION_A_ADDR + VOICE_INPUT_FIRMWARE_PARTITION_HEAD_SIZE;
                break;
            case FIRMWARE_PARTITION_B:
                startAddr = VOICE_INPUT_FIRMWARE_PARTITION_B_ADDR + VOICE_INPUT_FIRMWARE_PARTITION_HEAD_SIZE;
                break;
            default:
                return PARTITION_ERR;
        }
    }
    else
    {
        startAddr = *pWriteAddr;
    }

    hal_norflash_write(HAL_NORFLASH_ID_0,startAddr,pbuf,len);
    *pWriteAddr = startAddr + len;
    return PARTITION_NOERR;
}

int32_t voice_input_read_partition(enum VOICE_INPUT_CODE_PARTITION_T partitionIndex, uint32_t* pReadAddr, uint8_t* pbuf, uint32_t len)
{
    uint32_t startAddr = 0;

    if(partitionIndex >= FIRMWARE_PARTITION_MAX)
        return PARTITION_ERR;

    if(*pReadAddr == 0)
    {
        switch(partitionIndex)
        {
            case BOOTLOADER_PARTITION_A:
                startAddr = VOICE_INPUT_BOOTLOADER_PARTITION_A_ADDR + VOICE_INPUT_BOOTLOADER_PARTITION_HEAD_SIZE;
                break;
            case BOOTLOADER_PARTITION_B:
                startAddr = VOICE_INPUT_BOOTLOADER_PARTITION_B_ADDR + VOICE_INPUT_BOOTLOADER_PARTITION_HEAD_SIZE;
                break;
            case FIRMWARE_PARTITION_A:
                startAddr = VOICE_INPUT_FIRMWARE_PARTITION_A_ADDR + VOICE_INPUT_FIRMWARE_PARTITION_HEAD_SIZE;
                break;
            case FIRMWARE_PARTITION_B:
                startAddr = VOICE_INPUT_FIRMWARE_PARTITION_B_ADDR + VOICE_INPUT_FIRMWARE_PARTITION_HEAD_SIZE;
                break;
            default:
                return PARTITION_ERR;
        }
    }
    else
    {
        startAddr = *pReadAddr;
    }

    hal_norflash_read(HAL_NORFLASH_ID_0,startAddr,pbuf,len);
    *pReadAddr = startAddr + len;
    return PARTITION_NOERR;
}



int32_t voice_input_md5_check(enum VOICE_INPUT_CODE_PARTITION_T partitionIndex)
{
    #define TMPBUF_LEN  (512)
    MD5_CTX md5;
    voice_input_partition_head_t bootloaderHeader;
    voice_input_partition_head_t firmwareHeader;
    uint8_t md5Info[MD5_INFO_LEN+1] = {0};
    uint8_t decrypt[MD5_INFO_LEN/2+1] = {0};
    uint8_t decMd5Info[MD5_INFO_LEN+1] = {0};

    uint8_t tempBuf[TMPBUF_LEN] = {0};
    uint8_t highVal = 0, lowVal = 0;
    uint32_t readAddr = 0;
    uint32_t codeLen = 0;
    uint32_t readLen = 0, readSize = 0;
    uint32_t i = 0;
    int32_t ret = 0;
    
    if(partitionIndex >= FIRMWARE_PARTITION_MAX)
        return PARTITION_ERR;

    if(partitionIndex < FIRMWARE_PARTITION_A)
    {
        ret = voice_input_read_head_partition(partitionIndex,(void*)&bootloaderHeader);

        codeLen = bootloaderHeader.payloadLen[0];
        codeLen = (codeLen<<8) | bootloaderHeader.payloadLen[1];
        codeLen = (codeLen<<8) | bootloaderHeader.payloadLen[2];
        codeLen = (codeLen<<8) | bootloaderHeader.payloadLen[3];
        memcpy((void*)md5Info,(void*)bootloaderHeader.md5Info,VOICE_INPUT_MD5_INFO_LEN);
    }
    else
    {
        ret = voice_input_read_head_partition(partitionIndex,(void*)&firmwareHeader);

        codeLen = firmwareHeader.payloadLen[0];
        codeLen = (codeLen<<8) | firmwareHeader.payloadLen[1];
        codeLen = (codeLen<<8) | firmwareHeader.payloadLen[2];
        codeLen = (codeLen<<8) | firmwareHeader.payloadLen[3];
        memcpy((void*)md5Info,(void*)firmwareHeader.md5Info,VOICE_INPUT_MD5_INFO_LEN);
    }
    

	MD5Init(&md5);         		
    
    while(readLen < codeLen)
    {
        if(readLen + TMPBUF_LEN < codeLen)
            readSize = TMPBUF_LEN;
        else
            readSize = codeLen - readLen;

        voice_input_read_partition(partitionIndex,&readAddr,tempBuf,readSize);
	    MD5Update(&md5,tempBuf,readSize);
        readLen += readSize;
    }
	MD5Final(&md5,decrypt); 

    for(i = 0; i < MD5_INFO_LEN/2; ++i)
    {
        highVal = ((decrypt[i] >> 4) & 0x0f);
        lowVal = (decrypt[i] & 0x0f);
        *(decMd5Info+i*2) = (highVal > 9) ? ('A' + (highVal - 0x0A)) : ('0' + highVal);
        *(decMd5Info+i*2+1) = (lowVal > 9) ? ('A' + (lowVal - 0x0A)) : ('0' + lowVal);
    }

    ret = strncmp((const char*)decMd5Info, (const char*)md5Info, MD5_INFO_LEN);

    return ret == 0 ? 1 : 0;
}