#定义伪目标
.PHONY: test	    

#判断是否支持linein TWS
#grep -q "\/\/\#define __SUPPORT_LINEIN_TWS" ../inc/case_independent.h 搜索是否打开TWS的配置宏

#注意c语言中的#字符在makefile中有特殊含义，需要加转义字符
#设置linein ap及linein引擎的编译目录
AP_LINEIN_DIR := $(shell grep -q "\/\/\#define __SUPPORT_LINEIN_TWS" ../inc/case_independent.h; \
            if [ $$? == 0 ]; \
            then echo "./../ap/ap_linein"; \
            else echo "./../ap/ap_linein_tws"; \
            fi)

AP_LINEINEG_DIR := $(shell grep -q "\/\/\#define __SUPPORT_LINEIN_TWS" ../inc/case_independent.h; \
            if [ $$? == 0 ]; \
            then echo "./../ap/linein_engine"; \
            else echo "./../ap/linein_tws_engine"; \
            fi)
all:

	make -C ./../../psp_rel/usermodule/enhanced/make_build
	make -C ./../drv/welcome
	make -C ./../drv/Key_boombox
	make -C ./../drv/LED_7SEG
	make -C ./../drv/fm_qn8035
	make -C ./../drv/ccd
	make -C ./../ap/common
	make -C ./strip_lib   #必须在make enhanced 和 common 之后
	make -C ./../ap/ap_manager
	make -C ./../ap/ap_music
	make -C ./../ap/ap_alarm
	make -C ./../ap/ap_config
	make -C ./../ap/music_engine
	make -C ./../ap/music_scan	
	make -C ./../ap/$(AP_LINEIN_DIR)
	make -C ./../ap/$(AP_LINEINEG_DIR)
	make -C ./../ap/ap_usound	
	make -C ./../ap/ap_cardreader	
	make -C ./../ap/fm_engine
	make -C ./../ap/ap_radio	
	make -C ./../ap/btplay_engine
	make -C ./../ap/btcall_engine
	make -C ./../ap/ap_btplay
	make -C ./../ap/ap_btcall
	make -C ./../ap/ap_upgrade	
	make -C ./../ap/usound_engine	
clean:
	make -C ./../../psp_rel/usermodule/enhanced/make_build clean
	make -C ./../drv/welcome clean
	make -C ./../drv/Key_boombox clean
	make -C ./../drv/LED_7SEG clean
	make -C ./../drv/fm_qn8035 clean	
	make -C ./../drv/ccd clean
	make -C ./../ap/common clean
	make -C ./../ap/ap_manager clean
	make -C ./../ap/ap_music clean
	make -C ./../ap/ap_alarm clean
	make -C ./../ap/music_scan clean	
	make -C ./../ap/ap_config clean
	make -C ./../ap/music_engine clean
	make -C ./../ap/$(AP_LINEIN_DIR) clean
	make -C ./../ap/$(AP_LINEINEG_DIR) clean
	make -C ./../ap/ap_usound clean
	make -C ./../ap/ap_cardreader clean		
	make -C ./../ap/fm_engine clean
	make -C ./../ap/ap_radio clean
	make -C ./../ap/btplay_engine clean
	make -C ./../ap/btcall_engine clean
	make -C ./../ap/ap_btplay clean
	make -C ./../ap/ap_btcall clean
	make -C ./../ap/ap_upgrade clean		
	make -C ./../ap/usound_engine	clean		
clean_target:	
	make -C ./../ap/common clean_target
	make -C ./../ap/ap_manager clean_target
	make -C ./../ap/ap_music clean_target
	make -C ./../ap/ap_alarm clean_target
	make -C ./../ap/ap_config clean_target
	make -C ./../ap/btplay_engine clean_target
	make -C ./../ap/btcall_engine clean_target
	make -C ./../ap/ap_btplay clean_target
	make -C ./../ap/ap_btcall clean_target
	make -C ./../ap/music_engine clean_target
	make -C ./../ap/music_scan clean_target	
	make -C ./../ap/$(AP_LINEIN_DIR) clean_target
	make -C ./../ap/$(AP_LINEINEG_DIR) clean_target
	make -C ./../ap/ap_cardreader clean_target		
	make -C ./../ap/fm_engine clean_target
	make -C ./../ap/ap_radio clean_target
	make -C ./../ap/ap_upgrade clean_target	
	make -C ./../ap/ap_usound clean_target
	make -C ./../ap/usound_engine clean_target
