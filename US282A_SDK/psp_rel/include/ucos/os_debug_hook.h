/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：debug内核支持相关宏定义和函数声明
 * 作者：liminxian
 ********************************************************************************/
 
#ifndef __OS_DEBUG_HOOK_H__
#define __OS_DEBUG_HOOK_H__

#ifndef _ASSEMBLER_

typedef struct {
	unsigned char prio_cur;
	unsigned char prio_high_rdy;
} prio_change_info_t;

typedef struct {
	unsigned char prio_cur;
	unsigned char prio_high_rdy;
	unsigned int cause;
	void *event;
} sw_info_t;

typedef struct {
	unsigned int mode;
	void *arg;
} uview_mode_t;

/*!
 *  \brief
 *  UVIEW模式
 *  \li UVIEW_MODE_RT : 实时模式
 *  \li UVIEW_MODE_CACHE : 缓冲模式
 */
enum uview_mode {
	UVIEW_MODE_RT, UVIEW_MODE_CACHE
};

#define OSTASK_DEBUG_WHOLE_ENABLE       (1)
#define OSTASK_DEBUG_WHOLE_DISABLE      (0)

#define OSTASK_DEBUG_PCNT_ENABLE        (1)
#define OSTASK_DEBUG_PCNT_DISABLE       (0)

struct ostask_debug_cfg_t {
	char debug_whole_enable;
	char debug_pcnt_enable;
};

struct ostask_debug_data_t {
	unsigned long long debug_pcnt_event0_data;
	unsigned long long debug_pcnt_event1_data;
};

//extern struct ostask_debug_data_t ostask_debug_data[];
//
//extern void set_load_from_pc(int flag);
//extern int set_image_path_name_of_pc(int type, char *pathname);
//extern int remote_io_init(void);
//
//extern int ostasksw_debug_cfg(const struct ostask_debug_cfg_t *p_cfg);

#endif/*_ASSEMBLER_*/
#endif/*__OS_DEBUG_HOOK_H__*/
