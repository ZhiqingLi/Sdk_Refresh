::该批处理用于演示如何调用case/fwpkg目录下的makefile完成代码编译，固件生成操作
::请根据使用习惯，或使用命令行中直接调用makefile谓命令的方式，或调用该批处理实现特定功能
::批处理速度 build_only.bat > target_clean_build.bat > clean_build.bat

::make clean用于清除目标文件，如果代码确实只是修改部分文件，为了加快编译过程，可以忽略该操作
make clean

::以下几个配置项只能从中选择一个，不能同时生成

::make DVB_ATS2825用于生成2825普通固件
::make DVB_ATS2825

::make DVB_ATS2825W用于生成2825带W音效固件
::make DVB_ATS2825W

::make DVB_ATS2825_OTA用于生成2825含OTA功能的固件
::make DVB_ATS2825_OTA

::make DVB_ATS2825_TWS用于生成2825含TWS功能的固件
make DVB_ATS2825_TWS

::make DVB_ATS2823T用于生成2823T的固件
::make DVB_ATS2823T

::make DVB_ATS2823_TWS用于生成2823的固件
::make DVB_ATS2823_TWS

::make DVB_ATS2823W用于生成2823带W音效固件
::make DVB_ATS2823W

::make EVB用于生成EVB固件
::make EVB

::make EVB_TWS 用于生成EVB_TWS固件
::make EVB_TWS 

::make EVB_W用于生成EVB带W音效固件
::make EVB_W

::make EVB_W_TWS用于生成EVB对箱带W音效固件
::make EVB_W_TWS

::make DVB_ATS2825W_TWS用于生成DVB对箱带W音效固件
::make DVB_ATS2825W_TWS


::make DVB_ATS2825_TWS_BQB用于生成DVB对箱带W音效固件
::make DVB_ATS2825_TWS_BQB