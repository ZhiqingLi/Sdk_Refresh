/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _TYPES_UTIL_H_
#define _TYPES_UTIL_H_

#include <uw_types.h>

/*
 *      Generic comparison and types
 */

#ifndef MIN
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#endif

#ifndef ABS
#define ABS(a)  ((a) < 0 ? (-(a)) : (a))
#endif

#define J_OFFSET_OF(type, member) ((uint32_t)(&((type *)0)->member))
#define J_CONTAINER_OF(ptr, type, member) \
    ((type *)((uint8_t *)(ptr) - J_OFFSET_OF(type, member)))

#define UW_ARRAY_SIZE(a)           (sizeof(a) / sizeof((a)[0]))

#define UW_CONCAT(a,b)   a ## b

#define UW_U16_ZERO_INIT {0, 0}
#define UW_U32_ZERO_INIT {0, 0, 0, 0}

#if CONFIG_BYTE_ORDER == CPU_BIG_ENDIAN
#define J_UINT64_INIT(high, low) {high, low}
#else
#define J_UINT64_INIT(high, low) {low, high}
#endif

#define J_UINT64_ZERO_INIT {0, 0}
#define J_UINT64_IS_NON_ZERO(x) ((x).high || (x).low)
#define J_UINT64_HIGH(x) ((x).high)
#define J_UINT64_LOW(x) ((x).low)
#define J_UINT64_SET(x, h, l) do {  \
    (x).high = (h);                 \
    (x).low  = (l);                 \
} while (0)

#if CONFIG_BYTE_ORDER == CPU_BIG_ENDIAN
#define J_UINT128_INIT(high, low) {                         \
    J_UINT64_INIT(J_UINT64_HIGH(high), J_UINT64_LOW(high)), \
    J_UINT64_INIT(J_UINT64_HIGH(low), J_UINT64_LOW(low))    \
}
#else
#define J_UINT128_INIT(high, low) {                         \
    J_UINT64_INIT(J_UINT64_HIGH(low), J_UINT64_LOW(low)),   \
    J_UINT64_INIT(J_UINT64_HIGH(high), J_UINT64_LOW(high))  \
}
#endif

#define J_UINT128_ZERO_INIT {J_UINT64_INIT(0, 0), J_UINT64_INIT(0, 0)}
#define J_UINT128_IS_NON_ZERO(x) \
    (J_UINT64_IS_NON_ZERO((x).high) || J_UINT64_IS_NON_ZERO((x).low))
#define J_UINT128_HIGH(x) ((x).high)
#define J_UINT128_LOW(x) ((x).low)
#define J_UINT128_SET(x, h, l) do { \
    (x).high = (h);                 \
    (x).low  = (l);                 \
} while (0)

/*
 *      Unaligned access macros.
 *      Used to access PACKED structures and un-aligned integers
 */

#define ua_get_le16(ptr)        UW_CONVERT_LE(get16, ptr, 0)
#define ua_get_le32(ptr)        UW_CONVERT_LE(get32, ptr, 0)
#define ua_set_le16(ptr, val)   UW_CONVERT_LE(set16, ptr, val)
#define ua_set_le32(ptr, val)   UW_CONVERT_LE(set32, ptr, val)

#define ua_get_be16(ptr)        UW_CONVERT_BE(get16, ptr, 0)
#define ua_get_be32(ptr)        UW_CONVERT_BE(get32, ptr, 0)
#define ua_set_be16(ptr, val)   UW_CONVERT_BE(set16, ptr, val)
#define ua_set_be32(ptr, val)   UW_CONVERT_BE(set32, ptr, val)

#define ua_copy16(dst, src)                             \
    do {                                                \
        ((uint8_t *)(dst))[0] = ((uint8_t *)(src))[0];  \
        ((uint8_t *)(dst))[1] = ((uint8_t *)(src))[1];  \
    } while (0)

#define ua_copy32(dst, src)                             \
    do {                                                \
        ((uint8_t *)(dst))[0] = ((uint8_t *)(src))[0];  \
        ((uint8_t *)(dst))[1] = ((uint8_t *)(src))[1];  \
        ((uint8_t *)(dst))[2] = ((uint8_t *)(src))[2];  \
        ((uint8_t *)(dst))[3] = ((uint8_t *)(src))[3];  \
    } while (0)

/*
 *      Endianess handling macros
 */

#define uw_letoh16(val)            uw_le16(val)
#define uw_letoh32(val)            uw_le32(val)
#define uw_htole16(val)            uw_le16(val)
#define uw_htole32(val)            uw_le32(val)

#define uw_betoh16(val)            uw_be16(val)
#define uw_betoh32(val)            uw_be32(val)
#define uw_htobe16(val)            uw_be16(val)
#define uw_htobe32(val)            uw_be32(val)

/*
 *      Time structure handling macros
 */

/* Add "ms" milli-seconds to the time structure */
#define TIME_ADD_MS(time, ms) \
        do { \
        (time)->tv_usec += (ms) * 1000; \
        (time)->tv_sec  += ((time)->tv_usec / (1000000ul)); \
        (time)->tv_usec %= (1000000ul); \
        } while (0);

/* Returns 1 if time "a" is above time "b" */
#define TIME_OVER(a, b) \
        (((a).tv_sec > (b).tv_sec) || \
        (((a).tv_sec == (b).tv_sec) && \
        ((a).tv_usec > (b).tv_usec)) ? \
        1 : 0)

/* Returns 1 if time "a" is above or equal time "b" with millisecond
   resolution */
#define TIME_EXPIRED_MS(a, b) \
        ((TIME_DIFF(a, b) > 0) ? 0 : 1)

/* Returns the time difference between "start" and "end" in milli-seconds */
#define TIME_DIFF(start, end) \
        (((end).tv_sec - (start).tv_sec) * 1000 + \
        ((end).tv_usec - (start).tv_usec) / 1000)

/* Returns the time difference between "start" and "end" in micro-seconds */
#define TIME_DIFF_US(start, end) \
        (((end).tv_sec - (start).tv_sec) * 1000000 + \
        (end).tv_usec - (start).tv_usec)

#define TIME_GET_MS(time) \
        (((time).tv_sec) * 1000 + ((time).tv_usec) / 1000)




/* ======================================
 *              IMPLEMENTATIONS
 * ====================================== */

/*
 *      Unaligned macros implementation
 */

#define UW_CONVERT_BE(func, ptr, val)  \
    uw_packed_##func(ptr, (val), 1, 0, 3, 2, 1, 0)

#define UW_CONVERT_LE(func, ptr, val)  \
    uw_packed_##func(ptr, (val), 0, 1, 0, 1, 2, 3)

#define UW_PGET(ptr, byte)        (((uint8_t *)(ptr))[byte])
#define UW_PSET(ptr, byte, val)    ((uint8_t *)(ptr))[byte] = (uint8_t)(val)

#define uw_packed_get16(ptr, val, a0, a1, b0, b1, b2, b3) \
    ((uint16_t)((((uint16_t)UW_PGET(ptr, a0)) <<  0) | \
     (((uint16_t)UW_PGET(ptr, a1)) <<  8)))

#define uw_packed_get32(ptr, val, a0, a1, b0, b1, b2, b3) \
    ((((uint32_t)UW_PGET(ptr, b0)) <<  0) | \
     (((uint32_t)UW_PGET(ptr, b1)) <<  8) | \
     (((uint32_t)UW_PGET(ptr, b2)) << 16) | \
     (((uint32_t)UW_PGET(ptr, b3)) << 24))

#define uw_packed_set16(ptr, val, a0, a1, b0, b1, b2, b3) \
    do { \
        UW_PSET(ptr, a0, (val) >>  0); \
        UW_PSET(ptr, a1, (val) >>  8); \
    } while (0)

#define uw_packed_set32(ptr, val, a0, a1, b0, b1, b2, b3) \
    do { \
        UW_PSET(ptr, b0, (val) >>  0); \
        UW_PSET(ptr, b1, (val) >>  8); \
        UW_PSET(ptr, b2, (val) >> 16); \
        UW_PSET(ptr, b3, (val) >> 24); \
    } while (0)

/*
 *      Endianess handling macros implementation
 */
#if CONFIG_BYTE_ORDER == CPU_BIG_ENDIAN
#define UW_SWAP_BE     UW_NO_SWAP
#define UW_SWAP_LE     UW_SWAP

#define ua_get_16(ptr)          ua_get_be16(ptr)
#define ua_get_32(ptr)          ua_get_be32(ptr)
#define ua_set_16(ptr, val)     ua_set_be16(ptr, val)
#define ua_set_32(ptr, val)     ua_set_be32(ptr, val)

#elif CONFIG_BYTE_ORDER == CPU_LITTLE_ENDIAN
#define UW_SWAP_BE     UW_SWAP
#define UW_SWAP_LE     UW_NO_SWAP

#define ua_get_16(ptr)          ua_get_le16(ptr)
#define ua_get_32(ptr)          ua_get_le32(ptr)
#define ua_set_16(ptr, val)     ua_set_le16(ptr, val)
#define ua_set_32(ptr, val)     ua_set_le32(ptr, val)

#else
# error "Unknown byte order"
#endif

#define uw_le16(val)               UW_SWAP_LE(((uint16_t)(val)), 16)
#define uw_le32(val)               UW_SWAP_LE(((uint32_t)(val)), 32)
#define uw_be16(val)               UW_SWAP_BE(((uint16_t)(val)), 16)
#define uw_be32(val)               UW_SWAP_BE(((uint32_t)(val)), 32)

#define UW_SWAP(val, bits)         uw_bswap##bits(val)
#define UW_NO_SWAP(val, bits)      (val)

#define uw_bswap16(val) \
        (((val) >> 8) | (((val) << 8) & 0xff00))
#define uw_bswap32(val) \
        (((val) >> 24) | (((val) >> 8) & 0x0000ff00) | \
        (((val) << 8) & 0x00ff0000) | ((val) << 24))

/*
 *      Backwards compatibility macros
 */

#ifndef OS_ENDIANITY_MACROS

#define UGETW(ptr)              ua_get_le16(ptr)
#define UGETDW(ptr)             ua_get_le32(ptr)
#define USETW(ptr,v)            ua_set_le16(ptr, v)
#define USETDW(ptr,v)           ua_set_le32(ptr, v)

#define USETW2(w,h,l)           ((w)[0] = (uint8_t)(l), (w)[1] = (uint8_t)(h))

#define ua_be16toh(ptr)         ua_get_be16(ptr)
#define ua_be32toh(ptr)         ua_get_be32(ptr)

#define ua_htobe16(ptr,v)       ua_set_be16(ptr, v)
#define ua_htobe32(ptr,v)       ua_set_be32(ptr, v)

#define ua_le16toh(ptr)         ua_get_le16(ptr)
#define ua_le32toh(ptr)         ua_get_le32(ptr)

#define ua_htole16(ptr,v)       ua_set_le16(ptr, v)
#define ua_htole32(ptr,v)       ua_set_le32(ptr, v)

#define htobe16_t(ptr, x)       ua_set_be16(ptr, x)
#define htobe32_t(ptr, x)       ua_set_be32(ptr, x)

#define htole16_t(ptr, x)       ua_set_le16(ptr, x)
#define htole32_t(ptr, x)       ua_set_le32(ptr, x)

#define be16toh_t(ptr)          ua_get_be16(ptr)
#define be32toh_t(ptr)          ua_get_be32(ptr)

#define bswap16                 uw_bswap16
#define bswap32                 uw_bswap32

#define letoh16(val)            uw_letoh16(val)
#define letoh32(val)            uw_letoh32(val)

#define betoh16(val)            uw_betoh16(val)
#define betoh32(val)            uw_betoh32(val)

#ifndef ENDIANITY_MACRO_DEFINED
#define le16toh(x)              letoh16(x)
#define le32toh(x)              letoh32(x)
#define be16toh(x)              betoh16(x)
#define be32toh(x)              betoh32(x)
#define htole16(val)            uw_htole16(val)
#define htole32(val)            uw_htole32(val)
#define htobe16(val)            uw_htobe16(val)
#define htobe32(val)            uw_htobe32(val)
#endif

#endif
                                      
#endif

