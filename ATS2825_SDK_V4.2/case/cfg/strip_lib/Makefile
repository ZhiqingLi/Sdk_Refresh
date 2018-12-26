
#获得所有的.o文件名称
COMMON_LIB_DIR = ./../../lib
ENHANCED_LIB_DIR = ./../../../psp_rel/lib/lib_enhanced

COMMON_OBJ = $(foreach objfile, $(COMMON_LIB_DIR), $(wildcard $(objfile)/*.o))
ENHANCED_OBJ = $(foreach objfile, $(ENHANCED_LIB_DIR), $(wildcard $(objfile)/*.o))

OBJ = $(COMMON_OBJ) $(ENHANCED_OBJ)

# **********************************************
# Rules
# **********************************************
#make目标
.PHONY : all

#dump出lst文件
all :
	sde-strip --strip-debug $(OBJ)
