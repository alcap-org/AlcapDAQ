#!/bin/bash

export PATH
unset USERNAME

#export MIDASSYS=/home/mulan/DAQ/midas-1.9.5
export MIDASSYS=/home/data/daq/midas
export MIDAS_ROOT=$MIDASSYS
#export ROOTSYS=/home/mulan/DAQ/root
#export ROOTSYS=/home/data/daq/root
#export PATH=$MIDASSYS/linux/bin:$MIDASSYS/utils:$ROOTSYS/bin:$PATH
export ROOTSYS=/opt/root/5.16.00s
export PATH=$MIDASSYS/linux/bin:$MIDASSYS/utils:$ROOTSYS/bin:$PATH
export SHLIB_PATH=$SHLIB_PATH:$ROOTSYS/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTSYS/lib
export USE_MULAN_TOOLS=false

pngfile="png/$macro.png";
if [ -f $pngfile ]; then
    rm $pngfile
fi 

htmlfile="html/$macro.html";
if [ -f $htmlfile ]; then
    rm $htmlfile
fi 

cmd="root -l -q -b"
