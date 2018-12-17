#**************************************************************************
#                                                                         *
#   PROJECT     : MIPS port for UCOS-II                                   *
#                                                                         *
#   MODULE      : MakeFile                                                *
#                                                                         *
#   AUTHOR      : mengzh                                                  *
#                                                                         *
#   PROCESSOR   : MIPS 4KEc (32 bit RISC) - Actions board                 *
#                                                                         *
#   Tool-chain  :  Cygnus                                                 *
#                                                                         *
#   DESCRIPTION :                                                         *
#                                                                         *
#   targets are :                                                         *
#                                                                         *
#*************************************************************************/
# **********************************************
# Type of Processor	& Board
# **********************************************
Processor =	mips
# **********************************************
# Directories
# **********************************************
#以makefile文件的存放目录为起始点，指定代码管理的根目录的相对路径 uvob
ROOT	  =	./../../../../
#同时为了方便，可以指定其他目录，引用一个变量的格式为 $(变量名称)
FWSPROOT   = $(ROOT)/fwsp
PSPLIBDIR    = $(ROOT)/US282A/psp_rel/lib
PSPRELROOT = $(ROOT)/US282A/psp_rel

# **********************************************
# Name of application 开发人员需修改处(1)
IMAGENAME = nor_id
#所要编译的源文件的存放位置,开发人员需修改处(2)
SRC = ..
SRCDIR_16 = $(SRC)/nor_id 
SRCDIR_32 = 
#目标文件送往的文件夹路径,开发人员需修改处(3)
OBJECT_BIN_PATH = $(ROOT)/US282A/case/fwpkg/bin
# **********************************************

#工具名称
DRV_BUILDER = $(ROOT)/US282A/psp_rel/tools/drv_builder.exe
AP_BUILDER = $(ROOT)/US282A/psp_rel/tools/ap_builder.exe
MMM_CODEC_BUILDER = $(ROOT)/US282A/psp_rel/tools/mmm_codec_builder.exe 
OBJCOPY   = sde-objcopy 
#存放结果文件的目录，这里的设置为当前目录，即生成的结果文件放在当前目录
LINKDIR	  =	.
#指定依赖过程的文件的搜索路径，把源文件的路径写上即可
VPATH	=	$(SRCDIR_16) $(SRCDIR_32)
# **********************************************
# Image	file names and map file
# **********************************************
#指定结果文件的名称
IMAGE_ELF =	$(IMAGENAME).exe
IMAGE_LST =	$(IMAGENAME).lst
IMAGE_REC =	$(IMAGENAME).rec
IMAGE_BIN =	$(IMAGENAME).sys
MAP		  =	$(IMAGENAME).map
TARGET_DRV = $(IMAGENAME).drv
TARGET_BIN = $(IMAGENAME).bin
TARGET_EXT   = exe lst map bin info
TARGET_FILES = $(foreach n,$(TARGET_EXT),$(IMAGENAME).$(n))
# **********************************************
# Compiler and linker options
# **********************************************
#指定程序中引用的头文件的搜索路径，一般源码include时只写上头文件的名称，没有路径信息。这时就根据这个选项去搜索该头文件
INCLUDE	  =	 -I$(PSPRELROOT)/include/ucos -I$(PSPRELROOT)/include -I$(SRC)/nor_bsd  -I$(SRC)/nor_brec -I$(SRC)/nor_ksd  -I$(FWSPROOT)/inc \
			 -I$(PSPRELROOT)/include/asm-mips/mach-gl5116  -I$(PSPRELROOT)/include/asm-mips \
			 -I$(SRC)/common/include 

CC_OPTS    	    = -G0 -g -Wall -fsigned-char -mtune=4kec -mips32r2 -fno-builtin $(INCLUDE) -EL -c
CC        		= sde-gcc
LD              = sde-ld
OBJCOPY         = sde-objcopy
OBJDUMP         = sde-objdump
.PHONY : all
all:$(IMAGENAME)

$(IMAGENAME):
	$(CC) $(INCLUDE) -O2 $(CC_OPTS) spi_nor_id_tbl.c -o spi_nor_id_tbl.o
	$(OBJCOPY) -O binary spi_nor_id_tbl.o $@.bin
	cp $@.bin $(OBJECT_BIN_PATH)/$@.bin 

	rm *.o
	@echo ----------------------------------------
	@echo 恭喜您,编译成功！（小端）

	
clean:
	rm -f *.o *.bin *.exe *.elf *.lst *.map 

