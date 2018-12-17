/**
 *******************************************************************************
 * @file    bt_avrcp_app.c
 * @author  Halley
 * @version V1.0.1
 * @date    27-Apr-2016
 * @brief   Avrcp callback events and actions
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


static void SetAvrcpState(BT_AVRCP_STATE state);

void BtAvrcpEvent(uint32_t event, BT_STACK_CALLBACK_PARAMS * param)
{
	switch(event)
	{

		case BT_STACK_EVENT_AVRCP_CONNECTED:
			{
				DBG("Avrcp Connected : bt address = %02x:%02x:%02x:%02x:%02x:%02x\n",
						(param->params.bd_addr)[0],
						(param->params.bd_addr)[1],
						(param->params.bd_addr)[2],
						(param->params.bd_addr)[3],
						(param->params.bd_addr)[4],
						(param->params.bd_addr)[5]);

				SetAvrcpState(BT_AVRCP_STATE_CONNECTED);
			}
			break;

		case BT_STACK_EVENT_AVRCP_DISCONNECTED:
			{
				DBG("Avrcp disconnect\n");
				SetAvrcpState(BT_AVRCP_STATE_NONE);
			}
			break;

		default:
			break;
	}
}

static void SetAvrcpState(BT_AVRCP_STATE state)
{
	GetBtManager()->avrcpState = state;
}

BT_AVRCP_STATE GetAvrcpState(void)
{
	return GetBtManager()->avrcpState;
}

