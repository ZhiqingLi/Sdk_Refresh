@echo off
cd /d %~dp0
set proj_name=app
cd ..\..\
for %%a in ("%cd%") do echo 1 > %cd%\Output\obj\projects\%%~nxa\ram.o
cd Output\bin\
@echo on
riscv32-elf-objcopy -O binary %proj_name%.rv32 %proj_name%.bin || goto err
riscv32-elf-xmaker -b app.xm || goto err
if exist C:\upload\upload.bat       (call C:\upload\upload.bat %proj_name%.dcf)
riscv32-elf-xmaker -b download.xm || goto err
@echo off
exit /b 0
:err
@echo off
if "%1"=="" pause
exit /b 1