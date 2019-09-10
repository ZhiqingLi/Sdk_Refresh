#include "dump.h"
#include "ff.h"
#include "sd_card.h"
#include "debug.h"
#include "gpio.h"
#include "timer.h"
#include "irqn.h"
#include "otg_detect.h"
#include "otg_host_hcd.h"
#include "otg_host_standard_enum.h"
#include "otg_host_udisk.h"

//定义一个buf，用于缓存写卡数据
uint8_t DUMP_Buf[1024 * 20];
#define DUMP_SIZE				sizeof(DUMP_Buf)


bool 	IsFsMounted      = FALSE;
bool 	IsDumpFileOpened = FALSE;
FATFS 	gFatfs_sd;   				/* File system object */
FIL 	gFil;
//static FATFS  gDumpFile;
uint32_t PPPP;

bool SdCardInit(void);

bool CheckAllDiskLinkFlag(void)
{
	return TRUE;
}

bool IsCardLink(void)
{
	return TRUE;
}

bool IsUDiskLink(void)
{
	return TRUE;
}

void HIDSendData(uint8_t *pBuffer,uint32_t RxLength)
{

}

void HIDReceive(uint8_t *pBuffer)
{

}

uint32_t StringConvert(uint8_t* DstText, uint32_t MaxDstLen, uint8_t* SrcText, uint32_t SrcLen, uint32_t Type)
{
    return 0;
}

bool IsSdInitedAndFsMountedOK(void)
{
    return IsFsMounted;
}

void Timer2Interrupt(void)
{
	Timer_InterruptFlagClear(TIMER2, UPDATE_INTERRUPT_SRC);
	OTG_PortLinkCheck();
}

bool SdInitAndFsMount(void)
{
    IsFsMounted = FALSE;

	//SDIO config
	#if 0
	GPIO_PortAModeSet(GPIOA15, 1);
	GPIO_PortAModeSet(GPIOA16, 2);
	GPIO_PortAModeSet(GPIOA17, 1);
	GPIO_RegOneBitSet(GPIO_A_PU, GPIO_INDEX15);
	GPIO_RegOneBitClear(GPIO_A_PD, GPIO_INDEX15);
	GPIO_RegOneBitSet(GPIO_A_PU, GPIO_INDEX16);
	GPIO_RegOneBitClear(GPIO_A_PD, GPIO_INDEX16);
	GPIO_RegOneBitSet(GPIO_A_PU, GPIO_INDEX17);
	GPIO_RegOneBitClear(GPIO_A_PD, GPIO_INDEX17);

	//DBG("ready to check SDCard...\n");

	while(Cnt--)
	{
		if(SDCard_Init() == NONE_ERR)
		{
			DBG("SDCard Init Success!\n");
			break;
		}
		else
		{
			DBG("SdInit Failed!\n");
			goto end;
		}
	}



	if(f_mount(&gFatfs_sd, "0:/", 1) == 0)
	{
		DBG("SD卡挂载到 0:/--> 成功\n");
	}
	else
	{
		DBG("SD卡挂载到 0:/--> 失败\n");
		goto end;
	}
#else
	Timer_Config(TIMER2,1000,0);
	Timer_Start(TIMER2);
	NVIC_EnableIRQ(Timer2_IRQn);

	{
		OTG_HostControlInit();
		OTG_HostEnumDevice();
		if(UDiskInit() == 1)
		{
			DBG("枚举MASSSTORAGE接口OK\n");
			if(f_mount(&gFatfs_sd, "1:/", 1) == 0)
			{
				DBG("U盘卡挂载到 1:/--> 成功\n");
			}
			else
			{
				DBG("U盘卡挂载到 1:/--> 失败\n");
				goto end;
			}
		}
		else
		{
			DBG("UDisk init Failed!\n");
			goto end;
		}
	}
#endif
	IsFsMounted = TRUE;

	DumpBufferInit((uint8_t*)DUMP_Buf, DUMP_SIZE);

end:
    return IsFsMounted;
}

//name must be a short name!!!
bool OpenDumpDataFile(void)
{
    if(IsFsMounted && DUMP_FLAG)
    {
        //IsDumpFileOpened = FileOpen(&gDumpFile, name, FA_WRITE | FA_CREATE_ALWAYS);
    	if(f_open(&gFil, "1:/aaa.pcm", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
    	{
    		DBG("Open file error!\n");
    		IsDumpFileOpened = FALSE;
    	}
    	else
    	{
    		DBG("Open File ok!\n");
    		IsDumpFileOpened = TRUE;
    	}
    }
    return IsDumpFileOpened;
}

void OpenDumpDataFileInAppendMode(uint8_t* name)
{
//    if(IsFsMounted && DUMP_FLAG)
//    {
//        IsDumpFileOpened = FileOpen(&gDumpFile, name, FA_WRITE);
//
//        if(IsDumpFileOpened)
//        {
//            FileSeek(&gDumpFile, 0, SEEK_FILE_END);
//        }
//        else
//        {
//            IsDumpFileOpened = FileOpen(&gDumpFile, name, FA_WRITE | FA_CREATE_NEW);
//        }
//    }
}

bool IsDumpFileOpenedOk(void)
{
    return IsDumpFileOpened;
}

void DumpData(void* data, uint32_t len)
{
	FRESULT ret;
	//return;
  if(IsDumpFileOpened && DUMP_FLAG)
  {
		ret = f_write(&gFil, data, len, (UINT*)&PPPP);
		if(FR_OK != ret)
		{
			DBG("%d\n", ret);
		}
		else
		{
			//DBG(".");
		}
  }
}

void SaveDumpDataFile(void)
{
    if(IsDumpFileOpened && DUMP_FLAG)
    {
        //FileSave(&gFil);
    }
}

void CloseDumpDataFile(void)
{
    if(IsDumpFileOpened && DUMP_FLAG)
    {
    	f_close(&gFil);
        IsDumpFileOpened = FALSE;
    }
}

#include <string.h>

#define MIN(A, B)           ((A) < (B) ? (A) : (B))
#define	MAX(A, B)           ((A) > (B) ? (A) : (B))

static uint8_t* gDumpBuffer;
static uint32_t gDumpBufferCapacity;
static uint32_t gDumpBufferPushPointer;
static uint32_t gDumpBufferPopPointer;

void DumpBufferInit(uint8_t* buffer, uint32_t capacity)
{
    gDumpBuffer            = buffer;
    gDumpBufferCapacity    = capacity;
    gDumpBufferPushPointer = 0;
    gDumpBufferPopPointer  = 0;
}

uint32_t DumpBufferRemain(void)
{
    return gDumpBufferPushPointer >= gDumpBufferPopPointer ? gDumpBufferCapacity - gDumpBufferPushPointer + gDumpBufferPopPointer : gDumpBufferPopPointer - gDumpBufferPushPointer;
}

uint32_t DumpBufferPush(uint8_t* buffer, uint32_t length)
{
    uint32_t RemainBytes;
    uint32_t PopPointer = gDumpBufferPopPointer;

    if(gDumpBufferPushPointer >= PopPointer)
    {
        RemainBytes = gDumpBufferCapacity - gDumpBufferPushPointer + gDumpBufferPopPointer;
        if(RemainBytes >= length)
        {
            RemainBytes = gDumpBufferCapacity - gDumpBufferPushPointer;
            if(RemainBytes >= length)
            {
                memcpy(&gDumpBuffer[gDumpBufferPushPointer], buffer, length);
                gDumpBufferPushPointer += length;
            }
            else
            {
                memcpy(&gDumpBuffer[gDumpBufferPushPointer], buffer, RemainBytes);
                gDumpBufferPushPointer = length - RemainBytes;
                memcpy(&gDumpBuffer[0], &buffer[RemainBytes], gDumpBufferPushPointer);
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        RemainBytes = PopPointer - gDumpBufferPushPointer;
        if(RemainBytes >= length)
        {
            memcpy(&gDumpBuffer[gDumpBufferPushPointer], buffer, length);
            gDumpBufferPushPointer += length;
        }
        else
        {
            return 0;
        }
    }

    if(gDumpBufferPushPointer >= gDumpBufferCapacity)
    {
        gDumpBufferPushPointer = 0;
    }

    return length;
}

uint32_t DumpBufferPop(void)
{
    uint32_t PopBytes;
    uint32_t PushPointer = gDumpBufferPushPointer;

    if(PushPointer >= gDumpBufferPopPointer)
    {
        PopBytes = PushPointer - gDumpBufferPopPointer;
        DumpData(&gDumpBuffer[gDumpBufferPopPointer], PopBytes);
        gDumpBufferPopPointer += PopBytes;
    }
    else
    {
        PopBytes = gDumpBufferCapacity - gDumpBufferPopPointer;
        DumpData(&gDumpBuffer[gDumpBufferPopPointer], PopBytes);
        DumpData(&gDumpBuffer[0], PushPointer);
        PopBytes += PushPointer;
        gDumpBufferPopPointer = PushPointer;
    }

    if(gDumpBufferPopPointer >= gDumpBufferCapacity)
    {
        gDumpBufferPopPointer = 0;
    }

    return PopBytes;
}

uint32_t DumpBufferPopByBlock(void)
{
    uint32_t PopBytes;
    uint32_t PushPointer = gDumpBufferPushPointer;

    if(PushPointer >= gDumpBufferPopPointer)
    {
        PopBytes = ((PushPointer - gDumpBufferPopPointer)>>9)<<9;
        if(PopBytes)
        {
            DumpData(&gDumpBuffer[gDumpBufferPopPointer], PopBytes);
            gDumpBufferPopPointer += PopBytes;
        }
    }
    else
    {
        PopBytes = ((gDumpBufferCapacity - gDumpBufferPopPointer + PushPointer)>>9)<<9;
        if(PopBytes)
        {
            uint32_t len = gDumpBufferCapacity - gDumpBufferPopPointer;
            len = MIN(len, PopBytes);
            if(len)
            {
                DumpData(&gDumpBuffer[gDumpBufferPopPointer], len);
                gDumpBufferPopPointer += len;
            }
            len = PopBytes-len;
            if(len)
            {
                DumpData(&gDumpBuffer[0], len);
                gDumpBufferPopPointer = len;
            }
        }
    }

    if(gDumpBufferPopPointer >= gDumpBufferCapacity)
    {
        gDumpBufferPopPointer = 0;
    }

    return PopBytes;
}
