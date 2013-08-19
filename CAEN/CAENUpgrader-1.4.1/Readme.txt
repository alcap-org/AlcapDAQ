------------------------------------------------------------------------------

                   --- CAEN SpA - Computing R&D Division --- 

 ------------------------------------------------------------------------------
 ------------------------------------------------------------------------------

    CAENUpgrader version 1.4.1 README 
    
    February 2013

 ------------------------------------------------------------------------------

TABLE OF CONTENTS
------------------

A.  DESCRIPTION
B.  SYSTEM REQUIREMENTS
C.  INSTALLATION
D.  SUPPORT


A. DESCRIPTION
---------------

The CAENUpgrader is the new firmware upgrade tool for the CAEN boards or bridges.
CAENUpgrader supersedes the old cvUpgrade, CAENBridgeUpgrade and PLLConfig tools,
by offering a unified graphical user interface with the same functionality of the
separate command line programs. 
The cvUpgrade and the CAENBridgeUpgrade are still provided as command line tools,
while the old PLLConfig software is no longer available.

  
B. SYSTEM REQUIREMENTS
-----------------------
   
  - Microsoft Windows 2000/Xp/Vista/7 or Linux
  - CAENComm v. 1.02 or later
  - CAENVmeLib v. 2.22 or later
  - Java JRE 6 or later
   

C. INSTALLATION
----------------

Windows:
	- run the installer and follow the on-screen wizard

Linux:
	- untar the package
	- cd into the CAENUpgrader directory
	- run: ./configure
	- run: make
	- run: sudo make install
	- once installed the GUI can be started by running: CAENUpgradeGUI


D. SUPPORT   
-----------
        
For additional support, please contact CAEN (e-mail support.computing@caen.it). 

  
