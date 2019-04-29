#!/bin/sh

#./build/create_current_build.sh $INI_FILE $BUILD_TARGET $BUILD_PLATFORM $build_type

build_target=$1
build_platform=$2
build_target_type=$3
ini_file=$4

BUILD_OUT_DIR=out
BUILD_CURRENT_MK_FILE=$BUILD_OUT_DIR/current_build.mk

if [ -d out ]; then
	if [ -f $BUILD_CURRENT_MK_FILE ]; then
	rm -f $BUILD_CURRENT_MK_FILE
	fi
	touch $BUILD_CURRENT_MK_FILE
	echo "BUILD_INI_FILE=$ini_file" | tee $BUILD_CURRENT_MK_FILE
	echo "BUILD_TARGET=$build_target" | tee -a $BUILD_CURRENT_MK_FILE
	echo "BUILD_PLATFORM=$build_platform" | tee -a $BUILD_CURRENT_MK_FILE
	echo "BUILD_TYPE=$build_target_type" | tee -a $BUILD_CURRENT_MK_FILE

fi
