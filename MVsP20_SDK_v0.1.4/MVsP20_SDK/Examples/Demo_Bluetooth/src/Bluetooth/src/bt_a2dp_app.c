/**
 *******************************************************************************
 * @file    bt_a2dp_app.h
 * @author  Halley
 * @version V1.0.1
 * @date    27-Apr-2016
 * @brief   A2dp callback events and actions
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

static void SetA2dpState(BT_A2DP_STATE state);

void BtA2dpEvent(uint32_t event, BT_STACK_CALLBACK_PARAMS * param)
{
	switch(event)
	{
		case BT_STACK_EVENT_A2DP_CONNECTED:
		{
			DBG("A2dp Connected : bt address = %02x:%02x:%02x:%02x:%02x:%02x\n",
					(param->params.bd_addr)[0],
					(param->params.bd_addr)[1],
					(param->params.bd_addr)[2],
					(param->params.bd_addr)[3],
					(param->params.bd_addr)[4],
					(param->params.bd_addr)[5]);

			SetA2dpState(BT_A2DP_STATE_CONNECTED);
		}

		break;

		case BT_STACK_EVENT_A2DP_DISCONNECTED:
		{
			DBG("A2dp disconnect\n");
			SetA2dpState(BT_A2DP_STATE_NONE);
		}
		break;

		case BT_STACK_EVENT_A2DP_STREAM_START:
		{
			DBG("A2dp streaming...\n");
			SetA2dpState(BT_A2DP_STATE_STREAMING);
		}
		break;

		case BT_STACK_EVENT_A2DP_STREAM_SUSPEND:
		{
			DBG("A2dp suspend\n");
			SetA2dpState(BT_A2DP_STATE_CONNECTED);
		}
		break;

		case BT_STACK_EVENT_A2DP_STREAM_DATA_IND:
		{
		}
		break;

		default:
		break;
	}
}


static void SetA2dpState(BT_A2DP_STATE state)
{
	GetBtManager()->a2dpState = state;
}

BT_A2DP_STATE GetA2dpState(void)
{
	return GetBtManager()->a2dpState;
}

