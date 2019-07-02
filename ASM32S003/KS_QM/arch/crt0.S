//start from __start, 
//(0)initialize vector table
//(1)initialize all registers
//(2)prepare initial reg values for user process
//(3)initialize supervisor mode stack pointer
//(4)construct ASID Table
//(5)prepare PTE entry for user process start virtual address
//(6)creat a mapping between VPN:0 and PFN:0 for kernel
//(7)set VBR register
//(8)enable EE and MMU
//(9)jump to the main procedure using jsri main
#define UserOption   0xFFFFFFFF
.export __start
//.text
.export vector_table
//.import ISR_VecTable
.align  10
vector_table:           //totally 256 entries
//  .long   __start
//  .rept   128
//  .long   __dummy
//  .endr

.long __start
.long DummyHandler//MisalignedHandler
.long DummyHandler//AccessErrHandler
.long DummyHandler
.long DummyHandler//IllegalInstrHandler
.long DummyHandler//PriviledgeVioHandler
.long DummyHandler
.long DummyHandler//BreakPointHandler
.long DummyHandler//UnrecExecpHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler//Trap0Handler
.long DummyHandler//Trap1Handler
.long DummyHandler//Trap2Handler
.long DummyHandler//Trap3Handler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler//PendTrapHandler
.long CORETHandler//EPTHandler
.long SYSCONIntHandler
.long IFCIntHandler
.long ADCIntHandler
.long GTC0IntHandler//DummyHandler
.long GTC1IntHandler//GTC0IntHandler
.long GTC2IntHandler
.long EXI0IntHandler
.long EXI1IntHandler
.long GTC3IntHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long UART0IntHandler
.long UART1IntHandler
.long DummyHandler//PCM0IntHandler
.long DummyHandler//PCM1IntHandler
.long I2CIntHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long EXI2to3IntHandler
.long EXI4to8IntHandler
.long EXI9to13IntHandler
.long CNTAIntHandler
.long TKEYGRP0IntHandler
.long TKEYGRP1IntHandler
.long LEDIntHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler
.long DummyHandler



.text
//  .long __start
__start:
  //initialize all registers
  movi r0, 0
  movi r1, 0
  movi r2, 0
  movi r3, 0
  movi r4, 0
  movi r5, 0
  movi r6, 0
  movi r7, 0
  movi r8, 0
  movi r9, 0
  movi r10, 0
  movi r11, 0
  movi r12, 0
  movi r13, 0
  movi r14, 0
  movi r15, 0

//LVD Enable
/*MOVIH r3, 0x4001
ORI r4, r3, 0x204C
MOVIH r3, 0xB44B
ORI r5, r3, 0x01
ST.W r5, (r4,0)*/


//set VBR
  lrw r2, vector_table
  //lrw r2, ISR_VecTable
  mtcr r2, cr<1,0>

//enable EE bit of psr
  mfcr r2, cr<0,0>
  bseti r2, r2, 8
  mtcr r2, cr<0,0>

////set rom access delay
//  lrw     r1, 0xe00000
//  lrw     r2, 0x7
//  st.w    r2, (r1,0x0)

////enable cache
//  lrw     r1, 0xe000f000
//  movi    r2, 0x2
//  st.w    r2, (r1,0x0)
//  lrw     r2, 0x29
//  st.w    r2, (r1,0x4)
//  movi    r2, 0x1
//  st.w    r2, (r1,0x0)

//disable power peak 
  lrw     r1, 0xe000ef90
  movi    r2, 0x0
  st.w    r2, (r1, 0x0)

//initialize kernel stack
  lrw  r14, __kernel_stack
  subi r13,r14,0x4
  lrw  r3, 0x40
  subu r4, r14, r3
  lrw  r5, 0x0
INIT_KERLE_STACK:
  addi r4, 0x4
  st.w r5, (r4)
  //cmphs r14, r4
  cmphs r13, r4
  bt  INIT_KERLE_STACK
        
__to_main:
  lrw r0,__main
  jsr r0
  mov r0, r0
  mov r0, r0

  lrw r15, __exit
  lrw r0,main
  jmp r0
  mov r0, r0
  mov r0, r0
  mov r0, r0
  mov r0, r0
  mov r0, r0

.export __exit
__exit:

  lrw r4, 0x20003000
  lrw r5, 0x0
  st.w r5, (r4)

  mfcr r1, cr<0,0>
  movih r1, 0xFFFF
  mtcr r1, cr<11,0>
  movi    r1, 0xFFF
  movi    r0, 0x0
  st      r1, (r0)

.export DummyHandler
DummyHandler:
  //br      __fail
  movih r5,0xabcd
  ori r5,r5,0x1234
  mtcr r5,cr<31,0>
  br __start

.data
.align  10
.long __start
