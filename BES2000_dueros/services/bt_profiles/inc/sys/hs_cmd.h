#ifndef __HS_CMD_H_
#define __HS_CMD_H_

#include "hs.h"


/*HSP Command Definition*/
#define HS_CKPD_CMD	0x00
#define HS_COMMAND_REPORT_SPEAKER_VOLUME	HF_COMMAND_REPORT_SPEAKER_VOLUME
#define HS_COMMAND_REPORT_MIC_VOLUME	HF_COMMAND_REPORT_MIC_VOLUME
#define HS_COMMAND_SEND_AT_COMMAND		HF_COMMAND_SEND_AT_COMMAND
 
#define HS_COMMAND_TIMEOUT	HF_COMMAND_TIMEOUT
/* Unknown AT data received */ 
#define HS_AT_UNKNOWN                0xFFFF



BtStatus   HsAddCommand(HsChannel *Chan, HsCommand *Command);
BtStatus   HsCompleteCommand(   HsChannel *Chan, HsCommand *Command);
BtStatus   HsExecuteNextCommand(HsChannel *Chan);
BtStatus   HsRemoveCommand(     HsChannel *Chan, HsCommand *Command);
HsCommand *HsEnumCommands(      HsChannel *Chan, HsCommand *Command);
HsCommand *HsGetCurrentCommand( HsChannel *Chan);
void       HsFlushCommands(     HsChannel *Chan, BtStatus Status);
void HsAtParseResultCode(HsChannel *Chan,
                       U8        *Input,
                       U16        InputLen,
                       AtResults *Results);
BOOL HsStartNextQueuedCmd(HsChannel *hfChanP, BOOL preSend);
BtStatus HsAtSendCommand(HsChannel *Chan, AtCommands *command);



#endif /* __HF_CMD_H_ */ 

