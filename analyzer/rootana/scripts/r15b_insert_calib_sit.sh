#! /bin/bash

Channels=( SiT-{1..4}-S )
Gains=( 2.07393 1.96649 1.99088 2.00442)
Offsets=( -33.4731 -12.673 -37.2894 -24.4704)
for run in $@;do
  for i_channel in {0..3};do
    echo "INSERT into Energy  (run, channel, gain, offset) values ( $run, \"${Channels[$i_channel]}\", ${Gains[$i_channel]}, ${Offsets[$i_channel]}) ;"
  done
done
