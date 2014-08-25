#!/bin/bash
echo "WARNING: Make sure you're using gnuplot 4.6 or later because of for-loop syntax."
echo "         (On Merlin: module add gnuplot/gnuplot-4.6)"


chans="SiR1-1-S SiR1-2-S SiR1-3-S SiR1-4-S SiR1-1-F SiR1-2-F SiR1-3-F SiR1-4-F"
chans="$chans SiL1-1-S SiL1-2-S SiL1-3-S SiL1-4-S SiL1-1-F SiL1-2-F SiL1-3-F SiL1-4-F"
chans="$chans SiR2-S SiR2-F SiL2-S SiL2-F"
chans="$chans Ge-S Ge-F"
chans="$chans ScR ScL ScGe ScVe"
chans="$chans NDet NDet2"

for ch in $chans; do
    sqlite3 calibration.db -column "SELECT * FROM CoarseTimeOffset WHERE channel='$ch'" > $ch.toff_trend.csv
    sqlite3 calibration.db -column "SELECT run,pedestal,noise FROM PedestalAndNoise WHERE channel='$ch'" > $ch.ped_trend.csv
done

gnuplot scripts/plot_trends.gnuplot

rm *_trend.csv
