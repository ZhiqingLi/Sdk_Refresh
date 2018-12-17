#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#ifndef PC
#define	GET_REAL_ADDR

#define RegisterKeyCallback
#define UnregisterKeyCallback

#else

//#include "os_cpu.h"
/*
 // 把绝对地址转换为缓冲区的地址
 // 参数:    addr		小机端绝对地址
 // 返回:	返回缓冲区地址
 */
extern unsigned long GetRealAddrPC(unsigned long addr);

extern int SetOtherKeyCallback(char KeyChar, int(* KeyCallback)(int KeyValue));
extern int ClearOtherKeyCallback(char KeyChar);

#define	GET_REAL_ADDR			  GetRealAddrPC

#define SDKFLAG				2	// 需要用到 SDKFLAG 宏的，请包含该文件
// 修改 .\case\inlcude\caseconfig 中的 IMAGEDIRNAME 时，同时修改下面 IMAGEDIRNAME 的定义
#define CPU400240				1		// 400x240
#define RGB320240				2		// 320x240
#define RGB480272				3		// 480x272
#define CASEFLAG				RGB320240

#define NANDFLASH				1
#define SPINOR					2
#define DISKTYPE				NANDFLASH

#define __FUNCTION__			__FILE__

#define RegisterKeyCallback			SetOtherKeyCallback
#define UnregisterKeyCallback		ClearOtherKeyCallback

#define DLL_IMP __declspec(dllimport)

#endif

#endif

