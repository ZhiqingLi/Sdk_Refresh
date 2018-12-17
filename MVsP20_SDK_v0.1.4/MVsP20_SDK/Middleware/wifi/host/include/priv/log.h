/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef _LOG_H_
#define _LOG_H_

#include <host_config.h>
#include <ssv_types.h>

#include <cmd_def.h>
#include <msgevt.h>
#include <porting.h>



// ===============================================================================================
//											Global
// ===============================================================================================

//
// note	:
//		The 'sys_init_prnf' is to note that it is in system init stage now. All 'log printf' func
// can NOT be used during this stage because the log module is NOT inited yet and the 'log mutex'
// is NOT inited yet.
//
#define 	sys_init_prnf		hal_print

#define		LOG_DMSG_FILENAME	"ssv6200-uart.dmsg"
#define		LOG_MAX_TAG			16
#define		LOG_MAX_PATH			256
#define		LOG_MAX_FILENAME		48		// without directory path
#define		LOG_DMSG_MAX_ARGS	16

// global variables for acceleration
#define		PRN_BUF_MAX			256
#define		PRN_TAG_MAX			LOG_MAX_TAG
#define		ACC_BUF_MAX			(2*PRN_BUF_MAX + 3*PRN_TAG_MAX)






// ===============================================================================================
//									Level & Modules definition
// ===============================================================================================

#if SSV_LOG_DEBUG

extern u32 g_log_module;
extern u32 g_log_min_level;

#define LOG_PLATFORM_DIAG(x)   do { hal_print x; } while(0)
#define LOG_DEBUGF(debug, message) do { \
                                    if ( \
                                   ((debug) & g_log_module) && \
                                   ((s16)((debug) & LOG_LEVEL_MASK_LEVEL) >= g_log_min_level)) { \
                                    LOG_PLATFORM_DIAG(message); \
                                    } \
                                    } while(0)


#else  /* SSV_LOG_DEBUG */
#define LOG_DEBUGF(debug, message) do { ;} while(0)
#endif /* SSV_LOG_DEBUG */











//=====================================================
//Abandon
//=====================================================


// LOG_LEVEL_FATAL:
// -  print "program halt!!!" msg & terminate program immerdiately;
// -  the 'LOG_LEVEL_FATAL' will always execute, ignore the influence of 'module' & 'level' settings.
#define LOG_LEVEL_ON		0x0					// runtime option : show all level msg
#define LOG_LEVEL_TRACE		0x1
#define LOG_LEVEL_DEBUG		0x2
#define LOG_LEVEL_INFO		0x3
#define LOG_LEVEL_WARN		0x4
#define LOG_LEVEL_FAIL		0x5
#define LOG_LEVEL_ERROR		0x6
#define LOG_LEVEL_FATAL		0x7
#define LOG_LEVEL_OFF		(LOG_LEVEL_FATAL+1)	// runtime option : show NO  level msg

extern const char*	g_log_lvl_tag[LOG_LEVEL_OFF];

// switch to turn on/off the message belongs to the 'module'
// max # of module id define : 0~31
typedef enum {
        LOG_MODULE_EMPTY        = 0,    // 0
        LOG_MODULE_MRX,                 // 1
        LOG_MODULE_MTX,                 // 2
        LOG_MODULE_EDCA,                // 3
        LOG_MODULE_PBUF,                // 4
        LOG_MODULE_L3L4,                // 5
        LOG_MODULE_MGMT,                // 6
        LOG_MODULE_FRAG,                // 7
        LOG_MODULE_DEFRAG,              // 8
        LOG_MODULE_MLME,                // 9
        LOG_MODULE_CMD,                 // 10 Command Engine
        LOG_MODULE_WPA,                 // 11
        LOG_MODULE_MAIN,                // 12
        LOG_MODULE_ALL                  // 13
} LOG_MODULE_E;
extern const char*	g_log_mod_tag[LOG_MODULE_ALL];


#define LOG_MODULE_MASK(n)			(((n) == LOG_MODULE_ALL) ? 0xffffffffU :   (0x00000001U << (int)(n)))
#define LOG_MODULE_I_MASK(n)		(((n) == LOG_MODULE_ALL) ? 0x00000000U : (~(0x00000001U << (int)(n))))

#ifdef __SSV_UNIX_SIM__
#define LOG_LEVEL_SET(n)			{ LOG_MUTEX_LOCK(); g_log_prnf_cfg.lvl = (n);					LOG_MUTEX_UNLOCK(); }

#define LOG_MODULE_TURN_ON(n)		{ LOG_MUTEX_LOCK();	g_log_prnf_cfg.mod |= LOG_MODULE_MASK(n);	LOG_MUTEX_UNLOCK(); }
#define LOG_MODULE_TURN_OFF(n)		{ LOG_MUTEX_LOCK();	g_log_prnf_cfg.mod &= LOG_MODULE_I_MASK(n);	LOG_MUTEX_UNLOCK(); }

#define LOG_FILELINE_TURN_ON()		{ LOG_MUTEX_LOCK(); g_log_prnf_cfg.fl = 1;						LOG_MUTEX_UNLOCK(); }
#define LOG_FILELINE_TURN_OFF()		{ LOG_MUTEX_LOCK(); g_log_prnf_cfg.fl = 0;						LOG_MUTEX_UNLOCK(); }

#define LOG_TAG_LEVEL_TURN_ON()		{ LOG_MUTEX_LOCK(); g_log_prnf_cfg.prn_tag_lvl = true;			LOG_MUTEX_UNLOCK(); }
#define LOG_TAG_LEVEL_TURN_OFF()	{ LOG_MUTEX_LOCK(); g_log_prnf_cfg.prn_tag_lvl = false;			LOG_MUTEX_UNLOCK(); }
#define LOG_TAG_LEVEL_TURN(x)		{ LOG_MUTEX_LOCK(); g_log_prnf_cfg.prn_tag_lvl = (x);			LOG_MUTEX_UNLOCK(); }

#define LOG_TAG_MODULE_TURN_ON()	{ LOG_MUTEX_LOCK(); g_log_prnf_cfg.prn_tag_mod = true;			LOG_MUTEX_UNLOCK(); }
#define LOG_TAG_MODULE_TURN_OFF()	{ LOG_MUTEX_LOCK(); g_log_prnf_cfg.prn_tag_mod = false;			LOG_MUTEX_UNLOCK(); }
#define LOG_TAG_MODULE_TURN(x)		{ LOG_MUTEX_LOCK(); g_log_prnf_cfg.prn_tag_mod = (x);			LOG_MUTEX_UNLOCK(); }

#define LOG_TAG_SUPPRESS_ON() 		{ LOG_MUTEX_LOCK(); g_log_prnf_cfg.prn_tag_sprs = true;			LOG_MUTEX_UNLOCK(); }
#define LOG_TAG_SUPPRESS_OFF()		{ LOG_MUTEX_LOCK(); g_log_prnf_cfg.prn_tag_sprs = false;		LOG_MUTEX_UNLOCK(); }
#endif

// ===============================================================================================
//											log mutex
// ===============================================================================================
// just for debugging use
#define		LOG_MUTEX_DBGMSG	0
#define		LOG_MUTEX_DBG		0

#if (LOG_MUTEX_DBG == 0)
#define _os_mutex			OsMutex
#define _os_mutex_init		OS_MutexInit
#define _os_mutex_lock		OS_MutexLock
#define _os_mutex_unlock	OS_MutexUnLock
#else
#define _os_mutex			u32
#define _os_mutex_init(x)	(g_log_mutex = 0)
#define _os_mutex_lock(x)	(g_log_mutex++)
#define _os_mutex_unlock(x)	(g_log_mutex--)
#endif


extern _os_mutex			g_log_mutex;
extern u32					g_dbg_mutex;
//if (!gOsFromISR)
#define LOG_MUTEX_INIT()			\
{									\
	if (LOG_MUTEX_DBGMSG)  hal_print("%s() #%d: log mutex init   = %d\n\r", __FUNCTION__, __LINE__, (g_dbg_mutex = 0)); \
	_os_mutex_init(&g_log_mutex);	\
}
#define LOG_MUTEX_LOCK()			\
{									\
	if (!gOsFromISR)                \
    	_os_mutex_lock(g_log_mutex);\
	if (LOG_MUTEX_DBGMSG)	hal_print("%s() #%d: log mutex lock   = %d\n\r", __FUNCTION__, __LINE__, (++g_dbg_mutex)); \
}
#define LOG_MUTEX_UNLOCK()			\
{									\
	if (LOG_MUTEX_DBGMSG)	hal_print("%s() #%d: log mutex unlock = %d\n\r", __FUNCTION__, __LINE__, (--g_dbg_mutex)); \
	if (!gOsFromISR)				\
	_os_mutex_unlock(g_log_mutex);	\
}

// ===============================================================================================
//							log prnf cfg & output stream descriptor
// ===============================================================================================
typedef struct
{
	u32		lvl;
	u32 	mod;
	u32	    fl;

	u32     prn_tag_lvl;
	u32     prn_tag_mod;
	u32     prn_tag_sprs;
	u32     chk_tag_sprs;	// won't check 'level' & 'module', ONLY used by log_printf().
							// this is used when log_printf() is inside LOG_TAG_SUPPRESS_ON() & LOG_TAG_SUPPRESS_OFF()
} log_prnf_cfg_st;
extern log_prnf_cfg_st	g_log_prnf_cfg;


#define LOG_OUT_HOST_TERM			0x01	// host terminal
#define LOG_OUT_HOST_FILE			0x02	// host file
#define LOG_OUT_SOC_TERM			0x10	// soc terminal (UART)
#define LOG_OUT_SOC_HOST_TERM		0x20	// soc -> host terminal (DbgView)
#define LOG_OUT_SOC_HOST_FILE		0x40	// soc -> host file

#ifdef __SSV_UNIX_SIM__
#define	LOG_OUT_DST_IS_OPEN(x)		(g_log_out.dst_open & (x))
#define LOG_OUT_DST_OPEN(x)			{ LOG_MUTEX_LOCK(); g_log_out.dst_open |= (x);		LOG_MUTEX_UNLOCK(); }
#define LOG_OUT_DST_CLOSE(x)		{ LOG_MUTEX_LOCK(); g_log_out.dst_open &= (~(x));	LOG_MUTEX_UNLOCK(); }

#define LOG_OUT_DST_IS_CUR_ON(x)	(g_log_out.dst_cur & (x))
#define LOG_OUT_DST_CUR_ON(x)		{ LOG_MUTEX_LOCK(); g_log_out.dst_cur |= (x);		LOG_MUTEX_UNLOCK(); }
#define LOG_OUT_DST_CUR_OFF(x)		{ LOG_MUTEX_LOCK(); g_log_out.dst_cur &= (~(x));	LOG_MUTEX_UNLOCK(); }
#endif
// #define LOG_OUT_FILE_MODE		"a+"	// mode for fopen()
#define LOG_OUT_FILE_MODE		"w"	// mode for fopen()

typedef struct
{
	u8		dst_open;			// opened  destination
	u8		dst_cur;			// current destination
	void	*fp;				// the log file ptr  on host side
	u8		path[LOG_MAX_PATH];	// the log file path on host side
} log_out_desc_st;


#ifdef __SSV_UNIX_SIM__
extern log_out_desc_st	g_log_out;
#define LOG_out_dst_open		_sim_out_dst_open
#define LOG_out_dst_close		_sim_out_dst_close
#define LOG_out_dst_turn_on		_sim_out_dst_turn_on
#define LOG_out_dst_turn_off	_sim_out_dst_turn_off

extern bool	LOG_out_dst_open(u8 _dst, const u8 *_path);
extern bool	LOG_out_dst_close(u8 _dst);
extern bool	LOG_out_dst_turn_on(u8 _dst);
extern bool	LOG_out_dst_turn_off(u8 _dst);
extern void LOG_out_desc_dump(void);
extern bool LOG_out_init(void);
#endif


// ===============================================================================================
//								printf & fatal func & quick macro
// ===============================================================================================

extern void log_printf(const char *fmt, ...);
extern void ssv6xxx_printf(u32 n, u32 m, const char *file, u32 line, const char *fmt, ...);
extern void ssv6xxx_fatal(u32 m, const char *file, u32 line, const char *fmt, ...);

// 'LM' means 'level & module'
#define LOG_PRINTF_LM(l, m, fmt, ...)	(((l) == LOG_LEVEL_FATAL) ? ssv6xxx_fatal(m, __func__, __LINE__, fmt, ##__VA_ARGS__) : ssv6xxx_printf((l), (m), __FILE__, __LINE__, fmt, ##__VA_ARGS__))
#define log_printf hal_print
// quick macro
#ifndef __SSV_UNIX_SIM__
#define LOG_PRINTF hal_print
#define LOG_TRACE hal_print
#define LOG_DEBUG hal_print
#define LOG_ERROR hal_print
#define LOG_INFO hal_print
#else
#define LOG_PRINTF(fmt, ...)		LOG_PRINTF_LM(LOG_LEVEL_ON,		LOG_MODULE_EMPTY, fmt, ##__VA_ARGS__)
#define LOG_TRACE(fmt,  ...)		LOG_PRINTF_LM(LOG_LEVEL_TRACE,	LOG_MODULE_EMPTY, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt,  ...)		LOG_PRINTF_LM(LOG_LEVEL_DEBUG,	LOG_MODULE_EMPTY, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt,  ...)		LOG_PRINTF_LM(LOG_LEVEL_ERROR,	LOG_MODULE_EMPTY, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt,   ...)		LOG_PRINTF_LM(LOG_LEVEL_INFO,	LOG_MODULE_EMPTY, fmt, ##__VA_ARGS__)
#endif
#define LOG_WARN(fmt,   ...)		LOG_PRINTF_LM(LOG_LEVEL_WARN,	LOG_MODULE_EMPTY, fmt, ##__VA_ARGS__)
#define LOG_FAIL(fmt,   ...)		LOG_PRINTF_LM(LOG_LEVEL_FAIL,	LOG_MODULE_EMPTY, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt,  ...)	    LOG_PRINTF_LM(LOG_LEVEL_FATAL,	LOG_MODULE_EMPTY, fmt, ##__VA_ARGS__)
// 'M' means 'module'
#define LOG_PRINTF_M(m, fmt, ...)	LOG_PRINTF_LM(LOG_LEVEL_ON,					   m, fmt, ##__VA_ARGS__)
#define LOG_TRACE_M(m,  fmt, ...)	LOG_PRINTF_LM(LOG_LEVEL_TRACE,				   m, fmt, ##__VA_ARGS__)
#define LOG_DEBUG_M(m,  fmt, ...)	LOG_PRINTF_LM(LOG_LEVEL_DEBUG,				   m, fmt, ##__VA_ARGS__)
#define LOG_INFO_M(m,   fmt, ...)	LOG_PRINTF_LM(LOG_LEVEL_INFO,				   m, fmt, ##__VA_ARGS__)
#define LOG_WARN_M(m,   fmt, ...)	LOG_PRINTF_LM(LOG_LEVEL_WARN,				   m, fmt, ##__VA_ARGS__)
#define LOG_FAIL_M(m,   fmt, ...)	LOG_PRINTF_LM(LOG_LEVEL_FAIL,				   m, fmt, ##__VA_ARGS__)
#define LOG_ERROR_M(m,  fmt, ...)	LOG_PRINTF_LM(LOG_LEVEL_ERROR,				   m, fmt, ##__VA_ARGS__)
#define LOG_FATAL_M(m,  fmt, ...)   LOG_PRINTF_LM(LOG_LEVEL_FATAL,				   m, fmt, ##__VA_ARGS__)

// ===============================================================================================
//										dbgmsg stripping
// ===============================================================================================

#define	LOG_DMSG_MAX_ARGS	16


#define T(str)	str

// printf string to SOC_HOST_TERM (defaultly, DbgView in WIN32)
#if (defined _WIN32)
	#define _prnf_soc_host_term		OutputDebugStringA
#else
	#define _prnf_soc_host_term		log_printf
#endif

#define LOG_EVT_PRNF_BUF_MAX	(LOG_DMSG_MAX_ARGS*sizeof(u32) + 32)	// 32 -> rsvd space for safety
#define LOG_EVT_PRNF_MAXLEN		(sizeof(log_evt_prnf_st) + LOG_EVT_PRNF_BUF_MAX)

// ===============================================================================================
//											Misc
// ===============================================================================================
extern void LOG_init(bool tag_level, bool tag_mod, u32 level, u32 mod_mask, bool fileline);



#endif	// _LOG_H_

