
//#include "app/app_globals.h"

//#include "lcd_seg.h"
#include "app_config.h"
#ifdef FUNC_DISP_EN
#include "led_display.h"
#include "seg_panel.h"
#ifdef FUNC_SEG_LED_EN
#include "seg_led_disp.h"
#endif
#ifdef FUNC_SEG_LCD_EN
#include "seg_lcd_disp.h"
#endif

#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_AIP1629A_LED_EN))

#define CHAR_A_OFFSET	10
#if 0//def FUNC_SEG_LED_EN
#define MASK_STRING		0xFF
#else
#define MASK_STRING		0x7F
#endif

#define MAX_STRING_LEN	4

const DISP_TYPE  DispCharSet[] =
{
	NUM_0, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9,
	CHR_A, CHR_B, CHR_C, CHR_D, CHR_E, CHR_F, CHR_G, CHR_H, CHR_I, CHR_J,
	CHR_K, CHR_L, CHR_M, CHR_N, CHR_O, CHR_P, CHR_Q, CHR_R, CHR_S, CHR_T,
	CHR_U, CHR_V, CHR_W, CHR_X, CHR_Y, CHR_Z,
};

#if defined(FUNC_SEG_LCD_EN)
const DISP_TYPE   BIT_ADDR_TABLE[4][7] =
{
	{A1_ADDR, B1_ADDR, C1_ADDR, D1_ADDR, E1_ADDR, F1_ADDR, G1_ADDR},
	{A2_ADDR, B2_ADDR, C2_ADDR, D2_ADDR, E2_ADDR, F2_ADDR, G2_ADDR},
	{A3_ADDR, B3_ADDR, C3_ADDR, D3_ADDR, E3_ADDR, F3_ADDR, G3_ADDR},
	{A4_ADDR, B4_ADDR, C4_ADDR, D4_ADDR, E4_ADDR, F4_ADDR, G4_ADDR},
};

const DISP_TYPE   BIT_VALUE_TABLE[4][7] =
{
	{A1_VALUE, B1_VALUE, C1_VALUE, D1_VALUE, E1_VALUE, F1_VALUE, G1_VALUE},
	{A2_VALUE, B2_VALUE, C2_VALUE, D2_VALUE, E2_VALUE, F2_VALUE, G2_VALUE},
	{A3_VALUE, B3_VALUE, C3_VALUE, D3_VALUE, E3_VALUE, F3_VALUE, G3_VALUE},
	{A4_VALUE, B4_VALUE, C4_VALUE, D4_VALUE, E4_VALUE, F4_VALUE, G4_VALUE},
};
#endif



bool DispChar(DISP_TYPE Char, uint8_t DisCharPos)
{
	DISP_TYPE DispVal;
#if defined(FUNC_SEG_LCD_EN)
	uint8_t TAddr, TempI, TempJ, TempK;
#endif

#ifdef    FUNC_AIP1629A_LED_EN
	uint8_t const DispOrd[4] = {0, 4, 8, 12};
#else
	uint8_t const DispOrd[5] = {0, 1, 2, 3, 4};
#endif

	if(DisCharPos >= MAX_STRING_LEN)
	{
		return FALSE;
	}

	if((Char <= 'Z') && (Char >= 'A'))
	{
		DispVal = DispCharSet[Char - 'A' + CHAR_A_OFFSET];
		//DBG(("Char:%C, off:%bd, Val:%bx\n", Char, (Char - 'A' + CHAR_A_OFFSET), DispVal));
	}
	else if((Char <= 'z') && (Char >= 'a'))
	{
		DispVal = DispCharSet[Char - 'a' + CHAR_A_OFFSET];
	}
	else if((Char <= '9') && (Char >= '0'))
	{
		DispVal = DispCharSet[Char - '0'];
		//DBG(("Char:%C, off:%bd, Val:%bx\n", Char, (Char - '0'), DispVal));
	}
	else if(Char <= 9)
	{
		DispVal = DispCharSet[Char];
		//DBG(("Char:%C, off:%bd, Val:%bx\n", Char, (Char), DispVal));
	}
	else if(Char == '-')
	{
		DispVal = CHR_;
	}
	else
	{
		DispVal = CHR_NUL;
		//DBG(("%C, Val:%bx\n", c, DispVal));
	}

	if((DISP_DEV_LED1888 == gDisplayMode) && (DisCharPos == POS_COM_THOUSAND))
	{
		//if((DispVal == 1) || (DispVal == '1') || (DispVal == 'I'))
		if((DispVal == NUM_1) || (DispVal == CHR_I))
		{
			DispVal = NUM_7;	// 针对LED1888千位1显示可能为AB,AC不同情况而做修订。
		}
		else
		{
			//DBG(("1888,!1\n"));
			DispVal = CHR_NUL;
		}
	}

#if defined(FUNC_SEG_LCD_EN)
	TAddr = DispOrd[DisCharPos];

	for(TempJ = 0; TempJ < 7; TempJ++)
	{
		TempI = BIT_ADDR_TABLE[TAddr][TempJ];

		if(TempI != 0xFF)
		{
			TempK = BIT_VALUE_TABLE[TAddr][TempJ];

			if(DispVal & 0x01)
			{
				//DBG(("TempJ = %bx\n",	TempJ));
				//DBG(("TempK= %bx\n",	TempK));
				gDispBuff[TempI] |= TempK;
			}
			else
			{
				gDispBuff[TempI] &= (~TempK);
			}
		}

		DispVal >>= 1;
	}

//	DBG(("gDispBuff[0]: %u\n",  gDispBuff[0]));
//	DBG(("gDispBuff[1]: %u\n",  gDispBuff[1]));
//	DBG(("gDispBuff[2]: %u\n",  gDispBuff[2]));
//	DBG(("gDispBuff[3]: %u\n",  gDispBuff[3]));
#else
	DispVal |= gDispBuff[DispOrd[DisCharPos]] & (~MASK_STRING);	//保留ICON状态.
	gDispBuff[DispOrd[DisCharPos]] = DispVal;
	//DBG("gDispBuff[%d]: %x\n", DisCharPos, gDispBuff[DispOrd[DisCharPos]]);
#endif

	return TRUE;
}


void DispString(uint8_t* Str)
{
	uint8_t CharCnt = 0;
	while(*Str)
	{
		if(!DispChar(*Str++, CharCnt++))
		{
			break;
		}
	}
}


void DispIcon(ICON_IDX Icon, ICON_STATU Light)
{
#ifdef FUNC_SEG_LED_EN
	if((gDisplayMode == DISP_DEV_LED57) || (gDisplayMode == DISP_DEV_LED8888) || (gDisplayMode == DISP_DEV_LED1888))
	{
		DispLedIcon(Icon, Light);
	}
#endif

#ifdef FUNC_SEG_LCD_EN
	if(gDisplayMode == DISP_DEV_LCD58)
	{
		DispLcdIcon(Icon, Light);
	}
#endif
}


void Num2Char(uint16_t Num, bool ZeroFill)
{
	uint8_t Cnt;
	uint8_t DigitNum;
	DISP_TYPE Char;

	if(Num > 9999)
	{
		DigitNum = 5;
	}
	if(Num > 999)
	{
		DigitNum = 4;
	}
	else if(Num > 99)
	{
		DigitNum = 3;
	}
	else if(Num > 9)
	{
		DigitNum = 2;
	}
	else
	{
		DigitNum = 1;
	}

	for(Cnt = 0; Cnt < MAX_STRING_LEN; Cnt++)
	{
		Char = Num % 10;
		Num /= 10;
		if((Char == 0) && (Cnt >= DigitNum))
		{
#ifdef AU6210K_BOOMBOX_DEMO
			if((ZeroFill == TRUE) && (Cnt != 3))
#else
			if(ZeroFill == TRUE)
#endif
			{
				Char = '0';
			}
			else
			{
				Char = ' ';
			}
		}
		else if((gDisplayMode > DISP_DEV_LED1888) && (DigitNum > MAX_STRING_LEN) && (Cnt == (MAX_STRING_LEN - 1)))
		{
			// >9999,最高位显示'-'
			Char = '-';
		}
		else
		{
			Char += '0';
		}

//		// 数值千、百、十、个在显示Buffer中顺序
//		uint8_t CODE DigitOrder = {0, 1, 2, 3};
//		DispChar(Char, DigitOrder[Cnt]);
		//当千、百、十、个位值在Buffer中次序为3,2,1,0时有效。
		//DBG(("%bd, %bd(%bd), Num:%u\n", Cnt, Char, (Char-'0'), Num));
		DispChar(Char, (MAX_STRING_LEN - Cnt - 1));
	}
}

#else  //(defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN))
//以下定义空函数
void DispIcon(ICON_IDX Icon, ICON_STATU Light)
{
}

void DispString(uint8_t* Str)
{
}
#endif
#endif
