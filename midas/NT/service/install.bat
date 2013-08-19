@echo off
copy srvany.exe %windir%\system32
instsrv Midas %windir%\system32\srvany.exe
instsrv MHTTPD %windir%\system32\srvany.exe
regini midas.ini
echo .
echo ----------------------------------------------------
echo .
echo Please start the "Services" applet in Control Panel,
echo select the "Midas" service and press "Startup".
echo Click "This Account" under "Log On As" and enter
echo the user name and password of your account. 
echo Press "OK", then press the "Start" button.
 