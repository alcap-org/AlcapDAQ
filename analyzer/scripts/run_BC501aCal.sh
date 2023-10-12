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
RunNumbers=( 7192 7193 7194 7195 7196 7197 7198 7199 7200 7201 7202 7203 7204 )
# TEST
# RunNumbers=( 7192 7193 )

for Run in ${RunNumbers[@]}; do
    bash BC501aCal_Base.sh $Run &
done
