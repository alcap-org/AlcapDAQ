#! /bin/bash

Channel=SiR2-S
Gain=4.35313
Offset=85.7855
for run in $@;do
  echo "INSERT into Energy  (run, channel, gain, offset) values ( $run, \"${Channel}\", ${Gain}, ${Offset}) ;"
done
