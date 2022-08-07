@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by EGGSAS.HPJ. >"hlp\EGGSAS.hm"
echo. >>"hlp\EGGSAS.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\EGGSAS.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\EGGSAS.hm"
echo. >>"hlp\EGGSAS.hm"
echo // Prompts (IDP_*) >>"hlp\EGGSAS.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\EGGSAS.hm"
echo. >>"hlp\EGGSAS.hm"
echo // Resources (IDR_*) >>"hlp\EGGSAS.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\EGGSAS.hm"
echo. >>"hlp\EGGSAS.hm"
echo // Dialogs (IDD_*) >>"hlp\EGGSAS.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\EGGSAS.hm"
echo. >>"hlp\EGGSAS.hm"
echo // Frame Controls (IDW_*) >>"hlp\EGGSAS.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\EGGSAS.hm"
REM -- Make help for Project EGGSAS


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\EGGSAS.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\EGGSAS.hlp" goto :Error
if not exist "hlp\EGGSAS.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\EGGSAS.hlp" Debug
if exist Debug\nul copy "hlp\EGGSAS.cnt" Debug
if exist Release\nul copy "hlp\EGGSAS.hlp" Release
if exist Release\nul copy "hlp\EGGSAS.cnt" Release
echo.
goto :done

:Error
echo hlp\EGGSAS.hpj(1) : error: Problem encountered creating help file

:done
echo.
