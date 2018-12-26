::该批处理用于演示如何调用case/fwpkg目录下的makefile完成代码编译，固件生成操作
::请根据使用习惯，或使用命令行中直接调用makefile谓命令的方式，或调用该批处理实现特定功能
::批处理速度 build_only.bat > target_clean_build.bat > clean_build.bat

::以下几个配置项只能从中选择一个，不能同时生成
::make build_DVB_ATS2825用于生成2825普通固件
::make build_DVB_ATS2825

::make build_DVB_ATS2825W 用于生成2825带W音效固件
::make build_DVB_ATS2825W

::make build_DVB_ATS2825_OTA用于生成2825含OTA功能的固件
::make build_DVB_ATS2825_OTA

::make build_DVB_ATS2825_TWS用于生成2825含TWS功能的固件
make build_DVB_ATS2825_TWS

::make build_DVB_ATS2823T用于生成2823T固件
::make build_DVB_ATS2823T

::make build_DVB_ATS2823_TWS用于生成2823的固件
::make build_DVB_ATS2823_TWS

::make build_DVB_ATS2823W用于生成2823带W音效固件
::make build_DVB_ATS2823W

::make build_EVB用于生成EVB固件
::make build_EVB

::make build_EVB_TWS用于生成EVB_TWS固件
::make build_EVB_TWS

::make build_EVB_W用于生成EVB带W音效固件
::make build_EVB_W

::make build_EVB_W_TWS用于生成EVB对箱带W音效固件
::make build_EVB_W_TWS

::make build_DVB_W_TWS用于生成DVB对箱带W音效固件
::make build_DVB_W_TWS