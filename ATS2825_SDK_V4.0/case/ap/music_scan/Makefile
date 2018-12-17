# **********************************************
# Type of Processor	& Board
# **********************************************
Processor =	mips
# **********************************************
# Directories
# **********************************************
#把公共路径加载进来
include	./../../cfg/common_path
#把公共定义makefile加载进来
include	./../../cfg/rules.mk
# **********************************************
# Name of application 开发人员需修改处(1)
IMAGENAME = mus_scan

#所要编译的源文件的存放位置,开发人员需修改处(2)
SRC = $(CASE)/ap
SRCDIR_16 = $(SRC)/music_scan
SRCDIR_32 = $(SRCDIR_16)/O2
#目标文件送往的文件夹路径,开发人员需修改处(3)
OBJECT_BIN_PATH = $(CASE)/fwpkg/ap
# **********************************************


#工具名称
DRV_BUILDER = $(PSP_REL)/tools/drv_builder.exe
AP_BUILDER = $(PSP_REL)/tools/ap_builder.exe
MMM_CODEC_BUILDER = $(PSP_REL)/tools/mmm_codec_builder.exe  
#存放结果文件的目录，这里的设置为当前目录，即生成的结果文件放在当前目录
LINKDIR	  =	.
#指定依赖过程的文件的搜索路径，把源文件的路径写上即可
VPATH	=	$(SRCDIR_16) $(SRCDIR_32)  $(SRC)/common/common_func/

# **********************************************
# Image	file names and map file
# **********************************************
#指定结果文件的名称
IMAGE_ELF =	$(IMAGENAME).exe
IMAGE_LST =	$(IMAGENAME).lst
IMAGE_REC =	$(IMAGENAME).rec
IMAGE_BIN =	$(IMAGENAME).sys
IMAGE_INFO = $(IMAGENAME).info
MAP		  =	$(IMAGENAME).map
TARGET_DRV = $(IMAGENAME).al
TARGET_EXT   = exe lst map ap info
TARGET_FILES = $(foreach n,$(TARGET_EXT),$(IMAGENAME).$(n))

# **********************************************
# Compiler and linker options
# **********************************************

#指定程序中引用的头文件的搜索路径，一般源码include时只写上头文件的名称，没有路径信息。这时就根据这个选项去搜索该头文件
INCLUDE	  =	 -I$(PSP_REL)/include/ucos -I$(PSP_REL)/include -I$(CASE)/inc

#指定链接时搜索的路径
LINCLUDE =  -L$(PSPLIBDIR) -L$(PSPLIBDIR)/../include  

#指定自定义链接脚本的名称
LD_SCRIPT = music_scan_linker.xn

#链接命令行选项
LD_OPTS = $(LINCLUDE) -G 0 -T $(LD_SCRIPT) -o $(IMAGE_ELF) -Map $(MAP) $(OFORMAT) 
LD_OPTS_1   = $(LINCLUDE) -G0 -T $(LD_SCRIPT) $(OFORMAT)
LD_OPTS_2   = -o $(IMAGE_ELF) -Map $(MAP)


# **********************************************
# Files	to be compiled
# **********************************************

#获得.c后缀源码
SRC_C_16  = $(foreach dir,	$(SRCDIR_16), $(wildcard $(dir)/*.c))

SRC_C_16 += $(SRC)/common/common_func/common_share_data.c
#转换为.o格式文件名称，不带路径信息
OBJ_C_16  = $(notdir $(patsubst %.c, %.o, $(SRC_C_16)))

#获得.c后缀源码
SRC_C_32  = $(foreach dir,	$(SRCDIR_32), $(wildcard $(dir)/*.c))
#转换为.o格式文件名称，不带路径信息
OBJ_C_32  = $(notdir $(patsubst %.c, %.o, $(SRC_C_32)))

#获得.S后缀源码
SRC_S_16  = $(foreach dir,	$(SRCDIR_16), $(wildcard $(dir)/*.S))
#转换为.o格式文件名称，不带路径信息
OBJ_S_16  = $(notdir $(patsubst %.S, %.o, $(SRC_S_16)))

#获得.c后缀源码
SRC_S_32  = $(foreach dir,	$(SRCDIR_32), $(wildcard $(dir)/*.S))
#转换为.o格式文件名称，不带路径信息
OBJ_S_32  = $(notdir $(patsubst %.S, %.o, $(SRC_S_32)))

#获得所有的.o文件名称
OBJ = $(OBJ_C_16) $(OBJ_S_16) $(OBJ_C_32) $(OBJ_S_32)


# **********************************************
# Rules
# **********************************************
#make目标
.PHONY : all
.PHONY : pre_make

#dump出lst文件
all : pre_make $(IMAGE_ELF)
	$(OBJDUMP) -D $(IMAGE_ELF) > $(IMAGE_LST)
	$(READELF) -a $(IMAGE_ELF) > $(IMAGE_INFO)
	$(MMM_CODEC_BUILDER) $(IMAGE_ELF) $(TARGET_DRV) MP3
	cp -f $(TARGET_DRV) $(OBJECT_BIN_PATH)
pre_make :
	$(RM) -f $(SRC)/common/common_func/common_share_data.o
#链接过程
$(IMAGE_ELF) : $(OBJ)
	$(LD) $(LD_OPTS_1) $(LD_OPTS_2)

#编译过程	
$(OBJ_C_16) : %.o : %.c
	$(CC) $(CC_OPTS_BASE_16) -Os -DMUSIC_SCAN_SHARE_DATA -o	$@ $<
	@echo
	
$(OBJ_S_16) : %.o : %.S
	$(CC) $(CC_OPTS_O2_A_16) -o     $@ $<
	@echo

$(OBJ_C_32) : %.o : %.c
	$(CC) $(CC_OPTS_O2) -o	        $@ $<
	@echo
	
$(OBJ_S_32) : %.o : %.S
	$(CC) $(CC_OPTS_O2_A) -o        $@ $<
	@echo
	


	
#删除结果文件
.PHONY : clean

clean :
	rm -f $(LINKDIR)/*.o
	rm -f $(LINKDIR)/$(IMAGENAME).*

#只删除 target 文件，保留*.o，以避免没修改的源文件也要重编译
.PHONY : clean_target

clean_target :
	rm -f $(LINKDIR)/$(TARGET_FILES)
	rm -f size.txt
	