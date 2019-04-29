#ifndef SSP_ENGINE_HEADER
#define SSP_ENGINE_HEADER
/******************************************************************************
 * MODULE NAME:    lmp_ssp_engine.h
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

 void F1(u_int8 *U,u_int8 *V, u_int8 *X, u_int8 *Z,u_int8* OutPut);
 void F2(u_int8 *W,u_int8 *N1, u_int8 *N2, u_int8 *KeyId, u_int8 *A1, u_int8 *A2,u_int8* OutPut);
 void F3(u_int8 *W,u_int8 *N1, u_int8 *N2, u_int8 *R, u_int8 *IOcap, u_int8 *A1, u_int8 *A2,u_int8* OutPut);
 void G(u_int8 *U,u_int8 *V, u_int8 *X, u_int8 *Y,u_int8* OutPut);
 void F1_Invert(u_int8 *U,u_int8 *V, u_int8 *X, u_int8 *Z,u_int8* outPut);
#endif
