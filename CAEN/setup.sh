#!/bin/bash

mkdir -p CAENComm
cd CAENComm
tar -xvzf ../tar/CAENComm-1.2.tgz
cd ..

mkdir -p CAENDigitizer
cd CAENDigitizer
tar -xvzf ../tar/CAENDigitizer_2.6.5.tgz
cd ..

mkdir -p CAENUSB
cd CAENUSB
tar -xzvf ../tar/CAENUSBdrvB-1.4.tgz
cd ..

mkdir -p CAENV1x90
cd CAENV1x90
tar -xzvf ../tar/CAENV1x90_1_1_linux.tgz
cd CAENV1x90_1_1_linux
tar -xzvf CAENV1x90.tgz
cd CAENVMEToolBox/SDK/CAENV1x90SDK
make
cd ../../../../..

pwd
mkdir -p CAENVME
cd CAENVME
tar -xzvf ../tar/CAENVMELib-2.41.tgz
cd ..
