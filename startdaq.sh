#!/bin/bash

#odbedit -c clean
#odbedit -c "load analyzer/odb/Defaults.odb"
#odbedit -c "load analyzer/odb/Analyzer_WireMap.odb"
odbedit -c "load default_2015-05-11.odb"

. thisdaq.sh

screen -d -m -S mserver      bash -c 'cd ~/AlcapDAQ; . thisdaq.sh; mserver'
screen -d -m -S mhttpd       bash -c 'cd ~/AlcapDAQ; . thisdaq.sh; mhttpd -p 8080'

screen -d -m -S mlogger      bash -c 'cd ~/AlcapDAQ; . thisdaq.sh; /home/daq/AlcapDAQ/midas/linux64/bin/mlogger -e AlCap'
screen -d -m -S eventbuilder bash -c 'cd ~/AlcapDAQ; . thisdaq.sh; /home/daq/AlcapDAQ/evb/mevb -e AlCap -b BUF'

sleep 1s

screen -d -m -S crate3       bash -c 'cd ~/AlcapDAQ; . thisdaq.sh; ./crate3/crate -e "AlCap" -h localhost'
screen -d -m -S crate4       bash -c 'cd ~/AlcapDAQ; . thisdaq.sh; ./crate4_VME/crate -e "AlCap" -h localhost'
#screen -d -m -S crate5       bash -c 'cd ~/AlcapDAQ; . thisdaq.sh; ./crate5_CAEN_DT5720_OpticalLink/crate -e "AlCap" -h localhost'
#screen -d -m -S crate7       bash -c 'cd ~/AlcapDAQ; . thisdaq.sh; ./crate7_CAEN_DT5730_OpticalLink/crate -e "AlCap" -h localhost'
#screen -d -m -S crate8       bash -c 'cd ~/AlcapDAQ; . thisdaq.sh; ./crate8/crate -e "AlCap" -h localhost'

#screen -d -m -S analyzer     bash -c 'cd ~/AlcapDAQ; . thisdaq.sh; ./analyzer/work/onlineana/alcapana -e "AlCap" -h localhost -o /home/daq/AlcapDAQ/dir/run.mid -O /home/daq/AlcapDAQ/dir/hist.root -T /home/daq/AlcapDAQ/dir/tree.root'
screen -d -m -S analyzer     bash -c 'cd ~/AlcapDAQ; . thisdaq.sh; ./analyzer/work/onlineana/alcapana -e "AlCap" -h localhost -l -T tree.root'


