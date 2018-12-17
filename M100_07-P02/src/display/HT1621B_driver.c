//³ÌĞòÃû£ºHT1621B_driver.C¡£
//±àĞ´ÈË£ºÀîÖÎÇå
//±àĞ´ÈÕÆÚ£º2015.09.30
//ÏµÍ³¾§Ìå£º96MHz
//MCUÀàĞÍ£ºM3
//½Ó¿Ú¶¨Òå£ºHT_dat-1621µÄdataÊı¾İÏß£¬HT_wr-1621µÄĞ´Ê±ÖÓÍ¬²½Ïß£¬HT_cs-1621µÄÊ¹ÄÜ/¸´Î»Ïß£¬
//          HT_rd-1621µÄ¶ÁÊ±ÖÓÍ¬²½Ïß¡£
//Ê¹ÓÃËµÃ÷£º
//ĞŞ¸ÄÈÕÖ¾£º
/*************************************************************************************/
//Í·ÎÄ¼ş°üº¬
#include "gpio.h"
#include "HT1621B_driver.h"
#include "app_config.h"
#include "seg_panel.h"
#include "dev_detect_driver.h"
#include "sys_app.h"
#include "task_decoder.h"
#include "bt_app_func.h"
#include "bt_control_api.h"
#include "sd_card.h"
#include "eq.h"
#include "eq_params.h"
#include "clk.h"
#include "fsinfo.h"
#include "sys_vol.h"
#include "sound_remind.h"
#include "mixer.h"
#include "breakpoint.h"
#include "recorder_control.h"
#include "nvm.h"
#include "browser.h"
#include "lrc.h"
#include "timer_api.h"
#include "string_convert.h"
#include "player_control.h"
#include "rtc_control.h"
#include "radio_control.h"

#ifdef    FUNC_HT1621_DISPLAY_EN
/***********************************************************************************/
//³£ÓÃ¿ØÖÆÃüÁî
#define   SYS_dis    0x00   //¹Ø±ÕÏµÍ³Ê±ÖÓ
#define   SYS_on     0x01   //¿ªÏµÍ³Ê±ÖÓ
#define   LCD_off    0x02   //¹Ø±ÕLCDÏÔÊ¾
#define   LCD_on     0x03   //¿ªLCDÏÔÊ¾
#define   timer_dis  0x04   //¹Ø±ÕÊ±ÖÓÊä³ö
#define   WDT_dis    0x05   //¹Ø±Õ¿´ÃÅ¹·
#define   tone_off   0x08   //¹Ø±ÕÉùÒôÊä³ö
#define   XTAL       0x18   //ÏµÍ³¾§ÌåÑ¡Ôñ
#define   BIAS       0x29   //Æ«Ñ¹¼°Õ¼¿Õ±ÈÉèÖÃ

TIMER  ColDispTimer;
TIMER  PrevDispTimer;
TIMER  UpdataDispTimer;
DISP_MODULE_ID    CurDisplayId  =  DISP_MODULE_NONE;              //µ±Ç°ĞèÒªÏÔÊ¾µÄÄÚÈİ
 //ÉÏÒ»´ÎÏÔÊ¾µÄÄÚÈİ£¬ÓÃÓÚµ±Ä³¸öÄÚÈİĞèÒªÏÔÊ¾¹Ì¶¨Ê±¼ä£¬ÔÚÏÔÊ¾¹ı³ÌÖĞ²»ÄÜË¢ĞÂµ±Ç°ÏÔÊ¾ÄÚÈİµÄÎÊÌâ¬
static DISP_MODULE_ID    PrevDisplayId = DISP_MODULE_NONE;
static bool ColDispFlag;

static uint8_t const NumberTab [10] = {0xFD,0x0D,0xDB,0x9F,0x2F,0xBE,0xFE,0x1D,0xFF,0xBF};
//.......................................0.....1...2....3....4....5....6.....7....8....9.//
//ÎªÁËÔÚË¢ĞÂÏÔÊ¾Êı¾İÊ±²»Ó°ÏìÆäËû±êÖ¾µÄÏÔÊ¾£¬ËùÒÔ½«¿ÕÏĞBITÎª1£¬ÔÚË¢ĞÂÊı¾İÊ±ÓÃ¡°Âß¼­Óë¡±

uint8_t  DisplayBuffer [13] = {0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x67,0x00,0x00,0x00,0x00,0x00}; 	

/*************************************************************************************/
#ifdef FUNC_HT1621_DISPLAY_EN
static __INLINE void HT1621DisplayDelay(void)
{
	__nop();
}
#endif
/*************************************************************************************/
static  void  Init_1621(void)
{
      HT1621_CS_SET();
	    HT1621_CLK_SET();
	    HT1621_DAT_SET();
	    HT1621DisplayDelay();
	    HT1621_CS_CLR();
	    HT1621_CS_CLR();
	    HT1621_CLK_CLR();
	    HT1621_CLK_CLR();
}
/*************************************************************************************/
static  void  Sendchar_h(uint8_t dat,uint8_t count)                 //datµÄcountÎ»Ğ´Èë1621£¬¸ßÎ»ÔÚÇ°
{
      for (;count > 0;count--)
	    {
          if ((dat&0x80) == 0x00)
	           HT1621_DAT_CLR();
	        else
	           HT1621_DAT_SET();
	       HT1621_CLK_CLR();
	       HT1621DisplayDelay();
	       HT1621_CLK_SET();
	       dat <<= 1;
      }
}
/*************************************************************************************/
static  void  Sendchar_l(uint8_t dat,uint8_t count)                 //datµÄcountÎ»Ğ´Èë1621£¬µÍÎ»ÔÚÇ°
{
      for (;count > 0;count--)
	    {
          if ((dat&0x01) == 0x00)
	           HT1621_DAT_CLR();
	        else
	           HT1621_DAT_SET();
	       HT1621_CLK_CLR();
	       HT1621DisplayDelay();
	       HT1621_CLK_SET();
	       dat >>= 1;
      }
}
/*************************************************************************************/
static  void  write_cmd(uint8_t command)                          //Ğ´1621¿ØÖÆÃüÁî
{   
      HT1621_CS_CLR();
	    Init_1621();
      Sendchar_h(0x80,3);
      Sendchar_h(command,9);
      HT1621_CS_SET();
}
/*************************************************************************************/
void  HT1621DisplayClr(void)    //Çå³ı1621ËùÓĞÏÔÊ¾¡¢ÇåÆÁ
{
	    uint8_t count = 0;
	    for(count = 0; count < 13; count++)
			{
				DisplayBuffer[count] = 0x00;
			}
}
/*************************************************************************************/
void  SendHt1621DispMsg(DISP_MODULE_ID  DispId)               //´«ËÍÏÔÊ¾ÃüÁî
{
	    CurDisplayId = DispId;
}
/*************************************************************************************/
static void  DisplayModuleFlag(void)    //Ë¢ĞÂÄ£Ê½ºÍÃë±êÖ¾
{
	     //ÏÈÇå³ıËùÓĞÄ£Ê½Í¼±ê¡£
	    DISP_MODEFLAG_OFF();
	    if(gSys.CurModuleID != MODULE_ID_RTC)
			//Èç¹û²»ÊÇÔÚRTCÄ£Ê½£¬½«ÃëÍ¼±ê¹Ø±Õ¡£
			{
				 DISP_COL_OFF();
			}
	    if(gSys.CurModuleID == MODULE_ID_PLAYER_USB)
				//ÏÔÊ¾USBÍ¼±ê
			{
				 DISP_USB_ON();
			}
			else if(gSys.CurModuleID == MODULE_ID_PLAYER_SD)
			{
				 if((GetCurReadCard() == TFTOSD_CARD_TYPE) || \
					   (GetCurReadCard() == SD_CARD_TYPE))
				 //ÔÚSD¿¨Ä£Ê½¡£ÏÔÊ¾SDÍ¼±ê
				 {
					   DISP_SDCARD_ON();
				 }
				 else
					//ÔÚTF¿¨Ä£Ê½£¬ÏÔÊ¾TFÍ¼±ê¡£
				 {
					   DISP_TFCARD_ON();
				 }
			}
			else if (gSys.CurModuleID == MODULE_ID_RADIO)
			//ÏÔÊ¾FMÍ¼±ê
			{
				  DISP_FM_ON();
			}
}
/************************************************************************************/
static void DisplayWelcome(void)        //ÏÔÊ¾»¶Ó­ĞÅÏ¢
{
	    uint8_t  count = 0;
			HT1621_CS_CLR();
	    Init_1621();
      Sendchar_h(0xa0,3);
      Sendchar_h(0x18,6);
      for (count = 0;count < 13 ;count++)
	    {
			  if(count == 6)
				{
           Sendchar_l(0x05,8);
				}
				else if(count == 7)
				{
					 Sendchar_l(0x67,8);
				}
				else
				{
					 Sendchar_l(0x00,8);
				}
      }
      HT1621_CS_SET();
			TimeOutSet(&PrevDispTimer, 3000);
}
/************************************************************************************/
void  HT1621DisplayInit(void)                         //³õÊ¼»¯HT1621£¬²¢ÏÔÊ¾»¶Ó­ĞÅÏ¢
{
      write_cmd(BIAS);                                        //LCDÆ«Ñ¹ÉèÖÃ
      write_cmd(XTAL);                                      //Ê±ÖÓÔ´Ñ¡Ôñ
	    write_cmd(timer_dis);                               //¹Ø±ÕÊ±ÖÓÊä³ö
	    write_cmd(WDT_dis);                                //¹Ø±Õ¿´ÃÅ¹·
	    write_cmd(tone_off);                                  //¹Ø±ÕÉùÒôÊä³ö
      write_cmd(SYS_on);                                  //¿ªÏµÍ³Ê±ÖÓ
      HT1621DisplayDelay();                              //ÊÊµ±ÑÓÊ±
	    TimeOutSet(&ColDispTimer, 0);
	    TimeOutSet(&PrevDispTimer, 0);           //Æô¶¯ÏÔÊ¾Ïà¹Ø¶¨Ê±Æ÷
      write_cmd(LCD_on);                                  //¿ªLCDÏÔÊ¾
	    DisplayWelcome();                                  //ÏÔÊ¾»¶Ó­ĞÅÏ¢
}
/************************************************************************************/
static void DisplayTimer(void)        //ÏÔÊ¾²¥·ÅÊ±¼ä
{
	        DISP_COL_ON();                       //Ãë·ûºÅ³£ÁÁ£¬±íÊ¾ÕıÔÚ²¥·Å
	        DisplayBuffer[8] |= 0xF7;
				  DisplayBuffer[8] &= NumberTab[gPlayContrl->CurPlayTime / 1000 / 60 / 10];
				  DisplayBuffer[7] |= 0xF7;
	        DisplayBuffer[7] &= NumberTab[gPlayContrl->CurPlayTime/ 1000 / 60 % 10];
				  DisplayBuffer[6] |= 0xF7;
	        DisplayBuffer[6] &= NumberTab[gPlayContrl->CurPlayTime / 1000 % 60 / 10];
				  DisplayBuffer[5] |= 0xF7;
	        DisplayBuffer[5] &= NumberTab[gPlayContrl->CurPlayTime / 1000 % 60 % 10];
}
/************************************************************************************/
static void DispSongNumber(void)        //ÏÔÊ¾¸èÇúÇúÄ¿
{
	        DisplayBuffer[8] |= 0xF7;
					DisplayBuffer[8] &= NumberTab[gPlayContrl->CurFileIndex / 1000 ];
				  DisplayBuffer[7] |= 0xF7;
	        DisplayBuffer[7] &= NumberTab[gPlayContrl->CurFileIndex % 1000 / 100];
				  DisplayBuffer[6] |= 0xF7;
	        DisplayBuffer[6] &= NumberTab[gPlayContrl->CurFileIndex % 100  / 10];
				  DisplayBuffer[5] |= 0xF7;
	        DisplayBuffer[5] &= NumberTab[gPlayContrl->CurFileIndex % 10];
	        TimeOutSet(&PrevDispTimer, 2000);
}
/************************************************************************************/
static void DispAudioVol(void)        //ÏÔÒôÁ¿
{
	        DisplayBuffer[8] |= 0xF7;
					DisplayBuffer[8] &= 0xED;    // ×ÖÄ¸"U"
				  DisplayBuffer[7] |= 0xF7;
	        DisplayBuffer[7] &= 0xE8;     //×ÖÄ¸¡°L¡±
				  DisplayBuffer[6] |= 0xF7;
	        DisplayBuffer[6] &= NumberTab[(uint8_t)gSys.Volume  / 10];
				  DisplayBuffer[5] |= 0xF7;
	        DisplayBuffer[5] &= NumberTab[(uint8_t)gSys.Volume % 10];
	        TimeOutSet(&PrevDispTimer, 2000);
}
/************************************************************************************/
static void DispRtcTimer(void)        //ÏÔÊ¾ÊµÊ±Ê±ÖÓĞÅÏ¢¿
{
	     //ÏÔÊ¾ÄêĞÅÏ¢
	     if((GetRtcControlState() != RTC_STATE_IDLE) && (GetRtcControlSubState() == RTC_SET_YEAR)\
				   && (ColDispFlag == FALSE))
			 //µ±Ç°ÕıÔÚÉèÖÃÄê·İ£¬ÉÁË¸Äê·İ±êÖ¾£»ÒÔÏÂÏûÒş
			 {
				  DisplayBuffer[12] |= 0xD7;
					DisplayBuffer[12] &= DISP_HIDDEN | 0x20;
					DisplayBuffer[11] |= 0xF7;
					DisplayBuffer[11] &= DISP_HIDDEN;
				  DisplayBuffer[10] |= 0xF7;
					DisplayBuffer[10] &= DISP_HIDDEN;
				  DisplayBuffer[9] |= 0xF7;
					DisplayBuffer[9] &= DISP_HIDDEN;
			 }
			 else
			 {
					DisplayBuffer[12] |= 0xD7;
					DisplayBuffer[12] &= NumberTab[sRtcControl->DataTime.Year / 1000] | 0x20;
					DisplayBuffer[11] |= 0xF7;
					DisplayBuffer[11] &= NumberTab[sRtcControl->DataTime.Year % 1000 / 100];
				  DisplayBuffer[10] |= 0xF7;
					DisplayBuffer[10] &= NumberTab[sRtcControl->DataTime.Year % 100 / 10];
				  DisplayBuffer[9] |= 0xF7;
					DisplayBuffer[9] &= NumberTab[sRtcControl->DataTime.Year % 10];
			 }
			  //ÏÔÊ¾ÔÂĞÅÏ¢
			 if((GetRtcControlState() != RTC_STATE_IDLE) && (GetRtcControlSubState() == RTC_SET_MON)\
				   && (ColDispFlag == FALSE))
			 //ÏûÒşÔÂ·İÏÔÊ¾
			 {
				  DISP_MONDEC_OFF();
				  DisplayBuffer[4] |= 0x0F;
					DisplayBuffer[4] &= ((DISP_HIDDEN >> 4) | 0xF0);
					DisplayBuffer[3] |= 0x70;
					DisplayBuffer[3] &= ((DISP_HIDDEN << 4) | 0x0F);
					DISP_MON_OFF();
			 }
			 else
			 //ÏÔÊ¾ÔÂĞÅÏ¢
			 {
				  if((sRtcControl->DataTime.Mon / 10) > 0)
					{
						DISP_MONDEC_ON();
					}
					else
					{
						DISP_MONDEC_OFF();
					}
					DisplayBuffer[4] |= 0x0F;
					DisplayBuffer[4] &= ((NumberTab[sRtcControl->DataTime.Mon % 10] >> 4) | 0xF0);
					DisplayBuffer[3] |= 0x70;
					DisplayBuffer[3] &= ((NumberTab[sRtcControl->DataTime.Mon % 10] << 4) | 0x0F);
					DISP_MON_ON();
			 }
			 //ÏÔÊ¾ÌìĞÅÏ¢
			 if((GetRtcControlState() != RTC_STATE_IDLE) && (GetRtcControlSubState() == RTC_SET_DAY)\
				   && (ColDispFlag == FALSE))
			 //ÏûÒşÌìÏÔÊ¾ĞÅÏ¢
			 {
				  DisplayBuffer[2] |= 0xF7;
					DisplayBuffer[2] &= DISP_HIDDEN;
					DisplayBuffer[1] |= 0xF7;
					DisplayBuffer[1] &= DISP_HIDDEN;
					DISP_DATE_OFF();
			 }
			 else
			 //ÏÔÊ¾ÌìĞÅÏ¢
			 {
					DisplayBuffer[2] |= 0xF7;
					DisplayBuffer[2] &= ((sRtcControl->DataTime.Date / 10) == 0)? DISP_HIDDEN : (NumberTab[sRtcControl->DataTime.Date / 10]);
					DisplayBuffer[1] |= 0xF7;
					DisplayBuffer[1] &= NumberTab[sRtcControl->DataTime.Date % 10];
					DISP_DATE_ON();
			 }
			 //ÏÔÊ¾Ğ¡Ê±ĞÅÏ¢
			 if((GetRtcControlState() != RTC_STATE_IDLE) && (GetRtcControlSubState() == RTC_SET_HOUR)\
				   && (ColDispFlag == FALSE))
			 //ÏûÒşĞ¡Ê±
			 {
					DisplayBuffer[8] &= DISP_HIDDEN;
	        DisplayBuffer[7] &= DISP_HIDDEN;
			 }
			 else
			 //ÏÔÊ¾Ğ¡Ê±
			 {
					DisplayBuffer[8] |= 0xF7;
					DisplayBuffer[8] &= NumberTab[sRtcControl->DataTime.Hour / 10];
				  DisplayBuffer[7] |= 0xF7;
	        DisplayBuffer[7] &= NumberTab[sRtcControl->DataTime.Hour % 10];
			 }
			 //ÏÔÊ¾·ÖÖÓĞÅÏ¢
			 if((GetRtcControlState() != RTC_STATE_IDLE) && (GetRtcControlSubState() == RTC_SET_MIN)\
				   && (ColDispFlag == FALSE))
			 //ÏûÒş·ÖÖÓ
			 {
				  DisplayBuffer[6] &= DISP_HIDDEN;
	        DisplayBuffer[5] &= DISP_HIDDEN;
			 }
			 else
			 //ÏÔÊ¾·ÖÖÓ
			 {
				  DisplayBuffer[6] |= 0xF7;
	        DisplayBuffer[6] &= NumberTab[sRtcControl->DataTime.Min / 10];
				  DisplayBuffer[5] |= 0xF7;
	        DisplayBuffer[5] &= NumberTab[sRtcControl->DataTime.Min % 10];
			 }
}
/************************************************************************************/
static void DisplayRadioFreq(void)   //ÏÔÊ¾FMÆµÂÊ
{
#ifdef FUNC_RADIO_EN
	        DISP_DOT_ON();                           //¿ªĞ¡Êıµã                 
				  DISP_MHz_ON();                           //¿ªÆµÂÊµ¥Î»ÏÔÊ¾       
	        DisplayBuffer[8] |= 0xF7;
	        DisplayBuffer[8] &= (sRadioControl->Freq / 1000 == 0)?DISP_HIDDEN : NumberTab[sRadioControl->Freq / 1000 ];
				  DisplayBuffer[7] |= 0xF7;
	        DisplayBuffer[7] &=  (sRadioControl->Freq / 100 == 0)?DISP_HIDDEN : NumberTab[sRadioControl->Freq % 1000 / 100];
				  DisplayBuffer[6] |= 0xF7;
	        DisplayBuffer[6] &=  (sRadioControl->Freq / 10 == 0)?DISP_HIDDEN : NumberTab[sRadioControl->Freq % 100  / 10];
				  DisplayBuffer[5] |= 0xF7;
	        DisplayBuffer[5] &= NumberTab[sRadioControl->Freq % 10];   
#endif	
}
/************************************************************************************/
static void DisplayRadioNumber(void)   //ÏÔÊ¾FMµçÌ¨Ä¿Â¼
{
#ifdef FUNC_RADIO_EN
	        DisplayBuffer[8] |= 0xF7;
	        DisplayBuffer[8] &= DISP_HIDDEN;   //ÏûÒş
				  DisplayBuffer[7] |= 0xF7;
	        DisplayBuffer[7] &=  0x7B;                   //×ÖÄ¸¡°P¡±
				  DisplayBuffer[6] |= 0xF7; 
	        DisplayBuffer[6] &=  NumberTab[((uint8_t)sRadioControl->CurStaIdx +1) / 10];
				  DisplayBuffer[5] |= 0xF7;
	        DisplayBuffer[5] &= NumberTab[((uint8_t)sRadioControl->CurStaIdx+1)  % 10]; 
          TimeOutSet(&PrevDispTimer, 1000);	
#endif
}
/************************************************************************************/
static void DisplayString(void)   //ÏÔÊ¾Ïà¹ØÄ£Ê½ÏÂµÄ×Ö·û´®ĞÅÏ¢
{
	        if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
					{
	            DisplayBuffer[8] |= 0xF7;
	            DisplayBuffer[8] &= 0xEE;                    //×ÖÄ¸¡°b¡±
				      DisplayBuffer[7] |= 0xF7;
	            DisplayBuffer[7] &=  0xE8;                   //×ÖÄ¸¡°L¡±
				      DisplayBuffer[6] |= 0xF7; 
	            DisplayBuffer[6] &=  0xED;                   //×ÖÄ¸¡°U¡±
				      DisplayBuffer[5] |= 0xF7;
	            DisplayBuffer[5] &= 0xFA;                    //×ÖÄ¸¡°E¡±
					}
					else if(gSys.CurModuleID == MODULE_ID_PLAYER_SD || gSys.CurModuleID == MODULE_ID_PLAYER_USB)
					{
						  DisplayBuffer[8] |= 0xF7;
	            DisplayBuffer[8] &= 0x7B;                    //×ÖÄ¸¡°p¡±
				      DisplayBuffer[7] |= 0xF7;
	            DisplayBuffer[7] &=  0x7F;                   //×ÖÄ¸¡°A¡±
				      DisplayBuffer[6] |= 0xF7; 
	            DisplayBuffer[6] &=  0xED;                   //×ÖÄ¸¡°U¡±
				      DisplayBuffer[5] |= 0xF7;
	            DisplayBuffer[5] &= 0xBE;                    //×ÖÄ¸¡°S¡±
					}
					else if(gSys.CurModuleID == MODULE_ID_LINEIN)
					{
						  DisplayBuffer[8] |= 0xF7;
	            DisplayBuffer[8] &= DISP_HIDDEN;    //ÏûÒş
				      DisplayBuffer[7] |= 0xF7;
	            DisplayBuffer[7] &=  0x7F;                   //×ÖÄ¸¡°A¡±
				      DisplayBuffer[6] |= 0xF7; 
	            DisplayBuffer[6] &=  0xED;                  //×ÖÄ¸¡°U¡±
				      DisplayBuffer[5] |= 0xF7;
	            DisplayBuffer[5] &= 0x6F;                    //×ÖÄ¸¡°H¡±
					}
}
/************************************************************************************/
static void  HT1621DisplaySet(void)
{
	   //ÏÔÊ¾Í£ÁôµÄÊ±¼äµ½ÁË£¬»òÕßµ±Ç°ÕıÔÚÏÔÊ¾µÄÄÚÈİĞèÒªË¢ĞÂ
	  if(IsTimeOut(&PrevDispTimer) || CurDisplayId == PrevDisplayId)
		{
			  DISP_RTCDATE_OFF();                                      //Çå³ıÈÕÆÚĞÅÏ¢
			  DISP_MAINFLAG_OFF();                                    //¹Ø±ÕÖ÷ÆÁÄ»µÄÆäËûÍ¼±ê
			  switch(CurDisplayId)
				{
					case  DISP_MODULE_PLAYTIMER:              //ÏÔÊ¾²¥·ÅÊ±¼ä
						 DisplayTimer();
					   break;
					case  DISP_MODULE_SONGNUM:                //ÏÔÊ¾¸èÇúÇúÄ¿
						DispSongNumber();
					  break;
					case  DISP_MODULE_AUDIOVOL:                //ÏÔÊ¾ÒôÁ¿¼¶±ğĞÅÏ¢
						DispAudioVol();
					  break;
					case  DISP_MODULE_RTCTIMER:                //ÏÔÊ¾ÊµÊ±Ê±ÖÓĞÅÏ¢
						DispRtcTimer();
						break;
					case  DISP_MODULE_RADIOFREQ:             //ÏÔÊ¾FMÊÕÌ¨ÆµÂÊ
						DisplayRadioFreq();
            break;	
          case  DISP_MODULE_RADIONUM:              //ÏÔÊ¾FMµçÌ¨Ä¿Â¼
						DisplayRadioNumber();
            break;					
          case  DISP_MODULE_WELCOME:               //ÏÔÊ¾»¶Ó­ĞÅÏ¢
						//DisplayWelcome();
            break;
          case  DISP_MODULE_STRING:                     //ÏÔÊ¾Ïà¹ØÄ£Ê½ÏÂµÄ×Ö·û´®
						DisplayString();
            break;					
					default:
						break;
				}
				PrevDisplayId = CurDisplayId;
				CurDisplayId = DISP_MODULE_NONE;
		}
}
/*************************************************************************************/
void  HT1621UpdataDisplay(void)    //Ë¢ĞÂHT1621ÏÔÊ¾·
{
	    uint8_t  count = 0;
	    static bool DisplayState;
	    //ÕâÀïÅĞ¶ÏÊÇ·ñÓĞÓĞĞ§µÄĞÅÏ¢ĞèÒªÏÔÊ¾
	    if(CurDisplayId  !=  DISP_MODULE_NONE && CurDisplayId  !=  DISP_MODULE_WELCOME)
			{
				  DisplayModuleFlag();
	        HT1621DisplaySet();
			}
			//ÅĞ¶ÏÊÇ·ñĞèÒªÏÔÊ¾ÃëÍ¼±ê¡£
		 if(IsTimeOut(&ColDispTimer) && IsTimeOut(&PrevDispTimer) && (gSys.CurModuleID == MODULE_ID_RTC))
		 {
			    //ÕâÀï²»¶Ï´«ËÍÏÔÊ¾ÊµÊ±Ê±ÖÓÃüÁî£¬ÊÇÎªÁËÔÚÉèÖÃ×´Ì¬Ê±ÏàÓ¦ÉèÖÃÎ»ÉÁË¸£¬ÕâÀï¿ÉÒÔÈ¥µô
			    SendHt1621DispMsg(DISP_MODULE_RTCTIMER);
				  TimeOutSet(&ColDispTimer, 500);
				  ColDispFlag = !ColDispFlag;
				  if(ColDispFlag )
					{
						 DISP_COL_ON();
						//ÉèÖÃÄÖÖÓ×´Ì¬£¬ÄÖÖÓÍ¼±êÉÁË¸
						 if(GetRtcControlState() == RTC_STATE_SET_ALARM)
						 {
							 DISP_ALARM_ON();
						 }
					}
					else
					{
						 DISP_COL_OFF();
						//ÉèÖÃÄÖÖÓ×´Ì¬£¬ÄÖÖÓÍ¼±êÉÁË¸
						 if(GetRtcControlState() == RTC_STATE_SET_ALARM)
						 {
							 DISP_ALARM_OFF();
						 }
					}
			}
			//ÕâÀï·ÀÖ¹Ğ´HT1621Ê±³öÏÖÖØÈëÎÊÌâ£¬Ê±ÖÓ»ìÂÒ¶øÎŞÏÔÊ¾
	    if(DisplayState == FALSE)                        
		  {
				  DisplayState = TRUE;
				  HT1621_CS_CLR();
	        Init_1621();
          Sendchar_h(0xa0,3);
          Sendchar_h(0x18,6);
          for (count = 0;count < 13 ;count++)
	        {
                Sendchar_l(DisplayBuffer[count],8);
          }
          HT1621_CS_SET();
				  DisplayState = FALSE;
			}
}

#endif
