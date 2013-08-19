@echo off
rem Usage: build <version> like "build 161"
rem Build MIDAS version

echo Have you tagged the release? (cvs tag release-x-x-x) 
echo If not, enter Ctrl-Break now and do so!

sleep 10

rem go to midas root
cd ..

set version=%1
set dir=\tmp\midas%version%
set archive=\tmp\midas%version%.zip

rem create temporary directory
rmdir /S /Q %dir% > nul
mkdir \tmp > nul

rem export files
echo Getting standard files...
cd ..
cvs checkout -d %dir% midas
cd midas

echo on

rem merging NT files
xcopy /s msdos %dir%\msdos\ > nul
xcopy /s vms %dir%\vms\ > nul

rem merging short course
xcopy /s doc\course\*.* %dir%\doc\course\
del %dir%\doc\course\*.ppt

copy COPYING %dir% > nul
copy install.bat %dir% > nul

copy doc\readme.txt %dir% > nul
copy doc\install_nt.txt %dir%\install.txt > nul
copy doc\overview.txt %dir% > nul


mkdir %dir%\nt\hvedit
xcopy nt\hvedit\*.* %dir%\nt\hvedit\ > nul
mkdir %dir%\nt\hvedit\hlp
xcopy nt\hvedit\hlp\*.* %dir%\nt\hvedit\hlp\ > nul
mkdir %dir%\nt\hvedit\res
xcopy nt\hvedit\res\*.* %dir%\nt\hvedit\res\ > nul

echo Creating archive...
cd \tmp
zip -r midas.zip midas%version%
rem \program files\winzip\wzsepe32 midas.zip
copy /b c:\cygwin\bin\unzipsfx.exe+midas.zip midas.exe
zip -A midas.exe

cd \midas
rm midas%version%.zip > nul
rm midas%version%.exe > nul
move \tmp\midas.zip midas%version%.zip > nul
move \tmp\midas.exe midas%version%.exe > nul
rmdir /S /Q %dir% > nul

echo Sending archive to midas.psi.ch

scp midas%version%.exe midas@midas:html/download/windows/
scp midas%version%.zip midas@midas:html/download/windows/

rm midas%version%.exe > nul
