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

lib_common:
	cd ../cfg && make -C ./../ap/common/data
	cd ../cfg && make -C ./../ap/common/applib

lib_linkplay : lib_common
	echo "PROTOCAL := LINKPLAY" > linkplay.ini
	./config.sh ./config_txt/linkplay_demo.ini 2>&1 | tee ./config.log
	cd ../../LinkPlay/bluetooth_stack && make clean && make && cd ../build && make clean && make
	
linkplay_demo : 
	./config.sh ./config_txt/linkplay_demo.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/linkplay_demo.ini 2>&1 | tee ./build.log	
	@echo linkplay demo: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
	
build_linkplay_demo:
	./build.sh ./config_txt/linkplay_demo.ini 2>&1 | tee ./build.log

linkplay_muvo: 
	./config.sh ./config_txt/linkplay_muvo.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/linkplay_muvo.ini 2>&1 | tee ./build.log	
	@echo linkplay muvo: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
	
build_linkplay_muvo:
	./build.sh ./config_txt/linkplay_muvo.ini 2>&1 | tee ./build.log	
	@echo linkplay muvo: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
	
linkplay_far_demo:
	./config.sh ./config_txt/linkplay_far_demo.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/linkplay_far_demo.ini 2>&1 | tee ./build.log	
	@echo linkplay farfield demo: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
	
build_linkplay_far_demo:
	./build.sh ./config_txt/linkplay_far_demo.ini 2>&1 | tee ./build.log	
	@echo linkplay farfield demo: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
	
linkplay_far:
	./config.sh ./config_txt/jl_36410_far.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/jl_36410_far.ini 2>&1 | tee ./build.log	
	@echo linkplay 36410 demo: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
	
build_linkplay_far:
	./build.sh ./config_txt/jl_36410_far.ini 2>&1 | tee ./build.log	
	@echo linkplay 36410 demo: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
	
DVB_ATS2825P_TWS:
	./config.sh ./config_txt/US282A_BTBOX_DVB_ATS2825P_TWS.ini 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825P_TWS.ini 2>&1 | tee ./build.log	
	@echo DVB_ATS2825P_TWS: all, $$[$$(date +%s)-$(TIME_BEGIN)] seconds
		

build_DVB_ATS2825P_TWS:
	./build.sh ./config_txt/US282A_BTBOX_DVB_ATS2825P_TWS.ini 2>&1 | tee ./build.log
	

all:
	make clean;make DVB_ATS2825P_TWS

build:
	./build.sh  2>&1 | tee ./build.log  
	  
clean:
	./make.sh 0 2>&1 | tee ./make.log

clean_target:
	./make.sh 1 2>&1 | tee ./make.log
