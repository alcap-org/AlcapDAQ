#!/bin/bash

DAQdir=$(dirname ${BASH_SOURCE[0]} | xargs readlink -e)
hostname=`hostname`
domainname=`hostname -d`
if [ -f $DAQdir/root-install/bin/thisroot.sh ] ;then
    . $DAQdir/root-install/bin/thisroot.sh
elif [ "$domainname" == "bu.edu" ]; then
#    echo "On BU machine...adding ROOT with the module command"
    module add root
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
export LD_LIBRARY_PATH=$MIDASSYS/linux$arch/lib:$DAQdir/compress:$LD_LIBRARY_PATH:$DAQdir/CAEN/lib:$DAQdir/analyzer/rootana

if [ "$USER" == "daq" ]; then
    export MIDAS_EXPTAB=$DAQdir/exptab
    export LD_LIBRARY_PATH=$DAQdir/CAEN/lib:$LD_LIBRARY_PATH
else
    export MIDAS_DIR=$DAQdir/analyzer/work/$USER
fi 

# Set the environment variables for SQLite
export PATH=$DAQdir/sqlite-install/bin:$PATH
export LD_LIBRARY_PATH=$DAQdir/sqlite-install/lib:$LD_LIBRARY_PATH
