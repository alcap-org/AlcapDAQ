#! /bin/bash

Channels=( SiL1-{1..16}-S )
Gains=( 1.97946 2.39031 2.5706 2.55082 2.51599 2.49057 2.52952 2.47565 2.52563 2.52322 2.56239 2.60126 2.60489 2.48965 2.5297 2.06624)
Offsets=( 1001.98 48.9111 47.6367 37.2288 29.4285 23.3372 32.8178 13.0947 28.3126 16.382 28.5298 27.4012 39.5879 19.7905 21.0712 999.997)
for run in $@;do
  for i_channel in {0..16};do
    echo "INSERT into Energy  (run, channel, gain, offset) values ( $run, \"${Channels[$i_channel]}\", ${Gains[$i_channel]}, ${Offsets[$i_channel]}) ;"
  done
done
