#! /bin/bash

Channel=SiR2-S
Gain=4.42025
Offset=0.00377677
for run in $@;do
  echo "INSERT into Energy  (run, channel, gain, offset) values ( $run, \"${Channel}\", ${Gain}, ${Offset}) ;"
done
