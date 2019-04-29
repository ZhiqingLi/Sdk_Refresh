#define _ASM_SRC_
#include "spr_defs.h"
//#include "app_asm_sw_eq.h"
#if 1
	.section .data

//4618=-11dB
#define TotalEQnum 11
#define EQ0 1
#define EQ0A0 0
#define EQ0A1 0
#define EQ0B0 0x100000
#define EQ0B1 0
#define EQ0B2 0

#define EQ1 1
#define EQ1A0 0
#define EQ1A1 0
#define EQ1B0 0x100000
#define EQ1B1 0
#define EQ1B2 0

#define EQ2 1
#define EQ2A0 0
#define EQ2A1 0
#define EQ2B0 0x100000
#define EQ2B1 0
#define EQ2B2 0

#define EQ3 1
#define EQ3A0 0
#define EQ3A1 0
#define EQ3B0 0x100000
#define EQ3B1 0
#define EQ3B2 0

#define EQ4 1
#define EQ4A0 0
#define EQ4A1 0
#define EQ4B0 0x100000
#define EQ4B1 0
#define EQ4B2 0

#define EQ5 1
#define EQ5A0 0
#define EQ5A1 0
#define EQ5B0 0x100000
#define EQ5B1 0
#define EQ5B2 0

#define EQ6 1
#define EQ6A0 0
#define EQ6A1 0
#define EQ6B0 0x100000
#define EQ6B1 0
#define EQ6B2 0

#define EQ7A0 0
#define EQ7A1 0
#define EQ7B0 0x100000
#define EQ7B1 0
#define EQ7B2 0

#define EQ8A0 0
#define EQ8A1 0
#define EQ8B0 0x100000
#define EQ8B1 0
#define EQ8B2 0

#define EQ9A0 0
#define EQ9A1 0
#define EQ9B0 0x100000
#define EQ9B1 0
#define EQ9B2 0

#define EQ10A0 0
#define EQ10A1 0
#define EQ10B0 0x100000
#define EQ10B1 0
#define EQ10B2 0


//bypass
#define EQ11A0 0
#define EQ11A1 0
#define EQ11B0 0x100000
#define EQ11B1 0
#define EQ11B2 0

#define mEQ(index,para) EQ##index##para
#define nEQ(index,para) -(EQ##index##para)

.globl tbl_eq_coef
	tbl_eq_coef:
		.word nEQ(0,A0),nEQ(0,A1),mEQ(0,B0),mEQ(0,B1),mEQ(0,B2)
		.word nEQ(1,A0),nEQ(1,A1),mEQ(1,B0),mEQ(1,B1),mEQ(1,B2)
		.word nEQ(2,A0),nEQ(2,A1),mEQ(2,B0),mEQ(2,B1),mEQ(2,B2)
		.word nEQ(3,A0),nEQ(3,A1),mEQ(3,B0),mEQ(3,B1),mEQ(3,B2)
		.word nEQ(4,A0),nEQ(4,A1),mEQ(4,B0),mEQ(4,B1),mEQ(4,B2)
		.word nEQ(5,A0),nEQ(5,A1),mEQ(5,B0),mEQ(5,B1),mEQ(5,B2)
		.word nEQ(6,A0),nEQ(6,A1),mEQ(6,B0),mEQ(6,B1),mEQ(6,B2)
		.word nEQ(7,A0),nEQ(7,A1),mEQ(7,B0),mEQ(7,B1),mEQ(7,B2)
		.word nEQ(8,A0),nEQ(8,A1),mEQ(8,B0),mEQ(8,B1),mEQ(8,B2)
		.word nEQ(9,A0),nEQ(9,A1),mEQ(9,B0),mEQ(9,B1),mEQ(9,B2)

.globl s_eq_last_XY
	s_eq_last_XY:
		.word 0,0,0,0 //L_eq0:X(-1),X(0),Y(-1),Y(0)
		.word 0,0,0,0 //R_eq0:X(-1),X(0),Y(-1),Y(0)
		.word 0,0,0,0 //L_eq1:X(-1),X(0),Y(-1),Y(0)
		.word 0,0,0,0 //R_eq1:X(-1),X(0),Y(-1),Y(0)
		.word 0,0,0,0 //L_eq2:X(-1),X(0),Y(-1),Y(0)
		.word 0,0,0,0 //R_eq2:X(-1),X(0),Y(-1),Y(0)
		.word 0,0,0,0 //L_eq3:X(-1),X(0),Y(-1),Y(0)
		.word 0,0,0,0 //R_eq3:X(-1),X(0),Y(-1),Y(0)
		.word 0,0,0,0 //L_eq4:X(-1),X(0),Y(-1),Y(0)
		.word 0,0,0,0 //R_eq4:X(-1),X(0),Y(-1),Y(0)
		.word 0,0,0,0 //L_eq5:X(-1),X(0),Y(-1),Y(0)
		.word 0,0,0,0 //R_eq5:X(-1),X(0),Y(-1),Y(0)
		.word 0,0,0,0 //L_eq6:X(-1),X(0),Y(-1),Y(0)
		.word 0,0,0,0 //R_eq6:X(-1),X(0),Y(-1),Y(0)
		.word 0,0,0,0 //L_eq7:X(-1),X(0),Y(-1),Y(0)
		.word 0,0,0,0 //R_eq7:X(-1),X(0),Y(-1),Y(0)
		.word 0,0,0,0 //L_eq8:X(-1),X(0),Y(-1),Y(0)
		.word 0,0,0,0 //R_eq8:X(-1),X(0),Y(-1),Y(0)
		.word 0,0,0,0 //L_eq9:X(-1),X(0),Y(-1),Y(0)
		.word 0,0,0,0 //R_eq9:X(-1),X(0),Y(-1),Y(0)

        .section .text
//		b.mtspr r0,r13,SPR_MACLO
//		b.mtspr r0,r12,SPR_MACHI
#define fseq_r_input r3
#define fseq_r_input_tmp r3
#define fseq_r_output r4
#define fseq_r_len r5
#define fseq_r_eq_times r6
#define fseq_r_gain r7
#define fseq_r_index r7
#define fseq_r_mono r8

#define fseq_r_input_Ldata r9
#define fseq_r_input_Rdata r10

#define fseq_r_coef_a0 r11
#define fseq_r_coef_a1 r12
#define fseq_r_coef_b0 r13
#define fseq_r_coef_b1 r14
#define fseq_r_coef_b2 r15
//L_X0~R_Y1
#define fseq_r_L_X0 r16
#define fseq_r_L_X1 r17
//#define fseq_r_L_X2 r18
#define fseq_r_L_Y0 r18
#define fseq_r_L_Y1 r19
#define fseq_r_R_X0 r20
#define fseq_r_R_X1 r21
//#define fseq_r_R_X2 r23
#define fseq_r_R_Y0 r22
#define fseq_r_R_Y1 r23

#define fseq_r_0x80000 r24
#define fseq_r_len_i r25
#define fseq_r_output_tmp r26
#define fseq_r_16bit_max r28
#define fseq_r_16bit_min r29
#define fseq_r_coef_tbl r30
#define fseq_r_lastXY_ptr r31

#define c_mac_rshift 20
#define c_mac_result_GT_32bit 1
//void func_sw_eq((short *)input,(short *)output,int len,int eq_cnt);//len: 1=L+R=4bytes
.globl func_sw_eq
func_sw_eq:
		b.entri 15,(8*4) //8*4:r24~r31
		b.msw 0(r1),r24,3;
		//b.addi fseq_r_rshift,r0,c_mac_rshift
		b.addi fseq_r_16bit_max,r0,32767
		b.addi fseq_r_16bit_min,r0,-32767
		b.addi fseq_r_0x80000,r0,0x80000

		b.muli fseq_r_coef_tbl,fseq_r_index,4*5
		b.addi fseq_r_coef_tbl,fseq_r_coef_tbl,tbl_eq_coef
		b.muli fseq_r_lastXY_ptr,fseq_r_index,4*4*2
		b.addi fseq_r_lastXY_ptr,fseq_r_lastXY_ptr,s_eq_last_XY
		//b.mov fseq_r_input_tmp,fseq_r_input

	f_sweq_load_coef:
		//load last XY
        b.mlwz fseq_r_L_X0,0(fseq_r_lastXY_ptr),3 //load 8 para
		//load eq0_coef
        b.mlwz fseq_r_coef_a0,0(fseq_r_coef_tbl),0 //load 2 para
        b.mlwz fseq_r_coef_b0,8(fseq_r_coef_tbl),1 //load 3 para
		b.mov fseq_r_output_tmp,fseq_r_output
		b.mov fseq_r_len_i,r0
		f_sweq_load_sample:
		//--------main loop--------------------
			//load L/R sample
			//b.lwz fseq_r_input_Ldata,0(fseq_r_input_tmp)
			//b.rori fseq_r_input_Rdata,fseq_r_input_Ldata,16
			//b.exths fseq_r_input_Ldata,fseq_r_input_Ldata
			//b.exths fseq_r_input_Rdata,fseq_r_input_Rdata
			b.mlwz fseq_r_input_Ldata,0(fseq_r_input_tmp),0 //load 2 para
		//f_sweq_calc_one_sample:
			b.mtspr r0,fseq_r_0x80000,SPR_MACLO
			b.mtspr r0,r0,SPR_MACHI

			b.macs fseq_r_L_X0,fseq_r_coef_b2
			b.macs fseq_r_L_X1,fseq_r_coef_b1
			b.macs fseq_r_input_Ldata,fseq_r_coef_b0
			b.macs fseq_r_L_Y0,fseq_r_coef_a1
			b.macs fseq_r_L_Y1,fseq_r_coef_a0
			b.mov fseq_r_L_Y0,fseq_r_L_Y1
			b.mov fseq_r_L_X0,fseq_r_L_X1
			b.mov fseq_r_L_X1,fseq_r_input_Ldata


		#if (c_mac_result_GT_32bit == 1)
			//mac result more than 32bit:
			b.mfspr fseq_r_L_Y1,r0,SPR_MACHI
			b.mfspr fseq_r_input_Ldata,r0,SPR_MACLO
			b.srli fseq_r_input_Ldata,fseq_r_input_Ldata,c_mac_rshift
			b.slli fseq_r_L_Y1,fseq_r_L_Y1,(32-c_mac_rshift)
			b.or fseq_r_L_Y1,fseq_r_L_Y1,fseq_r_input_Ldata
		#endif
	//	#if (c_mac_result_GT_32bit == 0)
	//		//mac result le than 32bit:
	//		b.mfspr fseq_r_L_Y1,r0,SPR_MACLO
	//		b.sra fseq_r_L_Y1,fseq_r_L_Y1,fseq_r_rshift
	//		b.mov fseq_r_input_Ldata,fseq_r_L_Y1
	//	#endif
			//-------------------------------------
			//calculate R_eq0------------------
			//clr mac
			b.mtspr r0,fseq_r_0x80000,SPR_MACLO
			b.mtspr r0,r0,SPR_MACHI
			b.macs fseq_r_R_X0,fseq_r_coef_b2
			b.macs fseq_r_R_X1,fseq_r_coef_b1
			b.macs fseq_r_input_Rdata,fseq_r_coef_b0
			b.macs fseq_r_R_Y0,fseq_r_coef_a1
			b.macs fseq_r_R_Y1,fseq_r_coef_a0
			b.mov fseq_r_R_Y0,fseq_r_R_Y1
			b.mov fseq_r_R_X0,fseq_r_R_X1
			b.mov fseq_r_R_X1,fseq_r_input_Rdata

			b.mfspr fseq_r_R_Y1,r0,SPR_MACHI
			b.mfspr fseq_r_input_Rdata,r0,SPR_MACLO
			b.srli fseq_r_input_Rdata,fseq_r_input_Rdata,c_mac_rshift
			b.slli fseq_r_R_Y1,fseq_r_R_Y1,(32-c_mac_rshift)
			b.or fseq_r_R_Y1,fseq_r_R_Y1,fseq_r_input_Rdata

			//-----------------------------------------------------
			b.sw 0(fseq_r_output_tmp),fseq_r_L_Y1
			b.sw 4(fseq_r_output_tmp),fseq_r_R_Y1

			b.addi fseq_r_input_tmp,fseq_r_input_tmp,4*2
			b.addi fseq_r_output_tmp,fseq_r_output_tmp,4*2
			b.addi fseq_r_len_i,fseq_r_len_i,1
			b.bne fseq_r_len_i,fseq_r_len,f_sweq_load_sample

			//store last XY
	        b.msw 0(fseq_r_lastXY_ptr),fseq_r_L_X0,3 //store 8 para
			b.addi fseq_r_lastXY_ptr,fseq_r_lastXY_ptr,4*4*2
			b.addi fseq_r_coef_tbl,fseq_r_coef_tbl,4*5
			b.addi fseq_r_eq_times,fseq_r_eq_times,-1
			b.mov fseq_r_input_tmp,fseq_r_output //use output_ptr as input_ptr
			b.bnei fseq_r_eq_times,0,f_sweq_load_coef
/*
		//b.mov fseq_r_input_tmp,fseq_r_output
		b.slli fseq_r_len_i,fseq_r_len,1
		f_sweq_limit:
			b.lwz fseq_r_input_Ldata,0(fseq_r_input_tmp)
			//limit
			b.blts fseq_r_input_Ldata,fseq_r_16bit_max,1f
			b.mov fseq_r_input_Ldata,fseq_r_16bit_max
			b.sw 0(fseq_r_input_tmp),fseq_r_input_Ldata
			1:
			b.bgts fseq_r_input_Ldata,fseq_r_16bit_min,1f
			b.mov fseq_r_input_Ldata,fseq_r_16bit_min
			b.sw 0(fseq_r_input_tmp),fseq_r_input_Ldata
			1:
			b.addi fseq_r_input_tmp,fseq_r_input_tmp,4
			b.addi fseq_r_len_i,fseq_r_len_i,-1
			b.bnei fseq_r_len_i,0,f_sweq_limit
// */
		b.mlwz r24,0(r1),3
		b.reti 15,(8*4)
	#if 0
.globl func_sw_eq_mono
func_sw_eq_mono:
		b.entri 15,(8*4) //8*4:r24~r31
		b.msw 0(r1),r24,3;
		//b.addi fseq_r_rshift,r0,c_mac_rshift
		b.addi fseq_r_16bit_max,r0,32767
		b.addi fseq_r_16bit_min,r0,-32767
		b.addi fseq_r_0x80000,r0,0x80000

		b.muli fseq_r_coef_tbl,fseq_r_index,4*5
		b.addi fseq_r_coef_tbl,fseq_r_coef_tbl,tbl_eq_coef
		b.muli fseq_r_lastXY_ptr,fseq_r_index,4*4*2
		b.addi fseq_r_lastXY_ptr,fseq_r_lastXY_ptr,s_eq_last_XY
		//b.mov fseq_r_input_tmp,fseq_r_input

	f_sweq_load_coef_mono:
		//load last XY
        b.mlwz fseq_r_L_X0,0(fseq_r_lastXY_ptr),3 //load 8 para
		//load eq0_coef
        b.mlwz fseq_r_coef_a0,0(fseq_r_coef_tbl),0 //load 2 para
        b.mlwz fseq_r_coef_b0,8(fseq_r_coef_tbl),1 //load 3 para
		b.mov fseq_r_output_tmp,fseq_r_output
		b.mov fseq_r_len_i,r0
		f_sweq_load_sample_mono:
		//--------main loop--------------------
			//load L/R sample
	        b.lwz fseq_r_input_Ldata,0(fseq_r_input_tmp) //load 1 para
			b.mtspr r0,fseq_r_0x80000,SPR_MACLO
			b.mtspr r0,r0,SPR_MACHI

			b.macs fseq_r_L_X0,fseq_r_coef_b2
			b.macs fseq_r_L_X1,fseq_r_coef_b1
			b.macs fseq_r_input_Ldata,fseq_r_coef_b0
			b.macs fseq_r_L_Y0,fseq_r_coef_a1
			b.macs fseq_r_L_Y1,fseq_r_coef_a0
			b.mov fseq_r_L_Y0,fseq_r_L_Y1
			b.mov fseq_r_L_X0,fseq_r_L_X1
			b.mov fseq_r_L_X1,fseq_r_input_Ldata


			//mac result more than 32bit:
			b.mfspr fseq_r_L_Y1,r0,SPR_MACHI
			b.mfspr fseq_r_input_Ldata,r0,SPR_MACLO
			b.srli fseq_r_input_Ldata,fseq_r_input_Ldata,c_mac_rshift
			b.slli fseq_r_L_Y1,fseq_r_L_Y1,(32-c_mac_rshift)
			b.or fseq_r_L_Y1,fseq_r_L_Y1,fseq_r_input_Ldata
			//-------------------------------------
			//-----------------------------------------------------
			b.sw 0(fseq_r_output_tmp),fseq_r_L_Y1

			b.addi fseq_r_input_tmp,fseq_r_input_tmp,4*2
			b.addi fseq_r_output_tmp,fseq_r_output_tmp,4*2
			b.addi fseq_r_len_i,fseq_r_len_i,1
			b.bne fseq_r_len_i,fseq_r_len,f_sweq_load_sample_mono

			//store last XY
	        b.msw 0(fseq_r_lastXY_ptr),fseq_r_L_X0,3 //store 8 para
			b.addi fseq_r_lastXY_ptr,fseq_r_lastXY_ptr,4*4*2
			b.addi fseq_r_coef_tbl,fseq_r_coef_tbl,4*5
			b.addi fseq_r_eq_times,fseq_r_eq_times,-1
			b.mov fseq_r_input_tmp,fseq_r_output //use output_ptr as input_ptr
			b.bnei fseq_r_eq_times,0,f_sweq_load_coef_mono

/*
		//b.mov fseq_r_input_tmp,fseq_r_output
		b.mov fseq_r_len_i,fseq_r_len
		f_sweq_limit_mono:
			b.lwz fseq_r_input_Ldata,0(fseq_r_input_tmp)
			//limit
			b.blts fseq_r_input_Ldata,fseq_r_16bit_max,1f
			b.mov fseq_r_input_Ldata,fseq_r_16bit_max
			b.sw 0(fseq_r_input_tmp),fseq_r_input_Ldata
			1:
			b.bgts fseq_r_input_Ldata,fseq_r_16bit_min,1f
			b.mov fseq_r_input_Ldata,fseq_r_16bit_min
			b.sw 0(fseq_r_input_tmp),fseq_r_input_Ldata
			1:
			b.addi fseq_r_input_tmp,fseq_r_input_tmp,4*2
			b.addi fseq_r_len_i,fseq_r_len_i,-1
			b.bnei fseq_r_len_i,0,f_sweq_limit_mono
// */

		b.mlwz r24,0(r1),3
		b.reti 15,(8*4)
	#endif
#endif










