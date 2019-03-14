ENTRY(_start)

/* Define the flash max size */
__max_flash_size = 768k;

__base = 0x10000000;

/* Define the area */
__aram_vma = 0x50000;
__bram_vma = 0x18000;
__dram_vma = 0x54000;

__comm_vma = 0x14000;

MEMORY
{
    init            : org = __base,             len = 512
    flash(rx)       : org = __base + 512,       len = __max_flash_size
    comm(rx)        : org = __comm_vma,         len = 16k

    stack           : org = 0x10800,            len = 1k
    data            : org = 0x10C00,            len = 13k
    heap            : org = 0x1d800,            len = 10k
    aram            : org = __aram_vma,         len = 16k
    bram            : org = __bram_vma,         len = 22k
    dram            : org = __dram_vma,         len = 12k
}

SECTIONS
{
    .init : {
        *(.reset)
    } > init

    .comm : {
        *(.vector)
        *(.plt)
        *(.com_text*)
        *(.com_text.bb.leisr)
        *(.com_text.stack.handler)
        *(.com_rodata*)
        *debug.o(.rodata*)
        *(.data*)
        *(.sdata*)
        *\ext_scan.o(.rodata*)
        *\ble_evt.o(.rodata*)
        *\hci_transport_h3.o(.rodata*)
        *(.load_text)
        *(.load_rodata)
        . = ALIGN(512);
    } > comm AT > flash

    .flash : {
        __code_start_bt_comm = .;
        *(.bcom_text*)
        *(.bcom_rodata*)
        . = ALIGN(512);
        __code_end_bt_comm = .;

        __code_start_mp3dec = .;
        *(.mp3dec*)
        . = ALIGN(512);
        __code_end_mp3dec = .;

        __code_start_sniff = .;
        *(.sniff*)
        . = ALIGN(512);
        __code_end_sniff = .;

        __code_start_sbcdec = .;
        *(.sbcdec*)
        *(.sbc_com*)
        . = ALIGN(512);
        __code_end_sbcdec = .;

        __code_start_mp3enc = .;
        *(.mpaenc*)
        . = ALIGN(512);
        __code_end_mp3enc = .;

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
        *(.charge_text*)
        . = ALIGN(512);
        __code_end_sleep = .;

        __code_start_pwrsave = .;
        *(.pwrsave_text*)
        *(.pwrsave_rodata*)
        . = ALIGN(512);
        __code_end_pwrsave = .;

        __code_start_bb_test = .;
        *(.bb_test*)
        *(.bb_test_rodata*)
        . = ALIGN(512);
        __code_end_bb_test = .;

        __code_start_bt_voice = .;
        *(.bt_voice*)
        . = ALIGN(512);
        __code_end_bt_voice = .;

         __code_start_piano = .;
        *(.piano_com*)
        . = ALIGN(512);
        __code_end_piano = .;

        __code_start_record = .;
        *(.com_rec*)
        . = ALIGN(512);
        __code_end_record = .;

        __code_start_update = .;
        *(.text.update*)
        . = ALIGN(512);
        __code_end_update = .;

        __code_start_eq = .;
        *(.com_eq.text*)
        . = ALIGN(512);
        __code_end_eq = .;

        __code_start_stream = .;
        *(.com_stream.text*)
        . = ALIGN(512);
        __code_end_stream = .;

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

    .bss (NOLOAD) : {
        __bss_start = .;
        *(COMMON)
        *(.bss*)
        *(.sbss*)
        *(.buf*)
        *(.dac_obuf)
        *(.btstack_hci.buf)
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

    .flac_btmem __bram_vma (NOLOAD) : {
        *(.flac_decoded_buf)
        *(.spf_buf)
    } > bram

    .wma_btmem __bram_vma (NOLOAD) : {
        *(.wma_coefs_buf)
        *(.wma_ctx_buf)
        *(.wma_wincb_buf)
        *(.wma_id3_buf)
    } > bram

    .mp3enc_btmem __bram_vma (NOLOAD) : {
        *(.l2enc_sb_buf)
    } > bram

    .fmrx_btmem __bram_vma (NOLOAD) : {
        *(.fmrx_pcm_buf)
    } > bram

    .echo_btmem __bram_vma (NOLOAD) : {
        *(.echo_buf)
    } > bram

    .aec_ram __aram_vma (NOLOAD) : {
        *(.aec_buf)
        *(.wavenc.aec)
        . = 0x3800;
        *(.echo_cache)
        . = 0x4000;
    } > aram
	
    .fcc_buf __aram_vma (NOLOAD) : {
        . = 0x1000;
        *(.fcc_buf.huart)
    } > aram
	
    .sbc_cache __aram_vma (NOLOAD) : {
        . = 0x670;
        *(.a2dp_buf*)
        *(.mav_buf)
        . = 0x1000;
        *(.sbc_cache_buf)
        . = 0x3800;
        . = 0x3a2c;
        *(.ble_buf*)
        . = 0x4000;
    } > aram

    .aram_music __aram_vma (NOLOAD) : {
        *(.mp3buf.dec)
        . = 0x1000;
        *(.usb_buf*)
        *(.mp3_id3_buf)
        *(.pff.scan.tempbuf)
        *(.pff_buf*)
        *(.lrc_buf*)
        . = 0x3800;
        *(.avio_buf)
        *(.diskio_buf)
        *(.pff.buf)
        *(.fname.buf)
        . = 0x4000;
    } > aram

    .aram_fmrx __aram_vma (NOLOAD) : {
        . = 0x1800;
        *(.fmrx_cb_buf)
    } > aram

    .aram_sbc __aram_vma (NOLOAD) : {
        *(.sbcbuf.dec)
        *(.l2enc.buf*)
        . = 0x1000;
    } > aram

    .aram_msc_stm __aram_vma (NOLOAD) : {
        *(.stm_buf)
        . = 0x1000;
    } > aram

    .aram_upd __aram_vma (NOLOAD) : {
        *(.upd_ota*)
        . = 0x3200;
        *(.upd_buf*)
        . = 0x3800;
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

    .dram_sbc __dram_vma (NOLOAD) : {
        *(.sbc_decram_buf)
        . = 0xD38;
        *(.a2dp_play)
        *(.mav_cache)
        *(.ble_cache*)
        . = 0x3000;
    }  > dram

    .dram_sco __dram_vma (NOLOAD) : {
        *(.sco_data)
        *(.sco_cache)
        *(.sco_hci)
        *(.plc_buf)
        *(.plc_data)
        *(.wavenc.sco)
        . = 0x3000;
    } > dram

    .dram_rec __dram_vma (NOLOAD) : {
        . = 0xD38;
        *(.rec.wave.header)
        *(.recenc.obuf)
        *(.rec.obuf)
        *(.l2enc_bit_buf)
        . = 0x3000;
    } > dram

    .dram_usb __dram_vma (NOLOAD) : {
        *(.usb.device.desc.buf)
        *(.ude.aubuf)
        . = 0x3000;
    } > dram
}

/* Calc the lma */
__bank_size = SIZEOF(.flash);
__comm_lma = LOADADDR(.comm);
__comm_size = SIZEOF(.comm);
