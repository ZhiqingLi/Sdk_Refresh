# **********************************************
# Type of Processor	& Board
# **********************************************
Processor =	mips
# **********************************************
# Directories
# **********************************************
#以makefile文件的存放目录为起始点，指定代码管理的根目录的相对路径 uvob
ROOT	  =	./../../../..
#同时为了方便，可以指定其他目录，引用一个变量的格式为 $(变量名称)
PSPRELROOT = $(ROOT)/psp_rel
PSPLIBDIR    = $(ROOT)/psp_rel/lib
ENHANCED_SRC = $(PSPRELROOT)/usermodule/enhanced
PSPLIB_EHANCED = $(PSPLIBDIR)/lib_enhanced

# **********************************************
# Name of application 开发人员需修改处(1)
IMAGENAME = enhanced_simple
#所要编译的源文件的存放位置,开发人员需修改处(2)

SRC_FSEL = $(ENHANCED_SRC)/fsel_simple
SRC_ID3  = $(ENHANCED_SRC)/id3 
SRC_LRC  = $(ENHANCED_SRC)/lrc

SRCDIR_16 = $(SRC_FSEL)
SRCDIR_16 += $(SRC_ID3) 
SRCDIR_16 += $(SRC_LRC) 
#SRCDIR_16_O2 = $(SRC_ID3)  
SRCDIR_32 =
#目标文件送往的文件夹路径,开发人员需修改处(3) debug: 暂时写为"./make_obj"
#OBJECT_BIN_PATH = ./make_obj/
# **********************************************


#工具名称
DRV_BUILDER = $(ROOT)/psp_rel/tools/drv_builder.exe
AP_BUILDER = $(ROOT)/psp_rel/tools/ap_builder.exe
MMM_CODEC_BUILDER = $(ROOT)/psp_rel/tools/mmm_codec_builder.exe  
#存放结果文件的目录，这里的设置为当前目录，即生成的结果文件放在当前目录
LINKDIR	  =	.
#指定依赖过程的文件的搜索路径，把源文件的路径写上即可
VPATH	=	$(SRCDIR_16) $(SRCDIR_32) $(SRCDIR_16_O2) 

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

# **********************************************
# Compiler and linker options
# **********************************************

#指定程序中引用的头文件的搜索路径，一般源码include时只写上头文件的名称，没有路径信息。这时就根据这个选项去搜索该头文件
INCLUDE	  =	 -I$(PSPRELROOT)/include/ucos -I$(PSPRELROOT)/include  -I$(ENHANCED_SRC)/fsel_simple -I$(ENHANCED_SRC)/id3

#指定链接时搜索的路径
LINCLUDE = -L$(ROOT)/psp_rel/lib -L$(PSPRELROOT)/include

#把公共定义makefile加载进来
include	$(PSPRELROOT)/cfg/rules.mk

#指定自定义链接脚本的名称
LD_SCRIPT = ./eh_simple_link.xn

#链接命令行选项
LD_OPTS = -G0 $(LINCLUDE) -T $(LD_SCRIPT) -o $(IMAGE_ELF) -Map $(MAP) $(OFORMAT) 
#CC_OPTS_O0 += -mlong-calls


# **********************************************
# Files	to be compiled
# **********************************************

#获得.c后缀源码
SRC_C_16  = $(foreach dir,	$(SRCDIR_16), $(wildcard $(dir)/*.c))
#转换为.o格式文件名称，不带路径信息
OBJ_C_16  = $(notdir $(patsubst %.c, %.o, $(SRC_C_16)))

SRC_C_16_O2  = $(foreach dir,	$(SRCDIR_16_O2), $(wildcard $(dir)/*.c))
#转换为.o格式文件名称，不带路径信息
OBJ_C_16_O2  = $(notdir $(patsubst %.c, %.o, $(SRC_C_16_O2)))

#获得.c后缀源码
SRC_C_32  = $(foreach dir,	$(SRCDIR_32), $(wildcard $(dir)/*.c))
#转换为.o格式文件名称，不带路径信息
OBJ_C_32  = $(notdir $(patsubst %.c, %.o, $(SRC_C_32)))

#获得.S后缀源码
SRC_S_16  = $(foreach dir,	$(SRCDIR_16), $(wildcard $(dir)/*.S))
#转换为.o格式文件名称，不带路径信息
OBJ_S_16  = $(notdir $(patsubst %.S, %.o, $(SRC_S_16)))

#获得.S后缀源码
SRC_S_32  = $(foreach dir,	$(SRCDIR_32), $(wildcard $(dir)/*.S))
#转换为.o格式文件名称，不带路径信息
OBJ_S_32  = $(notdir $(patsubst %.S, %.o, $(SRC_S_32)))

#获得所有的.o文件名称
OBJ = $(OBJ_C_16) $(OBJ_S_16) $(OBJ_C_32) $(OBJ_S_32) $(OBJ_C_16_O2)


# **********************************************
# Rules
# **********************************************
#make目标
.PHONY : all

#dump出lst文件
all : $(IMAGE_ELF)
#	$(OBJDUMP) -D $(IMAGE_ELF) > $(IMAGE_LST)
#	$(DRV_BUILDER) $(IMAGE_ELF) $(TARGET_DRV)
#	cp $(TARGET_DRV) $(OBJECT_BIN_PATH)
# 
#	mv $(OBJ) ./make_obj
#	mv $(IMAGENAME).* ./make_obj
	cp $(OBJ) $(PSPLIB_EHANCED)

#链接过程
$(IMAGE_ELF) : $(OBJ)
#	$(LD) $(OBJ) $(PSPLIBDIR)/api.a $(LD_OPTS)

#编译过程	
$(OBJ_C_16) : %.o : %.c
	$(CC) $(CC_OPTS_O2_16) -o	$@ $<
	@echo
	
#独立O2编译
$(OBJ_C_16_O2) : %.o : %.c
	$(CC) $(CC_OPTS_O2_16) -o	$@ $<
	@echo	
	
$(OBJ_S_16) : %.o : %.S
	$(CC) $(CC_OPTS_O2_A_16) -o $@ $<
	@echo

$(OBJ_C_32) : %.o : %.c
	$(CC) $(CC_OPTS_O2) -o	$@ $<
	@echo
	
$(OBJ_S_32) : %.o : %.S
	$(CC) $(CC_OPTS_O0_A) -o $@ $<
	@echo
	


	
#删除结果文件
.PHONY : clean

clean :
	rm -f $(LINKDIR)/*.o
	rm -f $(LINKDIR)/$(IMAGENAME).*

	