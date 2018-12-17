#include "type.h"
#include "app_config.h"
#include "folder.h"
#include "Fat_file.h"
#include "audio_decoder.h"
#include "player_control.h"
#include "radio_control.h"
#include "line_in_control.h"
#include "sys_app.h"
#include "recorder_control.h"
#include "idle_control.h"
#include "eq.h"
#include "eq_params.h"
#include "sys_vol.h"
#include "rtc_control.h"
#include "ht1622.h" 
#include "disp_drive.h" 
#include "display.h" 
#ifdef FUNC_RTC_EN
#include "rtc_control.h"
#endif
#ifdef  FUNC_EX_DRV_DISP_EN      
uint8_t   DispBufferLine[DISP_ALINE_CHAR];
uint8_t   DispBufferLineBak[DISP_ALINE_CHAR];
uint8_t   gDispMsgId;
uint8_t   flash_time,flash_f;
uint8_t   VersionSpring[12] = "Ver-1515";
uint8_t   StereoIn;

TIMER	  DispMsgTmr;
TIMER	  DispInitFlashTmr;
TIMER	  DispFlashTmr;


#ifdef FUNC_LINEIN_EN
//========================================
//system run AUX message
//========================================
void AUXInMessage(void)
{
	uint8_t *pstr;
	
	ClearAllLine();
	pstr = "Aux";
	#ifdef FUNC_8_TYPE_LCD_EN
	DispChar(pstr,LINE1,0);
	#else
	DispChar(pstr,LINE1,3);
	#endif
}
#endif

//========================================
//StandBy message
//========================================
void StandByMessage(void)
{
	uint8_t *pstr;
	
	//ClearAllLine();
	#if 0
	//&Temp[0]="PowerOff";
	DispChar(&Temp[0],LINE1,0);
	#else
	//pstr="Off ";
	//DispChar(pstr,LINE1,0);
	#endif
}

//========================================
//Poweron message
//========================================
void PowerOnMessage(void)
{
	uint8_t *pstr;
	
	ClearAllLine();
	#if 1
	pstr="Hi";
	#ifdef FUNC_8_TYPE_LCD_EN
	DispChar(pstr,LINE1,1);	
	#else
	DispChar(pstr,LINE1,3);
	#endif
	#else
	if(StringDisp("Welcome to listen",LINE1,17,0))  
	{
		//MessageTime = 0;
		FirstMove = 1;
	}
	#endif
}

//========================================
//PowerOff message
//========================================
void PowerOffMessage(void)
{
	uint8_t *pstr;
	
	ClearAllLine();
	pstr = " BYE";
#ifdef FUNC_8_TYPE_LCD_EN
	DispChar(pstr,LINE1,0);
#else
	DispChar(pstr,LINE1,3);
#endif
}

//========================================
//Version message
//========================================
void VersionMessage(void)
{
	uint8_t *pstr;
	
	ClearAllLine();
	pstr = &VersionSpring[0];
	DispChar(pstr,LINE1,0);
}

//========================================
//System Init message
//========================================
void SystemInitMessage(void)
{	
	uint8_t *pstr;
	
	ClearAllLine();
	//pstr="  Init ";
	pstr = "NEW ";
	DispChar(pstr,LINE1,0);
}

//========================================
//Mute message
//========================================
void MuteMessage(void)
{
	uint8_t *pstr;
	
	ClearAllLine();
	if(gSys.MuteFlag)
	{
		pstr = "Mute";
	}
#ifdef FUNC_8_TYPE_LCD_EN
	DispChar(pstr,LINE1,0);
#else
	DispChar(pstr,LINE1,2);
#endif
}

//========================================
//Protect message
//========================================
void ProtectMessage(void)
{
	uint8_t *pstr;
	
	ClearAllLine();
#ifdef FUNC_8_TYPE_LCD_EN
	pstr = "Prot";
	DispChar(pstr,LINE1,0);
#else
	pstr = "Protect";
	DispChar(pstr,LINE1,0);
#endif
}

#ifdef FUNC_RADIO_EN
//========================================
//system run tuner message
//========================================
void RadioOnMessage(void)
{
	uint8_t *pstr;
	
	ClearAllLine();
#ifdef FUNC_8_TYPE_LCD_EN
	pstr = "RAD";
	DispChar(pstr,LINE1,1);
#else
	pstr = "RADIO";
	DispChar(pstr,LINE1,1);
#endif
}

//========================================
//Radio Band message
//========================================
void RadioBandMessage(void)
{
	uint8_t *pstr;
	
	ClearAllLine();
	if(gSys.RadioMode == RADIO_MODE_AM)
	{
		pstr = "AM";
		#ifdef FUNC_8_TYPE_LCD_EN
		DispChar(pstr,LINE1,1);		
		#else
		DispChar(pstr,LINE1,3);
		#endif
	}
	else if(gSys.RadioMode == RADIO_MODE_FM_ST)
	{
	#ifdef FUNC_8_TYPE_LCD_EN
		pstr = "FMST";
		DispChar(pstr,LINE1,0);	
	#else
		pstr = "FM ST";
		DispChar(pstr,LINE1,1);
	#endif
	}
	else if(gSys.RadioMode == RADIO_MODE_FM_MO)
	{
	#ifdef FUNC_8_TYPE_LCD_EN
		pstr = "FMMO";
		DispChar(pstr,LINE1,0);	
	#else
		pstr = "FM MO";
		DispChar(pstr,LINE1,1);
	#endif
	}	
}
#ifdef RADIO_SEMI_MEM_EN
//========================================
//Radio Program message
//========================================
void RadioProgMessage(void)
{	
	uint8_t *pstr;

	pstr = "P-" ;  
	#ifdef FUNC_RADIO_EN
	#if 1//ndef FWM582
	ClearLine(LINE1);
	DispChar(pstr,LINE1,0);
	if(sRadioControl->r_storeon)
	{
		if(flash_f)
		{
			DispNumber((sRadioControl->save_offset+1)/10,LINE1,2);
			DispNumber((sRadioControl->save_offset+1)%10,LINE1,3);							
		}
	}
	else
	{
		DispNumber((sRadioControl->CurStaIdx + 1)/10,LINE1,2);
		DispNumber((sRadioControl->CurStaIdx + 1)%10,LINE1,3);
	}
	//SetIcon(COL_ADDR,COL_VALUE);
	#endif
	#endif
}
#endif

//========================================
//Radio Preset station message
//========================================
void RadioPreChNumMessage(void)
{
	uint8_t *pstr;

	ClearLine(LINE1);
	pstr = "P-" ;  
	DispChar(pstr,LINE1,0);
	if(sRadioControl->State == RADIO_STATUS_PREVIEW)
	{
		DispNumber((sRadioControl->ChlCount)/10,LINE1,2);
		DispNumber((sRadioControl->ChlCount)%10,LINE1,3);
	}
	else
	{
		DispNumber((sRadioControl->CurStaIdx + 1)/10,LINE1,2);
		DispNumber((sRadioControl->CurStaIdx + 1)%10,LINE1,3);
	}
}
#endif

//========================================
//Volume message
//========================================
void VolumeDisp(void)
{
	uint8_t *pstr;
	
	ClearLine(LINE1);
	if(gSys.Volume >= MAX_VOLUME)
	{
		pstr = "Max";	
		#ifdef FUNC_8_TYPE_LCD_EN
		DispChar(pstr,LINE1,0);	
		#else
		DispChar(pstr,LINE1,3);	
		#endif
		return;
	}
	if(gSys.Volume == 0)
	{
		//MuteMessage();
		pstr = "Min";	
		#ifdef FUNC_8_TYPE_LCD_EN
		DispChar(pstr,LINE1,0);	
		#else
		DispChar(pstr,LINE1,3);		
		#endif
		return;
	}
	else
	{
		pstr = "U ";	
		#ifdef FUNC_8_TYPE_LCD_EN
		DispChar(pstr,LINE1,0);
		DispNumber(gSys.Volume/10,LINE1,2);
		DispNumber(gSys.Volume%10,LINE1,3);			
		#else
		DispChar(pstr,LINE1,2);
		DispNumber(gSys.Volume/10,LINE1,4);
		DispNumber(gSys.Volume%10,LINE1,5);	
		#endif
	}		
}

//========================================
//3d message
//========================================
void Tone3dMessage(void)
{
	uint8_t *pstr;
	
	ClearLine(LINE1);	
	if(gSys.Audio3dEffectFlag)
   	{
	   	pstr = "3d";	
		DispChar(pstr,LINE1,0);
   	}
	else
	{
		pstr = "Off";	
		DispChar(pstr,LINE1,0);
	}
}

#ifdef FUNC_RADIO_EN
//========================================
//Radio ST Display Process
//========================================
void RadioSDSignalCheck(void)
{
		#if 0
		if(stereo_mode)
			{				
				#ifdef SI4730		
				
				if(!radio_search&&(radio_status==R_CLEARSTATUS))
				{
					if(CheckStereoDelayTime==0)
						{
							CheckStereoDelayTime=50;// 1S
			                             if(ReadStereo())
								{	
									//if((StereoFilterTime==0)&&(StereoIn==0))
										{		
										//	StereoFilterTime=STEREO_FILTER;							
											StereoIn=1;			
										}
								//	else if((StereoFilterTime!=0)&&(StereoIn==1))
										{																	
									//		StereoFilterTime=STEREO_FILTER;	
										}							
								//	DispBufferIcon[STEREO_ADDR] = (disp_buffer[STEREO_ADDR]|STEREO_VALUE)  ;							
								}				
							else
								{
									//if((StereoFilterTime==0)&&(StereoIn==1))
										{
									//		StereoFilterTime=STEREO_FILTER;
											StereoIn=0;				
										}
									//else if((StereoFilterTime!=0)&&(StereoIn==0))
										{																	
									//		StereoFilterTime=STEREO_FILTER;	
										}	
								}
						}
				}
				else
					StereoIn=0;
				#endif
			}
		else
			StereoIn=0;
		#endif

}

//========================================
//Radio Display Process
//========================================
void RadioDisp(void)
{
#if 1
	unsigned int DispI;
	unsigned int DispC;
	unsigned char *pstr;
	
//	if(SystemExecState!=SYS_RADIO)
//		return;
	//if(NewMode!=RADIO_MODE)
	//	return;
	//if(SystemExecState!=SYS_RADIO)
	//	return ;	
	if(!sRadioControl) 
	{
		return;
	}
	ClearLine(LINE1);
	#if 0//def FWM582
	if(radio_status==R_SCANUP)
		{
			//DispChar("FM",LINE1,0);
			if(flash_f)
				{
					if(MessageTime&&(MessageState==RADIOPROG_MESSAGE))
						{
							goto FREQ_DISP;
						}
					#ifdef P5757
					ClearLine(LINE1);
					pstr="AUTO";
					DispChar(pstr,LINE1,1);
					SetIcon(PROG_ADDR,PROG_VALUE);	
					if(current_band==RADIO_FM)
						SetIcon(FM_ADDR,FM_VALUE);	
					else if(current_band==RADIO_AM)
						SetIcon(MW_ADDR,MW_VALUE);	
					#else
					SetIcon(PROG_ADDR,PROG_VALUE);		
					#endif
				}
			#ifdef P5757
			//SetIcon(PROG_ADDR,PROG_VALUE);		
			return;
			#endif
		}
	#endif
#if 0
FREQ_DISP:		
	if(RadioPresetOn||r_storeon)
		{
			//pstr="Pre    ";
			//DispChar(pstr,LINE1,0);
			#ifdef FWM582
			if(r_storeon)
				{
					if(flash_f)
						{
							#ifdef FWM582
							if(radio_status==R_SCANUP)
								{
									DispNumber((save_offset)/10,LINE1,0);
									DispNumber((save_offset)%10,LINE1,1);	
								}
							else
								{
									DispNumber((save_offset+1)/10,LINE1,0);
									DispNumber((save_offset+1)%10,LINE1,1);	
								}
							if(current_band==RADIO_FM)
								{	
									//DispChar("FM",LINE1,0);
									SetIcon(FM_ADDR,FM_VALUE);
									//DispChar("MHz",LINE2,5);
									DispC=(((unsigned int)current_fq*125)/10-RADIO_IF[radio_area][current_band])/10;
									DispI=DispC%10000;		
									if(DispC>=10000)
										DispNumber(DispC/10000,LINE1,3);	
									DispNumber(DispI/1000,LINE1,4);
									DispC=DispI%1000;
									DispNumber(DispC/100,LINE1,5);
									DispI=DispC%100;
									DispNumber(DispI/10,LINE1,6);
								//	if(DispI%10)
										DispNumber(DispI%10,LINE1,7);	
								//	else
								//		{
									//		pstr=" ";
									//		DispChar(pstr,LINE2,5);
									//	}
									//pstr=".";
									//DispChar(pstr,LINE1,4);	
									#if 0//def FWM582
									if(MessageTime==0)
									#endif
									SetIcon(POINT_ADDR,POINT_VALUE);
									#if 1
									if(stereo_mode)
										{
											#ifdef P5757
											SET_READ_ST;		
											if(!radio_search)
											{								
												if((READ_ST) == 0)
													{								
														StereoIn=1;
													}				
												else
													StereoIn=0;
											}
											else
												StereoIn=0;
											#endif		
										}
									else
										StereoIn=0;
									#endif

								}
							else if(current_band==RADIO_AM)
								{				
									#ifdef EUP_VERSION
									//DispChar("MW",LINE1,0);
									SetIcon(MW_ADDR,MW_VALUE);
									#endif
									#ifdef USA_VERSION
									//DispChar("AM",LINE1,0);
									SetIcon(AM_ADDR,AM_VALUE);
									#endif
									DispC=current_fq-RADIO_IF[radio_area][current_band];				
									DispI=DispC%1000;			
									if(DispC>=1000)
										DispNumber(DispC/1000,LINE1,4);	
									DispNumber(DispI/100,LINE1,5);
									DispC=DispI%100;
									DispNumber(DispC/10,LINE1,6);
									DispNumber(DispC%10,LINE1,7);	
									//pstr="KHz";
									//DispChar(pstr,LINE2,5);
								}							
							if(current_band==RADIO_FM)
								SetIcon(MHZ_ADDR,MHZ_VALUE);
							else if(current_band==RADIO_AM)
								SetIcon(KHZ_ADDR,KHZ_VALUE);
							else if(current_band==RADIO_LW)
								SetIcon(KHZ_ADDR,KHZ_VALUE);
							#else
							DispNumber((save_offset+1)/10,LINE1,0);
							DispNumber((save_offset+1)%10,LINE1,1);	
							#endif
						}
					return;
				}
			else
				{					
					DispNumber((current_offset+1)/10,LINE1,0);
					DispNumber((current_offset+1)%10,LINE1,1);			
				}
			#endif
		}
#endif
	#ifdef FUNC_AM_EN
	if(sRadioControl->State == RADIO_STATUS_PREVIEW)
	{
		RadioPreChNumMessage();
		return;
	}
	if(sRadioControl->CurBand == RADIO_BAND_AM)
	{	
		//DispChar("FM",LINE1,0);
		//SetIcon(MHZ_ADDR,MHZ_VALUE);
		//DispChar("MHz",LINE2,5);
		
		//DispC=(((unsigned int)current_fq*125)/10-RADIO_IF[radio_area][current_band])/10;
		DispC = sRadioControl->AmFreq;
		DispI = DispC/1000;		
		if(DispI)
		{
			DispNumber(DispI,LINE1,3);	
		}
		DispI = DispC%1000;	
		DispNumber(DispI/100,LINE1,4);
		DispC = DispI%100;
		DispNumber(DispC/10,LINE1,5);
		DispNumber(DispC%10,LINE1,6);
		//SetIcon(POINT_ADDR,POINT_VALUE);
		RadioSDSignalCheck();
	}
	else
	#endif
	{	
		//DispChar("FM",LINE1,0);
		//SetIcon(MHZ_ADDR,MHZ_VALUE);
		//DispChar("MHz",LINE2,5);
		if(sRadioControl->State == RADIO_STATUS_PREVIEW)
		{
		    RadioPreChNumMessage();
			return;
		}		
		//DispC=(((unsigned int)current_fq*125)/10-RADIO_IF[radio_area][current_band])/10;
		#ifdef FUNC_AP8048A_SA_CLOCK_PROJ_EN
		DispC = sRadioControl->Freq;
		#else
		DispC = sRadioControl->Freq/10;
		#endif
		DispI = DispC/1000;		
		#ifdef FUNC_8_TYPE_LCD_EN
		if(DispI)
		{
			DispNumber(DispI,LINE1,0);	
		}
		DispI = DispC%1000;	
		DispNumber(DispI/100,LINE1,1);
		DispC = DispI%100;
		DispNumber(DispC/10,LINE1,2);
		DispNumber(DispC%10,LINE1,3);
		#else
		if(DispI)
		{
			DispNumber(DispI,LINE1,3);	
		}
		DispI = DispC%1000;	
		DispNumber(DispI/100,LINE1,4);
		DispC = DispI%100;
		DispNumber(DispC/10,LINE1,5);
		DispNumber(DispC%10,LINE1,6);
		#endif
		//SetIcon(POINT_ADDR,POINT_VALUE);
		RadioSDSignalCheck();
	}
#endif
}
#endif

#ifdef FUNC_USB_EN
//========================================
//UHP Display Process
//========================================
void UHPDisp(void)
{
	uint8_t *pstr;
	uint8_t DispI;	
	
	//if(SystemExecState!=SYS_UHP)
	//	return;
	
	if((!IsPlayerEnvironmentOk()))
	{
	#if 1
	    ClearAllLine();
	#ifdef FUNC_8_TYPE_LCD_EN
		pstr="No";
		DispChar(pstr,LINE1,1);
	#else
		pstr="No Usb";
		DispChar(pstr,LINE1,1);
	#endif
	#else        	
    	if(StringDisp("No Usb",LINE1,6,0))  
		{
			//MessageTime=0;
			FirstMove=1;
		}
	#endif
		return;
	}
    if(gSys.NextModuleID != MODULE_ID_UNKNOWN)
	{
		return;
	}
	if(gPlayContrl == NULL)
	{
		return;
	}
	ClearLine(LINE1);	
	if(GetPlayState() == PLAYER_STATE_STOP)
	{
		if(gPlayContrl->TotalFileNumInDisk == 0)
		{
			#if 1
		    ClearAllLine();
			#ifdef FUNC_8_TYPE_LCD_EN
			pstr="No";
			DispChar(pstr,LINE1,1);
			#else
			pstr="No Song";
			DispChar(pstr,LINE1,1);
			#endif
			#else   	
			if(StringDisp("No Song",LINE1,7,0))  
			{
				//MessageTime=0;
				FirstMove=1;
			}
			#endif
		}
		else
		{
		    ClearAllLine();
		    #ifdef FUNC_PROGRAM_EN
			if(gSys.ProgramEdit)
			{
			    pstr = "P-" ;  
				DispChar(pstr,LINE1,0);
				if(flash_f)
				{
					DispNumber((gSys.ProgramFile)/10,LINE1,2);
					DispNumber((gSys.ProgramFile)%10,LINE1,3);
				}		
				DispNumber((gSys.ProgramOffset)/10,LINE1,6);
				DispNumber((gSys.ProgramOffset)%10,LINE1,7);
			}
			else if(gSys.ProgramMode)
			{
				DispNumber((gSys.ProgramFileLength)/10,LINE1,4);
				DispNumber((gSys.ProgramFileLength)%10,LINE1,5);
			}
			else
			#endif	
			{
	            if(gPlayContrl->TotalFileNumInDisk/1000)
				{	
				#ifdef FUNC_8_TYPE_LCD_EN
					DispNumber(gPlayContrl->TotalFileNumInDisk/1000,LINE1,0);
					DispI = ((gPlayContrl->TotalFileNumInDisk%1000)/100);							
					DispNumber(DispI,LINE1,1);
					DispI = gPlayContrl->TotalFileNumInDisk%100;
					DispNumber(DispI/10,LINE1,2);
					DispNumber(DispI%10,LINE1,3);				
				#else
					DispNumber(gPlayContrl->TotalFileNumInDisk/1000,LINE1,2);
					DispI = ((gPlayContrl->TotalFileNumInDisk%1000)/100);							
					DispNumber(DispI,LINE1,3);
					DispI = gPlayContrl->TotalFileNumInDisk%100;
					DispNumber(DispI/10,LINE1,4);
					DispNumber(DispI%10,LINE1,5);
				#endif
				}
				else
				{						
					DispI = gPlayContrl->TotalFileNumInDisk/100;
					#ifdef FUNC_8_TYPE_LCD_EN
					if(DispI)
					{
						DispNumber(DispI,LINE1,1);
					}
					DispI = gPlayContrl->TotalFileNumInDisk%100;
					DispNumber(DispI/10,LINE1,2);
					DispNumber(DispI%10,LINE1,3);
					#else
					if(DispI)
					{
						DispNumber(DispI,LINE1,3);
					}
					DispI = gPlayContrl->TotalFileNumInDisk%100;
					DispNumber(DispI/10,LINE1,4);
					DispNumber(DispI%10,LINE1,5);
					#endif
				}	
			}
		}
	}
	#if 0
	if(gPlayCtrl.State==PLAY_STATE_STOP)
	{
			if(gPlayCtrl.FileSum==0)
			{
				//pstr="NoSong";
				//DispChar(pstr,LINE1,0);		
				if(StringDisp("No Song",LINE1,7,0))  
				{
					//MessageTime=0;
					FirstMove=1;
				}
			}
			else
			{			
                            if(gPlayCtrl.FileSum/1000)
				{						
					DispNumber(gPlayCtrl.FileSum/1000,LINE1,0);
					DispI=((gPlayCtrl.FileSum%1000)/100);							
					DispNumber(DispI,LINE1,1);
					DispI=gPlayCtrl.FileSum%100;
					DispNumber(DispI/10,LINE1,2);
					DispNumber(DispI%10,LINE1,3);
				}
				else
				{						
					DispI=gPlayCtrl.FileSum/100;
					if(DispI)
						DispNumber(DispI,LINE1,1);
					DispI=gPlayCtrl.FileSum%100;
					DispNumber(DispI/10,LINE1,2);
					DispNumber(DispI%10,LINE1,3);
				}			
			}
	}
	else 
#endif
	else if(GetPlayState() == PLAYER_STATE_PAUSE)
	{
		//ClearLine(1);	
		#ifndef FUNC_8_TYPE_LCD_EN
		if(gPlayContrl->PlayerFile.FileNumInDisk/1000)
		{						
			DispNumber(gPlayContrl->PlayerFile.FileNumInDisk/1000,LINE1,0);
			DispI = ((gPlayContrl->PlayerFile.FileNumInDisk%1000)/100);							
			DispNumber(DispI,LINE1,1);
			DispI = gPlayContrl->PlayerFile.FileNumInDisk%100;
			DispNumber(DispI/10,LINE1,2);
			DispNumber(DispI%10,LINE1,3);
		}
		else
		{						
			DispI = gPlayContrl->PlayerFile.FileNumInDisk/100;
			if(DispI)
			{
				DispNumber(DispI,LINE1,0);
				DispI = gPlayContrl->PlayerFile.FileNumInDisk%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
			else
			{
				DispI = gPlayContrl->PlayerFile.FileNumInDisk%100;
				DispNumber(DispI/10,LINE1,0);
				DispNumber(DispI%10,LINE1,1);
			}			
		}
		#endif
		if(flash_f)
		{					
			DispI = (gPlayContrl->CurPlayTime / 1000)/60;
			#ifdef FUNC_8_TYPE_LCD_EN
			//if(DispI/10)
			{
				DispNumber(DispI/10,LINE1,0);
			}
			DispNumber(DispI%10,LINE1,1);
			DispI = (gPlayContrl->CurPlayTime / 1000)%60;
			DispNumber(DispI/10,LINE1,2);
			DispNumber(DispI%10,LINE1,3);
			#else
			//if(DispI/10)
			{
				DispNumber(DispI/10,LINE1,4);
			}
			DispNumber(DispI%10,LINE1,5);
			DispI = (gPlayContrl->CurPlayTime / 1000)%60;
			DispNumber(DispI/10,LINE1,6);
			DispNumber(DispI%10,LINE1,7);
			#endif
			SetIcon(COL1_ADDR,COL1_VALUE);
			SetIcon(COL2_ADDR,COL2_VALUE);			
		}	
		#if 0
	//	DisplayNameString(0,1,&SelectString,1,SelectBrowseInfo.LongFileNameFlag,SelectBrowseInfo.length);			
		if((ID3_ExistFlag!=0))//&&(PlayInfoTime==0)&&ID3OnSelect)
			{							
				DispNameAndID3();
			}	
		else
			DisplayNameString(0,1,&SelectString,1,SelectBrowseInfo.LongFileNameFlag,SelectBrowseInfo.length);
		#endif
		/*
		DispI=CurrentFileNum/100;
		if(DispI)
			{		
				DispNumber(DispI,LINE1,0);
				DispI=CurrentFileNum%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
		else
			{
				DispI=CurrentFileNum%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
			*/
		//SetIcon(COL_ADDR,COL_VALUE);
	}
	else if(GetPlayState() == PLAYER_STATE_PLAYING)
	{
	#ifndef FUNC_8_TYPE_LCD_EN
	    if(gPlayContrl->PlayerFile.FileNumInDisk/1000)
		{						
			DispNumber(gPlayContrl->PlayerFile.FileNumInDisk/1000,LINE1,0);
			DispI = ((gPlayContrl->PlayerFile.FileNumInDisk%1000)/100);							
			DispNumber(DispI,LINE1,1);
			DispI = gPlayContrl->PlayerFile.FileNumInDisk%100;
			DispNumber(DispI/10,LINE1,2);
			DispNumber(DispI%10,LINE1,3);
		}
		else
		{						
			DispI = gPlayContrl->PlayerFile.FileNumInDisk/100;
			if(DispI)
			{
				DispNumber(DispI,LINE1,0);
				DispI = gPlayContrl->PlayerFile.FileNumInDisk%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
			else
			{
				DispI = gPlayContrl->PlayerFile.FileNumInDisk%100;
				DispNumber(DispI/10,LINE1,0);
				DispNumber(DispI%10,LINE1,1);
			}			
		}
	#endif
		DispI = (gPlayContrl->CurPlayTime / 1000)/60;
	#ifdef FUNC_8_TYPE_LCD_EN
	    //if(DispI/10)
		{
			DispNumber(DispI/10,LINE1,0);
		}
		DispNumber(DispI%10,LINE1,1);
		DispI = (gPlayContrl->CurPlayTime / 1000)%60;
		DispNumber(DispI/10,LINE1,2);
		DispNumber(DispI%10,LINE1,3);	
	#else
		//if(DispI/10)
		{
			DispNumber(DispI/10,LINE1,4);
		}
		DispNumber(DispI%10,LINE1,5);
		DispI = (gPlayContrl->CurPlayTime / 1000)%60;
		DispNumber(DispI/10,LINE1,6);
		DispNumber(DispI%10,LINE1,7);	
	#endif
		SetIcon(COL1_ADDR,COL1_VALUE);
		SetIcon(COL2_ADDR,COL2_VALUE);
		/*
		DispI=CurrentFileNum/100;
		if(DispI)
			{		
				DispNumber(DispI,LINE1,0);
				DispI=CurrentFileNum%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
		else
			{
				DispI=CurrentFileNum%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
			*/
		
#if 0		
		if(PlayInfoTime==0)
			{		
				ClearLine(LINE1);
				if((ID3_ExistFlag!=0))//&&((PlayInfoTime==0))//&&ID3OnSelect))
					{							
						DispNameAndID3();
					}		
				else
					DisplayNameString(0,1,&SelectString,1,SelectBrowseInfo.LongFileNameFlag,64);
			}
		else
			SetIcon(COL_ADDR,COL_VALUE);
#endif
	}
#if 0
	else if(gPlayCtrl.State==PLAY_STATE_LOAD)
	{	
	       ClearLine(LINE1);
		pstr="Load";
		DispChar(pstr,LINE1,0);	
		//VolumeDisp();
		//StringDisp("READING USB",LINE1,11,1);
	}
#endif
	//ClearLine(3);//test for message display it will use display name after
}
#endif

#ifdef FUNC_CARD_EN
//========================================
//CARD Display Process
//========================================
void CardDisp(void)
{
	uint8_t *pstr;
	uint8_t DispI;
	
	//if(SystemExecState!=SYS_UHP)
	//	return;	
	if((!IsPlayerEnvironmentOk()))
	{
	#if 1
	    ClearAllLine();
		pstr="No Sd";
		DispChar(pstr,LINE1,2);
	#else     
    	if(StringDisp("No Card",LINE1,6,0))  
		{
			//MessageTime=0;
			FirstMove=1;
		}
	#endif
		return;
	}
    if(gSys.NextModuleID != MODULE_ID_UNKNOWN)
	{
		return;
	}
	if(gPlayContrl == NULL)
	{
		return;
	}
	ClearLine(LINE1);
	if(GetPlayState() == PLAYER_STATE_STOP)
	{
		if(gPlayContrl->TotalFileNumInDisk == 0)
		{
			#if 1
		    ClearAllLine();
			#ifdef FUNC_8_TYPE_LCD_EN
			pstr="No";
			DispChar(pstr,LINE1,1);
			#else
			pstr="No Song";
			DispChar(pstr,LINE1,1);
			#endif
			#else   	
			if(StringDisp("No Song",LINE1,7,0))  
			{
				//MessageTime=0;
				FirstMove=1;
			}
			#endif
		}
		else
		{	
		    #ifdef FUNC_PROGRAM_EN
			if(gSys.ProgramEdit)
			{
			    pstr = "P-" ;  
				DispChar(pstr,LINE1,0);
				if(flash_f)
				{
					DispNumber((gSys.ProgramFile)/10,LINE1,2);
					DispNumber((gSys.ProgramFile)%10,LINE1,3);
				}	
				DispNumber((gSys.ProgramOffset)/10,LINE1,6);
				DispNumber((gSys.ProgramOffset)%10,LINE1,7);
			}
			else if(gSys.ProgramMode)
			{
				DispNumber((gSys.ProgramFileLength)/10,LINE1,4);
				DispNumber((gSys.ProgramFileLength)%10,LINE1,5);
			}
			else
			#endif	
			{
	            if(gPlayContrl->TotalFileNumInDisk/1000)
				{	
				#ifdef FUNC_8_TYPE_LCD_EN
				    DispNumber(gPlayContrl->TotalFileNumInDisk/1000,LINE1,0);
					DispI = ((gPlayContrl->TotalFileNumInDisk%1000)/100);							
					DispNumber(DispI,LINE1,1);
					DispI = gPlayContrl->TotalFileNumInDisk%100;
					DispNumber(DispI/10,LINE1,2);
					DispNumber(DispI%10,LINE1,3);
				#else
					DispNumber(gPlayContrl->TotalFileNumInDisk/1000,LINE1,2);
					DispI = ((gPlayContrl->TotalFileNumInDisk%1000)/100);							
					DispNumber(DispI,LINE1,3);
					DispI = gPlayContrl->TotalFileNumInDisk%100;
					DispNumber(DispI/10,LINE1,4);
					DispNumber(DispI%10,LINE1,5);
				#endif
				}
				else
				{						
					DispI = gPlayContrl->TotalFileNumInDisk/100;
				#ifdef FUNC_8_TYPE_LCD_EN
				    if(DispI)
					{
						DispNumber(DispI,LINE1,1);
					}
					DispI = gPlayContrl->TotalFileNumInDisk%100;
					DispNumber(DispI/10,LINE1,2);
					DispNumber(DispI%10,LINE1,3);
				#else
					if(DispI)
					{
						DispNumber(DispI,LINE1,3);
					}
					DispI = gPlayContrl->TotalFileNumInDisk%100;
					DispNumber(DispI/10,LINE1,4);
					DispNumber(DispI%10,LINE1,5);
				#endif
				}		
			}
		}
	}
	else if(GetPlayState() == PLAYER_STATE_PAUSE)
	{
		//ClearLine(1);	
		#ifndef FUNC_8_TYPE_LCD_EN
		if(gPlayContrl->PlayerFile.FileNumInDisk/1000)
		{						
			DispNumber(gPlayContrl->PlayerFile.FileNumInDisk/1000,LINE1,0);
			DispI = ((gPlayContrl->PlayerFile.FileNumInDisk%1000)/100);							
			DispNumber(DispI,LINE1,1);
			DispI = gPlayContrl->PlayerFile.FileNumInDisk%100;
			DispNumber(DispI/10,LINE1,2);
			DispNumber(DispI%10,LINE1,3);
		}
		else
		{						
			DispI = gPlayContrl->PlayerFile.FileNumInDisk/100;
			if(DispI)
			{
				DispNumber(DispI,LINE1,0);
				DispI = gPlayContrl->PlayerFile.FileNumInDisk%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
			else
			{
				DispI = gPlayContrl->PlayerFile.FileNumInDisk%100;
				DispNumber(DispI/10,LINE1,0);
				DispNumber(DispI%10,LINE1,1);
			}			
		}
		#endif
		if(flash_f)
		{					
			DispI = (gPlayContrl->CurPlayTime / 1000)/60;
			#ifdef FUNC_8_TYPE_LCD_EN
			//if(DispI/10)
			{
				DispNumber(DispI/10,LINE1,0);
			}
			DispNumber(DispI%10,LINE1,1);
			DispI = (gPlayContrl->CurPlayTime / 1000)%60;
			DispNumber(DispI/10,LINE1,2);
			DispNumber(DispI%10,LINE1,3);
			#else
			//if(DispI/10)
			{
				DispNumber(DispI/10,LINE1,4);
			}
			DispNumber(DispI%10,LINE1,5);
			DispI = (gPlayContrl->CurPlayTime / 1000)%60;
			DispNumber(DispI/10,LINE1,6);
			DispNumber(DispI%10,LINE1,7);
			#endif
			SetIcon(COL1_ADDR,COL1_VALUE);
			SetIcon(COL2_ADDR,COL2_VALUE);
		}	
		#if 0
	//	DisplayNameString(0,1,&SelectString,1,SelectBrowseInfo.LongFileNameFlag,SelectBrowseInfo.length);			
		if((ID3_ExistFlag!=0))//&&(PlayInfoTime==0)&&ID3OnSelect)
			{							
				DispNameAndID3();
			}	
		else
			DisplayNameString(0,1,&SelectString,1,SelectBrowseInfo.LongFileNameFlag,SelectBrowseInfo.length);
		#endif
		/*
		DispI=CurrentFileNum/100;
		if(DispI)
			{		
				DispNumber(DispI,LINE1,0);
				DispI=CurrentFileNum%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
		else
			{
				DispI=CurrentFileNum%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
			*/
		//SetIcon(COL_ADDR,COL_VALUE);
	}
	else if(GetPlayState() == PLAYER_STATE_PLAYING)
	{
	#ifndef FUNC_8_TYPE_LCD_EN
	    if(gPlayContrl->PlayerFile.FileNumInDisk/1000)
		{						
			DispNumber(gPlayContrl->PlayerFile.FileNumInDisk/1000,LINE1,0);
			DispI = ((gPlayContrl->PlayerFile.FileNumInDisk%1000)/100);							
			DispNumber(DispI,LINE1,1);
			DispI = gPlayContrl->PlayerFile.FileNumInDisk%100;
			DispNumber(DispI/10,LINE1,2);
			DispNumber(DispI%10,LINE1,3);
		}
		else
		{						
			DispI = gPlayContrl->PlayerFile.FileNumInDisk/100;
			if(DispI)
			{
				DispNumber(DispI,LINE1,0);
				DispI = gPlayContrl->PlayerFile.FileNumInDisk%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
			else
			{
				DispI = gPlayContrl->PlayerFile.FileNumInDisk%100;
				DispNumber(DispI/10,LINE1,0);
				DispNumber(DispI%10,LINE1,1);
			}			
		}
	#endif
		DispI = (gPlayContrl->CurPlayTime / 1000)/60;
	#ifdef FUNC_8_TYPE_LCD_EN
	    //if(DispI/10)
		{
			DispNumber(DispI/10,LINE1,0);
		}
		DispNumber(DispI%10,LINE1,1);
		DispI = (gPlayContrl->CurPlayTime / 1000)%60;
		DispNumber(DispI/10,LINE1,2);
		DispNumber(DispI%10,LINE1,3);
	#else
		//if(DispI/10)
		{
			DispNumber(DispI/10,LINE1,4);
		}
		DispNumber(DispI%10,LINE1,5);
		DispI = (gPlayContrl->CurPlayTime / 1000)%60;
		DispNumber(DispI/10,LINE1,6);
		DispNumber(DispI%10,LINE1,7);
	#endif
		SetIcon(COL1_ADDR,COL1_VALUE);
		SetIcon(COL2_ADDR,COL2_VALUE);
		/*
		DispI=CurrentFileNum/100;
		if(DispI)
			{		
				DispNumber(DispI,LINE1,0);
				DispI=CurrentFileNum%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
		else
			{
				DispI=CurrentFileNum%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
			*/
		
#if 0		
		if(PlayInfoTime==0)
			{		
				ClearLine(LINE1);
				if((ID3_ExistFlag!=0))//&&((PlayInfoTime==0))//&&ID3OnSelect))
					{							
						DispNameAndID3();
					}		
				else
					DisplayNameString(0,1,&SelectString,1,SelectBrowseInfo.LongFileNameFlag,64);
			}
		else
			SetIcon(COL_ADDR,COL_VALUE);
#endif
	}
#if 0
	else if(gPlayCtrl.State==PLAY_STATE_LOAD)
	{	
		ClearLine(LINE1);
		pstr="Load";
		DispChar(pstr,LINE1,0);	
		//StringDisp("READING USB",LINE1,11,1);
	}
#endif
	//ClearLine(3);//test for message display it will use display name after
}
#endif

#ifdef FUNC_LINEIN_EN
//========================================
//AUX Display Process
//========================================
void AUXDisp(void)
{
	uint8_t *pstr;
	
	ClearAllLine();
	pstr = "Aux";
	#ifdef FUNC_8_TYPE_LCD_EN
	DispChar(pstr,LINE1,0);
	#else
	DispChar(pstr,LINE1,3);
	#endif
}
#endif

#ifdef FUNC_PHIN_EN
//========================================
//Phono Display Process
//========================================
void PhonoDisp(void)
{
	uint8_t *pstr;
	
	//if(SystemExecState!=SYS_AUX)
	//	return;
	ClearLine(LINE1);	
	pstr = "Phono";
	DispChar(pstr,LINE1,1);		
}
#endif

#ifdef FUNC_EXBT_EN
//========================================
//AUX Display Process
//========================================
void ExBtDisp(void)
{
	uint8_t *pstr;
	
	//if(SystemExecState!=SYS_AUX)
	//	return;
	ClearLine(LINE1);	
	pstr = "BT";
	DispChar(pstr,LINE1,3);		
}
#endif

#ifdef FUNC_BT_EN
//========================================
//Bt Display Process
//========================================
void BtDisp(void)
{
	uint8_t *pstr;
	
	//if(SystemExecState!=SYS_AUX)
	//	return;
	ClearLine(LINE1);	
	pstr = "BT";
	DispChar(pstr,LINE1,1);		
}
#endif
#ifdef FUNC_RECORD_EN
//========================================
//Record Display Process
//========================================
void RecordDisp(void)
{
	uint8_t *pstr;
	uint8_t DispI;	
	
	//if(SystemExecState!=SYS_AUX)
	//	return;
	ClearLine(LINE1);	
	if(RecordState == RECORD_STATE_INIT)
	{
		pstr = "Record";
		DispChar(pstr,LINE1,1);		
	}
	else if(RecordState == RECORD_STATE_REC)
	{	    
		if(gRecordContrl->RecordFileSum/1000)
		{						
			DispNumber(gRecordContrl->RecordFileSum/1000,LINE1,0);
			DispI = ((gRecordContrl->RecordFileSum%1000)/100);							
			DispNumber(DispI,LINE1,1);
			DispI = gRecordContrl->RecordFileSum%100;
			DispNumber(DispI/10,LINE1,2);
			DispNumber(DispI%10,LINE1,3);
		}
		else
		{						
			DispI = gRecordContrl->RecordFileSum/100;
			if(DispI)
			{
				DispNumber(DispI,LINE1,0);
				DispI = gRecordContrl->RecordFileSum%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
			else
			{
				DispI = gRecordContrl->RecordFileSum%100;
				DispNumber(DispI/10,LINE1,0);
				DispNumber(DispI%10,LINE1,1);
			}
			
		}		
		DispI = (GetRecordingTime() / 1000)/60;
		//if(DispI/10)
		{
			DispNumber(DispI/10,LINE1,4);
		}
		DispNumber(DispI%10,LINE1,5);
		DispI = (GetRecordingTime() / 1000)%60;
		DispNumber(DispI/10,LINE1,6);
		DispNumber(DispI%10,LINE1,7);	
		SetIcon(COL1_ADDR,COL1_VALUE);
		SetIcon(COL2_ADDR,COL2_VALUE);
	}
	else if(RecordState == RECORD_STATE_REC_PAUSE)
	{
		if(gRecordContrl->RecordFileSum/1000)
		{						
			DispNumber(gRecordContrl->RecordFileSum/1000,LINE1,0);
			DispI = ((gRecordContrl->RecordFileSum%1000)/100);							
			DispNumber(DispI,LINE1,1);
			DispI = gRecordContrl->RecordFileSum%100;
			DispNumber(DispI/10,LINE1,2);
			DispNumber(DispI%10,LINE1,3);
		}
		else
		{						
			DispI = gRecordContrl->RecordFileSum/100;
			if(DispI)
			{
				DispNumber(DispI,LINE1,0);
				DispI = gRecordContrl->RecordFileSum%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
			else
			{
				DispI = gRecordContrl->RecordFileSum%100;
				DispNumber(DispI/10,LINE1,0);
				DispNumber(DispI%10,LINE1,1);
			}			
		}		
		if(flash_f)
		{					
			DispI = (GetRecordingTime() / 1000)/60;
			//if(DispI/10)
			{
				DispNumber(DispI/10,LINE1,4);
			}
			DispNumber(DispI%10,LINE1,5);
			DispI = (GetRecordingTime() / 1000)%60;
			DispNumber(DispI/10,LINE1,6);
			DispNumber(DispI%10,LINE1,7);
			SetIcon(COL1_ADDR,COL1_VALUE);
			SetIcon(COL2_ADDR,COL2_VALUE);
		}	
	}	
}
//========================================
//Record Playback Display Process
//========================================
void RecPlaybackDisp(void)
{
	uint8_t *pstr;
	uint8_t DispI;
	
	if(gPlayContrl == NULL)
	{
	    
		return;
	}
	ClearLine(LINE1);
	if(GetPlayState() == PLAYER_STATE_IDLE)
	{
		if(gPlayContrl->TotalFileNumInDisk == 0)
		{
			#if 1
		    ClearAllLine();
			pstr="No Song";
			DispChar(pstr,LINE1,1);
			#else   	
			if(StringDisp("No Song",LINE1,7,0))  
			{
				//MessageTime=0;
				FirstMove=1;
			}
			#endif
		}
		else
		{			
            if(gPlayContrl->TotalFileNumInDisk/1000)
			{						
				DispNumber(gPlayContrl->TotalFileNumInDisk/1000,LINE1,2);
				DispI = ((gPlayContrl->TotalFileNumInDisk%1000)/100);							
				DispNumber(DispI,LINE1,3);
				DispI = gPlayContrl->TotalFileNumInDisk%100;
				DispNumber(DispI/10,LINE1,4);
				DispNumber(DispI%10,LINE1,5);
			}
			else
			{						
				DispI = gPlayContrl->TotalFileNumInDisk/100;
				if(DispI)
				{
					DispNumber(DispI,LINE1,3);
				}
				DispI = gPlayContrl->TotalFileNumInDisk%100;
				DispNumber(DispI/10,LINE1,4);
				DispNumber(DispI%10,LINE1,5);
			}			
		}
	}
	else if(GetPlayState() == PLAYER_STATE_PAUSE)
	{
		//ClearLine(1);	
		if(gPlayContrl->PlayerFile.FileNumInFolder/1000)
		{						
			DispNumber(gPlayContrl->PlayerFile.FileNumInFolder/1000,LINE1,0);
			DispI = ((gPlayContrl->PlayerFile.FileNumInFolder%1000)/100);							
			DispNumber(DispI,LINE1,1);
			DispI = gPlayContrl->PlayerFile.FileNumInFolder%100;
			DispNumber(DispI/10,LINE1,2);
			DispNumber(DispI%10,LINE1,3);
		}
		else
		{						
			DispI = gPlayContrl->PlayerFile.FileNumInFolder/100;
			if(DispI)
			{
				DispNumber(DispI,LINE1,0);
				DispI = gPlayContrl->PlayerFile.FileNumInFolder%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
			else
			{
				DispI = gPlayContrl->PlayerFile.FileNumInFolder%100;
				DispNumber(DispI/10,LINE1,0);
				DispNumber(DispI%10,LINE1,1);
			}			
		}
		if(flash_f)
		{					
			DispI = (gPlayContrl->CurPlayTime / 1000)/60;
			//if(DispI/10)
			{
				DispNumber(DispI/10,LINE1,4);
			}
			DispNumber(DispI%10,LINE1,5);
			DispI = (gPlayContrl->CurPlayTime / 1000)%60;
			DispNumber(DispI/10,LINE1,6);
			DispNumber(DispI%10,LINE1,7);
			SetIcon(COL1_ADDR,COL1_VALUE);
			SetIcon(COL2_ADDR,COL2_VALUE);
		}	
	}
	else if(GetPlayState() == PLAYER_STATE_PLAYING)
	{
	    if(gPlayContrl->PlayerFile.FileNumInFolder/1000)
		{						
			DispNumber(gPlayContrl->PlayerFile.FileNumInFolder/1000,LINE1,0);
			DispI = ((gPlayContrl->PlayerFile.FileNumInFolder%1000)/100);							
			DispNumber(DispI,LINE1,1);
			DispI = gPlayContrl->PlayerFile.FileNumInFolder%100;
			DispNumber(DispI/10,LINE1,2);
			DispNumber(DispI%10,LINE1,3);
		}
		else
		{						
			DispI = gPlayContrl->PlayerFile.FileNumInFolder/100;
			if(DispI)
			{
				DispNumber(DispI,LINE1,0);
				DispI = gPlayContrl->PlayerFile.FileNumInFolder%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
			else
			{
				DispI = gPlayContrl->PlayerFile.FileNumInFolder%100;
				DispNumber(DispI/10,LINE1,0);
				DispNumber(DispI%10,LINE1,1);
			}			
		}
		DispI = (gPlayContrl->CurPlayTime / 1000)/60;
		//if(DispI/10)
		{
			DispNumber(DispI/10,LINE1,4);
		}
		DispNumber(DispI%10,LINE1,5);
		DispI = (gPlayContrl->CurPlayTime / 1000)%60;
		DispNumber(DispI/10,LINE1,6);
		DispNumber(DispI%10,LINE1,7);	
		SetIcon(COL1_ADDR,COL1_VALUE);
		SetIcon(COL2_ADDR,COL2_VALUE);	
	}
}
#endif

//========================================
//IPOD Display Process
//========================================
#ifdef IPOD_INCLUDE
extern BYTE IpodCurPlayStatus;						//当前的播放状态
void IPODDisp(void)
{
	unsigned char *pstr;
	unsigned char DispI;
	//if(SystemExecState!=SYS_UHP)
	//	return;
	ClearLine(LINE1);
	
	if(!iPod_InsertFlag)
	{
		if(StringDisp("No iPod",LINE1,7,0))  
		{
			//MessageTime=0;
			FirstMove=1;
		}
		return;
	}
	#if 0
	if(IpodCurPlayStatus==IPOD_STATUS_STOPPED)
	{
			if(IpodTotalDBTrackNum==0)
			{
				//pstr="NoSong";
				//DispChar(pstr,LINE1,0);		
				if(StringDisp("No Song",LINE1,7,0))  
				{
					//MessageTime=0;
					FirstMove=1;
				}
			}
			else
			{			
                            if(IpodTotalDBTrackNum/1000)
				{						
					DispNumber(IpodTotalDBTrackNum/1000,LINE1,0);
					DispI=((IpodTotalDBTrackNum%1000)/100);							
					DispNumber(DispI,LINE1,1);
					DispI=IpodTotalDBTrackNum%100;
					DispNumber(DispI/10,LINE1,2);
					DispNumber(DispI%10,LINE1,3);
				}
				else
				{						
					DispI=IpodTotalDBTrackNum/100;
					if(DispI)
						DispNumber(DispI,LINE1,1);
					DispI=IpodTotalDBTrackNum%100;
					DispNumber(DispI/10,LINE1,2);
					DispNumber(DispI%10,LINE1,3);
				}			
			}
	}
	else 
#endif
	if(IpodCurPlayStatus==IPOD_STATUS_PAUSED)
	{
		//ClearLine(1);	
		if(flash_f)
			{					
				DispI=IpodCurPlayTime/60;
				if(DispI/10)
					DispNumber(DispI/10,LINE1,0);
				DispNumber(DispI%10,LINE1,1);
				DispI=IpodCurPlayTime%60;
				DispNumber(DispI/10,LINE1,2);
				DispNumber(DispI%10,LINE1,3);
				SetIcon(COL1_ADDR,COL1_VALUE);
				SetIcon(COL2_ADDR,COL2_VALUE);
			}	
		#if 0
	//	DisplayNameString(0,1,&SelectString,1,SelectBrowseInfo.LongFileNameFlag,SelectBrowseInfo.length);			
		if((ID3_ExistFlag!=0))//&&(PlayInfoTime==0)&&ID3OnSelect)
			{							
				DispNameAndID3();
			}	
		else
			DisplayNameString(0,1,&SelectString,1,SelectBrowseInfo.LongFileNameFlag,SelectBrowseInfo.length);
		#endif
		/*
		DispI=CurrentFileNum/100;
		if(DispI)
			{		
				DispNumber(DispI,LINE1,0);
				DispI=CurrentFileNum%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
		else
			{
				DispI=CurrentFileNum%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
			*/
		//SetIcon(COL_ADDR,COL_VALUE);
	}
	else if(IpodCurPlayStatus==IPOD_STATUS_PLAYING)
	{
		DispI=IpodCurPlayTime/60;
		if(DispI/10)
			DispNumber(DispI/10,LINE1,0);
		DispNumber(DispI%10,LINE1,1);
		DispI=IpodCurPlayTime%60;
		DispNumber(DispI/10,LINE1,2);
		DispNumber(DispI%10,LINE1,3);	
		SetIcon(COL1_ADDR,COL1_VALUE);
		SetIcon(COL2_ADDR,COL2_VALUE);
		/*
		DispI=CurrentFileNum/100;
		if(DispI)
			{		
				DispNumber(DispI,LINE1,0);
				DispI=CurrentFileNum%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
		else
			{
				DispI=CurrentFileNum%100;
				DispNumber(DispI/10,LINE1,1);
				DispNumber(DispI%10,LINE1,2);
			}
			*/
		
#if 0		
		if(PlayInfoTime==0)
			{		
				ClearLine(LINE1);
				if((ID3_ExistFlag!=0))//&&((PlayInfoTime==0))//&&ID3OnSelect))
					{							
						DispNameAndID3();
					}		
				else
					DisplayNameString(0,1,&SelectString,1,SelectBrowseInfo.LongFileNameFlag,64);
			}
		else
			SetIcon(COL_ADDR,COL_VALUE);
#endif
	}
	else if(IpodCurPlayStatus==IPOD_STATUS_LOAD)
	{	
		ClearLine(LINE1);
		pstr="Load";
		DispChar(pstr,LINE1,0);	
		//StringDisp("READING USB",LINE1,11,1);
	}
	//ClearLine(3);//test for message display it will use display name after
}
#endif

//========================================
//Mute Display Process
//========================================
void DispMute(void)
{
	uint8_t *pstr;
	
	ClearLine(LINE1);	
	//ClearIcon(COL_ADDR,COL_VALUE);
	//ClearIcon(POINT_ADDR,POINT_VALUE); 
	//DispChar("    ",LINE1,0);     
	//if(flash_f)
	{
		pstr = "Mute";		
		#ifdef FUNC_8_TYPE_LCD_EN
		DispChar(pstr,LINE1,0);
		#else
		DispChar(pstr,LINE1,3);
		#endif
	}		
}

//========================================
//Clock message
//========================================
void ClockMessage(void)
{
#ifdef CLOCK_INCLUDE
	unsigned char DispI;//,*pstr;	
	if(clock_status!=NORMAL_CLOCK)
	{
		#if 1//def FWM582
		if(clock_mode==CLOCK_12HOUR)
		{
		       #ifdef USE_RTC_FUN
		     //  if(hour12_mode==HOUR12_AM)
	       	//{
		       	//DispChar("AM",LINE1,1);
	       	//}
			//else
			//{
				//DispChar("PM",LINE1,1);
			//}
			#else
			//if(ClockHourSet>=12)
			//	DispChar("PM",LINE1,1);
			//else if(ClockHourSet>=0)
			//	DispChar("AM",LINE1,1);
			#endif
		}
		#endif
		if(clock_status==SETTING_HOUR)
		{
			if(flash_f)
			{	
				if(clock_mode==CLOCK_12HOUR)
				{
				       #ifdef USE_RTC_FUN
					if(ClockHourSet>0)
						DispI=ClockHourSet;
					else
						DispI=12;   
					#else
					if(ClockHourSet>12)
						DispI=ClockHourSet-12;
					else if(ClockHourSet>0)
						DispI=ClockHourSet;
					else
						DispI=12;
					#endif
					if(DispI/10)
						DispNumber(DispI/10,LINE1,0);
					DispNumber(DispI%10,LINE1,1);	
				}
				else if(clock_mode==CLOCK_24HOUR)
				{
					DispI=ClockHourSet;
					if(DispI/10)
						DispNumber(DispI/10,LINE1,0);
					DispNumber(DispI%10,LINE1,1);	
				}																								
			}
			DispI=ClockMinSet;
			DispNumber(DispI/10,LINE1,2);
			DispNumber(DispI%10,LINE1,3);	
			SetIcon(COL1_ADDR,COL1_VALUE);
			SetIcon(COL2_ADDR,COL2_VALUE);
		}
		else if(clock_status==SETTING_MIN)						
		{
			if(clock_mode==CLOCK_12HOUR)
			{
				#ifdef USE_RTC_FUN
				if(ClockHourSet>0)
					DispI=ClockHourSet;
				else
					DispI=12;
				#else
				if(ClockHourSet>12)
					DispI=ClockHourSet-12;
				else if(ClockHourSet>0)
					DispI=ClockHourSet;
				else
					DispI=12;
				#endif
				if(DispI/10)
					DispNumber(DispI/10,LINE1,0);
				DispNumber(DispI%10,LINE1,1);	
			}
			else if(clock_mode==CLOCK_24HOUR)
			{
				DispI=ClockHourSet;
				if(DispI/10)
					DispNumber(DispI/10,LINE1,0);
				DispNumber(DispI%10,LINE1,1);	
			}				
			if(flash_f)
			{	
																								
				DispI=ClockMinSet;
				DispNumber(DispI/10,LINE1,2);
				DispNumber(DispI%10,LINE1,3);	
				SetIcon(COL1_ADDR,COL1_VALUE);
				SetIcon(COL2_ADDR,COL2_VALUE);
			}
			
		}
	}
	#endif
}

//========================================
//Clock Display Process
//========================================
#ifdef CLOCK_INCLUDE
void ClockDisp(void)
{

	uint8_t DispI;
	
//	clock_hour=12;
//	clock_min=0;
	if(ClockInitOk)
	{
		ClearLine(LINE1);		
		if(clock_status!=NORMAL_CLOCK)
		{					
			ClockMessage();
			return ;
		}
		if(ClockModify)
		{
			#if 1//def FWM582
			if(clock_mode==CLOCK_12HOUR)
				{
					#ifndef USE_RTC_FUN
					//if(clock_hour>=12)
					//	DispChar("PM",LINE1,1);
					//else if(clock_hour>=0)
					//	DispChar("AM",LINE1,1);
					#else
					//if(hour12_mode==HOUR12_AM)
					//	DispChar("AM",LINE1,1);
					//else
					//	DispChar("PM",LINE1,1);	
					#endif

					#ifdef USE_RTC_FUN
					if(clock_hour>0)
						DispI=clock_hour;
					else
						DispI=12;
					#else
					if(clock_hour>12)
						DispI=clock_hour-12;
					else if(clock_hour>0)
						DispI=clock_hour;
					else
						DispI=12;
					#endif
					if(DispI/10)
						DispNumber(DispI/10,LINE1,0);
					DispNumber(DispI%10,LINE1,1);
					DispI=clock_min;
					DispNumber(DispI/10,LINE1,2);
					DispNumber(DispI%10,LINE1,3);		
				}
			else
				{
					DispI=12;
					if(DispI/10)
						DispNumber(DispI/10,LINE1,0);
					DispNumber(DispI%10,LINE1,1);
					DispI=0;
					DispNumber(DispI/10,LINE1,2);
					DispNumber(DispI%10,LINE1,3);		
				}
			#endif
			//if(flash_f)
				{	
					
					SetIcon(COL1_ADDR,COL1_VALUE);
					SetIcon(COL2_ADDR,COL2_VALUE);
				}						
		}
		else
		{
			if(clock_mode==CLOCK_12HOUR)
			{
				#ifndef USE_RTC_FUN
				//if(clock_hour>=12)
				//	DispChar("PM",LINE1,1);
				//else if(clock_hour>=0)
				//	DispChar("AM",LINE1,1);
				#else
				//if(hour12_mode==HOUR12_AM)
				//	DispChar("AM",LINE1,1);
				//else
				//	DispChar("PM",LINE1,1);	
				#endif	
				#ifdef USE_RTC_FUN
				if(clock_hour>0)
					DispI=clock_hour;
				else
					DispI=12;
				#else
				if(clock_hour>12)
					DispI=clock_hour-12;
				else if(clock_hour>0)
					DispI=clock_hour;
				else
					DispI=12;
				#endif
				if(DispI/10)
					DispNumber(DispI/10,LINE1,0);
				DispNumber(DispI%10,LINE1,1);
				DispI=clock_min;
				DispNumber(DispI/10,LINE1,2);
				DispNumber(DispI%10,LINE1,3);		
			}
			else if(clock_mode==CLOCK_24HOUR)
			{
				DispI=clock_hour;
				if(DispI/10)
					DispNumber(DispI/10,LINE1,0);
				DispNumber(DispI%10,LINE1,1);
			}
			DispI=clock_min;
			DispNumber(DispI/10,LINE1,2);
			DispNumber(DispI%10,LINE1,3);	
			if(flash_f)
			{
				SetIcon(COL1_ADDR,COL1_VALUE);
				SetIcon(COL2_ADDR,COL2_VALUE);
			}
		}
	}
}
#endif

#ifdef FUNC_RTC_EN
void ClockDisp(void)
{
    uint8_t DispI;
	uint8_t *pstr;
	
	ClearLine(LINE1);	
	//ClearIcon(COL_ADDR,COL_VALUE);
	//ClearIcon(POINT_ADDR,POINT_VALUE); 
	//DispChar("    ",LINE1,0);     
	//if(flash_f)
	{
		pstr = "Mute";		
		#ifdef FUNC_8_TYPE_LCD_EN
		DispChar(pstr,LINE1,0);
		#else
		DispChar(pstr,LINE1,3);
		#endif
	}			
    ClearLine(LINE1);		
	
	if(RTC_STATE_IDLE == sRtcControl->State)
	{
		DispI = sRtcControl->DataTime.Hour;
 		if(DispI/10)
 			DispNumber(DispI/10,LINE1,0);
 		DispNumber(DispI%10,LINE1,1);
		DispI = sRtcControl->DataTime.Min;
		DispNumber(DispI/10,LINE1,2);
		DispNumber(DispI%10,LINE1,3);	
		if(flash_f)
		{
			SetIcon(COL1_ADDR,COL1_VALUE);
			SetIcon(COL2_ADDR,COL2_VALUE);
		}
	}
	else if(RTC_STATE_SET_TIME == sRtcControl->State)
	{
		if(sRtcControl->SubState == RTC_SET_HOUR)
		{
			if(flash_f)
			{
				DispI = sRtcControl->DataTime.Hour;
		 		if(DispI/10)
		 			DispNumber(DispI/10,LINE1,0);
		 		DispNumber(DispI%10,LINE1,1);
			}
			DispI = sRtcControl->DataTime.Min;
			DispNumber(DispI/10,LINE1,2);
			DispNumber(DispI%10,LINE1,3);	
			SetIcon(COL1_ADDR,COL1_VALUE);
			SetIcon(COL2_ADDR,COL2_VALUE);
		}
		else if(sRtcControl->SubState == RTC_SET_MIN)
		{
			DispI = sRtcControl->DataTime.Hour;
	 		if(DispI/10)
	 			DispNumber(DispI/10,LINE1,0);
	 		DispNumber(DispI%10,LINE1,1);
			if(flash_f)
			{
				DispI = sRtcControl->DataTime.Min;
				DispNumber(DispI/10,LINE1,2);
				DispNumber(DispI%10,LINE1,3);				
			}
			SetIcon(COL1_ADDR,COL1_VALUE);
			SetIcon(COL2_ADDR,COL2_VALUE);
		}
	}
	else if(RTC_STATE_SET_ALARM == sRtcControl->State)
	{
		if(sRtcControl->SubState == RTC_SET_HOUR)
		{
			if(flash_f)
			{
				DispI = sRtcControl->AlarmTime.Hour;
		 		if(DispI/10)
		 			DispNumber(DispI/10,LINE1,0);
		 		DispNumber(DispI%10,LINE1,1);
			}
			DispI = sRtcControl->AlarmTime.Min;
			DispNumber(DispI/10,LINE1,2);
			DispNumber(DispI%10,LINE1,3);	
			SetIcon(COL1_ADDR,COL1_VALUE);
			SetIcon(COL2_ADDR,COL2_VALUE);
		}
		else if(sRtcControl->SubState == RTC_SET_MIN)
		{
			DispI = sRtcControl->AlarmTime.Hour;
	 		if(DispI/10)
	 			DispNumber(DispI/10,LINE1,0);
	 		DispNumber(DispI%10,LINE1,1);
			if(flash_f)
			{
				DispI = sRtcControl->AlarmTime.Min;
				DispNumber(DispI/10,LINE1,2);
				DispNumber(DispI%10,LINE1,3);				
			}
			SetIcon(COL1_ADDR,COL1_VALUE);
			SetIcon(COL2_ADDR,COL2_VALUE);
		}
		#ifdef FUNC_AP8048A_SA_CLOCK_PROJ_EN
		else if(sRtcControl->SubState == RTC_SET_ALARM_ONOFF)
		{
			if(gSys.AlarmOn)
			{
				pstr = "ON";		
				DispChar(pstr,LINE1,1);
			}
			else
			{
				pstr = "OFF";		
				DispChar(pstr,LINE1,1);
			}
		}
		#endif
	}
}
#endif

//========================================
//EQ Message
//========================================
void EQMessageDisp(void)
{
	uint8_t *pstr;
	
	ClearLine(LINE1);	
	//DispChar("EQ",LINE1,0);	
	switch(gSys.Eq)
	{
		case	EQ_STYLE_POP:
				pstr = "Pop";
				break;
				
		case	EQ_STYLE_JAZZ:
				pstr = "Jazz";
				break;
				
		case	EQ_STYLE_ROCK:
				pstr = "Rock";
				break;
				
		case	EQ_STYLE_FLAT:
				pstr = "Flat";
				break;
				
		case	EQ_STYLE_CLASSICAL:
			    #ifdef FUNC_8_TYPE_LCD_EN
				pstr = "Clas";
				#else
				pstr = "Classic";
				#endif
				//StringDisp(pstr,LINE1,7,1);
				return;
	}		
	#ifdef FUNC_8_TYPE_LCD_EN
	DispChar(pstr,LINE1,0);
	#else
	DispChar(pstr,LINE1,2);
	#endif
//	ClearLine(3);
}

//========================================
//Sleep Message
//========================================
#ifdef SLEEP_INCLUDE
void SleepMessage(void)
{
	uint8_t DispI;
	uint8_t *pstr;
	
	ClearAllLine();
	if(SleepState==0)
		{
			pstr="Off";
			DispChar(pstr,LINE1,0);
		}
	else
		{
			//pstr="Sleep  ";
			//DispChar(pstr,LINE1,0);
			if(SleepTimeOver%60)			
				DispI=SleepTimeOver/60+1;			
			else
				DispI=SleepTimeOver/60;
		//	DispI=SLEEP_TABLE[SleepState];
			if(DispI/100)
				DispNumber(DispI/100,LINE1,0);
			DispI=DispI%100;
			DispNumber(DispI/10,LINE1,1);
			DispNumber(DispI%10,LINE1,2);
		}
}
#endif

//========================================
//MVBass Message
//========================================
void BassMessageDisp(void)
{
	uint8_t *pstr;
	
	ClearAllLine();
	if(gSys.AudioVbEffectFlag)
	{
		pstr = "On";		
		#ifdef FUNC_8_TYPE_LCD_EN
		DispChar(pstr,LINE1,1);
		#else
		DispChar(pstr,LINE1,3);
		#endif
	}
	else
	{
		pstr = "Off";		
		#ifdef FUNC_8_TYPE_LCD_EN
		DispChar(pstr,LINE1,1);
		#else
		DispChar(pstr,LINE1,3);
		#endif
	}
}

//========================================
//Stereo Message
//========================================
void StereoMessageDisp(void)
{
#if 0
	unsigned char *pstr;
	ClearAllLine();
	if(stereo_mode)
		{
			pstr="Stereo ";
		}
	else
		{
			pstr=" Mono  ";
		}
	DispChar(pstr,LINE1,2);
#endif
}

//========================================
//Dimmer Message
//========================================
void DimmerMessage(void)
{
#ifdef DIMMER_CTL_INCLUDE  
	unsigned char *pstr;
	//ClearAllLine();
	//ClearIcon(COL_ADDR,COL_VALUE);
	//ClearIcon(POINT_ADDR,POINT_VALUE); 
	pstr="    ";
	DispChar(pstr,LINE1,0);   
	
	if(DimmerState)
		{
			pstr="DM-";
			DispChar(pstr,LINE1,1); 
			#ifdef DIMMER_CTL_INCLUDE
			DispNumber((DimmerState),LINE1,3);
			#endif
		}
	else
		{
			pstr="Off";
			DispChar(pstr,LINE1,1); 
		}
#endif
}

//========================================
//Soundmode Message
//========================================
void SoundModeMessage(void)
{ 
#if 0
       unsigned char far *pstr;
	   
       ClearLine(LINE1);
	switch(SoundMode)
	{
		case MOVIE_MODE:
			pstr=" MOVIE  ";
			DispChar(pstr,LINE1,0);
			break;
		case MUSIC_MODE:
			pstr=" MUSIC  ";
			DispChar(pstr,LINE1,0);
			break;
		case GAME_MODE:
			pstr="  GAME  ";
			DispChar(pstr,LINE1,0);
			break;
		case NEWS_MODE:
			pstr="  NEWS  ";
			DispChar(pstr,LINE1,0);
			break;
		case SPORTS_MODE:
			pstr=" SPORTS ";
			DispChar(pstr,LINE1,0);
			break;
		case STANDARD_MODE:
			pstr="STANDARD";
			DispChar(pstr,LINE1,0);
			break;
	}
#endif
}

//========================================
//TrebBass Message
//========================================
void TrebBassMessage(void)
{
}

//========================================
//File number Message
//========================================
void FileNumMessage(void)
{
	uint8_t *pstr;
	uint8_t DispI; 
	
	if(gPlayContrl == NULL)
	{
	   	return;
	}	   
	ClearLine(LINE1);		
	if(gPlayContrl->PlayerFile.FileNumInDisk/1000)
	{	
	#ifdef FUNC_8_TYPE_LCD_EN
		DispNumber(gPlayContrl->PlayerFile.FileNumInDisk/1000,LINE1,0);
		DispI = ((gPlayContrl->PlayerFile.FileNumInDisk%1000)/100);							
		DispNumber(DispI,LINE1,1);
		DispI = gPlayContrl->PlayerFile.FileNumInDisk%100;
		DispNumber(DispI/10,LINE1,2);
		DispNumber(DispI%10,LINE1,3);	
	#else
		DispNumber(gPlayContrl->PlayerFile.FileNumInDisk/1000,LINE1,2);
		DispI = ((gPlayContrl->PlayerFile.FileNumInDisk%1000)/100);							
		DispNumber(DispI,LINE1,3);
		DispI = gPlayContrl->PlayerFile.FileNumInDisk%100;
		DispNumber(DispI/10,LINE1,4);
		DispNumber(DispI%10,LINE1,5);
	#endif
	}
	else
	{						
		DispI = gPlayContrl->PlayerFile.FileNumInDisk/100;
		#ifdef FUNC_8_TYPE_LCD_EN
		if(DispI)
		{
			DispNumber(DispI,LINE1,1);
		}
		DispI = gPlayContrl->PlayerFile.FileNumInDisk%100;
		DispNumber(DispI/10,LINE1,2);
		DispNumber(DispI%10,LINE1,3);
		#else
		if(DispI)
		{
			DispNumber(DispI,LINE1,3);
		}
		DispI = gPlayContrl->PlayerFile.FileNumInDisk%100;
		DispNumber(DispI/10,LINE1,4);
		DispNumber(DispI%10,LINE1,5);
		#endif
	}		
}

//========================================
//File Sum Message
//========================================
void FileSumMessage(void)
{
	uint8_t *pstr;
	uint8_t DispI;   
	
	if(gPlayContrl == NULL)
	{
	   	return;
	}	   	
	ClearLine(LINE1);		   
	if(gPlayContrl->TotalFileNumInDisk/1000)
	{	
	#ifdef FUNC_8_TYPE_LCD_EN
	    DispNumber(gPlayContrl->TotalFileNumInDisk/1000,LINE1,0);
		DispI = ((gPlayContrl->TotalFileNumInDisk%1000)/100);							
		DispNumber(DispI,LINE1,1);
		DispI = gPlayContrl->TotalFileNumInDisk%100;
		DispNumber(DispI/10,LINE1,2);
		DispNumber(DispI%10,LINE1,3);
	#else
		DispNumber(gPlayContrl->TotalFileNumInDisk/1000,LINE1,2);
		DispI = ((gPlayContrl->TotalFileNumInDisk%1000)/100);							
		DispNumber(DispI,LINE1,3);
		DispI = gPlayContrl->TotalFileNumInDisk%100;
		DispNumber(DispI/10,LINE1,4);
		DispNumber(DispI%10,LINE1,5);
	#endif
	}
	else
	{						
		DispI = gPlayContrl->TotalFileNumInDisk/100;
		#ifdef FUNC_8_TYPE_LCD_EN
		if(DispI)
		{
			DispNumber(DispI,LINE1,1);
		}
		DispI = gPlayContrl->TotalFileNumInDisk%100;
		DispNumber(DispI/10,LINE1,2);
		DispNumber(DispI%10,LINE1,3);
		#else
		if(DispI)
		{
			DispNumber(DispI,LINE1,3);
		}
		DispI = gPlayContrl->TotalFileNumInDisk%100;
		DispNumber(DispI/10,LINE1,4);
		DispNumber(DispI%10,LINE1,5);
		#endif
	}			
}

//========================================
//Play mode Message
//========================================
void PlayModeMessage(void)
{
	uint8_t *pstr;

	ClearLine(LINE1);
	switch(gPlayContrl->CurPlayMode)
	{
		case PLAY_MODE_REPEAT_ALL:
			pstr = "all";
			#ifdef FUNC_8_TYPE_LCD_EN
			DispChar(pstr,LINE1,0);	
			#else
			DispChar(pstr,LINE1,3);	
			#endif
			break;

		case PLAY_MODE_REPEAT_FOLDER:
			pstr = "fold";
			#ifdef FUNC_8_TYPE_LCD_EN
			DispChar(pstr,LINE1,0);	
			#else
			DispChar(pstr,LINE1,3);	
			#endif
			break;

		case PLAY_MODE_REPEAT_ONE:
			pstr = "one";
			#ifdef FUNC_8_TYPE_LCD_EN
			DispChar(pstr,LINE1,0);	
			#else
			DispChar(pstr,LINE1,3);	
			#endif
			break;

		case PLAY_MODE_RANDOM:			
			#ifdef FUNC_8_TYPE_LCD_EN
			pstr = "Rand";
			DispChar(pstr,LINE1,0);	
			#else
			pstr = "Random";
			DispChar(pstr,LINE1,3);	
			#endif
			break;
	}
}

void RepeatABMessage(void)
{
	uint8_t *pstr;

	ClearLine(LINE1);
	
	if(gPlayContrl->RepeatAB.RepeatFlag == REPEAT_A_SETED)
	{
		pstr = " A- ";
		#ifdef FUNC_8_TYPE_LCD_EN
		DispChar(pstr,LINE1,0);	
		#else
		DispChar(pstr,LINE1,2);	
		#endif
	}
	else if(gPlayContrl->RepeatAB.RepeatFlag == REPEAT_OPENED)
	{
		pstr = " A-B";
		#ifdef FUNC_8_TYPE_LCD_EN
		DispChar(pstr,LINE1,0);	
		#else
		DispChar(pstr,LINE1,2);	
		#endif
	}
}

//========================================
//Load Message
//========================================
void LoadMessage(void)
{
	uint8_t *pstr;

	ClearLine(LINE1);
	pstr = "Read";
	#ifdef FUNC_8_TYPE_LCD_EN
	DispChar(pstr,LINE1,0);	
	#else
	DispChar(pstr,LINE1,2);	
	#endif
}

//========================================
//Usb Message
//========================================
void UsbMessage(void)
{
	uint8_t *pstr;

	ClearLine(LINE1);
	pstr = "Usb";
	#ifdef FUNC_8_TYPE_LCD_EN
	DispChar(pstr,LINE1,0);	
	#else
	DispChar(pstr,LINE1,3);	
	#endif
}

//========================================
//Sd Message
//========================================
void SdMessage(void)
{
	uint8_t *pstr;

	ClearLine(LINE1);
	pstr = "Sd";
	#ifdef FUNC_8_TYPE_LCD_EN
	DispChar(pstr,LINE1,0);	
	#else
	DispChar(pstr,LINE1,3);	
	#endif
}

//========================================
//Select num Message
//========================================
void SelectNumMessage(void)
{
	uint8_t *pstr;
	uint8_t DispI; 
	   
	ClearLine(LINE1);		
	if(gPlayContrl->InputNumber/1000)
	{						
		DispNumber(gPlayContrl->InputNumber/1000,LINE1,0);
		DispI = ((gPlayContrl->InputNumber%1000)/100);							
		DispNumber(DispI,LINE1,1);
		DispI = gPlayContrl->InputNumber%100;
		DispNumber(DispI/10,LINE1,2);
		DispNumber(DispI%10,LINE1,3);
	}
	else
	{						
		DispI = gPlayContrl->InputNumber/100;
		if(DispI)
		{
			DispNumber(DispI,LINE1,1);
		}
		DispI = gPlayContrl->InputNumber%100;
		DispNumber(DispI/10,LINE1,2);
		DispNumber(DispI%10,LINE1,3);
	}		
}

#ifdef FUNC_RECORD_EN
//========================================
//Record Message
//========================================
void RecordMessage(void)
{
	uint8_t *pstr;

	ClearLine(LINE1);
	pstr = "Record";
	DispChar(pstr,LINE1,1);	
}

//========================================
//RecPlayback Message
//========================================
void RecPlaybackMessage(void)
{
	uint8_t *pstr;

	ClearLine(LINE1);
	pstr = "Playback";
	DispChar(pstr,LINE1,0);	
}
#endif

#ifdef FUNC_RECORD_FILE_DELETE_EN
//========================================
//FileDelete Message
//========================================
void FileDeleteMessage(void)
{
	uint8_t *pstr;

	ClearLine(LINE1);
	if(gSys.DeleteStep == 1)
	{
		if(flash_f)
		{
		#ifdef FUNC_8_TYPE_LCD_EN
		    pstr="Dele";
			DispChar(pstr,LINE1,0);
		#else
			pstr="Delete";
			DispChar(pstr,LINE1,1);
		#endif
		}
	}
	else
	{
	#ifdef FUNC_8_TYPE_LCD_EN
	    pstr="Dele";
		DispChar(pstr,LINE1,0);
		#else
		pstr = "Delete";
		DispChar(pstr,LINE1,1);	
	#endif
	}
}
#endif

#ifdef FUNC_SPI_UPDATE_EN
//========================================
//Upgrade Message
//========================================
void UpgradeMessage(void)
{
	uint8_t *pstr;

	ClearLine(LINE1);
	#ifdef FUNC_8_TYPE_LCD_EN
	pstr = "Upgr";
	DispChar(pstr,LINE1,0);	
	#else
	pstr = "Upgrade";
	DispChar(pstr,LINE1,0);	
	#endif
}
#endif

#ifdef FUNC_PROGRAM_EN
void DispProgramFull(void)
{
	uint8_t *pstr;

	ClearLine(LINE1);
	pstr = "Full";
	DispChar(pstr,LINE1,3);	
}
	
	
void DispProgramNum(void)
{
	uint8_t *pstr;

	ClearLine(LINE1);
	pstr = "P-" ;  
	DispChar(pstr,LINE1,0);
	DispNumber((gSys.ProgramOffset)/10,LINE1,2);
	DispNumber((gSys.ProgramOffset)%10,LINE1,3);
}
#endif

void NoDeviceMessage(void)
{
	uint8_t *pstr;

	ClearLine(LINE1);
	#ifdef FUNC_8_TYPE_LCD_EN
	pstr = "No";
	DispChar(pstr,LINE1,1);	
	#else
	pstr = "No Dev";
	DispChar(pstr,LINE1,1);	
	#endif
}

//========================================
//Icon Display Process
//========================================
void IconDisp(void)
{
	//unsigned char DispK;
	uint8_t temp;
	if(!IsTimeOut(&DispMsgTmr) &&(gDispMsgId == DISP_MSG_PWR_ON))
	{
		return;
	}
	if(!IsTimeOut(&DispMsgTmr) &&(gDispMsgId == DISP_MSG_RADIOON))
	{
		return;
	}
	if(gSys.CurModuleID != MODULE_ID_STANDBY)
	{
		if(gSys.MuteFlag)
		{
			SetIcon(MUTE_ADDR,MUTE_VALUE);	
			return;
		}
		//if(SleepState!=SLEEP_00)
		//{	
			//SetIcon(SLEEP_ADDR,SLEEP_VALUE);
		//}
		if(gSys.CurModuleID == MODULE_ID_RADIO)
		{
			SetIcon(TUNER_ADDR,TUNER_VALUE);
			#ifdef FUNC_AM_EN
			if(sRadioControl->CurBand == RADIO_BAND_AM)
			{
				SetIcon(AM_KHZ_ADDR,AM_KHZ_VALUE);	
			}
			else
			#endif
			{
				SetIcon(FM_MHZ_ADDR,FM_MHZ_VALUE);	
				if((sRadioControl->State != RADIO_STATUS_PREVIEW) && (IsTimeOut(&DispMsgTmr)))
				{
					SetIcon(DP_ADDR,DP_VALUE);	
				}
				#ifdef FUNC_FM_ST_CHECK_EN
				if(sRadioControl->SteroFg && sRadioControl->SteroMode)
				{
					SetIcon(ST_ADDR,ST_VALUE);	
				}
				#endif
			}			
			//SetIcon(S3_ADDR,S3_VALUE);
			//if(MessageTime)
			//{
			//	return;
			//}
			#if 0
			if(current_band==RADIO_FM)
			{	
				SetIcon(MHZ_ADDR,MHZ_VALUE);		
				SetIcon(FM_ADDR,FM_VALUE);	
				SetIcon(DP_ADDR,DP_VALUE);	
				if(stereo_mode&&StereoIn&&(current_band==RADIO_FM))
				//if(stereo_mode&&(current_band==RADIO_FM))	
					{
						if(radio_status==R_CLEARSTATUS)
						{
						//	SetIcon(STEREO_ADDR,STEREO_VALUE);									
						//	SetIcon(S10_ADDR,S10_VALUE); //Left channel icon
						//	SetIcon(S8_ADDR,S8_VALUE); //Right channel icon
						}
					}	
				
			}
			#endif
		}
		else if(gSys.CurModuleID == MODULE_ID_PLAYER_USB)
		{
			SetIcon(USB_ADDR,USB_VALUE);	
			if(GetPlayState() == PLAYER_STATE_PLAYING)
			{
				SetIcon(PLAY_ADDR,PLAY_VALUE);
				
			}
			if((gPlayContrl->CurSongInfo->stream_type == STREAM_TYPE_MP2) ||(gPlayContrl->CurSongInfo->stream_type == STREAM_TYPE_MP3))	
			{
				SetIcon(MP3_ADDR,MP3_VALUE);				
			}
			else if(gPlayContrl->CurSongInfo->stream_type == STREAM_TYPE_WMA)
			{
				SetIcon(WMA_ADDR,WMA_VALUE);				
			}
			#ifdef FUNC_PROGRAM_EN
			if(gSys.ProgramEdit || gSys.ProgramMode)
			{
			    SetIcon(MEM_ADDR,MEM_VALUE);					
			}
			#endif
		}
		else if(gSys.CurModuleID == MODULE_ID_PLAYER_SD)
		{
			SetIcon(SD_ADDR,SD_VALUE);	
			if(GetPlayState() == PLAYER_STATE_PLAYING)
			{
				SetIcon(PLAY_ADDR,PLAY_VALUE);
				
			}
			if((gPlayContrl->CurSongInfo->stream_type == STREAM_TYPE_MP2) ||(gPlayContrl->CurSongInfo->stream_type == STREAM_TYPE_MP3))	
			{
				SetIcon(MP3_ADDR,MP3_VALUE);				
			}
			else if(gPlayContrl->CurSongInfo->stream_type == STREAM_TYPE_WMA)
			{
				SetIcon(WMA_ADDR,WMA_VALUE);				
			}
			#ifdef FUNC_PROGRAM_EN
			if(gSys.ProgramEdit || gSys.ProgramMode)
			{
			    SetIcon(MEM_ADDR,MEM_VALUE);					
			}
			#endif
		}
		#ifdef FUNC_RECORD_EN
		else if(gSys.CurModuleID == MODULE_ID_RECORDER)
		{
			if(gRecordContrl->RecordDeviceID == USB_DISK)
			{
			    if(flash_f)
		    	{
					SetIcon(USB_ADDR,USB_VALUE);	
		    	}
			}
			else
			{
				if(flash_f)
				{
					SetIcon(SD_ADDR,SD_VALUE);	
				}
			}
		}
		else if(gSys.CurModuleID == MODULE_ID_REC_BACK_PLAY)
		{
			if(gSys.DevId == USB_DISK)
			{
				if(flash_f)
		    	{
					SetIcon(USB_ADDR,USB_VALUE);	
		    	}	
			}
			else
			{
				if(flash_f)
				{
					SetIcon(SD_ADDR,SD_VALUE);	
				}
			}
			if(GetPlayState() == PLAYER_STATE_PLAYING)
			{
				SetIcon(PLAY_ADDR,PLAY_VALUE);				
			}
			if((gPlayContrl->CurSongInfo->stream_type == STREAM_TYPE_MP2) ||(gPlayContrl->CurSongInfo->stream_type == STREAM_TYPE_MP3))	
			{
				SetIcon(MP3_ADDR,MP3_VALUE);				
			}
		}
		#endif
		//else if(SystemState==SYS_IPOD)
		//{
		//	SetIcon(IPOD1_ADDR,IPOD1_VALUE);
		//	SetIcon(IPOD2_ADDR,IPOD2_VALUE);
		//}
	}
	else
	{
	}
}

void DispSendMessage(uint16_t DispMsgTime, uint8_t DispMsgId)
{
	if(DISP_MSG_MUTE == DispMsgId)
	{
		TimeOutSet(&DispMsgTmr, 0);
		DispMute();
	}
	else
	{
		TimeOutSet(&DispMsgTmr, DispMsgTime);
	}
	gDispMsgId = DispMsgId;
}

#ifdef FUNC_AP8048A_SA_CLOCK_PROJ_EN
TIMER	 BtDisconnectTimer;
void LedDisp(void)
{
	switch(gSys.CurModuleID)
   	{
   	    case MODULE_ID_BT_HF:
			AuxLedOff();	
			FmLedOff();
			MHzLedOff();
			HfLedOn();			
			break;
		case MODULE_ID_BLUETOOTH:			
			AuxLedOff();	
			FmLedOff();
			MHzLedOff();
			HfLedOff();
			break;
		case MODULE_ID_LINEIN:
			AuxLedOn();	
			FmLedOff();
			MHzLedOff();
			HfLedOff();		
			break;
		case MODULE_ID_RADIO:
			AuxLedOff();	
			FmLedOn();
			MHzLedOff();
			HfLedOff();		
			break;
		default:	
			AuxLedOff();	
			FmLedOff();
			MHzLedOff();
			HfLedOff();		
			break;
   	}
	if(gSys.AlarmOn)
	{
	    if(sRtcControl != NULL)
    	{
	    	AlarmLedOn();
    	}
		else
		{
		    if(sRtcControl->State == RTC_STATE_SET_ALARM)
	    	{
		    	if(flash_f)
	    		{
		    		AlarmLedOn();
	    		}
				else
				{
					AlarmLedOff();
				}
	    	}
			else
			{
				AlarmLedOn();
			}
		}
	}
	else
	{
		AlarmLedOff();
	}
	if(!gSys.bBtConnectFlag)
	{
		if(IsTimeOut(&BtDisconnectTimer))
		{
			if(gSys.bFlashInvFlag)
			{
				gSys.bFlashInvFlag = 0;
				BtLedOn();	
			}
			else 
			{
				gSys.bFlashInvFlag = 1;
				BtLedOff();	
			}
			TimeOutSet(&BtDisconnectTimer,500);
		}
	}
	else
	{
		BtLedOn();	
	}
}
#endif

//=========================================
//Display Process
//=========================================
void DisplayProcess(uint8_t Mode)
{	
	//	R_CLK_lo;
	if(Mode)
	{
	 	if(IsTimeOut(&DispInitFlashTmr))
       	{
       		DisplayInit();
			TimeOutSet(&DispInitFlashTmr, LCDFLASH_TIME);				
			return;
       	}
	}
	if(IsTimeOut(&DispFlashTmr))
   	{
		TimeOutSet(&DispFlashTmr, FLASH_TIME);	
		if(flash_f)
		{
			flash_f = 0;
		}
		else
		{
			flash_f = 1;
		}
   	}
	switch(gSys.CurModuleID)
	{
	#ifdef FUNC_RTC_EN
	    case    MODULE_ID_RTC:
			    ClockDisp();
				break;
	#endif
		case	MODULE_ID_STANDBY:
		#ifdef FUNC_AP8048A_SA_CLOCK_PROJ_EN
		        ClockDisp();
		#else
				StandByMessage();		
		#endif
				break;
				
		#ifdef FUNC_RADIO_EN
		case	MODULE_ID_RADIO:
			    #ifdef FUNC_NEW_RTC_MODE_EN
			    if(RTC_STATE_IDLE != sRtcControl->State)
		    	{
			    	ClockDisp();
					break;
		    	}
				#endif
				RadioDisp();
				break;
		#endif
		
		#ifdef FUNC_LINEIN_EN
		case	MODULE_ID_LINEIN:
			    #ifdef FUNC_NEW_RTC_MODE_EN
			    if(RTC_STATE_IDLE != sRtcControl->State)
		    	{
			    	ClockDisp();
					break;
		    	}
				#endif
				AUXDisp();
				break;
		#endif	

        #ifdef FUNC_BT_EN
		case	MODULE_ID_BLUETOOTH:
			    #ifdef FUNC_NEW_RTC_MODE_EN
			    if(RTC_STATE_IDLE != sRtcControl->State)
		    	{
			    	ClockDisp();
					break;
		    	}
				#endif
				BtDisp();
				break;
		#endif
		#ifdef FUNC_PHIN_EN
		case	MODULE_ID_PH:
				PhonoDisp();
				break;
		#endif

		#ifdef FUNC_EXBT_EN
		case	MODULE_ID_EX_BT:
				ExBtDisp();
				break;
		#endif
		
		#ifdef IPOD_INCLUDE
		case	SYS_IPOD:
				IPODDisp();
				break;
		#endif	
		
		#ifdef FUNC_USB_EN
		case	MODULE_ID_PLAYER_USB:
    			UHPDisp();
				break;
		#endif
		#ifdef FUNC_CARD_EN
		case	MODULE_ID_PLAYER_SD:
    			CardDisp();
				break;
		#endif
		#ifdef FUNC_RECORD_EN
		case    MODULE_ID_RECORDER:
				RecordDisp();
				break;
		case    MODULE_ID_REC_BACK_PLAY:
				RecPlaybackDisp();
			    break;
		#endif

	}			
	if(gSys.MuteFlag && (gSys.CurModuleID != MODULE_ID_STANDBY))
	{
		DispMute();
	}
	//if(MessageTime!=0)
	if(!IsTimeOut(&DispMsgTmr))
	{
		switch(gDispMsgId)
		{
			case	DISP_MSG_PWR_ON:
					PowerOnMessage();
					//SendDisplayData();	
					break;
					
			case	DISP_MSG_PWR_OFF:
					PowerOffMessage();
					//SendDisplayData();	
					//return;
					break;
								
            #ifdef FUNC_RADIO_EN
			case	DISP_MSG_STEREO:
					StereoMessageDisp();		
					break;
            #ifdef RADIO_SEMI_MEM_EN
			case	DISP_MSG_RADIOPROG:
					RadioProgMessage();
					break;	
			#endif		
            case    DISP_MSG_RADIO_CH:
					RadioPreChNumMessage();
					break;
					
			case	DISP_MSG_RADIOON:
					RadioOnMessage();
					break;	
			case    DISP_MSG_RADIO_BAND:
					RadioBandMessage();
					break;
			#endif		
			#ifdef SLEEP_INCLUDE
			case	DISP_MSG_SLEEP:
					SleepMessage();
					break;
			#endif
			
			#ifdef CLOCK_INCLUDE		
			case	DISP_MSG_CLOCK:
					ClockDisp();
					break;
			#endif	
			
			case	DISP_MSG_VER:
					VersionMessage();
					break;
					
			case	DISP_MSG_SYSINIT:
					SystemInitMessage();
					break;
					
			case	DISP_MSG_MUTE:
					MuteMessage();
					break;	
					
			case	DISP_MSG_MVOL:
					VolumeDisp();
					break;
					
			case	DISP_MSG_EQ:
					EQMessageDisp();
					break;
					
			case	DISP_MSG_BASS:
					BassMessageDisp();
					break;	
					
			case    DISP_MSG_3D:
					Tone3dMessage();
					break;		
					
			case	DISP_MSG_PROTECT:
					ProtectMessage();
					break;
					
			#ifdef DIMMER_CTL_INCLUDE
			case	DISP_MSG_DIMMER:
					DimmerMessage();
					break;
			#endif			
					
			case	DISP_MSG_FILE_NUM:
					FileNumMessage();
					break;
					
			case	DISP_MSG_FILE_SUM:
					FileSumMessage();
					break;
					
			case	DISP_MSG_LOAD:
					LoadMessage();
					break;
					
			case	DISP_MSG_USB:
					UsbMessage();
					break;	

			case	DISP_MSG_SD:
					SdMessage();
					break;	
					
			case    DISP_MSG_NO_DEV:
					NoDeviceMessage();
					break;
					
			case 	DISP_MSG_PLAY_MODE:
					PlayModeMessage();
					break;
					
			case 	DISP_MSG_REP_AB:
					RepeatABMessage();
					break;	
			case DISP_MSG_SEL_NUM:
					SelectNumMessage();
					break;
			#ifdef FUNC_RECORD_EN
			case DISP_MSG_RECORD:
					RecordMessage();
					break;
			case DISP_MSG_RECPLAYBACK:
					RecPlaybackMessage();
					break;
			#endif
			#ifdef FUNC_RECORD_FILE_DELETE_EN
			case DISP_MSG_FILE_DEL:
				    FileDeleteMessage();
					break;
			#endif
			#ifdef FUNC_SPI_UPDATE_EN
			case DISP_MSG_UPGRADE:
					UpgradeMessage();
					break;
			#endif		
			#ifdef FUNC_PROGRAM_EN
			case DISP_MSG_PROG_FULL:
				DispProgramFull();
				break;
			case DISP_MSG_PROG_NUM:
				DispProgramNum();
				break;
			#endif
		}
	}
	else
	{
	    #ifdef FUNC_AP8048B_SW_T450_PROJ_EN
		if(gSys.DeleteStep)
		{
			gSys.DeleteStep = 0;
		}
		#endif
	}
	IconDisp();	
#ifdef FUNC_AP8048A_SA_CLOCK_PROJ_EN
    LedDisp();
#endif
	SendDisplayData();	
}
#endif


