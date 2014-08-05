#include "TDPGeneratorFactory.h"
#include "%module%DPGenerator.h"
#include <iostream>
using std::cout;
using std::endl;

%module%DPGenerator::%module%DPGenerator(TDPGeneratorOptions* opts):
    TVDetectorPulseGenerator("%module%",opts){
    }

int %module%DPGenerator::ProcessPulses( DetectorPulseList& output){
    return 0;
}

ALCAP_TDP_GENERATOR(%module%);
