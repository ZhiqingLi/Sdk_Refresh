#ifndef _PARTHUS_LMP_SECURITY_ENGINE_
#define _PARTHUS_LMP_SECURITY_ENGINE_

/******************************************************************************
 * MODULE NAME:   lmp_security_engine.h 
 * PROJECT CODE:  BlueStream
 * DESCRIPTION:   Header File for the LMP Security Engine 
 * MAINTAINER:    Daire McNamara
 * CREATION DATE: 24 December 2000
 *
 * SOURCE CONTROL: $Id: lmp_sec_engine.h,v 1.8 2009/07/15 18:55:32 garyf Exp $
 *
 * LICENSE:
 *   This source code is copyright (c) 2000-2004 Ceva Inc.
 *   All rights reserved.
 *
 ******************************************************************************/

#include "sys_types.h"
#include "lmp_types.h"
#include "lmp_sec_types.h"
#include "lmp_ecc.h"

#define NO_ROUNDS 8
#define MAX_DEGREE_KC 127
#define BIT31 0x80000000UL
#define MAX_KEY_SIZE 16
#define MAX_KEY_REG_SIZE 4

typedef enum {AR_NORMAL, AR_MOD} saferplustag;

/* Used for Reduction of Encryption Key (Kc) */
typedef struct {
  u_int16 value;
  u_int8 degree;
}polynomial;

//#define MAX_DIGITS 60
//#define MAX_OCTETS 30
void LM_SecEng_Authenticate_E1(u_int8* p_randinput, t_link_key key,
        const t_bd_addr* p_bd_addr, t_sres sres, t_aco aco);
void LM_SecEng_Generate_Key_E21(t_link_key randinput,
        const t_bd_addr* p_bd_addr, t_link_key output_key);
void LM_SecEng_Generate_Key_E22(const t_rand randinput, t_pin_code PIN,
        u_int8 lengthPIN, const t_bd_addr* p_bd_addr, t_link_key outputkey);
void LM_SecEng_Generate_Key_E3(t_rand encr_rand, t_cof cof,
        t_link_key current_link_key, t_rand encr_key);
void LM_SecEng_Generate_Kc_Prime(u_int8 * kc, u_int8 kc_prime_len,
        u_int8 *kc_prime);


//void ECC_Point_Multiplication(bigHex* pk,ECC_Point* pPointP, ECC_Point* pPointQ);
//int notEqual(bigHex *bigHexA,bigHex *bigHexB);
#endif

