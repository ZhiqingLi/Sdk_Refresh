@echo off
set TARGET_PRJ=dueros
set DOWNLOAD_FOLDER=D:\Dev_env\dld
if "%1"=="1" (
make clean T=%TARGET_PRJ%
make T=%TARGET_PRJ% GEN_LIB=1
) else if "%1"=="0" (
make T=%TARGET_PRJ%
) else if "%1"=="2" (
"C:\Program Files (x86)\GNU Tools ARM Embedded\4.9 2014q4\bin\arm-none-eabi-objdump.exe"  -D out\%TARGET_PRJ%\%TARGET_PRJ%.elf>%TARGET_PRJ%.dis
)else if "%1"=="3" (
"%cd%\tools\sign_binary.exe"  out\%TARGET_PRJ%\%TARGET_PRJ%.bin %cd%\tools\private.pem
del temp_app.bin
del temp_sig.bin
)
@echo on