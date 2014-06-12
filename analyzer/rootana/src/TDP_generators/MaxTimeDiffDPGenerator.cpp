#include "TDPGeneratorFactory.h"
#include "MaxTimeDiffDPGenerator.h"
#include <iostream>
#include <vector>
#include <cmath>
using std::cout;
using std::endl;

void MaxTimeDiffDPGenerator::ProcessPulses(const TSetupData* setup,const std::string& detector, 
		  const AnalysedPulseList* fast_pulses, const AnalysedPulseList* slow_pulses,
		  DetectorPulseList& output){

	  AnalysedPulseList::const_iterator fastPulseIter = fast_pulses->begin();
	  AnalysedPulseList::const_iterator slowPulseIter = slow_pulses->begin();
	  AnalysedPulseList::const_iterator finalFastPulseIter = fast_pulses->end();
	  AnalysedPulseList::const_iterator finalSlowPulseIter = slow_pulses->end();

	  std::vector<AnalysedPulseList::const_iterator> pulseIters;
	  pulseIters.push_back(fastPulseIter);
	  pulseIters.push_back(slowPulseIter);

	  std::vector<AnalysedPulseList::const_iterator> finalIters;
	  finalIters.push_back(finalFastPulseIter);
	  finalIters.push_back(finalSlowPulseIter);

	  // Loop through both TAnalysedPulse vectors until they are both finished
	  // NB with this alogirthm this can be extended to more than 2
          double min_time = 999999; // something large
          double pulse_time;
	  while (pulseIters.size() > 0) {

	    // Find out which of the next fast or slow pulsees happened next
	    for (unsigned int b = 0; b < pulseIters.size(); ++b) {
	      pulse_time = (*pulseIters[b])->GetTime() * 1e-6; // convert to ms	      
	      min_time = std::min(min_time, pulse_time);

	    }

	    // Now go through and find all the pulses that are within a certain time of this
	    double time_difference = 0.1; // 0.1 ms
	    TAnalysedPulse* fast_pulse = NULL;
	    TAnalysedPulse* slow_pulse = NULL;
	    for (unsigned int b = 0; b < pulseIters.size(); ++b) {

	      TAnalysedPulse* pulse = *(pulseIters[b]);
	      double pulse_time = pulse->GetTime() * 1e-6; // convert to ms

	      if (std::fabs(pulse_time - min_time) < time_difference) {
		if ( pulse->GetSource()==IDs::Fast) {
		  if(Debug()) cout<<"Fast? " << pulse->GetSource().str() << std::endl;
		  fast_pulse = pulse;
		} else if ( pulse->GetSource()==IDs::Slow) {
		  if(Debug()) cout<<"Slow? " << pulse->GetSource().str() << std::endl;
		  slow_pulse = pulse;
		}

		++(pulseIters[b]); // increment the iterator because we used the pulse
	      }
	    }
	    TDetectorPulse* det_pulse = new TDetectorPulse(fast_pulse, slow_pulse, detector); // Create the TDetectorPulse
	    output.push_back(det_pulse);
	    if(Debug()){
	        cout<<"Created a TDetectorPulse with:\n";
	        cout<<"Fast Pulse: " << det_pulse->GetFastPulseTime() * 1e-6 << std::endl;
	        cout<< "Slow Pulse: " << det_pulse->GetSlowPulseTime() * 1e-6 << std::endl;
	        cout<< std::endl;
	    }

	    //Delete the iterators to finished banks. Go through in reverse to
	    //avoid invalidation problems
	    for (int b = pulseIters.size()-1; b >= 0; --b) {
	      if (pulseIters[b] == finalIters[b]){
		pulseIters.erase(pulseIters.begin() + b);
		finalIters.erase(finalIters.begin() + b);
	      }  
	    } // for (int b -reversed)	      
	  } // end for
}

ALCAP_TDP_GENERATOR(MaxTimeDiff);
