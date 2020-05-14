#!/bin/bash
echo "WARNING: Make sure you're using gnuplot 4.6 or later because of for-loop syntax."
echo "         (On Merlin: module add gnuplot/gnuplot-4.6)"


chans="SiT-1-S SiT-2-S SiT-3-S SiT-4-S"
chans="$chans SiR1-1-S SiR1-2-S SiR1-3-S SiR1-4-S"
chans="$chans SiL1-1-S SiL1-2-S  SiL1-3-S  SiL1-4-S  SiL1-5-S  SiL1-6-S  SiL1-7-S  SiL1-8-S"
chans="$chans SiL1-9-S SiL1-10-S SiL1-11-S SiL1-12-S SiL1-13-S SiL1-14-S SiL1-15-S SiL1-16-S"
chans="$chans SiR2-S SiL3-S"
chans="$chans GeLoGain GeHiGain"

# chans="$chans SiR1-1-F SiR1-2-F SiR1-3-F SiR1-4-F"
# chans="$chans SiL1-A-F SiL1-B-F"
# chans="$chans SiR2-F SiL2-F"
# chans="$chans SiR3-F SiL3-F"
# chans="$chans Ge-F"
# chans="$chans ScR ScL ScGe ScVe"
# chans="$chans NDet NDet2"

for ch in $chans; do
    sqlite3 calibration.db -column "SELECT * FROM CoarseTimeOffset WHERE channel='$ch'" > $ch.toff_trend.csv
    sqlite3 calibration.db -column "SELECT run,pedestal,noise FROM PedestalAndNoise WHERE channel='$ch'" > $ch.ped_trend.csv
done

gnuplot scripts/plot_trends.gnuplot

rm *_trend.csv
