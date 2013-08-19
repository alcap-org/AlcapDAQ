# Microsoft Developer Studio Generated NMAKE File, Based on msc.dsp
!IF "$(CFG)" == ""
CFG=msc - Win32 Debug
!MESSAGE No configuration specified. Defaulting to msc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "msc - Win32 Release" && "$(CFG)" != "msc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "msc.mak" CFG="msc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "msc - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "msc - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "msc - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : ".\bin\msc.exe"


CLEAN :
	-@erase "$(INTDIR)\msc.obj"
	-@erase "$(INTDIR)\mscb.obj"
	-@erase "$(INTDIR)\rpc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\bin\msc.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\msc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\msc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=wsock32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\msc.pdb" /machine:I386 /out:"c:\midas\nt\bin\msc.exe" 
LINK32_OBJS= \
	"$(INTDIR)\msc.obj" \
	"$(INTDIR)\mscb.obj" \
	"$(INTDIR)\rpc.obj"

".\bin\msc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "msc - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : ".\bin\msc.exe" "$(OUTDIR)\msc.bsc"


CLEAN :
	-@erase "$(INTDIR)\msc.obj"
	-@erase "$(INTDIR)\msc.sbr"
	-@erase "$(INTDIR)\mscb.obj"
	-@erase "$(INTDIR)\mscb.sbr"
	-@erase "$(INTDIR)\rpc.obj"
	-@erase "$(INTDIR)\rpc.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\msc.bsc"
	-@erase "$(OUTDIR)\msc.pdb"
	-@erase ".\bin\msc.exe"
	-@erase ".\bin\msc.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\msc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\msc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\msc.sbr" \
	"$(INTDIR)\mscb.sbr" \
	"$(INTDIR)\rpc.sbr"

"$(OUTDIR)\msc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=wsock32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\msc.pdb" /debug /machine:I386 /out:"c:\midas\nt\bin\msc.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\msc.obj" \
	"$(INTDIR)\mscb.obj" \
	"$(INTDIR)\rpc.obj"

".\bin\msc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("msc.dep")
!INCLUDE "msc.dep"
!ELSE 
!MESSAGE Warning: cannot find "msc.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "msc - Win32 Release" || "$(CFG)" == "msc - Win32 Debug"
SOURCE=..\mscb\msc.c

!IF  "$(CFG)" == "msc - Win32 Release"


"$(INTDIR)\msc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msc - Win32 Debug"


"$(INTDIR)\msc.obj"	"$(INTDIR)\msc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\mscb\mscb.c

!IF  "$(CFG)" == "msc - Win32 Release"


"$(INTDIR)\mscb.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msc - Win32 Debug"


"$(INTDIR)\mscb.obj"	"$(INTDIR)\mscb.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\mscb\rpc.c

!IF  "$(CFG)" == "msc - Win32 Release"


"$(INTDIR)\rpc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msc - Win32 Debug"


"$(INTDIR)\rpc.obj"	"$(INTDIR)\rpc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

