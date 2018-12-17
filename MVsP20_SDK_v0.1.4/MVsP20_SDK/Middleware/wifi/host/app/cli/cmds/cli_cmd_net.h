/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef _CLI_CMD_NET_
#define _CLI_CMD_NET_


void cmd_ifconfig(s32 argc, char *argv[]);
void cmd_route(s32 argc, char *argv[]);
void cmd_ping(s32 argc, char *argv[]);
void cmd_ttcp(s32 argc, char *argv[]);
void cmd_iperf3(s32 argc, char *argv[]);
void cmd_net_app(s32 argc, char *argv[]);
void cmd_net_mgr(s32 argc, char *argv[]);
void cmd_setnetif(s32 argc, char *argv[]);

void sim_set_link_status(bool on);
void sim_net_cfg(void);
extern void show_invalid_para(char * cmd_name);



#endif /* _CLI_CMD_NET_ */

