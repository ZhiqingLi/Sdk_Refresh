
#include "plat_addr_map.h"

#define HEAP_SECTION_SIZE			0x40
#define STACK_SECTION_SIZE			0xD00


#define OVERLAY_DATA_SECTION_SIZE	0x8000
#define FAST_XRAM_SECTION_SIZE		0xa000

#ifndef FLASH_REGION_BASE
#define FLASH_REGION_BASE			FLASH_BASE
#endif
#ifndef FLASH_REGION_SIZE
#define FLASH_REGION_SIZE			(FLASH_SIZE - (FLASH_REGION_BASE - FLASH_BASE))
#endif
#define FLASH_NC_REGION_BASE		FLASH_C_TO_NC(FLASH_REGION_BASE)
#define FLASHX_REGION_BASE			FLASH_TO_FLASHX(FLASH_REGION_BASE)

/* Linker script to configure memory regions. */
/* See plat_addr_map.h and common.mk for the macro definitions */
MEMORY
{
	ROM			(rx)	: ORIGIN = ROM_BASE,				LENGTH = ROM_SIZE
	FLASH 		(r)		: ORIGIN = FLASH_REGION_BASE,		LENGTH = FLASH_REGION_SIZE
	FLASH_NC	(r)		: ORIGIN = FLASH_NC_REGION_BASE,	LENGTH = FLASH_REGION_SIZE
	FLASHX		(rx)	: ORIGIN = FLASHX_REGION_BASE,		LENGTH = FLASH_REGION_SIZE
	RAM			(rwx)	: ORIGIN = RAM_BASE,				LENGTH = RAM_SIZE - FAST_XRAM_SECTION_SIZE
	RAMX		(rx)	: ORIGIN = RAMX_BASE,				LENGTH = RAM_SIZE - FAST_XRAM_SECTION_SIZE
	FRAMX		(rwx)	: ORIGIN = RAMX_BASE + RAM_SIZE - FAST_XRAM_SECTION_SIZE,	LENGTH = FAST_XRAM_SECTION_SIZE
#ifdef PSRAM_BASE
	PSRAM 		(rwx)	: ORIGIN = PSRAM_BASE,				LENGTH = PSRAM_SIZE
	PSRAM_NC	(rwx)	: ORIGIN = PSRAM_NC_BASE,			LENGTH = PSRAM_SIZE
	PSRAMX		(rx)	: ORIGIN = PSRAMX_BASE,				LENGTH = PSRAM_SIZE
#endif
}

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a)

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __export_fn_rom
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *   __free_ram
 *   __factory_start
 *   __factory_end
 *   __calib_start
 *   __calib_end
 *   __flash_start
 *   __flash_end
 *   __free_flash
 *   __boot_sram_start_flash__
 *   __boot_sram_end_flash__
 *   __boot_sram_start__
 *   __boot_bss_sram_start__
 *   __boot_bss_sram_end__
 *   __sram_text_data_start_flash__
 *   __sram_text_data_end_flash__
 *   __fast_sram_text_data_start__
 *   __fast_sram_text_data_end__
 *   __fast_sram_text_data_start_flash__
 *   __fast_sram_text_data_end_flash__
 *   __sram_text_data_start__
 *   __sram_bss_start__
 *   __sram_bss_end__
 *   __sram_userdata_pool_start__
 *   __overlay_text_start__
 *   __overlay_text_exec_start__
 *   __overlay_data_start__
 *   __overlay_bss_start__
 */
ENTRY(Boot_Loader)

SECTIONS
{
	__export_fn_rom = (ORIGIN(ROM) + LENGTH(ROM) - ROM_BUILD_INFO_SECTION_SIZE - ROM_EXPORT_FN_SECTION_SIZE);

	.boot_struct (ORIGIN(FLASH)) :
	{
		__flash_start = .;
		KEEP(*(.boot_struct))
		. = ALIGN(4);
	} > FLASH

	. = FLASH_TO_FLASHX(.);

	.boot_text_flash (.) : AT (FLASHX_TO_FLASH(ADDR(.boot_text_flash)))
	{
		*(.boot_loader)
		*(.boot_text_flash*)
		*(.boot_rodata*)
		. = ALIGN(4);
	} > FLASHX

	.vector_table (ORIGIN(RAM)) (NOLOAD) :
	{
		KEEP(*(.vector_table))
		. = VECTOR_SECTION_SIZE;
		. = ALIGN(4);
	} > RAM

	.reboot_param (.) (NOLOAD) :
	{
		KEEP(*(.reboot_param))
		. = REBOOT_PARAM_SECTION_SIZE;
		. = ALIGN(4);
	} > RAM

	.userdata_pool (.) (NOLOAD) :
	{
		*(.userdata_pool)
		. = ALIGN(4);
	} > RAM

	__boot_sram_start_flash__ = LOADADDR(.boot_text_flash) + SIZEOF(.boot_text_flash);
	__boot_sram_start__ = ADDR(.userdata_pool) + SIZEOF(.userdata_pool);
	.boot_text_sram (RAM_TO_RAMX(__boot_sram_start__)) : AT (__boot_sram_start_flash__)
	{
		*:hal_norflash*.o(.text*)
		*:norflash_*.o(.text*)
		*(.boot_text_sram*)
		. = ALIGN(4);
	} > RAMX

	.boot_data_sram (RAMX_TO_RAM(ADDR(.boot_text_sram) + SIZEOF(.boot_text_sram))) :
					AT (__boot_sram_start_flash__ + SIZEOF(.boot_text_sram))
	{
		*:hal_norflash*.o(.data* .rodata*)
		*:norflash_*.o(.data* .rodata*)
		*:hal_psram.o(.data* .rodata*)
		*(.boot_data*)
		. = ALIGN(4);
	} > RAM

	__boot_sram_end_flash__ = __boot_sram_start_flash__ +
								SIZEOF(.boot_text_sram) + SIZEOF(.boot_data_sram);

	.boot_bss_sram (.) (NOLOAD) :
	{
		__boot_bss_sram_start__ = .;
		*:hal_norflash*.o(.bss*)
		*:norflash_*.o(.bss*)
		*:hal_psram.o(.bss*)
		*(.boot_bss*)
		. = ALIGN(4);
		__boot_bss_sram_end__ = .;
	} > RAM

	__sram_text_data_start_flash__ = __boot_sram_end_flash__;
	__sram_text_data_start__ = __boot_bss_sram_end__;
	.sram_text (RAM_TO_RAMX(__sram_text_data_start__)) :
				AT (__sram_text_data_start_flash__)
	{
		*(.sram_text*)
#if defined(A2DP_AAC_ON)		
		*:utils.o(.text*)
		*:cqueue.o(.text*)
#endif		
		. = ALIGN(4);
	} > RAMX

	.sram_data (RAMX_TO_RAM(ADDR(.sram_text) + SIZEOF(.sram_text))) :
				AT (__sram_text_data_start_flash__ + SIZEOF(.sram_text))
	{
		*:utils.o(.data* .rodata*)
		*:cqueue.o(.data* .rodata*)
		*:iir_process.o(.data* .rodata*)	
		*(.sram_data*)
		. = ALIGN(4);
	} > RAM

	__sram_text_data_end_flash__ = __sram_text_data_start_flash__ +
								SIZEOF(.sram_text) + SIZEOF(.sram_data);

	.sram_bss (.) (NOLOAD) :
	{
		__sram_bss_start__ = .;
		*:utils.o(.bss*)
		*(.sram_bss*)
		. = ALIGN(4);
		__sram_bss_end__ = .;
	} > RAM

	__fast_sram_text_data_start_flash__ = __sram_text_data_end_flash__;
	.fast_text_sram (ORIGIN(FRAMX)) : AT (__fast_sram_text_data_start_flash__)
	{
		__fast_sram_text_data_start__ = .;		
		*:audio_resample_ex.o(.text*)
#if defined(HFP_1_6_ENABLE)
#if !defined(__SBC_FUNC_IN_ROM__)		
		*:sbc.o(.text.SbcIsValidStreamInfo)
		*:sbc.o(.text.SBC_FrameLen)
		*:sbc.o(.text.SbcCrcSum4)
		*:sbc.o(.text.SbcMonoBitAlloc)
		*:sbc.o(.text.SbcStereoBitAlloc)
		*:sbc.o(.text.SbcParseHeader)
		*:sbc.o(.text.SbcParseScaleFactors)
		*:sbc.o(.text.SbcUnpackSample)
		*:sbc.o(.text.SbcBuildSubbandSamples)
		*:sbc.o(.text.SbcJointProcessing)
		*:sbc.o(.text.SbcSynthesisFilter4)
		*:sbc.o(.text.SbcSynthesisFilter8)
		*:sbc.o(.text.SbcResetDecoderState)
		*:sbc.o(.text.SbcMuteFrame)
		*:sbc.o(.text.SBC_DecodeFrames)
#endif		
		*:sbc_math.o(.text*)
#endif
#if defined(A2DP_AAC_ON)
		*:fir_process.o(.text*)
		*:iir_process.o(.text.iir_run .text.*iir_run_per_*)
#else
		/* libc_nano.a(lib_a-memcpy.o) or memcpy.o */
		*memcpy.o(.text*)
		/* libc_nano.a(lib_a-memmove.o) or memmove.o */
		*memmove.o(.text*)
		/* libc_nano.a(lib_a-memset.o) or memset.o */
		*memset.o(.text*)	
		*:utils.o(.text*)
		*:cqueue.o(.text*)
#endif
		*(.fast_text_sram*)
		. = ALIGN(4);
		__fast_sram_text_data_end__ = .;
	} > FRAMX
	__fast_sram_text_data_end_flash__ = __fast_sram_text_data_start_flash__ + SIZEOF(.fast_text_sram);

	__overlay_text_start__ = RAMX_TO_RAM(__fast_sram_text_data_end__);
	__overlay_text_exec_start__ = RAM_TO_RAMX(__overlay_text_start__);
	OVERLAY (__overlay_text_exec_start__) : NOCROSSREFS AT (__fast_sram_text_data_end_flash__)
	{
		.overlay_text0
		{
			LONG(0);
			*:preprocess.o(.text.speex_preprocess_run)
			*:fftwrap.o(.text*)
			*:mdf.o(.text.speex_echo_cancellation)
			*:plc.o(.text*)		
			*:iir_process.o(.text.iir_run_mono_15bits .text.iir_run_mono_16bits .text.iir_run_mono_algorithmgain)
#if defined(_CVSD_BYPASS_)
			*:Pcm8k_Cvsd.o(.text*)
#endif
#if defined(HFP_1_6_ENABLE) && defined(__SBC_FUNC_IN_ROM__)
			*:sbc.o(.text.SBC_InitEncoder)
			*:sbc.o(.text.SbcAnalysisFilter4)
			*:sbc.o(.text.SbcAnalysisFilter8)
			*:sbc.o(.text.SbcCalculateScaleFactors)
			*:sbc.o(.text.SbcPrepareHeader)
			*:sbc.o(.text.SbcPackSample)
			*:sbc.o(.text.SbcQuantizeSamples)
			*:sbc.o(.text.SBC_EncodeFrames)
#endif
			*(.overlay_text0)
			. = ALIGN(4);
		}
		.overlay_text1
		{
			LONG(0);
#if !defined(RB_CODEC)
#if !defined(HFP_1_6_ENABLE)
			*:sbc.o(.text.SbcIsValidStreamInfo)
			*:sbc.o(.text.SBC_FrameLen)
			*:sbc.o(.text.SbcCrcSum4)
			*:sbc.o(.text.SbcMonoBitAlloc)
			*:sbc.o(.text.SbcStereoBitAlloc)
			*:sbc.o(.text.SbcParseHeader)
			*:sbc.o(.text.SbcParseScaleFactors)
			*:sbc.o(.text.SbcUnpackSample)
			*:sbc.o(.text.SbcBuildSubbandSamples)
			*:sbc.o(.text.SbcJointProcessing)
			*:sbc.o(.text.SbcSynthesisFilter4)
			*:sbc.o(.text.SbcSynthesisFilter8)
			*:sbc.o(.text.SbcResetDecoderState)
			*:sbc.o(.text.SbcMuteFrame)
			*:sbc.o(.text.SBC_DecodeFrames)
			*:sbc_math.o(.text*)
#endif
			*:fixedpoint.o(.text*)
			*:replaygain.o(.text*)
			*:dsp_core.o(.text*)
			*:dsp_misc.o(.text*)
			*:dsp_filter.o(.text*)
			*:dsp_sample_input.o(.text*)
			*:dsp_sample_output.o(.text*)
			*:eq.o(.text*)
			*:pga.o(.text*)
#if !defined(A2DP_AAC_ON)
			*:fir_process.o(.text*)
			*:iir_process.o(.text.iir_run .text.*iir_run_per_*)
#endif
#endif
			*(.overlay_text1)
			. = ALIGN(4);
		}
		.overlay_text2
		{
			LONG(0);
			*:fmdec.o(.text* .data* .rodata* .bss*)
			*(.overlay_text2)
			. = ALIGN(4);
		}
		.overlay_text3
		{
			LONG(0);
			*:fft128dot.o(.text*)
			*(.overlay_text3)
			. = ALIGN(4);
		}
		.overlay_text4
		{
			LONG(0);
#if defined(A2DP_AAC_ON)
			*:aacdec.o(.text*)
			*:aactabs.o(.text*)
			*:bitstream.o(.text*)
			*:buffers.o(.text*)
			*:dct4.o(.text*)
			*:decelmnt.o(.text*)
			*:dequant.o(.text*)
			*:fft.o(.text*)
			*:filefmt.o(.text*)
			*:huffman.o(.text*)
			*:hufftabs.o(.text*)
			*:imdct.o(.text*)
			*:noiseless.o(.text*)
			*:pns.o(.text*)
			*:stproc.o(.text*)
			*:tns.o(.text*)
			*:trigtabs.o(.text*)
			*:trigtabs_fltgen.o(.text*)
#endif
			*(.overlay_text4)
			. = ALIGN(4);
		}
		.overlay_text5
		{
			LONG(0);
			*(.overlay_text5)
			. = ALIGN(4);
		}
		.overlay_text6
		{
			LONG(0);
			*(.overlay_text6)
			. = ALIGN(4);
		}
		.overlay_text7
		{
			LONG(0);
			*(.overlay_text7)
			. = ALIGN(4);
		}
		.overlay_text_end
		{
			LONG(0);
			. = ALIGN(4);
		}
	} > FRAMX

	.dummy_overlay_text (.) (NOLOAD) :
	{
		__dummy_overlay_text_end__ = .;
	} > FRAMX

	ASSERT(__dummy_overlay_text_end__ - __fast_sram_text_data_start__ <= FAST_XRAM_SECTION_SIZE, "fast xram sections too large")
	__free_fram = FAST_XRAM_SECTION_SIZE - (__dummy_overlay_text_end__ - __fast_sram_text_data_start__);

	__overlay_data_start__ = __sram_bss_end__;
	OVERLAY (__overlay_data_start__) : NOCROSSREFS AT (__load_stop_overlay_text_end)
	{
		.overlay_data0
		{
			/* Explicitly place 4 bytes at section start to avoid "section type changed to PROGBITS" warnings */
			LONG(0);
			*:preprocess.o(.data* .rodata*)
			*:filterbank.o(.data* .rodata*)
			*:kiss_fft.o(.data* .rodata*)
			*:kiss_fftr.o(.data* .rodata*)
			*:fftwrap.o(.data* .rodata*)
			*:mdf.o(.data* .rodata*)
			*:plc.o(.data* .rodata*)
#if defined(_CVSD_BYPASS_)
			*:Pcm8k_Cvsd.o(.data* .rodata* .bss*)
#endif
			*(.overlay_data0 .overlay_rodata0)
			. = ALIGN(4);
		}
		
		.overlay_data1
		{
			LONG(0);
#if !defined(RB_CODEC)
#if !defined(HFP_1_6_ENABLE)
			*:sbc.o(.data* .rodata*)
			*:sbc_math.o(.data* .rodata*)
#endif
			*:fixedpoint.o(.data* .rodata*)
			*:replaygain.o(.data* .rodata*)
			*:dsp_core.o(.data* .rodata*)
			*:dsp_misc.o(.data* .rodata*)
			*:dsp_filter.o(.data* .rodata*)
			*:dsp_sample_input.o(.data* .rodata*)
			*:dsp_sample_output.o(.data* .rodata*)
			*:eq.o(.data* .rodata*)
			*:pga.o(.data* .rodata*)
#if !defined(A2DP_AAC_ON)
			*:fir_process.o(.data* .rodata*)
#endif
#endif
			*(.overlay_data1 .overlay_rodata1)
			. = ALIGN(4);
		}
		.overlay_data2
		{
			LONG(0);
			*(.overlay_data2 .overlay_rodata2)
			. = ALIGN(4);
		}
		.overlay_data3
		{
			LONG(0);
			*:fft128dot.o(.data* .rodata*)
			*(.overlay_data3 .overlay_rodata3)
			. = ALIGN(4);
		}
		.overlay_data4
		{
			LONG(0);
#if defined(A2DP_AAC_ON)
			*:aacdec.o(.data*)
			*:aactabs.o(.data*)
			*:bitstream.o(.data*)
			*:dct4.o(.data*)
			*:decelmnt.o(.data*)
			*:dequant.o(.data*)
			*:fft.o(.data*)
			*:filefmt.o(.data*)
			*:huffman.o(.data*)
			*:hufftabs.o(.data*)
			*:imdct.o(.data*)
			*:noiseless.o(.data*)
			*:pns.o(.data*)
			*:stproc.o(.data*)
			*:tns.o(.data*)
			*:trigtabs.o(.data*)
			*:trigtabs_fltgen.o(.data*)	
			*:buffers.o(.data* . .rodata*)
#endif
			*(.overlay_data4 .overlay_rodata4)
			. = ALIGN(4);
		}
		.overlay_data5
		{
			LONG(0);
			*(.overlay_data5 .overlay_rodata5)
			. = ALIGN(4);
		}
		.overlay_data6
		{
			LONG(0);
			*(.overlay_data6 .overlay_rodata6)
			. = ALIGN(4);
		}
		.overlay_data7
		{
			LONG(0);
			*(.overlay_data7 .overlay_rodata7)
			. = ALIGN(4);
		}
		.overlay_data_end
		{
			LONG(0);
			. = ALIGN(4);
		}
	} > RAM

	.dummy_overlay_data (.) (NOLOAD) :
	{
		__dummy_overlay_data_end__ = .;
	} > RAM

	__overlay_bss_start__ = __dummy_overlay_data_end__;
	OVERLAY (__overlay_bss_start__) :NOCROSSREFS AT (__load_stop_overlay_data_end)
	{
		.overlay_bss0
		{
			LONG(0);
			*:voicebtpcmplay.o(.bss*)
			*:preprocess.o(.bss*)
			*:filterbank.o(.bss*)
			*:kiss_fft.o(.bss*)
			*:kiss_fftr.o(.bss*)
			*:fftwrap.o(.bss*)
			*:mdf.o(.bss*)
			*:plc.o(.bss*)		
#if defined(_CVSD_BYPASS_)
			*:Pcm8k_Cvsd.o(.bss*)
#endif
			*(.overlay_bss0 .overlay_rwbss0)
			. = ALIGN(4);
		}
		
		.overlay_bss1
		{
			LONG(0);
			*(.overlay_bss1 .overlay_rwbss1)
			. = ALIGN(4);
		}
		.overlay_bss2
		{
			LONG(0);
			*(.overlay_bss2 .overlay_rwbss2)
			. = ALIGN(4);
		}
		.overlay_bss3
		{
			LONG(0);
			*:fft128dot.o(.data* .rodata*)
			*(.overlay_bss3 .overlay_rwbss3)
			. = ALIGN(4);
		}
		.overlay_bss4
		{
			LONG(0);
#if defined(A2DP_AAC_ON)
			*:aacdec.o(.bss*)
			*:aactabs.o(.bss*)
			*:bitstream.o( .bss*)
			*:dct4.o( .bss*)
			*:decelmnt.o( .bss*)
			*:dequant.o( .bss*)
			*:fft.o(.bss*)
			*:filefmt.o(.bss*)
			*:huffman.o(.bss*)
			*:hufftabs.o(.bss*)
			*:imdct.o(.bss*)
			*:noiseless.o(.bss*)
			*:pns.o(.bss*)
			*:stproc.o(.bss*)
			*:tns.o(.bss*)
			*:trigtabs.o(.bss*)
			*:trigtabs_fltgen.o(.bss*)	
			*:buffers.o(.bss*)
			*:a2dpplay.o(.bss.*raw_aac_pcm_data)
#endif		
			*(.overlay_bss4 .overlay_rwbss4)
			. = ALIGN(4);
		}
		.overlay_bss5
		{
			LONG(0);
			*(.overlay_bss5 .overlay_rwbss5)
			. = ALIGN(4);
		}
		.overlay_bss6
		{
			LONG(0);
			*(.overlay_bss6 .overlay_rwbss6)
			. = ALIGN(4);
		}
		.overlay_bss7
		{
			LONG(0);
			*(.overlay_bss7 .overlay_rwbss7)
			. = ALIGN(4);
		}
		.overlay_bss_end
		{
			LONG(0);
			. = ALIGN(4);
		}
	} > RAM

	.dummy_overlay_bss (.) (NOLOAD) :
	{
		__dummy_overlay_bss_end__ = .;
	} > RAM

	.text (FLASH_TO_FLASHX(__load_stop_overlay_bss_end)) : AT (FLASHX_TO_FLASH(ADDR(.text)))
	{
		*(.text*)
		*(.flash_text*)

		KEEP(*(.init))
		KEEP(*(.fini))

		/* .ctors */
		*crtbegin.o(.ctors)
		*crtbegin?.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)

		/* .dtors */
		*crtbegin.o(.dtors)
		*crtbegin?.o(.dtors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
		*(SORT(.dtors.*))
		*(.dtors)
	} > FLASHX

	.ARM.extab (.) : AT (FLASHX_TO_FLASH(ADDR(.ARM.extab)))
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > FLASHX

	__exidx_start = .;
	.ARM.exidx (.) : AT (FLASHX_TO_FLASH(ADDR(.ARM.exidx)))
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > FLASHX
	__exidx_end = .;

	. = FLASHX_TO_FLASH(.);

	.rodata (.) :
	{
		*(.rodata*)
		*(.flash_rodata*)
		*(.note.gnu.build-id)

		KEEP(*(.eh_frame*))
	} > FLASH

	.custom.cmd.table (.) :
	{
	. = ALIGN(4);
	__custom_handler_table_start = .;
		KEEP(*(.custom_handler_table))		
	__custom_handler_table_end = .;	
	} > FLASH

	/* To copy multiple FLASH to RAM sections,
	 * uncomment .copy.table section and,
	 * define __STARTUP_COPY_MULTIPLE in startup_ARMCMx.S */
	/*
	.copy.table (.) :
	{
		. = ALIGN(4);
		__copy_table_start__ = .;
		LONG (__etext)
		LONG (__data_start__)
		LONG (__data_end__ - __data_start__)
		LONG (__etext2)
		LONG (__data2_start__)
		LONG (__data2_end__ - __data2_start__)
		__copy_table_end__ = .;
	} > FLASH
	*/

	/* To clear multiple BSS sections,
	 * uncomment .zero.table section and,
	 * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */
	/*
	.zero.table (.) :
	{
		. = ALIGN(4);
		__zero_table_start__ = .;
		LONG (__bss_start__)
		LONG (__bss_end__ - __bss_start__)
		LONG (__bss2_start__)
		LONG (__bss2_end__ - __bss2_start__)
		__zero_table_end__ = .;
	} > FLASH
	*/

	__etext = .;

	.data : AT (__etext)
	{
		__data_start__ = .;
		*(vtable)
		*(.data*)

		. = ALIGN(4);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);

		. = ALIGN(4);
		/* init data */
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);

		. = ALIGN(4);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);

		KEEP(*(.jcr*))
		. = ALIGN(4);
		/* All data end */
		__data_end__ = .;

	} > RAM

	.bss (.) (NOLOAD) :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;
	} > RAM

	.heap (.) (NOLOAD) :
	{
		. = ALIGN(8);
		__HeapBase = .;
		__end__ = .;
		end = __end__;
		. += HEAP_SECTION_SIZE;
		. = ALIGN(8);
		__HeapLimit = .;
	} > RAM

	/* .stack_dummy section doesn't contains any symbols. It is only
	 * used for linker to calculate size of stack sections, and assign
	 * values to stack symbols later */
	.stack_dummy (.) (COPY) :
	{
		. = STACK_SECTION_SIZE;
		. = ALIGN(8);
	} > RAM

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(RAM) + LENGTH(RAM);
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);

	/* Check if data + heap + stack exceeds RAM limit */
	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
	__free_ram = __StackLimit - __HeapLimit;

	. = __etext + SIZEOF(.data);
	.build_info (.) :
	{
		KEEP(*(.build_info))
		. = ALIGN(4);
	} > FLASH = 0x00000000

	/* The following section be the last loaded section */
	.code_start_addr (.) :
	{
		LONG(BUILD_INFO_MAGIC);
		LONG(ABSOLUTE(__flash_start));
	} > FLASH

	__flash_end = .;

	.custom_parameter (ORIGIN(FLASH_NC) + LENGTH(FLASH_NC) - CUSTOM_PARAMETER_SECTION_SIZE - AUD_SECTION_SIZE - FACTORY_SECTION_SIZE - USERDATA_SECTION_SIZE) (NOLOAD) :
	{
		__custom_parameter_start = .;
		. = CUSTOM_PARAMETER_SECTION_SIZE;
		__custom_parameter_end = .;
	} > FLASH_NC
	
	.audio (ORIGIN(FLASH_NC) + LENGTH(FLASH_NC) - FACTORY_SECTION_SIZE - USERDATA_SECTION_SIZE - AUD_SECTION_SIZE) (NOLOAD) :
	{
		__aud_start = .;
		. = AUD_SECTION_SIZE;
		__aud_end = .;
	} > FLASH_NC

	
	.userdata (ORIGIN(FLASH_NC) + LENGTH(FLASH_NC) - FACTORY_SECTION_SIZE - USERDATA_SECTION_SIZE) (NOLOAD) :
	{
		__userdata_start = .;
		. = USERDATA_SECTION_SIZE;
		__userdata_end = .;
	} > FLASH_NC

	.factory (ORIGIN(FLASH_NC) + LENGTH(FLASH_NC) - FACTORY_SECTION_SIZE) (NOLOAD) :
	{
		__factory_start = .;
		. = FACTORY_SECTION_SIZE;
		__factory_end = .;
	} > FLASH_NC

	ASSERT(FLASH_NC_TO_C(__custom_parameter_start) >= __flash_end, "region FLASH overflowed")
	__free_flash = FLASH_NC_TO_C(__custom_parameter_start) - __flash_end;
}

