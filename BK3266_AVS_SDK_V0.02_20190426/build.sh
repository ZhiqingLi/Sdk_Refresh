#!/bin/sh

param_count=$#
build_type=normal

function usage()
{
    echo "build.sh: helpinfo"
    echo -e "usage: build.sh target [build_type]"
    echo -e "param: target, you can select one from following items;\n"
    local print_msg="target:Intro:Platform\n"
    for dirs in device/legacy/*
    do
        local device_mk_file=$dirs/device.mk
	#echo "device_mk_file: $device_mk_file"
        local build_target_name=`grep NAME= $device_mk_file | sed "s|NAME=||"`
        if [ "$build_target_name" == "" ]; then
            continue;
        fi
	local build_target_intro=`grep INTRO= $device_mk_file | sed "s|INTRO=||"`
        local build_target_platform=`grep BUILD_PLATFORM= $device_mk_file | sed "s|BUILD_PLATFORM=||"`
        if [ "$build_target_intro" == "" ]; then
		build_target_intro="*"
	fi
	if [ "$build_target_platform" == "" ]; then
		build_target_platform="*"
	fi
	print_msg+="$build_target_name:$build_target_intro:$build_target_platform\n"
#device_ini_file=`grep DEVICE_INI_FILE= $device_mk_file | sed "s|DEVICE_INI_FILE=||"`
    done
    echo -e "$print_msg" | column -t -s ":"
    echo -e "\nbuild_type options: "
    echo "release      -  for jenkins build, compile all include middleware"
    echo "config       -  only execute config.sh for project config"
    echo "build        -  only execute build.sh"
    echo "clean        -  clean object file"
    echo "clean_target -  clean target file only"
    echo "clean_all    -  clean object file inclue middleware dir"
    echo "you can also choose no build_type, it will execute config.sh make.sh and build.sh."
    echo -e "\nExample: \nbuild.sh actions_linkplay_demo"
    echo "build.sh actions_linkplay_demo release"
    echo "build.sh actions_linkplay_demo config"
    echo "build.sh actions_linkplay_demo build"
}

function error_exit()
{
    echo "!!!Warning: Build Error"
    #echo "Press [Ctrl+C] to exit..."
    #while true;
    #do
    #    sleep 5;
    #done
    usage
    exit 1
}
#if [$ param_count -gt 1]; then

echo "param_count: $param_count"
if [ $param_count -lt 1 ]; then
#ls - l . / case/fwpkg/config_txt/*.ini
    error_exit;
elif [ $param_count -gt 1 ]; then
    build_type=$2
    echo "build_type: $build_type"
fi

INI_FILE=
ROOT=`pwd`
BUILD_TARGET=$1
BUILD_PLATFORM=
device_mk_file=
device_proj_dir=
device_ini_file=
device_cfg_file=
device_txt_file=
search_device=0

for dir in device/legacy/*
do
	device_mk_file=$dir/device.mk
	device_proj_dir=$dir
	#echo "device_mk_file: $device_mk_file"
	search_ret=`grep -w $BUILD_TARGET $device_mk_file`
	#echo "search_ret: $search_ret"
	if [ "$search_ret" != "" ]; then
		target_name=`grep NAME= $device_mk_file | sed "s|NAME=||"`
		if [ "$target_name" != "$BUILD_TARGET" ]; then
			continue;
		fi
		device_ini_file=`grep DEVICE_INI_FILE= $device_mk_file | sed "s|DEVICE_INI_FILE=||"`
		if [ "$device_ini_file" != "" ]; then
			INI_FILE=$device_ini_file
		fi
		device_cfg_file=`grep DEVICE_CFG_FILE= $device_mk_file | sed "s|DEVICE_CFG_FILE=||"`
		device_txt_file=`grep DEVICE_TXT_FILE= $device_mk_file | sed "s|DEVICE_TXT_FILE=||"`
		build_platform=`grep BUILD_PLATFORM= $device_mk_file | sed "s|BUILD_PLATFORM=||"`
		if [ "$build_platform" != "" ]; then
			BUILD_PLATFORM=$build_platform
		fi
		search_device=1
		break;
	fi
done

if [ $search_device != 1 ]; then
	echo "can't find build target, please check again!"
    	error_exit;
fi
if [ ! -f $device_proj_dir/proj.inc ]; then
	echo "can't find proj.inc file, please check again!"
	exit 1;
fi
if [ "$BUILD_PLATFORM" == "" ]; then
	echo "build.sh: config BUILD_PLATFORM error."
	exit 1;
fi
if [ "$BUILD_PLATFORM" == "actions" ]; then
cp -f $device_proj_dir/proj.inc ./platform/$BUILD_PLATFORM/case/cfg
cp -f $device_proj_dir/$INI_FILE ./platform/$BUILD_PLATFORM/case/fwpkg/config_txt
cp -f $device_proj_dir/$device_cfg_file ./platform/$BUILD_PLATFORM/case/fwpkg/config_txt
cp -f $device_proj_dir/$device_txt_file ./platform/$BUILD_PLATFORM/case/fwpkg/config_txt
if [ ! -f "./platform/$BUILD_PLATFORM/case/fwpkg/config_txt/$INI_FILE" ]; then
    echo "build.sh: ./platform/$BUILD_PLATFORM/case/fwpkg/config_txt/$INI_FILE not exist"
    exit 1;
fi
fi

echo "ROOT: $ROOT"
echo "BUILD_TARGET: $BUILD_TARGET"
echo "INI_FILE: $INI_FILE"
echo "BUILD_PLATFORM: $BUILD_PLATFORM"
echo "device_cfg_file: $device_proj_dir/$device_cfg_file"
echo "device_txt_file: $device_proj_dir/$device_txt_file"

if [ ! -d ./out ]; then
	mkdir out
fi

if [ "$BUILD_PLATFORM" == "actions" ]; then
	./build/create_current_build.sh $BUILD_TARGET $BUILD_PLATFORM $build_type $INI_FILE
	./build/build_actions.sh $BUILD_TARGET $INI_FILE $build_type
elif [ "$BUILD_PLATFORM" == "bk" ]; then
	./build/create_current_build.sh $BUILD_TARGET $BUILD_PLATFORM $build_type $INI_FILE
	./build/build_bk.sh $BUILD_TARGET $build_type
fi
