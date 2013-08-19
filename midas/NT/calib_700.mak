# Microsoft Developer Studio Generated NMAKE File, Based on calib_700.dsp
!IF "$(CFG)" == ""
CFG=calib_700 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to calib_700 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "calib_700 - Win32 Release" && "$(CFG)" != "calib_700 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "calib_700.mak" CFG="calib_700 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "calib_700 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "calib_700 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "calib_700 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : ".\bin\calib_700.exe"


CLEAN :
	-@erase "$(INTDIR)\calib_700.obj"
	-@erase "$(INTDIR)\mscb.obj"
	-@erase "$(INTDIR)\rpc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\bin\calib_700.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\calib_700.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\calib_700.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=wsock32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\calib_700.pdb" /machine:I386 /out:"c:\midas\nt\bin\calib_700.exe" 
LINK32_OBJS= \
	"$(INTDIR)\calib_700.obj" \
	"$(INTDIR)\mscb.obj" \
	"$(INTDIR)\rpc.obj"

".\bin\calib_700.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "calib_700 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : ".\bin\calib_700.exe"


CLEAN :
	-@erase "$(INTDIR)\calib_700.obj"
	-@erase "$(INTDIR)\mscb.obj"
	-@erase "$(INTDIR)\rpc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\calib_700.pdb"
	-@erase ".\bin\calib_700.exe"
	-@erase ".\bin\calib_700.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\calib_700.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\calib_700.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=wsock32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\calib_700.pdb" /debug /machine:I386 /out:"c:\midas\nt\bin\calib_700.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\calib_700.obj" \
	"$(INTDIR)\mscb.obj" \
	"$(INTDIR)\rpc.obj"

".\bin\calib_700.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("calib_700.dep")
!INCLUDE "calib_700.dep"
!ELSE 
!MESSAGE Warning: cannot find "calib_700.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "calib_700 - Win32 Release" || "$(CFG)" == "calib_700 - Win32 Debug"
SOURCE=..\mscb\calib_700.c

"$(INTDIR)\calib_700.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\mscb\mscb.c

"$(INTDIR)\mscb.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\mscb\rpc.c

"$(INTDIR)\rpc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

