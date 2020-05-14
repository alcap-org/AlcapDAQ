d = "SiT-1-S SiT-2-S SiT-3-S SiT-4-S SiR1-1-S SiR1-2-S SiR1-3-S SiR1-4-S SiL1-1-S SiL1-2-S SiL1-3-S SiL1-4-S SiL1-5-S SiL1-6-S SiL1-7-S SiL1-8-S SiL1-9-S SiL1-10-S SiL1-11-S SiL1-12-S SiL1-13-S SiL1-14-S SiL1-15-S SiL1-16-S SiR2-S SiL3-S GeLoGain GeHiGain"

set term png

set output "toff_trend.png"
plot for [s in d] s.".toff_trend.csv" using 1:3 title s

set output "ped_trend.png"
plot for [s in d] s.".ped_trend.csv" using 1:2:3 title s
