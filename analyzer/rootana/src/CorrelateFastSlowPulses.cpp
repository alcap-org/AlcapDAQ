#include "CorrelateFastSlowPulses.h"
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

std::map<std::string, TH1F*> time_corrs;

CorrelateFastSlowPulses::CorrelateFastSlowPulses(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){  
  dir->cd("/");
}

CorrelateFastSlowPulses::~CorrelateFastSlowPulses(){  
}

int CorrelateFastSlowPulses::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  // Loop through and find a fast channel
  //  std::cout << "Size of gAnalysedPulseMap " << gAnalysedPulseMap.size() << std::endl;
  for (std::map<std::string, std::vector<TAnalysedPulse*> >::iterator findFastIter = gAnalysedPulseMap.begin(); findFastIter != gAnalysedPulseMap.end(); findFastIter++) {

    std::string fast_det_name = findFastIter->first;
    //    std::cout << "DetName; " << fast_det_name << std::endl;
    if ( *(fast_det_name.end()-1) == 'F') {
      //      std::cout << fast_det_name << " is a fast channel and I will now find the slow one" << std::endl;
      std::string detname = fast_det_name.substr(0, fast_det_name.size() - 2);
      std::string slow_det_name = detname + "-S"; // take off the F and add an S

      for (std::map<std::string, std::vector<TAnalysedPulse*> >::iterator findSlowIter = gAnalysedPulseMap.begin(); findSlowIter != gAnalysedPulseMap.end(); findSlowIter++) {
	
	if (slow_det_name == findSlowIter->first) {
	  //	  std::cout << "Found " << findSlowIter->first << ". Now need to match pulses" << std::endl;

	  // Create the histogram if it's not been created yet
	  if ( time_corrs.find(detname) == time_corrs.end() ) {

	    // hTimeCorrelation
	    std::string histname = "h" + detname + "_FastSlowTimeCorrelation";
	    std::string histtitle = "Plot of t_{F} - t_{S} in the " + detname + " detector";
	    TH1F* hTimeCorrelation = new TH1F(histname.c_str(), histtitle.c_str(), 1e4,-5e4,5e4);
	    hTimeCorrelation->GetXaxis()->SetTitle("t_{F} - t_{S} [ns]");
	    hTimeCorrelation->GetYaxis()->SetTitle("Arbitrary Unit");
	    time_corrs[detname] = hTimeCorrelation;
	  }

	  std::vector<TAnalysedPulse*> fast_pulses = findFastIter->second;
	  std::vector<TAnalysedPulse*> slow_pulses = findSlowIter->second;

	  // Loop through both sets of pulses and plot the time difference
	  for (std::vector<TAnalysedPulse*>::iterator fastPulseIter = fast_pulses.begin(); fastPulseIter != fast_pulses.end(); ++fastPulseIter) {
	    for (std::vector<TAnalysedPulse*>::iterator slowPulseIter = slow_pulses.begin(); slowPulseIter != slow_pulses.end(); ++slowPulseIter) {
	      double fast_pulse_time = (*fastPulseIter)->GetTime();
	      double slow_pulse_time = (*slowPulseIter)->GetTime();
	      time_corrs[detname]->Fill(fast_pulse_time - slow_pulse_time);
	    
	    } // end loop through slow pulses
	  } // end loop through fast pulses
	  
	} // end if slow channel found
      } // end loop to find corresponding slow channel
    } // end if fast channel
  } // end loop to find fast channel
  return 0;
}
