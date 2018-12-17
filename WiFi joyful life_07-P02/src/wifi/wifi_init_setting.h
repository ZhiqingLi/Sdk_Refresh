#ifndef _WIFI_INIT_SETTING_H_
#define _WIFI_INIT_SETTING_H_

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "app_config.h"

//****************************************************************************************
//					WiFi²úÆ·µÄÏà¹Ø¶¨ÖÆ²ÎÊýÉèÖÃ
//****************************************************************************************
//**MCU+CAP+PRJproject_name&:ÏîÄ¿Ãû³Æ¶¨Òå
#define MCU_CAP_PRJ_PROJECT_NAME	"WiFi joyful life"

//**MCU+SID+name&:ÉèÖÃÉè±¸Ä¬ÈÏSSIDÃû³Æ
#define MCU_SSID_NAME	"WiFi joyful life"

//**MCU+PTV+nnn:Ê¹ÓÃµÄMCU Ð­ÒéµÄ°æ±¾ºÅÉèÖÃ
#define MCU_PTV_NUM		002		//nnnÎª0~999µÄÊý×Ö£¬±íÊ¾Ê¹ÓÃµÄMCU Ð­ÒéµÄ°æ±¾ºÅ£¬Ä¬ÈÏ000£»´ËÏîÉè¶¨ºÍcapacity µÄÎ»¶¨ÒåÓÐÒÀÀµ¹ØÏµ?
								//ÀýÈçMCU+CAP+001µÄµÚ8Î»±íÊ¾Ä£×éµÄI2S master/slave Ä£Ê½£¬ÔÚMCUÐ­Òé001°æ±¾¼ÓÈëÖ§³Ö£¬ËùÒÔÒªÊ¹ÄÜÅäÖÃcap1 bit8¾Í±ØÐëÒªÉè¶¨ÊÇnnn >=001²ÅÖ§³Ö¡£Èç¹û²»·¢¾ÍÈÏÎªÊÇ000ÁË¡£

#ifdef FUNC_WIFI_ALI_PROJECT_EN
	//**MCU+ALI+PIDproductid&: °¢ÀïÐ¡ÖÇµÄÏîÄ¿ÉèÖÃ
	#define MCU_ALI_PID_PRODUCTID	"ESS_ENTERTAINMENT_ATALK_JOYFUL_LIFE_7_1_PREAMP"	//Èç¹ûÕâÊÇÒ»¸ö°¢ÀïÐ¡ÖÇµÄÏîÄ¿£¬Çë»Ø¸´´ËÏî£¬productidÀ´×Ô°¢ÀïÌá¹©µÄprodconf.jsonÎÄ¼þÀïÃæmodel×Ö¶Î£¬ÀýÈç£ºEDIFIER_ENTERTAINMENT_ATALK_MA1

	//**MCU+ALV+nnn:°¢ÀïÐ¡ÖÇµÄÏîÄ¿ Ð­ÒéµÄ°æ±¾ºÅÉèÖÃ
	#define MCU_ALV_NUMBER 		001		//nnnÎª0~999µÄÊý×Ö£¬±íÊ¾Ê¹ÓÃµÄ°¢ÀïÏîÄ¿Ð­Òé°æ±¾ºÅ¡£
#endif

//**MCU+SPY+NAMmodelname&:SpotifyÈÏÖ¤£¬ÉèÖÃmodename
#define MCU_SPY_NAM_MODE_NAME	"MV_WiFiDemo" //Èç¹ûÐèÒªSpotifyÈÏÖ¤£¬»Ø¸´´ËÏîÉèÖÃmodename£¬Èç¹û²»»Ø¸´Ä¬ÈÏÓëproject_nameÒ»ÖÂ

//**MCU+SPY+BRNbrandname&:SpotifyÈÏÖ¤£¬ÉèÖÃbrandname
#define	MCU_SPY_BRN_BRAND_NAME	"MV_WiFiDemo" //Èç¹ûÐèÒªSpotifyÈÏÖ¤£¬»Ø¸´´ËÏîÉèÖÃbrandname£¬Èç¹û²»»Ø¸´Ä¬ÈÏÄ¬ÈÏÓëproject_nameÒ»ÖÂ

//**MCU+SPY+MODmodedisplaylname&:SpotifyÈÏÖ¤£¬ÉèÖÃmodedisplayname
#define MCU_SPY_MOD_DISP_NAME	"MV_WiFiDemo" //Èç¹ûÐèÒªSpotifyÈÏÖ¤£¬»Ø¸´´ËÏîÉèÖÃmodedisplayname£¬Èç¹û²»»Ø¸´Ä¬ÈÏÄ¬ÈÏÓëmodenameÒ»ÖÂ

//**MCU+SPY+BRDbarnddisplayname&:SpotifyÈÏÖ¤£¬ÉèÖÃbranddisplayname
#define MCU_SPY_BRD_DISP_NAME	"MV_WiFiDemo" //Èç¹ûÐèÒªSpotifyÈÏÖ¤£¬»Ø¸´´ËÏîÉèÖÃbranddisplayname£¬Èç¹û²»»Ø¸´Ä¬ÈÏÄ¬ÈÏÓëbrandnameÒ»ÖÂ

//**MCU+SPY+TYPdevicetype&:SpotifyÈÏÖ¤£¬ÉèÖÃÀàÐÍ
#define	MCU_SPY_TYP_DEVICE_TYPE		1	//Èç¹ûÐèÒªSpotifyÈÏÖ¤£¬»Ø¸´´ËÏîÉèÖÃÀàÐÍ£¬ÒôÏäÎª1£¬ºÐ×ÓÎª0£»Ä¬ÈÏÎª1

//**MCU+CAP+001xxxxxxxx&:¹¦ÄÜ¿ª¹Ø±äÁ¿ÉèÖÃ
#define MCU_CAP_001_BIT0	0	//±£Áô
#define MCU_CAP_001_BIT1	0	//ÊÇ·ñ½ûÖ¹¿ª»úÒôÀÖ£¬Ä¬ÈÏ0²»½ûÖ¹
#define MCU_CAP_001_BIT2	0	//¿ª»úÒôÀÖÊÇ·ñÊ¹ÓÃuserÇøµÄÒôÆµ£¬Ä¬ÈÏ0£»½«Í³Ò»ÎªÓÐUserÇø¾ÍÊ¹ÓÃuserÇø£¬·ñÔò²¥·ÅÄ¬ÈÏÓïÒô
#define MCU_CAP_001_BIT3	0	//ÊÇ·ñ½ûÖ¹½ûÖ¹ÓïÒôÌáÊ¾£¬Ä¬ÈÏ0²»½ûÖ¹
#define MCU_CAP_001_BIT4	0	//±£Áô
#define MCU_CAP_001_BIT5	0	//ÓïÒôÌáÊ¾ÊÇ·ñÊ¹ÓÃuserÇøµÄÒôÆµ£¬Ä¬ÈÏ0£»½«Í³Ò»ÎªÓÐUserÇø¾ÍÊ¹ÓÃuserÇø£¬·ñÔò²¥·ÅÄ¬ÈÏÓïÒô¡£
#define MCU_CAP_001_BIT6	0	//²åÈëUÅÌ/T¿¨ÊÇ·ñUDisk/T¿¨µÄÒôÆµ£¬Ä¬ÈÏ0²»×Ô¶¯
#define MCU_CAP_001_BIT7	0	//¿ª»ú×Ô¶¯²¥·ÅUÅÌ/T¿¨ÊÇ·ñ×Ô¶¯²¥·Å£¬Ä¬ÈÏ0²»×Ô¶¯
#define MCU_CAP_001_BIT8	0	//Ä£×éI2S Ä£Ê½£¬Ä¬ÈÏ0 ´ÓÄ£Ê½£¬1 Ö÷Ä£Ê½£¬ÒªÊ¹ÄÜÕâÒ»Î»Éè¶¨£¬±ØÐëÒªÍ¬Ê±Éè¶¨MCU+PTV+nnn£ºnnn>=001 ²ÅÉúÐ§£»Ö»ÓÐA31/A28Ö§³ÖÕâ¸ö¹¦ÄÜ£»²¢ÇÒ¹Ì¼þÐèÒªÊÇ±ê×¼¹Ì¼þ¡£
#define MCU_CAP_001_BIT9	0	//ÒôÁ¿ÊÇ·ñÓÉMCU¿ØÖÆ£¬Ä¬ÈÏÓÐMCUµÄÎª1£¬·ñÔòÎª0,  Èç¹ûÒôÁ¿ÓÉMCU¿ØÖÆ£¬¿ª»úºóWiFi²»»á·¢ËÍÒôÁ¿¸øMCU£¬Ö»»á¶ÁÈ¡, ·ñÔò£¬¿ª»úÊ±WIFi»á·¢ËÍÒôÁ¿¸øMCU
#define MCU_CAP_001_BIT10	0	//ÒôÁ¿ÊÇ·ñ×Ô¶¯±£´æµ½WiFi¶Ë£¬Ä¬ÈÏÓÐMCUµÄÎª0£¬·ñÔòÎª1
#define MCU_CAP_001_BIT11	0	//¸èÇúÐÅÏ¢medadataÊÇ·ñÄ¬ÈÏÊÇGBK±àÂë£¬Ä¬ÈÏÎª0 UTF8±àÂë£¬·ñÔòÎª1 GBK±àÂë
#define MCU_CAP_001_BIT12	1	//Èç¹ûMCUÇÐ»»Ä£Ê½Ê±²»ÏëWiFi·µ»ØAXX+PLMµÄÖ¸Áî£¬Çë°ÑÕâ1Î»ÉèÖÃÎ»1£¬Ä¬ÈÏÎª0
#define MCU_CAP_001_BIT13	0	//±£Áô
#define MCU_CAP_001_BIT14	0	//EQÊÇ·ñÓÉMCU¿ØÖÆ£¬Ä¬ÈÏÓÐMCUµÄÎª1£¬·ñÔòÎª0
#define MCU_CAP_001_BIT15	0	//EQÊÇ·ñ×Ô¶¯±£´æ£¬Ä¬ÈÏÓÐMCUµÄÎª0£¬·ñÔòÎª1
#define MCU_CAP_001_BIT16	0	//MCUÊÇ·ñÓÐRTC£¬Ä¬ÈÏÎª0±íÊ¾Ã»ÓÐ
#define MCU_CAP_001_BIT17	0	//±£Áô
#define MCU_CAP_001_BIT18	0	//±£Áô
#define MCU_CAP_001_BIT19	0	//±£Áô
#define MCU_CAP_001_BIT20	1	//¾²ÒôÊÇ·ñÓÉMCU¿ØÖÆ£¬Ä¬ÈÏÓÐMCUµÄÎª1£¬·ñÔòÎª0
#define MCU_CAP_001_BIT21	0	//ÉùµÀÊÇ·ñ×Ô¶¯±£´æ£¬·ñÔòÎª0
#define MCU_CAP_001_BIT22	0	//±£Áô
#define MCU_CAP_001_BIT23	1	//WiFiÄ£¿éÊÇ·ñÓÐ½ÓUSBÂó¿Ë·ç£¬Ä¬ÈÏÎªÃ»ÓÐ½Ó0£¬·ñÔòÎª1£¬ÒªÊ¹ÄÜÕâÒ»Î»Éè¶¨£¬±ØÐëÒªÍ¬Ê±Éè¶¨MCU+PTV+nnn£ºnnn>=002 ²ÅÉúÐ§£»Ö»ÓÐA31/A28Ö§³ÖÕâ¸ö¹¦ÄÜ£»²¢ÇÒ¹Ì¼þÐèÒªÊÇ±ê×¼¹Ì¼þ¡£
#define MCU_CAP_001_BIT24	0	//WiFiÄ£¿éÊÇ·ñÓÐ½ÓUSBÎÞÏßÒ£¿ØÆ÷£¬Ä¬ÈÏÎªÃ»ÓÐ½Ó0£¬·ñÔòÎª1£¬ÒªÊ¹ÄÜÕâÒ»Î»Éè¶¨£¬±ØÐëÒªÍ¬Ê±Éè¶¨MCU+PTV+nnn£ºnnn>=002 ²ÅÉúÐ§£»Ö»ÓÐA31/A28Ö§³ÖÕâ¸ö¹¦ÄÜ£»²¢ÇÒ¹Ì¼þÐèÒªÊÇ±ê×¼¹Ì¼þ¡£
#define MCU_CAP_001_BIT25	0	//WiFiÄ£¿éÊÇ·ñ´ÓMCU»ñÈ¡Ê±¼ä£¬Ä¬ÈÏÎª0£»0£ºWiFiÄ£¿éÍ¨¹ýSNTP»ñÈ¡Ê±¼ä£¬1£ºWiFiÄ£¿é´ÓMCU»òÕßAPP»ñÈ¡Ê±¼ä
#define MCU_CAP_001_BIT26	0	//ÊÇ·ñMONOÊä³ö£¬Ä¬ÈÏ0£¬Ë«ÉùµÀ
#define MCU_CAP_001_BIT27	0	//×ÓÒôÏì°´ungroup°´¼üÖ»ÊÇ´Ó×éÀïÃæ½â³ý¸ÃÖ÷ÒôÏä£¬Ä¬ÈÏÎª0£»0£º½âÉ¢Õû¸ö×é£¬1£ºÖ»ÊÇ½â³ý¸Ã×ÓÒôÏì£¡£
#define MCU_CAP_001_BIT28	0	//°´group¼ü²»½âÉ¢ÆäËû×éÒôÏä£¬Ä¬ÈÏ0£¬0£º½âÉ¢¸ÃÂ·ÓÉÆ÷ÏÂµÄÆäËû×é£¬1£º²»½âÉ¢ÆäËû×é
#define MCU_CAP_001_BIT29	0	//±£Áô
#define MCU_CAP_001_BIT30	0	//±£Áô
#define MCU_CAP_001_BIT31	0	//±£Áô

//**MCU+CAP+002xxxxxxxx&:¹¦ÄÜ¿ª¹Ø±äÁ¿ÉèÖÃ
#define MCU_CAP_002_BIT0	0	//±£Áô
#define MCU_CAP_002_BIT1	0	//±£Áô
#define MCU_CAP_002_BIT2	0	//±£Áô
#define MCU_CAP_002_BIT3	0	//±£Áô
#define MCU_CAP_002_BIT4	0	//±£Áô
#define MCU_CAP_002_BIT5	0	//±£Áô
#define MCU_CAP_002_BIT6	0	//±£Áô
#define MCU_CAP_002_BIT7	0	//¶à·¿¼äÊÇ·ñÓÅÏÈÂ·ÓÉÆ÷×ª·¢£¬£¨±¸×¢£ºµ±Ö÷ÒôÏäºÍ×ÓÒôÏìÔÚÍ¬Ò»¾ÖÓòÍøÊ±£¬²»ÐèÒªÖ±Á¬£©£¬Ä¬ÈÏ0²»Ö§³Ö
#define MCU_CAP_002_BIT8	0	//¶à·¿¼äÊÇ·ñ¸ù¾ÝÂ·ÓÉÆ÷ºÍÖ÷ÒôÏäµÄÐÅºÅ×Ô¶¯ÅÐ¶ÏÊÇ·ñÐèÒªÖ±Á¬£¨¼´×Ô¶¯ÅÐ¶Ï×ÓÒôÏäÊÇ·ñÐèÒªÁ¬½ÓÖ÷ÒôÏä£©£¬Ä¬ÈÏ0²»Ö§³Ö£¬Ä¿Ç°²»½¨Òé´ò¿ª
#define MCU_CAP_002_BIT9	0	//ÊÇ·ñÖ§³Ö¿ì½ÝÁ¬½ÓÂ·ÓÉÆ÷(¼´Í¨¹ý»·¾³ÖÐÆäËûÉè±¸»ñÈ¡Â·ÓÉÆ÷Ãû×ÖºÍÃÜÂë£¬µ±ÓÃ»§ÓÐÁ½¸öÒôÏä£¬ÆäÖÐÒ»¸öÒôÏäÒÑ¾­Á¬½ÓÂ·ÓÉÆ÷£¬ÁíÒ»¸öÒôÏä¿ÉÒÔ²»ÐèÒªapp¾ÍÄÜÁ¬½ÓÂ·ÓÉÆ÷ÁË¡£)£¬Ä¬ÈÏÎª0²»Ö§³Ö
#define MCU_CAP_002_BIT10	0	//ÊÇ·ñÖ§³Ö×Ô¶¯ÃÜÂë£¨µ±Á¬ÉÏÂ·ÓÉÆ÷Ê±£¬Éè±¸µÄÃÜÂëÊÇÂ·ÓÉÆ÷µÄÃÜÂë£¬Ö±µ½ÓÃ»§ÊÖ¶¯ÉèÖÃ¹Ì¶¨ÃÜÂë£©£¬Ä¬ÈÏÎª0²»Ö§³Ö
#define MCU_CAP_002_BIT11	1	//Á¬½ÓÂ·ÓÉÆ÷ºó£¬ÊÇ·ñ×Ô¶¯Òþ²ØAP£¬Ä¬ÈÏÎª0£¬²»Òþ²Ø
#define MCU_CAP_002_BIT12	0	//USBÊÇ½ÓÔÚMCUÉÏ»¹ÊÇWiFiÉÏ£¬Ä¬ÈÏÎª0±íÊ¾ÔÚWiFiÉÏ»òÕßÃ»ÓÐUSB
#if (defined(FUNC_WIFI_PLAY_CARD_EN) || defined(FUNC_WIFI_PLAY_USB_EN))
#define MCU_CAP_002_BIT13	1	//ÊÇ·ñÓÐT¿¨Ä£Ê½£¬Ä¬ÈÏÎª0£¬±íÊ¾·ñ£¨ËùÎ½T¿¨Ä£Ê½£¬ÊÇ±íÊ¾Ä£Ê½ÇÐ»»Ðè²»ÐèÒªÌØ±ðÓÐÒ»¸öT¿¨Ä£Ê½£»Êµ¼ÊÉÏApp¿ÉÒÔÖ±½ÓÑ¡ÔñT¿¨¸èÇú£¬ËùÒÔ´ó²¿·ÖÇé¿öÏÂ²»ÐèÒªÑ¡Ôñ£©
#else
#define MCU_CAP_002_BIT13	1
#endif
#define MCU_CAP_002_BIT14	1	//ÊÇ·ñÓÐÓïÒôÊ¶±ð¹¦ÄÜ£¬Ä¬ÈÏ0ÎÞ£¬ÉèÖÃÎª1±íÊ¾ÓÐ
#define MCU_CAP_002_BIT15	0	//ÊÇ·ñ¹Ø±Õintercom¹¦ÄÜ£¬Ä¬ÈÏ0²»¹Ø±Õ£»ÉèÖÃÎª1¹Ø±Õ
#ifdef FUNC_WIFI_I2S_BYPASS_NEED_EN
#define MCU_CAP_002_BIT16	1
#else
#define MCU_CAP_002_BIT16	0	//ÊÇ·ñ´ò¿ªI2S Bypass Auto¹¦ÄÜ£¬Èç¹û´ò¿ª£¬Ôò´¦ÓÚAux-In, BTµÈÍâ²¿ÊäÈëÄ£Ê½Ê±£¬Èç¹ûÃ»ÓÐ×ÓÒôÏä£¬Ôò´ò¿ªbypass£¬I2S inµÄÊý¾ÝÖ±½ÓI2s out£»Èç¹ûÓÐ×ÓÒôÏä£¬Ôò¹Ø±Õbypass£¬WiFi°ÑI2S inµÄÊý¾Ý×ª·¢¸ø×ÓÒôÏä£¨»áÔö¼Ó1.5ÃëµÄ»º³å£©¡£
#endif
#define MCU_CAP_002_BIT17	0	//ÊÇ·ñ¹Ø±Õmultiroom¶àÒôÏä¹¦ÄÜ£¬Ä¬ÈÏ0²»¹Ø±Õ
#define MCU_CAP_002_BIT18	0	//MCUÊÇ·ñÖ§³ÖÍ¬Ê±²¥·ÅºÍÂ¼Òô£¬²»Ö§³ÖÎª1£¬Ä¬ÈÏ0Ö§³Ö
#define MCU_CAP_002_BIT19	1	//ÊÇ·ñÓÐÍâ²¿ÒôÔ´ÊäÈë£¨AUX/BT...) £¬Èç¹ûÓÐ£¬»¹ÐèÒª»Ø¸´MCU+CAP+PLM
#define MCU_CAP_002_BIT20	0	//±£Áô
#ifdef FUNC_WIFI_SUPPORT_RTC_EN
#define MCU_CAP_002_BIT21	1
#else
#define MCU_CAP_002_BIT21	0	//ÓÐÎÞÄÖÖÓÖ§³Ö£¬Ä¬ÈÏÎª0£¬²»Ö§³Ö
#endif
#define MCU_CAP_002_BIT22	0	//ÓÐÎÞ¶¨Ê±Í£Ö¹²¥·Å¹¦ÄÜ£¬Ä¬ÈÏÎª0£¬Ã»ÓÐ
#define MCU_CAP_002_BIT23	0	//ÓÐÎÞÍâ²¿¶¨Ê±¹Ø»ú¹¦ÄÜ£¬Ä¬ÈÏÎª0 £¨Èç¹ûÖ§³Ö£¬WiFi»áÔÚ¶¨Ê±µã·¢ËÍAXX+POW+OFF¸øMCU£¬MCU¿É»Ø¸´MCU+POW+OFF¸øWiFiÈ·ÈÏ£¬Èç¹ûWiFiÈÔÈ»»Ø¸´AXX+POW+OFF»òÕß2ÃëÄÚÎÞ»Ø¸´£¬¼´¿É¹Ø»ú£»Èç¹ûWiFi»Ø¸´AXX+BURNING£¬²»¿É¹Ø»ú£©
#define MCU_CAP_002_BIT24	0	//±£Áô
#define MCU_CAP_002_BIT25	0	//±£Áô
#define MCU_CAP_002_BIT26	0	//±£Áô
#define MCU_CAP_002_BIT27	1	//MCUÊÇ·ñÖ§³ÖÔÚÏßÉý¼¶Ð­Òé£¬Ä¬ÈÏÎª0£¬²»Ö§³Ö
#define MCU_CAP_002_BIT28	0	//ÊÇ·ñ½ûÓÃÒÔÌ«Íø£¬Ä¬ÈÏÎª0²»½ûÓÃ£¬Èç¹û½ûÓÃ£¬Ôò¼´Ê¹²åÈëÍøÏßÒ²²»ÄÜÓÃ
#define MCU_CAP_002_BIT29	1	//ÊÇ·ñ½ûÖ¹WIFIÄ£Ê½×Ô¶¯²¥·Å¹¦ÄÜ£¬Ä¬ÈÏÎª0¿ªÆôWIFIÄ£Ê½×Ô¶¯²¥·Å£¨×Ô¶¯²¥·ÅÉÏ´Î²¥·ÅµÄ¸èµ¥£©
#define MCU_CAP_002_BIT30	0	//±£Áô
#define MCU_CAP_002_BIT31	0	//±£Áô

//**MCU+CAP+LAUlanguage&:ÓïÒô²¥±¨ÓïÑÔÉèÖÃ
#if defined(FUNC_WIFI_ALI_PROJECT_EN)
	#define MCU_CAP_LAU_ENGLISH		0	//Ó¢ÎÄ
	#define MCU_CAP_LAU_CHINESE		1 //ÖÐÎÄ
	#define MCU_CAP_LAU_FRENCH		0	//·¨ÎÄ
	#define MCU_CAP_LAU_PORTUGUESE	0	//ÆÏÌÑÑÀ
	#define MCU_CAP_LAU_ITALIAN		0	//Òâ´óÀû
	#define MCU_CAP_LAU_GERMANY		0	//µÂÎÄ
	#define MCU_CAP_LAU_SPANISH		0	//Î÷°àÑÀ
	
#elif defined(FUNC_WIFI_ALEXA_PROJECT_EN)
	#define MCU_CAP_LAU_ENGLISH		1	//Ó¢ÎÄ
	#define MCU_CAP_LAU_CHINESE		0 //ÖÐÎÄ
	#define MCU_CAP_LAU_FRENCH		0	//·¨ÎÄ
	#define MCU_CAP_LAU_PORTUGUESE	0	//ÆÏÌÑÑÀ
	#define MCU_CAP_LAU_ITALIAN		0	//Òâ´óÀû
	#define MCU_CAP_LAU_GERMANY		0	//µÂÎÄ
	#define MCU_CAP_LAU_SPANISH		0	//Î÷°àÑÀ
#endif

//**MCU+CAP+STMxxxxxxxx&:ÒôÔ´¿ª¹Ø±äÁ¿ÉèÖÃ
#if defined(FUNC_WIFI_ALI_PROJECT_EN)
	#define MCU_CAP_STM_BIT0	0	//ÓÐÎÞMFI airplay£¬Ä¬ÈÏ0
	#define MCU_CAP_STM_BIT1	1	//ÓÐÎÞAirplay£¬Ä¬ÈÏ1£¬²»¿ÉÓëÎ»1Í¬Ê±Îª1
	#define MCU_CAP_STM_BIT2	1	//ÓÐÎÞDLNA£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT3	1	//ÓÐÎÞQPlay£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT4	0	//±£Áô
	#define MCU_CAP_STM_BIT5	1	//¾ÖÓòÍøDMSÖ§³Ö£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT6	0	//±£Áô
	#define MCU_CAP_STM_BIT7	0	//±£Áô
	#define MCU_CAP_STM_BIT8	0	//±£Áô
	#define MCU_CAP_STM_BIT9	0	//±£Áô
	#define MCU_CAP_STM_BIT10	1	//ÌìÌì¶¯Ìý£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT11	1	//¶¹°êFM£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT12	0	//±£Áô
	#define MCU_CAP_STM_BIT13	0	//±£Áô
	#define MCU_CAP_STM_BIT14	1	//òßòÑ£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT15	1	//Ï²ÂíÀ­ÑÅ£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT16	0	//TuneIn£¨ÍøÂçµçÌ¨£©£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT17	0	//iHeartRadio£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT18	0	//Tidal£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT19	0	//±£Áô
	#define MCU_CAP_STM_BIT20	0	//±£Áô
	#define MCU_CAP_STM_BIT21	0	//Pandora£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT22	0	//Spotify£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT23	0	//RHAPSODY,Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT24	0	// QQFM
	#define MCU_CAP_STM_BIT25	0	// VTUNER
	#define MCU_CAP_STM_BIT26	0	//ALEXA
	#define MCU_CAP_STM_BIT27	0	//±£Áô
	#define MCU_CAP_STM_BIT28	0	//±£Áô
	#define MCU_CAP_STM_BIT29	0	//±£Áô
	#define MCU_CAP_STM_BIT30	0	//±£Áô
	#define MCU_CAP_STM_BIT31	0	//±£Áô

#elif defined(FUNC_WIFI_ALEXA_PROJECT_EN)
	#define MCU_CAP_STM_BIT0	0	//ÓÐÎÞMFI airplay£¬Ä¬ÈÏ0
	#define MCU_CAP_STM_BIT1	1	//ÓÐÎÞAirplay£¬Ä¬ÈÏ1£¬²»¿ÉÓëÎ»1Í¬Ê±Îª1
	#define MCU_CAP_STM_BIT2	1	//ÓÐÎÞDLNA£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT3	1	//ÓÐÎÞQPlay£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT4	0	//±£Áô
	#define MCU_CAP_STM_BIT5	1	//¾ÖÓòÍøDMSÖ§³Ö£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT6	0	//±£Áô
	#define MCU_CAP_STM_BIT7	0	//±£Áô
	#define MCU_CAP_STM_BIT8	0	//±£Áô
	#define MCU_CAP_STM_BIT9	0	//±£Áô
	#define MCU_CAP_STM_BIT10	0	//ÌìÌì¶¯Ìý£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT11	0	//¶¹°êFM£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT12	0	//±£Áô
	#define MCU_CAP_STM_BIT13	0	//±£Áô
	#define MCU_CAP_STM_BIT14	0	//òßòÑ£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT15	0	//Ï²ÂíÀ­ÑÅ£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT16	1	//TuneIn£¨ÍøÂçµçÌ¨£©£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT17	1	//iHeartRadio£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT18	1	//Tidal£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT19	0	//±£Áô
	#define MCU_CAP_STM_BIT20	0	//±£Áô
	#define MCU_CAP_STM_BIT21	1	//Pandora£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT22	1	//Spotify£¬Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT23	1	//RHAPSODY,Ä¬ÈÏ1
	#define MCU_CAP_STM_BIT24	0	// QQFM
	#define MCU_CAP_STM_BIT25	0	// VTUNER
	#define MCU_CAP_STM_BIT26	1	//ALEXA
	#define MCU_CAP_STM_BIT27	0	//±£Áô
	#define MCU_CAP_STM_BIT28	0	//±£Áô
	#define MCU_CAP_STM_BIT29	0	//±£Áô
	#define MCU_CAP_STM_BIT30	0	//±£Áô
	#define MCU_CAP_STM_BIT31	0	//±£Áô
#endif

//**MCU+CAP+PLMxxxxxxxx&:Ä£Ê½¿ª¹Ø±äÁ¿ÉèÖÃ
#define MCU_CAP_PLM_BIT0	0	//±£Áô
#define MCU_CAP_PLM_BIT1	1	//ÓÐÎÞAux-In£¬Ä¬ÈÏ0£¨Èç¹ûApp²»ÐèÒªÏÔÊ¾µ±Ç°´¦ÓÚAux-In×´Ì¬£¬ÀýÈçÇÐ»»µ½Aux×´Ì¬WiFiÒÑ¾­¹Ø±Õ£¬Ôò´Ë´¦²»ÐèÒª»Ø¸´1£©
#define MCU_CAP_PLM_BIT2	0	//ÓÐÎÞÀ¶ÑÀ£¬Ä¬ÈÏ0£¨Èç¹ûApp²»ÐèÒªÏÔÊ¾µ±Ç°´¦ÓÚÀ¶ÑÀ×´Ì¬£¬ÀýÈçÇÐ»»µ½À¶ÑÀ×´Ì¬WiFiÒÑ¾­¹Ø±Õ£¬Ôò´Ë´¦²»ÐèÒª»Ø¸´1£©
#define MCU_CAP_PLM_BIT3	0	//ÓÐÎÞÍâ²¿²å¿¨ÊäÈë£¬Ä¬ÈÏ0£¨¼´Íâ²¿²å¿¨½ÓÔÚÆäËûÐ¾Æ¬ÉÏ²»ÊÇ½ÓÔÚWiFiÄ£×éÉÏ£©
#define MCU_CAP_PLM_BIT4	0	//ÓÐÎÞ¹âÏËÊäÈë£¬Ä¬ÈÏ0
#define MCU_CAP_PLM_BIT5	0	//ÓÐÎÞRCAÊäÈë£¬APPÉÐÎ´Ö§³Ö
#define MCU_CAP_PLM_BIT6	0	//ÓÐÎÞÍ¬ÖáÊäÈë£¬APPÉÐÎ´Ö§³Ö
#define MCU_CAP_PLM_BIT7	0	//ÓÐÎÞFMÊäÈë
#define MCU_CAP_PLM_BIT8	0	//ÓÐÎÞµÚ¶þ¸öAUX IN
#define MCU_CAP_PLM_BIT9	0	//±£Áô
#define MCU_CAP_PLM_BIT10	0	//±£Áô
#define MCU_CAP_PLM_BIT11	0	//±£Áô
#define MCU_CAP_PLM_BIT12	0	//±£Áô
#define MCU_CAP_PLM_BIT13	0	//±£Áô
#define MCU_CAP_PLM_BIT14	0	//±£Áô
#define MCU_CAP_PLM_BIT15	0	//±£Áô
#define MCU_CAP_PLM_BIT16	0	//±£Áô
#define MCU_CAP_PLM_BIT17	0	//±£Áô
#define MCU_CAP_PLM_BIT18	0	//±£Áô
#define MCU_CAP_PLM_BIT19	0	//±£Áô
#define MCU_CAP_PLM_BIT20	0	//±£Áô
#define MCU_CAP_PLM_BIT21	0	//±£Áô
#define MCU_CAP_PLM_BIT22	0	//±£Áô
#define MCU_CAP_PLM_BIT23	0	//±£Áô
#define MCU_CAP_PLM_BIT24	0	//±£Áô
#define MCU_CAP_PLM_BIT25	0	//±£Áô
#define MCU_CAP_PLM_BIT26	0	//±£Áô
#define MCU_CAP_PLM_BIT27	0	//±£Áô
#define MCU_CAP_PLM_BIT28	0	//±£Áô
#define MCU_CAP_PLM_BIT29	0	//±£Áô
#define MCU_CAP_PLM_BIT30	0	//±£Áô
#define MCU_CAP_PLM_BIT31	0	//±£Áô

//MCU+PARAMS+m:nnnn&:ÉèÖÃMCUÉý¼¶¸ñÊ½ºÍÃ¿´Î·¢ËÍÉý¼¶°üµÄ³¤¶È
#define MCU_PARAMS_FOR    0      //Îª0±íÊ¾Ê¹ÓÃescape·½Ê½£¬ÊÇÄ¬ÈÏ·½Ê½£¬Îª1±íÊ¾Ê¹ÓÃHEX±àÂë£¨Êý¾ÝÎªÁ½±¶£©
#define MCU_PARAMS_VAL    1024 //Ã¿´Î¸øMCU·¢ËÍÉý¼¶°üµÄÊý¾Ý³¤¶È£¬Ä¬ÈÏÎª1024£¬Í¨³£Õâ¸öÖµÇëÉèÖÃÎªMCU Êý¾Ý»º´æÇø³¤¶ÈµÄÒ»°ë£¬µ«×îµÍ²»ÄÜÐ¡ÓÚ128£¬×î´ó²»ÄÜ´óÓÚ4096£¬±ØÐëÊÇ16µÄ±¶Êý¡£

//**MCU+PRESET+n&£ºÉèÖÃÔ¤ÖÃ°´¼üÊýÁ¿
#define MCU_PRESET_VAL		6	//ÉèÖÃÔ¤ÖÃ°´¼üÊýÁ¿£¬×î¶àÎª10¸ö£¬ÎªÁËÏòÇ°¼æÈÝ£¬0±íÊ¾Ä¬ÈÏ6¸öÔ¤ÖÃ£¨²»»Ø¸´Ä¬ÈÏÎª0£©£¬-1±íÊ¾Ã»ÓÐÔ¤ÖÃ¹¦ÄÜ¡£×¢ÒânÇ°ÃæÓÐ¸ö+£¬Õâ¸öÖ¸ÁîÔÚÕý³£¿ª»úºó·¢ËÍÒ²ÓÐÐ§¡£

//MCU+CAP+PWDpassword&: ÉèÖÃWiFi APÃÜÂë
#define MCU_CAP_PWD       ""     //ÃÜÂëÎªÃ÷ÎÄ£¬²»Òª°üº¬ÌØÊâ×Ö·ûºÍÓëMCUÐ­Òé³åÍ»£¬³¤¶È8-15×Ö½Ú£¬Õâ¸öÃüÁîÔÚÕý³£¿ª»úºó·¢ËÍÎÞÐ§¡£

//**MCU+VMX+nnn:ÓïÒôÌáÊ¾×î¸ßÔÊÐíµÄÒôÁ¿ÉèÖÃ
#define MCU_VMX_VAL		80	       //nnnÎª0~100µÄÊý×Ö£¬Èç001, 022, 100£»ÓïÒôÌáÊ¾×î¸ßÔÊÐíµÄÒôÁ¿£¬Ò»µ©µ±Ç°ÒôÁ¿³¬¹ýÔÊÐíÒôÁ¿£¬Èí¼þ»á×ö½µÒô´¦Àí£¬Õâ¸öÖµÉèÖÃ<10»òÕß´óÓÚ90ÎÞÐ§¡£

//MCU+FMT+nnn:ÉèÖÃWiFiÄ£¿éIISÊý¾Ý¸ñÊ½£¬
#define MCU_FMT_VAL   000     //nnnÎª000Éè¶¨±ê×¼¸ñÊ½£¬nnn=001ÎªÉè¶¨Left justified¸ñÊ½£¬Ä¬ÈÏ000£¬±ê×¼IIS¸ñÊ½¡£
#ifdef __cplusplus
}
#endif//__cplusplus

#endif


