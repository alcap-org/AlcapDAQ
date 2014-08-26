#include "FastSlowCompare.h"
#include "RegisterModule.inc"
#include "TPulseIsland.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TAnalysedPulse.h"
#include "ModulesOptions.h"
#include "IdSource.h"
#include "definitions.h"

#include "TH1I.h"
#include "TH2I.h"
#include "TDirectory.h"

#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <climits>
#include <map>
#include <string>
#include <sstream>

extern SourceAnalPulseMap gAnalysedPulseMap;


static std::vector<double> TAP2Times(const std::vector<TAnalysedPulse*>& v) {
  std::vector<double> t;
  for (unsigned int i = 0; i < v.size(); ++i)
    t.push_back(v[i]->GetTime());
  return t;
}

FastSlowCompare::FastSlowCompare(modules::options* opts): BaseModule("FastSlowCompare",opts) {
}


// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int FastSlowCompare::BeforeFirstEntry(TGlobalData* gData, const TSetupData *setup){
  // Build up list of slow and fast sources
  std::vector<IDs::source> slows, fasts;
  for (std::map<IDs::source, std::vector<TAnalysedPulse*> >::const_iterator i = gAnalysedPulseMap.begin();
       i != gAnalysedPulseMap.end();
       ++i) {
    if (i->first.Channel().isSlow())
      slows.push_back(i->first);
    else if (i->first.Channel().isFast())
      fasts.push_back(i->first);
  }

  // Pair them up if they're of the same detector
  for (unsigned int s = 0; s < slows.size(); ++s)
    for (unsigned int f = 0; f < fasts.size(); ++f)
      if (slows[s].Channel().Detector() == fasts[f].Channel().Detector())
	fPairs.push_back(std::pair<IDs::source, IDs::source>(slows[s], fasts[f]));

  BookHistograms();
  
  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int FastSlowCompare::ProcessEntry(TGlobalData* gData, const TSetupData *setup){

  for (unsigned int i = 0; i < fPairs.size(); ++i) {
    const std::vector<double> fasts = TAP2Times(gAnalysedPulseMap[fPairs[i].first]);
    const std::vector<double> slows = TAP2Times(gAnalysedPulseMap[fPairs[i].second]);

    for (std::vector<double>::const_iterator slow = slows.begin(); slow < slows.end(); ++slow) {
      std::vector<double>::const_iterator fast[2] = {std::lower_bound(fasts.begin(), fasts.end(), *slow - 100000.),
						     std::upper_bound(fasts.begin(), fasts.end(), *slow + 100000.)};
      fHist_NPerSlowWide[i]->Fill(fast[1] - fast[0]);
      for(std::vector<double>::const_iterator iTime = fast[0]; iTime < fast[1]; ++iTime) {
	fHist_Time[i]->Fill(*iTime - *slow);
	fHist_MoreTime[i]->Fill(*iTime - *slow);
      }
      fHist_NPerSlowTight[i]->Fill(std::lower_bound(fasts.begin(), fasts.end(), *slow + 400.) -
					      std::upper_bound(fasts.begin(), fasts.end(), *slow - 400.));
    }
  }


  return 0;
}


// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int FastSlowCompare::AfterLastEntry(TGlobalData* gData, const TSetupData *setup) {
  return 0;
}


void FastSlowCompare::BookHistograms() {
  for (unsigned int i = 0; i < fPairs.size(); ++i) {
    std::stringstream name, title;
    std::string tail(fPairs[i].first.str() + IDs::field_separator + fPairs[i].second.str());
    name << "hTime" << tail;
    title << "Relative Time of Fast (" << tail << ")";
    fHist_Time.push_back(new TH1I(name.str().c_str(), title.str().c_str(), 400, -200., 200.));
    name.str(""); name << "hMoreTime" << tail;
    title.str(""); title << "Relative Time of Fast (" << tail << ")";
    fHist_MoreTime.push_back(new TH1I(name.str().c_str(), title.str().c_str(), 1000, -100000., 100000.));
    name.str(""); name << "hNPerSlowWide" << tail;
    title.str(""); title << "Number of Fast Pulses within 100us (" << tail << ")";
    fHist_NPerSlowWide.push_back(new TH1I(name.str().c_str(), title.str().c_str(), 10, -0.5, 9.5));
    name.str(""); name << "hNPerSlowTight" << tail;
    title.str(""); title << "Number of Fast Pulses within 400ns (" << tail << ")";
    fHist_NPerSlowTight.push_back(new TH1I(name.str().c_str(), title.str().c_str(), 10, -0.5, 9.5));
  }
}


// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(FastSlowCompare);
