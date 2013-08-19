del DirectIO.sys
cl -Fa -nologo -DWINVER=0x0400 -I\ddk\inc -I\ddk\inc\ddk -D_X86_=1 -Di386=1 -DSTD_CALL -DCONDITION_HANDLING=1 -DWIN32_LEAN_AND_MEAN=1 -DNT_UP=1 -DNT_INST=0 -DWIN32=100 -D_NT1X_=100 -DNT_UP=1 -Dtry=__try -Dleave=__leave -Dexcept=__except -Dfinally=__finally -D_CRTAPI1=__cdecl -D_CRTAPI2=__cdecl -Ditoa=_itoa -Dstrcmpi=_strcmpi -Dstricmp=_stricmp -Dwcsicmp=_wcsicmp -Dwcsnicmp=_wcsnicmp -DDBG=0 -DDEVL=1 -DFPO=1 -D_IDWBUILD /c /Zel /Zp8 /W3 /G5zyf /Oxsy  directio.cpp
rc -I\ddk\inc directio.rc
@rem Debugging-Symbole: -debug:notmapped,MINIMAL -debugtype:coff
link -out:DirectIO.sys -machine:i386 -MERGE:_PAGE=PAGE -MERGE:_TEXT=.text -SECTION:INIT,d -OPT:REF -RELEASE -INCREMENTAL:NO -FULLBUILD -FORCE:MULTIPLE -IGNORE:4001,4037,4039,4065,4070,4078,4087,4089 -PDB:NONE -version:1.0 -osversion:4.00 -MERGE:.rdata=.text -align:0x20 -driver -subsystem:native,4.00 -base:0x10000 -entry:DriverEntry@8 directio.obj directio.res \ddk\libfre\i386\ntoskrnl.lib \ddk\libfre\i386\hal.lib \ddk\libfre\i386\int64.lib
copy DirectIO.sys %SystemRoot%\system32\drivers
rem instdrv DirectIO %SystemRoot%\system32\drivers\DirectIO.sys
