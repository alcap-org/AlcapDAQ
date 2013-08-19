# Microsoft Developer Studio Project File - Name="analyzer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=analyzer - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ANALYZER.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ANALYZER.mak" CFG="analyzer - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "analyzer - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "analyzer - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "analyzer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE F90 /Ox /I "Release/" /c /nologo
# ADD F90 /Ox /I "Release/" /c /nologo
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "\midas\include" /I "\midas\zlib" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 wsock32.lib winmm.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"libc.lib" /out:"\midas\nt\bin\analyzer.exe" /libpath:"\cern\lib"

!ELSEIF  "$(CFG)" == "analyzer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\debug"
# PROP Intermediate_Dir ".\debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE F90 /Zi /I "Debug/" /c /nologo
# ADD F90 /FR /Zi /I "Debug/" /c /nologo
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /ML /W3 /Gm /GX /ZI /Od /I "\midas\include" /I "\midas\zlib" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 wsock32.lib winmm.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"\midas\nt\bin\analyzer.exe" /libpath:"\cern\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "analyzer - Win32 Release"
# Name "analyzer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=\Midas\EXAMPLES\experiment\adccalib.c
# End Source File
# Begin Source File

SOURCE=\Midas\EXAMPLES\experiment\adcsum.c
# End Source File
# Begin Source File

SOURCE=\Midas\EXAMPLES\experiment\analyzer.c
# End Source File
# Begin Source File

SOURCE=..\src\ftplib.c
# End Source File
# Begin Source File

SOURCE=\Midas\src\mana.c
# End Source File
# Begin Source File

SOURCE=\Midas\SRC\midas.c
# End Source File
# Begin Source File

SOURCE=\Midas\SRC\MRPC.C
# End Source File
# Begin Source File

SOURCE=\Midas\SRC\ODB.C
# End Source File
# Begin Source File

SOURCE=\Midas\EXAMPLES\experiment\scaler.c
# End Source File
# Begin Source File

SOURCE=\Midas\SRC\SYSTEM.C
# End Source File
# Begin Source File

SOURCE=\Midas\SRC\ybos.c
# End Source File
# Begin Source File

SOURCE=\cern\lib\packlib.lib
# End Source File
# Begin Source File

SOURCE=.\LIB\zlib.lib
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\EXAMPLES\experiment\analyzer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
