#include "TDPGeneratorFactory.h"
#include "MaxTimeDiffDPGenerator.h"
#include <iostream>
#include <vector>
#include <cmath>
#include "debug_tools.h"
using std::cout;
using std::endl;

MaxTimeDiffDPGenerator::MaxTimeDiffDPGenerator(TDPGeneratorOptions* opts):
    TVDetectorPulseGenerator("MaxTimeDiff",opts){
    }

int MaxTimeDiffDPGenerator::ProcessPulses(const TSetupData* setup,const std::string& detector, 
        const AnalysedPulseList* fast_pulses, const AnalysedPulseList* slow_pulses,
        DetectorPulseList& output){

    if(!slow_pulses) return CopyAnalToDet(fast_pulses,detector,output);

    AnalysedPulseList::const_iterator fastPulseIter = fast_pulses->begin();
    AnalysedPulseList::const_iterator slowPulseIter = slow_pulses->begin();
    AnalysedPulseList::const_iterator finalFastPulseIter = fast_pulses->end();
    AnalysedPulseList::const_iterator finalSlowPulseIter = slow_pulses->end();

    std::vector< std::pair<
        AnalysedPulseList::const_iterator,
        AnalysedPulseList::const_iterator> > pulseIters;
    pulseIters.push_back(std::make_pair(fastPulseIter,finalFastPulseIter));
    pulseIters.push_back(std::make_pair(slowPulseIter,finalSlowPulseIter));

    // Loop through both TAnalysedPulse vectors until they are both finished
    // NB with this alogirthm this can be extended to more than 2
    double min_time = 999999; // something large
    double pulse_time;
    //int count=0;
    AnalysedPulseList::const_iterator fast_pulse, slow_pulse;
    while (pulseIters.size() > 0) {
        min_time = 999999e9;
        //DEBUG_VALUE(count++);

        // Find out which of the next fast or slow pulsees happened next
        for (unsigned int b = 0; b < pulseIters.size(); ++b) {
            pulse_time = (*pulseIters.at(b).first)->GetTime() * 1e-6; // convert to ms	      
            min_time = std::min(min_time, pulse_time);

        }

        // Now go through and find all the pulses that are within a certain time of this
        double time_difference = 0.1; // 0.1 ms
        for (unsigned int b = 0; b < pulseIters.size(); ++b) {

            TAnalysedPulse* pulse = *(pulseIters[b].first);
            double pulse_time = pulse->GetTime() * 1e-6; // convert to ms

            if (std::fabs(pulse_time - min_time) < time_difference) {
                if ( pulse->GetSource().isFast()) {
                    if(Debug()) cout<<"Fast? " << pulse->GetSource().str() << std::endl;
                    fast_pulse = pulseIters[b].first;
                } else if ( pulse->GetSource().isSlow()) {
                    if(Debug()) cout<<"Slow? " << pulse->GetSource().str() << std::endl;
                    slow_pulse = pulseIters[b].first;
                }

                ++(pulseIters[b].first); // increment the iterator because we used the pulse
            }
        }
        TDetectorPulse* det_pulse = MakeTDP(
                fast_pulse- fast_pulses->begin(),
                slow_pulse - slow_pulses->begin()); // Create the TDetectorPulse
        output.push_back(det_pulse);
        if(Debug()){
            cout<<"Created a TDetectorPulse with:\n";
            cout<<"Fast Pulse: " << det_pulse->GetTime(TDetectorPulse::kFast) * 1e-6 << std::endl;
            cout<< "Slow Pulse: " << det_pulse->GetTime(TDetectorPulse::kSlow) * 1e-6 << std::endl;
            cout<< std::endl;
        }

        //Delete the iterators to finished banks. Go through in reverse to
        //avoid invalidation problems
        for (int b = pulseIters.size()-1; b >= 0; --b) {
            //DEBUG_VALUE((*pulseIters[b].first)->GetParentID());
            if (pulseIters[b].first == pulseIters[b].second){
                pulseIters.erase(pulseIters.begin() + b);
            }  
        } // for (int b -reversed)	      
    } // end for
    return 0;
}

int MaxTimeDiffDPGenerator::CopyAnalToDet(
        const AnalysedPulseList* fast_pulses, const std::string& detector,
        DetectorPulseList& output){
    if(!fast_pulses){
        cout<<"NULL TAP list passed to MaxTimeDiffDPGenerator for both fast and slow pulses"<<endl;
        return 1;
    }
    for(AnalysedPulseList::const_iterator i_pulse=fast_pulses->begin();
            i_pulse!=fast_pulses->end(); i_pulse++){

        TDetectorPulse* det_pulse = MakeTDP(i_pulse-fast_pulses->begin(), -1); // Create the TDetectorPulse
        output.push_back(det_pulse);
    }
    return 0;
}

ALCAP_TDP_GENERATOR(MaxTimeDiff);
