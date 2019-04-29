#!/bin/sh

function usage()
{
	echo "build_actions.sh: helpinfo"
	echo -e "\tusage: ./build/build_actions.sh target ini_file build_type" 
}
#ini_file=$2
build_target=$1
build_type=$2
#device_proj_file=$4

TIME_BEGIN=$(date +%s)
ROOT=`pwd`
BUILD_OUT_DIR=$ROOT/out
BUILD_FW_DIR=image
BUILD_OUT_LOG_DIR_NAME=build_log
BUILD_PLATFORM=bk
PLATFORM_DIR=./platform/$BUILD_PLATFORM

if [ "$build_type" == "normal" ]; then
	build_type="all"
fi

#build mdd first
make -C middleware/mdd -f Makefile.bk $build_type

#build pdd
make -C middleware/pdd_beken -f Makefile $build_type

#build the firmware
make -C $PLATFORM_DIR PROJECT=$build_target $build_type



echo "build end, $(expr $(date +%s) - $TIME_BEGIN) seconds"
