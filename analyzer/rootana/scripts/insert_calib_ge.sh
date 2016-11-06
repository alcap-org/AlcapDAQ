#! /bin/bash

echo "CREATE TABLE IF NOT EXISTS Energy(run INT, channel TEXT, 'gain' REAL, 'gain_err' REAL, 'offset' REAL, 'offset_err' REAL, 'chi2' REAL, 'ndf' REAL);"
for run in $@;do
    echo "INSERT into Energy  (run, channel, gain, gain_err, offset, offset_err) values ( $run, \"GeLoGain\", 0.397502, 0.0000062737, 0.368123, 0.00942246) ;"
    echo "INSERT into Energy  (run, channel, gain, gain_err, offset, offset_err) values ( $run, \"GeHiGain\", 0.185048, 0.00000254909, -0.158258, 0.00675345) ;"
done
