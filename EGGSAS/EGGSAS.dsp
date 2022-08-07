# Microsoft Developer Studio Project File - Name="EGGSAS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=EGGSAS - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "EGGSAS.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "EGGSAS.mak" CFG="EGGSAS - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "EGGSAS - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "EGGSAS - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "EGGSAS - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MT /W3 /GX /O2 /I "C:\Program Files\ProEssentials5\VC" /I ".\fftw" /I ".\EGGdriver" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 PEGRP32C.lib FFTW3st.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"Release/3CPM EGGSAS.exe" /libpath:"C:\Program Files\ProEssentials5\VC" /libpath:".\fftw" /libpath:".\EGGdriver"

!ELSEIF  "$(CFG)" == "EGGSAS - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /MTd /W3 /Gm /GX /ZI /Od /I "C:\Program Files\ProEssentials5\VC" /I ".\fftw" /I ".\EGGdriver" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 PEGRP32C.lib FFTW3st.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/3CPM EGGSAS.exe" /pdbtype:sept /libpath:"C:\Program Files\ProEssentials5\VC" /libpath:".\fftw" /libpath:".\EGGdriver"

!ENDIF 

# Begin Target

# Name "EGGSAS - Win32 Release"
# Name "EGGSAS - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\analyze.cpp
# End Source File
# Begin Source File

SOURCE=.\AnalyzePeriod.cpp
# End Source File
# Begin Source File

SOURCE=.\ArtifactID.cpp
# End Source File
# Begin Source File

SOURCE=.\Baseline.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigReport.cpp
# End Source File
# Begin Source File

SOURCE=.\Dbaccess.cpp
# End Source File
# Begin Source File

SOURCE=.\Demographics.cpp
# End Source File
# Begin Source File

SOURCE=.\Diagnosis.cpp
# End Source File
# Begin Source File

SOURCE=.\DiagnosticHistory.cpp
# End Source File
# Begin Source File

SOURCE=.\Driver.cpp
# End Source File
# Begin Source File

SOURCE=.\EGGSAS.cpp
# End Source File
# Begin Source File

SOURCE=.\hlp\EGGSAS.hpj

!IF  "$(CFG)" == "EGGSAS - Win32 Release"

# PROP Ignore_Default_Tool 1
USERDEP__EGGSA="hlp\AfxCore.rtf"	"hlp\AfxPrint.rtf"	"hlp\$(TargetName).hm"	
# Begin Custom Build - Making help file...
OutDir=.\Release
TargetName=3CPM EGGSAS
InputPath=.\hlp\EGGSAS.hpj
InputName=EGGSAS

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcw /C /E /M "hlp\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "hlp\$(InputName).hlp" goto :Error 
	copy "hlp\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo hlp\$(InputName).hpj(1) : error: 
	type "hlp\$(InputName).log" 
	:done 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "EGGSAS - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__EGGSA="hlp\AfxCore.rtf"	"hlp\AfxPrint.rtf"	"hlp\$(TargetName).hm"	
# Begin Custom Build - Making help file...
OutDir=.\Debug
TargetName=3CPM EGGSAS
InputPath=.\hlp\EGGSAS.hpj
InputName=EGGSAS

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcw /C /E /M "hlp\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "hlp\$(InputName).hlp" goto :Error 
	copy "hlp\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo hlp\$(InputName).hpj(1) : error: 
	type "hlp\$(InputName).log" 
	:done 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EGGSAS.odl
# End Source File
# Begin Source File

SOURCE=.\EGGSAS.rc
# End Source File
# Begin Source File

SOURCE=.\EGGSASDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\EGGSASView.cpp
# End Source File
# Begin Source File

SOURCE=.\EquipTest.cpp
# End Source File
# Begin Source File

SOURCE=.\FacilityInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphRSA.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Medications.cpp
# End Source File
# Begin Source File

SOURCE=.\PostWaterTest.cpp
# End Source File
# Begin Source File

SOURCE=.\PreStudyInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Recommendations.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportView.cpp
# End Source File
# Begin Source File

SOURCE=.\RespEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\RespRate.cpp
# End Source File
# Begin Source File

SOURCE=.\RSAView.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectPatient.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TenMinView.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=.\WaterLoad.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\analyze.h
# End Source File
# Begin Source File

SOURCE=.\AnalyzePeriod.h
# End Source File
# Begin Source File

SOURCE=.\ArtifactID.h
# End Source File
# Begin Source File

SOURCE=.\Baseline.h
# End Source File
# Begin Source File

SOURCE=.\ConfigReport.h
# End Source File
# Begin Source File

SOURCE=.\Dbaccess.h
# End Source File
# Begin Source File

SOURCE=.\Demographics.h
# End Source File
# Begin Source File

SOURCE=.\Diagnosis.h
# End Source File
# Begin Source File

SOURCE=.\DiagnosticHistory.h
# End Source File
# Begin Source File

SOURCE=.\Driver.h
# End Source File
# Begin Source File

SOURCE=.\EGGSAS.h
# End Source File
# Begin Source File

SOURCE=.\EGGSASDoc.h
# End Source File
# Begin Source File

SOURCE=.\EGGSASView.h
# End Source File
# Begin Source File

SOURCE=.\EquipTest.h
# End Source File
# Begin Source File

SOURCE=.\FacilityInfo.h
# End Source File
# Begin Source File

SOURCE=.\GraphRSA.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Medications.h
# End Source File
# Begin Source File

SOURCE=.\PostWaterTest.h
# End Source File
# Begin Source File

SOURCE=.\PreStudyInfo.h
# End Source File
# Begin Source File

SOURCE=.\Recommendations.h
# End Source File
# Begin Source File

SOURCE=.\ReportView.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h

!IF  "$(CFG)" == "EGGSAS - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help include file...
TargetName=3CPM EGGSAS
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "EGGSAS - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help include file...
TargetName=3CPM EGGSAS
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RespEdit.h
# End Source File
# Begin Source File

SOURCE=.\RespRate.h
# End Source File
# Begin Source File

SOURCE=.\RSAView.h
# End Source File
# Begin Source File

SOURCE=.\SelectPatient.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\sysdefs.h
# End Source File
# Begin Source File

SOURCE=.\TenMinView.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# Begin Source File

SOURCE=.\WaterLoad.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\EGGSAS.ico
# End Source File
# Begin Source File

SOURCE=.\res\EGGSAS.rc2
# End Source File
# Begin Source File

SOURCE=.\res\eggsas1.ico
# End Source File
# Begin Source File

SOURCE=.\res\EGGSASDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\GREEN_LED.ICO
# End Source File
# Begin Source File

SOURCE=.\res\RED_LED.ICO
# End Source File
# Begin Source File

SOURCE=.\res\REDO.BMP
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\UNDO.BMP
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter "cnt;rtf"
# Begin Source File

SOURCE=.\hlp\AfxCore.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\AfxPrint.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\AppExit.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Bullet.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw2.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw4.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurHelp.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditCopy.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditCut.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditPast.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditUndo.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EGGSAS.cnt

!IF  "$(CFG)" == "EGGSAS - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Copying contents file...
OutDir=.\Release
InputPath=.\hlp\EGGSAS.cnt
InputName=EGGSAS

"$(OutDir)\$(InputName).cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "hlp\$(InputName).cnt" $(OutDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "EGGSAS - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Copying contents file...
OutDir=.\Debug
InputPath=.\hlp\EGGSAS.cnt
InputName=EGGSAS

"$(OutDir)\$(InputName).cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "hlp\$(InputName).cnt" $(OutDir)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\FileNew.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileOpen.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FilePrnt.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileSave.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpSBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpTBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecFirst.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecLast.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecNext.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecPrev.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmax.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ScMenu.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmin.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\eggsas.ini
# End Source File
# Begin Source File

SOURCE=.\EGGSAS.reg
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
