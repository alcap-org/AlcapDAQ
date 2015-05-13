dirDGT="CAEN/CAENDigitizer/CAENDigitizer_2.6.5"
libDGT="lib/x86_64/libCAENDigitizer.so.2.6.5"
dirCOM="CAEN/CAENComm-1.2"
libCOM="lib/x64/libCAENComm.so.1.2.0"
dirVME="CAEN/CAENVMELib/CAENVMELib-2.41"
libVME="lib/x64/libCAENVME.so.2.41"


g++ -I$dirVME/include -I$dirCOM/include -I$dirDGT/include -o CAEN_Test test/CAEN_Test.cpp $dirDGT/$libDGT $dirCOM/$libCOM $dirVME/$libVME
