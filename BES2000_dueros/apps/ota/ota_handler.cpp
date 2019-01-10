
#if OTA_ENABLED

#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_timer.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "string.h"
#include "crc32.h"
#include "pmu.h"
#include "hal_norflash.h"
#include "ota_handler.h"
#include "nvrecord.h"
#include "hal_cache.h"
#include "cmsis_nvic.h"
#include "hal_bootmode.h"
#include "ota_common.h"

extern uint32_t __factory_start[];
static OTA_ENV_T    ota_env;
static uint32_t     user_data_nv_flash_offset;
#define OTA_BOOT_INFO_FLASH_OFFSET                                                       0x1000
#define otaUpgradeLogInFlash    (*(FLASH_OTA_UPGRADE_LOG_FLASH_T *)(OTA_FLASH_LOGIC_ADDR+0x3000))
#define otaUpgradeLog   otaUpgradeLogInFlash

static void ota_send_configuration_response(bool isDone);
static void ota_flush_data_to_flash(uint8_t* ptrSource, uint32_t lengthToBurn, uint32_t offsetInFlashToProgram);
static void ota_send_segment_verification_response(bool isPass);
static bool ota_check_whole_image_crc(void);
static void ota_send_result_response(uint8_t isSuccessful);

static void ota_update_nv_data(void)
{
    uint32_t lock;
    if (ota_env.configuration.isToClearUserData)
    {
        lock = int_lock();
        pmu_flash_write_config();
        hal_norflash_erase(HAL_NORFLASH_ID_0, OTA_FLASH_LOGIC_ADDR + ota_env.flasehOffsetOfUserDataPool,
                FLASH_SECTOR_SIZE_IN_BYTES);
        pmu_flash_read_config();
        int_unlock(lock);
    }

    if (ota_env.configuration.isToRenameBT || ota_env.configuration.isToRenameBLE ||
        ota_env.configuration.isToUpdateBTAddr || ota_env.configuration.isToUpdateBLEAddr)
    {
        uint32_t* pOrgFactoryData, *pUpdatedFactoryData;
        pOrgFactoryData = (uint32_t *)(OTA_FLASH_LOGIC_ADDR + ota_env.flasehOffsetOfFactoryDataPool);
        memcpy(ota_env.dataBufferForBurning, (uint8_t *)pOrgFactoryData,
            FLASH_SECTOR_SIZE_IN_BYTES);
        pUpdatedFactoryData = (uint32_t *)&(ota_env.dataBufferForBurning);

        uint32_t nv_record_dev_rev = pOrgFactoryData[dev_version_and_magic] >> 16;
        
        if (NVREC_DEV_VERSION_1 == nv_record_dev_rev)
        {
            if (ota_env.configuration.isToRenameBT)
            {
                memset((uint8_t *)(pUpdatedFactoryData + dev_name), 0, sizeof(uint32_t)*(dev_bt_addr - dev_name));
                memcpy((uint8_t *)(pUpdatedFactoryData + dev_name), (uint8_t *)(ota_env.configuration.newBTName),
                    NAME_LENGTH);
            }

            if (ota_env.configuration.isToUpdateBTAddr)
            {            
                memcpy((uint8_t *)(pUpdatedFactoryData + dev_bt_addr), (uint8_t *)(ota_env.configuration.newBTAddr),
                    BD_ADDR_LEN);
            }

            if (ota_env.configuration.isToUpdateBLEAddr)
            {
                memcpy((uint8_t *)(pUpdatedFactoryData + dev_ble_addr), (uint8_t *)(ota_env.configuration.newBLEAddr),
                    BD_ADDR_LEN);
            }

            pUpdatedFactoryData[dev_crc] = 
                crc32(0,(uint8_t *)(&pUpdatedFactoryData[dev_reserv1]),(dev_data_len-dev_reserv1)*sizeof(uint32_t));
        }
        else
        {
            if (ota_env.configuration.isToRenameBT) {
              memset((uint8_t *) (pUpdatedFactoryData + rev2_dev_name), 0,
                     sizeof(uint32_t) * (rev2_dev_bt_addr - rev2_dev_name));
              memcpy((uint8_t *) (pUpdatedFactoryData + rev2_dev_name),
                     (uint8_t *) (ota_env.configuration.newBTName),
                     NAME_LENGTH);
            }

            if (ota_env.configuration.isToRenameBLE) {
                memset((uint8_t *)(pUpdatedFactoryData + rev2_dev_ble_name), 
                    0, sizeof(uint32_t)*(rev2_dev_section_end - rev2_dev_ble_name));
                memcpy((uint8_t *)(pUpdatedFactoryData + rev2_dev_ble_name), 
                    (uint8_t *)(ota_env.configuration.newBLEName),
                    BLE_NAME_LEN_IN_NV);
            }

            if (ota_env.configuration.isToUpdateBTAddr) {
              memcpy((uint8_t *) (pUpdatedFactoryData + rev2_dev_bt_addr),
                     (uint8_t *) (ota_env.configuration.newBTAddr),
                     BD_ADDR_LEN);
            }

            if (ota_env.configuration.isToUpdateBLEAddr) {
              memcpy((uint8_t *) (pUpdatedFactoryData + rev2_dev_ble_addr),
                     (uint8_t *) (ota_env.configuration.newBLEAddr),
                     BD_ADDR_LEN);
            }

            pUpdatedFactoryData[rev2_dev_crc] = crc32(
                0, (uint8_t *) (&pUpdatedFactoryData[rev2_dev_section_start_reserved]),
                pUpdatedFactoryData[rev2_dev_data_len]);
        }

        lock = int_lock();
        pmu_flash_write_config();
        hal_norflash_erase(HAL_NORFLASH_ID_0, OTA_FLASH_LOGIC_ADDR + ota_env.flasehOffsetOfFactoryDataPool,
                FLASH_SECTOR_SIZE_IN_BYTES);

        hal_norflash_write(HAL_NORFLASH_ID_0, OTA_FLASH_LOGIC_ADDR + ota_env.flasehOffsetOfFactoryDataPool,
            (uint8_t *)pUpdatedFactoryData, FLASH_SECTOR_SIZE_IN_BYTES);
        pmu_flash_read_config();
        int_unlock(lock);        
    }
}

void ota_handler_init(void)
{
#ifdef __APP_USER_DATA_NV_FLASH_OFFSET__
    user_data_nv_flash_offset = __APP_USER_DATA_NV_FLASH_OFFSET__;
#else
    user_data_nv_flash_offset = hal_norflash_get_flash_total_size() - 2*4096;
#endif

    memset(&ota_env, 0, sizeof(ota_env));
    ota_reset_env();
}

void ota_update_MTU(uint16_t mtu)
{
    // remove the 3 bytes of overhead
    ota_env.dataPacketSize = mtu - 3;
    TRACE("updated data packet size is %d", ota_env.dataPacketSize);
}

void ota_reboot_to_use_new_image(void)
{
    TRACE("OTA data receiving is done successfully, systerm will reboot ");
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_ENTER_HIDE_BOOT);
    hal_cmu_sys_reboot();
}

void ota_disconnection_handler(void)
{
    if (ota_env.isPendingForReboot)
    {
        ota_reboot_to_use_new_image();
    }
    else
    {
        ota_reset_env();
    }      
}

void ota_reset_env(void)
{
    ota_env.configuration.startLocationToWriteImage = NEW_IMAGE_FLASH_OFFSET;
    ota_env.offsetInFlashToProgram = ota_env.configuration.startLocationToWriteImage;;
    ota_env.offsetInFlashOfCurrentSegment = ota_env.offsetInFlashToProgram;

    ota_env.configuration.isToClearUserData = true;
    ota_env.configuration.isToRenameBLE = false;
    ota_env.configuration.isToRenameBT = false;
    ota_env.configuration.isToUpdateBLEAddr = false;
    ota_env.configuration.isToUpdateBTAddr = false;
    ota_env.configuration.lengthOfFollowingData = 0;
    ota_env.AlreadyReceivedConfigurationLength = 0;
    ota_env.flasehOffsetOfUserDataPool = user_data_nv_flash_offset;
    ota_env.flasehOffsetOfFactoryDataPool = user_data_nv_flash_offset + FLASH_SECTOR_SIZE_IN_BYTES;
    ota_env.crc32OfSegment = 0;
    ota_env.crc32OfImage = 0;
    ota_env.offsetInDataBufferForBurning = 0;
    ota_env.alreadyReceivedDataSizeOfImage = 0;
    ota_env.offsetOfImageOfCurrentSegment = 0;
    ota_env.isOTAInProgress = false;
    ota_env.isPendingForReboot = false;

    ota_env.leftSizeOfFlashContentToRead = 0;

    if(ota_env.resume_at_breakpoint == false)
    {
        ota_env.breakPoint = 0;
        ota_env.i_log = -1;
    }
}

void ota_register_transmitter(ota_transmit_data_t transmit_handle)
{
    ota_env.transmitHander = transmit_handle;
}

void ota_set_datapath_type(OTA_DATAPATH_TYPE_E datapathType)
{
    ota_env.dataPathType = datapathType;
}

/** Program the data in the data buffer to flash
  *
  * @param[in] ptrSource  Pointer of the source data buffer to program.
  * @param[in] lengthToBurn  Length of the data to program.
  * @param[in] offsetInFlashToProgram  Offset in bytes in flash to program
  *
  * @return
  *
  * @note
  */
static void ota_flush_data_to_flash(uint8_t* ptrSource, uint32_t lengthToBurn, uint32_t offsetInFlashToProgram)
{
    uint32_t lock;

    TRACE("flush %d bytes to flash offset 0x%x", lengthToBurn, offsetInFlashToProgram);

    lock = int_lock();
    pmu_flash_write_config();

    uint32_t preBytes = (FLASH_SECTOR_SIZE_IN_BYTES - (offsetInFlashToProgram%FLASH_SECTOR_SIZE_IN_BYTES))%FLASH_SECTOR_SIZE_IN_BYTES;
    if (lengthToBurn < preBytes)
    {
        preBytes = lengthToBurn;
    }

    uint32_t middleBytes = 0;
    if (lengthToBurn > preBytes)
    {
       middleBytes = ((lengthToBurn - preBytes)/FLASH_SECTOR_SIZE_IN_BYTES*FLASH_SECTOR_SIZE_IN_BYTES);
    }
    uint32_t postBytes = 0;
    if (lengthToBurn > (preBytes + middleBytes))
    {
        postBytes = (offsetInFlashToProgram + lengthToBurn)%FLASH_SECTOR_SIZE_IN_BYTES;
    }

    TRACE("Prebytes is %d middlebytes is %d postbytes is %d", preBytes, middleBytes, postBytes);

    if (preBytes > 0)
    {
        hal_norflash_write(HAL_NORFLASH_ID_0, offsetInFlashToProgram, ptrSource, preBytes);

        ptrSource += preBytes;
        offsetInFlashToProgram += preBytes;
    }

    uint32_t sectorIndexInFlash = offsetInFlashToProgram/FLASH_SECTOR_SIZE_IN_BYTES;

    if (middleBytes > 0)
    {
        uint32_t sectorCntToProgram = middleBytes/FLASH_SECTOR_SIZE_IN_BYTES;
        for (uint32_t sector = 0;sector < sectorCntToProgram;sector++)
        {
            hal_norflash_erase(HAL_NORFLASH_ID_0, sectorIndexInFlash*FLASH_SECTOR_SIZE_IN_BYTES, FLASH_SECTOR_SIZE_IN_BYTES);
            hal_norflash_write(HAL_NORFLASH_ID_0, sectorIndexInFlash*FLASH_SECTOR_SIZE_IN_BYTES,
                ptrSource + sector*FLASH_SECTOR_SIZE_IN_BYTES, FLASH_SECTOR_SIZE_IN_BYTES);

            sectorIndexInFlash++;
        }

        ptrSource += middleBytes;
    }

    if (postBytes > 0)
    {
        hal_norflash_erase(HAL_NORFLASH_ID_0, sectorIndexInFlash*FLASH_SECTOR_SIZE_IN_BYTES, FLASH_SECTOR_SIZE_IN_BYTES);
        hal_norflash_write(HAL_NORFLASH_ID_0, sectorIndexInFlash*FLASH_SECTOR_SIZE_IN_BYTES,
                ptrSource, postBytes);
    }

	pmu_flash_read_config();
	int_unlock(lock);
}

static void ota_send_start_response(bool isViaBle)
{
    if (isViaBle)
    {
        ota_env.dataPacketSize = 
            (ota_env.dataPacketSize >= OTA_BLE_DATA_PACKET_MAX_SIZE)?OTA_BLE_DATA_PACKET_MAX_SIZE:ota_env.dataPacketSize;
    }
    else
    {
        ota_env.dataPacketSize = 
            (ota_env.dataPacketSize >= OTA_BT_DATA_PACKET_MAX_SIZE)?OTA_BT_DATA_PACKET_MAX_SIZE:ota_env.dataPacketSize;        
    }

    OTA_START_RSP_T tRsp = 
        {OTA_RSP_START, OTA_START_MAGIC_CODE, OTA_SW_VERSION, OTA_HW_VERSION, 
        ota_env.dataPacketSize};                
    
    ota_env.transmitHander((uint8_t *)&tRsp, sizeof(tRsp));    
}

static void ota_send_configuration_response(bool isDone)
{
    OTA_RSP_CONFIG_T tRsp = {OTA_RSP_CONFIG, isDone};
    ota_env.transmitHander((uint8_t *)&tRsp, sizeof(tRsp));
}

#if DATA_ACK_FOR_SPP_DATAPATH_ENABLED
static void ota_send_data_ack_response(void)
{
    uint8_t packeType = OTA_DATA_ACK;
    ota_env.transmitHander((uint8_t *)&packeType, sizeof(packeType));    
}
#endif

static void ota_send_segment_verification_response(bool isPass)
{
    TRACE("Segment of image's verification pass status is %d (1:pass 0:failed)", isPass);

    OTA_RSP_SEGMENT_VERIFY_T tRsp = {OTA_RSP_SEGMENT_VERIFY, isPass};
    ota_env.transmitHander((uint8_t *)&tRsp, sizeof(tRsp));
}

enum
{
    OTA_RESULT_ERR_RECV_SIZE = 2,
    OTA_RESULT_ERR_FLASH_OFFSET,
    OTA_RESULT_ERR_SEG_VERIFY,
    OTA_RESULT_ERR_BREAKPOINT,
    OTA_RESULT_ERR_IMAGE_SIZE,
};

static void ota_send_result_response(uint8_t isSuccessful)
{
    OTA_RSP_OTA_RESULT_T tRsp = {OTA_RSP_RESULT, isSuccessful};
    ota_env.transmitHander((uint8_t *)&tRsp, sizeof(tRsp));
}

static bool ota_check_whole_image_crc(void)
{
    uint32_t verifiedDataSize = 0;
    uint32_t crc32Value = 0;
    uint32_t verifiedBytes = 0;
    uint32_t lock;

    while (verifiedDataSize < ota_env.totalImageSize)
    {
        if (ota_env.totalImageSize - verifiedDataSize > OTA_DATA_BUFFER_SIZE_FOR_BURNING)
        {
            verifiedBytes = OTA_DATA_BUFFER_SIZE_FOR_BURNING;
        }
        else
        {
            verifiedBytes = ota_env.totalImageSize - verifiedDataSize;
        }

        lock = int_lock();

        hal_norflash_read(HAL_NORFLASH_ID_0, NEW_IMAGE_FLASH_OFFSET + verifiedDataSize,
            ota_env.dataBufferForBurning, OTA_DATA_BUFFER_SIZE_FOR_BURNING);

        int_unlock(lock);

        if (0 == verifiedDataSize)
        {
            if (*(uint32_t *)ota_env.dataBufferForBurning != NORMAL_BOOT)
            {
                return false;
            }
            else
            {
                *(uint32_t *)ota_env.dataBufferForBurning = 0xFFFFFFFF;
            }
        }

        crc32Value = crc32(crc32Value, (uint8_t *)ota_env.dataBufferForBurning, verifiedBytes);

        verifiedDataSize += verifiedBytes;
    }

    TRACE("Original CRC32 is 0x%x Confirmed CRC32 is 0x%x.", ota_env.crc32OfImage, crc32Value);
    if (crc32Value == ota_env.crc32OfImage)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool ota_is_in_progress(void)
{
    return ota_env.isOTAInProgress;
}

void ota_data_transmission_done_callback(void)
{
    if (ota_env.leftSizeOfFlashContentToRead > 0)
    {
        uint32_t sizeToRead = (128 > 
            ota_env.leftSizeOfFlashContentToRead)?
            ota_env.leftSizeOfFlashContentToRead:
            128;
        ota_env.bufForFlashReading[0] = OTA_FLASH_CONTENT_DATA;
        memcpy(&(ota_env.bufForFlashReading[1]), 
            (uint8_t *)(OTA_FLASH_LOGIC_ADDR+ota_env.offsetInFlashToRead),
            sizeToRead);

        TRACE("Send %d bytes at addr 0x%x", sizeToRead,
            ota_env.offsetInFlashToRead);
        ota_env.transmitHander(ota_env.bufForFlashReading, 
            sizeToRead + 1);

        ota_env.leftSizeOfFlashContentToRead -= sizeToRead;
        ota_env.offsetInFlashToRead += sizeToRead;
    }
}

static void ota_sending_flash_content(OTA_READ_FLASH_CONTENT_REQ_T* pReq)
{
    OTA_READ_FLASH_CONTENT_RSP_T rsp = {OTA_READ_FLASH_CONTENT, true};

    if (pReq->isToStart)
    {
        TRACE("Getreading flash content request start addr 0x%x size %d",
                    pReq->startAddr, pReq->lengthToRead);
        
        // check the sanity of the request
        if ((pReq->startAddr >= hal_norflash_get_flash_total_size()) ||
            ((pReq->startAddr + pReq->lengthToRead) > 
            hal_norflash_get_flash_total_size()))
        {
            rsp.isReadingReqHandledSuccessfully = false;
        }
        else
        {
            ota_env.offsetInFlashToRead = pReq->startAddr;
            ota_env.leftSizeOfFlashContentToRead = pReq->lengthToRead;
            TRACE("Start sending flash content start addr 0x%x size %d",
                pReq->startAddr, pReq->lengthToRead);
        }
    }
    else
    {
        TRACE("Get stop reading flash content request.");
        ota_env.leftSizeOfFlashContentToRead = 0;
    }
    
    ota_env.transmitHander((uint8_t *)&rsp, sizeof(rsp)); 
}

static void app_update_magic_number_of_app_image(uint32_t newMagicNumber)
{
    uint32_t lock;

    lock = int_lock();
    hal_norflash_read(HAL_NORFLASH_ID_0, ota_env.dstFlashOffsetForNewImage, 
        ota_env.dataBufferForBurning, FLASH_SECTOR_SIZE_IN_BYTES);
    int_unlock(lock); 
    
    *(uint32_t *)ota_env.dataBufferForBurning = newMagicNumber;
    
    lock = int_lock();
    pmu_flash_write_config();
    hal_norflash_erase(HAL_NORFLASH_ID_0, ota_env.dstFlashOffsetForNewImage,
        FLASH_SECTOR_SIZE_IN_BYTES);
    hal_norflash_write(HAL_NORFLASH_ID_0, ota_env.dstFlashOffsetForNewImage,
        ota_env.dataBufferForBurning, FLASH_SECTOR_SIZE_IN_BYTES);
    pmu_flash_read_config();
    int_unlock(lock);    
}

static void ota_update_boot_info(FLASH_OTA_BOOT_INFO_T* otaBootInfo)
{
    uint32_t lock;

    lock = int_lock();
    pmu_flash_write_config();
    hal_norflash_erase(HAL_NORFLASH_ID_0, OTA_BOOT_INFO_FLASH_OFFSET, FLASH_SECTOR_SIZE_IN_BYTES);
    hal_norflash_write(HAL_NORFLASH_ID_0, OTA_BOOT_INFO_FLASH_OFFSET, (uint8_t*)otaBootInfo, sizeof(FLASH_OTA_BOOT_INFO_T));
    pmu_flash_read_config();

    int_unlock(lock);
}

static unsigned int seed = 1;

static void set_rand_seed(unsigned int init)
{
    seed = init;
}

static int get_rand(void)
{
    //Based on Knuth "The Art of Computer Programming"
    seed = seed * 1103515245 + 12345;
    return ( (unsigned int) (seed / 65536) % (32767+1) );
}

void ota_randomCode_log(uint8_t randomCode[])
{
    uint32_t lock;
    lock = int_lock();
    pmu_flash_write_config();                    
    hal_norflash_erase(HAL_NORFLASH_ID_0, (uint32_t)&otaUpgradeLog, FLASH_SECTOR_SIZE_IN_BYTES);
    hal_norflash_write(HAL_NORFLASH_ID_0, (uint32_t)&otaUpgradeLog, randomCode, sizeof(otaUpgradeLog.randomCode));
    pmu_flash_read_config();
    int_unlock(lock);
}

void ota_upgradeSize_log(void)
{
    uint32_t lock;

    lock = int_lock();
    pmu_flash_write_config();

    if(++ota_env.i_log >= sizeof(otaUpgradeLog.upgradeSize)/4)
    {
        ota_env.i_log = 0;
        hal_norflash_read(HAL_NORFLASH_ID_0, (uint32_t)&otaUpgradeLog, ota_env.dataBufferForBurning, sizeof(otaUpgradeLog.randomCode));
        hal_norflash_erase(HAL_NORFLASH_ID_0, (uint32_t)&otaUpgradeLog, FLASH_SECTOR_SIZE_IN_BYTES);
        hal_norflash_write(HAL_NORFLASH_ID_0, (uint32_t)&otaUpgradeLog, ota_env.dataBufferForBurning, sizeof(otaUpgradeLog.randomCode));
    }

    hal_norflash_write(HAL_NORFLASH_ID_0, (uint32_t)&otaUpgradeLog.upgradeSize[ota_env.i_log],
                    (uint8_t*)&ota_env.alreadyReceivedDataSizeOfImage, 4);
    
    TRACE("{i_log: %d, RecSize: 0x%x, FlashWrSize: 0x%x}", ota_env.i_log, ota_env.alreadyReceivedDataSizeOfImage, otaUpgradeLog.upgradeSize[ota_env.i_log]);

	pmu_flash_read_config();
	int_unlock(lock);
}

void ota_upgradeLog_destroy(void)
{
    uint32_t lock;
    lock = int_lock();
    ota_env.resume_at_breakpoint = false;
    pmu_flash_write_config();
    hal_norflash_erase(HAL_NORFLASH_ID_0, (uint32_t)&otaUpgradeLog, FLASH_SECTOR_SIZE_IN_BYTES);
    pmu_flash_read_config();
    int_unlock(lock);

    TRACE("Destroyed upgrade log in flash.");
}

uint32_t get_upgradeSize_log(void)
{
    int32_t *p = (int32_t*)otaUpgradeLog.upgradeSize,
            left = 0, right = sizeof(otaUpgradeLog.upgradeSize)/4 - 1, mid;

    if(p[0] != -1)
    {
        while(left < right)
        {
            mid = (left + right) / 2;
            if(p[mid] == -1)
                right = mid - 1;
            else
                left = mid + 1;
        }
    }
    if(p[left]==-1)
        left--;
    
    ota_env.i_log = left;
    ota_env.breakPoint = left!=-1 ? p[left] : 0;
    ota_env.resume_at_breakpoint = ota_env.breakPoint?true:false;

    TRACE("ota_env.i_log: %d", ota_env.i_log);
    return ota_env.breakPoint;
}

void ota_send_resume_response(uint32_t breakPoint, uint8_t randomCode[])
{
    OTA_RSP_RESUME_VERIFY_T tRsp = 
        {OTA_RSP_RESUME_VERIFY, breakPoint,};
    for(uint32_t i = 0; i < sizeof(otaUpgradeLog.randomCode); i++)
        tRsp.randomCode[i] = randomCode[i];
    tRsp.crc32 = crc32(0, (uint8_t*)&tRsp.breakPoint, sizeof(tRsp.breakPoint) + sizeof(tRsp.randomCode));

    ota_env.transmitHander((uint8_t *)&tRsp, sizeof(tRsp));
}

#if 0
void start_new_ota(void)
{
    ota_env.resume_at_breakpoint = false;
    ota_reset_env();
    
    TRACE("Generate new random code for the new transmisson now. [breakPoint: 0x%x]", ota_env.breakPoint);
    for(uint32_t i = 0; i < sizeof(otaUpgradeLog.randomCode); i++)
    {
        set_rand_seed(hal_sys_timer_get());
        ota_env.dataBufferForBurning[i] = get_rand();
        hal_sys_timer_delay(MS_TO_TICKS(1));
    }
    
    uint32_t lock;
    lock = int_lock();
    pmu_flash_write_config();                    
    hal_norflash_erase(HAL_NORFLASH_ID_0, (uint32_t)&otaUpgradeLog, FLASH_SECTOR_SIZE_IN_BYTES);
    hal_norflash_write(HAL_NORFLASH_ID_0, (uint32_t)&otaUpgradeLog, ota_env.dataBufferForBurning, sizeof(otaUpgradeLog.randomCode));
    pmu_flash_read_config();
    int_unlock(lock);    
    
    TRACE("New random code:");
    DUMP8("%02x ", ota_env.dataBufferForBurning, sizeof(otaUpgradeLog.randomCode));
    
    ota_send_resume_response(0, ota_env.dataBufferForBurning);
}
#endif

#define IMAGE_RECV_FLASH_CHECK       1  // It's best to turn it on durning development and not a big deal off in the release.
#define MAX_IMAGE_SIZE              ((uint32_t)(NEW_IMAGE_FLASH_OFFSET - __APP_IMAGE_FLASH_OFFSET__))
#define MIN_SEG_ALIGN               256

void ota_handle_received_data(uint8_t *data, uint32_t len, bool isViaBle)
{
    uint8_t typeCode = data[0];
   
    switch (typeCode)
    {
        case OTA_DATA_PACKET:
        {
            if (!ota_env.isOTAInProgress)
            {
                return;
            }

            uint8_t* rawDataPtr = &data[1];
            
            uint32_t rawDataSize = len - 1; 
            TRACE("Received image data size %d", rawDataSize);
            uint32_t leftDataSize = rawDataSize;
            uint32_t offsetInReceivedRawData = 0;
            do
            {
                uint32_t bytesToCopy;
                // copy to data buffer
                if ((ota_env.offsetInDataBufferForBurning + leftDataSize) > 
                    OTA_DATA_BUFFER_SIZE_FOR_BURNING)
                {
                    bytesToCopy = OTA_DATA_BUFFER_SIZE_FOR_BURNING - ota_env.offsetInDataBufferForBurning;
                }
                else
                {
                    bytesToCopy = leftDataSize;
                }

                leftDataSize -= bytesToCopy;
            
                memcpy(&ota_env.dataBufferForBurning[ota_env.offsetInDataBufferForBurning],
                        &rawDataPtr[offsetInReceivedRawData], bytesToCopy);
                offsetInReceivedRawData += bytesToCopy;
                ota_env.offsetInDataBufferForBurning += bytesToCopy;
                TRACE("offsetInDataBufferForBurning is %d", ota_env.offsetInDataBufferForBurning);
                if (OTA_DATA_BUFFER_SIZE_FOR_BURNING <= ota_env.offsetInDataBufferForBurning)
                {
                    TRACE("Program the image to flash.");

                    #if (IMAGE_RECV_FLASH_CHECK == 1)
                    if((ota_env.offsetInFlashToProgram - ota_env.dstFlashOffsetForNewImage >= ota_env.totalImageSize) ||
                        (ota_env.totalImageSize > MAX_IMAGE_SIZE) ||
                        (ota_env.offsetInFlashToProgram & (MIN_SEG_ALIGN - 1)))
                    {
                        TRACE("ERROR: IMAGE_RECV_FLASH_CHECK");
                        TRACE(" ota_env(.offsetInFlashToProgram - .dstFlashOffsetForNewImage >= .totalImageSize)");
                        TRACE(" or (ota_env.totalImageSize > %d)", MAX_IMAGE_SIZE);
                        TRACE(" or .offsetInFlashToProgram isn't segment aligned");
                        TRACE(".offsetInFlashToProgram:0x%x  .dstFlashOffsetForNewImage:0x%x  .totalImageSize:%d", ota_env.offsetInFlashToProgram, ota_env.dstFlashOffsetForNewImage, ota_env.totalImageSize);
                        //ota_upgradeLog_destroy();  // In order to reduce unnecessary erasures and retransmissions we don't imeediately destory the log but reset ota, because a boundary check is performed before flashing and if there is really wrong we'll catch when an image CRC32 check finally.
                        ota_send_result_response(OTA_RESULT_ERR_FLASH_OFFSET);
                        return;
                    }
                    #endif

                    ota_flush_data_to_flash(ota_env.dataBufferForBurning, OTA_DATA_BUFFER_SIZE_FOR_BURNING, 
                        ota_env.offsetInFlashToProgram);            
                    ota_env.offsetInFlashToProgram += OTA_DATA_BUFFER_SIZE_FOR_BURNING;
                    ota_env.offsetInDataBufferForBurning = 0;
                }
            } while (offsetInReceivedRawData < rawDataSize);

            ota_env.alreadyReceivedDataSizeOfImage += rawDataSize;
            TRACE("Image already received %d", ota_env.alreadyReceivedDataSizeOfImage);

            #if (IMAGE_RECV_FLASH_CHECK == 1)
            if((ota_env.alreadyReceivedDataSizeOfImage > ota_env.totalImageSize) ||
                (ota_env.totalImageSize > MAX_IMAGE_SIZE))
            {
                TRACE("ERROR: IMAGE_RECV_FLASH_CHECK");
                TRACE(" ota_env(.alreadyReceivedDataSizeOfImage > .totalImageSize)");
                TRACE(" or (ota_env.totalImageSize > %d)", MAX_IMAGE_SIZE);
                TRACE(".alreadyReceivedDataSizeOfImage:%d  .totalImageSize:%d", ota_env.alreadyReceivedDataSizeOfImage, ota_env.totalImageSize);
                //ota_upgradeLog_destroy();  // In order to reduce unnecessary erasures and retransmissions we don't imeediately destory the log but reset ota, because a boundary check is performed before flashing and if there is really wrong we'll catch when an image CRC32 check finally.
                ota_send_result_response(OTA_RESULT_ERR_RECV_SIZE);
                return;
            }
            #endif

#if DATA_ACK_FOR_SPP_DATAPATH_ENABLED
            if (OTA_DATAPATH_SPP == ota_env.dataPathType)
            {
                ota_send_data_ack_response();
            }
#endif
            break;
        }
        case OTA_COMMAND_SEGMENT_VERIFY:
        {
            #define MAX_SEG_VERIFY_RETEY    3
            static uint32_t seg_verify_retry = MAX_SEG_VERIFY_RETEY;

            if (!ota_env.isOTAInProgress)
            {
                return;
            }
            
            OTA_SEGMENT_VERIFY_T* ptVerifyCmd = (OTA_SEGMENT_VERIFY_T *)(data);

            #if (IMAGE_RECV_FLASH_CHECK == 1)
            if((ota_env.offsetInFlashToProgram - ota_env.dstFlashOffsetForNewImage >= ota_env.totalImageSize) ||
                (ota_env.totalImageSize > MAX_IMAGE_SIZE) ||
                (ota_env.offsetInFlashToProgram & (MIN_SEG_ALIGN - 1)))
            {
                TRACE("ERROR: IMAGE_RECV_FLASH_CHECK");
                TRACE(" ota_env(.offsetInFlashToProgram - .dstFlashOffsetForNewImage >= .totalImageSize)");
                TRACE(" or (ota_env.totalImageSize > %d)", MAX_IMAGE_SIZE);
                TRACE(" or .offsetInFlashToProgram isn't segment aligned");
                TRACE(".offsetInFlashToProgram:0x%x  .dstFlashOffsetForNewImage:0x%x  .totalImageSize:%d", ota_env.offsetInFlashToProgram, ota_env.dstFlashOffsetForNewImage, ota_env.totalImageSize);
                //ota_upgradeLog_destroy();  // In order to reduce unnecessary erasures and retransmissions we don't imeediately destory the log but reset ota, because a boundary check is performed before flashing and if there is really wrong we'll catch when an image CRC32 check finally.
                ota_send_result_response(OTA_RESULT_ERR_FLASH_OFFSET);
                return;
            }
            #endif

            ota_flush_data_to_flash(ota_env.dataBufferForBurning, ota_env.offsetInDataBufferForBurning,
                    ota_env.offsetInFlashToProgram);
            ota_env.offsetInFlashToProgram += ota_env.offsetInDataBufferForBurning;
            ota_env.offsetInDataBufferForBurning = 0;

            TRACE("Calculate the crc32 of the segment.");

            uint32_t startFlashAddr = OTA_FLASH_LOGIC_ADDR + 
            ota_env.dstFlashOffsetForNewImage + ota_env.offsetOfImageOfCurrentSegment;
            uint32_t lengthToDoCrcCheck = ota_env.alreadyReceivedDataSizeOfImage-ota_env.offsetOfImageOfCurrentSegment;

            ota_env.crc32OfSegment = crc32(0, (uint8_t *)(startFlashAddr), lengthToDoCrcCheck);
            TRACE("CRC32 of the segement is 0x%x", ota_env.crc32OfSegment);

            if ((OTA_START_MAGIC_CODE == ptVerifyCmd->magicCode) && 
                (ptVerifyCmd->crc32OfSegment == ota_env.crc32OfSegment))
            {
                ota_send_segment_verification_response(true);
                
                ota_upgradeSize_log();
                seg_verify_retry = MAX_SEG_VERIFY_RETEY;

                // backup of the information in case the verification of current segment failed
                ota_env.offsetInFlashOfCurrentSegment = ota_env.offsetInFlashToProgram;
                ota_env.offsetOfImageOfCurrentSegment = ota_env.alreadyReceivedDataSizeOfImage;
            }
            else
            {
                if(--seg_verify_retry == 0)
                {
                    seg_verify_retry = MAX_SEG_VERIFY_RETEY;
                    
                    TRACE("ERROR: segment verification retry too much!");
                    ota_upgradeLog_destroy();  // Yes, destory it and retransmit the entire image.
                    ota_send_result_response(OTA_RESULT_ERR_SEG_VERIFY);
                    return;
                }

                // restore the offset
                ota_env.offsetInFlashToProgram = ota_env.offsetInFlashOfCurrentSegment;
                ota_env.alreadyReceivedDataSizeOfImage = ota_env.offsetOfImageOfCurrentSegment;                
                ota_send_segment_verification_response(false);
            }

            // reset the CRC32 value of the segment
            ota_env.crc32OfSegment = 0;

            // reset the data buffer
            TRACE("total size is %d already received %d", ota_env.totalImageSize,
                ota_env.alreadyReceivedDataSizeOfImage);

            break;
        }
        case OTA_COMMAND_START:
        {
            OTA_START_T* ptStart = (OTA_START_T *)(data);
            if (OTA_START_MAGIC_CODE == ptStart->magicCode)
            {
                TRACE("Receive command start request:");
                ota_reset_env();
                ota_env.totalImageSize = ptStart->imageSize;
                ota_env.crc32OfImage = ptStart->crc32OfImage; 
                ota_env.isOTAInProgress = true;

                ota_env.AlreadyReceivedConfigurationLength = 0;

                TRACE("Image size is 0x%x, crc32 of image is 0x%x",
                    ota_env.totalImageSize, ota_env.crc32OfImage);

                // send response
                ota_send_start_response(isViaBle);
            }
            break;
        }
		case OTA_COMMAND_RESUME_VERIFY:
        {
            OTA_RESUME_VERIFY_T* ptStart = (OTA_RESUME_VERIFY_T *)data;

            if (OTA_START_MAGIC_CODE == ptStart->magicCode)
            {
                uint32_t breakPoint;
                uint8_t *randomCode = ota_env.dataBufferForBurning;
                memset(randomCode, 0, sizeof(otaUpgradeLog.randomCode));

                TRACE("Receive command resuming verification:");
                if(ptStart->crc32 != crc32(0, ptStart->randomCode, sizeof(ptStart->randomCode) + sizeof(ptStart->segmentSize)))
                {
                    breakPoint = -1;
                    TRACE("Resuming verification crc32 check fail.");
                    TRACE("Original CRC32: 0x%x, confirmed CRC32:0x%X");
                    goto resume_response;
                }
                TRACE("Resuming verification crc32 check pass.");

                TRACE("Receive segment size 0x%x.", ptStart->segmentSize);

                TRACE("Receive random code:");
                DUMP8("%02x ", ptStart->randomCode, sizeof(otaUpgradeLog.randomCode));

                TRACE("Device's random code:");
                DUMP8("%02x ", otaUpgradeLog.randomCode, sizeof(otaUpgradeLog.randomCode));

                breakPoint = get_upgradeSize_log();
                if(breakPoint & (MIN_SEG_ALIGN - 1))  // Minimum segment alignment.
                {
                    TRACE("Breakpoint:0x%x isn't minimum segment alignment!", breakPoint);
                    ota_upgradeLog_destroy();  // Error in log, we'd better try to retransmit the entire image.
                    #if 1
                    breakPoint = get_upgradeSize_log();
                    #else
                    ota_send_result_response(OTA_RESULT_ERR_BREAKPOINT);
                    return;
                    #endif
                }
                if(breakPoint)
                {
                    if(!memcmp(otaUpgradeLog.randomCode, ptStart->randomCode, sizeof(otaUpgradeLog.randomCode)))
                    {
                        TRACE("OTA can resume. Resuming from the breakpoint at: 0x%x.", breakPoint);
                    }
                    else
                    {
                        TRACE("OTA can't resume because the randomCode is inconsistent. [breakPoint: 0x%x]", breakPoint);

                        breakPoint = ota_env.breakPoint = ota_env.resume_at_breakpoint = 0;
                    }
                }
                if(breakPoint == 0)
                {
                    TRACE("OTA resume none. Generate new random code for the new transmisson now. [breakPoint: 0x%x]", breakPoint);
                    for(uint32_t i = 0; i < sizeof(otaUpgradeLog.randomCode); i++)
                    {
                        set_rand_seed(hal_sys_timer_get());
                        randomCode[i] = get_rand();
                        hal_sys_timer_delay(MS_TO_TICKS(1));
                    }
                    
                    ota_randomCode_log(randomCode);
                    
                    TRACE("New random code:");
                    DUMP8("%02x ", randomCode, sizeof(otaUpgradeLog.randomCode));
                }

            resume_response:
                ota_send_resume_response(breakPoint, randomCode);
            }
            break;
        }
        case OTA_COMMAND_CONFIG_OTA:
        {
            OTA_FLOW_CONFIGURATION_T* ptConfig = (OTA_FLOW_CONFIGURATION_T *)&(ota_env.configuration);
            memcpy((uint8_t *)ptConfig + ota_env.AlreadyReceivedConfigurationLength, 
                &(data[1]), len - 1);            

            ota_env.AlreadyReceivedConfigurationLength += (len - 1);
            if ((ptConfig->lengthOfFollowingData + 4) <= ota_env.AlreadyReceivedConfigurationLength)
            {
                TRACE("lengthOfFollowingData 0x%x", ptConfig->lengthOfFollowingData);
                TRACE("startLocationToWriteImage 0x%x", ptConfig->startLocationToWriteImage);
                TRACE("isToClearUserData %d", ptConfig->isToClearUserData);
                TRACE("isToRenameBT %d", ptConfig->isToRenameBT);
                TRACE("isToRenameBLE %d", ptConfig->isToRenameBLE);
                TRACE("isToUpdateBTAddr %d", ptConfig->isToUpdateBTAddr);
                TRACE("isToUpdateBLEAddr %d", ptConfig->isToUpdateBLEAddr);
                TRACE("New BT name:");
                DUMP8("0x%02x ", ptConfig->newBTName, NAME_LENGTH);
                TRACE("New BLE name:");
                DUMP8("0x%02x ", ptConfig->newBLEName, NAME_LENGTH);
                TRACE("New BT addr:");
                DUMP8("0x%02x ", ptConfig->newBTAddr, BD_ADDR_LENGTH);
                TRACE("New BLE addr:");
                DUMP8("0x%02x ", ptConfig->newBLEAddr, BD_ADDR_LENGTH);
                TRACE("crcOfConfiguration 0x%x", ptConfig->crcOfConfiguration);   
                
                // check CRC
                if (ptConfig->crcOfConfiguration == crc32(0, (uint8_t *)ptConfig, sizeof(OTA_FLOW_CONFIGURATION_T) - sizeof(uint32_t)))
                {
                    if(ota_env.totalImageSize > MAX_IMAGE_SIZE)
                    {
                        TRACE("ImageSize 0x%x greater than 0x%x! Terminate the upgrade.", ota_env.totalImageSize, MAX_IMAGE_SIZE);
                        ota_send_result_response(OTA_RESULT_ERR_IMAGE_SIZE);
                        return;
                    }

                    if(ota_env.breakPoint > ota_env.totalImageSize)
                    {
                        ota_upgradeLog_destroy();  // Error in log, we'd better try to retransmit the entire image.
                        ota_send_result_response(OTA_RESULT_ERR_BREAKPOINT);
                        return;
                    }
                    
                    ota_env.offsetInFlashToProgram = NEW_IMAGE_FLASH_OFFSET;
                    ota_env.offsetInFlashOfCurrentSegment = ota_env.offsetInFlashToProgram;
                    ota_env.dstFlashOffsetForNewImage = ota_env.offsetInFlashOfCurrentSegment;

                    if(ota_env.resume_at_breakpoint == true)
                    {
                        ota_env.alreadyReceivedDataSizeOfImage = ota_env.breakPoint;
                        ota_env.offsetOfImageOfCurrentSegment = ota_env.alreadyReceivedDataSizeOfImage;
                        ota_env.offsetInFlashOfCurrentSegment = ota_env.dstFlashOffsetForNewImage + ota_env.offsetOfImageOfCurrentSegment;
                        ota_env.offsetInFlashToProgram = ota_env.offsetInFlashOfCurrentSegment;
                    }

                        TRACE("OTA config pass.");

                        TRACE("Start writing the received image to flash offset 0x%x", ota_env.offsetInFlashToProgram);
        
                        ota_send_configuration_response(true);
                }
                else
                {
                    TRACE("OTA config failed.");
                    ota_send_configuration_response(false);
                }
            }
            break;
        }
        case OTA_COMMON_GET_OTA_RESULT:
        {
            // check whether all image data have been received
            if (ota_env.alreadyReceivedDataSizeOfImage == ota_env.totalImageSize)
            {
                TRACE("The final image programming and crc32 check.");
                
                // flush the remaining data to flash
                ota_flush_data_to_flash(ota_env.dataBufferForBurning, ota_env.offsetInDataBufferForBurning,
                    ota_env.offsetInFlashToProgram);

                // update the magic code of the application image
                app_update_magic_number_of_app_image(NORMAL_BOOT);
            
                // check the crc32 of the image
                bool ret = ota_check_whole_image_crc();

                if (ret)
                {

                    FLASH_OTA_BOOT_INFO_T otaBootInfo = {COPY_NEW_IMAGE, ota_env.totalImageSize, ota_env.crc32OfImage};                    
                    ota_update_boot_info(&otaBootInfo);                 
                    ota_update_nv_data();
                    ota_send_result_response(ret);
                    TRACE("Whole image verification pass.");
                    ota_env.isPendingForReboot = true;
                }
                else
                {
                    ota_send_result_response(ret);
                    TRACE("Whole image verification failed.");
                }

                ota_upgradeLog_destroy();
                ota_reset_env();

            }  
            else
            {
                ota_send_result_response(false);
            }
            break;
        }
        case OTA_READ_FLASH_CONTENT:
        {
            ota_sending_flash_content((OTA_READ_FLASH_CONTENT_REQ_T*)data);
            break;
        }
        default:
            break;
    }         
}

#endif // #if OTA_ENABLED

