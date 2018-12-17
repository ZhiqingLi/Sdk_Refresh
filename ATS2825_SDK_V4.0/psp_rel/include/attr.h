/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：属性信息定义
 * 作者：liminxian
 ********************************************************************************/
#ifndef _AP_BANK_DATA_ATTR_H
#define _AP_BANK_DATA_ATTR_H

/**
 生成的COMMON/DATA/BSS段都是GLOBAL的，只有_BANK_DATA_ATTR_定义的全局变量、RODATA、和text是BANK的
 因为static生成bss段，但其实它又是bank的，所以不知道放哪好，因此如果只想在一个bank用，那就不要用static
 未初始化的变量有时候生成common段，有时生成bss段，一般是common段，但难确定，所以全部归到GLOBAL的，这样链接文件容易写。
 */

#ifndef _ASSEMBLER_
#ifndef PC
#define _BANK_DATA_ATTR_ __attribute__((section(".xdata")))
#define __section__(n) __attribute__((section (n)))
#define __FAR__ __attribute__( (far) )
#define _GLOBAL_DATA_ATTR_      //data
#define _GLOBAL_DATA_REUSED_ATTR_ __attribute__((section("reused_data")))     //双任务共用data
//#define static _BANK_DATA_ATTR_   //不推荐使用static
#define __RSTR(name, _str)  ({ static __section__(name) const char __rodata_str[] = _str; __rodata_str; })
#else
#define _BANK_DATA_ATTR_
#define __section__(n)
#define __FAR__
#define _GLOBAL_DATA_ATTR_      //data
#define _GLOBAL_DATA_REUSED_ATTR_
#endif

#else

#define _BANK_DATA_ATTR_ .section bank_data
#define _GLOBAL_DATA_ATTR_ .data
#define _RDATA_ATTR_ .section .rodata //asm default is .rdata
#define _GLOBAL_DATA_REUSED_ATTR_ .section reused_data

#endif /*_ASSEMBLER_*/

#endif
