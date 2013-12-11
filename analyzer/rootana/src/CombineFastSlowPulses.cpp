#include "CombineFastSlowPulses.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>

#include "TAnalysedPulse.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;

CombineFastSlowPulses::CombineFastSlowPulses(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){  
}

CombineFastSlowPulses::~CombineFastSlowPulses(){  
}

int CombineFastSlowPulses::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  // Loop through and find a fast channel
  std::cout << "Size of gAnalysedPulseMap " << gAnalysedPulseMap.size() << std::endl;
  for (std::map<std::string, std::vector<TAnalysedPulse*> >::iterator findFastIter = gAnalysedPulseMap.begin(); findFastIter != gAnalysedPulseMap.end(); findFastIter++) {

    std::string fast_det_name = findFastIter->first;
    std::cout << "DetName; " << fast_det_name << std::endl;
    if ( *(fast_det_name.end()-1) == 'F') {
      std::cout << fast_det_name << " is a fast channel and I will now find the slow one" << std::endl;
      std::string slow_det_name = fast_det_name.substr(0, fast_det_name.size() - 1) + 'S'; // take off the F and add an S
      for (std::map<std::string, std::vector<TAnalysedPulse*> >::iterator findSlowIter = gAnalysedPulseMap.begin(); findSlowIter != gAnalysedPulseMap.end(); findSlowIter++) {
	
	if (slow_det_name == findSlowIter->first) {
	  std::cout << "Found " << findSlowIter->first << ". Now need to match pulses" << std::endl;

	  std::vector<TAnalysedPulse*> fast_pulses = findFastIter->second;
	  std::vector<TAnalysedPulse*> slow_pulses = findSlowIter->second;
	  std::cout << "N(fast) = " << fast_pulses.size() << ", N(slow) = " << slow_pulses.size() << ", diff = " << (int)fast_pulses.size() 
- (int)slow_pulses.size() << std::endl;
	  // Loop through the fast pulses
	  for (std::vector<TAnalysedPulse*>::iterator fastPulseIter = fast_pulses.begin(); fastPulseIter != fast_pulses.end(); fastPulseIter++) {
	    double fast_pulse_time = (*fastPulseIter)->GetTime();
	    std::cout << "Pulse #" << fastPulseIter - fast_pulses.begin() << " happened at " << fast_pulse_time << std::endl;
	  }

	}
      }
    }
  
    std::cout << std::endl;
  }
  return 0;
}
