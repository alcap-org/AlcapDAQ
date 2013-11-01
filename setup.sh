#!/bin/bash
#. $PWD/root/bin/thisroot.sh
if [ ! -d "./dir" ]; then 
	mkdir dir
fi

export MIDAS_DIR=$PWD/dir
export MIDASSYS=$PWD/midas
export MIDAS_HOME=$MIDASSYS
export PATH=$MIDASSYS/linux/bin:$PATH
export LD_LIBRARY_PATH=$MIDASSYS/linux/lib:$PWD/compress:$LD_LIBRARY_PATH

