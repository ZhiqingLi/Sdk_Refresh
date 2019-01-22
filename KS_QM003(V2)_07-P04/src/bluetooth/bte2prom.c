////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2011, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:bte2prom.c
//
//		Description	:
//					BT Flash saving code
//
//		Changelog	:
//					yzhou@2013.10.16 initial version
///////////////////////////////////////////////////////////////////////////////
#include "type.h"
#include "app_config.h"
#include "os.h"
#include "spi_flash.h"
#include "nvm.h"
#include "bt_stack_api.h"
#include "os.h"
#include "gd_flash_drv.h"
#include "gpio.h"

//#define SHOW_BT_PAIRING_INFO 

/***********************************************
64KB at least
***********************************************/
#ifndef DWNLDR_USRDAT_ADDR
#define DWNLDR_USRDAT_ADDR      (0x90)
#endif

#define MAX_BT_PAIRING_DATA_SIZE    (52*1024)
#define MAX_BT_ALIVE_ADDR_DATA_SIZE (8*1024)

//max is 64KB, Total size MUST be 4KB aligned.
#define ALIVE_ADDR_REC_SIZE  (9)


//Bukk:重新划分区域,用于保存PAIRED_NAME信息
//StartOffset: 0x1f0000
//Size:8K
#define BT_PAIRED_LIST_ADDR_START	0x1f0000
#define BT_PAIRED_LIST_ADDR_END		0x1f2000//0x1f1000
#define MAX_BT_PAIRED_LIST_SIZE		(8*1024)//(4*1024)
//block: blockstatus(1) + flag(3) + size(68)*number(8)
#define BT_PAIRED_LIST_BLOCK_SIZE	548
//size: index(1)+addr(6)+devicename(60)+profile(1)
#define BT_PAIRED_LIST_REC_SIZE		68
//head: blockstatus(1) + flag(3)
#define BT_PAIRED_LIST_BLOCK_HEAD	4

#ifndef BT_DEVICE_NUM
#define BT_DEVICE_NUM				8 //支持最多8个蓝牙设备信息保存
#endif

#define BLOCK_UNUSED		0xff
#define BLOCK_USED			0xfe
#define BLOCK_FAIL			0x00

typedef struct
{
	uint8_t 		index;			//1byte
	uint8_t         bdAddr[6];		//6bytes
	uint8_t			deviceName[60];	//60bytes
    BtLinkedProfile linkedProfile;	//1byte  //note:为了节省对FLASH空间的擦写操作,PROFILE对应的保存的数据标志位,0=有效,1=无效

}BT_PAIRED_LIST_INFO;  //68byts

BT_PAIRED_LIST_INFO btPairedList[BT_DEVICE_NUM];

//bukk
//全局变量 用于保存device name and profile
static uint32_t BtPairedListOffset=BT_PAIRED_LIST_ADDR_START;  //pairList偏置地址
static bool  BtPairedListFlag=0; //PairedList更新标志
//static bool  BtPairedListSync=0; //和BT LIB配对信息同步标志

static uint32_t FlshUpdateAllPairedList(const uint32_t offset);
static bool FlshPairedListSync(int index, uint8_t *addr);
static bool FlshErasePairedListArea(void);
static uint32_t FlshSavePairedList(void);
static uint8_t GetpairedListIndex(uint8_t *addr);
static void FlshPairedListRegInit(void);
uint8_t BTDeleteRecord(uint8_t *addr);
uint32_t FlshLoadAllPairedListInfor(void);


static uint32_t FlshGetAliveAddrWriteOffset(uint32_t StartOffset)
{
	uint32_t i;
	uint8_t Tmp[4];
	uint32_t Step = ALIVE_ADDR_REC_SIZE;//9 BYTE for alive addr (3 majic, 6 addr)
		
	for(i = StartOffset; i <= StartOffset + MAX_BT_ALIVE_ADDR_DATA_SIZE  - Step ; i += Step)
	{
		SpiFlashRead(i, Tmp, 3);
		if((Tmp[0] == 'O') && (Tmp[1] == '1') && (Tmp[2] == '8'))
		{
			if(i != StartOffset)
			{
				return i - Step;
			}
			else
			{
				break; //data is full
			}
		}
		else if((Tmp[0] != 0xFF) || (Tmp[1] != 0xFF) || (Tmp[2] != 0xFF)) //some error data found, then skip it
		{
			if(i != StartOffset)
			{
				return i - Step;
			}
			else
			{
				break; //data is full
			}
		}
	}

	if(i > StartOffset + MAX_BT_ALIVE_ADDR_DATA_SIZE - Step)
	{
		//empty data aera, for first usage
		return (StartOffset + MAX_BT_ALIVE_ADDR_DATA_SIZE - (MAX_BT_ALIVE_ADDR_DATA_SIZE % Step) - Step);
	}

	//not found or buffer is full now
#ifdef FLASH_ERASE_IN_TCM
	SpiFlashEraseInTcm(StartOffset, MAX_BT_ALIVE_ADDR_DATA_SIZE);
#else
	SpiFlashErase(StartOffset, MAX_BT_ALIVE_ADDR_DATA_SIZE);
#endif
	return (StartOffset + MAX_BT_ALIVE_ADDR_DATA_SIZE - (MAX_BT_ALIVE_ADDR_DATA_SIZE % Step) - Step);

}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
bool FlshGetAliveAddr(uint8_t* Addr)
{
	uint32_t i;
	uint8_t Tmp[4];
	uint32_t StartOffset, Step = ALIVE_ADDR_REC_SIZE; //9 BYTE for alive addr (3 majic, 6 addr)

//    SpiFlashInfoInit();

	if(SpiFlashRead(DWNLDR_USRDAT_ADDR, (uint8_t*)&StartOffset, 4) < 0)
	{
		return 0;//error
	}

	StartOffset += MAX_BT_PAIRING_DATA_SIZE;

	for(i = StartOffset; i < StartOffset + MAX_BT_ALIVE_ADDR_DATA_SIZE / 9 * 9; i += Step)
	{
		SpiFlashRead(i, Tmp, 3);
		if((Tmp[0] == 'O') && (Tmp[1] == '1') && (Tmp[2] == '8'))
		{
			SpiFlashRead(i + 3, Addr, 6);
			return 1;
		}
		else if((Tmp[0] != 0xFF) || (Tmp[1] != 0xFF) || (Tmp[2] != 0xFF)) //some error data found, then skip it
		{
			break; //error data found, I just return fail.
		}
	}

	//not found or buufer is full now
	return 0;
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
bool FlshUpdateBtLatestDevAddr(uint8_t* BtAddr)
{
	uint32_t StartOffset, AliveOffset;
	uint32_t TmpOffset;
	uint8_t Tmp[4];

	//SpiFlashInfoInit();

	if(SpiFlashRead(DWNLDR_USRDAT_ADDR, (uint8_t*)&StartOffset, 4) < 0)
	{
		return 0;//error
	}

	AliveOffset = StartOffset + MAX_BT_PAIRING_DATA_SIZE;
	TmpOffset = FlshGetAliveAddrWriteOffset(AliveOffset);
	DBG("StartOffset = %x\n", StartOffset);
	DBG("AliveOffset = %x\n", AliveOffset);
	
	DBG("Addr write Offset = %x\n", TmpOffset);
	if(TmpOffset >= AliveOffset)
	{
		Tmp[0] = 'O';
		Tmp[1] = '1';
		Tmp[2] = '8';
		SpiFlashWrite(TmpOffset, Tmp, 3);
		return (0 == SpiFlashWrite(TmpOffset + 3, BtAddr, 6)) ? 1 : 0;
	}

	return 0; //error
}

//---------------------------------------------------------------------------------
// 1 mean OK, 0 means error or do NOT use flash-saving function
static bool FlshGetBtPairingInfoOffsetCallBk(uint32_t* StartOffset, uint32_t* MaxSize)
{
	//SpiFlashInfoInit();

	if(SpiFlashRead(DWNLDR_USRDAT_ADDR, (uint8_t*)StartOffset, 4) != 0)
	{
		return 0;//error
	}

	*MaxSize = MAX_BT_PAIRING_DATA_SIZE;

	return  1;
}

static uint32_t FlshGetPairingInfoWriteOffset(uint32_t StartOffset, uint32_t Step)
{
	uint32_t i;
	uint8_t Tmp[4];

	for(i = StartOffset; i <= StartOffset + MAX_BT_PAIRING_DATA_SIZE - Step - 4/* 4 byte Magic Number */; i += Step)
	{
		SpiFlashRead(i, Tmp, 3);
		if((Tmp[0] == 'O') && (Tmp[1] == '1') && (Tmp[2] == '8'))
		{
			if(i != StartOffset)
			{
				return i - Step;
			}
			else
			{
				break; //"data is full, need to erase Flash
			}
		}
		else
		{
			if((Tmp[0] != 0xFF) || (Tmp[1] != 0xFF) || (Tmp[2] != 0xFF)) //some error data found, then skip it
			{
				if(i != StartOffset)
				{
					return i - Step;
				}
				else
				{
					break; //data is full
				}
			}
		}
	}
	
	if( i > StartOffset + MAX_BT_PAIRING_DATA_SIZE - Step -4) /* 4 means 4 byte Magic Number */
	{
		//empty DATA aera, for first usage
		return (StartOffset + MAX_BT_PAIRING_DATA_SIZE - 4 - ((MAX_BT_PAIRING_DATA_SIZE - 4) % Step) - Step);
	}
		

	//erase & write magic number
#ifdef FLASH_ERASE_IN_TCM
	SpiFlashEraseInTcm(StartOffset, MAX_BT_PAIRING_DATA_SIZE);
#else
	SpiFlashErase(StartOffset, MAX_BT_PAIRING_DATA_SIZE);
#endif
	Tmp[0] = 'P';
	Tmp[1] = 'R';
	Tmp[2] = 'I';
	Tmp[3] = 'F';
	SpiFlashWrite(StartOffset + MAX_BT_PAIRING_DATA_SIZE - 4, Tmp, 4);	//write magic number
	return (StartOffset + MAX_BT_PAIRING_DATA_SIZE - 4 - ((MAX_BT_PAIRING_DATA_SIZE - 4) % Step) - Step);

}



//write the magic number "PRIF" at the end of PAIRING DATA BANK
//call this funtcion before using BT
bool FlshBTInfoAreaInit(void)
{
	uint32_t UserDataAddrBegin;
	uint8_t Tmp[4];
	
	if(SpiFlashRead(DWNLDR_USRDAT_ADDR, (uint8_t*)&UserDataAddrBegin, 4) != 0)
	{
		return FALSE;
	}
	
	SpiFlashRead(UserDataAddrBegin + MAX_BT_PAIRING_DATA_SIZE - 4, Tmp, 4);
	if((Tmp[0] == 'P') && (Tmp[1] == 'R') && (Tmp[2] == 'I' )&& (Tmp[3] == 'F'))
	{
		//initialized
		return TRUE;
	}
	else
	{
		//for first initialization, erase Flash and write magic number
		//erase all PAIRING info bank and alive address bank 52K+8K
#ifdef FLASH_ERASE_IN_TCM
		SpiFlashEraseInTcm(UserDataAddrBegin, MAX_BT_PAIRING_DATA_SIZE + MAX_BT_ALIVE_ADDR_DATA_SIZE);
#else
		SpiFlashErase(UserDataAddrBegin, MAX_BT_PAIRING_DATA_SIZE + MAX_BT_ALIVE_ADDR_DATA_SIZE);
#endif
		
		Tmp[0] = 'P';
		Tmp[1] = 'R';
		Tmp[2] = 'I';
		Tmp[3] = 'F';
		SpiFlashWrite(UserDataAddrBegin + MAX_BT_PAIRING_DATA_SIZE - 4, Tmp, 4);	//write magic number
		return TRUE;
		
	}

	//FlshLoadAllPairedListInfor();
}	


//save nvram info to flash, bt callback function
bool FlshUpDateBtPairingInfoCallBk(uint32_t NvRamOffset, uint32_t Step/*full record block step size*/, uint32_t NvRamDataSize/**/)
{
	uint32_t StartOffset, t;
	uint8_t Tmp[4];

	if(NvRamDataSize + 3 > Step)
	{
		return 0; //error, MUST including 3 majic number
	}

	if(!FlshGetBtPairingInfoOffsetCallBk(&StartOffset, &t))
	{
		return 0;
	}

	StartOffset = FlshGetPairingInfoWriteOffset(StartOffset, Step);

	DBG("-----------------------------------0x%x\n", StartOffset);
	Tmp[0] = 'O';
	Tmp[1] = '1';
	Tmp[2] = '8';
	SpiFlashWrite(StartOffset, Tmp, 3);
	StartOffset += 3;
	while(NvRamDataSize)
	{
		if(NvRamDataSize > 4)
		{
			t = 4;
		}
		else
		{
			t = NvRamDataSize;
		}

		NvmRead(NvRamOffset, Tmp, t);

		SpiFlashWrite(StartOffset, Tmp, t);

		StartOffset += t;
		NvRamDataSize -= t;
		NvRamOffset += t;
	}

	return 1;
}

//load flash info to nvram if any.
bool FlshLoadBtPairingInfoCallBk(uint32_t NvRamOffset, uint32_t RecStep)
{
	uint32_t StartOffset, t, i, j;
	uint8_t Tmp[4];

	if(!FlshGetBtPairingInfoOffsetCallBk(&StartOffset, &t))
	{
		return 0;
	}

	Tmp[0] = 0;
	NvmWrite(NvRamOffset, Tmp, 1); //save init default value

	for(i = StartOffset; i <= StartOffset + MAX_BT_PAIRING_DATA_SIZE - RecStep; i += RecStep)
	{
		SpiFlashRead(i, Tmp, 3);
		if((Tmp[0] == 'O') && (Tmp[1] == '1') && (Tmp[2] == '8'))
		{
			SpiFlashRead(i + 3, Tmp, 1); //rec count

			NvmWrite(NvRamOffset, Tmp, 1); //count

			t = (uint32_t)(Tmp[0]) * 23;
			i += 4;
			NvRamOffset++;

			while(t)
			{
				if(t >= 4)
				{
					j = 4;
				}
				else
				{
					j = t;
				}

				SpiFlashRead(i, Tmp, j);
				NvmWrite(NvRamOffset, Tmp, j);
				NvRamOffset += j;
				t -= j;
				i += j;
			}
			return 1;
		}
	}

	return 0;
}

#ifdef SHOW_BT_PAIRING_INFO
static void ShowFlashLoadResult(int RecIdxFromZero, unsigned char *BtAddr,unsigned char *LinkKey,unsigned char *Property);
#endif
//save up to 8 bt device rec method
//up to 8 device supported
#if 1
//读取一个设备的配对信息，返回1表示加载成功
typedef int(*FuncGet1of8RecInfo)(uint8_t RecIdx/*from 0*/, uint8_t* Data/*23B*/);
int FlshLoad1of8Dev(int OneFullRecBlockSize, int RecIdxFromZero, uint8_t* BtAddr, uint8_t* LinkKey, uint8_t* Property)
{
	uint8_t Tmp[3];
	uint32_t i, StartOffset;

	//step 1 get the legal offset
	if(!FlshGetBtPairingInfoOffsetCallBk(&StartOffset, &i))
	{
		return 0;
	}
    
    OneFullRecBlockSize += 3;//including 3Bytes of sync data,zhouyi,20140411

	//step 2 get the data
	for(i = StartOffset; i <= StartOffset + MAX_BT_PAIRING_DATA_SIZE - OneFullRecBlockSize; i += OneFullRecBlockSize)
	{
//		OSRescheduleTimeout(2);
		SpiFlashRead(i, Tmp, 3);
		if((Tmp[0] == 'O') && (Tmp[1] == '1') && (Tmp[2] == '8'))
		{
			SpiFlashRead(i + 3, Tmp, 1); //rec count

			DBG("RecIdx = [%d]\n", Tmp[0]);

			if(RecIdxFromZero >= (uint32_t)Tmp[0])
			{
				uint8_t cnt;
				for(cnt = Tmp[0]; cnt<BT_DEVICE_NUM; cnt++)
				{
					if(btPairedList[cnt].index < BT_DEVICE_NUM)
					{
						memset(&btPairedList[cnt], 0xff, sizeof(BT_PAIRED_LIST_INFO));
						btPairedList[cnt].linkedProfile = 0x00;

						BtPairedListFlag=1;
					}
				}
				
				if(BtPairedListFlag) FlshSavePairedList();
				return 0;//not found
			}

			i = i + RecIdxFromZero * 23 + 4;

			//get addr
			SpiFlashRead(i, BtAddr, 6);

			//key
			SpiFlashRead(i + 6, LinkKey, 16);

			//property
			SpiFlashRead(i + 22, Property, 1);

#ifdef SHOW_BT_PAIRING_INFO            
            ShowFlashLoadResult(RecIdxFromZero, BtAddr,LinkKey,Property);
#endif            

			FlshPairedListSync(RecIdxFromZero, BtAddr);
			if((RecIdxFromZero == 7)&&(BtPairedListFlag))
				FlshSavePairedList();
			return 1;
		}
	}

	return 0;
}

static uint8_t OneBtRec[23];

#ifdef SHOW_BT_PAIRING_INFO
static void ShowOneDevSavedPairingInfo(int RecId,int AddrOffset, int Size);
static void ShowOneDevMemoryPairedInfo(int RecId,unsigned char* Info, int Size);
#endif

void FlashSave8DevRec(int OneFullRecBlockSize, int TotalRecNum, FuncGet1of8RecInfo GetOneInf)
{
	uint8_t i, Tmp[3];
	uint32_t t, StartOffset;
	uint8_t addr[6];
	//step 1 get the legal offset
	if(!FlshGetBtPairingInfoOffsetCallBk(&StartOffset, &t))
	{
		return;
	}
    
    OneFullRecBlockSize += 3;//including 3Bytes of sync data,zhouyi,20140411

	StartOffset = FlshGetPairingInfoWriteOffset(StartOffset, OneFullRecBlockSize);
	DBG("-----------------------------------New0x%x\n", StartOffset);
	Tmp[0] = 'O';
	Tmp[1] = '1';
	Tmp[2] = '8';
	SpiFlashWrite(StartOffset, Tmp, 3);
	StartOffset += 3;

	//total num
	Tmp[0] = TotalRecNum & 0xFF;
	SpiFlashWrite(StartOffset, Tmp, 1);
	StartOffset++;
	for(i = 0; i < TotalRecNum; i++)
	{
		//get data
		if(GetOneInf(i, OneBtRec))
		{
#ifdef SHOW_BT_PAIRING_INFO            
			ShowOneDevMemoryPairedInfo(i,OneBtRec,23);
#endif            
			SpiFlashWrite(StartOffset, OneBtRec, 23);
			StartOffset += 23;
#ifdef SHOW_BT_PAIRING_INFO            
            ShowOneDevSavedPairingInfo(i,StartOffset - 23,23);
#endif            
			memcpy(addr, OneBtRec, 6);
			FlshPairedListSync(i, addr);
		}
	}

	//if(TotalRecNum)
	{
		uint8_t cnt;
		for(cnt = TotalRecNum; cnt<BT_DEVICE_NUM; cnt++)
		{
			if(btPairedList[cnt].index < BT_DEVICE_NUM)
			{
				memset(&btPairedList[cnt], 0xff, sizeof(BT_PAIRED_LIST_INFO));
				btPairedList[cnt].linkedProfile = 0x00;

				BtPairedListFlag=1;
			}
		}
	}
	
	if(BtPairedListFlag) FlshSavePairedList();
}

#ifdef SHOW_BT_PAIRING_INFO
//~~~~~~~~~~~~~~~~~~
static void ShowOneDevMemoryPairedInfo(int RecId,unsigned char* Info, int Size)
{
    int i,t;
    DBG("MemID = %d: ",RecId);
    for(i = 0 ; i < Size ; i ++)
    {
        t = Info[i];
        t &= 0xFF;
        DBG("%x ",t);
    }
    DBG("\n");
}
static void ShowOneDevSavedPairingInfo(int RecId,int AddrOffset, int Size)
{
    int i,t;
    uint8_t C;
    DBG("RecID = %d: ",RecId);
    for(i = 0; i < Size ; i ++)
    {
        SpiFlashRead(AddrOffset + i, &C, 1);
        t = C; 
        t &= 0xFF;
        DBG("%x ",t);
    }
    DBG("\n");
}
//#endif //#ifdef SHOW_BT_PAIRING_INFO
//#if 1//def SHOW_BT_PAIRING_INFO
static void ShowFlashLoadResult(int RecIdxFromZero, unsigned char *BtAddr,unsigned char *LinkKey,unsigned char *Property)
{
    int i,t;
    DBG("FlhID = %d: ",RecIdxFromZero);
    for(i = 0 ; i < 6 ; i ++)
    {
        t = BtAddr[i]; 
        t &= 0xFF;
        DBG("%x ",t);
    }
    
    for(i = 0 ; i < 16 ; i ++)
    {
        t = LinkKey[i]; 
        t &= 0xFF;
        DBG("%x ",t);
    }
    
    t = Property[0]; 
        t &= 0xFF;
        DBG("%x\n",t);
}
#endif //show bt pairing info

#endif //# If 1,支持8个设置，都保存在Flash中


/*
Format:
"BTUDIF"+6B"bdaddr"+N"length"+"Name"
*/

/* Name format:  @#.......@

*/
#define BTUDIF_INFO_ERROR  (0xFFFFFFFF)
uint32_t CheckValidatedBTUDIF(uint8_t* UniCode16/*BE*/, uint32_t Len)
{
	uint32_t i, Pos = 0;

	if(Len < 8)
	{
		return BTUDIF_INFO_ERROR;
	}

	//get start code
	for(i = 0; i < Len - 2; i += 2)
	{
		if(UniCode16[i] != 0x00)
		{
			continue;
		}
		if(UniCode16[i + 1] != '@')
		{
			continue;
		}

		//check next sig: #
		if(i < Len - 4)
		{
			if(UniCode16[i + 2] != 0x00)
			{
				continue;
			}
			if(UniCode16[i + 3] != '#')
			{
				continue;
			}
			//found "@#"
			Pos = i + 4;
			break;
		}
	}

	//check next sig: "#"
	if(Len - i < 4)
	{
		return BTUDIF_INFO_ERROR;
	}
	for(i = Pos; i < Len; i += 2)
	{
		if(UniCode16[i] != 0x00)
		{
			continue;
		}
		if(UniCode16[i + 1] != '@')
		{
			continue;
		}

		return Pos;
	}

	return BTUDIF_INFO_ERROR;
}
#define BTUDIN_OFFSET   (MAX_BT_PAIRING_DATA_SIZE + MAX_BT_ALIVE_ADDR_DATA_SIZE)
#define MAX_UDIN_NAME_LEN  (50)
//UniCode2Utf8
void NewBTDevInfoUpdate(uint8_t* UniCode16/*BE*/, uint32_t Len, uint8_t* NewBdAddr)
{
	uint32_t t, StartOffset, Size;
	uint8_t Addr[6] = "\0";

	//step 0 check the syntax
	StartOffset = CheckValidatedBTUDIF(UniCode16, Len);
	if(BTUDIF_INFO_ERROR == StartOffset)//not found
	{
		return;
	}
	UniCode16 = UniCode16 + StartOffset;
	Len = Len - StartOffset;

	//step 1 get the legal offset
	if(!FlshGetBtPairingInfoOffsetCallBk(&StartOffset, &t))
	{
		return;
	}

	SpiFlashRead(StartOffset + BTUDIN_OFFSET, Addr, 6); //Sync data
	if((Addr[0] != 'B') || (Addr[1] != 'T') ||
	        (Addr[2] != 'U') || (Addr[3] != 'D') ||
	        (Addr[4] != 'I') || (Addr[5] != 'F')) //first time
	{
		//erase all
#ifdef FLASH_ERASE_IN_TCM
		SpiFlashEraseInTcm(StartOffset + BTUDIN_OFFSET, 4096);
#else
		SpiFlashErase(StartOffset + BTUDIN_OFFSET, 4096);
#endif

		//sync data
		Addr[0] = 'B';
		Addr[1] = 'T';
		Addr[2] = 'U';
		Addr[3] = 'D';
		Addr[4] = 'I';
		Addr[5] = 'F';
		SpiFlashWrite(StartOffset + BTUDIN_OFFSET, Addr, 6); //sync data

		//new bdaddr if any
		if(NewBdAddr)
		{
			Addr[0] = NewBdAddr[0];
			Addr[1] = NewBdAddr[1];
			Addr[2] = NewBdAddr[2];
			Addr[3] = NewBdAddr[3];
			Addr[4] = NewBdAddr[4];
			Addr[5] = NewBdAddr[5];
		}
		else
		{
			Addr[0] = '\0';
			Addr[1] = '\0';
			Addr[2] = '\0';
			Addr[3] = '\0';
			Addr[4] = '\0';
			Addr[5] = '\0';
		}
		SpiFlashWrite(StartOffset + BTUDIN_OFFSET + 6, Addr, 6); //default bdaddr,zero
	}
	else
	{
		//Save Old BdAddr
		if(!NewBdAddr)
		{
			SpiFlashRead(StartOffset + BTUDIN_OFFSET + 6, Addr, 6);
		}
		else
		{
			Addr[0] = NewBdAddr[0];
			Addr[1] = NewBdAddr[1];
			Addr[2] = NewBdAddr[2];
			Addr[3] = NewBdAddr[3];
			Addr[4] = NewBdAddr[4];
			Addr[5] = NewBdAddr[5];
		}

		//erase all
#ifdef FLASH_ERASE_IN_TCM
		SpiFlashEraseInTcm(StartOffset + BTUDIN_OFFSET, 4096);
#else
		SpiFlashErase(StartOffset + BTUDIN_OFFSET, 4096);
#endif

		SpiFlashWrite(StartOffset + BTUDIN_OFFSET + 6, Addr, 6); //restore bdaddr

		//sync data
		Addr[0] = 'B';
		Addr[1] = 'T';
		Addr[2] = 'U';
		Addr[3] = 'D';
		Addr[4] = 'I';
		Addr[5] = 'F';
		SpiFlashWrite(StartOffset + BTUDIN_OFFSET, Addr, 6); //sync data
	}

	//get and save info
	Size = 0;
	while(Len >= 2)
	{
		t = *UniCode16++;
		t <<= 8;
		t |= *UniCode16++;

		Len -= 2;

		if(Size > MAX_UDIN_NAME_LEN - 4) //max MAX_UDIN_NAME_LEN bytes
		{
			Addr[0] = 0;
			SpiFlashWrite(StartOffset + BTUDIN_OFFSET + 13 + Size, Addr, 1);
			Size++;
			break;
		}
		//check sig #
		if(t == '@')//end of name
		{
			Addr[0] = 0;
			SpiFlashWrite(StartOffset + BTUDIN_OFFSET + 13 + Size, Addr, 1);
			Size++;
			break;
		}

		if(t <= 0x7F)
		{
			Addr[0] = t & 0xFF;
			SpiFlashWrite(StartOffset + BTUDIN_OFFSET + 13 + Size, Addr, 1);
			Size++;
		}
		else if(t < 0x7FF)
		{
			//Addr just one temp parameter for saving converted utf8 data
			Addr[0] = 0xC0 | ((t >> 6) & 0x1F);
			Addr[1] = 0x80 | (t & 0x3F);
			SpiFlashWrite(StartOffset + BTUDIN_OFFSET + 13 + Size, Addr, 2);
			Size = Size + 2;
		}
		else
		{
			//Addr just one temp parameter for saving converted utf8 data
			Addr[0] = 0xE0 | ((t >> 12) & 0x0F);
			Addr[1] = 0x80 | ((t >> 6) & 0x3F);
			Addr[2] = 0x80 | (t & 0x3F);
			SpiFlashWrite(StartOffset + BTUDIN_OFFSET + 13 + Size, Addr, 3);
			Size = Size + 3;
		}
	}

	//output size
	Addr[0] = Size; //max 256
	SpiFlashWrite(StartOffset + BTUDIN_OFFSET + 12, Addr, 1);
}

bool GetUsrDefinedBtDevName(uint8_t* Name)
{
	uint32_t t, StartOffset;
	uint8_t tmpName[6] = "\0";
	//step 1 get the legal offset
	if(!FlshGetBtPairingInfoOffsetCallBk(&StartOffset, &t))
	{
		return 0;
	}

	SpiFlashRead(StartOffset + BTUDIN_OFFSET, tmpName, 6); //Sync data
	if((tmpName[0] != 'B') || (tmpName[1] != 'T') ||
	        (tmpName[2] != 'U') || (tmpName[3] != 'D') ||
	        (tmpName[4] != 'I') || (tmpName[5] != 'F')) //first time
	{
		return 0;
	}

	//read name length
	SpiFlashRead(StartOffset + BTUDIN_OFFSET + 12, tmpName, 1);
	t = tmpName[0];
	if(t > MAX_UDIN_NAME_LEN)//cut
	{
		t = MAX_UDIN_NAME_LEN - 1;
		Name[t] = 0;
	}
	SpiFlashRead(StartOffset + BTUDIN_OFFSET + 13, tmpName, 1);
	if(tmpName[0] != 0x00)//check the name
	{
		Name[0] = tmpName[0];
		SpiFlashRead(StartOffset + BTUDIN_OFFSET + 14, Name + 1, t - 1); //read all rest data
	}

	return 1;
}

void GetUsrDefinedBtDevAddr(uint8_t* Addr)
{
}

bool FlshBTInfoAreaErase(void)
{
	uint32_t UserDataAddrBegin;

	if(SpiFlashRead(DWNLDR_USRDAT_ADDR, (uint8_t*)&UserDataAddrBegin, 4) < 0)
	{
		return FALSE;//error
	}
#ifdef FLASH_ERASE_IN_TCM
	SpiFlashEraseInTcm(UserDataAddrBegin, MAX_BT_PAIRING_DATA_SIZE + MAX_BT_ALIVE_ADDR_DATA_SIZE);
#else
	SpiFlashErase(UserDataAddrBegin, MAX_BT_PAIRING_DATA_SIZE + MAX_BT_ALIVE_ADDR_DATA_SIZE);
#endif
	FlshBTInfoAreaInit();
	
	FlshErasePairedListArea();
	FlshPairedListRegInit();

	BtStackRemoveAllDevice();
	return TRUE;
}


//Bukk
#ifdef SHOW_BT_PAIRING_INFO
void ShowPairedListWriteData(uint8_t index, uint8_t *addr, uint8_t *name, uint8_t profile)
{
	int i;
	uint8_t des[60];

	memset(des, 0, 6);
	memcpy(des, addr, 6);
    DBG("MEM :\n");
    DBG("Index = %d\n", index);
    DBG("Addr = [");
    for(i = 0 ; i < 6 ; i ++)
    {
        DBG("%x : ",des[i]);
    }
    DBG("]\n");

	memset(des, 0, 60);
	memcpy(des, name, 60);
	DBG("Name = %s", des);
	
    DBG("Profile = [0x%02x] \n", profile);
    DBG("\n");
}

void ShowPairedListReadData(uint8_t index, const uint32_t offset, uint8_t len)
{
	int i;
	uint8_t des[68];
	uint8_t temp;
	uint32_t StartOffset=offset;
	
    DBG("FLASH :\n");
	memset(&des[0], 0, 68);
	SpiFlashRead(StartOffset, &des[0], 68);

	
    DBG("Index = %d\n", des[0]);
	DBG("\n");
	
    DBG("MAC = ");
    for(i = 1; i < 7 ; i++)
    {
        DBG("%x : ",des[i]);
    }
	DBG("\n");

    DBG("Name = %s", &des[7]);
	DBG("\n");

    temp = ~des[67];
    DBG("Profile = [0x%x] \n", temp);
}
#endif

/***************************************
 *  FlshErasePairedListArea
 *  擦除Paired List保存的区域
 *  擦除一次, 4K大约需要40ms,以此类推
 ***************************************/
static bool FlshErasePairedListArea(void)
{
	int32_t ret=0;

	ret = SpiFlashErase(BT_PAIRED_LIST_ADDR_START, MAX_BT_PAIRED_LIST_SIZE);
	BtPairedListOffset = BT_PAIRED_LIST_ADDR_START;

	if(ret == 0)
	{
		//success
		DBG("SUCCESS \n");
		return 1;
	}
	else
	{
		//fail
		DBG("FAIL \n");
		return 0;
	}

}

static void FlshPairedListRegInit(void)
{
	uint8_t i;
	for(i=0; i<BT_DEVICE_NUM; i++)
	{
		memset(&btPairedList[i], 0xff, sizeof(BT_PAIRED_LIST_INFO));
		//btPairedList[i].index = 0xff;
		btPairedList[i].linkedProfile = 0x00;
	}

}

/***************************************
 *  FlshGetPairedListBlockOffset
 *  获取offset
 ***************************************/
static uint32_t FlshGetPairedListBlockOffset(void)
{
	uint32_t StartOffset = BT_PAIRED_LIST_ADDR_START;
	uint8_t Tmp[4];
	uint32_t BlockOffset;
	
	for(BlockOffset = StartOffset; BlockOffset < BT_PAIRED_LIST_ADDR_END; BlockOffset += BT_PAIRED_LIST_BLOCK_SIZE)
	{
		if(BlockOffset >= BT_PAIRED_LIST_ADDR_END)
		{
			//not found
			return 0;
		}
		
		SpiFlashRead(BlockOffset, Tmp, 4);

		if(Tmp[0] == BLOCK_USED)
		{
			return BlockOffset;
		}
		else if(Tmp[0] == BLOCK_FAIL)
		{
			//search next block
		}
		else
		{
			//new block or err block, search next block
		}
	}
	return 0;
}

/***************************************
 *  FlshUpdateAllPairedList
 *  更新所有的配对列表信息
 *  input : offset
 ***************************************/
static uint32_t FlshUpdateAllPairedList(const uint32_t offset)
{
	uint8_t i;
	
	uint8_t Temp[BT_PAIRED_LIST_REC_SIZE];
	uint32_t StartOffset = offset;

	DBG("FlshUpdateAllPairedList : StartOffset = [0x%08x]", StartOffset);

	if(StartOffset != BT_PAIRED_LIST_ADDR_START)
	{
		//step1:将上一个block置为无效
		StartOffset -= BT_PAIRED_LIST_BLOCK_SIZE;
		Temp[0] = BLOCK_FAIL;
		SpiFlashWrite(StartOffset, &Temp[0], 1);

		StartOffset = offset;
	}
	else 
	{
		//FLASH擦除后,重新保存数据,不需要将上一个block置为无效
	}
	
	//step2:刷新最新的BLOCK
	//1:写入标志位
	Temp[0] = BLOCK_USED;
	Temp[1] = 'O';
	Temp[2] = '1';
	Temp[3] = '8';
	SpiFlashWrite(StartOffset, &Temp[0], 4);
	StartOffset += 4;

	//2.写入每个MAC地址对应的信息
	for(i=0; i < BT_DEVICE_NUM; i++)
	{
		if(btPairedList[i].index == 0xff)
		{
			//null, exit;
			return 0 ;
		}
		
		memset(Temp, 0, BT_PAIRED_LIST_REC_SIZE);

	#ifdef SHOW_BT_PAIRING_INFO
		ShowPairedListWriteData(btPairedList[i].index, btPairedList[i].bdAddr, btPairedList[i].deviceName, btPairedList[i].linkedProfile);
	#endif
	
		//SpiFlashWrite(StartOffset, (&btPairedList[i]), sizeof(BT_PAIRED_LIST_INFO));
		Temp[0] = btPairedList[i].index;
		memcpy(&Temp[1], &btPairedList[i].bdAddr[0], 6);
		memcpy(&Temp[7], &btPairedList[i].deviceName[0], 60);
		Temp[67]=~btPairedList[i].linkedProfile; //linkedProfile取反保存到flash

		//DBG("FlshUpdateAllPairedList : StartOffset = [0x%08x]", StartOffset);
		SpiFlashWrite(StartOffset, &Temp[0], BT_PAIRED_LIST_REC_SIZE);
		
	#ifdef SHOW_BT_PAIRING_INFO
		ShowPairedListReadData(i, StartOffset, BT_PAIRED_LIST_REC_SIZE);
	#endif
		
		StartOffset += BT_PAIRED_LIST_REC_SIZE;
	}

	BtPairedListFlag=0;
	return 0;
}

/***************************************
 *  FlshSavePairedList
 *  保存配对信息到FLASH
 ***************************************/
static uint32_t FlshSavePairedList(void)
{
	BtPairedListOffset += BT_PAIRED_LIST_BLOCK_SIZE;
	if((BtPairedListOffset > (BT_PAIRED_LIST_ADDR_END-BT_PAIRED_LIST_BLOCK_SIZE))||(BtPairedListOffset < BT_PAIRED_LIST_ADDR_START))
	{
		FlshErasePairedListArea();
		BtPairedListOffset = BT_PAIRED_LIST_ADDR_START;
	}

	FlshUpdateAllPairedList(BtPairedListOffset);
	return 0;
}

/***************************************
 *  FlshLoadPairedListData
 *  读取配对信息数据
 *  input : offset
 ***************************************/
void FlshLoadPairedListData(const uint32_t offset)
{
	uint32_t StartOffset = offset;
	uint8_t  i;

	//BLOCK HEAD 4Bytes
	StartOffset += 4;

	for(i=0;i<BT_DEVICE_NUM;i++)
	{
		SpiFlashRead(StartOffset, &btPairedList[i].index, 1);
		SpiFlashRead(StartOffset+1, &btPairedList[i].bdAddr[0], 6);
		SpiFlashRead(StartOffset+7, &btPairedList[i].deviceName[0], 60);
		SpiFlashRead(StartOffset+67, &btPairedList[i].linkedProfile, 1);
		btPairedList[i].linkedProfile = ~btPairedList[i].linkedProfile;
		StartOffset += BT_PAIRED_LIST_REC_SIZE;

		if(btPairedList[i].index == 0xff) break;
	#ifdef SHOW_BT_PAIRING_INFO
		ShowPairedListWriteData(btPairedList[i].index, btPairedList[i].bdAddr, btPairedList[i].deviceName, btPairedList[i].linkedProfile);
	#endif
	
	}
}

/***************************************
 *  FlshLoadAllPairedListInfor
 *  读取配对信息接口函数
 ***************************************/
uint32_t FlshLoadAllPairedListInfor(void)
{
	bool ret=0;
	
	BtPairedListOffset = FlshGetPairedListBlockOffset();

	FlshPairedListRegInit();
	
	if(!BtPairedListOffset)
	{
		uint8_t usedFlag=0;
		//err or empty
		BtPairedListOffset = BT_PAIRED_LIST_ADDR_START;

		SpiFlashRead(BtPairedListOffset, &usedFlag, 1);

		if(usedFlag == BLOCK_UNUSED)
		{
			DBG("not need erase!!\n");
			return 0;
		}
		else
		{
			ret = FlshErasePairedListArea();
			if(ret)
			{
				DBG("Paired List Area Erase ok!\n");
				return 0;
			}
			else
			{
				DBG("Paired List Area Erase Err!\n");
				return 0;
			}
		}
	}

	DBG("FlshLoadAllPairedListInfor : offset = [0x%08x] \n", BtPairedListOffset);
	FlshLoadPairedListData(BtPairedListOffset);
	return 1;
}

/***************************************
 *  FlshPairedListSync
 *  和lib中配对列表信息同步
 *  input : index -- 索引序号
 *          addr -- MAC地址
 ***************************************/
static bool FlshPairedListSync(int index, uint8_t *addr)
{
	int ret;
	uint8_t listIndex;

	APP_DBG("FlshPairedListSync:index = %x;\n", index);
	ret = memcmp(&btPairedList[index].bdAddr[0], addr, 6);
	if(ret == 0)
	{
		DBG("not need to sync\n");
		return 1;
	}
	else
	{
		DBG("need to sync\n");

		if(btPairedList[index].index == 0xff)
		{
			DBG("add new record\n");
			//列表为空,新增

			memset(&btPairedList[index], 0xff, sizeof(BT_PAIRED_LIST_INFO));
			//btPairedList[index].index = 0xff;
			btPairedList[index].linkedProfile = 0x00;
			
			btPairedList[index].index = index;
			memcpy(&btPairedList[index].bdAddr[0], addr, 6);

			FlshSavePairedList();
		}
		else
		{
			listIndex = GetpairedListIndex(addr);
			if(listIndex == (index+1))
			{
				//list往前移
				DBG("index <- index+1 \n");
				memset(&btPairedList[index], 0xff, sizeof(BT_PAIRED_LIST_INFO));
				//btPairedList[index].index = 0xff;
				btPairedList[index].linkedProfile = 0x00;
				
				memcpy(&(btPairedList[index]), &(btPairedList[index+1]), sizeof(BT_PAIRED_LIST_INFO));
				btPairedList[index].index = index;
				BtPairedListFlag = 1;
			}
			else
			{
				//顺序错误
				DBG("cover the list infor!! \n");
				memset(&btPairedList[index], 0xff, sizeof(BT_PAIRED_LIST_INFO));
				//btPairedList[index].index = 0xff;
				btPairedList[index].linkedProfile = 0x00;
				
				btPairedList[index].index = index;
				memcpy(&btPairedList[index].bdAddr[0], addr, 6);
				BtPairedListFlag = 1;
			}
		}		
		return 0;
	}
}

/***************************************
 *  GetpairedListIndex
 *  通过MAC地址在配对列表中搜索索引序号
 *  input  : addr -- MAC地址
 *  output : 序号
 ***************************************/
static uint8_t GetpairedListIndex(uint8_t *addr)
{
	uint8_t index;
	int ret;

	for(index=0; index<BT_DEVICE_NUM; index++)
	{
		ret = memcmp(&btPairedList[index].bdAddr[0], addr, 6);
		if(ret == 0)
		{
			return index;
		}
	}
	return 0xff;
}

/***************************************
 *  FlshUpdatePairedName
 *  更新配对列表中MAC对应的名称
 *  input  : addr -- MAC地址
 *           name -- 设备名称
 ***************************************/
uint32_t FlshUpdatePairedName(uint8_t *addr, uint8_t *name)
{
	uint8_t index=0xff;
	int ret;
	uint32_t offset;
	uint8_t temp[60];

	index = GetpairedListIndex(addr);

	if(index == 0xff)
	{
		//not found
		DBG("the addr is not found, ERR!\n");
		return 0;
	}

	ret = memcmp(&btPairedList[index].deviceName[0], name, 60);
	if(ret == 0)
	{
		//not need update;
		DBG("the same device name, not need to update\n");
		return 1;
	}

	btPairedList[index].index = index;
	memcpy(&btPairedList[index].deviceName[0], name, 60);
	
	//FlshSavePairedList();
	
	DBG("Save Profile : BtPairedListOffset = [0x%x]\n", BtPairedListOffset);
	
#ifdef SHOW_BT_PAIRING_INFO
	ShowPairedListWriteData(btPairedList[index].index, btPairedList[index].bdAddr, btPairedList[index].deviceName, btPairedList[index].linkedProfile);
#endif

	//step1:查找该index的linkedProfile保存位置
	offset = (BtPairedListOffset+4+(index*BT_PAIRED_LIST_REC_SIZE)+7);

	//step2:写入flash
	memset(temp, 0, 60);
	memcpy(&temp[0], &btPairedList[index].deviceName[0], 60);
	SpiFlashWrite(offset, &temp[0], 60);

#ifdef SHOW_BT_PAIRING_INFO
	offset = BtPairedListOffset+4+(index*BT_PAIRED_LIST_REC_SIZE);
	ShowPairedListReadData(index, offset, BT_PAIRED_LIST_REC_SIZE);
#endif
		
	DBG("device name update success\n");
	return 2;
}

/***************************************
 *  FlshUpdatePairedProfile
 *  更新配对列表中MAC对应的已配对协议参数
 *  input  : addr -- MAC地址
 *           profile -- 协议参数
 *  note   : Profile保存在flash中是取反的值,以便对FLASH进行操作,减少对FLASH的写入次数
 ***************************************/
uint32_t FlshUpdatePairedProfile(uint8_t *addr, BtLinkedProfile profile)
{
	uint8_t index=0xff;

	index = GetpairedListIndex(addr);

	if(index == 0xff)
	{
		//not found
		DBG("the addr is not found, ERR!\n");
		return 0;
	}

	DBG("FlshUpdatePairedProfile : index = [%d]\n", index);
	DBG("profile = [0x%x]\n", profile);
	DBG("linkedProfile1 = [0x%x]\n", btPairedList[index].linkedProfile);

	switch(profile)
	{
		case PROFILE_A2DP:
			if((btPairedList[index].linkedProfile & PROFILE_A2DP) == 0)
			{
				btPairedList[index].linkedProfile|=PROFILE_A2DP;
				BtPairedListFlag=1;
			}
			break;

		case PROFILE_HFP:
			if((btPairedList[index].linkedProfile & PROFILE_HFP) == 0)
			{
				btPairedList[index].linkedProfile|=PROFILE_HFP;
				BtPairedListFlag=1;
			}
			break;

		case PROFILE_AVRCP:
			if((btPairedList[index].linkedProfile & PROFILE_AVRCP) == 0)
			{
				btPairedList[index].linkedProfile|=PROFILE_AVRCP;
				BtPairedListFlag=1;
			}
			break;

		case PROFILE_SPP:
			if((btPairedList[index].linkedProfile & PROFILE_SPP) == 0)
			{
				btPairedList[index].linkedProfile|=PROFILE_SPP;
				BtPairedListFlag=1;
			}
			break;
	}
	DBG("linkedProfile2 = [0x%x]\n", btPairedList[index].linkedProfile);

	if(BtPairedListFlag)
	{
		uint8_t temp;
		uint32_t offset = BtPairedListOffset;
		//FlshSavePairedList();

		BtPairedListFlag=0;
		
		DBG("Save Profile : BtPairedListOffset = [0x%x]\n", BtPairedListOffset);
		
	#ifdef SHOW_BT_PAIRING_INFO
		ShowPairedListWriteData(btPairedList[index].index, btPairedList[index].bdAddr, btPairedList[index].deviceName, btPairedList[index].linkedProfile);
	#endif
	
		//step1:查找该index的linkedProfile保存位置
		offset = (BtPairedListOffset+4+(index*BT_PAIRED_LIST_REC_SIZE)+BT_PAIRED_LIST_REC_SIZE-1);

		//step2:写入flash
		temp=~btPairedList[index].linkedProfile; //linkedProfile取反保存到flash
		SpiFlashWrite(offset, &temp, 1);

	#ifdef SHOW_BT_PAIRING_INFO
		offset = BtPairedListOffset+4+(index*BT_PAIRED_LIST_REC_SIZE);
		ShowPairedListReadData(index, offset, BT_PAIRED_LIST_REC_SIZE);
	#endif
		
		DBG("profile update success\n");
		return 2;
	}
	else
	{
		//not need update;
		DBG("the same profile, not need to update\n");
		return 1;
	}
}

/***************************************
 *  GetPairedListMac
 *  对比2个MAC ADDR是否一致
 *  input  : addr -- MAC地址
 *  output : 反馈值 1=一致 0=不一致
 ***************************************/
uint8_t GetPairedListMac(const uint8_t *addr1, const uint8_t *addr2)
{
	int32_t ret;
	ret = memcmp(addr1, addr2, 6);

	if(ret == 0) return 1;
	else return 0;
}

/***************************************
 *  ClearPairedList
 *  对比2个MAC ADDR是否一致
 *  input  : addr -- MAC地址
 *  output : 1=SUCCESS 0=FAIL
 ***************************************/
uint8_t ClearPairedList(uint8_t *addr)
{
	return BTDeleteRecord(addr);
}

/*
uint8_t *GetTestAddr(void)
{
	return btPairedList[0].bdAddr;
}
*/
