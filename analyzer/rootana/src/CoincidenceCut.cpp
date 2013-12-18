//#define USE_PRINT_OUT 

#include "CoincidenceCut.h"
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

CoincidenceCut::CoincidenceCut(char *HistogramDirectoryName, std::string corr_det_name, double start_window, double stop_window) :
  FillHistBase(HistogramDirectoryName){ 

  fCorrDetName = corr_det_name;
  fStartWindow = start_window;
  fStopWindow = stop_window;
  dir->cd("/");
}

CoincidenceCut::~CoincidenceCut(){  
}

int CoincidenceCut::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  // Get the corrDet pulses ready for later
  std::vector<TAnalysedPulse*> corrDet_pulses = gAnalysedPulseMap[fCorrDetName];
  
  // Loop through the detectors
  //  std::cout << "Size of gAnalysedPulseMap " << gAnalysedPulseMap.size() << std::endl;
  for (std::map<std::string, std::vector<TAnalysedPulse*> >::iterator detIter = gAnalysedPulseMap.begin(); detIter != gAnalysedPulseMap.end(); detIter++) {

    std::string detname = detIter->first;
    std::vector<TAnalysedPulse*>& pulses = detIter->second;
    std::vector<TAnalysedPulse*>::iterator currentCorrDetPulse = corrDet_pulses.begin(); // want to keep track of how far we are through the corrDet pulses

    // Loop through the detector pulses
    for (std::vector<TAnalysedPulse*>::iterator pulseIter = pulses.begin(); pulseIter != pulses.end(); ++pulseIter) {

      // Loop through the corrDet pulses
      bool coinc_found = false;
      for (std::vector<TAnalysedPulse*>::iterator corrDetPulseIter = currentCorrDetPulse; corrDetPulseIter != corrDet_pulses.end(); ++corrDetPulseIter) {

	double time = (*pulseIter)->GetTime();
	double corrDet_time = (*corrDetPulseIter)->GetTime();
	double t_diff = time - corrDet_time;
	//	std::cout << fCorrDetName << " #" << (corrDetPulseIter - corrDet_pulses.begin()) <<  ": " << corrDet_time * 1e-6 << " ms, " << detname << " #" << (pulseIter - pulses.begin()) << ": " << time * 1e-6 << std::endl;
	if (t_diff > fStartWindow && t_diff < fStopWindow) { // if within the window
	  coinc_found = true;
	  break; // no need to go through the corrDet pulses any more
	}
	// We should be time-order here and so if we go past the time for this pulse without finding a match we can quit
	if (corrDet_time > time) {
	  coinc_found = false;
	  break;
	}
	currentCorrDetPulse = corrDetPulseIter; // record where we are up to 
      } // end loop through corrDet pulses

      // If no coincidence was found
      if (coinc_found == false) {
	delete *pulseIter;
	pulses.erase(pulseIter);
	--pulseIter;
      }

    } // end loop through pulses
  } // end loop through detectors
  return 0;
}
