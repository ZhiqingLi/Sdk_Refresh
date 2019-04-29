.section fastcall,"ax",%progbits 
#define PCM  r3
#define VB1  r4
#define COEF r5

#define SUML	 r6
#define SUMH	 r7

#define CF1_0	 r9
#define CF2_0	 r10
#define CF1_1	 r11
#define CF2_1	 r12
#define CF1_2	 r13
#define CF2_2	 r14
#define CF1_3	 r15
#define CF2_3	 r16
#define CF1_4	 r17
#define CF2_4	 r18
#define CF1_5	 r19
#define CF2_5	 r20
#define CF1_6	 r21
#define CF2_6	 r22
#define CF1_7	 r23
#define CF2_7	 r24

#define VLO		 r25
#define VHI	 	 r26

#define PCM_TMP  r30
#define VB1_TMP  r27
#define COEF_TMP r28


#define SIGN     r25
#define SIGN1    r26

#define I		 r29

#define C64TOS(xl, xh)	        ;\
	b.srli xl,xl,0x1a           ;\
	b.slli xh,xh,0x06           ;\
	b.or   xl,xl,xh             ;\
	b.srai SIGN,xl,0x1f         ;\
	b.srai SIGN1,xl,0xf         ;\
	b.beq  SIGN,SIGN1,0xa       ;\
	b.xori xl,SIGN,0x7fff       ;\


#define SETCF(coef)   	        ;\
	b.mlwz CF1_0,0x0(coef),3    ;\
	b.mlwz CF1_4,0x4*8(coef),3  ;\

#define CF_REV                  ;\
	b.sub CF2_0,r8,CF2_0        ;\
	b.sub CF2_1,r8,CF2_1        ;\
	b.sub CF2_2,r8,CF2_2        ;\
	b.sub CF2_3,r8,CF2_3        ;\
	b.sub CF2_4,r8,CF2_4        ;\
	b.sub CF2_5,r8,CF2_5        ;\
	b.sub CF2_6,r8,CF2_6        ;\
	b.sub CF2_7,r8,CF2_7        ;\

#define MAC(vb, i, j)	        ;\
	b.lwz VLO,0x4*(j+i)(vb)	    ;\
	b.lwz VHI,0x4*(j+23-i)(vb)  ;\
	b.mac VLO,CF1_##i           ;\
	b.mac VHI,CF2_##i           ;\

#define MAC_REV(vb, i, j)       ;\
	b.lwz VLO,0x4*(j+i)(vb)	    ;\
	b.lwz VHI,0x4*(j+23-i)(vb)  ;\
	b.mac VLO,CF2_##i           ;\
	b.mac VHI,CF1_##i           ;\

#define CALSMP                  ;\
	b.ori  SUML,r0,0x2000000    ;\
	b.movi SUMH,0x0             ;\
	b.mtspr r0,SUML,10241       ;\
	b.mtspr r0,SUMH,10242       ;\
	b.mac r17,r9                ;\
	b.mac r18,r10               ;\
	b.mac r19,r11               ;\
	b.mac r20,r12               ;\
	b.mac r21,r13               ;\
	b.mac r22,r14               ;\
	b.mac r23,r15               ;\
	b.mac r24,r16               ;\
	b.mfspr SUML,r0,10241       ;\
	b.mfspr SUMH,r0,10242       ;\
	C64TOS(SUML, SUMH)          ;\


// void PolyphaseStereo(short *pcm, int *vbuf, const int *coefBase)

.globl xmp3_PolyphaseStereo
xmp3_PolyphaseStereo:

	b.entri 0x0e,0x0
	b.movi r8,0x0   //for -c2

	//10+(4+4*8+3+7)*2 = 10+46*2 = 102
	SETCF(COEF)
	CF_REV

	b.ori  SUML,r0,0x2000000
	b.movi SUMH,0x0
	b.mtspr r0,SUML,10241
	b.mtspr r0,SUMH,10242
	MAC(VB1, 0, 0)
	MAC(VB1, 1, 0)
	MAC(VB1, 2, 0)
	MAC(VB1, 3, 0)
	MAC(VB1, 4, 0)
	MAC(VB1, 5, 0)
	MAC(VB1, 6, 0)
	MAC(VB1, 7, 0)
	b.mfspr SUML,r0,10241
	b.mfspr SUMH,r0,10242
	C64TOS(SUML, SUMH)
	b.sh 0x0(PCM), SUML

	b.ori  SUML,r0,0x2000000
	b.movi SUMH,0x0
	b.mtspr r0,SUML,10241
	b.mtspr r0,SUMH,10242
	MAC(VB1, 0, 32)
	MAC(VB1, 1, 32)
	MAC(VB1, 2, 32)
	MAC(VB1, 3, 32)
	MAC(VB1, 4, 32)
	MAC(VB1, 5, 32)
	MAC(VB1, 6, 32)
	MAC(VB1, 7, 32)
	b.mfspr SUML,r0,10241
	b.mfspr SUMH,r0,10242
	C64TOS(SUML, SUMH)
	b.sh 0x2(PCM), SUML

	//coef = coefBase + 256;
	//vb1 = vbuf + 64*16;
	b.addi COEF_TMP,COEF,0x4*(256)
	b.addi VB1_TMP,VB1,0x4*(1024)

	b.mlwz r9, 0x0(COEF_TMP),3
	b.mlwz r17,0x0(VB1_TMP),3
	CALSMP
	b.sh 0x2*(2*16)(PCM), SUML
	b.mlwz r17,0x4*(32)(VB1_TMP),3
	CALSMP
	b.sh 0x2*(2*16+1)(PCM), SUML

	b.addi COEF_TMP,COEF,0x4*(16)
	b.addi VB1_TMP,VB1,0x4*(64)
	b.addi PCM,PCM,0x4*(1)
	b.addi I,r0,0x0f

LoopPS:

	SETCF(COEF_TMP)

    b.ori  SUML,r0,0x2000000  //sum2L
	b.movi SUMH,0x0
	b.mtspr r0,SUML,10241
	b.mtspr r0,SUMH,10242
	MAC_REV(VB1_TMP, 0, 0)
	MAC_REV(VB1_TMP, 1, 0)
	MAC_REV(VB1_TMP, 2, 0)
	MAC_REV(VB1_TMP, 3, 0)
	MAC_REV(VB1_TMP, 4, 0)
	MAC_REV(VB1_TMP, 5, 0)
	MAC_REV(VB1_TMP, 6, 0)
	MAC_REV(VB1_TMP, 7, 0)
	b.mfspr SUML,r0,10241
	b.mfspr SUMH,r0,10242
	C64TOS(SUML, SUMH)
	b.slli PCM_TMP,I,0x03
	b.add PCM_TMP,PCM_TMP,PCM
	b.sh 0x0(PCM_TMP), SUML

	b.ori  SUML,r0,0x2000000  //sum2R
	b.movi SUMH,0x0
	b.mtspr r0,SUML,10241
	b.mtspr r0,SUMH,10242
	MAC_REV(VB1_TMP, 0, 32)
	MAC_REV(VB1_TMP, 1, 32)
	MAC_REV(VB1_TMP, 2, 32)
	MAC_REV(VB1_TMP, 3, 32)
	MAC_REV(VB1_TMP, 4, 32)
	MAC_REV(VB1_TMP, 5, 32)
	MAC_REV(VB1_TMP, 6, 32)
	MAC_REV(VB1_TMP, 7, 32)
	b.mfspr SUML,r0,10241
	b.mfspr SUMH,r0,10242
	C64TOS(SUML, SUMH)
	b.sh 0x2(PCM_TMP), SUML

	CF_REV

	b.ori  SUML,r0,0x2000000  //sum1L
	b.movi SUMH,0x0
	b.mtspr r0,SUML,10241
	b.mtspr r0,SUMH,10242
	MAC(VB1_TMP, 0, 0)
	MAC(VB1_TMP, 1, 0)
	MAC(VB1_TMP, 2, 0)
	MAC(VB1_TMP, 3, 0)
	MAC(VB1_TMP, 4, 0)
	MAC(VB1_TMP, 5, 0)
	MAC(VB1_TMP, 6, 0)
	MAC(VB1_TMP, 7, 0)
	b.mfspr SUML,r0,10241
	b.mfspr SUMH,r0,10242
	C64TOS(SUML, SUMH)
	b.sh 0x0(PCM), SUML

	b.ori  SUML,r0,0x2000000  //sum1R
	b.movi SUMH,0x0
	b.mtspr r0,SUML,10241
	b.mtspr r0,SUMH,10242
	MAC(VB1_TMP, 0, 32)
	MAC(VB1_TMP, 1, 32)
	MAC(VB1_TMP, 2, 32)
	MAC(VB1_TMP, 3, 32)
	MAC(VB1_TMP, 4, 32)
	MAC(VB1_TMP, 5, 32)
	MAC(VB1_TMP, 6, 32)
	MAC(VB1_TMP, 7, 32)
	b.mfspr SUML,r0,10241
	b.mfspr SUMH,r0,10242
	C64TOS(SUML, SUMH)
	b.sh 0x2(PCM), SUML

	b.addi COEF_TMP,COEF_TMP,0x4*(16)
	b.addi VB1_TMP,VB1_TMP,0x4*(64)
	b.addi PCM,PCM,0x4*(1)
	b.addi I,I,-0x01

	b.bgts I,r0,LoopPS

	b.rtnei 0x0e,0x0
	b.jr r9



  //void PolyphaseMono_new(short *pcm, int *vbuf, const int *coefBase)
  .globl xmp3_PolyphaseMono
xmp3_PolyphaseMono:

	b.entri 0x0e,0x0
	b.movi r8,0x0   //for -c2

	SETCF(COEF)
	CF_REV

	b.ori  SUML,r0,0x2000000
	b.movi SUMH,0x0
	b.mtspr r0,SUML,10241
	b.mtspr r0,SUMH,10242
	MAC(VB1, 0, 0)
	MAC(VB1, 1, 0)
	MAC(VB1, 2, 0)
	MAC(VB1, 3, 0)
	MAC(VB1, 4, 0)
	MAC(VB1, 5, 0)
	MAC(VB1, 6, 0)
	MAC(VB1, 7, 0)
	b.mfspr SUML,r0,10241
	b.mfspr SUMH,r0,10242
	C64TOS(SUML, SUMH)
	b.sh 0x0(PCM), SUML

	//coef = coefBase + 256;
	//vb1 = vbuf + 64*16;
	b.addi COEF_TMP,COEF,0x4*(256)
	b.addi VB1_TMP,VB1,0x4*(1024)

	b.mlwz r9, 0x0(COEF_TMP),3
	b.mlwz r17,0x0(VB1_TMP),3
	CALSMP
	b.sh 0x2*(16)(PCM), SUML

	b.addi COEF_TMP,COEF,0x4*(16)
	b.addi VB1_TMP,VB1,0x4*(64)
	b.addi PCM,PCM,0x2*(1)
	b.addi I,r0,0x0f

LoopPM:

	SETCF(COEF_TMP)

    	b.ori  SUML,r0,0x2000000  //sum2L
	b.movi SUMH,0x0
	b.mtspr r0,SUML,10241
	b.mtspr r0,SUMH,10242
	MAC_REV(VB1_TMP, 0, 0)
	MAC_REV(VB1_TMP, 1, 0)
	MAC_REV(VB1_TMP, 2, 0)
	MAC_REV(VB1_TMP, 3, 0)
	MAC_REV(VB1_TMP, 4, 0)
	MAC_REV(VB1_TMP, 5, 0)
	MAC_REV(VB1_TMP, 6, 0)
	MAC_REV(VB1_TMP, 7, 0)
	b.mfspr SUML,r0,10241
	b.mfspr SUMH,r0,10242
	C64TOS(SUML, SUMH)
	b.slli PCM_TMP,I,0x02
	b.add PCM_TMP,PCM_TMP,PCM
	b.sh 0x0(PCM_TMP), SUML

	CF_REV

	b.ori  SUML,r0,0x2000000  //sum1L
	b.movi SUMH,0x0
	b.mtspr r0,SUML,10241
	b.mtspr r0,SUMH,10242
	MAC(VB1_TMP, 0, 0)
	MAC(VB1_TMP, 1, 0)
	MAC(VB1_TMP, 2, 0)
	MAC(VB1_TMP, 3, 0)
	MAC(VB1_TMP, 4, 0)
	MAC(VB1_TMP, 5, 0)
	MAC(VB1_TMP, 6, 0)
	MAC(VB1_TMP, 7, 0)
	b.mfspr SUML,r0,10241
	b.mfspr SUMH,r0,10242
	C64TOS(SUML, SUMH)
	b.sh 0x0(PCM), SUML

	b.addi COEF_TMP,COEF_TMP,0x4*(16)
	b.addi VB1_TMP,VB1_TMP,0x4*(64)
	b.addi PCM,PCM,0x2*(1)
	b.addi I,I,-0x01

	b.bgts I,r0,LoopPM

	b.rtnei 0x0e,0x0
	b.jr r9
