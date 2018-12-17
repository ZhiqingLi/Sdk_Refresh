#!/bin/bash

# This script build these files:
# *.FW---> firmware 
# *.OTA--> OTA firmare
# *.HEX--> upgrade file for end user
# *.FWU--> upgrade file for factory product
# *.ATF--> autotest file for ATT
# 2016-06-12	-	wuyufan

# 该脚本文件用于生成最终量产的fw文件，OTA文件，HEX升级文件以及ATT工具所需的atf文件
# 通过makefile调用该脚本，则需要传递一个参数，相应的ini文件

echo "making fw"

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

#读取固件名配置
FW_NAME=`grep -a "Firmware_name" $int_file | awk -F '=' '{print $2}'`

FW_DIR=$FW_NAME

#可根据实际需要确定固件名是否需要加时间信息
#FW_NAME="$FW_NAME"_`date +%y%m%d`

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

FWType=`grep -a "FWType" $int_file | awk -F '=' '{print $2}'`

#读取dae.txt配置
cfg_dae_txt_name=`grep -a "cfg_dae_txt" $int_file | awk -F '=' '{print $2}'`

#聚合atf文件和原有的fw文件形成新的fw文件
FW_ALL_NAME=${FW_NAME%}"_ALL"

echo $FW_EXT_NAME

function error_exit()
{
    echo "!!!Warning: Build Error"
    echo "Press [Ctrl+C] to exit..."
    while true;
    do
        sleep 5;
    done    
}

#判断config_txt_name文件是否存在 -f表示判断文件 -d表示判断目录    
if [ ! -f "./config_txt/$config_txt_name" ]
then
    echo "ConfigError: ./config_txt/$config_txt_name not exist"
    error_exit;
fi   

#判断config_cfg_name文件是否存在 -f表示判断文件 -d表示判断目录  
if [ ! -f "./config_txt/$config_cfg_name" ]
then
    echo "ConfigError: ./config_txt/$config_cfg_name not exist"
    error_exit;
fi 

#判断之前编译的结果是否正确,如果编译结果不正确，则不再做固件生成操作
grep -a "compile error" make.log 
if [ $? == 0 ]
then
    echo "make error-->exit"
    error_exit;
fi

#判断内存是否正确
grep -a "Error :section" make.log 
if [ $? == 0 ]
then
    echo "make error-->exit"
    error_exit;
fi

echo "Support OTA: $SupportOTA" 
echo "Support TWS: $SupportTWS" 
 
#if [ -d "../ap/bluetooth_stack" ]
#then
#    if [ $SupportTWS == 1 ]
#    then 
#        echo "copy btstack_tws.ap" 
#        cp -f ../ap/bluetooth_stack/btstack.ap ./ap/btstack_tws.ap        
#    else
#        echo "copy btstack_normal.ap" 
#        cp -f ../ap/bluetooth_stack/btstack.ap ./ap/btstack_normal.ap
#    fi
#fi

#if [ $SupportTWS == 1 ]
#then  
#    cp -f ./ap/btstack_tws.ap  ./ap/btstack.ap      
#else
#    cp -f ./ap/btstack_normal.ap ./ap/btstack.ap 
#fi

if [ $SupportWaves == 1 ]
then  
    cp -f ./dsplib/audiopp_waves.dsp  ./dsplib/audiopp.dsp      
else
    cp -f ./dsplib/audiopp_normal.dsp ./dsplib/audiopp.dsp   
fi

#判断cfg_dae_txt_name文件是否存在 -f表示判断文件 -d表示判断目录    
if [ ! -f "./config_txt/$cfg_dae_txt_name" ]
then
    echo "ConfigError: ./config_txt/$cfg_dae_txt_name not exist"
    error_exit;
fi 

cp -f ./config_txt/$config_txt_name ./bin/config.txt
cp -f ./config_txt/$cfg_dae_txt_name ./bin/cfg_dae.txt
cp -f ./config_txt/$config_cfg_name ./fwimage.cfg

../tools/Gen_config/Genbin.exe          ./config_txt/$config_txt_name ./config_txt/$cfg_dae_txt_name        ./bin/config.bin

#以下程序是生成一个版本号较新的固件及OTA文件，用于OTA固件升级测试，两个文件版本号必须不一致，并且MODULE_NUM
#必须一致
#程序会搜索config_txt目录下是否包含配置文件名+"_ver_new"的配置文件，如果存在，则进行新固件的生成
#这部分程序是测试程序，方便OTA测试，正式发布程序可去掉这部分固件的打包
OTA_new_cfg=0
if [ $SupportOTA == 1 ]
then
    new_config_cfg_name=${config_cfg_name%.cfg}"_ver_new.cfg"
    echo $new_config_cfg_name
    if [ -f "./config_txt/$new_config_cfg_name" ]
    then
        OTA_new_cfg=1
    else
        OTA_new_cfg=0
    fi 
fi

chgrp Users -R ./
chmod -R 777 ./
mkdir -p $FW_DIR

echo "*************************Reduce AP Size******************************"
../../psp_rel/tools/ap_post_builder.exe ./fwimage.cfg config.ap


echo "*************************Build AFI *****************************"
if [ -f "../../psp_rel/bin_original/buildAFI.bat" ]
then
    chgrp Users ../../psp_rel/bin_original/.*
    chmod -R 777 ../../psp_rel/bin_original/.*
    ./buildAFI.bat
fi    
				
echo "*************************Build Firmware *****************************"
../../psp_rel/tools/maker_py/Maker.exe -c ./fwimage.cfg -o  ./$FW_NAME.FW -mf

if [ $OTA_new_cfg == 1 ]
then
    cp -f ./config_txt/$new_config_cfg_name ./fwimage.cfg 
    ../../psp_rel/tools/maker_py/Maker.exe -c ./fwimage.cfg -o  ./"$FW_NAME"_ver_new.FW -mf   
fi

#rm -f ./bin/config.txt
#rm -f ./bin/config.bin
rm -f ./fwimage.cfg

if [ $FWType == 1 ]
then
    mv -f $FW_NAME.FW ./$FW_DIR
    exit 0;
fi 

echo "*************************CheckSize *****************************"	
../../psp_rel/tools/maker_py/FW2X.exe -fsss $FW_NAME.FW MakeBin FLASHSIZE 123

grep -a "ERROR!! Firmware size is BIGGER than flash size" ./build.log
if [ $? == 0 ]
then
    echo "build error-->exit"
    error_exit;   
fi

echo $FWType
if [ $FWType == 2 ]
then
    exit 0;
fi    

echo "*************************Build UPGRADE.HEX*****************************"
../../psp_rel/tools/maker_py/FW2X.exe -fssf  $FW_NAME.FW  MakeBin MakeUPG $FW_NAME.FWU
cp -f $FW_NAME.FWU $FW_NAME.HEX

if [ $FWType == 3 ]
then
    mv -f $FW_NAME.FW ./$FW_DIR
    exit 0;
fi 

echo "*************************Build ATF*****************************"
if [ -d "../ap_test/" ]
then
    cp -f ./$FW_NAME.FWU ../ap_test/ATT/UPGRADE.HEX
    cp -f ./$FW_NAME.FW ../ap_test/ATT/US282A.FW
    cd ../ap_test/ATT
    pwd
    make clean -f Makefile
    make -f Makefile    
    cp -f actionstest.fw ../../fwpkg/$FW_DIR/$FW_ALL_NAME.FW  
    cd ../../fwpkg 
fi

cp $FW_NAME.FW TEST.FW

chmod 777 TEST.FW

mv -f $FW_NAME.FW $FW_NAME.FWU $FW_NAME.HEX ./$FW_DIR

#判断是否支持OTA功能，支持OTA功能的固件才生成OTA文件
#FW2X f参数不能带相对路径
if [ $SupportOTA == 1 ]
then
    echo "*************************Build OTA*****************************"
    ../../psp_rel/tools/maker_py/FW2X.exe -fssf  TEST.FW  MakeBin MakeOTA $FW_NAME.OTA 
    mv -f $FW_NAME.OTA ./$FW_DIR
    rm -f TEST.FW
fi

if [ $OTA_new_cfg == 1 ]
then
    cp "$FW_NAME"_ver_new.FW TEST.FW
    chmod 777 TEST.FW
    mv -f "$FW_NAME"_ver_new.FW  ./$FW_DIR
    ../../psp_rel/tools/maker_py/FW2X.exe -fssf  TEST.FW  MakeBin MakeOTA "$FW_NAME"_ver_new.OTA 
    mv -f "$FW_NAME"_ver_new.OTA ./$FW_DIR
    rm -f TEST.FW    
fi

chmod -R 777 ./
	
