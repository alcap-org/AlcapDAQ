#!/bin/bash
# install-sqlite.sh: installs the SQLite package in AlcapDAQ from source
#                    creates three directories: sqlite-autoconf-3080500/ -- the source directory
#                                               sqlite-install/ -- the install directory
# Created by: Andrew Edmonds
# Date: 23-06-2014

set -e # Make bash stop if an error occurs

if [ -d "sqlite-autoconf-3080500" ] ;then
	echo It seems SQLite is already installed.  Check the directory:
	echo "  `pwd`/sqlite-autoconf-3080500"
	echo and remove it if you wish to replace that version of sqlite3.
	exit 1
fi

# Unpack the SQLite tarball, this creates the sqlite3/ directory
tar -xzvf sqlite-autoconf-3080500.tar.gz

# Create the other two directories
mkdir -p sqlite-install

# Use cmake to create the Makefiles for your system
cd sqlite-autoconf-3080500
./configure --prefix=$PWD/../sqlite-install 2>&1 |tee install_log.txt

# Now compile and install
make 2>&1 |tee -a install_log.txt
make install 2>&1 |tee -a install_log.txt
