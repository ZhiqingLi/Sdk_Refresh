#!/bin/sh

if [ $# -lt 3 ]; then
	echo "build_actions.sh: missing param"
	usage
	exit 1;
fi

function usage()
{
	echo "build_actions.sh: helpinfo"
	echo -e "\tusage: ./build/build_actions.sh target ini_file build_type" 
}
ini_file=$2
build_target=$1
build_type=$3
#device_proj_file=$4

TIME_BEGIN=$(date +%s)
ROOT=`pwd`
BUILD_OUT_DIR=out
BUILD_FW_DIR=image
BUILD_OUT_LOG_DIR_NAME=build_log
BUILD_PLATFORM=actions
PLATFORM_DIR=./platform/$BUILD_PLATFORM
BUILD_VAR_FILE=build_var.mk
BUILD_COMMON_MK_FILE=actions_common.mk

BUILD_FW_NAME=`grep -a "Firmware_name" $PLATFORM_DIR/case/fwpkg/config_txt/$ini_file | awk -F '=' '{print $2}'`
#if [ "$BUILD_FW_NAME" == "" ]; then
#	echo "can't get Firmware_name, please check again!"
#	exit 1;
#fi
if [ ! -d $BUILD_OUT_DIR/$build_target ]; then
	mkdir -p $BUILD_OUT_DIR/$build_target
	echo "mkdir $BUILD_OUT_DIR/$build_target"
fi
if [ ! -d $BUILD_OUT_DIR/$build_target/$BUILD_FW_DIR ]; then
	mkdir -p $BUILD_OUT_DIR/$build_target/$BUILD_FW_DIR
	echo "mkdir $BUILD_OUT_DIR/$build_target/$BUILD_FW_DIR"
fi
if [ ! -d $BUILD_OUT_DIR/$build_target/$BUILD_OUT_LOG_DIR_NAME ]; then
	mkdir -p $BUILD_OUT_DIR/$build_target/$BUILD_OUT_LOG_DIR_NAME
fi

function actions_prebuild()
{
	echo "build prebuild"

	#if [ "$device_proj_file" != "" ]; then
	#cp -f $device_proj_file $PLATFORM_DIR/case/cfg
	#fi
	touch $BUILD_OUT_DIR/$BUILD_VAR_FILE
	echo "#***************************************************************" | tee $BUILD_OUT_DIR/$BUILD_VAR_FILE
	echo "#   该文件有build.sh在编译时生成，请不要手动修改               *" | tee -a $BUILD_OUT_DIR/$BUILD_VAR_FILE
	echo "#   在common.mk中被包含                                        *" | tee -a $BUILD_OUT_DIR/$BUILD_VAR_FILE
	echo "#***************************************************************" | tee -a $BUILD_OUT_DIR/$BUILD_VAR_FILE
	echo -e "\n" | tee -a $BUILD_OUT_DIR/$BUILD_VAR_FILE
	echo "BUILD_PLATFORM  = $BUILD_PLATFORM" | tee -a $BUILD_OUT_DIR/$BUILD_VAR_FILE
	echo "ROOT_DIR        = $ROOT" | tee -a $BUILD_OUT_DIR/$BUILD_VAR_FILE
	echo -e "\n" | tee -a $BUILD_OUT_DIR/$BUILD_VAR_FILE
	mv $BUILD_OUT_DIR/$BUILD_VAR_FILE $PLATFORM_DIR/case/cfg/$BUILD_VAR_FILE
	#cp -f build/$BUILD_COMMON_MK_FILE $PLATFORM_DIR/case/cfg/common.mk
}

function actions_build_release()
{
	echo "****************************************************************"
	echo "*   build release begin"
	echo "****************************************************************"

	actions_prebuild
	./build/clean.sh $BUILD_PLATFORM $build_target
	echo "clean.sh done"

	local build_log_dir=./../../../../$BUILD_OUT_DIR/$build_target/$BUILD_OUT_LOG_DIR_NAME
	cd $PLATFORM_DIR/case/fwpkg
	./config.sh ./config_txt/$ini_file  2>&1 | tee $build_log_dir/config.log

	if [ $? != 0 ]; then
		exit 1
	fi
#exit 1
	make -C ./../ap/common
	if [ $? != 0 ]; then
    		exit 1
	fi

	#make -C ./../../../../middleware/linkPlay/build
	make -C ./../../../../middleware/mdd -f Makefile.actions
	make -C ./../../../../middleware/pdd_actions/ap_message
	make -C ./../../../../middleware/pdd_actions/pdd
	if [ $? != 0 ]; then
    		exit 1
	fi
	#exit 1

	./make.sh 2 $build_log_dir 2>&1 | tee $build_log_dir/make.log
#	make -C $PLATFORM_DIR/case/cfg | tee ./make.log
	grep -a "Error :section" $build_log_dir/make.log
	if [ $? == 0 ]; then
    		echo "make error-->exit"
    		error_exit;
	fi

	make -C ./../../../../middleware/bluetooth_stack
	if [ $? != 0 ]; then
    		exit 1
	fi

	./build.sh ./config_txt/$ini_file $build_log_dir 2>&1 | tee $build_log_dir/build.log
	cd -
}

function actions_build_part()
{
	local build_type=$1

	actions_prebuild
	pwd
	local build_log_dir=./../../../../$BUILD_OUT_DIR/$build_target/$BUILD_OUT_LOG_DIR_NAME
	cd $PLATFORM_DIR/case/fwpkg
	if [ $build_type == 1 ]; then
		# only excute config.sh for device build
		echo "****************************************************************"
		echo "*   build begin, only excute config.sh for build project"
		echo "****************************************************************"
		./config.sh ./config_txt/$ini_file  2>&1 | tee $build_log_dir/config.log
	elif [ $build_type == 2 ]; then
		# only excute build.sh for device build
		echo "****************************************************************"
		echo "*   build begin, only excute build.sh for build project"
		echo "****************************************************************"
		./build.sh ./config_txt/$ini_file $build_log_dir 2>&1 | tee $build_log_dir/build.log
	elif [ $build_type == 3 ]; then
		#clean:
		echo "make clean"
		./make.sh 0 $build_log_dir 2>&1 | tee $build_log_dir/make.log

	elif [ $build_type == 4 ] || [ $build_type == 5 ]; then
		#clean_target:
		echo "make clean_target"
		./make.sh 1 $build_log_dir 2>&1 | tee $build_log_dir/make.log
	elif [ $build_type == 8 ]; then
		echo "****************************************************************"
		echo "*   build begin, complile actions linkplay lib"
		echo "****************************************************************"
		# compile ap/common
		make -C ./../ap/common/data
		make -C ./../ap/common/applib
		# make asr.a
		echo "PROTOCAL := LINKPLAY" > linkplay.ini
		./config.sh ./config_txt/linkplay_demo.ini 2>&1 | tee $build_log_dir/config.log
		#make clean -C ./../../../../middleware/bluetooth_stack
		#make -C ./../../../../middleware/bluetooth_stack
		#make clean -C ./../../../../middleware/linkPlay/build
		#make -C ./../../../../middleware/linkPlay/build
		make clean -C ./../../../../middleware/mdd
		make -C ./../../../../middleware/mdd
		make clean -C ./../../../../middleware/pdd_actions/ap_message
		make -C ./../../../../middleware/pdd_actions/ap_message
		make clean -C ./../../../../middleware/pdd_actions/pdd
		make -C ./../../../../middleware/pdd_actions/pdd
	elif [ $build_type == 9 ]; then
		echo "****************************************************************"
		echo "*   build begin, complile actions linkplay bluetooth stack"
		echo "****************************************************************"
		# compile ap/common
		make -C ./../ap/common/data
		make -C ./../ap/common/applib
		echo "PROTOCAL := LINKPLAY" > linkplay.ini
		./config.sh ./config_txt/linkplay_demo.ini 2>&1 | tee $build_log_dir/config.log
		make clean -C ./../../../../middleware/bluetooth_stack
		make -C ./../../../../middleware/bluetooth_stack
	else
		echo "****************************************************************"
		echo "*   build begin, complile all except middleware"
		echo "****************************************************************"
		pwd
		./config.sh ./config_txt/$ini_file  2>&1 | tee $build_log_dir/config.log
		./make.sh 2 $build_log_dir 2>&1 | tee $build_log_dir/make.log
		./build.sh ./config_txt/$ini_file $build_log_dir 2>&1 | tee $build_log_dir/build.log
	fi
	cd -
	if [ $build_type == 5 ]; then
		# clean middleware
		echo "make clean middleware"
		make -C middleware/bluetooth_stack clean
		#make -C middleware/linkplay/build clean
		make -C middleware/mdd clean
		make -C middleware/pdd_actions/ap_message clean
		make -C middleware/pdd_actions/pdd clean
		make -C middleware/ap_test/ATT clean
	fi
}
function actions_show_firmware()
{
	echo "firmware out dir: $BUILD_OUT_DIR/$build_target/$BUILD_FW_DIR"
	ls -l $BUILD_OUT_DIR/$build_target/$BUILD_FW_DIR
}

function actions_check_target_before_build()
{
	echo "check target($build_target) should to build or not befor build_type"
	if [ "$build_target" == "actions_sdk_linkplay" ]; then
		actions_prebuild
		./build/build_actions_sdk.sh $build_target
		return 0
	elif [ "$build_target" == "actions_lib_linkplay" ]; then
		actions_build_part 8
		ls -l platform/$BUILD_PLATFORM/psp_rel/lib/asr.a
		return 0
	elif [ "$build_target" == "actions_btstack_linkplay" ]; then
		actions_build_part 9
		return 0
	else
		return 1
	fi
}

need_continue=1

actions_check_target_before_build
if [ $? == 0 ]; then
	need_continue=0
fi

echo "need_continue: $need_continue"
if [ $need_continue == 1 ]; then
if [ "$build_type" == "release" ]; then
	actions_build_release
	actions_show_firmware
elif [ "$build_type" == "config" ]; then
	actions_build_part 1
elif [ "$build_type" == "build" ]; then
	actions_build_part 2
	actions_show_firmware
elif [ "$build_type" == "clean" ]; then
	actions_build_part 3
elif [ "$build_type" == "clean_target" ]; then
	actions_build_part 4
elif [ "$build_type" == "clean_all" ]; then
	actions_build_part 5
else
	actions_build_part 0
	actions_show_firmware
fi
fi
echo "build end, $(expr $(date +%s) - $TIME_BEGIN) seconds"
