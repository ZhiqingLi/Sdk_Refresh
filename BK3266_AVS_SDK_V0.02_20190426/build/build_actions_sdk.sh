#!/bin/sh

echo "****************************************************************"
echo "*   build actions sdk"
echo "****************************************************************"

if [ $# -lt 1 ]; then
	echo "$0: missing param"
	exit 1
fi

build_target=$1

time_begin=$(date +%s)
actions_linkplay_lib_name=asr.a
actions_linkplay_lib_mdd_name=mdd.a
actions_linkplay_lib_pdd_name=pdd.a
actions_linkplay_lib_lp_mds_name=lp_msg.a
actions_btstack_ap_name=btstack.ap
platform_dir=platform/actions
actions_case_dir=$platform_dir/case
actions_psp_rel_dir=$platform_dir/psp_rel
target_out_dir=out/$build_target
target_out_log_dir=$target_out_dir/build_log
target_out_sdk_dir=$target_out_dir/$build_target
target_platform_dir=$target_out_sdk_dir/$platform_dir
target_actions_case_dir=$target_platform_dir/case
target_actions_psp_rel_dir=$target_platform_dir/psp_rel
echo "actions_linkplay_lib_name: $actions_linkplay_lib_name"
echo "actions_btstack_ap_name: $actions_btstack_ap_name"
echo "platform_dir: $platform_dir"
echo "actions_case_dir: $actions_case_dir"
echo "actions_psp_rel_dir: $actions_psp_rel_dir"
echo "target_out_dir: $target_out_dir"
echo "target_out_log_dir: $target_out_log_dir"
echo "target_out_sdk_dir: $target_out_sdk_dir"
echo "target_platform_dir: $target_platform_dir"
echo "target_actions_case_dir: $target_actions_case_dir"
echo "target_actions_psp_rel_dir: $target_actions_psp_rel_dir"

BUILD_FW_DIR=image
#BUILD_FW_NAME=`grep -a "Firmware_name" $platform_dir/case/fwpkg/config_txt/linkplay_demo.ini | awk -F '=' '{print $2}'`

if [ -d $target_out_dir ]; then
	echo "$target_out_dir is exist, remove it first."
	rm -rf $target_out_dir
fi
mkdir -p $target_out_dir
mkdir -p $target_out_dir/$BUILD_FW_DIR
mkdir -p $target_out_sdk_dir
mkdir -p $target_out_sdk_dir/build
mkdir -p $target_out_sdk_dir/device/legacy
mkdir -p $target_platform_dir

function do_pack_actions_sdk()
{
	# copy platform/actions/case and psp_rel to out/actions_sdk_linkplay/actions_sdk_linkplay
	echo "copy $actions_case_dir -> $target_platform_dir"
	cp -rf $actions_case_dir $target_platform_dir
	echo "copy $actions_psp_rel_dir -> $target_platform_dir"
	cp -rf $actions_psp_rel_dir $target_platform_dir

	# make linkplay lib(asr.a) and bluetooth stack
	local build_log_dir=./../../../../$target_out_log_dir
	cd $platform_dir/case/fwpkg
	# compile ap/common
	make -C ./../ap/common/data
	make -C ./../ap/common/applib
	# make asr.a
	echo "PROTOCAL := LINKPLAY" > linkplay.ini
	./config.sh ./config_txt/linkplay_demo.ini 2>&1 | tee $build_log_dir/config.log
	make clean -C ./../../../../middleware/bluetooth_stack
	make -C ./../../../../middleware/bluetooth_stack
	#make clean -C ./../../../../middleware/linkPlay/build
	#make -C ./../../../../middleware/linkPlay/build
	make -C ./../../../../middleware/mdd
	make -C ./../../../../middleware/pdd_actions/ap_message
	make -C ./../../../../middleware/pdd_actions/pdd
	cd -

	# copy asr.a and btstack.ap to out/actions_sdk_linkplay/actions_sdk_linkplay
	#echo "copy $actions_psp_rel_dir/lib/$actions_linkplay_lib_name -> $target_actions_psp_rel_dir/lib/$actions_linkplay_lib_name"
	#cp -f $actions_psp_rel_dir/lib/$actions_linkplay_lib_name $target_actions_psp_rel_dir/lib/$actions_linkplay_lib_name
	echo "copy $actions_psp_rel_dir/lib/$actions_linkplay_lib_mdd_name -> $target_actions_psp_rel_dir/lib/$actions_linkplay_lib_mdd_name"
	cp -f $actions_psp_rel_dir/lib/$actions_linkplay_lib_mdd_name $target_actions_psp_rel_dir/lib/$actions_linkplay_lib_mdd_name
	echo "copy $actions_psp_rel_dir/lib/$actions_linkplay_lib_pdd_name -> $target_actions_psp_rel_dir/lib/$actions_linkplay_lib_pdd_name"
	cp -f $actions_psp_rel_dir/lib/$actions_linkplay_lib_pdd_name $target_actions_psp_rel_dir/lib/$actions_linkplay_lib_pdd_name
	echo "copy $actions_psp_rel_dir/lib/$actions_linkplay_lib_lp_mds_name -> $target_actions_psp_rel_dir/lib/$actions_linkplay_lib_lp_mds_name"
	cp -f $actions_psp_rel_dir/lib/$actions_linkplay_lib_lp_mds_name $target_actions_psp_rel_dir/lib/$actions_linkplay_lib_lp_mds_name
	echo "copy $actions_case_dir/fwpkg/ap/$actions_btstack_ap_name $target_actions_case_dir/fwpkg/ap/$actions_btstack_ap_name"

	cp -f $actions_case_dir/fwpkg/ap/$actions_btstack_ap_name $target_actions_case_dir/fwpkg/ap/$actions_btstack_ap_name
	cp -f $actions_case_dir/inc/lp_mdd.h $target_actions_case_dir/inc/lp_mdd.h
	cp -f $actions_case_dir/inc/lp_message.h $target_actions_case_dir/inc/lp_message.h
	cp -f $actions_case_dir/inc/lp_type.h $target_actions_case_dir/inc/lp_type.h

	# copy build.sh build/ and device/ to out/actions_sdk_linkplay/actions_sdk_linkplay
	cp -f build.sh $target_out_sdk_dir
	sed -i "/echo \"release/d" $target_out_sdk_dir/build.sh
	sed -i "/build.sh actions_linkplay_demo release/d" $target_out_sdk_dir/build.sh
	cp -f build/build_actions.sh $target_out_sdk_dir/build
	cp -f build/clean.sh $target_out_sdk_dir/build
	cp -f build/create_current_build.sh $target_out_sdk_dir/build
	cp -f build/create_project.sh $target_out_sdk_dir/build
	cp -rf device/legacy/actions_linkplay_demo $target_out_sdk_dir/device/legacy

	# delete useless file
	rm -rf $target_actions_case_dir/fwpkg/config_txt/config_txt_bak
#exit 
	#tar out/actions_sdk_linkplay/actions_sdk_linkplay to out actions_sdk_linkplay.tgz
	echo "tar actions_sdk_linkplay.tgz begin"
	cd $target_out_dir
	tar czf $build_target.tgz $build_target
	cp $build_target.tgz $BUILD_FW_DIR
	cd -
	echo "build actions sdk end"
}

do_pack_actions_sdk

