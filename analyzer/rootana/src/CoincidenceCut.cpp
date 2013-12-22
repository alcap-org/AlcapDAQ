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

CoincidenceCut::CoincidenceCut(char *HistogramDirectoryName, std::string det_name_a, std::string det_name_b, double start_window, double stop_window) :
  FillHistBase(HistogramDirectoryName){ 

  fDetNameA = det_name_a;
  fDetNameB = det_name_b;
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

  // Get the detA pulses ready for later
  std::vector<TAnalysedPulse*> detA_pulses = gAnalysedPulseMap[fDetNameA];
  std::vector<TAnalysedPulse*>& detB_pulses = gAnalysedPulseMap[fDetNameB];
  

  std::vector<TAnalysedPulse*>::iterator currentDetAPulse = detA_pulses.begin(); // want to keep track of how far we are through the detA pulses

  // Loop through the detB pulses
  for (std::vector<TAnalysedPulse*>::iterator detBPulseIter = detB_pulses.begin(); detBPulseIter != detB_pulses.end(); ++detBPulseIter) {

    // Loop through the detA pulses
    bool coinc_found = false;
    for (std::vector<TAnalysedPulse*>::iterator detAPulseIter = currentDetAPulse; detAPulseIter != detA_pulses.end(); ++detAPulseIter) {

      double detB_time = (*detBPulseIter)->GetTime();
      double detA_time = (*detAPulseIter)->GetTime();
      double t_diff = detB_time - detA_time;

      if (t_diff > fStartWindow && t_diff < fStopWindow) { // if within the window
	coinc_found = true;
	break; // no need to go through the detA pulses any more
      }
      // We should be time-order here and so if we go past the time for this pulse without finding a match we can quit
      if (detA_time > detB_time) {
	coinc_found = false;
	break;
      }
      currentDetAPulse = detAPulseIter; // record where we are up to 
    } // end loop through detA pulses

      // If no coincidence was found
    if (coinc_found == false) {
      delete *detBPulseIter;
      detB_pulses.erase(detBPulseIter);
      --detBPulseIter;
    }

  } // end loop through detB pulses

  return 0;
}
