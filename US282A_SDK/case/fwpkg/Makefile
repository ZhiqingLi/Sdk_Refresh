# 该makefile用于自动完成功能配置，程序编译，固件生成等功能，请根据实际情况修改该makefile，以方便
# 生成所需的固件
# makefile会调用三个shell文件
#       config.sh用于功能配置，可根据配置的板型和功能是否支持，来修改部分文件，完成功能项的打开和关闭
#       make.sh 用于源代码编译，该shell脚本会调用case/cfg目录下的makefile进行程序编译
#       build.sh 用于固件生成，可根据配置的板型和功能是否支持，生成相应的目标固件，该脚本会生成fw文件，用于量产功能量产
#                hex和fwu文件用于升级功能，atf文件用于ATT工具自动化测试，以及OTA文件用于apk升级
# 如果调用过程中，有一个shell脚本出错，则整个编译过程会停止并报错。三个shell脚本会各自生成相应的log文件，可通过分析log文件分析出错原因

# 例如想编译一个支持TWS功能的ATS2823的固件，可在命令行输入make clean;make DVB_ATS2823_TWS,可以看到DVB_ATS2823_TWS是一个伪目标，该目标会调用
# config.sh,将TWS功能打开。接着调用make.sh和build.sh完成固件的生成

# 如果之前源代码已经编译过，只是修改了某个文件，想再次生成固件，可输入make DVB_ATS2823_TWS,完成程序的编译

# 如果源文件都没有修改，仅仅只想再次生成固件，可输入make build_DVB_ATS2823_TWS或make build完成固件的生成

# 起始时间
TIME_BEGIN := $(shell (date +%s))

.PHONY : all clean clean_target test

DVB_ATS2825:
	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2825.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825.ini 2>&1 | tee ./build.log	
	@echo DVB_ATS2825: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
	
EVB:
	./config.sh ./config_txt/US282A_BTBOX_EVB.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_EVB.ini 2>&1 | tee ./build.log		
	@echo EVB: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
	
DVB_ATS2823:
	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2823.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2823.ini 2>&1 | tee ./build.log	
	@echo DVB_ATS2823: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds	

DVB_ATS2823T:
	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2823T.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2823T.ini 2>&1 | tee ./build.log
	@echo DVB_ATS2823T: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
	
DVB_ATS2823B:
	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2823B.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2823B.ini 2>&1 | tee ./build.log	
	@echo DVB_ATS2823B: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
	
EVB_OTA:
	./config.sh ./config_txt/US282A_BTBOX_EVB_OTA.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_EVB_OTA.ini 2>&1 | tee ./build.log	
	@echo EVB_OTA: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
	
DVB_ATS2825_OTA:
	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_OTA.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_OTA.ini 2>&1 | tee ./build.log	

	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_OTA_1M.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_OTA_1M.ini 2>&1 | tee ./build.log	
	@echo DVB_ATS2825_OTA: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
	
EVB_TWS:
	./config.sh ./config_txt/US282A_BTBOX_EVB_TWS.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_EVB_TWS.ini 2>&1 | tee ./build.log
	@echo EVB_TWS: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
	
DVB_ATS2825_TWS:
	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_TWS.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_TWS.ini 2>&1 | tee ./build.log	
	@echo DVB_ATS2825_TWS: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
		
DVB_ATS2825_TWS_ESD:
	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_TWS_ESD.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_TWS_ESD.ini 2>&1 | tee ./build.log	
	@echo DVB_ATS2825_TWS_ESD: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
	
DVB_ATS2825_TWS_48K:
	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_TWS_48K.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_TWS_48K.ini 2>&1 | tee ./build.log	
	@echo DVB_ATS2825_TWS_48K: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds

DVB_ATS2825_TWS_BQB:
	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_TWS_BQB.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_TWS_BQB.ini 2>&1 | tee ./build.log
	@echo DVB_ATS2825_TWS_BQB: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds	
			
DVB_ATS2823_TWS:
	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2823_TWS.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2823_TWS.ini 2>&1 | tee ./build.log
	@echo DVB_ATS2823_TWS: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
	
DVB_ATS2823W:
	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2823_W.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2823_W.ini 2>&1 | tee ./build.log	
	@echo DVB_ATS2823W: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds

DVB_ATS2825W:
	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_W.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_W.ini 2>&1 | tee ./build.log	
	@echo DVB_ATS2825W: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds

EVB_W:
	./config.sh ./config_txt/US282A_BTBOX_EVB_W.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_EVB_W.ini 2>&1 | tee ./build.log	
	@echo EVB_W: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds	

DVB_ATS2823W_TWS:	
	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2823_TWS_W.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2823_TWS_W.ini 2>&1 | tee ./build.log	
	@echo DVB_ATS2823W_TWS: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds

DVB_ATS2825W_TWS:
	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_TWS_W.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_TWS_W.ini 2>&1 | tee ./build.log	
	@echo DVB_ATS2825W_TWS: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
	
DVB_ATS2825CW:
	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_CW.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_CW.ini 2>&1 | tee ./build.log
	@echo DVB_ATS2825CW: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds	

DVB_ATS2825CW_TWS:
	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2825CW_TWS.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825CW_TWS.ini 2>&1 | tee ./build.log
	@echo DVB_ATS2825CW_TWS: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds	
		
DVB_ATS2825C_TWS:
	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2825C_TWS.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825C_TWS.ini 2>&1 | tee ./build.log	
	@echo DVB_ATS2825C_TWS: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds

DVB_ATS2825_TWS_LOW_DELAY: 
	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_TWS_LOW_DELAY.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_TWS_LOW_DELAY.ini 2>&1 | tee ./build.log	
	@echo DVB_ATS2825_TWS_LOW_DELAY: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds

EVB_W_TWS:
	./config.sh ./config_txt/US282A_BTBOX_EVB_TWS_W.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_EVB_TWS_W.ini 2>&1 | tee ./build.log
	@echo EVB_W_TWS: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
	
build_EVB:
	./build.sh ./config_txt/US282A_BTBOX_EVB.ini 2>&1 | tee ./build.log    	
build_EVB_W:
	./build.sh ./config_txt/US282A_BTBOX_EVB_W.ini 2>&1 | tee ./build.log  
	
build_DVB_ATS2825:
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825.ini 2>&1 | tee ./build.log	
	
build_DVB_ATS2825W:	
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_W.ini 2>&1 | tee ./build.log		

build_DVB_ATS2823:
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2823.ini 2>&1 | tee ./build.log
	
build_DVB_ATS2823W:	
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2823_W.ini 2>&1 | tee ./build.log			

build_DVB_ATS2823T:	
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2823T.ini 2>&1 | tee ./build.log	
	
build_DVB_ATS2823B:	
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2823B.ini 2>&1 | tee ./build.log
		
build_EVB_OTA:
	./build.sh ./config_txt/US282A_BTBOX_EVB_OTA.ini 2>&1 | tee ./build.log	

build_DVB_ATS2825_OTA:
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_OTA.ini 2>&1 | tee ./build.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_OTA_1M.ini 2>&1 | tee ./build.log
build_EVB_TWS:
	./build.sh ./config_txt/US282A_BTBOX_EVB_TWS.ini 2>&1 | tee ./build.log

build_DVB_ATS2825_TWS:
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_TWS.ini 2>&1 | tee ./build.log
	
build_DVB_ATS2825_TWS_48K:
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_TWS_48K.ini 2>&1 | tee ./build.log

build_DVB_ATS2825_TWS_BQB:
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_TWS_BQB.ini 2>&1 | tee ./build.log	
	
build_DVB_ATS2825CW:
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_CW.ini 2>&1 | tee ./build.log	

build_DVB_ATS2825CW_TWS:
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825CW_TWS.ini 2>&1 | tee ./build.log	
	
build_DVB_ATS2825C_TWS:
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825C_TWS.ini 2>&1 | tee ./build.log		

build_DVB_ATS2825_TWS_LOW_DELAY:
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_TWS_LOW_DELAY.ini 2>&1 | tee ./build.log	

build_DVB_ATS2823_TWS:
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2823_TWS.ini 2>&1 | tee ./build.log	
	
build_DVB_W_TWS:
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825_TWS_W.ini 2>&1 | tee ./build.log
	
build_EVB_W_TWS:
	./build.sh ./config_txt/US282A_BTBOX_EVB_TWS_W.ini 2>&1 | tee ./build.log
all:
	make clean;make EVB_TWS
	make clean;make DVB_ATS2825
	make clean;make DVB_ATS2825W	
	make clean;make DVB_ATS2825_TWS
	make clean;make DVB_ATS2825W_TWS
	make clean;make DVB_ATS2823_TWS
	make clean;make DVB_ATS2823W_TWS	
	make clean;make DVB_ATS2825_OTA		
	make clean;make DVB_ATS2825_TWS_ESD	
	make clean;make DVB_ATS2823T
	make clean;make DVB_ATS2825_TWS_48K

test:
	#make clean;make EVB
	#make clean;make DVB_ATS2825
	#make clean;make DVB_ATS2823
	#make clean;make EVB_OTA
	#make clean;make EVB_TWS
	#make clean;make DVB_ATS2825W
	#make clean;make DVB_ATS2823W		
	#make clean;make DVB_ATS2825_TWS
	make clean;make DVB_ATS2823_TWS
	#make clean;make DVB_ATS2823W_TWS	
	#make clean;make DVB_ATS2825_OTA
	#make clean;make DVB_ATS2825CW_TWS
	make clean;make DVB_ATS2825_TWS_LOW_DELAY
	make clean;make DVB_ATS2825W_TWS
	make clean;make DVB_ATS2825_TWS	
build:
	./build.sh  2>&1 | tee ./build.log  
	  
clean:
	./make.sh 0 2>&1 | tee ./make.log

clean_target:
	./make.sh 1 2>&1 | tee ./make.log
