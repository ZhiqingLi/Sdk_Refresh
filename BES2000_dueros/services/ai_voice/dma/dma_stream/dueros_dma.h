#ifndef _DUEROS_DMA_H_
#define _DUEROS_DMA_H_

#ifdef __cplusplus
extern "C" {
#endif

#define DMA_PUSH_TO_TALK

#include "platform.h"
#include "dma.pb-c.h"
#define BLE_UUID_SERVICE "0000FE04-0000-1000-8000-00805F9B34FB"
#define BLE_UUID_TX_CHARACTERISTIC  "B84AC9C6-29C5-46D4-BBA1-9D534784330F"
#define BLE_UUID_RX_CHARACTERISTIC "C2E758B9-0E78-41E0-B0CB-98A593193FC5"
#define BLE_UUID16_SERVICE "FE04"
#define RFCOMM_UUID "51DBA109-5BA9-4981-96B7-6AFE132093DE"

#define DUEROS_DMA_MAGIC 0xFE04

#define DUEROS_DMA_HEAD_LEN (4)

#define DUEROS_DMA_LOW_VERSION  0
#define DUEROS_DMA_HIGH_VERSION 0xff
#define DUEROS_DMA_INITED      2
#define DUEROS_DMA_LINKED      0x10101

#define DUEROS_DMA_VERISON_SIZE 20
#define DUEROS_DMA_HEADER_SIZE  2
#define DUEROS_DMA_AUDIO_STREAM_ID  1
#define DUEROS_DMA_CMD_STREAM_ID   0

#define DUEROS_DMA_CMD_QUEUE     1
#define DUEROS_AUDIO_DATA_QUEUE  0
#define DMA_VERSION             0
#define DUEROS_DMA_SIGNAL     15

#define INITIAL_REQUEST_ID "001"

#define _1_BITS    0x1
#define _4_BITS    0xf
#define _5_BITS    0x1f
#define _6_BITS    0x3f



#define BigLittleSwap32(A)  ((((WORD32)(A) & 0xff000000) >> 24) | \
                                                 (((WORD32)(A) & 0x00ff0000) >> 8) | \
                                                 (((WORD32)(A) & 0x0000ff00) << 8) | \
                                                 (((WORD32)(A) & 0x000000ff) << 24))




enum DMA_ErrorCode{
	DUEROS_DMA_OK=0,
	DUEROS_DMA_FAIL=1,
};


typedef  unsigned char      WORD8;
typedef  unsigned short int WORD16;
typedef  unsigned int	    WORD32;
typedef  unsigned long long WORD64;

typedef  signed char        SWORD8;
typedef  signed short int 	SWORD16;
typedef  signed int	    	SWORD32;
typedef  signed long long 	SWORD64;

typedef  unsigned char uint8_t;

//typedef unsigned int uint32_t;


typedef struct protocol_version{
	WORD16 magic;
	WORD8  hVersion;
	WORD8  lVersion;
	WORD8 reserve[16];
} PROTOCOL_VER;

typedef struct dueros_dma_header{
	unsigned short int version:4;
	unsigned short int streamID:5;
	unsigned short int reserve:6;
	unsigned short int length:1;
} DUEROS_DMA_HEADER;

/*
typedef struct dueros_dma_usr_reg_operation{

}DUEROS_DMA_USR_REG_OPER;
*/
typedef unsigned int (*data_callback)(unsigned char *buff, unsigned int length);

typedef struct dueros_dma_operation{
    unsigned int (*get_trans_mtu)(void);
	int (*sent_raw_audio_data)(uint8_t *buf, uint32_t len);
	//WORD32 (*receive_raw_aduio_data)();
	//WORD32 (*get_mic_raw_data)();
	int (*sent_raw_cmd)(uint8_t *buf,  uint32_t len);
	int (*should_send)(uint32_t len);
	//WORD32 (*receive_raw_cmd)();
	int (*start_speech)(void);
	int (*stop_speech)(void);
	void (*get_setting)(SpeechSettings*);
	void (*get_dev_info)(DeviceInformation *);
	char*(*get_key)();
	void (*get_dev_config)(DeviceConfiguration*);
	void (*set_state)(State *);
	void (*get_state)(uint32_t ,State *);
	void (*notify_speech_state)(SpeechState );
	int (*at_command)(const char *);
	int(*stop_clear)(void);
	const unsigned char* (*summary_function)(const void *input,int size, unsigned char *out);
	void (*forward_test_command)(char *);
/*
	void(*atcommand_ata)();
	void(*atcommand_at_chup)();
	void(*atcommand_at_bldn)();
	void(*atcommand_at_chld_0)();
	void(*atcommand_at_chld_1)();
	void(*atcommand_at_chld_2)();
	void(*atcommand_at_chld_3)();
*/

} DUEROS_DMA_OPER;

typedef struct send_frame{
	void*buffer;
	WORD32 len;
	struct send_frame *next;
}SEND_FRAME;

typedef struct dueros_dma_queque
{
	SEND_FRAME * head;
	SEND_FRAME *tail;
	unsigned int  depth;
	DUEROS_MUTEX_ID mutex;

}DUEROS_QUEQUE;

typedef struct dueros_dma{
	volatile WORD32 inited;
	volatile WORD32 dialog ;
	volatile WORD32 linked ;
	char *rand;
	SpeechState  speech_state;
	SignMethod signMode;
	PROTOCOL_VER protocol_version;
	DUEROS_DMA_OPER dma_oper;
	DUEROS_QUEQUE audio_data_queque;
	DUEROS_QUEQUE cmd_data_queque;
}DUEROS_DMA;


int dueros_dma_init(PROTOCOL_VER*dma_version,DUEROS_DMA_OPER *dma_oper,SignMethod signMethod, char *rand);
int dueros_dma_sent_ver(void);
int dueros_process_cmd_from_phone(unsigned char *buf, int len);
unsigned int aud_data_callback(unsigned char *buff, unsigned int length);
int dueros_dma_start_speech(void);
int dueros_dma_stop_speech(void);
void dueros_dma_send_sync_state(uint32_t feature);
int dueros_clear_all_queue();
int dueros_dma_connected();
int dueros_dma_disconnected();
void wakeup_dueros_dma_audio_thread();
SpeechState  dueros_dma_get_speech_state();
void	dueros_dam_set_speech_state(SpeechState speech_state);
int dueros_stop_send(void);

#ifdef __cplusplus
}
#endif
#endif
