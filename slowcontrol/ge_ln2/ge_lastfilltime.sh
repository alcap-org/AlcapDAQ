#!/bin/bash

# Get the last fill
# JF  = Just Filled     (BOOL in MIDAS)
# LF  = Last Fill       (STRING in MIDAS)
# TSF = Time Since Fill (INT in MIDAS)
# _D  = Date string
# _T  = Seconds since the beginning
# Define ODB commands to use
CMD_READ_JF="ls -v /Equipment/Germanium/Settings/Alarm/JustFilled"
CMD_READ_LF="ls -v /Equipment/Germanium/Settings/Alarm/LastFill"
CMD_ALLOWWRITE_LF="chmod 3 /Equipment/Germanium/Settings/Alarm/LastFill"
CMD_DISALLOWWRITE_LF="chmod 1 /Equipment/Germanium/Settings/Alarm/LastFill"
CMD_ALLOWWRITE_TSF="chmod 3 /Equipment/Germanium/Settings/Alarm/TimeSinceFill"
CMD_DISALLOWWRITE_TSF="chmod 1 /Equipment/Germanium/Settings/Alarm/TimeSinceFill"
CMD_SET_LF="set /Equipment/Germanium/Settings/Alarm/LastFill"
CMD_SET_TSF="set /Equipment/Germanium/Settings/Alarm/TimeSinceFill"
CMD_RESET_JF="set /Equipment/Germanium/Settings/Alarm/JustFilled n"

# Allow writing of certain fields
odbedit -e AlCap -c "$CMD_ALLOWWRITE_LF"
odbedit -e AlCap -c "$CMD_ALLOWWRITE_TSF"

# Check if there was recently a fill
NOW_D=$(date)
NOW_T=$(date -d "$NOW_D" +%s)
JF=$(odbedit -e AlCap -c "$CMD_READ_JF")
if [ $JF = "y" ]; then
    LF_D=$NOW_D
    LF_T=$(date -d "$LF_D" +%s)
    odbedit -e AlCap -c "$CMD_RESET_JF"
else
    LF_D=$(odbedit -e AlCap -c "$CMD_READ_LF")
    if [ -z "$LF_D" ]; then
	# If the ODB doesn't have a value, set it to a long time ago
	LF_T=0
    else
	LF_T=$(date -d "$LF_D" +%s)
    fi
fi
TSF=$((NOW_T-LF_T))

# Load time into ODB
odbedit -e AlCap -c "$CMD_SET_LF '$LF_D'"
odbedit -e AlCap -c "$CMD_SET_TSF '$TSF'"
# Reset permissions so fields aren't accidentally changed
odbedit -e AlCap -c "$CMD_DISALLOWWRITE_LF"
odbedit -e AlCap -c "$CMD_DISALLOWWRITE_TSF"
