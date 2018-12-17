/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef _SSV_LIB_H_
#define _SSV_LIB_H_

#include <host_config.h>
#include <rtos.h>

#if (CONFIG_HOST_PLATFORM == 0)
#include <stdarg.h>
#endif

struct llist {
    struct llist   *next;
};

struct llist_head {
    struct llist   *list;
    struct llist   *last;
    u32 llen;
};

struct list_q {
    struct list_q   *next;
    struct list_q   *prev;
    unsigned int    qlen;
};

void llist_head_init(struct llist_head *lhd);
struct llist *llist_pop(struct llist_head *lhd);
void llist_push(struct llist_head *lhd, struct llist *new);
u32 llist_l_len(struct llist_head *lhd);
struct llist *llist_pop_safe(struct llist_head *lhd, OsMutex *pmtx);
void llist_push_safe(struct llist_head *lhd, struct llist *new, OsMutex *pmtx);
u32 llist_l_len_safe(struct llist_head *lhd, OsMutex *pmtx);


void list_q_init(struct list_q *qhd);
void list_q_qtail(struct list_q *qhd, struct list_q *newq);
struct list_q * list_q_deq(struct list_q *qhd);
unsigned int list_q_len(struct list_q *qhd);
void list_q_insert(struct list_q *qhd, struct list_q *prev, struct list_q *newq);
void list_q_remove(struct list_q *qhd,struct list_q *curt);

void list_q_qtail_safe(struct list_q *qhd, struct list_q *newq, OsMutex *pmtx);
struct list_q * list_q_deq_safe(struct list_q *qhd, OsMutex *pmtx);
u32 list_q_len_safe(struct list_q *qhd, OsMutex *pmtx);
void list_q_insert_safe(struct list_q *qhd, struct list_q *prev, struct list_q *newq, OsMutex *pmtx);
void list_q_remove_safe(struct list_q *qhd,struct list_q *curt, OsMutex *pmtx);

LIB_APIs u32 ssv6xxx_atoi_base( const char *s, u32 base );
LIB_APIs s32 ssv6xxx_atoi( const char *s );
LIB_APIs s32 ssv6xxx_isalpha(s32 c);
LIB_APIs s32 ssv6xxx_str_tolower(char *s);
LIB_APIs s32 ssv6xxx_str_toupper(char *s);

LIB_APIs s32 ssv6xxx_strrpos(const char *str, char delimiter);

#if (CONFIG_HOST_PLATFORM == 1)
u64 ssv6xxx_64atoi( char *s );
#endif



LIB_APIs char ssv6xxx_toupper(char ch);
LIB_APIs char ssv6xxx_tolower(char ch);


LIB_APIs s32 ssv6xxx_strcmp( const char *s0, const char *s1 );
LIB_APIs char *ssv6xxx_strcat(char *s, const char *append);
LIB_APIs char *ssv6xxx_strncpy(char *dst, const char *src, size_t n);
LIB_APIs size_t ssv6xxx_strlen(const char *s);
LIB_APIs char *ssv6xxx_strcpy(char *dst, const char *src);
LIB_APIs s32 ssv6xxx_strncmp ( const char * s1, const char * s2, size_t n);
LIB_APIs void *ssv6xxx_memset(void *s, s32 c, size_t n);
LIB_APIs void *ssv6xxx_memcpy(void *dest, const void *src, size_t n);
LIB_APIs s32 ssv6xxx_memcmp(const void *s1, const void *s2, size_t n);


#if 0
LIB_APIs void ssv6xxx_vsnprintf(char *out, size_t size, const char *format, va_list args);
LIB_APIs void ssv6xxx_snprintf(char *out, size_t size, const char *format, ...);
LIB_APIs void ssv6xxx_printf(const char *format, ...);
#endif

//LIB_APIs s32 putstr(const char *str, size_t size);
//LIB_APIs s32 snputstr(char *out, size_t size, const char *str, size_t len);
//LIB_APIs void fatal_printf(const char *format, ...);


#if (CLI_ENABLE==1 && CONFIG_HOST_PLATFORM==0)
LIB_APIs s32 kbhit(void);
LIB_APIs s32 getch(void);
LIB_APIs s32 putchar(s32 ch);
#endif

#if 0
LIB_APIs void ssv6xxx_raw_dump(u8 *data, s32 len);

// with_addr : (true) -> will print address head "xxxxxxxx : " in begining of each line
// addr_radix: 10 (digial)  -> "00000171 : "
//		     : 16 (hex)		-> "000000ab : "
// line_cols : 8, 10, 16, -1 (just print all in one line)
// radix     : 10 (digital) ->  171 (max num is 255)
//			   16 (hex)		-> 0xab
// log_level : log level  pass to LOG_PRINTF_LM()
// log_module: log module pass to LOG_PRINTF_LM()
//
LIB_APIs bool ssv6xxx_raw_dump_ex(u8 *data, s32 len, bool with_addr, u8 addr_radix, s8 line_cols, u8 radix, u32 log_level, u32 log_module);
#endif

LIB_APIs void hex_dump(const void *addr, u32 size);

void _packetdump(const char *title, const u8 *buf,
                             size_t len);
LIB_APIs void halt(void);
//=============================================================
void ssv6xxx_HW_enable(void);
void ssv6xxx_HW_disable(void);
int ssv6xxx_init_mac(void);
int ssv6xxx_init_sta_mac(u32 wifi_mode);
int ssv6xxx_init_ap_mac(void);
bool ssv6xxx_download_fw(u8*, u32);
void ssv6xxx_set_wakeup_bb_gpio(bool en);
int ssv6xxx_promiscuous_enable(void);
int ssv6xxx_promiscuous_disable(void);

#if(ENABLE_DYNAMIC_RX_SENSITIVE==1)
void ssv6xxx_sta_mode_disconnect(void);
void ssv6xxx_sta_mode_connect(void);
#endif

#endif /* _SSV_LIB_H_ */

