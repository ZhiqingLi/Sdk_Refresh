#!/bin/bash

# This script compile source files and generate object files:
# 2016-06-12	-	wuyufan

# 该脚本文件用于编译源文件，包括case下的各个文件，并生成最终的目标文件
# 该脚本有一个参数
# 0 --- clean object files and target files
# 1 --- clean only target files
# 2 --- rebuild all object files and target files


if [ $# != 1 ]
then
    echo "Enter Build type[0/1/2]:"
    echo "0: clean object files and target files(make clean)"
    echo "1: clean target files(make clean_target)"
    echo "2: rebuild all object files and target files(make)"
        
    read -n1 buildtype
     
    case $buildtype in
    0)
          echo "rebuild all source file and link";;
    1)

          echo "rebuild modify source file and link";;
    2) 
          echo "link only";;
    *)
          echo "error build type:$buildtype"
          exit 1;;
    esac
else
    buildtype=$1
fi

#判断之前配置结果是否正确，如果不正确，则停止程序编译
grep "ValueError" config.log 
if [ $? == 0 ]
then
    echo "config error-->exit"
    echo "compile error" 
    exit 1;
fi

grep "ConfigError" config.log 
if [ $? == 0 ]
then
    echo "config error-->exit"
    echo "compile error" 
    exit 1;
fi

cd ../cfg
if [[ $buildtype = 0 ]]	
then
	make clean 
elif [[ $buildtype = 1 ]]
then
	make clean_target 
else
    make -j16
    if [ $? != 0 ]
    then
        echo "compile error"     
    fi
fi
cd ../fwpkg