# R15a_Prun1_Stainless.sh
# -- A script that will send alcapana to run over the stainless steel runs on Merlin
# CAREFUL, no DQ checks done

./batch_R15a_Prun1.sh -n 20 -t 60 -r 6995 7002 -r 7004 7010 -r 7012 7015
