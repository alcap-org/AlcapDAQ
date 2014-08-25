d = "Ge-F ScGe SiL1-1-F SiL1-3-F SiL2-F SiR1-2-F SiR1-4-F Ge-S ScL SiL1-1-S SiL1-3-S SiL2-S SiR1-2-S SiR1-4-S NDet2 ScR SiL1-2-F SiL1-4-F SiR1-1-F SiR1-3-F SiR2-F NDet ScVe SiL1-2-S SiL1-4-S SiR1-1-S SiR1-3-S SiR2-S"
set term png

set output "toff_trend.png"
plot for [s in d] s.".toff_trend.csv" using 1:3 title s

set output "ped_trend.png"
plot for [s in d] s.".ped_trend.csv" using 1:2:3 title s
