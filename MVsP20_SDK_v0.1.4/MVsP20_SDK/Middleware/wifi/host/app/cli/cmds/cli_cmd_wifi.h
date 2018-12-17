/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef _CLI_CMD_WIFI_H_
#define _CLI_CMD_WIFI_H_

void cmd_iw(s32 argc, char *argv[]);
void cmd_ctl(s32 argc, char *argv[]);
void cmd_rc(s32 argc, char *argv[]);
void cmd_mib(s32 argc, char *argv[]);
void cmd_ampdu(s32 argc, char *argv[]);
void ssv6xxx_wifi_cfg(void);


#define GET_SEC_INDEX(start,end,c)         \
	while(start <end ) {                   \
		if(c & BIT(start)){                \
			break;					       \
		}                                  \
		start++;                           \
	}                                      \


#endif /* _CLI_CMD_WIFI_H_ */

