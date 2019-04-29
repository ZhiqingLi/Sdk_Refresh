#!/bin/sh

echo "create a new project"

#***************************************************************
#   select your building project's platform
#   
#   actions
#   bk
#
#***************************************************************

platform_actions=actions
platform_bk=bk
echo "please select platform for building project"
echo "Enter platform type:"
echo "[0] actions"
echo "[1] bk"

read -n1 -p "select platform number:" platform_number

case $platform_number in
0)
	PlatformType=$platform_actions
	echo -e "\nPlatform=$PlatformType";;
1)
	PlatformType=$platform_bk
	echo -e "\nPlatform=$PlatformType";;
*)
	echo -e "\nerror platform, exit"
	exit 1;;
esac

#***************************************************************
#   set device/legacy/project_name/device.mk
#
#   need to set param:
#   NAME=xxx  
#   INTRO=
#   DEVICE_INI_FILE=
#   DEVICE_CFG_FILE=
#   DEVICE_TXT_FILE=
#
#***************************************************************
project_name=
function set_project_name()
{
	if [ "$PlatformType" == "$platform_actions" ]; then
		echo -n -e "\nPlease set project name:actions_"
		read -n32 tmp_project_name
		#echo "tmp_project_name: $tmp_project_name"
		project_name=actions_$tmp_project_name
	elif [ "$PlatformType" == "$platform_bk" ]; then
		echo -n -e "\nPlease set project name:bk_"
		read -n32 tmp_project_name
		project_name=bk_$tmp_project_name
	fi
}

function check_project_name_isvalid()
{
	local name=$1
	local name_length=${#name}

	if [ "$PlatformType" == "$platform_actions" ]; then
		if [ $name_length -le 8 ]; then
			echo "Error! project name length is too short, please set again."
			return 1
		fi
	elif [ "$PlatformType" == "$platform_bk" ]; then
		if [ $name_length -lt 3 ]; then
			echo "Error! project name length is too short, please set again."
			return 1
		fi
	fi
	for dir in device/legacy/*
	do
		local device_mk_file=$dir/device.mk
		local tmp_project_name=`grep NAME= $device_mk_file | sed "s|NAME=||"`
		#echo "tmp_project_name: $tmp_project_name"
		if [ "$tmp_project_name" == "$name" ]; then
			echo "Error! '$name' is used already, please set again."
			return 1
		fi
	done
	return 0
}
set_project_name
if [ $project_name == "" ]; then
	echo "no project name, exit"
	exit 1
fi
#echo "set project name: $project_name"
check_project_name_isvalid $project_name
while [ $? != 0 ]
do
	#echo "project name maybe used already, please enter again"
	set_project_name
	#echo "set project name: $project_name"
	check_project_name_isvalid $project_name
done

read -n32 -p "Please set project intr info:" project_intro
#echo "project intro info: $project_intro"

# set project config file: *.ini, *.txt and *.cfg
project_ini_file=""
project_txt_file=""
project_cfg_file=""

function check_project_config_file()
{
	file_type=$1
	file_name=$2
	
	if [ $file_type == 1 ]; then
		check_result=`echo $file_name | grep "\.ini"`
		err_msg="Error! file name should end with '.ini'"
	elif [ $file_type == 2 ]; then
		check_result=`echo $file_name | grep "\.txt"`
		err_msg="Error! file name should end with '.txt'"
	elif [ $file_type == 3 ]; then
		check_result=`echo $file_name | grep "\.cfg"`
		err_msg="Error! file name should end with '.cfg'"
	fi
	if [ "$check_result" == "" ]; then
		echo "$err_msg"
		echo -e "\n"
		return 1
	fi
	return 0
}
function set_project_file_dialoge()
{
	file_type=$1

	if [ $file_type == 1 ]; then
		read -n32 -p "Please set project config ini file(*.ini):" file_name
	elif [ $file_type == 2 ]; then
		read -n32 -p "Please set project config txt file(*.txt):" file_name
	elif [ $file_type == 3 ]; then
		read -n32 -p "Please set project firmware image cfg file(*.cfg):" file_name
		project_cfg_file=$file_name
	fi
	echo "$file_name"
}
function set_project_config_file()
{
	file_type=$1

	file_name=`set_project_file_dialoge $file_type`
	#echo "file_name: $file_name"
	check_project_config_file $file_type $file_name
	while [ $? != 0 ]
	do
		file_name=`set_project_file_dialoge $file_type`
		#echo "file_name: $file_name"
		check_project_config_file $file_type $file_name
	done
	if [ $file_type == 1 ]; then
		project_ini_file=$file_name
	elif [ $file_type == 2 ]; then
		project_txt_file=$file_name
	elif [ $file_type == 3 ]; then
		project_cfg_file=$file_name
	fi
}
set_project_config_file 1
set_project_config_file 2
set_project_config_file 3

project_dir=device/legacy/$project_name
project_device_mk_file=$project_dir/device.mk
project_inc_file=$project_dir/proj.inc

if [ ! -d $project_dir ]; then
	mkdir -p $project_dir
else
	echo "Error! $project_dir is exist, please check again."
	exit 1
fi
if [ -f $project_device_mk_file ]; then
	rm -f $project_device_mk_file
fi
if [ -f $project_inc_file ]; then
	rm -f $project_inc_file
fi
touch $project_device_mk_file
touch $project_inc_file

#***************************************************************
#   write file device/legacy/project_name/device.mk
#***************************************************************
echo "project_name: $project_name"
echo "project_intro: $project_intro"

echo "NAME=$project_name" | tee $project_device_mk_file
echo "INTRO=$project_intro" | tee -a $project_device_mk_file
echo "BUILD_PLATFORM=$PlatformType" | tee -a $project_device_mk_file
echo "CHIPSET=" | tee -a $project_device_mk_file
echo -e "\n" | tee -a $project_device_mk_file

echo "DEVICE_INI_FILE=$project_ini_file" | tee -a $project_device_mk_file
echo "DEVICE_CFG_FILE=$project_txt_file" | tee -a $project_device_mk_file
echo "DEVICE_TXT_FILE=$project_cfg_file" | tee -a $project_device_mk_file



#***************************************************************
#   set file device/legacy/proj.inc
#
#   need to set param:
#   LINKPLAY_CFLAGS :=
#
#***************************************************************
echo -e "\n"
echo "you can set project compile cflags, for example: -DBOARD_LINKPLAY_DEMO"
read -p "set compile cflags or no cflags:" project_cflags

#***************************************************************
#   write file device/legacy/project_name/proj.inc
#***************************************************************

echo "VENDOR          := Linkplay" | tee $project_inc_file
echo "BUILD_PLATFORM  := $PlatformType" | tee -a $project_inc_file
echo "LINKPLAY_CFLAGS := $project_cflags" | tee -a $project_inc_file

echo -e "\nwarning!!! please don't forget copy file $project_ini_file, $project_txt_file and $project_cfg_file to directory '$project_dir'."


