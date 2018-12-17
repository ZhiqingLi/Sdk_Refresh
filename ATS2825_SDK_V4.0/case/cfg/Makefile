#定义伪目标
.PHONY: test	    

#判断是否支持linein TWS
#grep -q "\/\/\#define __SUPPORT_LINEIN_TWS" ../inc/case_independent.h 搜索是否打开TWS的配置宏

#注意c语言中的#字符在makefile中有特殊含义，需要加转义字符
#设置linein ap及linein引擎的编译目录
AP_LINEIN_DIR := $(shell grep -aq "\/\/\#define __SUPPORT_LINEIN_TWS" ../inc/case_independent.h; \
            if [ $$? == 0 ]; \
            then echo "./../ap/ap_linein"; \
            else \
            grep -aq "\/\/\#define __SUPPORT_LINEIN_LOW_DELAY" ../inc/case_independent.h; \
            if [ $$? == 0 ]; \
            then echo "./../ap/ap_linein_tws"; \
            else \
            echo "./../ap/ap_linein_tws_ext"; \
            fi;\
            fi)

AP_LINEINEG_DIR := $(shell grep -aq "\/\/\#define __SUPPORT_LINEIN_TWS" ../inc/case_independent.h; \
            if [ $$? == 0 ]; \
            then echo "./../ap/linein_engine"; \
            else \
            grep -aq "\/\/\#define __SUPPORT_LINEIN_LOW_DELAY" ../inc/case_independent.h; \
            if [ $$? == 0 ]; \
            then echo "./../ap/linein_tws_engine"; \
            else \
            echo "./../ap/linein_tws_engine_ext"; \
            fi;\
            fi)
all:

	make -j16 -C ./../../psp_rel/usermodule/enhanced/make_build
	make -j16 -C ./../drv/welcome
	make -j16 -C ./../drv/Key_boombox
	make -j16 -C ./../drv/LED_7SEG
	make -j16 -C ./../drv/fm_qn8035
	make -j16 -C ./../drv/ccd
	make -j16 -C ./../drv/usb_sound
	make -j16 -C ./../drv/usb_speaker	
	make -j16 -C ./../ap/common
	make -C ./strip_lib   #必须在make enhanced 和 common 之后
	make -j16 -C ./../ap/ap_manager
	make -j16 -C ./../ap/ap_music
	make -j16 -C ./../ap/ap_alarm
	make -j16 -C ./../ap/ap_config
	make -j16 -C ./../ap/music_engine
	make -j16 -C ./../ap/$(AP_LINEIN_DIR)
	make -j16 -C ./../ap/$(AP_LINEINEG_DIR)
	make -j16 -C ./../ap/ap_usound	
	make -j16 -C ./../ap/ap_cardreader	
	make -j16 -C ./../ap/fm_engine
	make -j16 -C ./../ap/ap_radio
	make -j16 -C ./../ap/btplay_engine
	make -j16 -C ./../ap/btcall_engine
#	make -j16 -C ./../ap/bluetooth_stack
	make -j16 -C ./../ap/ap_btplay
	make -j16 -C ./../ap/ap_btcall
	make -j16 -C ./../ap/ap_upgrade	
	make -j16 -C ./../ap/usound_engine	
	make -C ./../ap/music_scan		
clean:
	make -j16 -C ./../../psp_rel/usermodule/enhanced/make_build clean
	make -j16 -C ./../drv/welcome clean
	make -j16 -C ./../drv/Key_boombox clean
	make -j16 -C ./../drv/LED_7SEG clean
	make -j16 -C ./../drv/fm_qn8035 clean	
	make -j16 -C ./../drv/ccd clean
	make -j16 -C ./../drv/usb_sound clean
	make -j16 -C ./../drv/usb_speaker clean	
	make -j16 -C ./../ap/common clean
	make -j16 -C ./../ap/ap_manager clean
	make -j16 -C ./../ap/ap_music clean
	make -j16 -C ./../ap/ap_alarm clean
	make -j16 -C ./../ap/music_scan clean
	make -j16 -C ./../ap/ap_config clean
	make -j16 -C ./../ap/music_engine clean
	make -j16 -C ./../ap/$(AP_LINEIN_DIR) clean
	make -j16 -C ./../ap/$(AP_LINEINEG_DIR) clean
	make -j16 -C ./../ap/ap_usound clean
	make -j16 -C ./../ap/ap_cardreader clean		
	make -j16 -C ./../ap/fm_engine clean
	make -j16 -C ./../ap/ap_radio clean
	make -j16 -C ./../ap/btplay_engine clean
	make -j16 -C ./../ap/btcall_engine clean
#	make -j16 -C ./../ap/bluetooth_stack clean
	make -j16 -C ./../ap/ap_btplay clean
	make -j16 -C ./../ap/ap_btcall clean
	make -j16 -C ./../ap/ap_upgrade clean		
	make -j16 -C ./../ap/usound_engine	clean	
clean_target:	
	make -j16 -C ./../ap/common clean_target
	make -j16 -C ./../ap/ap_manager clean_target
	make -j16 -C ./../ap/ap_music clean_target
	make -j16 -C ./../ap/ap_alarm clean_target
	make -j16 -C ./../ap/ap_config clean_target
	make -j16 -C ./../ap/btplay_engine clean_target
	make -j16 -C ./../ap/btcall_engine clean_target
#	make -j16 -C ./../ap/bluetooth_stack clean_target
	make -j16 -C ./../ap/ap_btplay clean_target
	make -j16 -C ./../ap/ap_btcall clean_target
	make -j16 -C ./../ap/music_engine clean_target
	make -j16 -C ./../ap/music_scan clean_target	
	make -j16 -C ./../ap/$(AP_LINEIN_DIR) clean_target
	make -j16 -C ./../ap/$(AP_LINEINEG_DIR) clean_target
	make -j16 -C ./../ap/ap_cardreader clean_target		
	make -j16 -C ./../ap/fm_engine clean_target
	make -j16 -C ./../ap/ap_radio clean_target
	make -j16 -C ./../ap/ap_upgrade clean_target	
	make -j16 -C ./../ap/ap_usound clean_target
	make -j16 -C ./../ap/usound_engine clean_target
