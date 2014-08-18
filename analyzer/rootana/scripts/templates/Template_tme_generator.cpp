#include "TMEGeneratorFactory.h"
#include "%module%MEGenerator.h"
#include "TDetectorPulse.h"
#include "TMuonEvent.h"
#include <iostream>
using std::cout;
using std::endl;

%module%MEGenerator::%module%MEGenerator(TMEGeneratorOptions* opts):
	TVMuonEventGenerator("%module%",opts){
	// Do things to set up the generator here. 
}

int %module%MEGenerator::ProcessPulses(MuonEventList& muonEventsOut,
        const SourceDetPulseMap& detectorPulsesIn){

	// returning 0 tells the caller that we were successful, return non-zero if not
	return 0;
}

ALCAP_TME_GENERATOR(%module%);
