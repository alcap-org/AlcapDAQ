#include "TDPGeneratorFactory.h"
#include "debug_tools.h"
#include "PassThroughDPGenerator.h"
#include <iostream>
using std::cout;
using std::endl;

IDs::generator PassThroughDPGenerator::fStaticId = IDs::generator("PassThrough", IDs::kAnyConfig);

PassThroughDPGenerator::PassThroughDPGenerator(TDPGeneratorOptions* opts):
    TVDetectorPulseGenerator("PassThrough",opts), fUseFast(opts->GetBool("use_fast",true)){
    // double check that we can use the slow source, ie. for detectors where
    // the fast / slow channel are different
}

int PassThroughDPGenerator::ProcessPulses( DetectorPulseList& output){
    const AnalysedPulseList* pulses=PulseList(fUseFast);
    // Major hack to force fill both channels in the produced TDP which makes
    // later analysis easier
    SetPulseLists(pulses,pulses);
    for(AnalysedPulseList::const_iterator i_pulse=pulses->begin();
            i_pulse!=pulses->end(); i_pulse++){
        TDetectorPulse* det_pulse = MakeTDP(i_pulse-pulses->begin(),i_pulse-pulses->begin()); // Create the TDetectorPulse
        output.push_back(det_pulse);
    }
    return 0;
}

ALCAP_TDP_GENERATOR(PassThrough,use_fast);
