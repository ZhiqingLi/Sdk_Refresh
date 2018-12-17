#ifndef __AP_AUTOTEST_MEM_DEFINE_H
#define __AP_AUTOTEST_MEM_DEFINE_H

//DAC数据缓存buffer，共1k的数据量 16*16*2*2
#define DAC_BUFFER                  (0x9fc22400)

#define DAC_BUFFER_LEN              (0x400)

//数据采集buffer
#define SOUND_DATA_ADDR             (DAC_BUFFER + DAC_BUFFER_LEN)

//每次采集共4k的数据量
#define SOUND_DATA_LEN              (0x1000)

#define SAVE_SOUND_DATA_ADDR        (LINEIN_SOUND_DATA_ADDR + SOUND_DATA_LEN)

//stub发送参数缓冲区，共256字节
#define STUB_ATT_RW_TEMP_BUFFER     (0x9fc3a000)

#define STUB_ATT_RW_TEMP_BUFFER_LEN (256)

#define STUB_ATT_RETURN_DATA_BUFFER (STUB_ATT_RW_TEMP_BUFFER + STUB_ATT_RW_TEMP_BUFFER_LEN)

#define STUB_ATT_RETURN_DATA_LEN    (256)

#define STUB_ONCE_BANK_READ_SIZE    (1024)

#define STUB_ATT_READ_BANK_BUFFER   (STUB_ATT_RETURN_DATA_BUFFER + STUB_ATT_RETURN_DATA_LEN)

#define ATT_MPDATA_TEMP_BUFFER      (STUB_ATT_READ_BANK_BUFFER + STUB_ONCE_BANK_READ_SIZE + 8)

#define ATT_MPDATA_MEX_LENGTH       (0x800)


//卡读取config.txt缓存的数据空间
#define CFG_FILE_BUFFER        (0x3a800)

#define MAX_CFG_FILE_LENGTH    (0x800)

#define ATT_LOG_FILE_BUFFER    (0x3b000)

#define ATT_LOG_FILE_BUFFER_LEN (0x800)

//日志文件缓存空间，每个日志文件2k大小
#define LOG_FILE_BUFFER             (0x9fc3b800)

#define LOG_FILE_LEN                (2*1024)


#endif

