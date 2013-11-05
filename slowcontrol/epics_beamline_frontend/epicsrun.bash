#!/bin/bash

#define epics specific environment variables, PATH and LD_LIBRARY_PATH
. /usr/local/midas/defepics.bash

# shell parameters 
# $1 is program name NOT including path -> add path
# $2 is -h
# $3 is <hostname>
# $4 is -e
# $5 is td_musr
# $6 is -p
# $7 is <path>
# $8 is -D

#/usr/local/midas/$1 $2 $3 $4 $5 $6 $7 $8
/usr/local/midas/$1 $2 $3 $4 $5 $6 $7 $8 >> /dev/null 2>> /dev/null
