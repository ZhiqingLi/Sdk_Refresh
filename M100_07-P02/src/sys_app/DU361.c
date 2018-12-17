#include "os.h"
#include "uart.h"
#include "sd_card.h"
#include "gpio.h"
#include "irqs.h"
#include "uart.h"
#include "timer.h"
#include "watchdog.h"
#include "clk.h"
#include "cache.h"
#include "sdio.h"
#include "app_config.h"
#include "line_in_control.h"
#include "TimeOut.h"
#include "sys_app.h"
#include "i2c.h"
#include "eq.h"
#include "eq_params.h"
#include "DU361.h"
#include <math.h>

#ifdef FUNC_DU361_EN
extern bool I2cChkAck(void* I2cMasterHandle);
extern bool I2cWriteByte(void* I2cMasterHandle, uint8_t Val);
extern uint8_t I2cReadByte(void* I2cMasterHandle);

uint8_t  gDU361_OkFlag;
uint8_t	DU361_Addr = DU361_I2C_ADDR ;
static void * DU361_I2cHandler = NULL;
uint16_t	DU361_EqFilterCount = 0;
uint8_t Temp_EqBuf[EQ_MAX_IIC_DATA_SIZE];

uint8_t g_DU361_init_tbl[]=
{
    //0xA3, 0x72,
    0x00, 0x00,
    0x82, 0x02,
    0x00, 0x01,
    0x00, 0x0A,
    0x00, 0x00,
    0x38, 0x38,
    0x28, 0x28,
    0x2A, 0x2A,
    0xFF, 0xFF,
    0x00, 0x00,
    0x03, 0x02,
    0x0D, 0x72,
    0x00, 0x00,
    0x00, 0x00,
    0xFF, 0xFF,
    0x2F, 0x2F,
    0x00, 0x00,
    0x80, 0x7E,
    0x00, 0x00,
    0x00, 0x04,
    0x00, 0x29,
    0x02, 0xCD,
    0x02, 0xAC,
    0x00, 0x00,
    0x00, 0x5D,
    0x01, 0x38,
    0x04, 0xF6,
    0x00, 0x00,
    0x01, 0xB5,
    0x02, 0x66,
    0x01, 0x56,
    0x00, 0x00,
    0x14, 0x43,
    0x02, 0xCD,
    0xFF, 0x7F,
    0x00, 0x00,
    0x34, 0x02,
    0x01, 0xE3,
    0xFF, 0x1D,
    0x00, 0x02,
    0x3C, 0x43,
    0x03, 0x9A,
    0x02, 0xDD,
    0x00, 0x00,
    0x4E, 0x20,
    0x02, 0xCD,
    0x00, 0x00,
    0x00, 0x00,
    0x4E, 0x20,
    0x02, 0xCD,
    0x00, 0x00,
    0x00, 0x00,
    0x4E, 0x20,
    0x02, 0xCD,
    0x00, 0x00,
    0x00, 0x00,
    0x4E, 0x20,
    0x02, 0xCD,
    0x00, 0x00,
    0x1A, 0x58,
    0x20, 0x05,
    0x20, 0x01,
    0x03, 0xFF,
    0x00, 0xBF,
    0x0A, 0x89,
    0x00, 0x06,
    0x00, 0x00,
    0x24, 0xD1,
    0x29, 0x40,
    0xA2, 0x94,
    0x00, 0x03,
    0x64, 0x20,
    0xBE, 0x01,
    0x64, 0x00,
    0x8C, 0x01,
    0x00, 0x00,
    0x05, 0xAD,
    0x00, 0x00,
};

#ifdef FUNC_USE_NEW_DU361_EQ_MODE_EN
uint8_t g_DU361_flat_tbl[]=
{
    0x00, 0x00,/////////
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
};

uint8_t g_DU361_classic_tbl[]=
{
	0x40, 0x2E,
    0x00, 0x00,
    0x00, 0x01,
    0x01, 0x0A,
    0x00, 0xB7,
    0x05, 0x28,
    0x00, 0x00,
    0x02, 0x7F,
    0x03, 0x42,
    0xFB, 0x89,
    0x00, 0x00,
    0x06, 0xEE,
    0x02, 0xD5,
    0xFE, 0x2E,
    0x00, 0x00,
    0x03, 0xE8,
    0x02, 0xD4,
    0x00, 0x00,
    0x00, 0x02,
    0x0E, 0xB2,
    0x01, 0x25,
    0x04, 0x91,

    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
};

uint8_t g_DU361_pop_tbl[]=
{
	0x40, 0x26,
    0x00, 0x00,
    0x00, 0x01,
    0x00, 0xCC,
    0x02, 0x26,
    0xFD, 0xD8,
    0x00, 0x00,
    0x02, 0xCA,
    0x01, 0xE8,
    0x03, 0xF9,
    0x00, 0x00,
    0x02, 0x1F,
    0x01, 0x4D,
    0x04, 0x1A,
    0x00, 0x00,
    0x09, 0xA3,
    0x03, 0x06,
    0x01, 0x17,
    0x00, 0x02,
    0x12, 0x33,
    0x02, 0x8C,
    0xFE, 0x0F,

    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
};

uint8_t g_DU361_rock_tbl[]=
{
	0x40, 0x26,
    0x00, 0x00,
    0x00, 0x01,
    0x00, 0xC4,
    0x02, 0x84,
    0x04, 0xD1,
    0x00, 0x00,
    0x03, 0xC3,
    0x01, 0x33,
    0xFD, 0xBA,
    0x00, 0x00,
    0x02, 0x1F,
    0x01, 0x4D,
    0x04, 0x1A,
    0x00, 0x00,
    0x09, 0xA3,
    0x03, 0x06,
    0x01, 0x17,
    0x00, 0x02,
    0x0D, 0x8F,
    0x02, 0x14,
    0x04, 0x40,

    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
};

uint8_t g_DU361_booster_tbl[]=
{
	0x40, 0x3C,
    0x00, 0x00,
    0x00, 0x01,
    0x01, 0x0A,
    0x00, 0xB7,
    0x05, 0x28,
    0x00, 0x00,
    0x00, 0x60,
    0x01, 0xFA,
    0xFB, 0x0D,
    0x00, 0x00,
    0x02, 0x1F,
    0x01, 0x4D,
    0x04, 0x1A,
    0x00, 0x00,
    0x09, 0xA3,
    0x03, 0x06,
    0x01, 0x17,
    0x00, 0x02,
    0x29, 0xE3,
    0x02, 0xCD,
    0xFE, 0x45,

    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
};

#endif

uint8_t g_DU361_Vb_On_tbl[]=
{ 
    0x40, 0xC1, 
};
uint8_t g_DU361_Vb_Off_tbl[]=
{ 
    0x40, 0xC0, 
};

uint8_t g_DU361_3d_On_Music_tbl[]=
{ 
    0xC0, 0x0D, 
};

uint8_t g_DU361_3d_On_Movie_tbl[]=
{ 
    0xC0, 0x1F, 
};

uint8_t g_DU361_3d_Off_tbl[]=
{ 
    0xC0, 0x0E, 
};

uint8_t g_DU361_pitchshifter_on_tbl[]=
{ 
    0x01, 0xf1, 
};

uint8_t g_DU361_pitchshifter_off_tbl[]=
{ 
    0x01, 0xf0, 
};

void DU361_CheckVer(void)
{
	uint8_t ChipID;
	uint8_t FirmwareVer;
	uint16_t Tempdata = 0;

	I2cReadNByte(DU361_I2cHandler, DU361_I2C_ADDR, DU361_VERSION, (uint8_t *)&Tempdata, 2);
    FirmwareVer = Tempdata>>8;
	ChipID = Tempdata&0xff;
	APP_DBG("ChipID:%02x\n", ChipID);
	APP_DBG("FirmwareVer:%02x\n", FirmwareVer);	
}
void DU361_init(void) 
{
	gDU361_OkFlag = 1;
	DU361_Addr = DU361_I2C_ADDR;	
	
	if (!DU361_I2cHandler)
	{			
	#ifndef USE_DEVELOP_BD_EN
		DU361_I2cHandler= I2cMasterCreate(PORT_A, 24, PORT_A, 25);		
	#else
		DU361_I2cHandler= I2cMasterCreate(PORT_C, 13, PORT_C, 14);	
  #endif
		if (!DU361_I2cHandler)
		{
			APP_DBG("DU361 I2cMasterCreate Error!\n");
		}
		else
		{
			DBG("DU361 I2cMasterCreate OK!\n");
		}
	}    
	DU361_CheckVer();
	#ifdef FUNC_USE_DU361_MIC_HOWLING_EN
	I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_PWR_CTL,g_DU361_init_tbl,67*2);	
	#else
	I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_PWR_CTL,g_DU361_init_tbl,78*2);
	#endif
	#ifdef FUNC_USE_DU361_MIC_HOWLING_EN
	#ifdef FUNC_USE_DU361_GPIO_EN
	DU361_SetLineDetInput();
	DU361_SetBtPwrCtlOutput();
	DU361_SetBatDetInput();
	DU361_SetBtPwrOff();
	DU361_SetBtPwrOn();
	DU361_ReadLineDetState();
	DU361_SetBatDetInput();
	DU361_ReadBatDetState();	
	#endif
	#ifdef FUNC_USE_DU361_MIC_AUTO_MUTE_EN
	TimeOutSet(&DU361_MicValTimer, 1000);
	TimeOutSet(&DU361_CheckTimer, 1000);	
	#endif
	#endif
}	

void  EqFilterStyleConvert2I2CData(uint32_t FilterCount, EqFilterParams* param)
{
    uint32_t  f;
    uint16_t* p   = (uint16_t*)Temp_EqBuf;
    uint8_t   byte;
    

    for(f = 0; f < FilterCount; f++)
    {
        *p++ = param[f].type;
        *p++ = param[f].f0;
        *p++ = param[f].Q;
        *p++ = (int16_t)(80 * log10(param[f].sqrtA / 1024.0) * 256 + 0.5);
    }

    for(f = 0; f < FilterCount*2*4; f += 2)
    {
        byte     = Temp_EqBuf[f+0];
        Temp_EqBuf[f+0] = Temp_EqBuf[f+1];
        Temp_EqBuf[f+1] = byte;
    }
}

/**
 * @brief  Configure EQ style parameters
 * @param  SamplingRate sample rate8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000 are supported.
 * @param  FilterParams EQ style filters parameters
 * @param  FilterNum    EQ style filters count, the value can be 0~5
 * @param  PreGain      EQ style pre-gain, magnitude value, data format Q3.16,
 *         value range 0x0000~0x7FFFF, convert dB vale is 20*log10(PreGain/2^16) dB,
 *         0x00000 means MUTE !
 *         0x04000 convert dB vale is -12.04 dB
 *         0x08000 convert dB vale is -6.02  dB
 *         0x10000 convert dB vale is 0      dB
 *         0x7FFFF convert dB vale is 18.06  dB
 * @return NONE
 */
void DU361_EqStyleConfigure(uint16_t SamplingRate, EqFilterParams* FilterParams, uint32_t FilterNum, uint32_t PreGain)
{
#ifndef FUNC_USE_ONLY_DU361_INIT_PARA_EN
	uint16_t Tempdata;
	uint16_t DU361_SampleRatePara;
    uint8_t i;
    uint8_t Buffer[2];
	
	DU361_SampleRatePara &=0xfff0;
	switch(SamplingRate)
	{
		case 48000:
			Tempdata = DU361_SampleRatePara | 0x00;
			break;
		case 44100:
			Tempdata = DU361_SampleRatePara | 0x01;
			break;
		case 32000:
			Tempdata = DU361_SampleRatePara | 0x02;
			break;
		case 24000:
			Tempdata = DU361_SampleRatePara | 0x03;
			break;
		case 22050:
			Tempdata = DU361_SampleRatePara | 0x04;
			break;
		case 16000:
			Tempdata = DU361_SampleRatePara | 0x05;
			break;
		case 12000:
			Tempdata = DU361_SampleRatePara | 0x06;
			break;
		case 11025:
			Tempdata = DU361_SampleRatePara | 0x07;
			break;
		case 8000:
			Tempdata = DU361_SampleRatePara | 0x08;
			break;	
	}
	Buffer[0] = Tempdata>>8;
	Buffer[1] = Tempdata;
	I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_SAMPLERATE,Buffer,2);
	
	DU361_EqFilterCount = FilterNum;
	for(i = 0;i < EQ_MAX_IIC_DATA_SIZE;i++)
	{
		Temp_EqBuf[i] = 0;
	}
	EqFilterStyleConvert2I2CData(DU361_EqFilterCount,FilterParams);
	I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_EQ_F1_TYPE,Temp_EqBuf,DU361_EqFilterCount*2*4);	
	
	Tempdata = (uint16_t)(20 * log10(PreGain / 65536.0) * 256.0);
	Buffer[0] = Tempdata>>8;
	Buffer[1] = Tempdata;
	I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_EQ_PREGAIN,Buffer,2);
#endif
}

/**
 * @brief  Set EQ style pre-gain
 * @param  PreGain EQ style pre-gain, magnitude value, data format Q3.16,
 *         value range 0x0000~0x7FFFF, convert dB vale is 20*log10(PreGain/2^16) dB,
 *         0x00000 means MUTE !
 *         0x04000 convert dB vale is -12.04 dB
 *         0x08000 convert dB vale is -6.02  dB
 *         0x10000 convert dB vale is 0      dB
 *         0x7FFFF convert dB vale is 18.06  dB
 * @return NONE
 */
void DU361_EqPreGainSet(uint32_t PreGain)
{
#ifndef FUNC_USE_ONLY_DU361_INIT_PARA_EN
    uint8_t Buffer[2];

	Buffer[0] = PreGain>>8;
	Buffer[1] = PreGain;
	I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_EQ_PREGAIN,Buffer,2);
#endif
}

/**
 * @brief  Enable EQ
 * @param  None
 * @return None
 */
void DU361_EqEnable(void)
{
#ifndef FUNC_USE_ONLY_DU361_INIT_PARA_EN

	uint16_t Tempdata,TempEqFilterCount;
    uint8_t Buffer[2];
	
	TempEqFilterCount = ((1 << DU361_EqFilterCount) - 1) << 1;
	Tempdata = 0x4000|TempEqFilterCount;
	Buffer[0] = Tempdata>>8;
	Buffer[1] = Tempdata;
	I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_EQ_CTL,Buffer,2);
#endif
}

/**
 * @brief  Disable EQ
 * @param  None
 * @return None
 */
void DU361_EqDisable(void)
{
#ifndef FUNC_USE_ONLY_DU361_INIT_PARA_EN

	uint16_t Tempdata;
	uint8_t Buffer[2];
	
	Tempdata = 0;
	Buffer[0] = Tempdata>>8;
	Buffer[1] = Tempdata;
	I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_EQ_CTL,Buffer,2);
#endif
}

void DU361_EqModeSelect(uint8_t Id)
{
#ifdef FUNC_USE_NEW_DU361_EQ_MODE_EN
    switch(Id)
	{
		case EQ_STYLE_FLAT:
			I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_EQ_CTL,g_DU361_flat_tbl,42*2);
			break;
		case EQ_STYLE_CLASSICAL:
			I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_EQ_CTL,g_DU361_classic_tbl,42*2);
			break;
		case EQ_STYLE_POP:
			I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_EQ_CTL,g_DU361_pop_tbl,42*2);
			break;
		case EQ_STYLE_ROCK:
			I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_EQ_CTL,g_DU361_rock_tbl,42*2);
			break;
		case EQ_VOCAL_BOOSTER:
			I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_EQ_CTL,g_DU361_booster_tbl,42*2);
			break;
	}
#endif
}

void DU361_DrcConfig(uint16_t Drc1Para,uint16_t Drc2Para,uint16_t Drc3Para)
{
#ifndef FUNC_USE_ONLY_DU361_INIT_PARA_EN

	uint8_t Buffer[2];
	
	Buffer[0] = Drc1Para>>8;
	Buffer[1] = Drc1Para;
	I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_DRC1,Buffer,2);

	Buffer[0] = Drc2Para>>8;
	Buffer[1] = Drc2Para;
	I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_DRC2,Buffer,2);

	Buffer[0] = Drc3Para>>8;
	Buffer[1] = Drc3Para;
	I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_DRC3,Buffer,2);
#endif
}

void DU361_EffectsBass(uint16_t EffectsBassOnOff)
{
#ifndef FUNC_USE_ONLY_DU361_INIT_PARA_EN
    if(EffectsBassOnOff)
	{
		I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_EFFECTS_BASS1,g_DU361_Vb_On_tbl,2);
	}
	else
	{
		I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_EFFECTS_BASS1,g_DU361_Vb_Off_tbl,2);
	} 
#endif
}

void DU361_Effects3D(uint16_t Effects3DOnOff)
{
#ifndef FUNC_USE_ONLY_DU361_INIT_PARA_EN

	if(Effects3DOnOff)
	{
		I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_EFFECTS_3D,g_DU361_3d_On_Movie_tbl,2);
	}
	else
	{
		I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_EFFECTS_3D,g_DU361_3d_On_Music_tbl,2);
	}  
#endif
}

#ifdef CHF_DEBUG_I2C_COM
void TestI2C_Com(void)
{
	I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_EFFECTS_3D,g_DU361_3d_On_Music_tbl,2);
}
#endif

void DU361_EffectsPitchShifer(uint16_t EffectsPitchShiferOnOff)
{
#ifndef FUNC_USE_ONLY_DU361_INIT_PARA_EN

	if(EffectsPitchShiferOnOff)
	{
		I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_EFFECTS_PITCH_SHIFTER,g_DU361_pitchshifter_on_tbl,2);
	}
	else
	{
		I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_EFFECTS_PITCH_SHIFTER,g_DU361_pitchshifter_off_tbl,2);
	}  
#endif
}

#ifdef FUNC_USE_DU361_MIC_HOWLING_EN
#define DEFAUT_MIN_MIC_VAL   0x3e3e
#define ON_STEP_MIC_VAL      0xc1c
#define DEFAUT_MAX_MIC_VAL   0xffff
#define MAX_MIC_VOL_INDEX    16
uint8_t g_DU361_mic_vol_tbl[]=
{ 
    0x3e, 0x3e, 
	0x90, 0x90,
	0xa0, 0xa0,
	0xba, 0xba,
	0xc0, 0xc0,
	0xca, 0xca,
	0xd0, 0xd0,
	0xd7, 0xd7,
	0xdf, 0xdf,
	0xe3, 0xe3,
	0xe7, 0xe7,
	0xeb, 0xeb,
	0xef, 0xef,
	0xf3, 0xf3,
	0xf7, 0xf7,
	0xfb, 0xfb,
	0xff, 0xff,
};

void DU361_MicVolumeSet(uint8_t MicVolume)
{
    if(MicVolume <= MAX_MIC_VOL_INDEX)
	{
		I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_ADC_DIGITAL_VOL,&g_DU361_mic_vol_tbl[MicVolume*2],2);
	}
}

void DU361_MicMuteSet(uint8_t MuteState)
{
    if(MuteState)
	{
		I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_DAC_DIGITAL_VOL,&g_DU361_mic_vol_tbl[0],2);
	}
	else
	{
		I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_DAC_DIGITAL_VOL,&g_DU361_mic_vol_tbl[MAX_MIC_VOL_INDEX*2],2);
	}
}

uint8_t g_DU361_echo_tbl[]=
{
    0x27, 0xe9, //250ms,-9db
    0x2b, 0xe9, //240ms,-10db
    0x2f, 0xe9, //230ms,-11db
    0x33, 0xe9, //220ms,-12db
    0x37, 0xe9, //210ms,-13db
    0x3b, 0xe9, //200ms,-14db
    0x3f, 0xe9, //200ms,-15db
    0x43, 0xe9, //200ms,-16db
    0x47, 0xe9, //200ms,-17db
    0x4b, 0xe9, //200ms,-18db
    0x4f, 0xe9, //200ms,-19db
    //0x53, 0xe9, //200ms,-20db
    0x57, 0xe9, //200ms,-21db
    //0x5b, 0xe9, //200ms,-22db
    0x5f, 0xe9, //200ms,-23db
    //0x63, 0xe9, //200ms,-24db
    0x67, 0xe9, //200ms,-25db
    //0x6b, 0xe9, //200ms,-26db
    0x6f, 0xe9, //200ms,-27db
    //0x73, 0xe9, //200ms,-28db
    0x77, 0xe9, //200ms,-29db
    //0x7b, 0xe9, //200ms,-30db
    0x7f, 0xe9, //200ms,-31db
};
uint8_t g_DU361_reverb_tbl[]=
{
    0x00, 0x00, 
};
void DU361_EchoSet(uint8_t EchoStep)
{	
	I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_EFFECT_ECHO,&g_DU361_echo_tbl[EchoStep*2],2);
}

#ifdef FUNC_USE_DU361_GPIO_EN
uint8_t g_DU361_LineDetInput[]=
{ 
    0x10, 0x00, //A19,输入，打开上拉
};
void DU361_SetLineDetInput(void)
{
    I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_GPIO_CONFIG,g_DU361_LineDetInput,2);
}

uint8_t DU361_ReadLineDetState(void)
{
	uint16_t Tempdata = 0;
	I2cReadNByte(DU361_I2cHandler, DU361_I2C_ADDR, DU361_GPIO_DATA, (uint8_t *)&Tempdata, 2);
	if(Tempdata & 0x0200)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t g_DU361_BtPwrCtlOutput[]=
{ 
    0x20, 0x05, //A20,输出，取消上下拉
};

void DU361_SetBtPwrCtlOutput(void)
{
    I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_GPIO_CONFIG,g_DU361_BtPwrCtlOutput,2);
}

uint8_t g_DU361_BtPwrCtlHigh[]=
{ 
    0x20, 0x01, //A20,输出高
};

void DU361_SetBtPwrOn(void)
{
    uint16_t Tempdata = 0;
	
    I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_GPIO_WRITE,g_DU361_BtPwrCtlHigh,2);
}

uint8_t g_DU361_BtPwrCtlLow[]=
{ 
    0x20, 0x00, //A20,输出低
};

void DU361_SetBtPwrOff(void)
{
    I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_GPIO_WRITE,g_DU361_BtPwrCtlLow,2);
}

uint8_t g_DU361_BatDetInput[]=
{ 
    0x30, 0x04, //A21,输入，取消上下拉
};

void DU361_SetBatDetInput(void)
{
	uint16_t Tempdata = 0;
    I2cWriteNByte(DU361_I2cHandler,DU361_I2C_ADDR,DU361_GPIO_CONFIG,g_DU361_BatDetInput,2);
	//I2cReadNByte(DU361_I2cHandler, DU361_I2C_ADDR, DU361_GPIO_CONFIG, (uint8_t *)&Tempdata, 2);
}

uint8_t DU361_ReadBatDetState(void)
{
	uint16_t Tempdata = 0;
	I2cReadNByte(DU361_I2cHandler, DU361_I2C_ADDR, DU361_GPIO_DATA, (uint8_t *)&Tempdata, 2);
	if(Tempdata & 0x0800)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
#endif

#ifdef FUNC_USE_DU361_MIC_AUTO_MUTE_EN
uint16_t DU361_ReadAdcLevel(void)
{
	uint16_t Tempdata = 0;
	uint16_t RetVal = 0;
	
	I2cReadNByte(DU361_I2cHandler, DU361_I2C_ADDR, DU361_ADC_LEVEL, (uint8_t *)&Tempdata, 2);
	//APP_DBG("Tempdata:%02x\n", Tempdata);
	RetVal = (Tempdata >> 8);
	RetVal = (Tempdata << 8) | RetVal;
	return RetVal;
}

void DU361_MicAutoMuteProc(void)
{	
    if(!IsTimeOut(&DU361_CheckTimer))
	{
		return;
	}
	TimeOutSet(&DU361_CheckTimer, 100);
	
    if(DU361_ReadAdcLevel() < 200)
	{
		if(IsTimeOut(&DU361_MicValTimer))
		{
		    if(!DU361_MicAutoMuteOnFlag)
	    	{
				DU361_MicMuteSet(1);
				DU361_MicAutoMuteOnFlag = 1;
				DBG("DU361_MicAutoMuteOnFlag = 1!\n");
	    	}
		}
	}
	else
	{
		TimeOutSet(&DU361_MicValTimer, 1000);
		if(DU361_MicAutoMuteOnFlag)
		{
			DU361_MicMuteSet(0);
			DU361_MicAutoMuteOnFlag = 0;
			DBG("DU361_MicAutoMuteOnFlag = 0!\n");
		}
	}
}
#endif
#endif
#endif

