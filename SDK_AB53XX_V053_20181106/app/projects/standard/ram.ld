ENTRY(_start)

/* Define the flash max size */
__max_flash_size = 768k;

/* Define the area size */
__comm_ram_size = 34k;
__heap_ram_size = 12k;
__cram_ram_size = 18k;

__base = 0x10000000;

/* Define the area */
__aram_vma = 0x50000;
__bram_vma = 0x18000;
__dram_vma = 0x54000;
__eram_vma = 0x57000;
__fram_vma = 0x17000;

__comm_vma = 0x20000;
__heap_vma = __comm_vma + __comm_ram_size;
__cram_vma = __heap_vma + __heap_ram_size;

__bcomm_vma = __bram_vma + 24k;

MEMORY
{
    init            : org = __base,             len = 512
    flash(rx)       : org = __base + 512,       len = __max_flash_size
    comm(rx)        : org = __comm_vma,         len = __comm_ram_size
    bcomm(rx)       : org = __bcomm_vma,        len = 8k

    stack           : org = 0x10800,            len = 1k
    data            : org = 0x10C00,            len = 25k
    heap            : org = __heap_vma,         len = __heap_ram_size
    aram            : org = __aram_vma,         len = 16k
    bram            : org = __bram_vma,         len = 24k
    cram            : org = __cram_vma,         len = __cram_ram_size
    dram            : org = __dram_vma,         len = 12k
    eram            : org = __eram_vma,         len = 0xa00
    fram            : org = __fram_vma,         len = 4k
}

SECTIONS
{
    .init : {
        *(.reset)
    } > init

    .bcomm : {
        *(.com_text.bb.btisr)
        *(.com_text.bb.rf)
        *(.com_text.bb.sco)
        . = ALIGN(512);
    } > bcomm AT > flash

    .comm : {
        *(.vector)
        *(.plt)
        *(.com_text*)
        *(.com_rodata*)
        *debug.o(.rodata*)
        *(.data*)
        *(.sdata*)
        *(.load_text)
        *(.load_rodata)
        . = ALIGN(512);
    } > comm AT > flash

    .flash : {
        __code_start_sbcdec = .;
        *(.sbcdec*)
        *(.sbc_com*)
        . = ALIGN(512);
        __code_end_sbcdec = .;

        __code_start_mp3dec = .;
        *(.mp3dec*)
        . = ALIGN(512);
        __code_end_mp3dec = .;

        __code_start_sniff = .;
        *(.sniff*)
        . = ALIGN(512);
        __code_end_sniff = .;

        __code_start_wavdec = .;
        *(.wavdec*)
        . = ALIGN(512);
        __code_end_wavdec = .;

        __code_start_wmadec = .;
        *(.wmadec*)
        . = ALIGN(512);
        __code_end_wmadec = .;

        __code_start_apedec = .;
        *(.apedec*)
        . = ALIGN(512);
        __code_end_apedec = .;

        __code_start_flacdec = .;
        *(.flacdec*)
        . = ALIGN(512);
        __code_end_flacdec = .;

        __code_start_mp3enc = .;
        *(.mp3enc*)
        . = ALIGN(512);
        __code_end_mp3enc = .;

        __code_start_mp2enc = .;
        *(.mp2enc*)
        . = ALIGN(512);
        __code_end_mp2enc = .;

        __code_start_usbdev = .;
        *(.usbdev*)
        . = ALIGN(512);
        __code_end_usbdev = .;

        __code_start_wma_rlc44q = .;
        *(.wma_huff_rlc44q_table)
        . = ALIGN(512);
        __code_end_wma_rlc44q = .;

        __code_start_wma_rlc44o = .;
        *(.wma_huff_rlc44o_table)
        . = ALIGN(512);
        __code_end_wma_rlc44o = .;

        __code_start_wma_rlc16 = .;
        *(.wma_huff_rlc16_table)
        . = ALIGN(512);
        __code_end_wma_rlc16 = .;

        __code_start_sleep = .;
        *(.saradc_text*)
        *(.sleep_text*)
        . = ALIGN(512);
        __code_end_sleep = .;

        __code_start_bb_test = .;
        *(.bb_test*)
        *(.bb_test_rodata*)
        . = ALIGN(512);
        __code_end_bb_test = .;

        __code_start_bt_voice = .;
        *(.bt_voice*)
        . = ALIGN(512);
        __code_end_bt_voice = .;

        __code_start_update = .;
        *(.text.update*)
        . = ALIGN(512);
        __code_end_update = .;

        *(.irq_init.aligned)
        *(.text*)
        *(.rodata*)
        *(.rela*)
        LONG(0)
        . = ALIGN(512);

    } > flash

    .stack (NOLOAD) : {
        __irq_stack_start = .;
        . = 0x400;
        __irq_stack = .;
    } > stack
    __irq_stack_size = __irq_stack - __irq_stack_start;

    .fram_a2dp __fram_vma (NOLOAD) : {
        *(.bss.a2dp_play)
    } > fram

    .fram_buf __fram_vma (NOLOAD) : {
        *(.buf.mp3)
        *(.buf.deccom)
        *(.buf.fmrx.cb)
        *(.buf.sbcen)
        *(.rec.wave.header)
    } > fram

    .bss (NOLOAD) : {
        __bss_start = .;
        *(COMMON)
        *(.bss*)
        *(.sbss*)
        *(.buf*)
        *(.sdadc_buf)
        *(.dac_obuf)
        __bss_end = .;
    } > data
    __bss_size = __bss_end - __bss_start;

    .heap (NOLOAD) : {
        *(.mem_heap)
    } > heap

    .bram __bram_vma (NOLOAD) : {
        *(.btmem.bthw)
        *(.btmem*)
    } > bram

    .bram_echo __bram_vma (NOLOAD) : {
        *(.bram_echo)
    } > bram

    .cram_flac __cram_vma (NOLOAD) : {
        *(.flac_decoded_buf)
    } > cram

    .cram_upd __cram_vma (NOLOAD) : {
        *(.upd_buf*)
        *(.fcc_buf*)
    } > cram

    .cram_wma __cram_vma (NOLOAD) : {
        *(.wma_coefs_buf)
        *(.wma_ctx_buf)
        *(.wma_wincb_buf)
    } > cram

    .cram_mp3enc __cram_vma (NOLOAD) : {
        *(.mp3_enc_buf)
        *(.recenc.obuf)
    } > cram

    .cram_mp2enc __cram_vma (NOLOAD) : {
        *(.l2enc_sb_buf)
        . = 0x2400;
    } > cram

    .cram_echo __cram_vma (NOLOAD) : {
        *(.cram_echo)
    } > cram

    .cram_fmrx __cram_vma (NOLOAD) : {
        *(.fmrx_pcm_buf)
    } > cram

    .aec_ram __aram_vma (NOLOAD) : {
        *(.aec_buf)
    } > aram

    .echo_ram __aram_vma (NOLOAD) : {
        *(.echo_buf)
    } > aram

    .sbc_cache __aram_vma (NOLOAD) : {
        *(.sbc_cache_buf)
    } > aram

    .aram_music __aram_vma (NOLOAD) : {
        *(.mp3buf.dec)
        *(.mp3_id3_buf)
        *(.pff.scan.tempbuf)
        *(.fmrx_cb_buf)
        *(.mp3enbuf.cb)
    } > aram

    .dram_mp3 __dram_vma (NOLOAD) : {
        *(.mp3_decram_buf)
        . = 0x3000;
    } > dram

    .dram_ape __dram_vma (NOLOAD) : {
        *(.apebuf.cb)
        *(.apebuf.filter)
        *(.apebuf.decode)
        *(.apebuf.seektbl)
        . = 0x3000;
    } > dram

    .dram_wma __dram_vma (NOLOAD) : {
        *(.wmabuf*)
        . = 0x3000;
    } > dram

    .dram_flac __dram_vma (NOLOAD) : {
        *(.flacbuf*)
        *(.wavbuf.dec)
        . = 0x3000;
    } > dram

    .dram_sco __dram_vma (NOLOAD) : {
        *(.sbc_decram_buf)
        *(.sbcbuf.dec)
        *(.sco_data)
        *(.sco_cache)
        . = 0x3000;
    } > dram

    .dram_rec __dram_vma (NOLOAD) : {
        *(.rec.sco)
        . = 0x1400;
        *(.recbuf.cb)
        *(.rec.obuf)
        *(.mp3enbuf*)
        *(.l2enc_smr_buf)
        . = 0x3000;
    } > dram

    .dram_usb __dram_vma (NOLOAD) : {
        *(.usb.device.desc.buf)
        *(.ude.aubuf)
        . = 0x1400;
        . = 0x3000;
    } > dram

    .dram_spf __dram_vma (NOLOAD) : {
        *(.spf_buf)
        . = 0x1400;
        . = 0x3000;
    } > dram

    .eram_mp3enc __eram_vma (NOLOAD) : {
        *(.mp3_enc_hwbuf)
        . = 0xa00;
    } > eram

    .eram_mp2enc __eram_vma (NOLOAD) : {
        *(.l2enc.buf*)
        . = 0xa00;
    } > eram

    .eram_adpcmenc __eram_vma (NOLOAD) : {
        *(.adpcmenc.buf)
        . = 0xa00;
    } > eram

    .eram_wavres __eram_vma (NOLOAD) : {
        *(.wavres.buf)
        . = 0xa00;
    } > eram

    .fram_mav __fram_vma (NOLOAD) : {
        . = 0x200;
        *(.mav*)
    } > fram
}

/* Calc the lma */
__bank_size = SIZEOF(.flash);
__comm_lma = LOADADDR(.comm);
__comm_size = SIZEOF(.comm);
__bcomm_lma = LOADADDR(.bcomm);
__bcomm_size = SIZEOF(.bcomm);
