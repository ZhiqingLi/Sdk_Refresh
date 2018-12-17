/**
 *******************************************************************************
 * @file    bt_common_app.c
 * @author  Halley
 * @version V1.0.1
 * @date    27-Apr-2016
 * @brief   Common callback events and actions
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
#include "bt_manager.h"

void BtCommandEvent(uint32_t event, BT_STACK_CALLBACK_PARAMS * param)
{
	switch(event)
	{
		case BT_STACK_EVENT_COMMON_STACK_INITIALIZED:
		{
		/*
			fc:e0:e4:67:c7:e0
			uint8_t addr[6] = {0xfc, 0xe0, 0xe4, 0x67, 0xc7, 0xe0};
			HfpConnect(addr);
		*/
		}
		break;

		case BT_STACK_EVENT_COMMON_INQUIRY_RESULT:
		{
			DBG("Inquriy Result:\n");
			DBG("\t%02x:%02x:%02x:%02x:%02x:%02x",
					(param->params.inquiryResult.addr)[0],
					(param->params.inquiryResult.addr)[1],
					(param->params.inquiryResult.addr)[2],
					(param->params.inquiryResult.addr)[3],
					(param->params.inquiryResult.addr)[4],
					(param->params.inquiryResult.addr)[5]);
			DBG("\t %d dB", param->params.inquiryResult.rssi);
			DBG("\t extLen = %d , ext = %s \n",param->params.inquiryResult.extRespLen, param->params.inquiryResult.extResp);
		}
		break;

		case BT_STACK_EVENT_COMMON_INQUIRY_COMPLETE:
		{
			DBG("BT_STACK_EVENT_COMMON_INQUIRY_COMPLETE\n");
		}
		break;

		case BT_STACK_EVENT_COMMON_INQUIRY_CANCELED:
		{
			DBG("BT_STACK_EVENT_COMMON_INQUIRY_CANCELED\n");
		}
		break;


		case BT_STACK_EVENT_COMMON_MODE_CHANGE:
			DBG("mode - %d, addrs -  %02x:%02x:%02x:%02x:%02x:%02x\n",
					param->params.modeChange.mode,
					(param->params.modeChange.addr)[0],
					(param->params.modeChange.addr)[1],
					(param->params.modeChange.addr)[2],
					(param->params.modeChange.addr)[3],
					(param->params.modeChange.addr)[4],
					(param->params.modeChange.addr)[5]);
			if(param->params.modeChange.mode == BTLinkModeSniff)
			{
//				SwitchModeTo(APP_MODE_IDLE);
			}
		break;

		case BT_STACK_EVENT_COMMON_GET_REMDEV_NAME:
		{
			DBG("Remote Device:\n");
			DBG("\t%02x:%02x:%02x:%02x:%02x:%02x",
					(param->params.remDevName.addr)[0],
					(param->params.remDevName.addr)[1],
					(param->params.remDevName.addr)[2],
					(param->params.remDevName.addr)[3],
					(param->params.remDevName.addr)[4],
					(param->params.remDevName.addr)[5]);
			DBG("\t nameLen = %d , name = %s \n",param->params.remDevName.nameLen, param->params.remDevName.name);
		}
		break;

		case BT_STACK_EVENT_COMMON_ACCESS_MODE:
		{
			DBG("AccesMode = %d\n", param->params.accessMode);
		}
		break;

		default:
		break;
	}
}

void BtTrustListEvent(uint32_t event, BT_STACK_CALLBACK_PARAMS * param)
{
	switch(event)
	{
		case BT_STACK_EVENT_TRUST_LIST_ADD_NEW_RECORD:
			{
				DBG("A new device has been added into db : bt address = %02x:%02x:%02x:%02x:%02x:%02x\n",
						(param->params.bd_addr)[0],
						(param->params.bd_addr)[1],
						(param->params.bd_addr)[2],
						(param->params.bd_addr)[3],
						(param->params.bd_addr)[4],
						(param->params.bd_addr)[5]);
			}
			break;

		case BT_STACK_EVENT_TRUST_LIST_DELETE_RECORD:
			break;

		default:
			break;
	}
}


