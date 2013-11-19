#!/bin/bash
unset MIDAS_DIR
unset MIDASSYS
unset MIDAS_HOME
unset MIDAS_EXPTAB

mkdir -p dir

cd midas && make && cd -

#export MIDAS_DIR=$PWD/dir
if [[ ! -f exptab ]]; then
	echo "Alcap $PWD/dir $(whoami)" > exptab
fi
export MIDAS_EXPTAB=$PWD/exptab
export MIDASSYS=$PWD/midas
export MIDAS_HOME=$MIDASSYS

uname=`uname -p`
arch=''
if [ "$uname" == "x86_64" ]; then
    arch='64'
fi

export PATH=$MIDASSYS/linux$arch/bin:$PATH
export LD_LIBRARY_PATH=$MIDASSYS/linux$arch/lib:$PWD/compress:$LD_LIBRARY_PATH

#cd compress && make && cd -

cd CAEN && mkdir -p lib && ./link.sh && cd -

for crate in $(ls -d crate*);
do
	echo "****************************************************"
	echo "Making "$crate
	cd $crate && make && cd -
	echo "Done "$crate"!"
done
