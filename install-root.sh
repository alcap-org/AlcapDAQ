#!/bin/bash
# install-root.sh: installs the ROOT package in AlcapDAQ from source
#                  creates three directories: root/ -- the source directory
#                                             root-build/ -- the build directory
#                                             root-install/ -- the install directory
# Created by: Andrew Edmonds
# Date: 25-04-2014
# 
# Requirements: Cmake v2.8 or higher

# Unpack the ROOT tarball, this creates the root/ directory
tar -xzvf root_v5.34.18.source.tar.gz

# Create the other two directories
mkdir root-build root-install

# Use cmake to create the Makefiles for your system
cd root-build
cmake -DCMAKE_INSTALL_PREFIX=../root-install ../root/

# Now compile and install
make
make install

# And source the environment variables
. ../root-install/bin/thisroot.sh