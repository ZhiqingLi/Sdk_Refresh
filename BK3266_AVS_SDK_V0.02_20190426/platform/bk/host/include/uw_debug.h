/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define COMMON_PRINT_FUNC os_printf
#define remote_debug_dump_prints()              do { } while (0)

/* Can be used to enforce compile time restrictions
 * (that cannot be provided by the preprocessor) */
#define STATIC_ASSERT(cond, msg)        \
    typedef char STATIC_ASSERT_##msg[(cond)?1:-1]

#define DBG_E(type, message) do { } while (0)
#define DBG_W(type, message) do { } while (0)
#define DBG_I(type, message) do { } while (0)
#define DBG_V(type, message) do { } while (0)
#define DBG_X(type, message) do { } while (0)
#define TEST_MSG(type, message) do { } while (0)
#define RETURN_WITH_ERROR(type, code, message) do { return code; } while (0)
#define DBG_HEX_DUMP(type, level, buffer, size, per_row) do { } while (0)
#define DBG_RUN(type, level, code)      do { } while (0)
#define DBG_IF(expr)                    if(0)
#define DBG_SET_LEVEL(type, level)      do { } while (0)
#define DBG_SET_GLOBAL_LEVEL(level)     do { } while (0)
#define DBG_GET_LEVEL(type)             do { } while (0)
#define PRINT_DBG(type, level, message) do { } while (0)
#define DECLARE_FNAME(name)				 //os_printf(name"\r\n")
#define FNAME
#define KASSERT(expr, msg)              do { } while (0)
#define KASSERT0(expr)                  do { } while (0)
#define DBG_ASSERT_CTX(mask)            do { } while (0)

#define NOT_IMPLEMENTED(func)           do { } while (0)

#define DBG_RC(rc, func, type, message) do { } while (0)

/* Buffer validation routines */
result_t jdbg_pattern_start_update(uint32_t *start, uint32_t size);

result_t jdbg_pattern_set(mem_desc_h membuf, uint32_t size, uint32_t start);

result_t jdbg_pattern_check(mem_desc_h membuf, uint32_t size, uint32_t start);

#define DBG_RC_W(rc, type, message) DBG_RC(rc, DBG_W, type, message)
#define DBG_RC_I(rc, type, message) DBG_RC(rc, DBG_I, type, message)
#define DBG_RC_V(rc, type, message) DBG_RC(rc, DBG_V, type, message)
#define DBG_RC_X(rc, type, message) DBG_RC(rc, DBG_X, type, message)

#ifdef __cplusplus
}
#endif

#endif

