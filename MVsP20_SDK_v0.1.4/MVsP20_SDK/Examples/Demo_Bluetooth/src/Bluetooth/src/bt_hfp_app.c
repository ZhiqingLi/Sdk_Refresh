/**
 *******************************************************************************
 * @file    bt_hfp_app.c
 * @author  Halley
 * @version V1.0.1
 * @date    27-Apr-2016
 * @brief   Hfp callback events and actions
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
#include "type.h"
#include "debug.h"

#include "bt_hfp_api.h"
#include "bt_manager.h"





#define BT_MANAGER_HFP_ERROR_NONE					0

#define BT_MANAGER_ERROR_PARAMETER_INVAILD			-1
#define BT_MANAGER_ERROR_NOT_INITIALIZED			-2
#define BT_MANAGER_HFP_ERROR_NOT_CONNECTED			-3


/*
* Previous declare
*/
static void SetHfpState(BT_HFP_STATE state);
static void SetHfpConnectedAddr(uint8_t * addr);
static void SetScoConnectFlag(bool flag);
static int16_t SetBtCallInPhoneNumber(const uint8_t * number, uint16_t len);
static int16_t SetBtCallWaitingNotify(bool flag);
static int16_t SetBtBatteryLevel(uint8_t level);
static int16_t SetBtHfpSignalLevel(uint8_t level);
static int16_t SetBtHfpRoam(bool flag);
static int16_t SetBtHfpVoiceRecognition(bool flag);
static int16_t SetBtHfpSpeakerVolume(uint8_t gain);

void BtHfpEvent(uint32_t event, BT_STACK_CALLBACK_PARAMS * param)
{
	switch(event)
	{

		case BT_STACK_EVENT_HFP_CONNECTED:
			{
				DBG("Hfp Connected : bt address = %02x:%02x:%02x:%02x:%02x:%02x\n",
						(param->params.bd_addr)[0],
						(param->params.bd_addr)[1],
						(param->params.bd_addr)[2],
						(param->params.bd_addr)[3],
						(param->params.bd_addr)[4],
						(param->params.bd_addr)[5]);

				SetHfpState(BT_HFP_STATE_CONNECTED);

				SetHfpConnectedAddr(param->params.bd_addr);
			}
			break;

		case BT_STACK_EVENT_HFP_DISCONNECTED:
			{
				DBG("Hfp disconnect\n");

				SetHfpState(BT_HFP_STATE_NONE);

				SetHfpConnectedAddr(NULL); // clear connected address
			}
			break;

		case BT_STACK_EVENT_HFP_SCO_CONNECTED:
			{
				DBG("Hfp sco connect\n");

				SetScoConnectFlag(TRUE);

			}
			break;

		case BT_STACK_EVENT_HFP_SCO_DISCONNECTED:
			{
				DBG("Hfp sco disconnect\n");

				SetScoConnectFlag(FALSE);
			}
			break;

		case BT_STACK_EVENT_HFP_CALL_CONNECTED:
			{
				DBG("Hfp call connected\n");

				SetHfpState(BT_HFP_STATE_ACTIVE);
			}
			break;

		case BT_STACK_EVENT_HFP_CALL_DISCONNECTED:
			{
				BT_HFP_STATE	hfpState;
				DBG("Hfp call disconnect\n");

				hfpState = GetHfpState();
				switch(hfpState)
				{
					case BT_HFP_STATE_ACTIVE:
						SetHfpState(BT_HFP_STATE_CONNECTED);
					break;
				}
			}
			break;

		case BT_STACK_EVENT_HFP_CALLSETUP_NONE:
			{
				BT_HFP_STATE	hfpState;
				
				DBG("Hfp call setup none\n");

				hfpState = GetHfpState();
				switch(hfpState)
				{
					case BT_HFP_STATE_INCOMING:
					case BT_HFP_STATE_OUTGOING:
						SetHfpState(BT_HFP_STATE_CONNECTED);
						break;

					default:
						break;
				}
			}
			break;

		case BT_STACK_EVENT_HFP_CALLSETUP_IN:
			{
				DBG("Hfp call setup incoming\n");
				SetHfpState(BT_HFP_STATE_INCOMING);

			}
			break;

		case BT_STACK_EVENT_HFP_CALLSETUP_OUT:
			{
				DBG("Hfp call setup outgoing\n");
				SetHfpState(BT_HFP_STATE_OUTGOING);
			}
			break;

		case BT_STACK_EVENT_HFP_CALLSETUP_ALERT:
			{
				DBG("Hfp call setup alert\n");
				SetHfpState(BT_HFP_STATE_OUTGOING);
			}
			break;

		case BT_STACK_EVENT_HFP_RING:
			{
				DBG("Hfp RING...\n");
				SetHfpState(BT_HFP_STATE_INCOMING);
			}
			break;

		case BT_STACK_EVENT_HFP_CALLER_ID_NOTIFY:
			{
				DBG("Hfp caller id : %s\n", param->params.hfpPhoneNumber);

				SetBtCallInPhoneNumber(param->params.hfpPhoneNumber, param->paramsLen);
			}
			break;

		case BT_STACK_EVENT_HFP_CURRENT_CALLS:
			{
				DBG("Hfp current caller id : %s\n", param->params.hfpPhoneNumber);
			}
			break;

		case BT_STACK_EVENT_HFP_CALL_WAIT_NOTIFY:
			{
				DBG("Hfp call wait : %s\n", param->params.hfpPhoneNumber);

				SetBtCallWaitingNotify(TRUE);

				SetBtCallInPhoneNumber(param->params.hfpPhoneNumber, param->paramsLen);
			}
			break;

		case BT_STACK_EVENT_HFP_BATTERY:
			{

				DBG("Hfp battery level : [%d]\n", param->params.hfpBattery);

				SetBtBatteryLevel(param->params.hfpBattery);
			}
			break;

		case BT_STACK_EVENT_HFP_SIGNAL:
			{
				DBG("Hfp signal level : [%d]\n", param->params.hfpSignal);

				SetBtHfpSignalLevel(param->params.hfpSignal);
			}
			break;

		case BT_STACK_EVENT_HFP_ROAM:
			{
				if(param->params.hfpRoam)
					DBG("Hfp roam TRUE\n");
				else
					DBG("Hfp roam FALSE\n");

				SetBtHfpRoam(param->params.hfpRoam);
			}
			break;

		case BT_STACK_EVENT_HFP_VOICE_RECOGNITION:
			{
				if(param->params.hfpVoiceRec)
					DBG("Hfp vocie recognition TRUE\n");
				else
					DBG("Hfp vocie recognition FALSE\n");

				SetBtHfpVoiceRecognition(param->params.hfpVoiceRec);
			}
			break;

		case BT_STACK_EVENT_HFP_SPEAKER_VOLUME:
			{
				DBG("Hfp speaker vol : [%d]\n", param->params.hfpVolGain);

				SetBtHfpSpeakerVolume(param->params.hfpVolGain);
			}
			break;

		case BT_STACK_EVENT_HFP_IN_BAND_RING:
			{
				if(param->params.hfpInBandRing)
					DBG("Hfp inBandRing TRUE\n");
				else
					DBG("Hfp inBandRing FALSE\n");
			}
			break;
		case BT_STACK_EVENT_HFP_MANUFACTORY_INFO:
			{
				DBG("%s\n", param->params.hfpRemoteManufactory);
			}
			break;

		case BT_STACK_EVENT_HFP_DEVICE_TYPE:
			{
				DBG("%s\n", param->params.hfpRemoteDeviceType);
			}
			break;

		case BT_STACK_EVENT_HFP_UNSOLICITED_DATA:
			{
				uint16_t i;
				
				DBG("Hfp unsolicited data (char): ");
				for(i = 0; i < param->paramsLen; i++)
				{
					DBG("%c", param->params.hfpUnsolicitedData[i]);
				}
				DBG("\n");
			}
			break;

		default:
			break;
	}
}

void BtHfpConnect(uint8_t * addr)
{
	SetHfpState(BT_HFP_STATE_CONNECTING);
	HfpConnect(addr);
}

void BtHfpDisconnect(void)
{
	HfpDisconnect();
}





static void SetHfpState(BT_HFP_STATE state)
{
	BT_MANAGER_ST *	tempBtManager = NULL;

	tempBtManager = GetBtManager();

	if(tempBtManager == NULL)
		return;

	tempBtManager->hfpState = state;
}

static void SetHfpConnectedAddr(uint8_t * addr)
{
	BT_MANAGER_ST *	tempBtManager = NULL;

	tempBtManager = GetBtManager();

	if(tempBtManager == NULL)
		return;

	if(addr == NULL) // clear bt address
		memset(tempBtManager->btDevAddr, 0, sizeof(tempBtManager->btDevAddr));
	else
		memcpy(tempBtManager->btDevAddr, addr, 6);
}

BT_HFP_STATE GetHfpState(void)
{
	BT_MANAGER_ST *	tempBtManager = NULL;

	tempBtManager = GetBtManager();

	if(tempBtManager == NULL)
		return BT_HFP_STATE_NONE;

	return GetBtManager()->hfpState;
}

static void SetScoConnectFlag(bool flag)
{
	BT_MANAGER_ST *	tempBtManager = NULL;

	tempBtManager = GetBtManager();

	if(tempBtManager == NULL)
		return;

	tempBtManager->scoConnected = flag;
}

bool GetScoConnectFlag(void)
{
	BT_MANAGER_ST *	tempBtManager = NULL;

	tempBtManager = GetBtManager();

	if(tempBtManager == NULL)
		return FALSE;

	return tempBtManager->scoConnected;
}

static int16_t SetBtCallInPhoneNumber(const uint8_t * number, uint16_t len)
{
	BT_MANAGER_ST *	tempBtManager = NULL;
	uint16_t		tempLen;

	tempBtManager = GetBtManager();

	if(number == NULL || len == 0 )
		return BT_MANAGER_ERROR_PARAMETER_INVAILD;

	if(tempBtManager == NULL)
		return BT_MANAGER_ERROR_NOT_INITIALIZED;

	if(tempBtManager->hfpState < BT_HFP_STATE_CONNECTED)
		return BT_MANAGER_HFP_ERROR_NOT_CONNECTED;

	tempLen = len > (MAX_PHONE_NUMBER_LENGTH - 1) ? (MAX_PHONE_NUMBER_LENGTH - 1) : len;
	
	memset(tempBtManager->phoneNumber, 0, MAX_PHONE_NUMBER_LENGTH);
	strncpy((char*)tempBtManager->phoneNumber, (char*)number, tempLen);

	return tempLen;
}

int16_t GetBtCallInPhoneNumber(uint8_t * number)
{
	BT_MANAGER_ST *	tempBtManager = NULL;
	uint16_t		tempLen;

	tempBtManager = GetBtManager();

	if(number == NULL)
		return BT_MANAGER_ERROR_PARAMETER_INVAILD;

	if(tempBtManager == NULL)
		return BT_MANAGER_ERROR_NOT_INITIALIZED;

	if(tempBtManager->hfpState < BT_HFP_STATE_CONNECTED)
		return BT_MANAGER_HFP_ERROR_NOT_CONNECTED;

	tempLen = strlen((const char*)tempBtManager->phoneNumber);

	strncpy((char*)number, (const char*)tempBtManager->phoneNumber, tempLen);

	return tempLen;
}

static int16_t SetBtCallWaitingNotify(bool flag)
{
	BT_MANAGER_ST *	tempBtManager = NULL;

	tempBtManager = GetBtManager();

	if(tempBtManager == NULL)
		return BT_MANAGER_ERROR_NOT_INITIALIZED;

	tempBtManager->callWaitingFlag = flag;

	return BT_MANAGER_HFP_ERROR_NONE;
	
}

int16_t GetBtCallWaitingNotify(bool * flag)
{
	BT_MANAGER_ST *	tempBtManager = NULL;

	if(flag == NULL)
		return BT_MANAGER_ERROR_PARAMETER_INVAILD;

	tempBtManager = GetBtManager();

	if(tempBtManager == NULL)
		return BT_MANAGER_ERROR_NOT_INITIALIZED;

	*flag = tempBtManager->callWaitingFlag;

	return BT_MANAGER_HFP_ERROR_NONE;
}

static int16_t SetBtBatteryLevel(uint8_t level)
{
	BT_MANAGER_ST *	tempBtManager = NULL;

	tempBtManager = GetBtManager();

	if(tempBtManager == NULL)
		return BT_MANAGER_ERROR_NOT_INITIALIZED;

	tempBtManager->batteryLevel = level;

	return BT_MANAGER_HFP_ERROR_NONE;
}

int16_t GetBtBatteryLevel(uint8_t * level)
{

	BT_MANAGER_ST *	tempBtManager = NULL;

	if(level == NULL)
		return BT_MANAGER_ERROR_PARAMETER_INVAILD;

	tempBtManager = GetBtManager();

	if(tempBtManager == NULL)
		return BT_MANAGER_ERROR_NOT_INITIALIZED;

	*level = tempBtManager->batteryLevel;

	return BT_MANAGER_HFP_ERROR_NONE;
}


static int16_t SetBtHfpSignalLevel(uint8_t level)
{
	BT_MANAGER_ST *	tempBtManager = NULL;

	tempBtManager = GetBtManager();

	if(tempBtManager == NULL)
		return BT_MANAGER_ERROR_NOT_INITIALIZED;

	tempBtManager->signalLevel = level;

	return BT_MANAGER_HFP_ERROR_NONE;
}

int16_t GetBtSignalLevel(uint8_t * level)
{

	BT_MANAGER_ST *	tempBtManager = NULL;

	if(level == NULL)
		return BT_MANAGER_ERROR_PARAMETER_INVAILD;

	tempBtManager = GetBtManager();

	if(tempBtManager == NULL)
		return BT_MANAGER_ERROR_NOT_INITIALIZED;

	*level = tempBtManager->signalLevel;

	return BT_MANAGER_HFP_ERROR_NONE;
}


static int16_t SetBtHfpRoam(bool flag)
{
	BT_MANAGER_ST *	tempBtManager = NULL;

	tempBtManager = GetBtManager();

	if(tempBtManager == NULL)
		return BT_MANAGER_ERROR_NOT_INITIALIZED;

	tempBtManager->roamFlag = flag;

	return BT_MANAGER_HFP_ERROR_NONE;
	
}

int16_t GetBtHfpRoam(bool * flag)
{
	BT_MANAGER_ST *	tempBtManager = NULL;

	if(flag == NULL)
		return BT_MANAGER_ERROR_PARAMETER_INVAILD;

	tempBtManager = GetBtManager();

	if(tempBtManager == NULL)
		return BT_MANAGER_ERROR_NOT_INITIALIZED;

	*flag = tempBtManager->roamFlag;

	return BT_MANAGER_HFP_ERROR_NONE;
}


static int16_t SetBtHfpVoiceRecognition(bool flag)
{
	BT_MANAGER_ST *	tempBtManager = NULL;

	tempBtManager = GetBtManager();

	if(tempBtManager == NULL)
		return BT_MANAGER_ERROR_NOT_INITIALIZED;

	tempBtManager->voiceRecognition = flag;

	return BT_MANAGER_HFP_ERROR_NONE;
	
}

int16_t GetBtHfpVoiceRecognition(bool * flag)
{
	BT_MANAGER_ST *	tempBtManager = NULL;

	if(flag == NULL)
		return BT_MANAGER_ERROR_PARAMETER_INVAILD;

	tempBtManager = GetBtManager();

	if(tempBtManager == NULL)
		return BT_MANAGER_ERROR_NOT_INITIALIZED;

	*flag = tempBtManager->voiceRecognition;

	return BT_MANAGER_HFP_ERROR_NONE;
}


static int16_t SetBtHfpSpeakerVolume(uint8_t gain)
{
	BT_MANAGER_ST *	tempBtManager = NULL;

	tempBtManager = GetBtManager();

	if(tempBtManager == NULL)
		return BT_MANAGER_ERROR_NOT_INITIALIZED;

	tempBtManager->volGain = gain;

	return BT_MANAGER_HFP_ERROR_NONE;
}

int16_t GetBtHfpSpeakerVolume(uint8_t * gain)
{

	BT_MANAGER_ST *	tempBtManager = NULL;

	if(gain == NULL)
		return BT_MANAGER_ERROR_PARAMETER_INVAILD;

	tempBtManager = GetBtManager();

	if(tempBtManager == NULL)
		return BT_MANAGER_ERROR_NOT_INITIALIZED;

	*gain = tempBtManager->volGain;

	return BT_MANAGER_HFP_ERROR_NONE;
}
