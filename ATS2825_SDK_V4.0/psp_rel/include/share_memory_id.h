#ifndef __SHARE_MEM_ID_H__
#define __SHARE_MEM_ID_H__

#define SHARE_MEM_ID_BTPLAYPIPE      0 //蓝牙推歌pipe管理结构体，由 bt stack 共享
#define SHARE_MEM_ID_BTSTACK_STATUS  1 //蓝牙协议栈当前状态，由 bt common 共享
#define SHARE_MEM_ID_BTRCPPIPE       2 //蓝牙RCP pipe管理结构体，由 bt stack 共享
#define SHARE_MEM_ID_DAECFG          3 //DAE配置结构体，由 config 申请空间并共享
#define SHARE_MEM_ID_USOUND          4 //usound引擎与前台共享pc同步的音量和标志
#define SHARE_MEM_ID_AGC_AEC         5 //btcall and dap lib share memory
#define SHARE_MEM_ID_TWS_INFO        6 //蓝牙协议栈 TWS 信息共享
#endif
