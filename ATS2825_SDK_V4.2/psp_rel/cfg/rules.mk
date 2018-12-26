# **********************************************
# Tool-chains
# **********************************************
CC        = sde-gcc
LD        = sde-ld
OBJCOPY   = sde-objcopy
OBJDUMP   = sde-objdump
READELF   = sde-readelf
AR        = sde-ar
STRIP     = sde-strip
MV        = mv
RM        = rm
RM_OBJ    = for dir in $(SRC_DIR); do rm -rf $$dir/*.o; done
# **********************************************
# Endianness
# **********************************************
#小端格式
ENDIAN  = EL

ifeq ($(ENDIAN),EL)
OFORMAT = -EL
else
OFORMAT = -EB
endif

# **********************************************
# Type of Processor & Board
# **********************************************
Processor = mips

# **********************************************
# Basic Compiler Arguments
# **********************************************
#编译命令行选项
#CC_OPTS_BASE = -g -c -G0 -Wall -Wundef -Wno-unknown-pragmas $(INCLUDE)  -msoft-float -fsigned-char -mtune=4kec -mips32r2 -nostdinc -nostdlib -fno-builtin $(OFORMAT) $(BUILD_MACRO) 
CC_OPTS_BASE_16 = -g -c -G0 -Wall -Wundef -Wno-unknown-pragmas $(INCLUDE)  -msoft-float -fsigned-char -fshort-enums -mtune=m4k -mips16e -nostdinc -nostdlib -fno-builtin $(OFORMAT) $(BUILD_MACRO) 
CC_OPTS_BASE = -g -c -G0 -Wall -Wundef -Wno-unknown-pragmas $(INCLUDE)  -msoft-float -fsigned-char -fshort-enums -mtune=m4k -mips32r2 -nostdinc -nostdlib -fno-builtin $(OFORMAT) $(BUILD_MACRO) 

#-fpack-struct -fshort-enums -Wno-bad-function-cast 
 

ifeq ($(COV_PROFILE),1)
	CC_OPTS_BASE += -fprofile-arcs -ftest-coverage -D__UNITTEST__ -D__COVTEST__
	CC_OPTS_BASE_16 += -fprofile-arcs -ftest-coverage -D__UNITTEST__ -D__COVTEST__
endif

CC_OPTS_O0 = $(CC_OPTS_BASE) -O0
CC_OPTS_O0_A = $(CC_OPTS_O0) -D_ASSEMBLER_

CC_OPTS_O2 = $(CC_OPTS_BASE) -O2
CC_OPTS_O2_A = $(CC_OPTS_O2) -D_ASSEMBLER_

CC_OPTS_O0_16 = $(CC_OPTS_BASE_16) -O0
CC_OPTS_O0_A_16 = $(CC_OPTS_O0_16) -D_ASSEMBLER_

CC_OPTS_O2_16 = $(CC_OPTS_BASE_16) -O2
CC_OPTS_O2_A_16 = $(CC_OPTS_O2_16) -D_ASSEMBLER_


ifeq ($(GCC_OPTIMIZE),1)
	CC_OPTS = $(CC_OPTS_BASE) -O2
	CC_OPTS_16 = $(CC_OPTS_BASE_16) -O2
else
	CC_OPTS = $(CC_OPTS_BASE) -O0
	CC_OPTS_16 = $(CC_OPTS_BASE_16) -O0
endif

CC_OPTS_A = $(CC_OPTS) -D_ASSEMBLER_
CC_OPTS_A_16 = $(CC_OPTS_16) -D_ASSEMBLER_
