# R15a_Prun1_MT.sh
# -- A script that will send alcapana to run over the MT runs on Merlin
# CAREFUL, no DQ checks done

./batch_R15a_Prun1.sh -n 20 -t 60 -r 6033 6047 -r 6050 6054 -r 6564 6583 -r 6585 6591 -r 6594 6601
