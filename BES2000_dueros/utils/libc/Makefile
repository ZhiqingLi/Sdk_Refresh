cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.S=.o)

# Use relative path to specify the include path (so that no change is needed after rename)
ccflags-y := -I$(obj)/inc

ifeq ($(LIBC_ROM),1)
EXCLUDE_OBJS := strsep.o strtok.o strncat.o strnicmp.o strstr.o memset.o memcpy.o memmove.o \
				atoi.o strtol.o strtoul.o qsort.o bsearch.o \
				vsprintf.o vsscanf.o
ccflags-y += -Itests/rom
else
EXCLUDE_OBJS := libc_rom.o
endif

obj-y := $(filter-out $(EXCLUDE_OBJS),$(obj-y))
