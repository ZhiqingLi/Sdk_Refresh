////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2011, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:endian.h
//
//		Description	:
//					supply a kind of method to get or set basic data type for 
//						different platform endian layout.pls,note that only 
//						char,short,long conversion available.
//		Changelog	:	
//					2012-02-21	Contributed by Robert
//					2014-10-24  rename the type conversation
///////////////////////////////////////////////////////////////////////////////

/**
 * @addtogroup MVUtils
 * @{
 * @defgroup endian endian
 * @{
 */


#ifndef		__ENDIAN_H__
#define		__ENDIAN_H__
#include	"type.h"
/*
 * define the architiecture half/word operation
 */
#define		GET_INT8(ptr)		(((int8_t*)(ptr))[0])
#define		GET_UINT8(ptr)		(((uint8_t*)(ptr))[0])
#define		PUT_INT8(ch,ptr)	(((int8_t*)(ptr))[0] = (int8_t)(ch))
#define		PUT_UINT8(ch,ptr)	(((uint8_t*)(ptr))[0] = (uint8_t)(ch))

#if (BYTE_ORDER == BYTE_ORDER_LITTLE)
#define		GET_INT16(ptr)\
				((((int8_t*)ptr)[0] & 0xff) + ((((int8_t*)ptr)[1] << 8) & 0xff00))

#define		GET_UINT16(ptr)\
				((((uint8_t*)ptr)[0] & 0xff) + ((((uint8_t*)ptr)[1] << 8) & 0xff00))

#define		GET_INT32(ptr)\
				((((int8_t*)ptr)[0] & 0xff) + ((((int8_t*)ptr)[1] << 8) & 0xff00) +\
					((((int8_t*)ptr)[2] << 16) & 0xff0000) + ((((int8_t*)ptr)[3] << 24) & 0xff000000))

#define		GET_UINT32(ptr)\
				((((uint8_t*)ptr)[0] & 0xff) + ((((uint8_t*)ptr)[1] << 8) & 0xff00) +\
					((((uint8_t*)ptr)[2] << 16) & 0xff0000) + ((((uint8_t*)ptr)[3] << 24) & 0xff000000))

#define		PUT_INT16(w,ptr)\
				(((int8_t*)ptr)[0] = ((int16_t)w) & 0xff ,((int8_t*)ptr)[1] = (((int16_t)w) >> 8) & 0xff)

#define		PUT_UINT16(w,ptr)\
				(((uint8_t*)ptr)[0] = ((uint16_t)w) & 0xff ,\
							((uint8_t*)ptr)[1] = (((uint16_t)w) >> 8) & 0xff)

#define		PUT_INT32(dw,ptr)\
				(((int8_t*)ptr)[0] = ((int32_t)dw) & 0xff , ((int8_t*)ptr)[1] = (((int32_t)dw) >> 8) & 0xff,\
				  ((int8_t*)ptr)[2] = (((int32_t)dw) >> 16) & 0xff , ((int8_t*)ptr)[3] = (((int32_t)dw) >> 24) & 0xff)

#define		PUT_UINT32(dw,ptr)\
				(((uint8_t*)ptr)[0] = ((uint32_t)dw) & 0xff ,\
						((uint8_t*)ptr)[1] = (((uint32_t)dw) >> 8) & 0xff,\
								((uint8_t*)ptr)[2] = (((uint32_t)dw) >> 16) & 0xff ,\
											((uint8_t*)ptr)[3] = (((uint32_t)dw) >> 24) & 0xff)

#else /* //BYTE_ORDER == BIG_ENDIAN_EX */

#define		INT16(ptr)\
				((((int8_t*)ptr)[1] & 0xff) + ((((int8_t*)ptr)[0] << 8) & 0xff00))

#define		GET_UINT16(ptr)\
				((((uint8_t*)ptr)[1] & 0xff) + ((((uint8_t*)ptr)[0] << 8) & 0xff00))

#define		GET_INT32(ptr)\
				((((int8_t*)ptr)[3] & 0xff) + ((((int8_t*)ptr)[2] << 8) & 0xff00) +\
					((((int8_t*)ptr)[1] << 16) & 0xff0000) + ((((int8_t*)ptr)[0] << 24) & 0xff000000))

#define		GET_UINT32(ptr)\
				((((uint8_t*)ptr)[3] & 0xff) + ((((uint8_t*)ptr)[2] << 8) & 0xff00) +\
					((((uint8_t*)ptr)[1] << 16) & 0xff0000) + ((((uint8_t*)ptr)[0] << 24) & 0xff000000))

#define		PUT_INT16(w,ptr)\
				(((int8_t*)ptr)[1] = ((int16_t)w) & 0xff , ((int8_t*)ptr)[0] = (((int16_t)w) >> 8) & 0xff)

#define		PUT_UINT16(w,ptr)\
				(((uint8_t*)ptr)[1] = ((uint16_t)w) & 0xff ,\
							((uint8_t*)ptr)[0] = (((uint16_t)w) >> 8) & 0xff)

#define		PUT_INT32(dw,ptr)\
				(((int8_t*)ptr)[3] = ((int32_t)dw) & 0xff , ((int8_t*)ptr)[2] = (((int32_t)dw) >> 8) & 0xff,\
				  ((int8_t*)ptr)[1] = (((int32_t)dw) >> 16) & 0xff , ((int8_t*)(ptr))[0] = (int32_t(dw) >> 24) & 0xff)

#define		PUT_UINT32(dw,ptr)\
				(((uint8_t*)ptr)[3] = ((uint32_t)dw) & 0xff ,\
						((uint8_t*)ptr)[2] = (((uint32_t)dw) >> 8) & 0xff,\
								((uint8_t*)ptr)[1] = (((uint32_t)dw) >> 16) & 0xff ,\
											((uint8_t*)ptr)[0] = (((uint32_t)dw) >> 24) & 0xff)
#endif //BYTE_ORDER == BYTE_ORDER_LITTLE 

#endif	//_ENDIAN_H_
