#include "type.h"
#include "app_config.h"
#include "ht1622.h" 
#include "disp_drive.h" 
#include "display.h" 

#ifdef  FUNC_EX_DRV_DISP_EN      //A92950_LCD
uint8_t   BufferSeat,FlyOffset;
uint8_t   ShowHoldTime,MoveTime,NameOffset,FirstMove;

const uint8_t ADDR_TABLE[] =
{
	DIGI1_ADDR,
	DIGI2_ADDR,
	DIGI3_ADDR,
	DIGI4_ADDR,
	DIGI5_ADDR,
	DIGI6_ADDR,
	DIGI7_ADDR,
	DIGI8_ADDR,
};

const uint16_t ASCII_TABLE[] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,Sign1,Sign2,Sign3,0,Sign4,Sign5,Sign6,Sign7,Sign8,Sign9,Sign10,Sign11,Sign12,Sign13,Sign14,
	D_0,D_1,D_2,D_3,D_4,D_5,D_6,
	D_7,D_8,D_9,0,0,Sign15,Sign16,Sign17,0,//0~9
	Sign18,D_A,D_B,D_C,D_D,D_E,D_F,D_G,
	D_H,D_I,D_J,D_K,D_L,D_M,D_N,D_O,
	D_P,D_Q,D_R,D_S,D_T,D_U,D_V,D_W,D_X,D_Y,D_Z,//A~Z
	Sign19,Sign20,Sign21,Sign22,Sign23,Sign24,
	D_a,D_b,D_c,D_d,D_e,D_f,D_g,D_h,D_i,D_j,
	D_k,D_l,D_m,D_n,D_o,D_p,D_q,D_r,D_s,D_t,
	D_u,D_v,D_w,D_x,D_y,D_z,					//a~z
	Sign25,Sign26,Sign27,Sign28,Sign29,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0		
};

#ifdef FUNC_8_TYPE_LCD_EN
const uint8_t  BIT_ADDR_TABLE[4][7] =
{
	{A1_ADDR, B1_ADDR, C1_ADDR, D1_ADDR, E1_ADDR, F1_ADDR, G1_ADDR},
	{A2_ADDR, B2_ADDR, C2_ADDR, D2_ADDR, E2_ADDR, F2_ADDR, G2_ADDR},	
	{A3_ADDR, B3_ADDR, C3_ADDR, D3_ADDR, E3_ADDR, F3_ADDR, G3_ADDR},
	{A4_ADDR, B4_ADDR, C4_ADDR, D4_ADDR, E4_ADDR, F4_ADDR, G4_ADDR},	
};
#endif
#ifdef FUNC_MI_TYPE_LCD_EN
const uint8_t  BIT_ADDR_TABLE[8][16]=
{
    {0xff,ADDR_1A,ADDR_1I,ADDR_1F,ADDR_1G,ADDR_1M,ADDR_1N,ADDR_1E,0xff,ADDR_1B,ADDR_1K,ADDR_1J,ADDR_1H,ADDR_1C,ADDR_1L,ADDR_1D},
	{0xff,ADDR_2A,ADDR_2I,ADDR_2F,ADDR_2G,ADDR_2M,ADDR_2N,ADDR_2E,0xff,ADDR_2B,ADDR_2K,ADDR_2J,ADDR_2H,ADDR_2C,ADDR_2L,ADDR_2D},
	{0xff,ADDR_3A,ADDR_3I,ADDR_3F,ADDR_3G,ADDR_3M,ADDR_3N,ADDR_3E,0xff,ADDR_3B,ADDR_3K,ADDR_3J,ADDR_3H,ADDR_3C,ADDR_3L,ADDR_3D},
	{0xff,ADDR_4A,ADDR_4I,ADDR_4F,ADDR_4G,ADDR_4M,ADDR_4N,ADDR_4E,0xff,ADDR_4B,ADDR_4K,ADDR_4J,ADDR_4H,ADDR_4C,ADDR_4L,ADDR_4D},	
	{0xff,ADDR_5A,ADDR_5I,ADDR_5F,ADDR_5G,ADDR_5M,ADDR_5N,ADDR_5E,0xff,ADDR_5B,ADDR_5K,ADDR_5J,ADDR_5H,ADDR_5C,ADDR_5L,ADDR_5D},
	{0xff,ADDR_6A,ADDR_6I,ADDR_6F,ADDR_6G,ADDR_6M,ADDR_6N,ADDR_6E,0xff,ADDR_6B,ADDR_6K,ADDR_6J,ADDR_6H,ADDR_6C,ADDR_6L,ADDR_6D},
	{0xff,ADDR_7A,ADDR_7I,ADDR_7F,ADDR_7G,ADDR_7M,ADDR_7N,ADDR_7E,0xff,ADDR_7B,ADDR_7K,ADDR_7J,ADDR_7H,ADDR_7C,ADDR_7L,ADDR_7D},
	{0xff,ADDR_8A,ADDR_8I,ADDR_8F,ADDR_8G,ADDR_8M,ADDR_8N,ADDR_8E,0xff,ADDR_8B,ADDR_8K,ADDR_8J,ADDR_8H,ADDR_8C,ADDR_8L,ADDR_8D},	

};
#endif

#ifdef FUNC_8_TYPE_LCD_EN
const uint8_t  BIT_VALUE_TABLE[4][7] =
{
	{A1_VALUE, B1_VALUE, C1_VALUE, D1_VALUE, E1_VALUE, F1_VALUE, G1_VALUE},
	{A2_VALUE, B2_VALUE, C2_VALUE, D2_VALUE, E2_VALUE, F2_VALUE, G2_VALUE},	
	{A3_VALUE, B3_VALUE, C3_VALUE, D3_VALUE, E3_VALUE, F3_VALUE, G3_VALUE},
	{A4_VALUE, B4_VALUE, C4_VALUE, D4_VALUE, E4_VALUE, F4_VALUE, G4_VALUE},	
};
#endif
#ifdef FUNC_MI_TYPE_LCD_EN
const uint8_t  BIT_VALUE_TABLE[8][16]=
{
	{0x0,VALUE_1A,VALUE_1I,VALUE_1F,VALUE_1G,VALUE_1M,VALUE_1N,VALUE_1E,0x0,VALUE_1B,VALUE_1K,VALUE_1J,VALUE_1H,VALUE_1C,VALUE_1L,VALUE_1D},
	{0x0,VALUE_2A,VALUE_2I,VALUE_2F,VALUE_2G,VALUE_2M,VALUE_2N,VALUE_2E,0x0,VALUE_2B,VALUE_2K,VALUE_2J,VALUE_2H,VALUE_2C,VALUE_2L,VALUE_2D},
	{0x0,VALUE_3A,VALUE_3I,VALUE_3F,VALUE_3G,VALUE_3M,VALUE_3N,VALUE_3E,0x0,VALUE_3B,VALUE_3K,VALUE_3J,VALUE_3H,VALUE_3C,VALUE_1L,VALUE_1D},
	{0x0,VALUE_4A,VALUE_4I,VALUE_4F,VALUE_4G,VALUE_4M,VALUE_4N,VALUE_4E,0x0,VALUE_4B,VALUE_4K,VALUE_4J,VALUE_4H,VALUE_4C,VALUE_2L,VALUE_2D},
	{0x0,VALUE_5A,VALUE_5I,VALUE_5F,VALUE_5G,VALUE_5M,VALUE_5N,VALUE_5E,0x0,VALUE_5B,VALUE_5K,VALUE_5J,VALUE_5H,VALUE_5C,VALUE_5L,VALUE_5D},
	{0x0,VALUE_6A,VALUE_6I,VALUE_6F,VALUE_6G,VALUE_6M,VALUE_6N,VALUE_6E,0x0,VALUE_6B,VALUE_6K,VALUE_6J,VALUE_6H,VALUE_6C,VALUE_6L,VALUE_6D},
	{0x0,VALUE_7A,VALUE_7I,VALUE_7F,VALUE_7G,VALUE_7M,VALUE_7N,VALUE_7E,0x0,VALUE_7B,VALUE_7K,VALUE_7J,VALUE_7H,VALUE_7C,VALUE_7L,VALUE_7D},
	{0x0,VALUE_8A,VALUE_8I,VALUE_8F,VALUE_8G,VALUE_8M,VALUE_8N,VALUE_8E,0x0,VALUE_8B,VALUE_8K,VALUE_8J,VALUE_8H,VALUE_8C,VALUE_8L,VALUE_8D},
};
#endif

//========================================
// display init
//========================================	
void DisplayInit(void)
{ 
    TimeOutSet(&DispFlashTmr, 0);	
#ifdef FUNC_HT1622_DRIVE_EN
    HT1622_init();
    HT1622_ClearDispRam();
	ClearAllLine();
#endif
}

//========================================
// display off(for power down)
//========================================
void DisplayOff(void)
{
#ifdef FUNC_HT1622_DRIVE_EN
    HT1622_LcdOff();
#endif
}

//========================================
// clear all display
//========================================
void DispClearAll(void)
{
#ifdef FUNC_HT1622_DRIVE_EN
    HT1622_ClearDispRam();
#endif

}   

//========================================
// display all 
//========================================
void DispSetAll(void)
{
#ifdef FUNC_HT1622_DRIVE_EN
    HT1622_SetDispRam();
#endif
}

//========================================
// Clear the panel(initial used)
//========================================
void ClearAllLine (void)
{	
	unsigned char DispI;
	
    for(DispI = 0;DispI < DISP_ALINE_CHAR;DispI++)
    {
       DispBufferLine[DispI] = 0;    
    }
}

//========================================
// clear line data
//========================================
void ClearLine(uint8_t Line)
{
	uint8_t DispI;
	
	for(DispI = 0;DispI < DISP_ALINE_CHAR;DispI++)
    {			
    	DispBufferLine[DispI] = 0;		
    }	
}

//=================================================
//display one ascii char in some seat
//=================================================
void DispChar(uint8_t *pstr,uint8_t Line,uint8_t Seat)
{
    #ifdef FUNC_8_TYPE_LCD_EN
	uint8_t Wtemp;
	#endif
	#ifdef FUNC_MI_TYPE_LCD_EN
	uint16_t Wtemp;
	#endif
	uint8_t Addr,InputChar,DispI;	
	uint8_t tempi,tempk,tempj;
	
	DispI = Seat;
	while(*pstr)
	{
	    #ifdef FUNC_MI_TYPE_LCD_EN
		if(DispI > 8)	
	    #endif
		#ifdef FUNC_8_TYPE_LCD_EN
		if(DispI > 3)
		#endif
		{
			return ;
		}
		InputChar = *pstr;
		Addr = ADDR_TABLE[DispI];
		Wtemp = ASCII_TABLE[InputChar];	
		#ifdef FUNC_8_TYPE_LCD_EN
		for(tempj = 0;tempj < 7;tempj++)
		#endif
		#ifdef FUNC_MI_TYPE_LCD_EN
		for(tempj = 0;tempj < 16;tempj++)
		#endif
		{	
		    #ifdef FUNC_MI_TYPE_LCD_EN
			if(Wtemp & 0x8000)
			#endif
			#ifdef FUNC_8_TYPE_LCD_EN
			if(Wtemp & 0x01)
			#endif
			{
				tempi = BIT_ADDR_TABLE[Addr][tempj];			
				tempk = BIT_VALUE_TABLE[Addr][tempj];
				if(tempi != 0xff)
				{
					if(tempi >= DISP_ALINE_CHAR)
					{
						return;
					}
					DispBufferLine[tempi]|=tempk;
				}
			}
			else
			{
				tempi = BIT_ADDR_TABLE[Addr][tempj];			
				tempk = BIT_VALUE_TABLE[Addr][tempj];
				if(tempi != 0xff)
				{
					if(tempi >= DISP_ALINE_CHAR)
						return;
					DispBufferLine[tempi]&=(~tempk);
				}
			}
			#ifdef FUNC_8_TYPE_LCD_EN
			Wtemp >>= 1;
			#endif
			#ifdef FUNC_MI_TYPE_LCD_EN
			Wtemp <<= 1;
			#endif
		}				
		DispI++;
		pstr++;
	}
}

//=================================================
//clear one ascii char in some seat
//=================================================
void DispClearChar(uint8_t *pstr,uint8_t Line,uint8_t Seat)
{
	uint16_t Wtemp;
	uint8_t Addr,InputChar;//,DispI;	
	uint8_t tempj,tempi,tempk;//,Btemp;

	#ifdef FUNC_MI_TYPE_LCD_EN
	if(Seat > 8)
	#endif
	#ifdef FUNC_8_TYPE_LCD_EN
	if(Seat > 3)
	#endif
	{
	 	return ;
	}
	Addr = ADDR_TABLE[Seat];
	Wtemp = ASCII_TABLE[InputChar];	
	#ifdef FUNC_MI_TYPE_LCD_EN
	for(tempj = 0;tempj < 16;tempj++)
	#endif
	#ifdef FUNC_8_TYPE_LCD_EN
	for(tempj = 0;tempj < 7;tempj++)
	#endif
	{														
		tempi = BIT_ADDR_TABLE[Addr][tempj];			
		tempk = BIT_VALUE_TABLE[Addr][tempj];
		if(tempi != 0xff)
		{
			if(tempi >= DISP_ALINE_CHAR)
				return;
			DispBufferLine[tempi]&=(~tempk);										
			Wtemp <<= 1;
		}
	}		
}

//=================================================
//display one ascii number in some seat
//=================================================
void DispNumber(uint8_t Data,uint8_t Line,uint8_t Seat)
{
	uint16_t Wtemp;
	uint8_t Addr;	
	uint8_t tempi,tempk,tempj;

	#ifdef FUNC_MI_TYPE_LCD_EN
	if(Seat > 8)
	#endif
	#ifdef FUNC_8_TYPE_LCD_EN
	if(Seat > 3)
	#endif
	{
		return;
	}
	Wtemp = ASCII_TABLE[Data+0x30];
	Addr = ADDR_TABLE[Seat];
	#ifdef FUNC_MI_TYPE_LCD_EN
	for(tempj = 0;tempj < 16;tempj++)
	#endif
	#ifdef FUNC_8_TYPE_LCD_EN
	for(tempj = 0;tempj < 7;tempj++)
	#endif
	{	
	    #ifdef FUNC_MI_TYPE_LCD_EN
		if(Wtemp & 0x8000)
		#endif
		#ifdef FUNC_8_TYPE_LCD_EN
		if(Wtemp & 0x01)
		#endif
		{
			tempi = BIT_ADDR_TABLE[Addr][tempj];			
			tempk = BIT_VALUE_TABLE[Addr][tempj];
			if(tempi != 0xff)
			{
				if(tempi >= DISP_ALINE_CHAR)
					return;
				DispBufferLine[tempi]|=tempk;
			}
		}
		#ifdef FUNC_8_TYPE_LCD_EN
		Wtemp >>= 1;
		#endif
		#ifdef FUNC_MI_TYPE_LCD_EN
		Wtemp <<= 1;
		#endif
	}		
}

//=================================================
//display one icon
//=================================================
void SetIcon(uint8_t icon_add,uint8_t icon_val)			
{
    #if 1//cong add 070822
    if(icon_add >= DISP_ALINE_CHAR)
	{
		return;
	}
	#endif
	DispBufferLine[icon_add]|=icon_val;
}

//=================================================
//clear one icon
//=================================================
void ClearIcon(uint8_t icon_add,uint8_t icon_val)			
{
    #if 1//cong add 070822
    if(icon_add>=DISP_ALINE_CHAR)
	{
		return;
	}
	#endif
	DispBufferLine[icon_add]&=(~icon_val);
}

//=================================================
//roll string display
//=================================================
void DispRollString(uint8_t *pstr,uint8_t Line,uint8_t Length,uint8_t StartSeat)
{
	uint8_t DispI;
	uint8_t Btemp[2];
	
	for(DispI = 0;DispI < 8;DispI++)
	{
		//if(Line==LINE1)
		{
			if(StartSeat > DispI)
			{
				DispChar(" ",Line,DispI);
			}
			else
			{
				if(*pstr != 0)
				{
					if(Length != NameOffset)
					{										
						//Btemp[1]=0;
						//Btemp[0]=*pstr;
					    	//DispChar(&Btemp,Line,DispI);		
						DispChar(pstr,Line,DispI);		
						pstr++;
					}
					else
					{
						DispChar(" ",Line,DispI);	
					}									
				}
				else
				{
					DispChar(" ",Line,DispI);						
				}
			}
		}
	}	
}

//=================================================
//repeat roll string display
//=================================================
void DispRepRollString(uint8_t *pstr,uint8_t Line,uint8_t Length,uint8_t StartSeat)
{
	uint8_t DispI,DispK;
	uint8_t Btemp[2];
	
	DispK = 0;
	for(DispI = 0;DispI < 8;DispI++)
	{
	//	if(Line==LINE1)
		{
			if(StartSeat > DispI)
			{
				DispChar(" ",Line,DispI);
			}
			else
			{
				if(*(pstr+NameOffset+DispK) != 0)
				{																
					//DispBufferLine[Line][DispI]=*(pstr+NameOffset+DispK);	
					//Btemp[1]=0;
					//Btemp[0]=*(pstr+NameOffset+DispK);
					//DispChar(&Btemp,Line,DispI);	
					pstr = pstr+NameOffset+DispK;
					DispChar(pstr,Line,DispI);	
					DispK++;															
				}
				else
				{								
				//	DispBufferLine[Line][DispI]=*(pstr+(DispI-DispK));	
					//Btemp[1]=0;
					//Btemp[0]=*(pstr+NameOffset+DispK);					
					//DispChar(&Btemp,Line,DispI);	
					pstr = pstr+NameOffset+DispK;
					DispChar(pstr,Line,DispI);	
				}
			}
		}
	}
}

//=================================================
//fly string display
//=================================================
void DispFlyString(uint8_t *pstr,uint8_t Line,uint8_t Length,uint8_t StartSeat)
{
	uint8_t DispI,DispK;
	uint8_t Btemp[2];
	
	DispK = NameOffset;
	for(DispI = 0;DispI < 8;DispI++)
	{		
		if(StartSeat > DispI)
		{
			DispChar(" ",Line,DispI);
		}
		else
		{
			if(DispK != 0)
			{
				//DispBufferLine2[DispI]=*(pstr+(NameOffset-DispK));
				//Btemp[1]=0;
				//Btemp[0]=*(pstr+(NameOffset-DispK));
				//DispChar(&Btemp,Line,DispI);		
				pstr = pstr+(NameOffset-DispK);
				DispChar(pstr,Line,DispI);		
				DispK--;
			}
			else
			{
				if(FlyOffset == DispI)								
				{																
					//DispBufferLine2[FlyOffset]=*(pstr+NameOffset);			
					//Btemp[1]=0;
					//Btemp[0]=*(pstr+NameOffset);
				//	DispChar(&Btemp,Line,DispI);	
					pstr = pstr+NameOffset;
					DispChar(pstr,Line,DispI);		
				}
				else
				{
					DispChar(" ",Line,DispI);	
				}
			}
		}
	}
}

//=================================================
//scroll string display
//=================================================
uint8_t StringScrollDisp(uint8_t *pstr,uint8_t Line,uint8_t Length,uint8_t StartSeat)
{
	if(FirstMove)
	{
		NameOffset = 0;
		FirstMove = 0;
		if(StartSeat)
		{
			BufferSeat = 6;
		}
		else
		{
			BufferSeat = 0;
		}
		MoveTime = 12;			
	}
	//if(Mode==0) //if length > 9 will roll
	{
		if(Length > 8)
		{						
			DispRollString(pstr+NameOffset,Line,Length,BufferSeat);
		}
		else
		{
			DispChar(pstr,Line,0);
		}
		if(MoveTime == 0)
		{
			if(BufferSeat != 0)
			{
				BufferSeat--;
			}
			else
			{
				if(NameOffset < (Length-1))
				{
					NameOffset++;
				}
				else
				{
					return 1;
				}
			}
			MoveTime = 12;
		}
	}
	return 0;
}

//=================================================
//some type string display mode 
//=================================================
uint8_t StringDisp(uint8_t *pstr,uint8_t Line,uint8_t Length,uint8_t Mode)
{
	if(FirstMove)
	{
		NameOffset = 0;
		FirstMove = 0;
		BufferSeat = 0;
		#if 1//def FWM582
		MoveTime = 12;
		ShowHoldTime = HOLD_TIME;
		#else
		MoveTime=12;
		#endif
		FlyOffset=8;
	}
	if(Mode == 0) //if length > 9 will roll
	{
		if(Length > 8)
		{						
			DispRollString(pstr+NameOffset,Line,Length,BufferSeat);
		}
		else
		{
			DispChar(pstr,Line,0);
		}
		#if 1//def FWM582
		if((MoveTime == 0)&&(ShowHoldTime == 0))
		#else
		if(MoveTime==0)
		#endif	
		{
			if(BufferSeat != 0)
			{
				BufferSeat--;
			}
			else
			{
				if(NameOffset < (Length-1))
				{
					NameOffset++;
				}
				else
				{
					return 1;
				}
			}
		#if 1//def FWM582
		MoveTime = 12;
		#else
		MoveTime=12;
		#endif
		}
	}
	if(Mode == 1)
	{
		DispRepRollString(pstr,Line,Length,BufferSeat);
		if(MoveTime == 0)
		{
			if(BufferSeat != 0)
			{
				BufferSeat--;
			}
			else
			{
				if(NameOffset < (Length-1))
				{
					NameOffset++;
				}
				else
				{
					NameOffset=0;
				}
			}
			#if 1//def FWM582
			MoveTime = 8;
			#else
			MoveTime=12;
			#endif
		}
	}
	if(Mode == 2)
	{
		DispFlyString(pstr,Line,Length,BufferSeat);
		if(MoveTime == 0)
		{
			//if(BufferSeat>((9-Length)/2))
			//	BufferSeat--;
			//else
			BufferSeat = ((4-Length)/2);
			{				
				if(FlyOffset > (BufferSeat+NameOffset))
				{
					FlyOffset--;
				}
				else
				{
					if(NameOffset < (Length-1))
					{
						NameOffset++;
					}
					else
					{
						return 1;
					}
					FlyOffset = 6;
				}
			}		
			#if 1//def FWM582
			MoveTime = 2;
			#else
			MoveTime=5;
			#endif
		}
	}	
	return 0;
}

//========================================
//Send data to Display Process
//========================================
void SendDisplayData(void)
{
	unsigned char DispI,DispK;		
#ifdef DEBUG_LCD

    uint8_t *pstr;

    DispBufferLine[0]=0x00;
	DispBufferLine[1]=0x00;
	DispBufferLine[2]=0x00;
	DispBufferLine[3]=0x00;
	DispBufferLine[4]=0x00;
	DispBufferLine[5]=0x00;
	DispBufferLine[6]=0x00;
	DispBufferLine[7]=0x00;
	DispBufferLine[8]=0x00;
	DispBufferLine[9]=0x00;
	DispBufferLine[10]=0x00;
	DispBufferLine[11]=0x00;
	DispBufferLine[12]=0x00;
	DispBufferLine[13]=0x00;
	DispBufferLine[14]=0x00;
	DispBufferLine[15]=0x00;
	DispBufferLine[16]=0x00;

	
	
	ClearAllLine();
	pstr = "YYYY";
	DispChar(pstr,LINE1,0);
#else
	for(DispK = 0;DispK < DISP_TOTAL_LINE;DispK++)
	{
		for(DispI = DISP_ALINE_CHAR;DispI >= 0;DispI--)
		{
			if(DispBufferLine[DispI] != DispBufferLineBak[DispI])	
			{
				break;		
			}
			if(DispI == 0)
			{
				return;		
			}
		}
	}
#endif
#ifdef FUNC_HT1622_DRIVE_EN
	HT1622_start();
	HT1622_send_data(0xa0,3);
	HT1622_send_data(0x0,6);
	for(DispK = 0;DispK < DISP_TOTAL_LINE;DispK++)
	{		
		for(DispI = 0; DispI < DISP_ALINE_CHAR; DispI++) 
		{	
		    #if 0
			HT1622_send_data( 0xff,8);
			#else
			HT1622_send_data( DispBufferLine[DispI],8);
			#endif
			DispBufferLineBak[DispI] = DispBufferLine[DispI];
		}
   	}
	HT1622_end();
#endif
}

#endif


