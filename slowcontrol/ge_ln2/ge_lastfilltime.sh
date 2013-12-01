#!/bin/bash

. /home/daq/AlcapDAQ/thisdaq.sh

# Get the last fill
odbedit -e AlCap -c "exp /Equipment/Germanium/LastFill /home/daq/AlcapDAQ/slowcontrol/lastfill.txt"

LASTFILL=`date -f /home/daq/AlcapDAQ/slowcontrol/lastfill.txt +%s`
NOW=`date +%s`
CMD_ALLOWWRITE="chmod 3 /Equipment/Germanium/AlarmCheck/LastFillTime"
CMD_DISALLOWWRITE="AlCap chmod 1 /Equipment/Germanium/AlarmCheck/LastFillTime"
CMD_SETTIME="set /Equipment/Germanium/AlarmCheck/LastFillTime $((NOW-LASTFILL))"

# Load time into ODB
odbedit -e AlCap -c "$CMD_ALLOWWRITE"
odbedit -e AlCap -c "$CMD_SETTIME"
odbedit -e AlCap -c "$CMD_DISALLOWWRITE"
