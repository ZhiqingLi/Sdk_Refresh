@echo off
cd /d %~dp0
@echo on
riscv32-elf-xmaker -b res.xm || goto err
riscv32-elf-xmaker -b xcfg.xm || goto err
exit /b 0

:err
@echo off
if "%1"=="" pause
exit /b 1
