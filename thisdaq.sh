#!/bin/bash

DAQdir=$(dirname ${BASH_SOURCE[0]} | xargs readlink -e)

if [ -f $DAQdir/root-install/bin/thisroot.sh ] ;then
	. $DAQdir/root-install/bin/thisroot.sh
else
	echo "Check you've got root set up properly on this machine"
fi

uname=`uname -m`
arch=''
if [ "$uname" == "x86_64" ]; then
    arch='64'
fi

export DAQdir
export MIDASSYS=$DAQdir/midas
export MIDAS_HOME=$MIDASSYS
export PATH=$MIDASSYS/linux$arch/bin:$DAQdir/online-display:$PATH
export LD_LIBRARY_PATH=$MIDASSYS/linux$arch/lib:$DAQdir/compress:$LD_LIBRARY_PATH:$DAQdir/CAEN/lib

if [ "$USER" == "daq" ]; then
    export LD_LIBRARY_PATH=$DAQdir/CAEN/lib:$LD_LIBRARY_PATH
else
    export MIDAS_DIR=$DAQdir/analyzer/work/$USER
fi 
