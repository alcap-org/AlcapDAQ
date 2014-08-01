#include "TDPGeneratorFactory.h"
#include "PassThroughDPGenerator.h"
#include <iostream>
using std::cout;
using std::endl;

PassThroughDPGenerator::PassThroughDPGenerator(TDPGeneratorOptions* opts):
    TVDetectorPulseGenerator("PassThrough",opts){
    }

int PassThroughDPGenerator::ProcessPulses( DetectorPulseList& output){
    for(AnalysedPulseList::const_iterator i_pulse=FastPulseList()->begin();
            i_pulse!=FastPulseList()->end(); i_pulse++){
        TDetectorPulse* det_pulse = MakeTDP(i_pulse-FastPulseList()->begin(), -1); // Create the TDetectorPulse
        output.push_back(det_pulse);
    }
    return 0;
}

ALCAP_TDP_GENERATOR(PassThrough);
