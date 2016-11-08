#! /bin/bash

echo "CREATE TABLE IF NOT EXISTS Energy(run INT, channel TEXT, 'gain' REAL, 'gain_err' REAL, 'offset' REAL, 'offset_err' REAL, 'chi2' REAL, 'ndf' REAL);"
for run in $@;do
# Calib Run 10319
    echo "INSERT into Energy  (run, channel, gain, gain_err, offset, offset_err) values ( $run, \"GeLoGain\", 0.399112, 0.0000111913, 0.230034, 0.0155854) ;"
    echo "INSERT into Energy  (run, channel, gain, gain_err, offset, offset_err) values ( $run, \"GeHiGain\", 0.185084, 0.00000477178, 0.114321, 0.0125212) ;"

# Calibr run 9282
#    echo "INSERT into Energy  (run, channel, gain, gain_err, offset, offset_err) values ( $run, \"GeLoGain\", 0.395778, 0.00000643957, 0.282495, 0.00893179) ;"
#    echo "INSERT into Energy  (run, channel, gain, gain_err, offset, offset_err) values ( $run, \"GeHiGain\", 0.184707, 0.0000293453, -0.246972, 0.19808) ;"
done
