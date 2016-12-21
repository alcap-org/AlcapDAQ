#! /bin/bash

Channel=SiL3-S
Gain=4.47046
Offset=11.0748
for run in $@;do
  echo "INSERT into Energy  (run, channel, gain, offset) values ( $run, \"${Channel}\", ${Gain}, ${Offset}) ;"
done
