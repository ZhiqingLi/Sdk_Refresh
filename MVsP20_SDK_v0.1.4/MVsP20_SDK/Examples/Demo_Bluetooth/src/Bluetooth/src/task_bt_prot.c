///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: task_bt.c
///////////////////////////////////////////////////////////////////////////////
#include "type.h"
#include "freeRtos.h"
#include "task.h"
#include "debug.h"
//#include "task_bt_prot.h"
//#include "app_msg.h"
//#include "uart.h"

static xTaskHandle btTaskHandle = 0;

static void BtTaskEntrance(void * param)
{
	if(!InitBt())
	{
		DBG("error init bt device\n");
		while(1);
	}

	while(1)
	{
		BtStackProcess();
	}
}

static void EnterBtProcess(void)
{
	if(!InitBt())
	{
		DBG("error init bt device\n");
		while(1);
	}
	while(1)
	{
		BtStackProcess();
	}
}
bool StartBtProtStackTask(uint16_t taskSize, void * taskParams, uint16_t taskPrio)
{
	xTaskCreate( (void*)BtTaskEntrance, "Bluetooth", taskSize, taskParams, taskPrio, &btTaskHandle );
//	EnterBtProcess();
	return TRUE;
}


void StopBtProtStackTask(void)
{
	vTaskDelete(btTaskHandle);
	UninitBt();
}

