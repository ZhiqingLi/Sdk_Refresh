/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef _USER_SLINK_H_
#define _USER_SLINK_H_


typedef enum{
    EN_SCONFIG_NOT_READY=0, //SmartConfig solution is not ready.
    EN_SCONFIG_GOING, //SmartConfig solution is running, we can feed the data to it)
    EN_SCONFIG_LOCK, //SmartConfig solution lock the channel of target AP.
    EN_SCONFIG_DONE, //SmartConfig solution get the AP's information
}EN_SCONFIG_STATUS;

typedef struct
{
	u8 		ssid[MAX_SSID_LEN];//ssid[] didnot include '\0' at end of matrix
	u8		ssid_len;
	u8		key[MAX_PASSWD_LEN+1];
	u8		key_len;
} SMART_COFIG_RESULT;

int SmartConfigInit(void);
int SmartConfigSM(void);
void SmartConfigDeinit(void);
void SmartConfigPaserData(u8 channel, u8 *rx_buf, u32 len);
void SmartConfigConnect(void);

#endif /* _USER_SLINK_H_ */

