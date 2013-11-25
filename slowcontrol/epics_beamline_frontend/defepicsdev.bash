#! /bin/bash

  LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/lib
  export LD_LIBRARY_PATH

  PATH=$PATH:$PWD/bin
  export PATH

  export EPICS_CA_AUTO_ADDR_LIST=NO
# export EPICS_CA_ADDR_LIST=proscan-cagw01
# export EPICS_CA_ADDR_LIST="hipa-cagw01 mpc1365"
  export EPICS_CA_ADDR_LIST="mpc1365 hipa-cagw01"
# export EPICS_CA_ADDR_LIST=mpc1365
# export EPICS_CA_ADDR_LIST=hipa-cagw01
  export EPICS_CA_SERVER_PORT=5062
