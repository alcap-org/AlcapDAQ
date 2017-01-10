#! /bin/bash

Channels=( SiR1-{1..4}-S )
Gains=( 1.81475 1.80298 1.83618 1.80375)
Offsets=( -48.8053 -34.7606 -62.3042 -76.4902)
for run in $@;do
  for i_channel in {0..3};do
    echo "INSERT into Energy  (run, channel, gain, offset) values ( $run, \"${Channels[$i_channel]}\", ${Gains[$i_channel]}, ${Offsets[$i_channel]}) ;"
  done
done
