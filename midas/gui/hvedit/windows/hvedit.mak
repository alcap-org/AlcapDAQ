# Microsoft Developer Studio Generated NMAKE File, Based on hvedit.dsp
!IF "$(CFG)" == ""
CFG=HVEDIT - Win32 Debug
!MESSAGE No configuration specified. Defaulting to HVEDIT - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "HVEDIT - Win32 Debug" && "$(CFG)" != "HVEDIT - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "HVEDIT - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\nt\bin\hvedit.exe" "$(OUTDIR)\hvedit.pch" "$(OUTDIR)\hvedit.bsc"


CLEAN :
	-@erase "$(INTDIR)\HVABOUT.OBJ"
	-@erase "$(INTDIR)\HVABOUT.SBR"
	-@erase "$(INTDIR)\HVCONFIG.OBJ"
	-@erase "$(INTDIR)\HVCONFIG.SBR"
	-@erase "$(INTDIR)\HVEDIDLG.OBJ"
	-@erase "$(INTDIR)\HVEDIDLG.SBR"
	-@erase "$(INTDIR)\HVEDIT.OBJ"
	-@erase "$(INTDIR)\hvedit.pch"
	-@erase "$(INTDIR)\HVEDIT.res"
	-@erase "$(INTDIR)\HVEDIT.SBR"
	-@erase "$(INTDIR)\HVVIEW.OBJ"
	-@erase "$(INTDIR)\HVVIEW.SBR"
	-@erase "$(INTDIR)\MExperiment.obj"
	-@erase "$(INTDIR)\MExperiment.sbr"
	-@erase "$(INTDIR)\MHostDlg.obj"
	-@erase "$(INTDIR)\MHostDlg.sbr"
	-@erase "$(INTDIR)\MKey.obj"
	-@erase "$(INTDIR)\MKey.sbr"
	-@erase "$(INTDIR)\MPasswdDlg.obj"
	-@erase "$(INTDIR)\MPasswdDlg.sbr"
	-@erase "$(INTDIR)\PrintDlg.obj"
	-@erase "$(INTDIR)\PrintDlg.sbr"
	-@erase "$(INTDIR)\STDAFX.OBJ"
	-@erase "$(INTDIR)\STDAFX.SBR"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\hvedit.bsc"
	-@erase "$(OUTDIR)\hvedit.pdb"
	-@erase "..\..\..\nt\bin\hvedit.exe"
	-@erase "..\..\..\nt\bin\hvedit.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=fl32.exe
CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "\midas\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\hvedit.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
MTL_PROJ=/mktyplib203 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\HVEDIT.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hvedit.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\HVABOUT.SBR" \
	"$(INTDIR)\HVCONFIG.SBR" \
	"$(INTDIR)\HVEDIDLG.SBR" \
	"$(INTDIR)\HVEDIT.SBR" \
	"$(INTDIR)\HVVIEW.SBR" \
	"$(INTDIR)\MExperiment.sbr" \
	"$(INTDIR)\MHostDlg.sbr" \
	"$(INTDIR)\MKey.sbr" \
	"$(INTDIR)\MPasswdDlg.sbr" \
	"$(INTDIR)\PrintDlg.sbr" \
	"$(INTDIR)\STDAFX.SBR"

"$(OUTDIR)\hvedit.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=winmm.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\hvedit.pdb" /debug /machine:I386 /out:"\midas\nt\bin\hvedit.exe" 
LINK32_OBJS= \
	"$(INTDIR)\HVABOUT.OBJ" \
	"$(INTDIR)\HVCONFIG.OBJ" \
	"$(INTDIR)\HVEDIDLG.OBJ" \
	"$(INTDIR)\HVEDIT.OBJ" \
	"$(INTDIR)\HVVIEW.OBJ" \
	"$(INTDIR)\MExperiment.obj" \
	"$(INTDIR)\MHostDlg.obj" \
	"$(INTDIR)\MKey.obj" \
	"$(INTDIR)\MPasswdDlg.obj" \
	"$(INTDIR)\PrintDlg.obj" \
	"$(INTDIR)\STDAFX.OBJ" \
	"$(INTDIR)\HVEDIT.res" \
	"..\..\..\NT\LIB\midas.lib"

"..\..\..\nt\bin\hvedit.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "HVEDIT - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "..\..\..\nt\bin\hvedit.exe" "$(OUTDIR)\hvedit.pch" "$(OUTDIR)\hvedit.bsc"


CLEAN :
	-@erase "$(INTDIR)\HVABOUT.OBJ"
	-@erase "$(INTDIR)\HVABOUT.SBR"
	-@erase "$(INTDIR)\HVCONFIG.OBJ"
	-@erase "$(INTDIR)\HVCONFIG.SBR"
	-@erase "$(INTDIR)\HVEDIDLG.OBJ"
	-@erase "$(INTDIR)\HVEDIDLG.SBR"
	-@erase "$(INTDIR)\HVEDIT.OBJ"
	-@erase "$(INTDIR)\hvedit.pch"
	-@erase "$(INTDIR)\HVEDIT.res"
	-@erase "$(INTDIR)\HVEDIT.SBR"
	-@erase "$(INTDIR)\HVVIEW.OBJ"
	-@erase "$(INTDIR)\HVVIEW.SBR"
	-@erase "$(INTDIR)\MExperiment.obj"
	-@erase "$(INTDIR)\MExperiment.sbr"
	-@erase "$(INTDIR)\MHostDlg.obj"
	-@erase "$(INTDIR)\MHostDlg.sbr"
	-@erase "$(INTDIR)\MKey.obj"
	-@erase "$(INTDIR)\MKey.sbr"
	-@erase "$(INTDIR)\MPasswdDlg.obj"
	-@erase "$(INTDIR)\MPasswdDlg.sbr"
	-@erase "$(INTDIR)\PrintDlg.obj"
	-@erase "$(INTDIR)\PrintDlg.sbr"
	-@erase "$(INTDIR)\STDAFX.OBJ"
	-@erase "$(INTDIR)\STDAFX.SBR"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\hvedit.bsc"
	-@erase "..\..\..\nt\bin\hvedit.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=fl32.exe
CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "\midas\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\hvedit.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
MTL_PROJ=/mktyplib203 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\HVEDIT.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hvedit.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\HVABOUT.SBR" \
	"$(INTDIR)\HVCONFIG.SBR" \
	"$(INTDIR)\HVEDIDLG.SBR" \
	"$(INTDIR)\HVEDIT.SBR" \
	"$(INTDIR)\HVVIEW.SBR" \
	"$(INTDIR)\MExperiment.sbr" \
	"$(INTDIR)\MHostDlg.sbr" \
	"$(INTDIR)\MKey.sbr" \
	"$(INTDIR)\MPasswdDlg.sbr" \
	"$(INTDIR)\PrintDlg.sbr" \
	"$(INTDIR)\STDAFX.SBR"

"$(OUTDIR)\hvedit.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=winmm.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\hvedit.pdb" /machine:I386 /out:"\midas\nt\bin\hvedit.exe" 
LINK32_OBJS= \
	"$(INTDIR)\HVABOUT.OBJ" \
	"$(INTDIR)\HVCONFIG.OBJ" \
	"$(INTDIR)\HVEDIDLG.OBJ" \
	"$(INTDIR)\HVEDIT.OBJ" \
	"$(INTDIR)\HVVIEW.OBJ" \
	"$(INTDIR)\MExperiment.obj" \
	"$(INTDIR)\MHostDlg.obj" \
	"$(INTDIR)\MKey.obj" \
	"$(INTDIR)\MPasswdDlg.obj" \
	"$(INTDIR)\PrintDlg.obj" \
	"$(INTDIR)\STDAFX.OBJ" \
	"$(INTDIR)\HVEDIT.res" \
	"..\..\..\NT\LIB\midas.lib"

"..\..\..\nt\bin\hvedit.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("hvedit.dep")
!INCLUDE "hvedit.dep"
!ELSE 
!MESSAGE Warning: cannot find "hvedit.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "HVEDIT - Win32 Debug" || "$(CFG)" == "HVEDIT - Win32 Release"
SOURCE=.\HVABOUT.CPP

"$(INTDIR)\HVABOUT.OBJ"	"$(INTDIR)\HVABOUT.SBR" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HVCONFIG.CPP

"$(INTDIR)\HVCONFIG.OBJ"	"$(INTDIR)\HVCONFIG.SBR" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HVEDIDLG.CPP

"$(INTDIR)\HVEDIDLG.OBJ"	"$(INTDIR)\HVEDIDLG.SBR" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HVEDIT.CPP

"$(INTDIR)\HVEDIT.OBJ"	"$(INTDIR)\HVEDIT.SBR" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HVEDIT.RC

"$(INTDIR)\HVEDIT.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\HVVIEW.CPP

"$(INTDIR)\HVVIEW.OBJ"	"$(INTDIR)\HVVIEW.SBR" : $(SOURCE) "$(INTDIR)"


SOURCE=.\MExperiment.cpp

"$(INTDIR)\MExperiment.obj"	"$(INTDIR)\MExperiment.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\MHostDlg.cpp

"$(INTDIR)\MHostDlg.obj"	"$(INTDIR)\MHostDlg.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\MKey.cpp

"$(INTDIR)\MKey.obj"	"$(INTDIR)\MKey.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\MPasswdDlg.cpp

"$(INTDIR)\MPasswdDlg.obj"	"$(INTDIR)\MPasswdDlg.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\PrintDlg.cpp

"$(INTDIR)\PrintDlg.obj"	"$(INTDIR)\PrintDlg.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\STDAFX.CPP

!IF  "$(CFG)" == "HVEDIT - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "\midas\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\hvedit.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\STDAFX.OBJ"	"$(INTDIR)\STDAFX.SBR"	"$(INTDIR)\hvedit.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "HVEDIT - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "\midas\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\hvedit.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\STDAFX.OBJ"	"$(INTDIR)\STDAFX.SBR"	"$(INTDIR)\hvedit.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

