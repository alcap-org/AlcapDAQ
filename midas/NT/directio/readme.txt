Usage of the directio driver
============================

Under Windows NT it is normally not possible for an application
to access IO ports directly. With a trick described in Dr. Dobb's
Journal 5/96 and extended in C't 1/97 it becomes possible very 
easily. Follow these steps:

1) copy DirectIO.sys to the Windows NT driver directory, usually
   \winnt\system32\drivers.

2) install the directio system driver with

   instdrv DirectIO \winnt\system32\drivers\directio.sys

   If you open the "drivers" applet in the control pad of Windows
   NT, you should see the directio driver being started. You can
   change the startup behaviour with the "startup..." button
   to automatic if you want this driver to be started automatically
   when Windows NT is booted. Note that you need administrator
   priviledges to execute these steps.

3) Run the tstio program to see if it's working. tstio accesses
   the PC speaker directly and plays some sound.

4) To write a byte to port 0x300 in your own application, issue the
   commands   

     ss_directio_init();
     ss_directio_give_port(0x300, 0x300);
     ss_directio_exit();

   at the initialization of the application. This will grant
   direct IO access to this application. Then you can use

     _outp(0x300, data);

   to write data to port 0x300. Be aware that writing
   to the wrong IO addresses can crash Windows NT, so be 
   very careful.

For further information, read the above mentioned article in
Dr. Dobb's Journal or have a look at the sourcecode in
drdobbs.zip. The source code of the modified driver from the
C't magazine is in directio.cpp, the makefile in m.bat (you
need the DDK to recompile the driver).


S. Ritt, Nov. 96
