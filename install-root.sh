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

if [ -d "root-install" ] ;then
	echo It seems Root is already installed.  Check the directory:
	echo "  `pwd`/root-install"
	echo and remove it if you wish to replace that version of root.
	exit 1
fi

if [ ! -d "sqlite-install" ] ;then
	echo You should compile sqlite before compiling root.
	echo Run the ./install-sqlite.sh script first.
	echo Aborting....
	exit 2
fi

# Unpack the ROOT tarball, this creates the root/ directory
# using checkpoint and the pipe tricks is to reduce output down to ~20 lines
tar --checkpoint -xzf root_v5.34.20.source.tar.gz  2>&1 | grep ".*000$"

# Create the other two directories
mkdir -p root-build root-install

# Use cmake to create the Makefiles for your system
cd root-build/
cmake -Dminuit2=ON -Dsqlite=on -DSQLITE_INCLUDE_DIR=../sqlite-install/include -DSQLITE_LIBRARIES=../sqlite-install/lib/libsqlite3.so -DCMAKE_INSTALL_PREFIX=../root-install ../root/ 2>&1 |tee install_log.txt

# Now compile and install
make 2>&1 |tee -a install_log.txt
make install 2>&1 |tee -a install_log.txt
