# Microsoft Developer Studio Generated NMAKE File, Based on mscb.dsp
!IF "$(CFG)" == ""
CFG=mscb - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mscb - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mscb - Win32 Release" && "$(CFG)" != "mscb - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mscb.mak" CFG="mscb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mscb - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mscb - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "mscb - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : ".\lib\mscb.dll"


CLEAN :
	-@erase "$(INTDIR)\mscb.obj"
	-@erase "$(INTDIR)\rpc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\mscb.exp"
	-@erase "$(OUTDIR)\mscb.lib"
	-@erase ".\lib\mscb.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MSCB_EXPORTS" /Fp"$(INTDIR)\mscb.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mscb.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=wsock32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\mscb.pdb" /machine:I386 /out:"\midas\nt\lib\mscb.dll" /implib:"$(OUTDIR)\mscb.lib" 
LINK32_OBJS= \
	"$(INTDIR)\mscb.obj" \
	"$(INTDIR)\rpc.obj"

".\lib\mscb.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mscb - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : ".\lib\mscb.dll"


CLEAN :
	-@erase "$(INTDIR)\mscb.obj"
	-@erase "$(INTDIR)\rpc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\mscb.exp"
	-@erase "$(OUTDIR)\mscb.lib"
	-@erase "$(OUTDIR)\mscb.pdb"
	-@erase ".\lib\mscb.dll"
	-@erase ".\lib\mscb.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MSCB_EXPORTS" /Fp"$(INTDIR)\mscb.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mscb.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=wsock32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\mscb.pdb" /debug /machine:I386 /out:"\midas\nt\lib\mscb.dll" /implib:"$(OUTDIR)\mscb.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\mscb.obj" \
	"$(INTDIR)\rpc.obj"

".\lib\mscb.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("mscb.dep")
!INCLUDE "mscb.dep"
!ELSE 
!MESSAGE Warning: cannot find "mscb.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mscb - Win32 Release" || "$(CFG)" == "mscb - Win32 Debug"
SOURCE=..\mscb\mscb.c

"$(INTDIR)\mscb.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\mscb\rpc.c

"$(INTDIR)\rpc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

