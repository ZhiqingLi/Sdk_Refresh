#ifndef ECC_HEADER
#define ECC_HEADER
/******************************************************************************
 * MODULE NAME:    lmp_ecc.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:   
 * MAINTAINER:     Gary Fleming
 * CREATION DATE:        
 *
 * SOURCE CONTROL: $ $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2009 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 ******************************************************************************/

#include "lmp_acl_connection.h"
#define u_int32 unsigned int
#define u_int16 unsigned short int
#define u_int8 unsigned char

#define MAX_DIGITS 56
#define MAX_OCTETS 28

typedef struct bigHex 
{
	u_int32 num[MAX_OCTETS/4];
	u_int32  len;
	u_int32  sign;
} bigHex;

typedef struct veryBigHex 
{
	u_int32 num[MAX_OCTETS/2];
	u_int32  len;
	u_int32  sign;
} veryBigHex;

typedef struct ECC_Point
{
	bigHex x;
	bigHex y;
} ECC_Point;

typedef struct ECC_Jacobian_Point
{
	bigHex x;
	bigHex y;
	bigHex z;
} ECC_Jacobian_Point;


void ECC_Point_Multiplication(const bigHex* pk,const ECC_Point* pPointP,t_lmp_link* p_link,boolean blocking);
int notEqual(const bigHex *bigHexA,const bigHex *bigHexB);
__INLINE__ void GF_Point_Copy(const ECC_Point *source,ECC_Point *destination);

__INLINE__ void GF_Jacobian_Point_Copy(const ECC_Jacobian_Point *source,ECC_Jacobian_Point *destination);

void LMecc_Generate_ECC_Key(const u_int8* secret_key, const u_int8* public_key_x,
					   const u_int8* public_key_y,t_lmp_link* p_link,boolean blocking);

int LMecc_isValidSecretKey(u_int8* secretKey);

#endif
