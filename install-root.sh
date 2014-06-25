#!/bin/bash
# install-root.sh: installs the ROOT package in AlcapDAQ from source
#                  creates three directories: root/ -- the source directory
#                                             root-build/ -- the build directory
#                                             root-install/ -- the install directory
# Created by: Andrew Edmonds
# Date: 25-04-2014
# 
# Requirements: Cmake v2.8 or higher

set -e # Make bash stop if an error occurs

if [ -d "root" ] ;then
	echo It seems Root is already installed.  Check the directory:
	echo "  `pwd`/root"
	echo and remove it if you wish to replace that version of root.
	exit 1
fi

# Unpack the ROOT tarball, this creates the root/ directory
# using checkpoint and the pipe tricks is to reduce output down to ~20 lines
tar --checkpoint -xzf root_v5.34.18.source.tar.gz  2>&1 | grep ".*000$"

# Create the other two directories
mkdir -p root-build root-install

# Use cmake to create the Makefiles for your system
cd root-build/
cmake -Dminuit2=ON -DCMAKE_INSTALL_PREFIX=../root-install ../root/ 2>&1 |tee install_log.txt

# Now compile and install
make 2>&1 |tee -a install_log.txt
make install 2>&1 |tee -a install_log.txt
