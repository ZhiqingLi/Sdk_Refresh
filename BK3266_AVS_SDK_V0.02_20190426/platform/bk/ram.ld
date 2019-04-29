/* 
 * Script for GNU linker.
 * Describes layout of sections, location of stack.
 *
 * In this case vectors are at location 0 (reset @ 0x08)
 *
 * +------------+  0x00400000
 * data         |
 * end
 * |(heap)      |
 * .            .
 * .            .
 * |(heap limit)|
 *
 * |- - - - - - |
 * stack bottom    64k
 * +------------+  
 *
 * +------------+  0x0000000
 * |vectors     |
 * |            |
 * |------------+
 * |text        |  0x0000300
 * |data        |
 * |            |  512k
 * +------------+
 */

/* memory:
 *  D-QMEM                     64kB (start at 00400000)
 *  IAHB-MEM EBI Flash         96kB (start at 00000000, mapped to 00000000)
 *  CACHES 2way x8K/way        16kB
 */

/* Split memory into area for vectors and ram */

/* _HALF_1ST_CODE_ADDR  =   0x00000F20;  */
/* _HALF_2ND_CODE_ADDR  =   0x000F0F20; */
_HALF_1ST_MARK	 = 0x5aa56630;
_HALF_2ND_MARK   = 0xa55a6631;
_HALF_1ST_CFG	 = 0xED800;
_HALF_2ND_CFG	 = 0x1DE800;

_CFG_OFFSET_ = 0;
_software_mark_	 = _HALF_1ST_MARK;
_FLASH_ENVCFG_DEF_ADDR    	  = _HALF_1ST_CFG; 

MEMORY
	{
	boot_section     (rx)  : ORIGIN = 0x00000000, 			LENGTH = 3872
	flash            (rx)  : ORIGIN = 0x00000F20, 			LENGTH = 983040
	ram     (rw!x): ORIGIN = 0x00400000, LENGTH = 112k
       iqmem   (rw!x): ORIGIN = 0x0041c000, LENGTH = 0k 
	}

/* start execution at default reset vector location */
ENTRY(_reset_vector);
_reset_vector = 0x00000008;

SECTIONS
{
/* vectors go to vectors region */
	.vectors : 
	{ 
		*(.boot_vectors)
		. = ALIGN (8);
		*(boot_code boot_code.*) 
		. = ALIGN (8);           
	} > boot_section
	
	. = ORIGIN(flash); 
	.flash_magic :     
	{                  
		*(.boot_magic_word) 
	} > flash 
	

/* instructions go to the text region*/

	. = ALIGN(0x8);

/* code, instructions    i=i+1; */
	.flash_code : 
	{
		*(.boot_handlers)
		. = ALIGN (4);
		*(ram_code ram_code.*)
		
		. = ALIGN (4);
		*(dram_code dram_code.*)
		
		. = ALIGN (4);
		/* *(mp3_code mp3_code.*) */
		/* . = ALIGN (4); */
		*(ram_test_code ram_test_code.*)
		. = ALIGN (4);
		*(flash_code flash_code.* )
        . = ALIGN(0x8);
	} > flash
	
/* functions that should be put in iqmem */
   .= ORIGIN(iqmem) + 16;
   .text.iqmem ALIGN(8) :
   {  					
	*(dram_code dram_code.*) 
	. = ALIGN (8);		     
   }   > iqmem  AT>flash       

   _iqmem_start_lma = LOADADDR(.text.iqmem); 
   _iqmem_start_vma = ADDR(.text.iqmem);		
   _iqmem_end_vma = ADDR(.text.iqmem) + SIZEOF(.text.iqmem); 
   
/* move the pointer back to the rom location */
   . = ORIGIN(flash) + SIZEOF(.flash_code) + SIZEOF(.text.iqmem);
   
/* code, instructions    i=i+1; */
	.text : 
	{
		*(.text .text.* ) 
	} > flash

/* read only data    const int rom_data[3]={1,2,3}; */
	.rodata ALIGN(8) : 
	{
		*(.rodata)
		. = ALIGN (4);
		*(.rodata.*)
		*(OPUS_CONST_DATA)
	} > flash					/* in flash */

	. = ORIGIN(ram);

/* globals  int ram_data[3]={4,5,6}; */
	.data ALIGN(8) : AT( ALIGN( LOADADDR(.rodata)+SIZEOF(.rodata), 4 ) )		/* VMA in RAM, but keep LMA in flash */
	{
		*(.data .data.*)
		/* *(.rodata) */
		/* *(.rodata.*) */
	} >ram
	
	/* Loader will copy data from _flash_begin to _ram_begin..ram_end */
	_data_flash_begin = LOADADDR(.data);
	_data_ram_begin = ADDR(.data);
	_data_ram_end = .;

/* small read-write data section */
	.sdata ALIGN(8) : AT( ALIGN( LOADADDR(.data)+SIZEOF(.data), 4 ) )		/* VMA in RAM, but keep LMA in flash */
	{ 
		*(.sdata) 
	} >ram

	/* Loader will copy sdata from _flash_begin to ram_begin..ram_end */
	_sdata_flash_begin = LOADADDR(.sdata);
	_sdata_ram_begin = ADDR(.sdata);
	_sdata_ram_end = .;


/* frame unside informatino for exception handling */
	/DISCARD/ :
	{
		*(.eh_frame)
	}


/* global pointer in middle of data, so as much as possible is accessible */
	. = ALIGN(4);  __gp = . + 0x1000;


/* uninitialized data section - global   int i; */
	.bss ALIGN(8):
	{
		_bss_start = .;
		. = ALIGN (4);
		*(.bss .bss.*)
		. = ALIGN (4);
		_bss_end = .;
	} > ram						/* in RAM */

	. = ALIGN (8);
	_empty_ram = .;

/* small uninitialized data section */
	.sbss ALIGN(8): 
	{
		_sbss_start = .;
		. = ALIGN (4);
		*(.scommon)
		. = ALIGN (4);
		*(.sbss)
        . = ALIGN (4);
		_sbss_end = .;
	} > ram
	

	PROVIDE(_FLASH_ENVDATA2_DEF_ADDR		  = 1024k - 0x1000 + _CFG_OFFSET_); 
	PROVIDE(_FLASH_ENVDATA_DEF_ADDR		  = _FLASH_ENVDATA2_DEF_ADDR - 0x1000);
	PROVIDE(_FLASH_CHANGED_BT_ADDR		  = _FLASH_ENVDATA_DEF_ADDR - 0x1000);

/* This symbol defines end of code/data sections. Heap starts here. */
	PROVIDE(end    	  = .);
	
/* Define symbol for heap limit. Heap spans from end to here. When this is exhausted,
   heap allocations will fail. If you don't use heap (malloc etc.) this is irrelevant.
   This also defines the size of stack that will not be overwritten by dynamic
   allocations. If you need more/less stack, modify below definition.
   Default memory area reserved for stack is 8k here.
 */
	PROVIDE(_heap_limit = 4M + 112k - 8k); /* 4M + 84k - 8k*/
	
/* _stack symbol defines initial stack bottom addres. Stack grows to lower addresses.
   Typically you set this to be top of your RAM. Note: code never checks, if stack
   grows into heap area!
*/
	PROVIDE(_stack      = 4M + 112k);/*4M + 84k*/
}

