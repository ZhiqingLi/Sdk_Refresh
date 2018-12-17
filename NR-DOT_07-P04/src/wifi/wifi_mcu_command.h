#ifndef _WIFI_MCU_COMMAND_H_
#define _WIFI_MCU_COMMAND_H_

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//****************************************************************************************
//					MCU ¶ËÃüÁî¼¯
//****************************************************************************************

//******************************************************************
//	3 ³ö³§¿ª»ú
//******************************************************************
#define MCU_CAP_PRJ 	0x01	//MCU»Ø¸´ÏîÄ¿Ãû,Õâ¸ö±ØÐë»Ø¸´£¬¸ñÊ½ÒªÇóÎª¡°branchname_modelname¡±£¬ÀýÈç¡°MUZO_W001¡±,ÏÂ»®ÏßÇ°ÎªÆ·ÅÆÃû£¬ÏÂ»®ÏßºóÎªÐÍºÅ¡£
#define MCU_PTV_XXX		0x02	//ÉèÖÃÊ¹ÓÃµÄMCU Ð­ÒéµÄ°æ±¾ºÅ£¬Ä¬ÈÏ000£»
#define MCU_ALI_PID		0x03	//°¢Àï²úÆ·IDÉèÖÃ
#define MCU_SPY_NAM 	0x04	//Èç¹ûÐèÒªSpotifyÈÏÖ¤£¬»Ø¸´´ËÏîÉèÖÃmodename£¬Èç¹û²»»Ø¸´Ä¬ÈÏÓëproject_nameÒ»ÖÂ
#define MCU_SPY_BRN 	0x05	//Èç¹ûÐèÒªSpotifyÈÏÖ¤£¬»Ø¸´´ËÏîÉèÖÃbrandname£¬Èç¹û²»»Ø¸´Ä¬ÈÏÄ¬ÈÏÓëproject_nameÒ»ÖÂ
#define MCU_SPY_MOD 	0x06	//Èç¹ûÐèÒªSpotifyÈÏÖ¤£¬»Ø¸´´ËÏîÉèÖÃmodedisplayname£¬Èç¹û²»»Ø¸´Ä¬ÈÏÄ¬ÈÏÓëmodenameÒ»ÖÂ
#define MCU_SPY_BRD 	0x07	//Èç¹ûÐèÒªSpotifyÈÏÖ¤£¬»Ø¸´´ËÏîÉèÖÃbranddisplayname£¬Èç¹û²»»Ø¸´Ä¬ÈÏÄ¬ÈÏÓëbrandnameÒ»ÖÂ
#define MCU_SPY_TYP 	0x08	//Èç¹ûÐèÒªSpotifyÈÏÖ¤£¬»Ø¸´´ËÏîÉèÖÃÀàÐÍ
#define MCU_CAP_001 	0x09	//-MCUÒ»Ð©¿ª¹Ø±äÁ¿ÉèÖÃ£¬ÓÃ16½øÖÆ±íÊ¾£¬Ç°Ãæ²»Òª¼Ó0x
#define MCU_CAP_002 	0x0A	//-MCUÒ»Ð©¿ª¹Ø±äÁ¿ÉèÖÃ£¬ÓÃ16½øÖÆ±íÊ¾£¬Ç°Ãæ²»Òª¼Ó0x
#define MCU_CAP_LAU 	0x0B	//-MCUÒ»ÉèÖÃÄ¬ÈÏÓïÑÔ
#define MCU_CAP_STM 	0x0C	//-MCUÒ»Ð©¿ª¹Ø±äÁ¿ÉèÖÃ£¬ÓÃ16½øÖÆ±íÊ¾£¬Ç°Ãæ²»Òª¼Ó0x
#define MCU_CAP_PLM 	0x0D	//-MCUÒ»Ð©¿ª¹Ø±äÁ¿ÉèÖÃ£¬ÓÃ16½øÖÆ±íÊ¾£¬Ç°Ãæ²»Òª¼Ó0x
#define MCU_CAP_EXT 	0x0E	//Ä¿Ç°Îª0±íÊ¾Ã»ÓÐÍâÖÃµÆÐ§£¬·ñÔò±íÊ¾ÓÐÍâÖÃµÆ¿Ø¡£
#define MCU_PRESETN 	0x0F	//ÉèÖÃÔ¤ÖÃ°´¼üÊýÁ¿£¬×î¶àÎª10¸ö£¬ÎªÁËÏòÇ°¼æÈÝ£¬0±íÊ¾Ä¬ÈÏ6¸öÔ¤ÖÃ£¨²»»Ø¸´Ä¬ÈÏÎª0£©£¬-1±íÊ¾Ã»ÓÐÔ¤ÖÃ¹¦ÄÜ¡£×¢ÒânÇ°ÃæÓÐ¸ö+£¬Õâ¸öÖ¸ÁîÔÚÕý³£¿ª»úºó·¢ËÍÒ²ÓÐÐ§¡£
#define MCU_SET_PWD 	0x10	//ÉèÖÃÉè±¸APÄ£Ê½µÄÄ¬ÈÏÃÜÂë£¬ÃÜÂëÎªÃ÷ÎÄ£¬²»Òª°üº¬ÌØÊâ×Ö·û»òÕßÓëMCUÐ­ÒéµÄ¶¨Òå³åÍ»¡£ÃÜÂë×îÉÙ8×Ö½Ú£¬×î¶à15×Ö½Ú¡£Õâ¸öÖ¸ÁîÔÚÕý³£¿ª»úºó·¢ËÍÎÞÐ§¡£
#define MCU_VMX_XXX 	0x11	//ÓïÒôÌáÊ¾×î¸ßÔÊÐíµÄÒôÁ¿£¬nnnÎª0~100µÄÊý×Ö£¬Èç001, 022, 100£»Ò»µ©µ±Ç°ÒôÁ¿³¬¹ýÔÊÐíÒôÁ¿£¬Èí¼þ»á×ö½µÒô´¦Àí£¬Õâ¸öÖµÉèÖÃ<10»òÕß´óÓÚ90ÎÞÐ§¡£
#define MCU_SID_VAL 	0x12	//ÓÃÓÚÉèÖÃÉè±¸Ä¬ÈÏSSIDÃû³Æ, ÇëÔÚËùÓÐÆäËûÉèÖÃÏîÉèÖÃÍê³Éºó£¬×îºó»Ø¸´Õâ¸öÖ¸ÁîÉèÖÃSSID£¬Õâ¸ö±ØÐë»Ø¸´¡£
#define MCU_ALV_NUM   0x13
#define MCU_PAR_XXX   0x14  //ÉèÖÃMCUÉý¼¶ÎÄ¼þµÄ¸ñÊ½ºÍ·¢ËÍÎÄ¼þ°üµÄ³¤¶È¡£
#define MCU_FMT_XXX   0x15  //MCUÉèÖÃWIFi  IISÊý¾Ý¸ñÊ½¡£

//******************************************************************
//	4 Õý³£¿ª»ú/ÖØÆô/»Ö¸´³ö³§/ÐÝÃß»½ÐÑ/µç³Ø
//******************************************************************
#define MCU_BOT_DON   0x1A  	//Ä£×éÔÚÊÕµ½MCU+BOT+DONÊ±»á»Ø¸´AXX+BOT+DON£¨ÓÃÓÚÐÄÌø¼ì²â£©
#define MCU_GET_UID 	0x1B	//»ñÈ¡µ±Ç°Ä£×é¹Ì¼þ°æ±¾µÄÏîÄ¿uuid£¬Ä£×é»Ø¸´ AXX+UID+uuid&£¬ÏîÄ¿µÄuuidÊÇ8¸ö×Ö½ÚµÄasciiÂë£¬ÒÔFF¿ªÍ·
#define MCU_GET_MAC 	0x1C	//»ñÈ¡µ±Ç°Ä£×éµÄMACµØÖ·£¬Ä£×é»Ø¸´ AXX+MAC+%02X:%02X:%02X:%02X:%02X:%02X&
#define MCU_DEV_RST 	0x1D	//MCUÈÃWiFiÄ£×éÖØÆô
#define MCU_POW_OFF 	0x1E  	//MCUÒ²¿ÉÒÔ·¢ËÍMCU+POW+OFF¸øÄ£×éÈ¥¹Ø±ÕÒ»Ð©½ø³Ì£¬Èç¹ûWiFi»Ø¸´AXX+POW+OFF»òÕß2ÃëÄÚÎÞ»Ø¸´£¬MCU¼´¿É¸øÄ£×é¶Ïµç£»Èç¹ûWiFi»Ø¸´AXX+BURNING£¬²»¿É¹Ø»ú
#define MCU_POW_SAV 	0x1F	//MCU ÈÃWiFiÄ£×é½øÈëÐÝÃß×´Ì¬
#define MCU_POW_RES 	0x20	//MCU ÈÃWiFiÄ£×é»½ÐÑ
#define MCU_POW_STA 	0x21	//»ñÈ¡WiFi Ä£×é×´Ì¬£¬Ä£×é»Ø¸´ AXX+POW+%03d£¬ÆäÖÐÊý×Ö¿ÉÄÜÎª£º000£º Õý³£  001£º Ê¡µçÄ£Ê½(RF¹Ø±Õ)    002£º¹Ì¼þÉý¼¶ÖÐ 003£ºÉè±¸ÖØÆôÖÐ
#define MCU_FACTORY 	0x22	//MCUÒ²¿ÉÒÔ·¢ËÍÖ¸Áî¸øÄ£×é»Ö¸´³ö³§ÉèÖÃ£ºMCU+FACTORY Èç¹ûMCU·¢ËÍMCU+FCRYPOW£¬ÔòÄ£×é»Ö¸´³ö³§ºó²»»áÖØÆô£¬¶øÊÇ·¢ËÍAXX+POW+OFF¸øMCUÇëÇó¶Ïµç¡£
#define MCU_BAT_ON  	0x23	//MCUÍ¨ÖªÄ£×é£¬µç³Ø´¦ÓÚ³äµç×´Ì¬
#define MCU_BAT_OFF 	0x24	//MCUÍ¨ÖªÄ£×é£¬µç³Ø´¦ÓÚ·Åµç×´Ì¬
#define MCU_BAT_VAL 	0x25	//MCUÍ¨ÖªÄ£×éµç³ØµçÁ¿£¬xxxÎª°Ù·Ö±È£¬ÀýÈç001±íÊ¾1%£¬100±íÊ¾100%£»Èç¹ûMCU´ÓÎ´Í¨ÖªÄ£×éµç³ØµçÁ¿£¬ÔòÄ£×éÈÏÎªÃ»ÓÐµç³Ø´æÔÚ¡£
#define MCU_BAT_LOW 	0x26	//MCUÍ¨ÖªÄ£¿éµç³ØµÍµç

//******************************************************************
//	6  MCUÔÚÏßÉý¼¶
//******************************************************************
#define MCU_VER_NUM 	0x30	//MCU·¢ËÍµ±Ç°µÄ°æ±¾ºÅ
#define MCU_UPG_STA 	0x31	//MCUÍ¬ÒâÉý¼¶¿ªÊ¼
#define MCU_UPG_RUN 	0x32	//MCUÉý¼¶³É¹¦¼ÌÐø
#define MCU_UPG_FAL 	0x33	//MCUÉý¼¶Ê§°ÜÖÕÖ¹Éý¼¶
#define MCU_UPG_END 	0x34	//MCUÉý¼¶ÒÑÍê³É

//******************************************************************
//	7 ÍøÂçÃüÁî
//******************************************************************
//WPSÅäÍø
#define MCU_WIF_WPS 	0x38	//°´ÏÂWPS°´¼ü·¢µÄÖ¸Áî£¬MCU¿ØÖÆÄ£¿é¿ªÊ¼WPSÉ¨Ãè£¬É¨ÃèWPS Server£¬£¨´ËÊ±Èç¹ûÓÐÂ·ÓÉÆ÷°´ÏÂWPS°´¼ü£¬»òÕßÁíÒ»¸öÄ£¿éÊÕµ½MCU+WIF+WSSÖ¸Áî£¬Ôò´ËÄ£¿é»áÁ¬½ÓÖ®£©©
#define MCU_WIF_WSS 	0x39	//MCU¿ØÖÆÄ£¿é½øÈëWPS ServerÄ£Ê½,WSSÄ£Ê½ºÍWPSÄ£Ê½ÊÇÁ½¸ö¶ÔÓ¦µÄÄ£Ê½£¬WPSÄ£Ê½µÄÉè±¸»á×Ô¶¯Á¬½ÓWSSÄ£Ê½µÄÉè±¸£¬²»ÐèÒªÊäÈëÃÜÂë
#define MCU_WIF_EPS 	0x3A	//MCU¿ØÖÆÄ£¿éÍË³öWPS/WSSÅäÍøÄ£Ê½

//¿ØÖÆStation
#define MCU_STA_ON  	0x3D 	//Enable WiFi Station£¬Ä¬ÈÏ¿ª»úÊÇenableµÄ 
#define MCU_STA_OFF 	0x3E	//¹Ø±ÕWiFi Station£¬Ä£×é»á¶Ï¿ªÓëÂ·ÓÉÆ÷»òÕßÖ÷ÒôÏäµÄÁ´½Ó
#define MCU_STA_DEL 	0x3F	//¶Ï¿ªµ±Ç°Â·ÓÉÆ÷Á´½Ó²¢É¾³ýÕâ¸öÂ·ÓÉÆ÷¼ÇÂ¼

//¿ØÖÆAP
#define MCU_RA0_ON		0x42	//´ò¿ªAPÈÈµã¹¦ÄÜ
#define MCU_RA0_OFF 	0x43	//Òþ²ØAPÈÈµã¹¦ÄÜ

//ÍøÂç×´Ì¬Í¨Öª
#define MCU_STA_GET 	0x46	//- MCUÍ¨ÖªÄ£¿é·¢ËÍÂ·ÓÉÆ÷Á¬½ÓÇé¿ö
#define MCU_RA0_GET 	0x47	//- MCUÍ¨ÖªÄ£¿é·¢ËÍÉè±¸±»Á¬½ÓÇé¿ö
#define MCU_ETH_GET 	0x48	//- MCUÍ¨ÖªÄ£¿é·¢ËÍÉè±¸ÒÔÌ«ÍøÁ¬½ÓÇé¿ö
#define MCU_WWW_GET 	0x49	//- MCUÍ¨ÖªÄ£¿é·¢ËÍÉè±¸ÉÏÍøÇé¿ö

//******************************************************************
//	8 ²¥·ÅÃüÁî
//******************************************************************
//²¥·Å¿ØÖÆ
#define MCU_PLY_PAU 	0x50	//-	MCU¿ØÖÆÄ£¿é£¬Èç¹ûÔÝÍ£Ôò¿ªÊ¼²¥·Å£¬Èç¹û²¥·ÅÖÐÔòÔÝÍ£²¥·Å
#define MCU_PLY_PLA		0x51	//-	MCU¿ØÖÆÄ£¿é»Ö¸´²¥·Å£¬Èç¹û²»ÊÇÔÝÍ£×´Ì¬ÔòºöÂÔ
#define MCU_PLY_PUS		0x52	//-MCU¿ØÖÆÄ£¿éÔÝÍ£²¥·Å£¬Èç¹û²»ÊÇ²¥·Å×´Ì¬ÔòºöÂÔ
#define MCU_PLY_STP		0x53	//MCU¿ØÖÆÄ£¿éÍ£Ö¹²¥·Å£¬²¢°Ñµ±Ç°²¥·Å¸èµ¥Çå¿Õ
#define MCU_PLY_PUQ 	0x54	//MCU¿ØÖÆÄ£¿é²¥·ÅÉÏ´ÎµÄ¸èµ¥£¬Èç¹ûÊÇ¿ª»ú£¬Ôò²¥·ÅÉÏ´Î¹Ø»úÊ±ÕýÔÚ²¥·ÅµÄ¸èµ¥
#define MCU_PLY_NXT		0x55	//-	MCU¿ØÖÆÄ£¿é²¥·ÅÏÂÒ»Ê×
#define MCU_PLY_PRV		0x56	//-	MCU¿ØÖÆÄ£¿é²¥·ÅÉÏÒ»Ê×

//ÒôÁ¿¿ØÖÆ
#define MCU_VOL_ADD 	0x5A	//-	MCU¿ØÖÆÄ£¿éµ÷½ÚÒôÁ¿£¬xxxÎª0~100£¬Ä£¿é»áÍ¨¹ýAXX+VOL+xxx·µ»ØÊµ¼ÊÒôÁ¿
#define MCU_CUR_VOL 	0x5B	//-	MCUÍ¨ÖªÄ£¿éµ±Ç°ÒôÁ¿£¬xxxÎª0~100£¬×¢Òâ£ºÊÇ-xxx²»ÊÇ+xxx£¬Õâ±íÊ¾Ä£×éÖ»»á¸üÐÂµ±Ç°ÒôÁ¿µÄÖµ£¬²»»áÊµ¼Êµ÷½ÚÒôÁ¿£¬Ò²²»»á»Ø¸´MCU
#define MCU_VOL_GET 	0x5C	//-	MCUÍ¨ÖªÄ£¿é·¢ËÍÒôÁ¿£¬Ä£¿é»á»Ø¸´AXX+VOL+xxx
#define MCU_MUT_SET 	0x5D	//-	MCU¿ØÖÆÄ£¿éÒôÁ¿¾²Òô, xxxÎª0»òÕß1, Èç¹ûMCU×ÔÐÐ¿ØÖÆÒôÁ¿£¬´ËÖ¸ÁîÎÞÐ§
#define MCU_MUT_GET 	0x5E	//-	MCUÍ¨ÖªÄ£¿é·¢ËÍ¾²Òô×´Ì¬£¬Ä£¿é»á»Ø¸´AXX+MUT+xxx
#define MCU_CUR_MUT 	0x5F	//-	Èç¹ûMCU×ÔÐÐ¿ØÖÆÒôÁ¿£¬MCUÍ¨ÖªÄ£¿é¾²Òô×´Ì¬£¬xxxÎª0»òÕß1

//Ô¤ÖÃ¸èµ¥ºÍÇÐ»»
#define MCU_KEY_XNN 	0x63	//MCUÍ¨ÖªÄ£¿éÓÐ°´¼üÊÂ¼þ·¢Éú, nn²ÎÊý´ú±í°´¼üÖµ (10½øÖÆ£¬01¨C99), x±íÊ¾ÏûÏ¢:  2 ³¤°´£» 3°´ÏÂ£»4ËÉ¿ª; ÆäËûÖµÎª¶Ì°´
#define MCU_KEY_NXT 	0x64	// MCUÍ¨ÖªÄ£¿é²¥·ÅÏÂÒ»¸ö°´¼üÔ¤ÖÃ¸èµ¥
#define MCU_KEY_PRE 	0x65	// MCUÍ¨ÖªÄ£¿é²¥·ÅÉÏÒ»¸ö°´¼üÔ¤ÖÃ¸èµ¥
#define MCU_PRE_SET 	0x66	//°Ñµ±Ç°²¥·ÅµÄ¸èµ¥Ô¤ÖÃµ½¿Õ°×°´¼ü£¬Èç¹û¶¼ÒÑ¾­ÓÐÔ¤ÖÃÄÚÈÝ£¬ÔòÒÀ´Î¸²¸Ç¡£
#define MCU_PRE_NNN 	0x67	//°Ñµ±Ç°²¥·ÅµÄ¸èµ¥Ô¤ÖÃµ½Ö¸¶¨°´¼ü£¬nnn¿ÉÒÔ´Ó001µ½009

//²¥·Å×´Ì¬Í¨Öª
#define MCU_PLY_GET		0x6A	//-	MCUÍ¨ÖªÄ£¿é·¢ËÍ²¥·Å×´Ì¬£¬Ä£¿é»á»Ø¸´AXX+PLY+xxx
#define MCU_PLP_GET		0x6B	//- MCUÍ¨ÖªÄ£¿é·¢ËÍµ±Ç°²¥·ÅRepeatÄ£Ê½
#define MCU_PLP_XXX		0x6C	//Ñ­»·Ä£Ê½ÉèÖÃ£ºMCU¿ÉÒÔÍ¨¹ýMCU+PLP+GETÀ´»ñÈ¡µ±Ç°Ä£Ê½(000 loop all 001 single loop 002 shuffle 003 shuffle once Others: no loop)

//Ö¸¶¨²¥·ÅUSB/TF¿¨Ä³Ê×¸èÇú
#define MCU_MSX_NNN 	0x6F	//MCUÍ¨ÖªÄ£¿é²¥·ÅUSB(x=1)»òTF¿¨(x=2)ÉÏµÚnnnÊ×¸èÇú(n´Ó1¿ªÊ¼)

//******************************************************************
//	9  Ê±¼äºÍ¶¨Ê±Æ÷ÃüÁî
//******************************************************************
#define MCU_RTC_GET 	0x75	//²éÑ¯µ±Ç°ÏµÍ³Ê±¼ä
#define MCU_WEK_GET 	0x76	//²éÑ¯µ±Ç°ÏµÍ³ÈÕÆÚ
#define MCU_SND_RTC 	0x77	//ÉèÖÃWiFi µÄRTC
#define MCU_ALM_NXT 	0x78	//»ñÈ¡ÏÂÒ»¸öÄÖÖÓµÄÊ±¼ä.

//******************************************************************
//	10 ¶à·¿¼ä¹¦ÄÜÃüÁî
//******************************************************************
#define MCU_SLV_CHK		0x80	//MCU²éÑ¯µ±Ç°Éè±¸×ÓÒôÏä×´Ì¬
#define MCU_SLV_GET		0x81	//-MCU»ñÈ¡×ÓÒôÏäµÄÊýÁ¿
#define MCU_JNGROUP 	0x82	//-MCUÒ»¼ü×éÍø¹¦ÄÜÄ¿Ç°¶¨ÒåÎª£ºÈç¹ûÉè±¸´¦ÓÚÁªÍø×´Ì¬£¬Ôò°ÑÍ¬Ò»Íø¶ÎÏÈµÄÆäËûÉè±¸À­Îª×ÓÒôÏä¡£Èç¹ûÉè±¸´¦ÓÚ²»ÁªÍø×´Ì¬£¬Ôò°ÑÆäËû²»ÁªÍøµÄÉè±¸À­Îª×ÓÒôÏä
#define MCU_UNGROUP 	0x83	//-MCUÒ»¼üÍËÍø¹¦ÄÜ

//******************************************************************
//	11 AUX/BT/RCA/USB Ä£Ê½ÇÐ»»ÃüÁî
//******************************************************************
#define MCU_PLM_SUB   	0x88  	//- Í¨¹ý°´¼üÇÐ»»Ä£Ê½Ê±£¬MCU·¢ËÍ MCU+PLM-xxx¸øÄ£×é¡£Çë×¢ÒâºóÒ»¸öÖ¸ÁîÊÇ-²»ÊÇ+£¬Ä£×éÖ»»áÐÞ¸ÄÄ£Ê½£¬²»»á×öÈÎºÎÊÂÇé¡£
#define MCU_PLM_ADD   	0x89  	//- Í¨¹ý°´¼üÇÐ»»Ä£Ê½Ê±£¬MCU·¢ËÍ MCU+PLM+xxx¸øÄ£×é¡£Çë×¢ÒâºóÒ»¸öÖ¸ÁîÊÇ+²»ÊÇ-£¬Ä£×é»áÐÞ¸ÄÄ£Ê½£¬Í¬Ê±´ÓI2S data-in×¥Êý¾ÝÍ¬²½×ª·¢µ½¸÷¸ö×ÓÒôÏä²¥·Å
#define MCU_PLM_GET 	0x8A	//- ²éÑ¯µ±Ç°Ä£Ê½
#define MCU_USB_GET 	0x8B	//-²éÑ¯uÅÌÊÇ·ñ´æÔÚ£¬Ä£×é»á»Ø¸´AXX+USB+001»òÕßAXX+USB+000
#define MCU_MMC_GET 	0x8C	//-²éÑ¯T¿¨ÊÇ·ñ´æÔÚ£¬Ä£×é»á»Ø¸´AXX+MMC+001»òÕßAXX+MMC+000

//******************************************************************
//	12  MCUÅäÖÃ
//******************************************************************
//ÅäÖÃÍøÂç
#define MCU_AP_SCAN 	0x90	//MCUÇëÇóÄ£×éÉ¨Ãè²¢·¢ËÍÍøÂçÖÐµÄAPÁÐ±í£¬Ä£×éÒÔAXX+WAN+LST·µ»Ø
#define MCU_AP_CONN 	0x91	//MCUÇëÇóÄ£×éÁ¬½ÓÖ¸¶¨µÄAP£¬Ö¸¶¨apµÄSSIDºÍÃÜÂë£¬SSIDºÍÃÜÂëÖ®¼äÓÃ : ¸ô¿ª¡£
#define MCU_STA_INF 	0x92	//MCU²éÑ¯Ä£×éµ±Ç°µÄÍøÂçÁ¬½ÓÐÅÏ¢£¬Ä£×é·µ»ØAXX+STA+INFessid&£¬ÆäÖÐessidÊÇµ±Ç°Á¬½ÓµÄÂ·ÓÉÆ÷µÄÃû×Ö¡£
#define MCU_WCH_SET 	0x93	//ÉèÖÃ²»ÁªÍøÊ±£¬Ä¬ÈÏWiFiÐÅµÀ
#define MCU_PWD_SET 	0x94	//ÉèÖÃWiFi APÄ£Ê½µÄÄ¬ÈÏÃÜÂë

//ÉèÖÃÃû³Æ
#define MCU_SID_SET		0x99	//ÉèÖÃSSIDÐÅºÅÃû³Æ
#define MCU_NAM_SET 	0x9A	//ÉèÖÃÉè±¸µÄÃû³Æ£¨DLNA£¬AirplayÏÔÊ¾µÄÃû³Æ£©

//******************************************************************
//	13  LCDÏÔÊ¾ÆÁ
//******************************************************************
#define MCU_INF_GET 	0xA0	//»ñÈ¡ÏêÏ¸µÄÉè±¸ÐÅÏ¢£¬¾ßÌå¸ñÊ½¿É²Î¿¼ HttpAPI getStatusExÖ¸Áî¡£ÒÔJSON¸ñÊ½±àÂë£¬±È½Ï³¤£¬ÓÐÊý°Ù¸ö×Ö½Ú¡£
#define MCU_PINF_GET 	0xA1	//»ñÈ¡²¥·ÅµÄÏêÏ¸ÐÅÏ¢£¬¾ßÌå¸ñÊ½¿É²Î¿¼ HttpAPI getPlayerStatusÖ¸Áî¡£ÒÔJSON¸ñÊ½±àÂë£»Èç¹û¸èÇúÃû±È½Ï³¤µÄ»°£¬»áÓÐÊý°Ù¸ö×Ö·û¡£
#define MCU_WEA_GET 	0xA2	//»ñÈ¡ÌìÆøÐÅÏ¢£¬ÉÏÍø×´Ì¬²ÅÓÐÐ§£¨AXX+WWW+001£©£¬¸ñÊ½Îª£º³ÇÊÐ;ÈÕÆÚ;ÐÇÆÚ¼¸;ÌìÆø;×î¸ßÎÂ¶È;×îµÍÎÂ¶È;·ç;PM2.5
#define MCU_DEV_GET 	0xA3	//»ñÈ¡Éè±¸µÄ¼òÒªÐÅÏ¢£¬¸ñÊ½Îª£ºSSID;°æ±¾ÀàÐÍ;Éè±¸Ãû³Æ;Á¬½ÓµÄÂ·ÓÉÆ÷µÄÃû×Ö;ÐÅºÅÇ¿¶È;µç³Ø×´Ì¬;µç³ØµçÁ¿ÆäÖÐ°æ±¾ÀàÐÍÈç¹ûÎªbackup£¬±íÊ¾ÏµÍ³½øÈëÒì³£Ä£Ê½
#define MCU_MEA_GET 	0xA4	//MCUÖ÷¶¯²éÑ¯µ±Ç°µÄ¸èÇúÐÅÏ¢Metadata£¬»Ø¸´ÄÚÈÝÄ¬ÈÏ±àÂëÊÇUTF8¸ñÊ½£¬»Ø¸´¸ñÊ½ºÍAXX+MEA+DATÃüÁîÒ»Ñù
#define MCU_MEA_GBK 	0xA5	//MCUÖ÷¶¯²éÑ¯µ±Ç°µÄ¸èÇúÐÅÏ¢Metadata£¬»Ø¸´ÄÚÈÝ±àÂëÊÇGBK¸ñÊ½£¬»Ø¸´¸ñÊ½ºÍAXX+MEA+DATÃüÁîÒ»Ñù

//******************************************************************
//	14  °¢ÀïÐ¡ÖÇÆ½Ì¨ÌØÊâÃüÁî
//******************************************************************
#define MCU_PLY_LKE		0xAB	//MCUÍ¨ÖªÄ£¿éÊÕ²Ø/Ï²»¶
#define MCU_CHL_NXT 	0xAC	//MCUÍ¨ÖªÄ£¿éÏÂÒ»¸öÆµµÀ
#define MCU_REC_ON  	0xAD	//µ±¶Ì°´Â¼Òô¼üºó£¬MCU·¢ËÍ¸ÃÃüÁî¿ªÊ¼Â¼Òô
#define MCU_REC_OFF 	0xAE	//µ±ÔÙ´Î¶Ì°´Â¼Òô¼üºó£¬MCU·¢ËÍ¸ÃÃüÁî£¬½áÊøÂ¼Òô
#define MCU_AIM_ON  	0xAF	//µ±³¤°´ÈºÁÄ¼ü1Ãëºó£¬MCU·¢ËÍ¸ÃÃüÁî¿ªÊ¼·¢ÆðÈºÁÄÂ¼Òô
#define MCU_AIM_OFF 	0xB0	//µ±ÈºÁÄ¼üµ¯ÆðÊ±£¬MCU·¢ËÍ¸ÃÃüÁî£¬½áÊøÂ¼Òô²¢·¢ËÍ
#define MCU_AIM_PLY 	0xB1	//µ±¶Ì°´°´ÈºÁÄ¼üºó£¬MCU·¢ËÍ¸ÃÃüÁî£¬¸æÖªÄ£¿é²¥·ÅÈºÁÄÏûÏ¢

//******************************************************************
//	15 ²ú²âÃüÁî
//******************************************************************
#define MCU_FCRYPOW 	0xB3	//Èç¹ûMCU·¢ËÍMCU+FCRYPOW£¬ÔòÄ£×é»Ö¸´³ö³§ºó²»»áÖØÆô£¬¶øÊÇ·¢ËÍAXX+POW+OFF¸øMCUÇëÇó¶Ïµç¡£

//******************************************************************
//	16 Í¸´«
//******************************************************************
//´¦ÓÚÍ¬Ò»¸ö¶à·¿¼äµÄMCUÖ®¼ä¿ÉÒÔ´«µÝÖ¸Áî
#define MCU_M2S_NNN 	0xB7	//Éè±¸ÊÇMasterÊ±£¬MCU·¢ËÍMCU+M2S+nnnÖ¸Áî£¬Ä£×é»á°ÑÖ¸Áî×ª·¢¸øËùÓÐµÄSlave
#define MCU_S2M_NNN 	0xB8	//Éè±¸ÊÇSlaveÊ±£¬MCU·¢ËÍMCU+S2M+nnnÖ¸Áî£¬Ä£×é»á°ÑÖ¸Áî×ª·¢¸øMaster

//AppºÍMCUÖ®¼äÒ²¿ÉÒÔÍ¸´«Êý¾Ý
#define MCU_PAS_DATA 	0xBD	//MCU·¢¸øApp, WiFiÄ£×éÊÕµ½º¬ÓÐMCU+PAS+µÄMCUÖ¸ÁîÊ±£¬»á¼ÌÐø½ÓÊÕÊý¾ÝÖ±µ½·¢ÏÖ & ·ûºÅ£¬È»ºó·¢ËÍ MCU+PAS+data&Ö¸Áî¸øApp

//******************************************************************
//	18 ÓïÒôÌáÊ¾ÃüÁî
//******************************************************************
#define MCU_LAU_XXX 	0xCA	//-MCU»Ø¸´µ±Ç°ÓïÑÔ
#define MCU_LAU_GET 	0xCD	//-MCU²éÑ¯µ±Ç°ÓïÑÔ
#define MCU_PMT_GET 	0xCE	//-MCU²éÑ¯ÓïÒôÌáÊ¾×´Ì¬

//******************************************************************
//	19 I2S bypass
//******************************************************************
#define MCU_BYP_000 	0xD3 	//¹Ø±Õi2s bypass mode
#define MCU_BYP_001 	0xD4	//´ò¿ª i2s bypass mode
#define MCU_BYP_GET 	0xD5	//²éÑ¯I2S bypass Ä£Ê½

//******************************************************************
//	20 ÓïÒôÊ¶±ðÃüÁî
//******************************************************************
#define MCU_TALK_ON  	0xDA	//- MCU·¢ËÍÆô¶¯ÓïÒôÊ¶±ðÃüÁî
#define MCU_TLK_OFF 	0xDB	//- µ±ÓïÒôÊ¶±ð¼üµ¯ÆðÊ±£¬MCU·¢ËÍ¸ÃÃüÁî£¬½áÊøÓïÒôÊ¶±ð
#define MCU_TLK_TIM 	0xDC	//-MCUÍ¨ÖªÄ£¿éÓïÒô²¥±¨Ê±¼ä£¬Ä¬ÈÏ²»Ö§³Ö
#define MCU_WEATHER 	0xDD	//-MCUÍ¨ÖªÄ£¿éÓïÒô²¥±¨ÌìÆø£¬Ä¬ÈÏ²»Ö§³Ö
#define MCU_TLK_ENA 	0xDE	//-MCUÍ¨ÖªÄ£¿éÓïÒô²¥±¨ÌìÆø£¬Ä¬ÈÏ²»Ö§³Ö
#define MCU_TLK_DIS 	0xDF	//-MCUÍ¨ÖªÄ£¿éÓïÒô²¥±¨ÌìÆø£¬Ä¬ÈÏ²»Ö§³Ö

//******************************************************************
//	21 EQÉèÖÃ
//******************************************************************
#define MCU_PAS_EQ_TREBLE	0xE2	//»Ø¸´EQ TrebleÖµ
#define MCU_PAS_EQ_BASS 	0xE3	//»Ø¸´EQ BassÖµ


#ifdef __cplusplus
}
#endif//__cplusplus

#endif

