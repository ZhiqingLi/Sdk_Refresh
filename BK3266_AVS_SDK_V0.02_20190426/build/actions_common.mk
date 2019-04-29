#***************************************************************
#   该文件在Makefile中使用                                     *
#   在Makefile中include该文件前，请先定义正确的ROOT变量        *
#***************************************************************

#***************************************************************
#   Directories                                                *
#   把公共路径加载进来                                         *
#   以根目录为起始点                                           *
#   引用一个变量的格式为 $(变量名称)                           *
#***************************************************************
CASE            = $(ROOT)/case
CASELIBDIR      = $(CASE)/lib
PSP_REL         = $(ROOT)/psp_rel
PSPLIBDIR       = $(PSP_REL)/lib
ENHANCEDIR      = $(PSP_REL)/lib/lib_enhanced

# case链接ENHANCED 的xn路径
ENHANCED_XN     = $(PSP_REL)/usermodule/enhanced/case_link_xn
OBJ_DIR         = ./obj

# build_var.mk 在编译时生成，拷贝到case/cfg/ 
include $(CASE)/cfg/build_var.mk
# proj.inc在新建一个编译项目是生成，包含一些私有的编译参数.编译时拷贝到case/cfg/
include $(CASE)/cfg/proj.inc
# 把公共定义makefile加载进来
include $(CASE)/cfg/rules.mk
