# Microsoft Developer Studio Project File - Name="HVEDIT" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=HVEDIT - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "hvedit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hvedit.mak" CFG="HVEDIT - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HVEDIT - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "HVEDIT - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "HVEDIT - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\WinDebug"
# PROP BASE Intermediate_Dir ".\WinDebug"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
F90=fl32.exe
# ADD BASE F90 /I "WinDebug/"
# ADD F90 /FR /I "Debug/"
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "\midas\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX /FD /c
# ADD MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 winmm.lib /nologo /subsystem:windows /debug /machine:I386 /out:"\midas\nt\bin\hvedit.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "HVEDIT - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\WinRel"
# PROP BASE Intermediate_Dir ".\WinRel"
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
F90=fl32.exe
# ADD BASE F90 /I "WinRel/"
# ADD F90 /FR /I "Release/"
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "\midas\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Fr /YX /FD /c
# ADD MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 winmm.lib /nologo /subsystem:windows /machine:I386 /out:"\midas\nt\bin\hvedit.exe"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "HVEDIT - Win32 Debug"
# Name "HVEDIT - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\HVABOUT.CPP
# End Source File
# Begin Source File

SOURCE=.\HVCONFIG.CPP
# End Source File
# Begin Source File

SOURCE=.\HVEDIDLG.CPP
# End Source File
# Begin Source File

SOURCE=.\HVEDIT.CPP
# End Source File
# Begin Source File

SOURCE=.\HVEDIT.RC
# End Source File
# Begin Source File

SOURCE=.\HVVIEW.CPP
# End Source File
# Begin Source File

SOURCE=.\MExperiment.cpp
# End Source File
# Begin Source File

SOURCE=.\MHostDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MKey.cpp
# End Source File
# Begin Source File

SOURCE=.\MPasswdDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PrintDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\README.TXT
# End Source File
# Begin Source File

SOURCE=.\STDAFX.CPP
# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=\Midas\NT\LIB\midas.lib
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\HVCONFIG.H
# End Source File
# Begin Source File

SOURCE=.\HVEDIDLG.H
# End Source File
# Begin Source File

SOURCE=.\HVEDIT.H
# End Source File
# Begin Source File

SOURCE=.\HVVIEW.H
# End Source File
# Begin Source File

SOURCE=.\MExperiment.h
# End Source File
# Begin Source File

SOURCE=.\MHostDlg.h
# End Source File
# Begin Source File

SOURCE=.\MKey.h
# End Source File
# Begin Source File

SOURCE=.\MPasswdDlg.h
# End Source File
# Begin Source File

SOURCE=.\PrintDlg.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\STDAFX.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\RES\HVEDIT.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\HVEDIT.RC2
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\HLP\ICON.BMP
# End Source File
# End Group
# End Target
# End Project
