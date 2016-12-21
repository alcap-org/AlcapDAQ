#! /bin/bash

Channel=SiR3-S
Gain=5.90075
Offset=35.8103
for run in $@;do
  echo "INSERT into Energy  (run, channel, gain, offset) values ( $run, \"${Channel}\", ${Gain}, ${Offset}) ;"
done
