/* ***** BEGIN LICENSE BLOCK *****
; * Version: RCSL 1.0/RPSL 1.0 
; *  
; * Portions Copyright (c) 1995-2002 RealNetworks, Inc. All Rights Reserved. 
; *      
; * The contents of this file, and the files included with this file, are 
; * subject to the current version of the RealNetworks Public Source License 
; * Version 1.0 (the "RPSL") available at 
; * http://www.helixcommunity.org/content/rpsl unless you have licensed 
; * the file under the RealNetworks Community Source License Version 1.0 
; * (the "RCSL") available at http://www.helixcommunity.org/content/rcsl, 
; * in which case the RCSL will apply. You may also obtain the license terms 
; * directly from RealNetworks.  You may not use this file except in 
; * compliance with the RPSL or, if you have a valid RCSL with RealNetworks 
; * applicable to this file, the RCSL.  Please see the applicable RPSL or 
; * RCSL for the rights, obligations and limitations governing use of the 
; * contents of the file.  
; *  
; * This file is part of the Helix DNA Technology. RealNetworks is the 
; * developer of the Original Code and owns the copyrights in the portions 
; * it created. 
; *  
; * This file, and the files included with this file, is distributed and made 
; * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
; * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
; * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
; * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
; * 
; * Technology Compatibility Kit Test Suite(s) Location: 
; *    http://www.helixcommunity.org/content/tck 
; * 
; * Contributor(s): 
; *  
; * ***** END LICENSE BLOCK ***** */ 

/*
	AREA	|.text|, CODE, READONLY
; int xmp3_MULSHIFT32(int x, int y)
	EXPORT	xmp3_MULSHIFT32
*/
/*
.globl xmp3_MULSHIFT32
xmp3_MULSHIFT32
	b.ori      r7,r0,0xffffff8
	b.ori      r3,r0,0xffffff7
	//smull	r2, r0, r1, r0
	b.mulh     r4,r3,r7
	;mov		pc, lr
	b.jr       r9
*/
/*	EXPORT xmp3_FASTABS
xmp3_FASTABS
	;mov		pc, lr
	BX LR
*/
	//END
    .section fastcall,"ax",%progbits

.globl MADD64_ASM
MADD64_ASM  :
	b.mtspr r0,r4,10242;
	b.mtspr r0,r3,10241;
	b.mac r5,r6;
	b.mfspr r4,r0,10242;
	b.mfspr r3,r0,10241;
	b.jr r9

/*
.globl MULSHIFT32_ASM
MULSHIFT32_ASM :
    	b.mulh r3,r3,r4;
	b.jr r9
*/

.globl SAR64_ASM
SAR64_ASM :
	b.addi r6,r5,-0x8
	b.srl  r3,r3,r5
	b.sll  r4,r4,r6
	b.or   r3,r3,r4
	b.jr   r9




