#! /bin/bash

Channel=SiR3-S
Gain=5.75035
Offset=58.5218
for run in $@;do
  echo "INSERT into Energy  (run, channel, gain, offset) values ( $run, \"${Channel}\", ${Gain}, ${Offset}) ;"
done
