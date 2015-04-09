###############################################################################
C.A.E.N. S.p.A.  -  Viareggio
###############################################################################

In this folder you find the necessary file to upgrade a V2718 VME Bridge that communicate with a CONET 2 Optical Controller.

An Application Note "CONET1 to CONET2 Migration" is available on the website.

Please read the Application Note before doing any upgrade.


This File contain two firmwares in .rbf format:

1) V1718VUB_REV2.14.RBF: this is the Firmware for the Mother Board of the V2718. It's in common with the V1718 Bridge hence the name is the same.

2) a2719ci_rev1.5.rbf: This is the firmware for the Mezzanine Card of the V2718. This define the Communication Protocol used, CONET 2 in this case.

CAEN Bridge Firmware Upgrade is executed with the CAENBridgeUpgrade software available for free in the Software section of the web site.

###############################################################################

ATTENTION: CONET 2 Bridge communicate only with CONET 2 Optical Controller (A2818 with firmware upgrade and A3818). A3818 communicate only through CONET 2 Communication Protocol and so there's no need of firmware upgrade.

