obj-y := cmsis/ drivers/ hal/ dsp/

ifneq ($(DUAL_BOOT),1)
obj-y += main/
endif

subdir-ccflags-y += -Iplatform/cmsis/inc -Iplatform/hal -Iutils/hwtimer_list