#!/bin/bash
# Converts the modules of the Si detector to the other detectors (Ge, Sc)

cp ../../src/Si/MSiPulseFinder.cpp ../../src/Ge/MGePulseFinder.cpp
cp ../../src/Si/TSimpleSiPulse.h ../../src/Ge/TSimpleGePulse.h
sed -i 's/Si\([^m]\)/Ge\1/g' ../../src/Ge/MGePulseFinder.cpp ../../src/Ge/TSimpleGePulse.h # Make sure we don't replace the 'Si' in 'Simple'
sed -i 's/SI/GE/g' ../../src/Ge/TSimpleGePulse.h # change the include guards

cp ../../src/Si/MSiPulseFinder.cpp ../../src/Sc/MScPulseFinder.cpp
cp ../../src/Si/TSimpleSiPulse.h ../../src/Sc/TSimpleScPulse.h
sed -i 's/Si\([^m]\)/Sc\1/g' ../../src/Sc/MScPulseFinder.cpp ../../src/Sc/TSimpleScPulse.h # Make sure we don't replace the 'Si' in 'Simple'
sed -i 's/SI/SC/g' ../../src/Sc/TSimpleScPulse.h # change the include guards
