#!/bin/bash

# This script modify config.txt, fwimage.cfg and source files
# config.txt --> used for configure function support or not
# fwimage.cfg--> used for configure function support or not
# source file--> open or close some C source files
# 
# 2016-06-12	-	wuyufan

# 该脚本文件用于配置源文件，包括config.txt,fwimage_xx.cfg文件等，
# 也会修改部分源文件的宏定义来打开或关闭某些功能
# 当板型或功能有所变更时，必须先调用config.sh配置源文件，然后再进行程序的编译
# 该脚本需要传递一个参数，就是对应的ini文件，相关功能配置会在ini文件里有描述

echo "config source file"

FW_NAME_EVB=US282A_BTBOX_EVB
FW_NAME_DEMO=US282A_BTBOX_DEMO
FW_NAME_DVB_2825=US282A_BTBOX_DVB_ATS2825
FW_NAME_DVB_2823=US282A_BTBOX_DVB_ATS2823
FW_NAME_EVB_2823=US282A_BTBOX_EVB_ATS2823

if [ $# != 1 ]
then
    echo "missing ini file name"
    exit 1
else
    int_file=$1
fi

#shell命令解释:`command`,`符号是tab上面的字符,用于命令替换，把command的执行结果赋值给前面的变量
#grep是字符串过滤命令，后面跟过滤的字符串，以及要查找的文件名，这一句意思是在ini文件中查找包含关键字的行，'|'表示连接命令
#awk -F '=' '{print $2} 是根据特定格式分割字符串，分割的标识是'=',同时找到分割的第二个字符串，这样就找到了相应的参数
#总之，这个shell语句是在ini文件中找到关键字定义的值，并赋值给定义的变量

#读取板型配置
BoardType=`grep -a "BoardType" $int_file | awk -F '=' '{print $2}'`

#读取config.txt配置
config_txt_name=`grep -a "config_txt" $int_file | awk -F '=' '{print $2}'`

#读取fwimage.cfg配置
config_cfg_name=`grep -a "fwimage_cfg" $int_file | awk -F '=' '{print $2}'`

#读取是否支持TWS
SupportTWS=`grep -a "SupportTWS" $int_file | awk -F '=' '{print $2}'`

#读取是否支持OTA
SupportOTA=`grep -a "SupportOTA" $int_file | awk -F '=' '{print $2}'`

#读取是否支持Waves音效
SupportWaves=`grep -a "SupportWaves" $int_file | awk -F '=' '{print $2}'`

#读取是否支持SPP/BLE
SupportSPPBLE=`grep -a "SupportSPPBLE" $int_file | awk -F '=' '{print $2}'`

#读取是否支持ESD
SupportESD=`grep -a "SupportESD" $int_file | awk -F '=' '{print $2}'`

#读取是否支持固定采样率
SupportFIXSAMPLE=`grep -a "SupportFIXSAMPLE" $int_file | awk -F '=' '{print $2}'`

#读取dae_config.txt配置
cfg_dae_txt_name=`grep -a "cfg_dae_txt" $int_file | awk -F '=' '{print $2}'`

SDKRelease=`grep -a "SDKRelease" $int_file | awk -F '=' '{print $2}'`

SupportBQB=`grep -a "SupportBQB" $int_file | awk -F '=' '{print $2}'`

SupportLowDelayLINEIN=`grep -a "SupportLOWDELAYLINEIN" $int_file | awk -F '=' '{print $2}'`

echo "BoardType: $BoardType"
echo "config.txt name: $config_txt_name"
echo "fwimage.cfg name: $config_cfg_name"
echo "SupportTWS: $SupportTWS"
echo "SupportOTA: $SupportOTA"
echo "SupportWaves: $SupportWaves"
echo "cfg_dae.txt name: $cfg_dae_txt_name"
echo "SupportESD: $SupportESD"
echo "SupportFIXSAMPLE: $SupportFIXSAMPLE"
echo "SDKRelease: $SDKRelease"
echo "SupportBQB: $SupportBQB"
echo "SupportLowDelayLINEIN: $SupportLowDelayLINEIN"
#判断config_txt_name文件是否存在 -f表示判断文件 -d表示判断目录    
if [ ! -f "./config_txt/$config_txt_name" ]
then
    echo "ConfigError: ./config_txt/$config_txt_name not exist"
    exit 1;
fi   

#判断config_cfg_name文件是否存在 -f表示判断文件 -d表示判断目录  
if [ ! -f "./config_txt/$config_cfg_name" ]
then
    echo "ConfigError: ./config_txt/$config_cfg_name not exist"
    exit 1;
fi  

#tws version and normal version use same btstack
grep -aq "\/\/\#define ENABLE_TRUE_WIRELESS_STEREO" ../inc/btstack_common.h
if [ $? == 0 ]
then
    echo "ConfigError: ../inc/btstack_common.h #define ENABLE_TRUE_WIRELESS_STEREO must be open"
    exit 1;
fi  

#这里判断是否支持OTA,支持OTA，固件模式要改为1，普通固件模式为0 
#sed是shell中字符串替换命令，基本格式为sed '样式' 文件,样式中'//'代表查找  
#sed '0, /a/s//b/' filename 用于把文件filename中第一次出现a的字符串替换成b，0表示第一次替换，s表示替换功能 
if [ $SupportOTA == 1 ]
then   	
	sed '0,/INF_MAKE_FW_MODE.*;/s//INF_MAKE_FW_MODE = 1;/' ./config_txt/$config_cfg_name > tmpfile
	mv tmpfile ./config_txt/$config_cfg_name  
else	
	sed '0,/INF_MAKE_FW_MODE.*;/s//INF_MAKE_FW_MODE = 0;/' ./config_txt/$config_cfg_name > tmpfile
	mv tmpfile ./config_txt/$config_cfg_name 	 	
fi  

#判断配置文件是否存在otatest.ap，如果存在，则打开OTA测试ap这个配置项
grep -aq '//FWIM="otatest.ap"' ./config_txt/$config_cfg_name
if [ $? == 0 ]
then 
	sed '0,/SETTING_APP_SUPPORT_OTA_SONG_TEST.*;/s//SETTING_APP_SUPPORT_OTA_SONG_TEST   = 0[0,1];/' ./config_txt/$config_txt_name > tmpfile
	mv tmpfile ./config_txt/$config_txt_name     
else 
    grep -aq 'FWIM="otatest.ap"' ./config_txt/$config_cfg_name
    if [ $? == 0 ]
    then
        if [ $SupportOTA == 1 ]
        then   
        	sed '0,/SETTING_APP_SUPPORT_OTA_SONG_TEST.*;/s//SETTING_APP_SUPPORT_OTA_SONG_TEST   = 1[0,1];/' ./config_txt/$config_txt_name > tmpfile
        	mv tmpfile ./config_txt/$config_txt_name 
        else
	        sed '0,/SETTING_APP_SUPPORT_OTA_SONG_TEST.*;/s//SETTING_APP_SUPPORT_OTA_SONG_TEST   = 0[0,1];/' ./config_txt/$config_txt_name > tmpfile
	        mv tmpfile ./config_txt/$config_txt_name  
	    fi       
    else
	    sed '0,/SETTING_APP_SUPPORT_OTA_SONG_TEST.*;/s//SETTING_APP_SUPPORT_OTA_SONG_TEST   = 0[0,1];/' ./config_txt/$config_txt_name > tmpfile
	    mv tmpfile ./config_txt/$config_txt_name  
	fi   
fi

if [ $SupportSPPBLE = 0 ]
then
	sed '0,/BTMANAGER_ENABLE_SPP.*;/s//BTMANAGER_ENABLE_SPP                 = 0[0,1];/' ./config_txt/$config_txt_name > tmpfile
	mv tmpfile ./config_txt/$config_txt_name 

	sed '0,/BTMANAGER_ENABLE_BLE.*;/s//BTMANAGER_ENABLE_BLE                 = 0[0,1];/' ./config_txt/$config_txt_name > tmpfile
	mv tmpfile ./config_txt/$config_txt_name 
else			
	sed '0,/BTMANAGER_ENABLE_SPP.*;/s//BTMANAGER_ENABLE_SPP                 = 1[0,1];/' ./config_txt/$config_txt_name > tmpfile
	mv tmpfile ./config_txt/$config_txt_name 

	sed '0,/BTMANAGER_ENABLE_BLE.*;/s//BTMANAGER_ENABLE_BLE                 = 1[0,1];/' ./config_txt/$config_txt_name > tmpfile
	mv tmpfile ./config_txt/$config_txt_name 	 
fi 

if [ $SupportTWS == 1 ]
then   
	sed '0,/BTMANAGER_SUPPORT_DEVICE_NUM.*;/s//BTMANAGER_SUPPORT_DEVICE_NUM         = 2[1~2,1];/' ./config_txt/$config_txt_name > tmpfile
	mv tmpfile ./config_txt/$config_txt_name   

	sed '0,/BTMANAGER_ENABLE_ANCS .*;/s//BTMANAGER_ENABLE_ANCS                = 0[0,1];/' ./config_txt/$config_txt_name > tmpfile
	mv tmpfile ./config_txt/$config_txt_name  
	
	sed '0,/^\/\/\#define __SUPPORT_LINEIN_TWS.*/s//\#define __SUPPORT_LINEIN_TWS/' ../inc/case_independent.h > tmpfile
	mv tmpfile ../inc/case_independent.h
    if [ $SupportLowDelayLINEIN == 1 ]
    then
    	sed '0,/^\/\/\#define __SUPPORT_LINEIN_LOW_DELAY.*/s//\#define __SUPPORT_LINEIN_LOW_DELAY/' ../inc/case_independent.h > tmpfile
    	mv tmpfile ../inc/case_independent.h
    else
    	sed '0,/^\#define __SUPPORT_LINEIN_LOW_DELAY.*/s//\/\/\#define __SUPPORT_LINEIN_LOW_DELAY/' ../inc/case_independent.h > tmpfile
    	mv tmpfile ../inc/case_independent.h  	
    fi	
else
	sed '0,/BTMANAGER_SUPPORT_DEVICE_NUM.*;/s//BTMANAGER_SUPPORT_DEVICE_NUM         = 1[1~2,1];/' ./config_txt/$config_txt_name > tmpfile
	mv tmpfile ./config_txt/$config_txt_name  
	
	sed '0,/BTMANAGER_ENABLE_ANCS .*;/s//BTMANAGER_ENABLE_ANCS                = 1[0,1];/' ./config_txt/$config_txt_name > tmpfile
	mv tmpfile ./config_txt/$config_txt_name  	

	sed '0,/^\#define __SUPPORT_LINEIN_TWS.*/s//\/\/\#define __SUPPORT_LINEIN_TWS/' ../inc/case_independent.h > tmpfile
	mv tmpfile ../inc/case_independent.h  
	
    if [ $SupportLowDelayLINEIN == 1 ]
    then
    	echo "Not support low delay linein when SupportTWS == 0"
    	exit 1
    else
    	sed '0,/^\#define __SUPPORT_LINEIN_LOW_DELAY.*/s//\/\/\#define __SUPPORT_LINEIN_LOW_DELAY/' ../inc/case_independent.h > tmpfile
    	mv tmpfile ../inc/case_independent.h  	
    fi			
fi	


if [ $SupportESD == 1 ]
then     
	sed '0,/^\/\/\#define __ESD_MODE_.*/s//\#define __ESD_MODE_/' ../inc/case_independent.h > tmpfile
	mv tmpfile ../inc/case_independent.h
else
	sed '0,/^\#define __ESD_MODE_.*/s//\/\/\#define __ESD_MODE_/' ../inc/case_independent.h > tmpfile
	mv tmpfile ../inc/case_independent.h  	
fi

if [ $SupportFIXSAMPLE == 0 ]
then     
	sed '0,/^\/\/\#define	__PACFG339_H__.*/s//\#define	__PACFG339_H__/' ../drv/ccd/STA339BWS.h > tmpfile
	mv tmpfile ../drv/ccd/STA339BWS.h

	sed '0,/SETTING_AUDIO_SAMPLE_RATE_FIXED.*;/s//SETTING_AUDIO_SAMPLE_RATE_FIXED = 0[0,1];/' ./config_txt/$config_txt_name > tmpfile
	mv tmpfile ./config_txt/$config_txt_name 
else
	sed '0,/^\#define	__PACFG339_H__.*/s//\/\/\#define	__PACFG339_H__/' ../drv/ccd/STA339BWS.h > tmpfile
	mv tmpfile ../drv/ccd/STA339BWS.h  	
	
	sed '0,/SETTING_AUDIO_SAMPLE_RATE_FIXED.*;/s//SETTING_AUDIO_SAMPLE_RATE_FIXED = 1[0,1];/' ./config_txt/$config_txt_name > tmpfile
	mv tmpfile ./config_txt/$config_txt_name 

fi

if [ $SDKRelease == 1 ]
then     
	sed '0,/SETTING_UART_PRINT_ENABLE.*;/s//SETTING_UART_PRINT_ENABLE{124} = 0[0,1];/' ./config_txt/$config_txt_name > tmpfile
	mv tmpfile ./config_txt/$config_txt_name 
	
	sed '0,/SETTING_SYS_DEBUG_MODE_ENABLE.*;/s//SETTING_SYS_DEBUG_MODE_ENABLE{121} = 0[0,1];/' ./config_txt/$config_txt_name > tmpfile
	mv tmpfile ./config_txt/$config_txt_name 
	
	sed '0,/BTSTACK_LINKKEY_PRINT_ENABLE.*;/s//BTSTACK_LINKKEY_PRINT_ENABLE      = 0[0,1];/' ./config_txt/$config_txt_name > tmpfile
	mv tmpfile ./config_txt/$config_txt_name 	
else
	sed '0,/SETTING_UART_PRINT_ENABLE.*;/s//SETTING_UART_PRINT_ENABLE{124} = 1[0,1];/' ./config_txt/$config_txt_name > tmpfile		
	mv tmpfile ./config_txt/$config_txt_name 
	
	sed '0,/SETTING_SYS_DEBUG_MODE_ENABLE.*;/s//SETTING_SYS_DEBUG_MODE_ENABLE{121} = 1[0,1];/' ./config_txt/$config_txt_name > tmpfile
	mv tmpfile ./config_txt/$config_txt_name 
	
	sed '0,/BTSTACK_LINKKEY_PRINT_ENABLE.*;/s//BTSTACK_LINKKEY_PRINT_ENABLE      = 1[0,1];/' ./config_txt/$config_txt_name > tmpfile
	mv tmpfile ./config_txt/$config_txt_name 			
fi

	
#根据输入参数判断要修改的头文件位置
if [ $BoardType = "$FW_NAME_EVB" ]
then 	
	#修改case_independent.h头文件BOARD类型为CASE_BOARD_EVB
	sed '0,/CASE_BOARD_TYPE.*CASE_BOARD.*/s//CASE_BOARD_TYPE         CASE_BOARD_EVB/' ../inc/case_independent.h > tmpfile
	mv tmpfile ../inc/case_independent.h    
elif [ $BoardType = "$FW_NAME_DVB_2825" ]
then 
	#修改case_independent.h头文件BOARD类型为CASE_BOARD_DVB_ATS2825
	sed '0,/CASE_BOARD_TYPE.*CASE_BOARD.*/s//CASE_BOARD_TYPE         CASE_BOARD_DVB_ATS2825/' ../inc/case_independent.h > tmpfile
	mv tmpfile ../inc/case_independent.h   	
elif [ $BoardType = "$FW_NAME_DVB_2823" ]
then 	
	#修改case_independent.h头文件BOARD类型为CASE_BOARD_DVB_ATS2823
	sed '0,/CASE_BOARD_TYPE.*CASE_BOARD.*/s//CASE_BOARD_TYPE         CASE_BOARD_DVB_ATS2823/' ../inc/case_independent.h > tmpfile
	mv tmpfile ../inc/case_independent.h  			
elif [ $BoardType = "$FW_NAME_EVB_2823" ]
then 	
	#修改case_independent.h头文件BOARD类型为CASE_BOARD_EVB_ATS2823
	sed '0,/CASE_BOARD_TYPE.*CASE_BOARD.*/s//CASE_BOARD_TYPE         CASE_BOARD_ATS2823/' ../inc/case_independent.h > tmpfile
	mv tmpfile ../inc/case_independent.h  	
elif [ $BoardType = "$FW_NAME_DEMO" ]
then 	
	#修改case_independent.h头文件BOARD类型为CASE_BOARD_DEMO
	sed '0,/CASE_BOARD_TYPE.*CASE_BOARD.*/s//CASE_BOARD_TYPE         CASE_BOARD_DEMO/' ../inc/case_independent.h > tmpfile
	mv tmpfile ../inc/case_independent.h  	
else
	echo "unknown case type!!!"
	echo "Support Type:US282A_BTBOX_EVB/US282A_BTBOX_DVB_ATS2825/US282A_BTBOX_DVB_ATS2823/US282A_BTBOX_EVB_ATS2823"
	exit 1
fi

if [ $SupportWaves == 1 ]
then
	sed '0,/^\/\/\#define WAVES_ASET_TOOLS/s//\#define WAVES_ASET_TOOLS/' ../inc/case_independent.h> tmpfile
	mv tmpfile ../inc/case_independent.h		 		    
else
	sed '0,/^\#define WAVES_ASET_TOOLS/s//\/\/\#define WAVES_ASET_TOOLS/' ../inc/case_independent.h > tmpfile
	mv tmpfile ../inc/case_independent.h  						
fi

#判断cfg_dae_txt_name文件是否存在 -f表示判断文件 -d表示判断目录    
if [ ! -f "./config_txt/$cfg_dae_txt_name" ]
then
    echo "ConfigError: ./config_txt/$cfg_dae_txt_name not exist"
    exit 1;
fi 

if [ $SupportBQB == 1 ]
then
	sed '0,/^\/\/\#define __BQB_TEST_PROFILE_/s//\#define __BQB_TEST_PROFILE_/' ../inc/case_independent.h> tmpfile
	mv tmpfile ../inc/case_independent.h

	cp -f 	../ap/bluetooth_stack_BQB/bt_src_lib/*.o  ../ap/bluetooth_stack/bt_src_lib	
fi

#在编译之前生成config.h，保证更新config.txt,程序可正常编译
echo "*************************Build config.bin******************************"
../tools/Gen_config/Genheader.exe       ./config_txt/$config_txt_name ./config_txt/$cfg_dae_txt_name     ../inc/config_id.h

#打包的时候再产生config.bin
#../tools/Gen_config/Genbin.exe          ./config_txt/$config_txt_name ./config_txt/$cfg_dae_txt_name     ./bin/config.bin

#比较不同板型生成的config_id.h是否一致，如果不一致，说明配置项有漏加的情况，需要做同步
#如果方案只有一个板型，可以去除以下部分语句
#echo "*************************Compare config.bin******************************"
#../tools/Gen_config/Genheader.exe     ./config_txt/US282A_BTBOX_EVB.txt   tmpfile   
#
#diff tmpfile ../inc/config_id.h
#if [ $? != 0 ]
#then
#  echo -e "config.bin is different!"
#	sleep 3s;
#	rm -f tmpfile
#	exit 1		
#else
#  echo -e "config.bin is same"
#  rm -f tmpfile
#fi	

#转换文件格式
unix2dos ./config_txt/*
    


 
