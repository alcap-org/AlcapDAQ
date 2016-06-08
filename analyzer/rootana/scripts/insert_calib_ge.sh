#! /bin/bash

#TODO: Double check values
#TODO: Add GeHiGain
for run in $@;do
    echo "INSERT into Energy  (run, channel, gain, gain_err, offset, offset_err) values ( $run, \"GeLoGain\", 0.394267, 0.00000595129, -2.06898, 0.00900507) ;"
#    echo "INSERT into Energy  (run, channel, gain, gain_err, offset, offset_err) values ( $run, \"GeHiGain\", 0.394267, 0.00000595129, -2.06898, 0.00900507) ;"
done