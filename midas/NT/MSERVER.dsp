# Microsoft Developer Studio Project File - Name="MSERVER" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=MSERVER - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MSERVER.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MSERVER.mak" CFG="MSERVER - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MSERVER - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "MSERVER - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MSERVER - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\WinRel"
# PROP BASE Intermediate_Dir ".\WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\WinRel"
# PROP Intermediate_Dir ".\WinRel"
F90=fl32.exe
# ADD BASE F90 /I "WinRel/"
# ADD F90 /I "WinRel/"
# ADD BASE CPP /nologo /W3 /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_X86_" /FR /YX /c
# ADD CPP /nologo /W3 /O2 /I "..\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_X86_" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:IX86
# ADD LINK32 winmm.lib wsock32.lib /nologo /subsystem:console /machine:IX86 /out:"c:\midas\nt\bin\mserver.exe"

!ELSEIF  "$(CFG)" == "MSERVER - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\WinDebug"
# PROP BASE Intermediate_Dir ".\WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\debug"
# PROP Intermediate_Dir ".\debug"
# PROP Ignore_Export_Lib 0
F90=fl32.exe
# ADD BASE F90 /I "WinDebug/"
# ADD F90 /FR /I "WinDebug/"
# ADD BASE CPP /nologo /W3 /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_X86_" /FR /YX /c
# ADD CPP /nologo /W3 /Gm /Gi /ZI /Od /I "..\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_X86_" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 netapi32.lib winmm.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:IX86
# ADD LINK32 winmm.lib wsock32.lib advapi32.lib /nologo /subsystem:console /debug /machine:IX86 /out:"\midas\nt\bin\mserver.exe"
# SUBTRACT LINK32 /profile /map

!ENDIF 

# Begin Target

# Name "MSERVER - Win32 Release"
# Name "MSERVER - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=\MIDAS\SRC\MIDAS.C
# End Source File
# Begin Source File

SOURCE=\MIDAS\SRC\MRPC.C
# End Source File
# Begin Source File

SOURCE=\MIDAS\SRC\MSERVER.C
# End Source File
# Begin Source File

SOURCE=\MIDAS\SRC\ODB.C
# End Source File
# Begin Source File

SOURCE=\MIDAS\SRC\SYSTEM.C
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
