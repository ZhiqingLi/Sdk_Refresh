//bt_play_api.h
#include "type.h"

#ifndef __BT_PLAY_API_H__
#define __BT_PLAY_API_H__


typedef enum _BT_PLAYER_STATE
{
	BT_PLAYER_STATE_STOP = 0,    // ø’œ–
	BT_PLAYER_STATE_PLAYING,     // ≤•∑≈
	BT_PLAYER_STATE_PAUSED,       // ‘›Õ£
	BT_PLAYER_STATE_FWD_SEEK,
	BT_PLAYER_STATE_REV_SEEK,
	
	BT_PLAYER_STATE_ERROR = 0xff,
} BT_PLAYER_STATE;


#define BT_SBC_PACKET_SIZE					595
#define BT_SBC_DECODER_INPUT_LEN			(10*1024)
#define BT_SBC_LEVEL_HIGH					(BT_SBC_DECODER_INPUT_LEN - BT_SBC_PACKET_SIZE * 4)
#define BT_SBC_LEVEL_LOW					(BT_SBC_LEVEL_HIGH  - BT_SBC_PACKET_SIZE * 3)


uint32_t GetValidSbcDataSize(void);

uint32_t InsertDataToSbcBuffer(uint8_t * data, uint16_t dataLen);

void SetSbcDecoderStarted(bool flag);

bool GetSbcDecoderStarted(void);

int32_t SbcDecoderInit(void);

void BtSbcDecoderRefresh(void);


#ifdef CFG_APP_BT_MODE_EN
#ifdef CFG_FUNC_REMIND_SOUND_EN
#include "decoder_service.h"
bool BtPlayerBackup(void);
void SbcDecoderRefresh(void);
//void BtPlayerSetDecoderShare(DecoderShare State);
//DecoderShare BtPlayerGetDecoderShare(void);
#endif
#endif

int32_t SbcDecoderDeinit(void);

int32_t SbcDecoderStart(void);

void BtPlayerPlay(void);

void BtPlayerPause(void);

//≤•∑≈&‘›Õ£
void BtPlayerPlayPause(void);

void BtAutoPlayMusic(void);
#endif

