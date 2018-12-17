/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：此头文件声明的函数用于对单个字符进行分类和转换操作，所有函数或宏
 *			需要一个int类型的自变量，其值介于0到255之间，或者为EOF.
 * 作者：liminxian
 ********************************************************************************/
/*!\cond LIBC*/
/*!
 * \defgroup libc_char 字符分类和转换
 */
/*!\endcond*/

#ifndef __OS_CTYPE_H__
#define __OS_CTYPE_H__

/*
 * NOTE! This ctype does not handle EOF like the standard C
 * library is required to.
 */

/*!
 * \brief
 *      变元c为字母或数字时，函数返回非0值，否则返回0。
 * \li  c: int类型自变量
 * \ingroup libc_char
 */
#define isalnum(c) (((((c) | 0x20) - 'a') < 26U) || (((c) - '0') < 10U))

/*!
 * \brief
 *      变元c为字母时，函数返回非0值，否则返回0。
 * \li  c: int 类型自变量
 * \ingroup libc_char
 */
#define isalpha(c) ((('a' <= (c)) && ((c) <= 'z')) || (('A' <= (c)) && ((c) <= 'Z')))

/*!
 * \brief
 *      变元c为ascii字符时，函数返回非0值，否则返回0。
 * \li  c: int 类型自变量
 * \ingroup libc_char
 */
#define isascii(c) (((c) & ~0x7f) == 0)

/*!
 * \brief
 *      转换变量c为ascii字符，函数返回转换后的值。
 * \li  c: int 类型自变量
 * \ingroup libc_char
 */
#define toascii(c) ((c) & 0x7f)

/*!
 * \brief
 *      变元c为空格（含制表符）时，函数返回非0值，否则返回0。
 * \li  c: int 类型自变量
 * \ingroup libc_char
 */
#define isblank(c) (((c) == ' ') || ((c) == '\t') || ((c) == '\v'))

/*!
 * \brief
 *      变元c是控制字符时，函数返回非0，否则返回0。
 * \li  c: int 类型自变量
 * \ingroup libc_char
 */
#define iscntrl(c) (((c) < 32) || ((c) == 127))

/*!
 * \brief
 *      变元是十进制数字时，函数返回非0值，否则返回0。
 * \li  c: int 类型自变量
 * \ingroup libc_char
 */
#define isdigit(c) (('0' <= (c)) && ((c) <= '9'))

/*!
 * \brief
 *      变元c为除空格之外的任何可打印字符，则函数返回非0值，否则返回0。
 * \li  c: int 类型自变量
 * \ingroup libc_char
 */
#define isgraph(c) ('!' <= (c) && (c) <= '~')

/*!
 * \brief
 *      变量c为小写字母时，函数返回非0值，否则返回0。
 * \li  c: int 类型自变量
 * \ingroup libc_char
 */
#define islower(c) (('a' <= (c)) && ((c) <= 'z'))

/*!
 * \brief
 *      c为大写字母时，返回其对应的小写字母；否则返回c。
 * \li  c: int 类型自变量
 * \ingroup libc_char
 */
#define tolower(c) (isupper(c) ? (((c) - 'A') + 'a') : (c))

/*!
 * \brief
 *      变元c是可打印字符(含空格)，函数返回非0值，否则返回0。
 * \li  c: int 类型自变量
 * \ingroup libc_char
 */
#define isprint(c) (' ' <= (c) && (c) <= '~')

/*!
 * \brief
 *      变元c为空白字符，函数返回非0值，否则返回0。
 * \li  c: int 类型自变量
 * \ingroup libc_char
 */
#define isspace(c) (((((unsigned int)(c)) - '\t') < 5U) || ((c) == ' '))

/*!
 * \brief
 *      变元c为标点字符，函数返回非0值，否则返回0。
 * \li  c: int 类型自变量
 * \ingroup libc_char
 */
#define ispunct(c) (isprint(c) && !isalnum(c) && !isspace(c))

/*!
 * \brief
 *      变元c为大写字母时，函数返回非0值，否则返回0。
 * \li  c: int 类型自变量
 * \ingroup libc_char
 */
#define isupper(c) (('A' <= (c)) && ((c) <= 'Z'))

/*!
 * \brief
 *      当c为小写字母时，返回其对应的大写字母；否则返回c。
 * \li  c: int 类型自变量
 * \ingroup libc_char
 */
#define toupper(c) (islower(c) ? (((c) - 'a') + 'A') : (c))

/*!
 * \brief
 *      变元c为十六进制数字时，函数返回非0，否则返回0。
 * \li  c: int 类型自变量
 * \ingroup libc_char
 */
#define isxdigit(c)	((('0' <= (c)) && ((c) <= '9')) || (('a' <= (c)) && ((c) <= 'f')) \
                || (('A' <= (c)) && ((c) <= 'F')))

/*!
 * \brief
 *      转换c为小写字母，应用程序保证c为大写字母，等价于tolower()
 * \li  c: int 类型自变量
 * \ingroup libc_char
 */
#define _tolower(c) ((c) - 'A' + 'a')

/*!
 * \brief
 *      转换c为大写字母，应用程序保证c为小写字母，等价于toupper()
 * \li  c: int 类型自变量
 * \ingroup libc_char
 */
#define _toupper(c) ((c) - 'a' + 'A')

#endif/*end of __OS_CTYPE_H__*/
