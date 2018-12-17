/******************************************************************************
 * @file    get_bootup_info.c
 * @author  Sean
 * @version V1.0.0
 * @date    16-Jan-2014
 * @brief   get information from boot
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */
#include <string.h>
#include	"gpio.h"
#include	"errno.h"
#include 	"miscfg.h"
//#include	"OrioleReg.h"

#define IMAGE_SIZE_OFF	0
#define PIDnCHKSUM		4
#define CONSTDATA_OFF	0xC
#define USERDATA_OFF	0x10
#define SDK_VER_OFF		0x14
#define	CODE_CRC_OFF	0x18
#define	CODE_MAGIC_OFF	0x1C
#define	OSC32K_CAP_OFF	0x20
#define	FASTCODE_CRC_OFF	0X24

static char CodeBuf[1024];
extern void stub(void);
extern unsigned short CRC16(const char* Buf,unsigned long BufLen,unsigned short CRC);
/**
 * @brief  Get Sdk version
 * @param  None
 * @return Sdk version
 */
int GetSdkVer(void)
{
	int SdkVer;
	//SpiFlashRead(0x94, (uint8_t*)&SdkVer, 4);
	SdkVer = *(unsigned long*)(&stub+SDK_VER_OFF);
	return SdkVer;
}

/**
 * @brief  Get wakeup source name
 * @param  WakeupSrc wakeup source index 
 * @return wakeup source name
 */
//const char* GetWakeupSrcName(unsigned int WakeupSrc)
//{
//	/*
//	 * ignore the combination reset source.for example,POR+SYSRESET signal when debugging reset 
//	 * with	system reset
//	 */
//	WakeupSrc = __RBIT(WakeupSrc);
//	WakeupSrc = __CLZ(WakeupSrc);
//	WakeupSrc = 1 << WakeupSrc;
//	switch(WakeupSrc)
//	{
//		case	WAKEUP_FLAG_POR_LDOIN:
//				return "POR";
//		case	WAKEUP_FLAG_WDG:
//				return "WDG";
//		case 	WAKEUP_FLAG_RTC:
//			    return "RTC_WAKEUP";
//		case 	WAKEUP_FLAG_POWERKEY:
//			    return "POWER_KEY";
//		case 	WAKEUP_FLAG_SYSRST:
//			    return "SYS_REST";
//		case 	WAKEUP_FLAG_RSTPIN:
//			    return "REST_PIN";
//		case 	WAKEUP_FLAG_GPIOA10:
//			    return "GPIO_A10";
//		case 	WAKEUP_FLAG_GPIOB5:
//			    return "GPIO_B5";
//		case 	WAKEUP_FLAG_GPIOB6:
//			    return "GPIO_B6";
//		case 	WAKEUP_FLAG_GPIOB7:
//			    return "GPIO_B7";
//		case 	WAKEUP_FLAG_GPIOB22:
//			    return "GPIO_B22";
//		case 	WAKEUP_FLAG_GPIOB23:
//			    return "GPIO_B23";
//		case 	WAKEUP_FLAG_GPIOB24:
//			    return "GPIO_B24";
//		case 	WAKEUP_FLAG_GPIOB25:
//			    return "GPIO_B25";
//		case 	WAKEUP_FLAG_GPIOC2:
//			    return "GPIO_C2";
//		case 	WAKEUP_FLAG_GPIOC7:
//			    return "GPIO_C7";
//		case 	WAKEUP_FLAG_GPIOC8:
//			    return "GPIO_C8";
//		case 	WAKEUP_FLAG_GPIOC9:
//			    return "GPIO_C9";
//		case 	WAKEUP_FLAG_POR_RTC:
//			    return "RTC_PWON";
//		case 	WAKEUP_FLAG_POR_POWERKEY:
//			    return "POR_PWK";
//		case 	WAKEUP_FLAG_POR_8SRST:
//				return "POR_8SRST";
//		default:
//				return "UNKNOWN";
//	}
//}

/**
 * @brief  get code size
 * @param  None 
 * @return code size in char version
 */
uint32_t GetCodeSize(void)
{
	uint32_t Size;
	//SpiFlashRead(0x80, (uint8_t*)&Size, 4);
	Size =  *(unsigned long*)(&stub+IMAGE_SIZE_OFF);;
	Size &= 0xFFFFFF;
    return Size;
}

/**
 * @brief  get code encryption information
 * @param  None 
 * @return code engryption information printf
 */
const char* GetCodeEncryptedInfo(void)
{
	char Encrypted;
	//SpiFlashRead(0xFF, (unsigned char*)&Encrypted, 1);
	Encrypted = *(unsigned char*)0xFF;
    return 0x55 == Encrypted ? "Yes" : (0xFF == Encrypted ? "No" : "Err");
}


///**
// * @brief  Get current spi flash clock frequency
// * @param  None
// * @return spi clk frequency
// */
//uint32_t ClkGetFshcClkFreq(void)
//{
//	uint8_t	fshc_clk_sel;
//	uint32_t	FshcClkFreq;
//	
//	fshc_clk_sel = ClkFshcClkSelGet();
//	switch(fshc_clk_sel)
//	{
//		case 0: //pll 60M
//			FshcClkFreq = 60000000;
//			break;
//		
//		case 1: //sys clk
//			FshcClkFreq = ClkGetCurrentSysClkFreq();
//			break;
//		
//		case 2: //pll 80M
//			FshcClkFreq = 80000000;
//			break;
//		
//		case 3: //pll 48M
//			FshcClkFreq = 48000000;
//			break;
//		
//		default:
//			FshcClkFreq = 0xffffffff;
//			break;
//	}

//	return FshcClkFreq;		
//}



/**
 * @Brief	See if FPGA image release time is identical to OrioleReg.h generate time,
 * 			otherwise issue warning
 * @Param	None
 * @Return	0,		be equal to each other
 * 			-EINVAL	maybe mismatch to each other
 * @Note@
 */
/*
int FpgaImgMatchOrioleRegFile(void)
{
#ifndef 	SOC_RELEASE_DATE
#define 	SOC_RELEASE_DATE "20000102030405"
#endif
	int i;
	long long time = 0;

	for(i = 0; i < 14; i ++)
	{
		time = time * 10 + (SOC_RELEASE_DATE[i] & 0xF);
	}
	time &= 0xFFFFFFFF;
	return time == SREG_GPIO_SOC_RELEASE_DATE ? 0 : -EINVAL;
}
*/

/**
 * @brief  keep FPGA image with OrioleReg.h in same version
 * @param  None 
 * @return judgement information
 */
/*
const char* GetFpgaImgMatchRegInfo(void)
{	
	return FpgaImgMatchOrioleRegFile() == 0 ? "OK" : "***ERROR***";
}
*/

/**
 * @brief  Flash code CRC self check
 * @param  None 
 * @return judgement information
 */

int FlashCodeCrc()
{
	int i;
	unsigned long Len,Off,CrcOff,Crc,Crc2;
	
	Off = 0;
	Len = *(unsigned long*)DWNLDR_IMGSIZE_ADDR;
	CrcOff = DWNLDR_CODECRC_ADDR;


//	if(Len >= SpiCodeFlashInfo.Capacity) {
//	   /*
//		* it is impossible to exceed the flash capacity
//		*/
//		return -ELIBBAD;
//	}

	memcpy((char*)&Crc2, (void*)CrcOff, 4);
	memcpy(CodeBuf, (void*)Off, sizeof(CodeBuf));

	Crc = CRC16(CodeBuf,CrcOff - Off, 0);
	if(sizeof(CodeBuf) < Len)
		Crc = CRC16(&CodeBuf[CrcOff - Off + 4],sizeof(CodeBuf) - (CrcOff - Off) - 4, Crc);
	else{
		Crc = CRC16(&CodeBuf[CrcOff - Off + 4],Len - (CrcOff - Off) - 4, Crc);
		Len = 0;
	}

	for(i = 1;i < Len / sizeof(CodeBuf);i ++){
		memcpy((void*)CodeBuf, (void*)(sizeof(CodeBuf) * i), sizeof(CodeBuf));
		//Crc = CRC16(CodeBuf,sizeof(CodeBuf),Crc);
		Crc = CRC16((void*)(sizeof(CodeBuf) * i), sizeof(CodeBuf),Crc);
	}

	if(Len % sizeof(CodeBuf)){
		memcpy(CodeBuf, (void*)(sizeof(CodeBuf) * i), Len % sizeof(CodeBuf));
		Crc = CRC16(CodeBuf,Len % sizeof(CodeBuf),Crc);
	}
	return (Crc == Crc2 ? 0 : -ELIBBAD);
}






