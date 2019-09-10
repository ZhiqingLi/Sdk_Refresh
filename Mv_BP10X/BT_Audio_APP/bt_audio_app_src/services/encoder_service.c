/**
 **************************************************************************************
 * @file    encoder_service.c
 * @brief   
 *
 * @author  pi
 * @version V1.0.0
 *
 * $Created: 2019-2-28 13:06:47$
 *
 * @Copyright (C) 2018, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#include <string.h>

#include "app_config.h"
#include "rtos_api.h"
#include "app_message.h"
#include "type.h"
#include "debug.h"
#include "recorder_service.h"
#include "encoder_service.h"
#include "audio_core_service.h"



#ifdef CFG_FUNC_RECORDER_EN //only for recorder


/***************************************************************************************
 *
 * External defines
 *
 */

#define ENCODER_SERVICE_TASK_STACK_SIZE			512//1024
#define ENCODER_SERVICE_TASK_PRIO				4



/***************************************************************************************
 *
 * Internal defines
 *
 */

typedef struct _EncoderServiceContext
{
	xTaskHandle 		taskHandle;
	MessageHandle		msgHandle;
	MessageHandle		parentMsgHandle;
	TaskState			EncoderServiceState;
	
}EncoderServiceContext;



/***************************************************************************************
 *
 * Internal varibles
 *
 */

#define ENCODER_SERVICE_NUM_MESSAGE_QUEUE		15

static EncoderServiceContext		EncoderServiceCt;

/** remind sound task name*/
const char EncoderServiceName[] = "EncoderService";

/***************************************************************************************
 *
 * Internal functions
 *
 */

/////////////////////////////////////////////////////////////////////////

static int32_t EncoderServiceInit(MessageHandle parentMsgHandle)
{
	memset(&EncoderServiceCt, 0, sizeof(EncoderServiceCt));

	/* message handle */
	EncoderServiceCt.msgHandle = MessageRegister(ENCODER_SERVICE_NUM_MESSAGE_QUEUE);

	/* Parent message handle */
	EncoderServiceCt.parentMsgHandle = parentMsgHandle;
	EncoderServiceCt.EncoderServiceState = TaskStateCreating;
	return 0;
}


static void EncoderServiceEntrance(void * param)
{
	MessageContext		msgRecv;
	MessageContext		msgSend;

	msgSend.msgId		= MSG_ENCODER_SERVICE_CREATED;
	MessageSend(EncoderServiceCt.parentMsgHandle, &msgSend);

	while(1)
	{
		MessageRecv(EncoderServiceCt.msgHandle, &msgRecv, 1);
		switch(msgRecv.msgId)
		{
			case MSG_TASK_START:
				msgSend.msgId		= MSG_ENCODER_SERVICE_STARTED;
				MessageSend(EncoderServiceCt.parentMsgHandle, &msgSend);
				EncoderServiceCt.EncoderServiceState = TaskStateRunning;
				break;

			
			case MSG_TASK_STOP:
				msgSend.msgId		= MSG_ENCODER_SERVICE_STOPPED;
				MessageSend(EncoderServiceCt.parentMsgHandle, &msgSend);
				break;
		}
		if(EncoderServiceCt.EncoderServiceState == TaskStateRunning)
		{
			MediaRecorderEncode();
			MessageContext		msgSend;
			msgSend.msgId		= MSG_NONE;
			MessageSend(GetAudioCoreServiceMsgHandle(), &msgSend);
		}
	}
}


/***************************************************************************************
 *
 * APIs
 *
 */
MessageHandle GetEncoderServiceMessageHandle(void)
{
	return EncoderServiceCt.msgHandle;
}


int32_t EncoderServiceCreate(MessageHandle parentMsgHandle)
{
	int32_t		ret = 0;
	ret = EncoderServiceInit(parentMsgHandle);
	if(!ret)
	{
		EncoderServiceCt.taskHandle = NULL;
		xTaskCreate(EncoderServiceEntrance, EncoderServiceName, ENCODER_SERVICE_TASK_STACK_SIZE, NULL, ENCODER_SERVICE_TASK_PRIO, &EncoderServiceCt.taskHandle);
		if(EncoderServiceCt.taskHandle == NULL)
		{
			ret = -1;
		}
	}
	if(ret)
	{
		DBG("Encoder:%s create fail!\n", EncoderServiceName);
	}
	return ret;
}

void EncoderServiceStart(void)
{
	MessageContext		msgSend;
	msgSend.msgId		= MSG_TASK_START;
	MessageSend(EncoderServiceCt.msgHandle, &msgSend);
}


void EncoderServiceStop(void)
{
	MessageContext		msgSend;

	msgSend.msgId		= MSG_TASK_STOP;
	MessageSend(EncoderServiceCt.msgHandle, &msgSend);
}

void EncoderServiceKill(void)
{
	//task
	if(EncoderServiceCt.taskHandle != NULL)
	{
		vTaskDelete(EncoderServiceCt.taskHandle);
		EncoderServiceCt.taskHandle = NULL;
	}
	
	//Msgbox
	if(EncoderServiceCt.msgHandle != NULL)
	{
		MessageDeregister(EncoderServiceCt.msgHandle);
		EncoderServiceCt.msgHandle = NULL;
	}
	
	//PortFree...
}


#endif //CFG_FUNC_RECORDER_EN

