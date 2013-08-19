Sample mini MIDAS experiment
============================

This directory contains an example of a MIDAS experiment. It
contains an even definition for a trigger event (ID 1) with
eight ADC and TDC channels and a scaler event (ID 2) also with
eight channels.

The frontend program generates both types of events and fills
them with data. The data is stored in the events in MIDAS bank 
format which is very similar to BOS/YBOS banks.

The analyzer uses a single user routine to recover these banks
from the events and analyzes them, then fills them into 
histograms and N-tuples.

Running the sample experiment
=============================

To run the dummy experiment (no hardware access), compile the
frontend and analyzer with "make". Alternatively, compile the
frontend-analyzer-logger progam with "make fal".

The frontend can be started on a computer which has direct access 
to the experiment hardware. Most likely this is a VxWorks machine
or a PC running Windows NT or Linux. Alternatively, the frontend,
the analyzer and the logger can run in a single process on the
same computer. Under WindowsNT, you first have to install the
directio driver (see midas/nt/directio), under Linux you can use
the "dio" program to start the frontend with "dio frontend".

To test the experiment, do the following:

1) Start "frontend", "analyzer" and "mlogger", or start "fal".

2) Check the data directory in the ODB under /logger/data dir:

> odbedit
[local]/> cd /logger
[lodal]/> ls

You can change it with:

[local]/Logger> set "data dir" /your/data/dir

3) Check the logging channel under /logger/channels/0/Settings. 
If tape writing is desired, set the logging channel type to 
"tape" and the filename to something like /dev/nst0 under Linux or
\\.\tape0 under Windows NT. When writing to disk, set the filename
to run%05d.mid, which gererates names of the type runxxxxx.mid with
xxxxx the current run number automatically. Additional logging 
channels can be defined by duplicating the channel settings and
changing them:

[local]/Logger> cd /Logger/Channels
[local]/Logger/Channels> copy 0 1
[local]/Logger/Channels> cd 1/Settings

4) Start/stop runs with the ODBEdit "start"/"stop" commands. Note
that a run is started and data written to the data directory at
a rate of a few MB per second.

5) You can inspect online histograms and N-tuples with PAW
running on the backend computer:

  PAW> global_s onln
  PAW> ldir
  ...

To turn N-tuples on, set /Analyzer/Bank switches to one for those
banks which should be conainted in N-tuple #1. You can then see
the results with

  PAW> nt/print 1

and display variables with cuts:

  PAW> nt/pl 1.adc00 adc00>10.and.tdc00<100

6) Check if run00001.mid etc. is created when logging to disk
is turned on.

7) You can offline-analyze the .mid files with the same
analyzer executable:

  analyzer -i run00001.mid -o run00001.rz

to produce directly a N-tuple file which can be loaded inside
PAW with

  PAW> hi/file 1 run00001.rz 8190

Modifying the sample experiment
===============================

To adapt the sample experiment to a real experiment, following
tasks have to be performed:

1) A real CAMAC driver has to be linked to the frontend program.
The dummy experiment just uses camacnul.c which is a "null" driver
which doe actually noting and is only for testing. Different CAMAC
drivers can be found under midas/drivers. Common examples are
hyt1331.c or kcs2926.c. Copy thos drivers to your experiment 
directory and change the Makefile to use those instead of camacnul.c

1) Modify the frontend readout code in frontend.c according to
the experiment needs (different slot address, different modules).

2) Modify the event definition. If you want to send new banks 
of variable size from the frontend, create them in the frontend 
readout code and define them in the bank list in analyzer.c. 

3) Modify the analyzer to suit your needs in analyzer.c. Add new
calculations like pedestal correction and create new histos. 

4) Recompile the components with "make" or "make fal". 



