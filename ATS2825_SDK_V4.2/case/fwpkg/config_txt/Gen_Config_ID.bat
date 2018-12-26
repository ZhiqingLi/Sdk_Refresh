@echo off
for /f %%i in ('dir /b .\*.txt') do (
.\..\..\tools\Gen_config\Genheader.exe    .\%%i      .\..\..\inc\config_id.h
GOTO _DONE
)
:_DONE
pause
