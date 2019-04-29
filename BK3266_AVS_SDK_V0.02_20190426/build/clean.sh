#!/bin/sh

build_platform=$1
build_target=$2

if [ "$build_platform" == "actions" ]; then
	cd ./platform/$build_platform/case/fwpkg
	./make.sh 0 ./../../../../out/$build_target/build_log 2>&1
	cd -
	#make -C platform/$build_platform/case/fwpkg clean
	make -C middleware/bluetooth_stack clean
	#make -C middleware/linkplay/build clean
	make -C middleware/mdd clean
	make -C middleware/pdd_actions/ap_message clean
	make -C middleware/pdd_actions/pdd clean
	make -C middleware/ap_test/ATT clean
fi
