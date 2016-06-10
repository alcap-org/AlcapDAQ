#! /bin/bash

#TODO: Double check values
#TODO: Add GeHiGain
for run in $@;do
    echo "INSERT into Energy  (run, channel, gain, gain_err, offset, offset_err) values ( $run, \"GeLoGain\", 0.394281, 0.00000484182, -2.10752, 0.00741736) ;"
    echo "INSERT into Energy  (run, channel, gain, gain_err, offset, offset_err) values ( $run, \"GeHiGain\", 0.184685, 0.00000176319, -9.00143, 0.00474831) ;"
done
