//#define USE_PRINT_OUT 

#include "CreateDetectorPulse.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <cmath>

#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;
extern std::map<std::string, std::vector<TDetectorPulse*> > gDetectorPulseMap;

CreateDetectorPulse::CreateDetectorPulse(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){ 
  dir->cd("/");
}

CreateDetectorPulse::~CreateDetectorPulse(){  
}

int CreateDetectorPulse::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  //typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  //typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  //typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  // Loop through and find a fast channel
   //std::cout << "Size of gAnalysedPulseMap " << gAnalysedPulseMap.size() << std::endl;
  for (std::map<std::string, std::vector<TAnalysedPulse*> >::iterator findFastIter = gAnalysedPulseMap.begin(); findFastIter != gAnalysedPulseMap.end(); findFastIter++) {

    std::string fast_det_name = findFastIter->first;
     //std::cout << "DetName; " << fast_det_name << std::endl;
    if (! TSetupData::IsFast(fast_det_name)) continue;
      //      std::cout << fast_det_name << " is a fast channel and I will now find the slow one" << std::endl;
      std::string detname = fast_det_name.substr(0, fast_det_name.size() - 2);
      std::string slow_det_name = detname + "-S"; // take off the F and add an S

      std::map<std::string, std::vector<TAnalysedPulse*> >::iterator findSlowIter= gAnalysedPulseMap.find(slow_det_name);
      if(findSlowIter== gAnalysedPulseMap.end()) std::cout<<"Unable to find slow channel, "<<slow_det_name<<std::endl;
//      for (std::map<std::string, std::vector<TAnalysedPulse*> >::iterator findSlowIter = gAnalysedPulseMap.begin();
//              findSlowIter != gAnalysedPulseMap.end();
//              findSlowIter++) {
//	
//	if (slow_det_name != findSlowIter->first) continue;
	  //	  std::cout << "Found " << findSlowIter->first << ". Now need to match pulses" << std::endl;

	  std::vector<TAnalysedPulse*> fast_pulses = findFastIter->second;
	  std::vector<TAnalysedPulse*> slow_pulses = findSlowIter->second;

	  std::vector<TAnalysedPulse*>::iterator fastPulseIter = fast_pulses.begin();
	  std::vector<TAnalysedPulse*>::iterator slowPulseIter = slow_pulses.begin();
	  std::vector<TAnalysedPulse*>::iterator finalFastPulseIter = fast_pulses.end();
	  std::vector<TAnalysedPulse*>::iterator finalSlowPulseIter = slow_pulses.end();

	  std::vector<std::vector<TAnalysedPulse*>::iterator> pulseIters;
	  pulseIters.push_back(fastPulseIter);
	  pulseIters.push_back(slowPulseIter);

	  std::vector<std::vector<TAnalysedPulse*>::iterator> finalIters;
	  finalIters.push_back(finalFastPulseIter);
	  finalIters.push_back(finalSlowPulseIter);

	  std::vector<TDetectorPulse*> detectorPulses;

	  // Loop through both TAnalysedPulse vectors until they are both finished
	  // NB with this alogirthm this can be extended to more than 2
          double min_time = 999999; // something large
          double pulse_time;
	  while (pulseIters.size() > 0) {

	    // Find out which of the next fast or slow pulsees happened next
	    for (unsigned int b = 0; b < pulseIters.size(); ++b) {
	      pulse_time = (*pulseIters[b])->GetTime() * 1e-6; // convert to ms	      
	      min_time = std::min(min_time, pulse_time);

	      //	      std::cout << b << ": " << pulse_time << " ms\n";
	    }
	    //	    std::cout << "Min Time: " << min_time << std::endl;

	    // Now go through and find all the pulses that are within a certain time of this
	    double time_difference = 0.1; // 0.1 ms
	    TAnalysedPulse* fast_pulse = NULL;
	    TAnalysedPulse* slow_pulse = NULL;
	    for (unsigned int b = 0; b < pulseIters.size(); ++b) {

	      TAnalysedPulse* pulse = *(pulseIters[b]);
	      double pulse_time = pulse->GetTime() * 1e-6; // convert to ms

	      if (std::fabs(pulse_time - min_time) < time_difference) {
		if ( TSetupData::IsFast(pulse->GetDetName())) {
		  PrintOut("Fast? " << pulse->GetDetName() << std::endl);
		  fast_pulse = pulse;
		}
		else if ( TSetupData::IsSlow(pulse->GetDetName()))  {
		  PrintOut( "Slow? " << pulse->GetDetName() << std::endl);
		  slow_pulse = pulse;
		}

		++(pulseIters[b]); // increment the iterator because we used the pulse
	      }
	    }
	    TDetectorPulse* det_pulse = new TDetectorPulse(fast_pulse, slow_pulse, detname); // Create the TDetectorPulse
	    detectorPulses.push_back(det_pulse);
	    PrintOut("Created a TDetectorPulse with:\n");
	    PrintOut("Fast Pulse: " << det_pulse->GetFastPulseTime() * 1e-6 << std::endl);
	    PrintOut( "Slow Pulse: " << det_pulse->GetSlowPulseTime() * 1e-6 << std::endl);
	    PrintOut( std::endl);

	    //Delete the iterators to finished banks. Go through in reverse to
	    //avoid invalidation problems
	    for (int b = pulseIters.size()-1; b >= 0; --b) {
	      if (pulseIters[b] == finalIters[b]){
		pulseIters.erase(pulseIters.begin() + b);
		finalIters.erase(finalIters.begin() + b);
	      }  
	    } // for (int b -reversed)	      
	  } // end for
	  
	  gDetectorPulseMap[detname] = detectorPulses;
      //} // end loop to find corresponding slow channel
  } // end loop to find fast channel
  return 0;
}
