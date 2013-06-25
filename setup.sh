#!/bin/bash
. $PWD/root/bin/thisroot.sh
mkdir dir
export MIDAS_DIR=$PWD/dir
export MIDASSYS=$PWD/midas
export MIDAS_HOME=$MIDASSYS
export PATH=$MIDASSYS/linux/bin:$PATH
export LD_LIBRARY_PATH=$MIDASSYS/linux/lib:$ROOTSYS/lib:$PWD/compress:$LD_LIBRARY_PATH

