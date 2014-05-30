#include "MaxTimeDiffMEGenerator.h"
#include <iostream>
#include <vector>
#include <cmath>
using std::cout;
using std::endl;

int MaxTimeDiffMEGenerator::ProcessPulses(MuonEventList& muonEventsOut,
		const StringDetPulseMap& detectorPulsesIn){

	  std::vector<DetectorPulseList::const_iterator> pulseIters;
	  std::vector<DetectorPulseList::const_iterator> finalIters;
	  for(StringDetPulseMap::const_iterator i_detector=detectorPulsesIn.begin();
			  i_detector!=detectorPulsesIn.end(); i_detector++){
			  pulseIters.push_back(i_detector->second.begin());
			  finalIters.push_back(i_detector->second.end());
	  }

	  // Loop through both TAnalysedPulse vectors until they are both finished
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
	    TMuonEvent* muon_event=new TMuonEvent();
	    for (unsigned int b = 0; b < pulseIters.size(); ++b) {

	      TDetectorPulse* pulse = *(pulseIters[b]);
	      double pulse_time = pulse->GetTime() * 1e-6; // convert to ms

	      if (std::fabs(pulse_time - min_time) < time_difference) {
		 if(Debug()) cout<<"Using pulse from: " << pulse->GetDetName() << std::endl;
		 muon_event->SetPulse(pulse->GetDetName(), pulse);
		++(pulseIters[b]); // increment the iterator because we used the pulse
	      }
	    }
	    muonEventsOut.push_back(muon_event);
	    if(Debug()){
	        cout<<"Created a TMuonEvent with "<<muon_event->GetNumPulses();
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

	  return 0;
}
