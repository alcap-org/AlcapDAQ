#! /bin/bash

# Al2mm Y
# RunNumbers=( 6892 )
# Cs137 C -- None
# NdetU
# RunNumbers=( 7196 )
# NdetD
#RunNumbers=( 7197 )
# AmBe C
# NdetU 7194, 7195
# NdetD 7192, 7193
# RunNumbers=( 7192 7193 7194 7195 )
#RunNumbers=( 7192 )

# ALL CALIBRATIONS (NdetD and NdetU)
# key: (AmBeD, AmBeD, AmBeU, AmBeU, CsU, CsD, NaU, NaD, YU, YD, BiU, BiD, BKG)
# RunNumbers=( 7192 7193 7194 7195 7196 7197 7198 7199 7200 7201 7202 7203 7204 )
# TEST
# RunNumbers=( 7192 7193 )
# Rerun failed - Cs U
# RunNumbers=( 7196 )
# Al golden
#RunNumbers=( 6892 )
# H2O first run
#RunNumbers=( 7320 )
# Ti first run (after Al golden); Pb first run (after Al golden)
#RunNumbers=( 6905 7205 )
# W first run I see
#RunNumbers=( 7384 )
# Mix of runs:
# Al, Al, Al, H2O, Ti, Pb, W
#RunNumbers=( 6892 7311 7370 7320 6905 7205 7384 )
# H2O, H2O, Ti, Ti, Pb, Pb
#RunNumbers=( 7360 7426 7050 7188 7240 7284 )
#### 08-20-24 Rerunning all with WFD and TDC sorting.
#RunNumbers=( 6892 7311 7370 7320 7360 7426 6905 7050 7188 7205 7240 7284 7384 7192 7193 7194 7195 7196 7197 7198 7199 7200 7201 7202 7203 7204 )
# rerunning some
#RunNumbers=( 6892 6905 7050 7188 7202 7240 7311 7426 )
# rerunning again
#RunNumbers=( 7360 7205 7284 7203 7384 )
####
# Ndet Calibrations, Al Golden
#RunNumbers=( 6892 7192 7193 7194 7195 7196 7197 7198 7199 7200 7201 7202 7203 7204 )
# Ge Calibrations
#RunNumbers=( 7238 7239 )
# Bkg
#RunNumbers=( 7204 )
# Pb test
RunNumbers=( 5433 )

for Run in ${RunNumbers[@]}; do
    bash DebugFullAnalysis_Base.sh $Run &
    sleep 1.0
done
