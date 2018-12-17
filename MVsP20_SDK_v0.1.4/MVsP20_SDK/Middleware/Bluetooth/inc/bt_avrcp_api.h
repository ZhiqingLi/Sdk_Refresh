/**
 *******************************************************************************
 * @file    bt_avrcp_api.h
 * @author  Halley
 * @version V1.0.1
 * @date    27-Apr-2016
 * @brief   AVRCP related api
 *******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

/**
* @addtogroup Bluetooth
* @{
* @defgroup bt_avrcp_api bt_avrcp_api.h
* @{
*/

#include "type.h"

#ifndef __BT_AVRCP_API_H__
#define __BT_AVRCP_API_H__


typedef struct _AvrcpAppFeatures
{
	bool		supportAdvanced;	/*!< Advanced AVRCP includes songs info and playing status report...*/
	bool		supportTgSide;		/*!< TG side includes volume synchronization (remote volume control) */
}AvrcpAppFeatures;


/**
 * AvrcpMediaStatus type
 *
 * Defines play status of the currently playing media.
 */
typedef uint8_t AvrcpAdvMediaStatus;

#define AVRCP_ADV_MEDIA_STOPPED			0x00
#define AVRCP_ADV_MEDIA_PLAYING			0x01
#define AVRCP_ADV_MEDIA_PAUSED			0x02
#define AVRCP_ADV_MEDIA_FWD_SEEK		0x03
#define AVRCP_ADV_MEDIA_REV_SEEK		0x04
#define AVRCP_ADV_MEDIA_ERROR			0xFF

typedef struct _AvrcpAdvTrackStruct
{
	uint32_t			ms; /* The most significant 32 bits of the track index information.  */ 
	uint32_t			ls; /* The least significant 32 bits of the track index information.  */ 
}AvrcpAdvTrackStruct;

/*---------------------------------------------------------------------------
 * AvrcpBatteryStatus type
 *
 * Defines values for battery status.
 */
typedef uint8_t AvrcpAdvBatteryStatus;

#define AVRCP_ADV_BATT_STATUS_NORMAL		0
#define AVRCP_ADV_BATT_STATUS_WARNING		1
#define AVRCP_ADV_BATT_STATUS_CRITICAL		2
#define AVRCP_ADV_BATT_STATUS_EXTERNAL		3
#define AVRCP_ADV_BATT_STATUS_FULL_CHARGE	4

/*---------------------------------------------------------------------------
 * AvrcpAdvSystemStatus type
 *
 * Defines values for system status.
 */
typedef uint8_t AvrcpAdvSystemStatus;

#define AVRCP_ADV_SYS_POWER_ON		0
#define AVRCP_ADV_SYS_POWER_OFF		1
#define AVRCP_ADV_SYS_UNPLUGGED		2

/*---------------------------------------------------------------------------
 * AvrcpAdvEqValue type
 *
 * Defines values for the player equalizer status.
 */
typedef uint8_t AvrcpAdvEqValue;

#define AVRCP_ADV_EQ_OFF			1
#define AVRCP_ADV_EQ_ON				2

/* End of AvrcpAdvEqValue */ 

/*---------------------------------------------------------------------------
 * AvrcpAdvRepeatValue type
 *
 * Defines values for the player repeat mode status.
 */
typedef uint8_t AvrcpAdvRepeatValue;

#define AVRCP_ADV_REPEAT_OFF		1
#define AVRCP_ADV_REPEAT_SINGLE		2
#define AVRCP_ADV_REPEAT_ALL		3
#define AVRCP_ADV_REPEAT_GROUP		4

/* End of AvrcpAdvRepeatValue */ 

/*---------------------------------------------------------------------------
 * AvrcpShuffleAdvValue type
 *
 * Defines values for the player shuffle mode status.
 */
typedef uint8_t AvrcpAdvShuffleValue;

#define AVRCP_ADV_SHUFFLE_OFF		1
#define AVRCP_ADV_SHUFFLE_ALL		2
#define AVRCP_ADV_SHUFFLE_GROUP		3

/* End of AvrcpAdvShuffleValue */ 

/*---------------------------------------------------------------------------
 * AvrcpAdvScanValue type
 *
 * Defines values for the player scan mode status.
 */
typedef uint8_t AvrcpAdvScanValue;

#define AVRCP_ADV_SCAN_OFF			1
#define AVRCP_ADV_SCAN_ALL			2
#define AVRCP_ADV_SCAN_GROUP		3

/* End of AvrcpAdvScanValue */ 

typedef struct _AvrcpAdvAppSettings
{
	AvrcpAdvEqValue			eq;
	AvrcpAdvRepeatValue		repeat;
	AvrcpAdvShuffleValue	shuffle;
	AvrcpAdvScanValue		scan;
}AvrcpAdvAppSettings;

typedef struct _AvrcpAdvAddressedPlayer
{
	uint16_t		playerId;
	uint16_t		uidCounter;
}AvrcpAdvAddressedPlayer;


/*---------------------------------------------------------------------------
 * AvrcpAdvEventMask
 *
 * Bitmask of supported AVRCP events.  By default, only 
 * AVRCP_ENABLE_PLAY_STATUS_CHANGED and AVRCP_ENABLE_TRACK_CHANGED are 
 * enabled when a channel is registered.  The application must explicitly
 * enable any other supported events.

 */
typedef uint16_t AvrcpAdvEventMask;

#define AVRCP_ADV_ENABLE_PLAY_STATUS_EVENT			0x0001  /* Change in playback status */ 
#define AVRCP_ADV_ENABLE_TRACK_EVENT				0x0002  /* Current track changed */ 
#define AVRCP_ADV_ENABLE_TRACK_END_EVENT			0x0004  /* Reached end of track  */ 
#define AVRCP_ADV_ENABLE_TRACK_START_EVENT			0x0008  /* Reached track start   */ 
#define AVRCP_ADV_ENABLE_PLAY_POS_EVENT				0x0010  /* Change in playback position */ 
#define AVRCP_ADV_ENABLE_BATT_STATUS_EVENT			0x0020  /* Change in battery status */ 
#define AVRCP_ADV_ENABLE_SYS_STATUS_EVENT			0x0040  /* Change in system status */ 
#define AVRCP_ADV_ENABLE_APP_SETTING_EVENT			0x0080  /* Change in player application setting */ 
#define AVRCP_ADV_ENABLE_NOW_PLAYING_EVENT			0x0100  /* Change in the now playing list */
#define AVRCP_ADV_ENABLE_AVAIL_PLAYERS_EVENT		0x0200  /* Available players changed */
#define AVRCP_ADV_ENABLE_ADDRESSED_PLAYER_EVENT		0x0400  /* Addressed player changed */
#define AVRCP_ADV_ENABLE_UIDS_EVENT					0x0800  /* UIDS changed */
#define AVRCP_ADV_ENABLE_VOLUME_EVENT				0x1000  /* Volume Changed */



typedef union
{
	AvrcpAdvMediaStatus 	avrcpAdvMediaStatus;
	AvrcpAdvTrackStruct 	avrcpAdvTrack;
	uint32_t				avrcpAdvPlayPosition;
	AvrcpAdvBatteryStatus	avrcpAdvBatteryStatus;
	AvrcpAdvSystemStatus	avrcpAdvSystemStatus;
	AvrcpAdvAppSettings 	avrcpAdvAppSettings;
	AvrcpAdvAddressedPlayer avrcpAdvAddressedPlayer;
	uint16_t 				avrcpAdvUidCounter;
	uint8_t					avrcpAdvVolumePercent;
	uint8_t					*avrcpAdvCompanyId;
	AvrcpAdvEventMask		avrcpAdvEventMask;
} AvrcpAdvParam;


/**
 * @brief  AVRCP connect command.
 * @param  addr The address of bt device to connect.
 * @return True for the command implement successful 
 * @Note If the device has been connected with a2dp profile, the event 
 *		BT_STACK_EVENT_AVRCP_CONNECTED will be received in the callback. Otherwise
 *		BT_STACK_EVENT_PAGE_TIMEOUT will be received.
 *
 */
bool AvrcpConnect(uint8_t * addr);


/**
 * @brief  AVRCP disconnect command.
 * @param  None
 * @return True for the command implement successful 
 * @Note If the device has been disconnected. the event BT_STACK_EVENT_AVRCP_DISCONNECTED
 *		will be received in the callback. 
 */
bool AvrcpDisconnect(void);

/**
 * @brief  Send 'Play' command to the target
 * @param  None
 * @return True for the command implement successful 
 * @Note 
 */
bool AvrcpCtrlPlay(void);
#define BTCtrlPlay		AvrcpCtrlPlay

/**
 * @brief  Send 'Pause' command to the target
 * @param  None
 * @return True for the command implement successful 
 * @Note 
 */
bool AvrcpCtrlPause(void);
#define BTCtrlPause		AvrcpCtrlPause

/**
 * @brief  Send 'Stop' command to the target
 * @param  None
 * @return True for the command implement successful 
 * @Note 
 */
bool AvrcpCtrlStop(void);
#define BTCtrlStop		AvrcpCtrlStop

/**
 * @brief  Send 'Next' command to the target
 * @param  None
 * @return True for the command implement successful 
 * @Note 
 */
bool AvrcpCtrlNext(void);
#define BTCtrlNext		AvrcpCtrlNext

/**
 * @brief  Send 'Previous' command to the target
 * @param  None
 * @return True for the command implement successful 
 * @Note 
 */
bool AvrcpCtrlPrev(void);
#define BTCtrlPrev		AvrcpCtrlPrev

/**
 * @brief  Send 'FF' press command to the target
 * @param  None
 * @return True for the command implement successful 
 * @Note 
 */
bool AvrcpCtrlPressFastFoward(void);
#define BTCtrlFF		AvrcpCtrlPressFastFoward

/**
 * @brief  Send 'FF' release command to the target
 * @param  None
 * @return True for the command implement successful 
 * @Note 
 */
bool AvrcpCtrlReleaseFastFoward(void);
#define BTCtrlEndFF		AvrcpCtrlReleaseFastFoward


/**
 * @brief  Send 'FB' press command to the target
 * @param  None
 * @return True for the command implement successful 
 * @Note 
 */
bool AvrcpCtrlFastBackward(void);
#define BTCtrlFB		AvrcpCtrlFastBackward

/**
 * @brief  Send 'FB' release command to the target
 * @param  None
 * @return True for the command implement successful 
 * @Note 
 */
bool AvrcpCtrlReleaseFastBackward(void);
#define BTCtrlEndFB		AvrcpCtrlReleaseFastBackward

#endif

/**
 * @}
 * @}
 */
 

