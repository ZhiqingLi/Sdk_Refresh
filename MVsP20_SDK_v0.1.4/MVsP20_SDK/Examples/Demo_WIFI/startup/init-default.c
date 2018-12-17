#include <nds32_intrinsic.h>
#include "debug.h"
#include "cache_tcm.h"
#include "lp.h"
#include "irqn.h"
#include "P20_version.h"

/* It will use Default_Handler if you don't have one */
#pragma weak tlb_exception_handler   = Default_Handler
#pragma weak error_exception_handler = Default_Handler
#pragma weak syscall_handler         = Default_Handler
#pragma weak general_exception_handler = Default_Handler

//#pragma weak  Timer2Interrupt   = Default_Handler
#pragma weak  WakeupInterrupt   = Default_Handler
#pragma weak  GpioInterrupt     = Default_Handler
#pragma weak  RtcInterrupt      = Default_Handler
#pragma weak  WatchDogInterrupt = Default_Handler
#pragma weak  SWInterrupt       = Default_Handler
#pragma weak  OS_Trap_Interrupt_SWI = Default_Handler
#pragma weak  I2C_Interrupt     = Default_Handler
#pragma weak  UART0_Interrupt   = Default_Handler
#pragma weak  Timer1Interrupt   = Default_Handler
#pragma weak  DMA0_Interrupt    = Default_Handler
#pragma weak  DMA1_Interrupt    = Default_Handler
#pragma weak  DMA2_Interrupt    = Default_Handler
#pragma weak  DMA3_Interrupt    = Default_Handler
#pragma weak  DMA4_Interrupt    = Default_Handler
#pragma weak  DMA5_Interrupt    = Default_Handler
#pragma weak  DMA6_Interrupt    = Default_Handler
#pragma weak  DMA7_Interrupt    = Default_Handler
#pragma weak  UART1_Interrupt   = Default_Handler
#pragma weak  UART2_Interrupt   = Default_Handler
#pragma weak  I2sInterrupt      = Default_Handler
#pragma weak  Timer3Interrupt   = Default_Handler
#pragma weak  Timer4Interrupt   = Default_Handler
#pragma weak  Timer5Interrupt   = Default_Handler
#pragma weak  Timer6Interrupt   = Default_Handler
#pragma weak  Timer7Interrupt   = Default_Handler
#pragma weak  Timer8Interrupt   = Default_Handler
#pragma weak  SDIO_Interrupt    = Default_Handler
#pragma weak  UsbInterrupt      = Default_Handler
#pragma weak  SPIM0_Interrupt   = Default_Handler
#pragma weak  SPIM1_Interrupt   = Default_Handler
#pragma weak  SPIS_Interrupt    = Default_Handler
#pragma weak  FDMAInterrupt     = Default_Handler

__attribute__((unused))
static void Default_Handler()
{
	while (1) ;
}

__attribute__((unused))
static void general_exception_handler(unsigned stack)
{

	unsigned int mask_itype,mask_ipc;
	unsigned int *pstack;

	  pstack = (unsigned int *)stack;
	  DBG("Error exception happened\r\n");

	  mask_itype = __nds32__mfsr(NDS32_SR_ITYPE);
	  mask_ipc = __nds32__mfsr(NDS32_SR_IPC);
	  mask_itype &= 0x0F;
	  DBG("Error Type:");
	  if(mask_itype == 0)
	  {
		  DBG("Alignment check (+I/D bit) or branch target alignment\r\n");
	  }
	  else if(mask_itype == 1)
	  {
		  DBG("Reserved instruction\r\n");
	  }
	  else if(mask_itype == 2)
	  {
		  DBG("Trap\r\n");
	  }
	  else if(mask_itype == 3)
	  {
		  DBG("Arithmetic\r\n");
	  }
	  else if(mask_itype == 4)
	  {
		  DBG("Precise bus error\r\n");
	  }
	  else if(mask_itype == 5)
	  {
		  DBG("Imprecise bus error\r\n");
	  }
	  else if(mask_itype == 6)
	  {
		  DBG("Coprocessor\r\n");
	  }
	  else if(mask_itype == 7)
	  {
		  DBG("Privileged instruction\r\n");
	  }
	  else if(mask_itype == 8)
	  {
		  DBG("Reserved value\r\n");
	  }
	  else if(mask_itype == 9)
	  {
		  DBG("Nonexistent memory address (+I/D bit)\r\n");
	  }
	  else if(mask_itype == 10)
	  {
		  DBG("MPZIU Control (+I/D bit)\r\n");
	  }
	  else if(mask_itype == 11)
	  {
		  DBG("Next precise stack overflow\r\n");
	  }
	  else
	  {
		  DBG("Unknown\r\n");
	  }

	  DBG("PC = 0x%08x\r\n",mask_ipc);
	  DBG("R0 = 0x%08x\r\n",*pstack++);
	  DBG("R1 = 0x%08x\r\n",*pstack++);
	  DBG("R2 = 0x%08x\r\n",*pstack++);
	  DBG("R3 = 0x%08x\r\n",*pstack++);
	  DBG("R4 = 0x%08x\r\n",*pstack++);
	  DBG("R5 = 0x%08x\r\n",*pstack++);
	  DBG("lp = 0x%08x\r\n",*pstack++);
	  DBG("sp = 0x%08x\r\n",*pstack++);
	//DBG("Error exception happened\r\n");
	//NVIC_SystemReset();
	while(1) ;
}

void __c_init()
{

/* Use compiler builtin memcpy and memset */
#define MEMCPY(des, src, n) __builtin_memcpy ((des), (src), (n))
#define MEMSET(s, c, n) __builtin_memset ((s), (c), (n))

	extern char _end;
	extern char __bss_start;
	int size;

	/* data section will be copied before we remap.
	 * We don't need to copy data section here. */
	extern char __data_lmastart;
	extern char __data_start;
	extern char _edata;

	/* Copy data section to RAM */
	size = &_edata - &__data_start;
	MEMCPY(&__data_start, &__data_lmastart, size);

	/* Clear bss section */
//	size = &_end - &__bss_start;
//	MEMSET(&__bss_start, 0, size);
	return;
}

void __cpu_init()
{
	unsigned int tmp;

	LP_GlitchFlagClear();
	LP_FastPowerDownEnable();
	LP_LVDEventFastISOEnable();
	LP_LVDPowerDownEnable();	//lvd pd en do not move this

	/* turn on BTB */
	tmp = 0x0;
	__nds32__mtsr(tmp, NDS32_SR_MISC_CTL);

	tmp = __nds32__mfsr(NDS32_SR_MMU_CTL);
	tmp |= 0x800000;
	__nds32__mtsr(tmp,NDS32_SR_MMU_CTL);
	
	/* disable all hardware interrupts */
	__nds32__mtsr(0x0, NDS32_SR_INT_MASK);
#if (defined(__NDS32_ISA_V3M__) || defined(__NDS32_ISA_V3__))
	if (__nds32__mfsr(NDS32_SR_IVB) & 0x01)
		__nds32__mtsr(0x0, NDS32_SR_INT_MASK);
#endif

#if defined(CFG_EVIC)
	/* set EVIC, vector size: 4 bytes, base: 0x0 */
	__nds32__mtsr(0x1<<13, NDS32_SR_IVB);
#else
# if defined(USE_C_EXT)
	/* If we use v3/v3m toolchain and want to use
	 * C extension please use USE_C_EXT in CFLAGS
	 */
#ifdef __NDS32_ISA_V3__
	/* set IVIC, vector size: 4 bytes, base: 0x0 */
	__nds32__mtsr(0x0, NDS32_SR_IVB);
#else
	/* set IVIC, vector size: 16 bytes, base: 0x0 */
	__nds32__mtsr(0x1<<14, NDS32_SR_IVB);
#endif
# else
	/* set IVIC, vector size: 4 bytes, base: 0x0
	 * If we use v3/v3m toolchain and want to use
	 * assembly version please don't use USE_C_EXT
	 * in CFLAGS */
	__nds32__mtsr(0x0, NDS32_SR_IVB);
# endif
#endif
	/* Set PSW INTL to 0 */
	tmp = __nds32__mfsr(NDS32_SR_PSW);
	tmp = tmp & 0xfffffff9;
#if (defined(__NDS32_ISA_V3M__) || defined(__NDS32_ISA_V3__))
	/* Set PSW CPL to 7 to allow any priority */
	tmp = tmp | 0x70008;
#endif
	__nds32__mtsr_dsb(tmp, NDS32_SR_PSW);
#if (defined(__NDS32_ISA_V3M__) || defined(__NDS32_ISA_V3__))
	/* Check interrupt priority programmable*
	* IVB.PROG_PRI_LVL
	*      0: Fixed priority       -- no exist ir18 1r19
	*      1: Programmable priority
	*/
	if (__nds32__mfsr(NDS32_SR_IVB) & 0x01) {
		/* Set PPL2FIX_EN to 0 to enable Programmable
	 	* Priority Level */
		__nds32__mtsr(0x0, NDS32_SR_INT_CTRL);
		/* Check IVIC numbers (IVB.NIVIC) */
		if ((__nds32__mfsr(NDS32_SR_IVB) & 0x0E)>>1 == 5) {	// 32IVIC
			/* set priority HW9: 0, HW13: 1, HW19: 2,
			* HW#-: 0 */
			__nds32__mtsr(0xAAAAAEAB, NDS32_SR_INT_PRI);
			__nds32__mtsr(0x2AAAAAAA, NDS32_SR_INT_PRI2);
		} else {
			/* set priority HW0: 0, HW1: 1, HW2: 2, HW3: 3
			* HW4-: 0 */
			__nds32__mtsr(0x000000e4, NDS32_SR_INT_PRI);
		}
	}
#endif

	CacheTcmDeInit();

	return;
}

void __init()
{
/*----------------------------------------------------------
   !!  Users should NOT add any code before this comment  !!
------------------------------------------------------------*/
	__cpu_init();
	__c_init();     //copy data section, clean bss
}

void _nds32_init_mem(void) __attribute__((naked, optimize("Os")));

void _nds32_init_mem(void) //The function is weak (optional)
{
  //HAL_MEMORY_SETUP(ORIG_RAM_BASE);
}

__attribute__ ((section(".stub_section"),used)) __attribute__((naked))
void stub(void)
{
__asm__ __volatile__(

    		".long 0xFFFFFFFF \n\n"	//0xA4
    		".long 0xFFFFFFFF \n\n" //0xA8
    		".long 0xFFFFFFFF \n\n" //0xAC
    		".long 0x100000 \n\n"	//0xB0 constant data @ 0x8C
    		".long 0x1D0000 \n\n"	//0xB4 user data
			SDK_VER_CHIPID		    //0xB8
			SDK_VER_MAJOR			//0xB9
			SDK_VER_MINOR			//0xBA
			SDK_VER_USER			//0xBB
    		".long 0xFFFFFFFF \n\n"	//0xBC code crc
    		".long 0xB0BEBDC9 \n\n"	//0xC0 magic number
    		".long 0x00000706 \n\n"	//0xC4 32KHz external oscillator input/output capacitance calibration value
    		".long 0xFFFFFFFF \n\n"	//0xC8 fast code crc	@ 0xA4
    		".long 0xFFFFFFFF \n\n"	//0xCC fast code crc	@ 0xA4
    		".long 0xFFFFFFFF \n\n"	//0xD0 fast code crc	@ 0xA4
    		".long 0x03444846 \n\n"	//0xD4 fast code crc	@ 0xA4
		    ".rept (0x100-0xD8)/4 \n\n"
    		".long 0xFFFFFFFF \n\n"
		    ".endr \n\n"
		    ".short 0xFFFF \n\n"

    );
}
