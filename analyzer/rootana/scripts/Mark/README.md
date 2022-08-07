```console
Server mu2e.bu.edu
export R15b_DATA=/home/m-wong/data/R15b
export R15b_TM=/home/m-wong/data/transfer
export R15b_OUT=/home/m-wong/data/out
MC /data/hdd4/R15bMC/
Code /home/m-wong/AlcapDAQ/analyzer/rootana/scripts/Al100/ or https://github.com/alcap-org/AlcapDAQ/tree/MW_R15b/analyzer/rootana/scripts/Mark
Tested with al50, al100 (ti50, si16b, mt soon)

Al100 note: A (2a1+a2)>350 amplitude cut was used when preparing the TMETree (PlotTAP_TDiff).

            +--------+         +----------+       +-----------+    +-----------+
            |TMETrees+-------->+Processing+------>+GeGainDrift+--->+finetune.db+---+
            +--------+         +----+-----+       +-----------+    +-----------+   |     +----------+
                                    |                                              +---->+          |
                                    |             +---+            +-----+               |Processing|
                                    +------------>+PID+----------->+TCutG+-------------->+          |
                                                  +---+            +-----+               +-----+----+
    +---------------+-----------------+--------------------------+----------------+--------+   |
    |               |                 |            |             |                |            |
    |               |                 |            |             |                |            |
    |               |                 |            |             |                |            |
    v               v                 v            v             v                v            v
+---+----+   +------+--------+   +----+---+      +----+       +--+---+       +----+----+  +----+------+
|Lifetime|   |TimeCoincidence|   |Xray2p1s|      |EvdE|       |Folded|       |RooUnfold|  |Systematics|
+--------+   +---------------+   +--------+      +----+       +------+       +----+----+  +-----------+
                                                                                  |
                                                                                  v
                                                                             +----+-------+
                                                                             |DrawUnfolded|
                                                                             +------------+


Transfer matrix

$ cd /home/m-wong/g4sim
$ . env.sh
$ cd alcap
$ root -l 
> . L scripts/R15b_TM.C
> R15b_TM("/data/hdd4/protons/sf1.02.al50.proton.root", "proton")
Processing: proton 938.27 MeV
0 / 10000000

Processing

$ cd Parallel/
$ root -b runProof.C\(\"al50\"\)
   ------------------------------------------------------------
  | Welcome to ROOT 6.16/00                  https://root.cern |
  |                               (c) 1995-2018, The ROOT Team |
  | Built for linuxx8664gcc on Jan 23 2019, 09:06:13           |
  | From tags/v6-16-00@v6-16-00                                |
  | Try '.help', '.demo', '.license', '.credits', '.quit'/'.q' |
   ------------------------------------------------------------

root [0]
Processing runProof.C("al50")...
+++ Starting PROOF-Lite with 12 workers +++
Opening connections to workers: OK (12 workers)                
Setting up worker servers: OK (12 workers)                
PROOF set to parallel mode (12 workers)

Info in <TProofLite::SetQueryRunning>: starting query: 1
Info in <TProofQueryResult::SetRunning>: nwrks: 12
Looking up for exact location of files: OK (220 files)                
Looking up for exact location of files: OK (220 files)                
Info in <TPacketizer::TPacketizer>: Initial number of workers: 12
Validating files: OK (220 files)                
[TProof::Progress] Total 466124857 eventsorkers |====================| 100.00 % [907632.9 evts/s, 89.0 MB/s, time left: 0.0 s]]
Query processing time: 513.6 s
Lite-0: merging output objects ... / (1 workers still sending)  
Output file: al50.root
Lite-0: all output objects have been merged

Load environment variables

$ . env.sh

GeGainShift

$ cd GeGainShift
$ root -l GeGainShift.C\(\"al50\"\)
$ sqlite3 finetune.db
> .read finetune.sql

Xray2p1s

$ cd xray
$ root -l Xray2p1s.C

Pid

$ cd PID
$ root -l Pid.C //al50
$ root -l Pid100.C //al100

DrawEvdE

$ cd Plotters
$ root -l DrawEvdE.C

Lifetime

$ cd Lifetime
[] root -l 
[] .L Lifetime.C
[] Lifetime("al50", "proton", "SiR")

TimeCoincidence

$ cd Plotters
$ root -l DrawTimeCoincidence.C

DrawFolded

$ cd Plotters
$ root -l DrawFolded.C

RooUnfoldAlCap

$ cd RooUnfold
$ make //if not done yet
$ . env.sh
$ root -l
[] .L RooUnfoldAlCap.C
[] RooUnfoldAlCap("al50", "proton", kTRUE)

DrawUnfolded

$ cd Plotters
$ root -l DrawUnfolded.C

Systematics

$ cd RooUnfold
$ root -l
$ .L Systematics.C
$ Unfolding("SiR", "proton")

Comparison with TWIST

$ cd Plotters
$ root -l DrawComparisonWithTwist.C

Appendix

Tree structure

******************************************************************************
*Tree    :tree      : tree                                                   *
*Entries :  9617472 : Total =      1525037846 bytes  File  Size =  393346582 *
*        :          : Tree compression factor =   3.88                       *
******************************************************************************
*Br    0 :timeToPrevTME : timeToPrevTME/D                                    *
*Entries :  9617472 : Total  Size=   77162021 bytes  File Size  =   34102528 *
*Baskets :     2219 : Basket Size=    4367360 bytes  Compression=   2.26     *
*............................................................................*
*Br    1 :timeToNextTME : timeToNextTME/D                                    *
*Entries :  9617472 : Total  Size=   77162021 bytes  File Size  =   34112731 *
*Baskets :     2219 : Basket Size=    4367360 bytes  Compression=   2.26     *
*............................................................................*
*Br    2 :runId     : runId/I                                                *
*Entries :  9617472 : Total  Size=   38572865 bytes  File Size  =     310368 *
*Baskets :     1116 : Basket Size=    2183168 bytes  Compression= 124.21     *
*............................................................................*
*Br    3 :blockId   : blockId/I                                              *
*Entries :  9617472 : Total  Size=   38575105 bytes  File Size  =    3100593 *
*Baskets :     1116 : Basket Size=    2183168 bytes  Compression=  12.43     *
*............................................................................*
*Br    4 :tmeId     : tmeId/I                                                *
*Entries :  9617472 : Total  Size=   38572865 bytes  File Size  =   10386305 *
*Baskets :     1116 : Basket Size=    2183168 bytes  Compression=   3.71     *
*............................................................................*
*Br    5 :channel   : TString                                                *
*Entries :  9617472 : Total  Size=  120841352 bytes  File Size  =   25599106 *
*Baskets :     4559 : Basket Size=   25600000 bytes  Compression=   4.72     *
*............................................................................*
*Br    6 :tpi_id1   : tpi_id1/I                                              *
*Entries :  9617472 : Total  Size=   38575105 bytes  File Size  =    7923710 *
*Baskets :     1116 : Basket Size=    2183168 bytes  Compression=   4.87     *
*............................................................................*
*Br    7 :tpi_id2   : tpi_id2/I                                              *
*Entries :  9617472 : Total  Size=   38575105 bytes  File Size  =    4268451 *
*Baskets :     1116 : Basket Size=    2183168 bytes  Compression=   9.03     *
*............................................................................*
*Br    8 :tpi_id3   : tpi_id3/I                                              *
*Entries :  9617472 : Total  Size=   38575105 bytes  File Size  =     289267 *
*Baskets :     1116 : Basket Size=    2183168 bytes  Compression= 133.28     *
*............................................................................*
*Br    9 :a1        : a1/I                                                   *
*Entries :  9617472 : Total  Size=   38569416 bytes  File Size  =   19785084 *
*Baskets :     1115 : Basket Size=    2183168 bytes  Compression=   1.95     *
*............................................................................*
*Br   10 :a2        : a2/I                                                   *
*Entries :  9617472 : Total  Size=   38569416 bytes  File Size  =    3834747 *
*Baskets :     1115 : Basket Size=    2183168 bytes  Compression=  10.05     *
*............................................................................*
*Br   11 :a3        : a3/I                                                   *
*Entries :  9617472 : Total  Size=   38569416 bytes  File Size  =     308412 *
*Baskets :     1115 : Basket Size=    2183168 bytes  Compression= 124.98     *
*............................................................................*
*Br   12 :e1        : e1/D                                                   *
*Entries :  9617472 : Total  Size=   77137212 bytes  File Size  =   48989368 *
*Baskets :     2215 : Basket Size=    4365824 bytes  Compression=   1.57     *
*............................................................................*
*Br   13 :e2        : e2/D                                                   *
*Entries :  9617472 : Total  Size=   77137212 bytes  File Size  =    5335888 *
*Baskets :     2215 : Basket Size=    4365824 bytes  Compression=  14.45     *
*............................................................................*
*Br   14 :e3        : e3/D                                                   *
*Entries :  9617472 : Total  Size=   77137212 bytes  File Size  =     670669 *
*Baskets :     2215 : Basket Size=    4365824 bytes  Compression= 114.95     *
*............................................................................*
*Br   15 :t1        : t1/D                                                   *
*Entries :  9617472 : Total  Size=   77137212 bytes  File Size  =   58181673 *
*Baskets :     2215 : Basket Size=    4365824 bytes  Compression=   1.33     *
*............................................................................*
*Br   16 :t2        : t2/D                                                   *
*Entries :  9617472 : Total  Size=   77137212 bytes  File Size  =    8849073 *
*Baskets :     2215 : Basket Size=    4365824 bytes  Compression=   8.71     *
*............................................................................*
*Br   17 :t3        : t3/D                                                   *
*Entries :  9617472 : Total  Size=   77137212 bytes  File Size  =     675973 *
*Baskets :     2215 : Basket Size=    4365824 bytes  Compression= 114.05     *
*............................................................................*
*Br   18 :sig1      : TString                                                *
*Entries :  9617472 : Total  Size=   48661831 bytes  File Size  =   14137745 *
*Baskets :     2500 : Basket Size=    8689152 bytes  Compression=   3.44     *
*............................................................................*
*Br   19 :sig2      : TString                                                *
*Entries :  9617472 : Total  Size=   48807703 bytes  File Size  =   14248308 *
*Baskets :     2508 : Basket Size=    8697344 bytes  Compression=   3.42     *
*............................................................................*
*Br   20 :sig3      : TString                                                *
*Entries :  9617472 : Total  Size=   48859825 bytes  File Size  =   14287627 *
*Baskets :     2510 : Basket Size=    8700416 bytes  Compression=   3.42     *
*............................................................................*
*Br   21 :sig4      : TString                                                *
*Entries :  9617472 : Total  Size=   48902850 bytes  File Size  =   14315622 *
*Baskets :     2510 : Basket Size=    8702976 bytes  Compression=   3.41     *
*............................................................................*
*Br   22 :pt1       : TString                                                *
*Entries :  9617472 : Total  Size=   48331779 bytes  File Size  =   13811428 *
*Baskets :     2491 : Basket Size=    8670208 bytes  Compression=   3.50     *
*............................................................................*
*Br   23 :pt2       : TString                                                *
*Entries :  9617472 : Total  Size=   48331779 bytes  File Size  =   13811428 *
*Baskets :     2491 : Basket Size=    8670208 bytes  Compression=   3.50     *
*............................................................................*
*Br   24 :pt3       : TString                                                *
*Entries :  9617472 : Total  Size=   48331779 bytes  File Size  =   13811428 *
*Baskets :     2491 : Basket Size=    8670208 bytes  Compression=   3.50     *
*............................................................................*
*Br   25 :pt4       : TString                                                *
*Entries :  9617472 : Total  Size=   48331779 bytes  File Size  =   13811428 *
*Baskets :     2491 : Basket Size=    8670208 bytes  Compression=   3.50     *
*............................................................................*
*Br   26 :quality   : TString                                                *
*Entries :  9617472 : Total  Size=   51334379 bytes  File Size  =   13898846 *
*Baskets :     2585 : Basket Size=    8918016 bytes  Compression=   3.69     *
*............................................................................*
```
