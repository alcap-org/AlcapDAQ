#!/bin/bash

RUN_NUMBER=10204

./rootana -m configurations/pedestalsandnoise_calib.cfg -i ~/data/tree/local/tree${RUN_NUMBER}.root -o ~/data/out/local/out${RUN_NUMBER}_pedestals.root -c
python scripts/merge_calibration_database.py ~/data/calib/local/calibration.db calib_db/calib.run${RUN_NUMBER}.PedestalAndNoise.csv
./rootana -m configurations/R15b_t-muon-event_timeoffset.cfg -i ~/data/tree/local/tree${RUN_NUMBER}.root -o ~/data/out/local/out${RUN_NUMBER}_tme-timeoffset.root -d ~/data/calib/local/calibration.db -c
python scripts/merge_calibration_database.py ~/data/calib/local/calibration.db calib_db/calib.run${RUN_NUMBER}.CoarseTimeOffset.csv
python scripts/insert_energy_calibration.py ~/data/calib/local/calibration.db ${RUN_NUMBER}
./rootana -m configurations/R15b_tme-tree.cfg -i ~/data/tree/local/tree${RUN_NUMBER}.root -o ~/data/out/local/out${RUN_NUMBER}_tme-tree.root -d ~/data/calib/local/calibration.db
