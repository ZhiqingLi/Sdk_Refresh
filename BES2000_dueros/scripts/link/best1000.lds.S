
#include "plat_addr_map.h"

#ifdef MEDIA_PLAYER_RBCODEC
#define HEAP_SECTION_SIZE			0x1000
#else
#define HEAP_SECTION_SIZE			0x400
#endif

#define STACK_SECTION_SIZE			0xD00

#ifdef MEDIA_PLAYER_RBCODEC
#define OVERLAY_DATA_SECTION_SIZE	0xA000
#else
#define OVERLAY_DATA_SECTION_SIZE	0x8000
#endif

#if (RAM_SIZE >= 0x40000)
#if defined(BTUSB_AUDIO_MODE)
#define FAST_XRAM_SECTION_SIZE		0x18000
#else
#define FAST_XRAM_SECTION_SIZE		0xb000
#endif
#else
#define FAST_XRAM_SECTION_SIZE		0x8000
#endif

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
#ifdef SYSTEM_USE_PSRAM
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
 *   __overlay_text_start__
 *   __overlay_text_exec_start__
 *   __overlay_data_start__
 */
ENTRY(Boot_Loader)

SECTIONS
{
#if defined(EXPORT_FN_ROM_CUST)
	__export_fn_rom_cust = (ORIGIN(ROM) + LENGTH(ROM) - ROM_BUILD_INFO_SECTION_SIZE - ROM_EXPORT_FN_SECTION_SIZE);
#else
	__export_fn_rom = (ORIGIN(ROM) + LENGTH(ROM) - ROM_BUILD_INFO_SECTION_SIZE - ROM_EXPORT_FN_SECTION_SIZE);
#endif

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
		__userdata_pool_end__ = .;
	} > RAM

	__boot_sram_start_flash__ = LOADADDR(.boot_text_flash) + SIZEOF(.boot_text_flash);
	__boot_sram_start__ = __userdata_pool_end__;
	.boot_text_sram (RAM_TO_RAMX(__boot_sram_start__)) : AT (__boot_sram_start_flash__)
	{
		/* memcpy.o or libc_nano.a(lib_a-memcpy.o/lib_a-memcpy-stub.o) */
		*:memcpy.o(.text*)
		*:lib_a-memcpy*.o(.text*)

		/* memset.o or libc_nano.a(lib_a-memset.o) */
		*:memset.o(.text*)
		*:lib_a-memset*.o(.text*)

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
		*:audioflinger.o(.bss*)
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
		. = ALIGN(4);
	} > RAMX

	.sram_data (RAMX_TO_RAM(ADDR(.sram_text) + SIZEOF(.sram_text))) :
				AT (__sram_text_data_start_flash__ + SIZEOF(.sram_text))
	{
#if defined(BTUSB_AUDIO_MODE)
		*:hal_usb.o(.rodata*)
#endif

#if defined(SPEECH_CAPTURE_TWO_CHANNEL)
		*:list.o(.data* .rodata*)
		*:list_ext.o(.data* .rodata*)
#endif
		*:utils.o(.data* .rodata*)
		*:cqueue.o(.data* .rodata*)
		*:iir_process.o(.data* .rodata*)
#if defined(HFP_1_6_ENABLE)
		*:sbcplc.o(.data* .rodata*)
		*:sbc.o(.data* .rodata*)
		*:sbc_math.o(.data* .rodata*)
#endif
		*(.sram_data*)
		. = ALIGN(4);
	} > RAM

	__sram_text_data_end_flash__ = __sram_text_data_start_flash__ +
								SIZEOF(.sram_text) + SIZEOF(.sram_data);

	.sram_bss (.) (NOLOAD) :
	{
		__sram_bss_start__ = .;
		*:utils.o(.bss*)
        *:app_audio.o(.bss.*app_audio_buff*)
		*(.sram_bss*)
		. = ALIGN(4);
		__sram_bss_end__ = .;
	} > RAM

	__fast_sram_text_data_start_flash__ = __sram_text_data_end_flash__;
	.fast_text_sram (ORIGIN(FRAMX)) : AT (__fast_sram_text_data_start_flash__)
	{
		__fast_sram_text_data_start__ = .;

#if defined(BTUSB_AUDIO_MODE) || defined(SPEECH_CAPTURE_TWO_CHANNEL)
		*:hal_dma.o(.text*)
		*:hal_uart.o(.text*)
		*:hal_trace.o(.text*)
#endif
#if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)
        *:app_ai_voice.o(.text*)
		*:audioflinger.o(.text.af_stream_update_dma_buffer*)
		*:audioflinger.o(.text.af_dma_irq_handler*)
		*:audioflinger.o(.text.af_stream_get_cfg*)
		*:audioflinger.o(.text.af_stream_get_cur_dma_addr*)
		*:audioflinger.o(.text.af_thread*)
#endif
#if defined(__AMA_VOICE__)
        *:ama_stream.o(.text*)		
#endif
#if defined(BTUSB_AUDIO_MODE)
		*:hal_usb.o(.text*)

		*:usb_audio.o(.text*)
		*:usb_audio_sync.o(.text*)
		*:usb_audio_cdc.o(.text*)

		*:usbaudio_entry.o(.text*)
		*:usb_audio_app.o(.text*)
#endif

		/* memmove.o or libc_nano.a(lib_a-memmove.o) */
		*:memmove.o(.text*)
		*:lib_a-memmove.o(.text*)
/*#if defined(SPEECH_CAPTURE_TWO_CHANNEL)*/
#if 0
		*:app_bt_stream.o(.text*)
#endif
#if 1
        *:app_bt_stream.o(.text.*bt_sbc_player_setup*)
        *:app_bt_stream.o(.text.*app_bt_stream_volume_ptr_update*)
        *:app_bt_stream.o(.text.*app_bt_stream_volume_get_ptr*)
        *:app_bt_stream.o(.text.*app_bt_stream_isrun*)
#endif

		*:intersyshci.o(.text*)
		*:hal_intersys.o(.text*)

		*:list.o(.text*)
		*:list_ext.o(.text*)
/* #endif */
		*:utils.o(.text*)
		*:cqueue.o(.text*)
		*:audio_resample_ex.o(.text*)
#if defined(HFP_1_6_ENABLE)
#if defined(SPEECH_CAPTURE_TWO_CHANNEL)
		*:voicebtpcmplay.o(.text*)
#endif
#if 0
#if defined(__SBC_FUNC_IN_ROM__)
		*:sbc.o(.text*)
#else
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
		*:sbc.o(.text.SbcQuantizeSamples)
		*:sbc.o(.text.SBC_InitDecoder)
		*:sbc.o(.text.SBC_InitEncoder)
		*:sbc.o(.text.SBC_EncodeFrames)
#endif
		*:sbc_math.o(.text*)
#endif		
#endif
#if defined(A2DP_AAC_ON)
		*:fir_process.o(.text*)
		*:iir_process.o(.text.iir_run .text.*iir_run_per_*)
#endif
#if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)
        *:voice_opus.o(.text*)
        *:opus_kiss_fft.o(.text*)
#endif
        *:kfifo.o(.text*)
#if !defined(A2DP_SCALABLE_ON)
		*:kiss_fft.o(.text*)
#endif
		*:kiss_fftr.o(.text*)
		*:fftwrap.o(.text*)
		*:lc_mmse_ns.o(.text*)
		*(.fast_text_sram*)
		. = ALIGN(4);
		__fast_sram_text_data_end__ = .;
	} > FRAMX
	__fast_sram_text_data_end_flash__ = __fast_sram_text_data_start_flash__ + SIZEOF(.fast_text_sram);

	__overlay_text_start__ = RAMX_TO_RAM(__fast_sram_text_data_end__);
	__overlay_text_exec_start__ = RAM_TO_RAMX(__overlay_text_start__);
	OVERLAY (__overlay_text_exec_start__) : NOCROSSREFS AT (__fast_sram_text_data_end_flash__)
	{
#ifndef NO_OVERLAY
		.overlay_text0
		{
			LONG(0);
#if 1
			*:preprocess.o(.text*)
			*:filterbank.o(.text*)

			*:mdf.o(.text*)
			*:plc_8000.o(.text*)
			*:iir_process.o(.text.iir_run_mono_15bits .text.iir_run_mono_16bits .text.iir_run_mono_algorithmgain)
#if defined(HFP_1_6_ENABLE)
			*:plc_16000.o(.text*)
#endif
#if defined(_CVSD_BYPASS_)
			*:Pcm8k_Cvsd.o(.text*)
#endif
#if defined(SPEECH_TX_2MIC_NS)
			*:webrtc_vad.o(.text.WebRtcVad_Process)
			*:webrtc_vad.o(.text.WebRtcVad_ValidRateAndFrameLength)
			*:webrtc_vad.o(.text.WebRtcVad_CalcVad16khz)
			*:webrtc_vad.o(.text.WebRtcVad_Downsampling)
			*:webrtc_vad.o(.text.WebRtcVad_CalcVad8khz)
			*:vad_filterbank.o(.text.*)
			*:energy.o(.text.*)
			*:get_scaling_square.o(.text.*)
			*:dual_mic_denoise.o(.text*)
			*:cross_correlation.o(.text*)
			*:min_max_operations.o(.text*)
			*:resample_by_2_internal.o(.text*)
			*:division_operations.o(.text*)
			*:downsample_fast.o(.text*)
			*:resample_fractional.o(.text*)
			*:resample_48khz.o(.text*)
			*:vad_sp.o(.text*)
			*:vad_core.o(.text*)
			*:webrtc_vad.o(.text*)
			*:vad_gmm.o(.text*)
#endif
#if defined(SPEECH_TX_2MIC_NS2)
			*:coherent_denoise.o(.text*)
#endif

#if defined(SPEECH_TX_NS2FLOAT)
			*:lc_mmse_ns_float.o(.text*)
			*:cmsis_fftwrap.o(.text*)
			*:arm_rfft_fast_f32.o(.text*)
			*:arm_cfft_f32.o(.text*)
#endif
#if defined(SPEECH_TX_AEC2FLOAT)
			*:coherence.o(.text*)
			*:echo_canceller.o(.text*)
			*:echo_suppression(.text*)
			*:iirfilt.o(.text*)
			*:integer_delay.o(.text*)
			*:mdf2.o(.text*)
			*:ringbuf.o(.text*)
#endif
#endif
			*(.overlay_text0)
			. = ALIGN(4);
		}
		.overlay_text1
		{
			LONG(0);
#if !defined(RB_CODEC)
#if !defined(HFP_1_6_ENABLE)
			*:sbc.o(.text*)
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
			*:mpa.o(.text*)
			*:bit.o(.text*)
			*:frame.o(.text*)
			*:huffman.o(.text*)
			*:layer12.o(.text*)
			*:layer3.o(.text*)
			*:stream.o(.text*)
			*:synth.o(.text*)
			*(.overlay_text5)
			. = ALIGN(4);
		}
		.overlay_text6
		{
			LONG(0);
#if defined(A2DP_SCALABLE_ON)
			*:Audio_asm.o(.text*)
			*:vq.o(.text*)
			*:util.o(.text*)
			*:rate.o(.text*)
			*:quant_bands.o(.text*)
			*:modes.o(.text*)
			*:mdct.o(.text*)
			*:mathops.o(.text*)
			*:laplace.o(.text*)
			*:entdec.o(.text*)
			*:entcode.o(.text*)
			*:cwrs.o(.text*)
			*:audio_int32.o(.text*)
			*:audio_decoder.o(.text*)
			*:audio.o(.text*)
#endif
			*(.overlay_text6)
			. = ALIGN(4);
		}
		.overlay_text7
		{
			LONG(0);
			*(.overlay_text7)
			. = ALIGN(4);
		}
#endif
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
	__overf_fram = (__dummy_overlay_text_end__ - __fast_sram_text_data_start__) - FAST_XRAM_SECTION_SIZE;

	__overlay_data_start__ = __sram_bss_end__;
	OVERLAY (__overlay_data_start__) : NOCROSSREFS AT (__load_stop_overlay_text_end)
	{
#ifndef NO_OVERLAY
		.overlay_data0
		{
			/* Explicitly place 4 bytes at section start to avoid "section type changed to PROGBITS" warnings */
			LONG(0);
#if 1
			*:preprocess.o(.data* .rodata*)
			*:filterbank.o(.data* .rodata*)

			*:mdf.o(.data* .rodata*)
			*:plc_8000.o(.data* .rodata*)
#if defined(HFP_1_6_ENABLE)
			*:plc_16000.o(.data* .rodata*)
#endif			
#if defined(_CVSD_BYPASS_)
			*:Pcm8k_Cvsd.o(.data* .rodata* .bss*)
#endif
#if defined(SPEECH_TX_2MIC_NS)
			*:dual_mic_denoise.o(.data* .rodata*)
			*:vad_filterbank.o(.data* .rodata*)
			*:energy.o(.data* .rodata*)
			*:get_scaling_square.o(.data* .rodata*)

			*:cross_correlation.o(.data* .rodata*)
			*:min_max_operations.o(.data* .rodata*)
			*:resample_by_2_internal.o(.data* .rodata*)
			*:division_operations.o(.data* .rodata*)
			*:downsample_fast.o(.data* .rodata*)
			*:resample_fractional.o(.data* .rodata*)
			*:resample_48khz.o(.data* .rodata*)
			*:vad_sp.o(.data* .rodata*)
			*:vad_core.o(.data* .rodata*)
			*:webrtc_vad.o(.data* .rodata*)
			*:vad_gmm.o(.data* .rodata*)
#endif
#if defined(SPEECH_TX_2MIC_NS2)
			*:coherent_denoise.o(.data* .rodata*)
#endif

#if defined(SPEECH_TX_NS2FLOAT)
			*:lc_mmse_ns_float.o(.data* .rodata*)
			*:cmsis_fftwrap.o(.data* .rodata*)
			*:arm_rfft_fast_f32.o(.data* .rodata*)
			*:arm_cfft_f32.o(.data* .rodata*)
			*:arm_common_tables.o(.rodata.armBitRevIndexTable256)
			*:arm_common_tables.o(.rodata.twiddleCoef_256)
			*:arm_common_tables.o(.rodata.twiddleCoef_rfft_512)
			*:arm_common_tables.o(.rodata.armBitRevIndexTable128)
			*:arm_common_tables.o(.rodata.twiddleCoef_128)
			*:arm_common_tables.o(.rodata.twiddleCoef_rfft_256)
#endif
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
			*:aacdec.o(.data* .bss*)
			*:aactabs.o(.data* .bss*)
			*:bitstream.o(.data* .bss*)
			*:dct4.o(.data* .bss*)
			*:decelmnt.o(.data* .bss*)
			*:dequant.o(.data* .bss*)
			*:fft.o(.data* .bss*)
			*:filefmt.o(.data* .bss*)
			*:huffman.o(.data* .bss*)
			*:hufftabs.o(.data* .bss*)
			*:imdct.o(.data* .bss*)
			*:noiseless.o(.data* .bss*)
			*:pns.o(.data* .bss*)
			*:stproc.o(.data* .bss*)
			*:tns.o(.data* .bss*)
			*:trigtabs.o(.data* .bss*)
			*:trigtabs_fltgen.o(.data* .bss*)
			*:buffers.o(.data* .bss* .rodata*)
			*:a2dpplay.o(.bss.*raw_aac_pcm_data)
#endif
			*(.overlay_data4 .overlay_rodata4)
			. = ALIGN(4);
		}
		.overlay_data5
		{
			LONG(0);
			*:mpa.o(.data* .bss*)
			*:bit.o(.data* .bss*)
			*:frame.o(.data* .bss*)
			*:huffman.o(.data* .bss*)
			*:layer12.o(.data* .bss*)
			*:layer3.o(.data* .bss*)
			*:stream.o(.data* .bss*)
			*:synth.o(.data* .bss*)
			*(.overlay_data5 .overlay_rodata5)
			. = ALIGN(4);
		}
		.overlay_data6
		{
			LONG(0);
#if defined(A2DP_SCALABLE_ON)
			*(.constdata)
			*:Audio_asm.o(.data* .rodata*)
			*:vq.o(.data* .rodata*)
			*:util.o(.data* .rodata*)
			*:rate.o(.data* .rodata*)
			*:quant_bands.o(.data* .rodata*)
			*:modes.o(.data* .rodata*)
			*:mdct.o(.data* .rodata*)
			*:mathops.o(.data* .rodata*)
			*:laplace.o(.data* .rodata*)
			*:entdec.o(.data* .rodata*)
			*:entcode.o(.data* .rodata*)
			*:cwrs.o(.data* .rodata*)
			*:audio_int32.o(.data* .rodata*)
			*:audio_decoder.o(.data* .rodata*)
			*:audio.o(.data* .rodata*)
			*:a2dpplay.o(.bss.*scalable_decoder_place)
			*:a2dpplay.o(.bss.*scalable_input_mid_buf)
#endif
			*(.overlay_data6 .overlay_rodata6)
			. = ALIGN(4);
		}
		.overlay_data7
		{
			LONG(0);
			*(.overlay_data7 .overlay_rodata7)
			. = ALIGN(4);
		}
#endif
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

	ASSERT(__dummy_overlay_data_end__ - __overlay_data_start__ <= OVERLAY_DATA_SECTION_SIZE, "overlay sections too large")
	
#if defined(SYSTEM_USE_PSRAM)
	__psram_text_data_start_flash__ = __load_stop_overlay_data_end;
	__psram_text_data_start__ = ORIGIN(PSRAMX);

	.psram_text (__psram_text_data_start__) :
				AT (__psram_text_data_start_flash__)
	{
		. = ALIGN(4);
		*(.psram_text)
#if 0
		*:app_bt_stream.o(.text*)
#endif
		*:app_a2dp.o(.text*)
		*:app_hfp.o(.text*)
		*:app_bt.o(.text*)
		*:besmain.o(.text*)
        *:a2dpplay.o(.text*)
        *:app_media_player.o(.text*)
		*:app_bt_media_manager.o(.text*)
#if 0
		*:btalloc.o(.text*)
		*:btrom.o(.text*)
		*:btutils.o(.text*)
		*:debug.o(.text*)
		*:eventmgr.o(.text*)
		*:radiomgr.o(.text*)
		*:rxbuff.o(.text*)
		*:hci.o(.text*)
		*:hci_evnt.o(.text*)
		*:hci_proc.o(.text*)
		*:hci_util.o(.text*)
		*:l2cap.o(.text*)
		*:l2cap_if.o(.text*)
		*:l2cap_sm.o(.text*)
		*:l2cap_utl.o(.text*)
		
		*:me.o(.text*)
		*:meaac.o(.text*)
		*:mecon.o(.text*)
		*:medev.o(.text*)
		*:mescodat.o(.text*)
		*:mescoif.o(.text*)
		*:mesec.o(.text*)

		*:sdp.o(.text*)
		*:sdpclient.o(.text*)
		*:sdpserv.o(.text*)
		*:sdputil.o(.text*)
		*:rfc_ctl.o(.text*)
		*:rfc_fcs.o(.text*)
		*:rfcomm.o(.text*)
		*:rfc_sm.o(.text*)
		*:rfc_util.o(.text*)
	
		*:a2alloc.o(.text*)
		*:a2dp.o(.text*)
		*:a2dp_sdp.o(.text*)
		*:a2dp_sm.o(.text*)
		*:avctpmsg.o(.text*)
		*:avctpcon.o(.text*)
		*:avctp.o(.text*)	
		
		*:avalloc.o(.text*)
		*:avdtp.o(.text*)
		*:avdtpcon.o(.text*)
		*:avsigmgr.o(.text*)

		*:avrcpact.o(.text*)
		*:avrcpbrws.o(.text*)
		*:avrcp.o(.text*)
		*:avrcppnl.o(.text*)
		*:avrcpsdp.o(.text*)
		*:avrcp_typ.o(.text*)
		*:avdevice.o(.text*)
		*:avtp.o(.text*)
		*:conmgr.o(.text*)
		*:hfalloc.o(.text*)
		*:hf_at.o(.text*)
		*:hf.o(.text*)
		*:hf_cmd.o(.text*)
		*:hf_cmgr.o(.text*)
		*:hf_sdp.o(.text*)
		*:hf_sm.o(.text*)
		*:hf_util.o(.text*)
		*:hidalloc.o(.text*)		
		*:hid.o(.text*)
		*:hidcon.o(.text*)
		*:hid_sdp.o(.text*)
		*:hs.o(.text*)
		*:hs_cmd.o(.text*)
#endif		
		
		
#if 1
		__opus_text_start__ = .;
		*:opus_memory.o(.text*)

		*:analysis.o(.text*)
		*:mlp_data.o(.text*)
		*:mlp.o(.text*)
		*:opus_compare.o(.text*)
		*:opus_decoder.o(.text*)
		*:opus_encoder.o(.text*)
		*:opus_multistream_decoder.o(.text*)
		*:opus_multistream_encoder.o(.text*)
		*:opus_multistream.o(.text*)
		*:opus.o(.text*)
		*:repacketizer.o(.text*)

		*:A2NLSF.o(.text*)
		*:ana_filt_bank_1.o(.text*)
		*:biquad_alt.o(.text*)
		*:bwexpander_32.o(.text*)
		*:bwexpander.o(.text*)
		*:check_control_input.o(.text*)
		*:CNG.o(.text*)
		*:code_signs.o(.text*)
		*:control_audio_bandwidth.o(.text*)
		*:control_codec.o(.text*)
		*:control_SNR.o(.text*)
		*:debug.o(.text*)
		*:dec_API.o(.text*)
		*:decode_core.o(.text*)
		*:decode_frame.o(.text*)
		*:decode_indices.o(.text*)
		*:decode_parameters.o(.text*)
		*:decode_pitch.o(.text*)
		*:decode_pulses.o(.text*)
		*:decoder_set_fs.o(.text*)
		*:enc_API.o(.text*)
		*:encode_indices.o(.text*)
		*:encode_pulses.o(.text*)
		*:gain_quant.o(.text*)
		*:HP_variable_cutoff.o(.text*)
		*:init_decoder.o(.text*)
		*:init_encoder.o(.text*)
		*:inner_prod_aligned.o(.text*)
		*:interpolate.o(.text*)
		*:lin2log.o(.text*)
		*:log2lin.o(.text*)
		*:LPC_analysis_filter.o(.text*)
		*:LPC_inv_pred_gain.o(.text*)
		*:LP_variable_cutoff.o(.text*)
		*:NLSF2A.o(.text*)
		*:NLSF_decode.o(.text*)
		*:NLSF_del_dec_quant.o(.text*)
		*:NLSF_encode.o(.text*)
		*:NLSF_stabilize.o(.text*)
		*:NLSF_unpack.o(.text*)
		*:NLSF_VQ.o(.text*)
		*:NLSF_VQ_weights_laroia.o(.text*)
		*:NSQ_del_dec.o(.text*)
		*:NSQ.o(.text*)
		*:pitch_est_tables.o(.text*)
		*:PLC.o(.text*)
		*:process_NLSFs.o(.text*)
		*:quant_LTP_gains.o(.text*)
		*:resampler_down2_3.o(.text*)
		*:resampler_down2.o(.text*)
		*:resampler.o(.text*)
		*:resampler_private_AR2.o(.text*)
		*:resampler_private_down_FIR.o(.text*)
		*:resampler_private_IIR_FIR.o(.text*)
		*:resampler_private_up2_HQ.o(.text*)
		*:resampler_rom.o(.text*)
		*:shell_coder.o(.text*)
		*:sigm_Q15.o(.text*)
		*:sort.o(.text*)
		*:stereo_decode_pred.o(.text*)
		*:stereo_encode_pred.o(.text*)
		*:stereo_find_predictor.o(.text*)
		*:stereo_LR_to_MS.o(.text*)
		*:stereo_MS_to_LR.o(.text*)
		*:stereo_quant_pred.o(.text*)
		*:sum_sqr_shift.o(.text*)
		*:table_LSF_cos.o(.text*)
		*:tables_gain.o(.text*)
		*:tables_LTP.o(.text*)
		*:tables_NLSF_CB_NB_MB.o(.text*)
		*:tables_NLSF_CB_WB.o(.text*)
		*:tables_other.o(.text*)
		*:tables_pitch_lag.o(.text*)
		*:tables_pulses_per_block.o(.text*)
		*:VAD.o(.text*)
		*:VQ_WMat_EC.o(.text*)

		*:apply_sine_window_FIX.o(.text*)
		*:autocorr_FIX.o(.text*)
		*:burg_modified_FIX.o(.text*)
		*:corrMatrix_FIX.o(.text*)
		*:encode_frame_FIX.o(.text*)
		*:find_LPC_FIX.o(.text*)
		*:find_LTP_FIX.o(.text*)
		*:find_pitch_lags_FIX.o(.text*)
		*:find_pred_coefs_FIX.o(.text*)
		*:k2a_FIX.o(.text*)
		*:k2a_Q16_FIX.o(.text*)
		*:LTP_analysis_filter_FIX.o(.text*)
		*:LTP_scale_ctrl_FIX.o(.text*)
		*:noise_shape_analysis_FIX.o(.text*)
		*:pitch_analysis_core_FIX.o(.text*)
		*:prefilter_FIX.o(.text*)
		*:process_gains_FIX.o(.text*)
		*:regularize_correlations_FIX.o(.text*)
		*:residual_energy16_FIX.o(.text*)
		*:residual_energy_FIX.o(.text*)
		*:schur64_FIX.o(.text*)
		*:schur_FIX.o(.text*)
		*:solve_LS_FIX.o(.text*)
		*:vector_ops_FIX.o(.text*)
		*:warped_autocorrelation_FIX.o(.text*)
		*:quant_bands.o(.text*)            
		*:celt_decoder.o(.text*)
		*:celt_encoder.o(.text*)
		*:celt_lpc.o(.text*)
		*:celt.o(.text*)
		*:cwrs.o(.text*)
		*:entcode.o(.text*)
		*:entdec.o(.text*)
		*:entenc.o(.text*)
		*:laplace.o(.text*)
		*:mathops.o(.text*)
		*:mdct.o(.text*)
		*:modes.o(.text*)
		*:opus_custom_demo.o(.text*)
		*:pitch.o(.text*)
		*:rate.o(.text*)

		*:vq.o(.text*)
		*:celt_kiss_fft.o(.text*)
		__opus_text_end__ = .;
		__opus_text_size__ = __opus_text_end__ - __opus_text_start__;
#endif

		. = ALIGN(4);
	} > PSRAMX

	.psram_data (PSRAMX_TO_PSRAM(ADDR(.psram_text) + SIZEOF(.psram_text))) :
				AT (__psram_text_data_start_flash__ + SIZEOF(.psram_text))
	{
		. = ALIGN(4);

		*(.psram_data)
		
		*:app_a2dp.o(.rodata*)
		*:app_hfp.o(.rodata*)
		*:app_bt.o(.rodata*)
		*:besmain.o(.rodata*)
        *:a2dpplay.o(.rodata*)
		*:app_media_player.o(.rodata*)
		*:app_bt_media_manager.o(.rodata*)

#if 0
		*:btalloc.o(.data* .rodata*)
		*:btrom.o(.data* .rodata*)
		*:btutils.o(.data* .rodata*)
		*:debug.o(.data* .rodata*)
		*:eventmgr.o(.data* .rodata*)
		*:radiomgr.o(.data* .rodata*)
		*:rxbuff.o(.data* .rodata*)
		*:hci.o(.data* .rodata*)
		*:hci_evnt.o(.data* .rodata*)
		*:hci_proc.o(.data* .rodata*)
		*:hci_util.o(.data* .rodata*)
		*:l2cap.o(.data* .rodata*)
		*:l2cap_if.o(.data* .rodata*)
		*:l2cap_sm.o(.data* .rodata*)
		*:l2cap_utl.o(.data* .rodata*)
		
		*:me.o(.data* .rodata*)
		*:meaac.o(.data* .rodata*)
		*:mecon.o(.data* .rodata*)
		*:medev.o(.data* .rodata*)
		*:mescodat.o(.data* .rodata*)
		*:mescoif.o(.data* .rodata*)
		*:mesec.o(.data* .rodata*)

		*:sdp.o(.data* .rodata*)
		*:sdpclient.o(.data* .rodata*)
		*:sdpserv.o(.data* .rodata*)
		*:sdputil.o(.data* .rodata*)
		*:rfc_ctl.o(.data* .rodata*)
		*:rfc_fcs.o(.data* .rodata*)
		*:rfcomm.o(.data* .rodata*)
		*:rfc_sm.o(.data* .rodata*)
		*:rfc_util.o(.data* .rodata*)
	
		*:a2alloc.o(.data* .rodata*)
		*:a2dp.o(.data* .rodata*)
		*:a2dp_sdp.o(.data* .rodata*)
		*:a2dp_sm.o(.data* .rodata*)
		*:avctpmsg.o(.data* .rodata*)
		*:avctpcon.o(.data* .rodata*)
		*:avctp.o(.data* .rodata*)	
		
		*:avalloc.o(.data* .rodata*)
		*:avdtp.o(.data* .rodata*)
		*:avdtpcon.o(.data* .rodata*)
		*:avsigmgr.o(.data* .rodata*)

		*:avrcpact.o(.data* .rodata*)
		*:avrcpbrws.o(.data* .rodata*)
		*:avrcp.o(.data* .rodata*)
		*:avrcppnl.o(.data* .rodata*)
		*:avrcpsdp.o(.data* .rodata*)
		*:avrcp_typ.o(.data* .rodata*)
		*:avdevice.o(.data* .rodata*)
		*:avtp.o(.data* .rodata*)
		*:conmgr.o(.data* .rodata*)
		*:hfalloc.o(.data* .rodata*)
		*:hf_at.o(.data* .rodata*)
		*:hf.o(.data* .rodata*)
		*:hf_cmd.o(.data* .rodata*)
		*:hf_cmgr.o(.data* .rodata*)
		*:hf_sdp.o(.data* .rodata*)
		*:hf_sm.o(.data* .rodata*)
		*:hf_util.o(.data* .rodata*)
		*:hidalloc.o(.data* .rodata*)		
		*:hid.o(.data* .rodata*)
		*:hidcon.o(.data* .rodata*)
		*:hid_sdp.o(.data* .rodata*)
		*:hs.o(.data* .rodata*)
		*:hs_cmd.o(.data* .rodata*)
#endif		
#if 1
		*:opus_memory.o(.rodata*)

		*:analysis.o(.rodata*)
		*:mlp_data.o(.rodata*)
		*:mlp.o(.rodata*)
		*:opus_compare.o(.rodata*)
		*:opus_decoder.o(.rodata*)
		*:opus_encoder.o(.rodata*)
		*:opus_multistream_decoder.o(.rodata*)
		*:opus_multistream_encoder.o(.rodata*)
		*:opus_multistream.o(.rodata*)
		*:opus.o(.rodata*)
		*:repacketizer.o(.rodata*)

		*:A2NLSF.o(.rodata*)
		*:ana_filt_bank_1.o(.rodata*)
		*:biquad_alt.o(.rodata*)
		*:bwexpander_32.o(.rodata*)
		*:bwexpander.o(.rodata*)
		*:check_control_input.o(.rodata*)
		*:CNG.o(.rodata*)
		*:code_signs.o(.rodata*)
		*:control_audio_bandwidth.o(.rodata*)
		*:control_codec.o(.rodata*)
		*:control_SNR.o(.rodata*)
		*:debug.o(.rodata*)
		*:dec_API.o(.rodata*)
		*:decode_core.o(.rodata*)
		*:decode_frame.o(.rodata*)
		*:decode_indices.o(.rodata*)
		*:decode_parameters.o(.rodata*)
		*:decode_pitch.o(.rodata*)
		*:decode_pulses.o(.rodata*)
		*:decoder_set_fs.o(.rodata*)
		*:enc_API.o(.rodata*)
		*:encode_indices.o(.rodata*)
		*:encode_pulses.o(.rodata*)
		*:gain_quant.o(.rodata*)
		*:HP_variable_cutoff.o(.rodata*)
		*:init_decoder.o(.rodata*)
		*:init_encoder.o(.rodata*)
		*:inner_prod_aligned.o(.rodata*)
		*:interpolate.o(.rodata*)
		*:lin2log.o(.rodata*)
		*:log2lin.o(.rodata*)
		*:LPC_analysis_filter.o(.rodata*)
		*:LPC_inv_pred_gain.o(.rodata*)
		*:LP_variable_cutoff.o(.rodata*)
		*:NLSF2A.o(.rodata*)
		*:NLSF_decode.o(.rodata*)
		*:NLSF_del_dec_quant.o(.rodata*)
		*:NLSF_encode.o(.rodata*)
		*:NLSF_stabilize.o(.rodata*)
		*:NLSF_unpack.o(.rodata*)
		*:NLSF_VQ.o(.rodata*)
		*:NLSF_VQ_weights_laroia.o(.rodata*)
		*:NSQ_del_dec.o(.rodata*)
		*:NSQ.o(.rodata*)
		*:pitch_est_tables.o(.rodata*)
		*:PLC.o(.rodata*)
		*:process_NLSFs.o(.rodata*)
		*:quant_LTP_gains.o(.rodata*)
		*:resampler_down2_3.o(.rodata*)
		*:resampler_down2.o(.rodata*)
		*:resampler.o(.rodata*)
		*:resampler_private_AR2.o(.rodata*)
		*:resampler_private_down_FIR.o(.rodata*)
		*:resampler_private_IIR_FIR.o(.rodata*)
		*:resampler_private_up2_HQ.o(.rodata*)
		*:resampler_rom.o(.rodata*)
		*:shell_coder.o(.rodata*)
		*:sigm_Q15.o(.rodata*)
		*:sort.o(.rodata*)
		*:stereo_decode_pred.o(.rodata*)
		*:stereo_encode_pred.o(.rodata*)
		*:stereo_find_predictor.o(.rodata*)
		*:stereo_LR_to_MS.o(.rodata*)
		*:stereo_MS_to_LR.o(.rodata*)
		*:stereo_quant_pred.o(.rodata*)
		*:sum_sqr_shift.o(.rodata*)
		*:table_LSF_cos.o(.rodata*)
		*:tables_gain.o(.rodata*)
		*:tables_LTP.o(.rodata*)
		*:tables_NLSF_CB_NB_MB.o(.rodata*)
		*:tables_NLSF_CB_WB.o(.rodata*)
		*:tables_other.o(.rodata*)
		*:tables_pitch_lag.o(.rodata*)
		*:tables_pulses_per_block.o(.rodata*)
		*:VAD.o(.rodata*)
		*:VQ_WMat_EC.o(.rodata*)

		*:apply_sine_window_FIX.o(.rodata*)
		*:autocorr_FIX.o(.rodata*)
		*:burg_modified_FIX.o(.rodata*)
		*:corrMatrix_FIX.o(.rodata*)
		*:encode_frame_FIX.o(.rodata*)
		*:find_LPC_FIX.o(.rodata*)
		*:find_LTP_FIX.o(.rodata*)
		*:find_pitch_lags_FIX.o(.rodata*)
		*:find_pred_coefs_FIX.o(.rodata*)
		*:k2a_FIX.o(.rodata*)
		*:k2a_Q16_FIX.o(.rodata*)
		*:LTP_analysis_filter_FIX.o(.rodata*)
		*:LTP_scale_ctrl_FIX.o(.rodata*)
		*:noise_shape_analysis_FIX.o(.rodata*)
		*:pitch_analysis_core_FIX.o(.rodata*)
		*:prefilter_FIX.o(.rodata*)
		*:process_gains_FIX.o(.rodata*)
		*:regularize_correlations_FIX.o(.rodata*)
		*:residual_energy16_FIX.o(.rodata*)
		*:residual_energy_FIX.o(.rodata*)
		*:schur64_FIX.o(.rodata*)
		*:schur_FIX.o(.rodata*)
		*:solve_LS_FIX.o(.rodata*)
		*:vector_ops_FIX.o(.rodata*)
		*:warped_autocorrelation_FIX.o(.rodata*)
		*:quant_bands.o(.rodata*)            
		*:celt_decoder.o(.rodata*)
		*:celt_encoder.o(.rodata*)
		*:celt_lpc.o(.rodata*)
		*:celt.o(.rodata*)
		*:cwrs.o(.rodata*)
		*:entcode.o(.rodata*)
		*:entdec.o(.rodata*)
		*:entenc.o(.rodata*)
		*:laplace.o(.rodata*)
		*:mathops.o(.rodata*)
		*:mdct.o(.rodata*)
		*:modes.o(.rodata*)
		*:opus_custom_demo.o(.rodata*)
		*:pitch.o(.rodata*)
		*:rate.o(.rodata*)

		*:vq.o(.rodata*)
		*:celt_kiss_fft.o(.rodata*)
#endif

		. = ALIGN(4);
	} > PSRAM

	__psram_text_data_end_flash__ = __psram_text_data_start_flash__ +
								SIZEOF(.psram_text) + SIZEOF(.psram_data);

	.psram_bss (.) (NOLOAD) :
	{
		. = ALIGN(4);
		__psram_bss_start__ = .;
		*(.psram_bss)
#if 0		
		*:app_a2dp.o(.bss*)
		*:app_hfp.o(.bss*)
		*:app_bt.o(.bss*)
		*:besmain.o(.bss*)
        *:a2dpplay.o(.bss*)
		*:app_media_player.o(.bss*)
		*:app_bt_media_manager.o(.bss*)
#endif
#if 0
		*:btalloc.o(.bss*)
		*:btrom.o(.bss*)
		*:btutils.o(.bss*)
		*:debug.o(.bss*)
		*:eventmgr.o(.bss*)
		*:radiomgr.o(.bss*)
		*:rxbuff.o(.bss*)
		*:hci.o(.bss*)
		*:hci_evnt.o(.bss*)
		*:hci_proc.o(.bss*)
		*:hci_util.o(.bss*)
		*:l2cap.o(.bss*)
		*:l2cap_if.o(.bss*)
		*:l2cap_sm.o(.bss*)
		*:l2cap_utl.o(.bss*)
		
		*:me.o(.bss*)
		*:meaac.o(.bss*)
		*:mecon.o(.bss*)
		*:medev.o(.bss*)
		*:mescodat.o(.bss*)
		*:mescoif.o(.bss*)
		*:mesec.o(.bss*)

		*:sdp.o(.bss*)
		*:sdpclient.o(.bss*)
		*:sdpserv.o(.bss*)
		*:sdputil.o(.bss*)
		*:rfc_ctl.o(.bss*)
		*:rfc_fcs.o(.bss*)
		*:rfcomm.o(.bss*)
		*:rfc_sm.o(.bss*)
		*:rfc_util.o(.bss*)
	
		*:a2alloc.o(.bss*)
		*:a2dp.o(.bss*)
		*:a2dp_sdp.o(.bss*)
		*:a2dp_sm.o(.bss*)
		*:avctpmsg.o(.bss*)
		*:avctpcon.o(.bss*)
		*:avctp.o(.bss*)	
		
		*:avalloc.o(.bss*)
		*:avdtp.o(.bss*)
		*:avdtpcon.o(.bss*)
		*:avsigmgr.o(.bss*)

		*:avrcpact.o(.bss*)
		*:avrcpbrws.o(.bss*)
		*:avrcp.o(.bss*)
		*:avrcppnl.o(.bss*)
		*:avrcpsdp.o(.bss*)
		*:avrcp_typ.o(.bss*)
		*:avdevice.o(.bss*)
		*:avtp.o(.bss*)
		*:conmgr.o(.bss*)
		*:hfalloc.o(.bss*)
		*:hf_at.o(.bss*)
		*:hf.o(.bss*)
		*:hf_cmd.o(.bss*)
		*:hf_cmgr.o(.bss*)
		*:hf_sdp.o(.bss*)
		*:hf_sm.o(.bss*)
		*:hf_util.o(.bss*)
		*:hidalloc.o(.bss*)		
		*:hid.o(.bss*)
		*:hidcon.o(.bss*)
		*:hid_sdp.o(.bss*)
		*:hs.o(.bss*)
		*:hs_cmd.o(.bss*)
#endif




		. = ALIGN(4);
		__psram_bss_end__ = .;
	} > PSRAM
#endif

#if defined(SYSTEM_USE_PSRAM)
	.text (FLASH_TO_FLASHX(__psram_text_data_end_flash__)) : AT (FLASHX_TO_FLASH(ADDR(.text)))
#else
	.text (FLASH_TO_FLASHX(__load_stop_overlay_data_end)) : AT (FLASHX_TO_FLASH(ADDR(.text)))
#endif
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

	.thirdparty.event.table (.) :
	{
	. = ALIGN(4);
	__thirdparty_handler_table_start = .;
		KEEP(*(.thirdparty_handler_table))		
	__thirdparty_handler_table_end = .;	
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
	__overflow_ram = __HeapLimit - __StackLimit;

	. = __etext + SIZEOF(.data);
	.system_info (.) :
	{
		KEEP(*(.system_info_list_size))
		KEEP(*(.system_info_list))
		. = ALIGN(4);
	}  > FLASH
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

