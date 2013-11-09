#!/bin/bash 

link32 ()
{
	ln -sf $PWD/CAENComm/lib/libCAENComm.so.1.02 $PWD/lib/libCAENComm.so 

	ln -sf \
		$PWD/CAENDigitizer/CAENDigitizer_2.3.2/lib/i686/libCAENDigitizer.so.2.3.2 \
		$PWD/lib/libCAENDigitizer.so

	ln -sf $PWD/CAENVMELib/CAENVMELib-2.30.2/lib/x86/libCAENVME.so.2.30.2 \
	 $PWD/lib/libCAENVME.so	
}

link64 ()
{
	ln -sf $PWD/CAENComm/lib/x64/libCAENComm.so.1.02 $PWD/lib/libCAENComm.so 

	ln -sf \
		$PWD/CAENDigitizer/CAENDigitizer_2.3.2/lib/x86_64/libCAENDigitizer.so.2.3.2 \
		$PWD/lib/libCAENDigitizer.so

	ln -sf $PWD/CAENVMELib/CAENVMELib-2.30.2/lib/x64/libCAENVME.so.2.30.2 \
	 $PWD/lib/libCAENVME.so	
}

# get OS type from shell
echo "**************************************"
ARCH=$(uname -p)

if [[ $ARCH == "x86_64" ]]; then
	echo "Linking CAEN x64 libraries ..."
	link64
else
	echo "Linking CAEN x86 libraries ..."
	link32
fi

echo "**************************************"
