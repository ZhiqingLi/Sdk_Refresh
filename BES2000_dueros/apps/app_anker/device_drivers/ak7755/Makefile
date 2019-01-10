cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

#ifeq ($(GEN_LIB),1)
# start complie code to lib
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
HSLIMITE_LIB_NAME := libconexant
$(HSLIMITE_LIB_NAME)-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)
obj-y += $(HSLIMITE_LIB_NAME).a
#else
#obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))
#obj-y := $(obj-y:.c=.o)
#obj-y := $(obj-y:.cpp=.o)
#obj-y := $(obj-y:.S=.o)
#endif

subdir-ccflags-y += \
				-Iplatform/hal	\
				-Iplatform/hal/best2000 \
				-Iservices/bt_profiles/inc \
				-Iapps/app_anker/device_drivers/bes_hal \
			

