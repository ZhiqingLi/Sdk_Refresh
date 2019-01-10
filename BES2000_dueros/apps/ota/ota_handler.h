#ifndef __OTA_BLE_H
#define __OTA_BLE_H

#ifdef __cplusplus
extern "C" {
#endif

#define OTA_SW_VERSION		2
#define OTA_HW_VERSION		1

#define FLASH_SECTOR_SIZE_IN_BYTES          4096
#define OTA_DATA_BUFFER_SIZE_FOR_BURNING    FLASH_SECTOR_SIZE_IN_BYTES

#define OTA_START_MAGIC_CODE    (0x54534542)  // BEST
#define NORMAL_BOOT				0xBE57EC1C
#define COPY_NEW_IMAGE			0x5a5a5a5a

#ifndef DATA_ACK_FOR_SPP_DATAPATH_ENABLED
#define DATA_ACK_FOR_SPP_DATAPATH_ENABLED 0
#endif

#ifndef NEW_IMAGE_FLASH_OFFSET
#define NEW_IMAGE_FLASH_OFFSET  (0x200000)
#endif

#ifndef __APP_IMAGE_FLASH_OFFSET__
#define __APP_IMAGE_FLASH_OFFSET__ (0x18000)
#endif

#define OTA_FLASH_LOGIC_ADDR        (FLASH_NC_BASE)
#define OTA_FLASH_OFFSET_OF_IMAGE   __APP_IMAGE_FLASH_OFFSET__
#define OTA_FLASH_ENTRY_OFFSET	    (OTA_FLASH_LOGIC_ADDR + OTA_FLASH_OFFSET_OF_IMAGE)

#define BD_ADDR_LENGTH					6
#define NAME_LENGTH     				32
#define OTA_BLE_DATA_PACKET_MAX_SIZE    257
#define OTA_BT_DATA_PACKET_MAX_SIZE     513

typedef void (*ota_transmit_data_t)(uint8_t* ptrData, uint32_t dataLen);

/**
 * @brief The format of the otaBootInfo
 *
 */
typedef struct
{
	uint32_t magicNumber;
	uint32_t imageSize;
	uint32_t imageCrc;
} FLASH_OTA_BOOT_INFO_T;

typedef struct
{
    uint8_t     randomCode[32];
    uint32_t    upgradeSize[(FLASH_SECTOR_SIZE_IN_BYTES - 32) / 4];
} FLASH_OTA_UPGRADE_LOG_FLASH_T;

/**
 * @brief The format of the start OTA control packet
 *
 */
typedef struct
{
    uint8_t     packetType;     // should be OTA_COMMAND_START
    uint32_t    magicCode;      // should be OTA_START_MAGIC_CODE
    uint32_t    imageSize;      // total image size
    uint32_t    crc32OfImage;   // crc32 of the whole image
}__attribute__ ((__packed__))OTA_START_T;

/**
 * @brief The format of the start OTA response packet
 *
 */
typedef struct
{
    uint8_t     packetType;     // should be OTA_RSP_START
    uint32_t    magicCode;      // should be OTA_START_MAGIC_CODE
    uint16_t    swVersion;
    uint16_t    hwVersion;
    uint16_t    MTU;            // MTU exchange result, central will send images in the unit of MTU
}__attribute__ ((__packed__))OTA_START_RSP_T;

/**
 * @brief The format of the OTA configuration response packet
 *
 */
typedef struct
{
    uint8_t     packetType;     // should be OTA_RSP_CONFIG
    uint8_t     isConfigurationDone;    // 1 if the configuration has been done successfully, otherwise, 0
} __attribute__ ((__packed__)) OTA_RSP_CONFIG_T;

/**
 * @brief The format of the segment verification request
 *
 */
typedef struct
{
    uint8_t     packetType;     // should be OTA_COMMAND_SEGMENT_VERIFY
    uint32_t    magicCode;      // should be OTA_START_MAGIC_CODE
    uint32_t    crc32OfSegment;          // crc32 of the 1 segment
} __attribute__ ((__packed__)) OTA_SEGMENT_VERIFY_T;

/**
 * @brief The format of the segment verification request
 *
 */
typedef struct
{
    uint8_t     packetType;     // should be OTA_RSP_SEGMENT_VERIFY
    uint8_t     isVerificationPassed;
} __attribute__ ((__packed__)) OTA_RSP_SEGMENT_VERIFY_T;

/**
 * @brief The format of the OTA result response
 *
 */
typedef struct
{
    uint8_t     packetType;     // should be OTA_RSP_RESULT
    uint8_t     isVerificationPassed;
} __attribute__ ((__packed__)) OTA_RSP_OTA_RESULT_T;

/**
 * @brief The format of the OTA reading flash content command
 *
 */
typedef struct
{
    uint8_t     packetType;     // should be OTA_READ_FLASH_CONTENT
    uint8_t     isToStart;      // true to start, false to stop
    uint32_t    startAddr;
    uint32_t    lengthToRead;
} __attribute__ ((__packed__)) OTA_READ_FLASH_CONTENT_REQ_T;

/**
 * @brief The format of the OTA reading flash content command
 *
 */
typedef struct
{
    uint8_t     packetType;     // should be OTA_READ_FLASH_CONTENT
    uint8_t     isReadingReqHandledSuccessfully;
} __attribute__ ((__packed__)) OTA_READ_FLASH_CONTENT_RSP_T;

typedef struct
{
    uint8_t     packetType;     // should be OTA_CONTROL_RESUME_VERIFY
    uint32_t    magicCode;      // should be OTA_START_MAGIC_CODE
    uint8_t     randomCode[32];
    uint32_t    segmentSize;
    uint32_t    crc32;          // CRC32 of randomCode and segment size
} __attribute__ ((__packed__)) OTA_RESUME_VERIFY_T;

typedef struct
{
    uint8_t     packetType;     // should be OTA_RSP_RESUME_VERIFY
    uint32_t    breakPoint;
    uint8_t     randomCode[32];
    uint32_t    crc32;          // CRC32 of breakPoint and randomCode
} __attribute__ ((__packed__)) OTA_RSP_RESUME_VERIFY_T;

/**
 * @brief The control packet type
 *
 */
typedef enum
{
    OTA_COMMAND_START = 0x80,   // from central to device, to let OTA start
    OTA_RSP_START,              // from device to centrl, to let it know that it has been ready for OTA
    OTA_COMMAND_SEGMENT_VERIFY, // from central to device, sent per 1% of image, inform device to do CRC check on those 1%
    OTA_RSP_SEGMENT_VERIFY,
    OTA_RSP_RESULT,
    OTA_DATA_PACKET,
    OTA_COMMAND_CONFIG_OTA,     // from central to device, to configure the following OTA flow
    OTA_RSP_CONFIG,
    OTA_COMMON_GET_OTA_RESULT,
    OTA_READ_FLASH_CONTENT,
    OTA_FLASH_CONTENT_DATA,
    OTA_DATA_ACK,
    OTA_COMMAND_RESUME_VERIFY,
    OTA_RSP_RESUME_VERIFY,
} OTA_CONTROL_PACKET_TYPE_E;

typedef enum
{
	OTA_DATAPATH_BLE = 0,	
	OTA_DATAPATH_SPP
} OTA_DATAPATH_TYPE_E;

typedef struct
{
    uint32_t    lengthOfFollowingData;
    uint32_t    startLocationToWriteImage;    // the offset of the flash to start writing the image
    uint32_t    isToClearUserData   : 1;
    uint32_t    isToRenameBT        : 1;
    uint32_t    isToRenameBLE       : 1;
    uint32_t    isToUpdateBTAddr    : 1;
    uint32_t    isToUpdateBLEAddr   : 1;
    uint32_t    reserve             : 27;
    uint8_t     newBTName[NAME_LENGTH];
    uint8_t     newBLEName[NAME_LENGTH];
    uint8_t     newBTAddr[BD_ADDR_LENGTH];
    uint8_t     newBLEAddr[BD_ADDR_LENGTH];
    uint32_t    crcOfConfiguration; // CRC of data from lengthOfFollowingData to newBLEAddr
}__attribute__ ((__packed__))OTA_FLOW_CONFIGURATION_T;

/**
 * @brief The OTA handling data structure
 *
 */
typedef struct
{
    uint8_t     dataBufferForBurning[OTA_DATA_BUFFER_SIZE_FOR_BURNING];
    uint32_t    dstFlashOffsetForNewImage;
    uint32_t    offsetInDataBufferForBurning;
    uint32_t    offsetInFlashToProgram;
    uint32_t    totalImageSize;
    uint32_t    alreadyReceivedDataSizeOfImage;
    uint32_t    offsetInFlashOfCurrentSegment;
    uint32_t    offsetOfImageOfCurrentSegment;
    uint32_t    crc32OfImage;
    uint32_t    crc32OfSegment;
    uint32_t    flasehOffsetOfUserDataPool;
    uint32_t    flasehOffsetOfFactoryDataPool;

    // configuration of the OTA
    OTA_FLOW_CONFIGURATION_T    configuration;
    uint32_t    AlreadyReceivedConfigurationLength;

    uint16_t     dataPacketSize;
    ota_transmit_data_t  transmitHander;
	uint8_t     isOTAInProgress;
	uint8_t 	isPendingForReboot;
    uint32_t    offsetInFlashToRead;
    uint32_t    leftSizeOfFlashContentToRead;
    uint8_t     bufForFlashReading[FLASH_SECTOR_SIZE_IN_BYTES];
    OTA_DATAPATH_TYPE_E     dataPathType;

    bool        resume_at_breakpoint;
    uint32_t    breakPoint;
    uint32_t    i_log;
} OTA_ENV_T;


void ota_handler_init(void);
void ota_handle_received_data(uint8_t *data, uint32_t len, bool isViaBle);
void ota_register_transmitter(ota_transmit_data_t transmit_handle);
void ota_update_MTU(uint16_t mtu);
void ota_set_datapath_type(OTA_DATAPATH_TYPE_E datapathType);
bool ota_is_in_progress(void);
void ota_reset_env(void);
void ota_data_transmission_done_callback(void);
void ota_reboot_to_use_new_image(void);
void ota_disconnection_handler(void);

#ifdef __cplusplus
	}
#endif

#endif
