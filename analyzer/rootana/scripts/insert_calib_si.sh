#! /bin/bash

RightChannels=( SiR{1_{1..4},2}-S )
RightGains=(    2.53   2.62  2.49   2.53  7.96 )
RightOffsets=( -87.97  -139.5 -30.2  -103.7 -182.99 )

LeftChannels=( SiL{1_{1..4},2}-S )
LeftGains=(    2.61   2.54  2.65   2.54   7.86 )
LeftOffsets=( -113.17 -52.19 -131.1 -84.29 -111.23 )

for run in $@;do
  for i_channel in {0..4};do
    echo "INSERT into Energy  (run, channel, gain, offset) values ( $run, \"${RightChannels[$i_channel]}\", ${RightGains[$i_channel]}, ${RightOffsets[$i_channel]}) ;"
    echo "INSERT into Energy  (run, channel, gain, offset) values ( $run, \"${LeftChannels[$i_channel]}\", ${LeftGains[$i_channel]}, ${LeftOffsets[$i_channel]}) ;"
  done
done