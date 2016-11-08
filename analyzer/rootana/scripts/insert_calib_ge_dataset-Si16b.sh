#!/bin/bash

# Runs the insert_calib_ge.sh script for Andy's Si16b tranches
echo "!!! When the sqlite3 prompt appears, type '.q' and press enter to move to the next tranche"
sqlite3 -init <(/gpfs/home/edmonds_a/AlcapDAQ/analyzer/rootana/scripts/insert_calib_ge.sh 1036{2..8}) /gpfs/home/edmonds_a/calibration.db
sqlite3 -init <(/gpfs/home/edmonds_a/AlcapDAQ/analyzer/rootana/scripts/insert_calib_ge.sh 10{383..421}) /gpfs/home/edmonds_a/calibration.db
sqlite3 -init <(/gpfs/home/edmonds_a/AlcapDAQ/analyzer/rootana/scripts/insert_calib_ge.sh 104{25..50}) /gpfs/home/edmonds_a/calibration.db

