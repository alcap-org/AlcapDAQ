#!/bin/bash 

linkinc ()
{
    ln -sf $PWD/CAENComm/CAENComm-1.2/include/*.h $PWD/include/

    ln -sf $PWD/CAENDigitizer/CAENDigitizer_2.6.5/include/*.h $PWD/include/

    ln -sf $PWD/CAENVME/CAENVMELib-2.41/include/*.h $PWD/include/

    ln -sf $PWD/CAENV1x90/CAENV1x90_1_1_linux/CAENVMEToolBox/SDK/CAENV1x90SDK/include/*.h $PWD/include/
}

link32 ()
{
	ln -sf $PWD/CAENComm/CAENComm-1.2/lib/x86/libCAENComm.so.1.2.0 $PWD/lib/libCAENComm.so

	ln -sf \
		$PWD/CAENDigitizer/CAENDigitizer_2.6.5/lib/i686/libCAENDigitizer.so.2.6.5 \
		$PWD/lib/libCAENDigitizer.so

	ln -sf $PWD/CAENVME/CAENVMELib-2.41/lib/x86/libCAENVME.so.2.41 \
	 $PWD/lib/libCAENVME.so

	ln -sf $PWD/CAENV1x90/CAENV1x90_1_1_linux/CAENVMEToolBox/SDK/CAENV1x90SDK/libCAENV1x90SDK.so.1.1 \
	    $PWD/lib/libCAENV1x90SDK.so
}

link64 ()
{
	ln -sf $PWD/CAENComm/CAENComm-1.2/lib/x64/libCAENComm.so.1.2.0 $PWD/lib/libCAENComm.so

	ln -sf \
		$PWD/CAENDigitizer/CAENDigitizer_2.6.5/lib/x86_64/libCAENDigitizer.so.2.6.5 \
		$PWD/lib/libCAENDigitizer.so

	ln -sf $PWD/CAENVME/CAENVMELib-2.41/lib/x64/libCAENVME.so.2.41 \
	 $PWD/lib/libCAENVME.so	

	ln -sf $PWD/CAENV1x90/CAENV1x90_1_1_linux/CAENVMEToolBox/SDK/CAENV1x90SDK/libCAENV1x90SDK.so.1.1 \
	    $PWD/lib/libCAENV1x90SDK.so
}

mkdir -p include
mkdir -p lib

linkinc
# get OS type from shell
echo "**************************************"
ARCH=$(uname -m)

if [[ $ARCH == "x86_64" ]]; then
	echo "Linking CAEN x64 libraries ..."
	link64
else
	echo "Linking CAEN x86 libraries ..."
	link32
fi

echo "**************************************"
