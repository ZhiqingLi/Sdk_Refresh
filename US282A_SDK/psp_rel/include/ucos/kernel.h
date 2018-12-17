/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：内核使用的编译器特性相关头文件
 * 作者：liminxian
 ********************************************************************************/
 
#ifndef __KERNEL_H__
#define __KERNEL_H__

#define	KERN_EMERG	"<0>"	/* system is unusable			*/
#define	KERN_ALERT	"<1>"	/* action must be taken immediately	*/
#define	KERN_CRIT	"<2>"	/* critical conditions			*/
#define	KERN_ERR	"<3>"	/* error conditions			*/
#define	KERN_WARNING	"<4>"	/* warning conditions			*/
#define	KERN_NOTICE	"<5>"	/* normal but significant condition	*/
#define	KERN_INFO	"<6>"	/* informational			*/
#define	KERN_DEBUG	"<7>"	/* debug-level messages			*/

#ifndef _ASSEMBLER_
extern int console_printk[];

#define console_loglevel (console_printk[0])
#define default_message_loglevel (console_printk[1])
#define minimum_console_loglevel (console_printk[2])
#define default_console_loglevel (console_printk[3])

//	#define printk(x...)   printf(x)
/* Optimization barrier */
/* The "volatile" is due to gcc bugs */
#define barrier() __asm__ __volatile__("": : :"memory")

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define ALIGN(x,a) (((x)+(a)-1)&~((a)-1))

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define SIZE(TYPE, MEMBER) sizeof(((TYPE *)0)->MEMBER)

/**
 * container_of - cast a member of a structure out to the containing structure
 *
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({			\
        const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
        (type *)( (char *)__mptr - offsetof(type,member) );})

#endif
#endif	/*__KERNEL_H__*/
