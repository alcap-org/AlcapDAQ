# R15a_Prun1_Polyethylene.sh
# -- A script that will send alcapana to run over the Polyethylene runs on Merlin
# CAREFUL, no DQ checks done

./batch_R15a_Prun1.sh -n 20 -t 60 7288 -r 7290 7292 -r 7295 7310
