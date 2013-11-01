#!/bin/bash
#. $PWD/root/bin/thisroot.sh
expname=Alcap

if [ ! -d "./dir" ]; then 
	mkdir dir
fi

cd midas && make && cd -

export MIDASSYS=$PWD/midas
export MIDAS_EXPTAB=$PWD/exptab
export MIDAS_HOME=$MIDASSYS
export PATH=$MIDASSYS/linux/bin:$PATH
export LD_LIBRARY_PATH=$MIDASSYS/linux/lib:$PWD/compress:$LD_LIBRARY_PATH

cd compress && make && cd -

for crate in $(ls -d crate*);
do
	echo "****************************************************"
	echo "Making "$crate
	cd $crate && make && cd -
	echo "Done "$crate"!"
done

echo ""$expname" $PWD"/dir" $(whoami)" > exptab
