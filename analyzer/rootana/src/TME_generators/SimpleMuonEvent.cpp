#include "SimpleMuonEvent.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <cmath>
#include "debug_tools.h"

#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "RegisterModule.inc"
#include "ModulesParser.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

extern SourceAnalPulseMap gAnalysedPulseMap;

SimpleMuonEvent::SimpleMuonEvent(modules::options* opts):
  BaseModule("SimpleMuonEvent", opts, false)
{ 
  dir->cd("/"); 

  fDetNames["LaBr3"] = "LaBr3";
  fDetNames["TSc"] = "TSc";

  fTimeWindow = opts->GetDouble("time_window", 10.E3);
  fCoinWindow = opts->GetDouble("coin_window", 1.E3);
  fPileupProtectionWindow = opts->GetDouble("pileup_protection_window", 15.E3);
  fThreshold_TSc = opts->GetDouble("musc_threshold", 240.);
  fDebug = opts->GetBool("debug", false);

  if (fDebug) {
    std::cout << "SimpleMuonEvent:" << std::endl;
    std::cout << "\t- time_window: " << fTimeWindow << std::endl;
    std::cout << "\t- coin_window: " << fCoinWindow << std::endl;
    std::cout << "\t- pileup_protection_window: " << fPileupProtectionWindow << std::endl;
    std::cout << "\t- musc_threshold: " << fThreshold_TSc << std::endl;
  }
}

SimpleMuonEvent::~SimpleMuonEvent() { }

#define DeclareBranch(det)\
  sprintf(branchname, "E_%s", #det);\
  mutree->Branch(branchname, &fE_##det);\
  sprintf(branchname, "t_%s", #det);\
  mutree->Branch(branchname, &ft_##det);


int SimpleMuonEvent::BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup)
{
  char branchname[100];
  mutree = new TTree("mutree", "mutree");
  DeclareBranch(TSc)
  DeclareBranch(LaBr3)

  return 0;
}
#undef DeclareBranch

#define DeclarePulseList(detector)\
  tmpstr = #detector;\
  AnalysedPulseList  pulses_##detector = GetDetectorPulse(fDetNames[tmpstr]);

#define DetectorLoop(det)\
  while ((current ## det ## pulse != pulses_ ## det.end()) \
      && (*current ## det ## pulse)->GetTime()  < ft_TSc - fTimeWindow )\
        current ## det ## pulse++;\
  AnalysedPulseList::iterator iter ## det;\
  iter ## det = current ## det ## pulse;\
  while (iter ## det != pulses_ ## det.end())\
  {\
    if ((*iter ## det)->GetTime() < ft_TSc + fTimeWindow )\
    {\
      ft_##det.push_back((*iter ## det)->GetTime());\
      fE_##det.push_back((*iter ## det)->GetAmplitude());\
    }\
    iter##det++;\
  }


#define ClearDetectorVector(det)\
  ft_##det.clear();\
  fE_##det.clear();


int SimpleMuonEvent::ProcessEntry(TGlobalData *gData, const TSetupData *gSetup)
{
  std::string tmpstr;
  DeclarePulseList(TSc)
  DeclarePulseList(LaBr3)

  // Make a list of muon hits: over threshold
  AnalysedPulseList muCandidates;

  for (AnalysedPulseList::iterator TScIter = pulses_TSc.begin(); 
      TScIter != pulses_TSc.end(); ++TScIter) 
  {
    if (IsMuonHit(*TScIter))
      muCandidates.push_back(*TScIter);
  }

  // Pile-up protection
  AnalysedPulseList muHits;
  for (AnalysedPulseList::iterator muIter = muCandidates.begin(); 
      muIter+1 < muCandidates.end(); ++muIter)
  {
    double delta_t = (*(muIter + 1))->GetTime() - (*muIter)->GetTime();
    if (delta_t > fPileupProtectionWindow)
      muHits.push_back(*muIter);
    else
      muIter ++; // skip the next event, too
  }

  // Keep track of pulse iterators
  AnalysedPulseList::iterator currentLaBr3pulse = pulses_LaBr3.begin();

  // Loop over muon hits
  for (AnalysedPulseList::iterator muIter = muHits.begin(); 
      muIter+1 < muHits.end(); ++muIter)
  {
    ft_TSc = (*muIter)->GetTime();
    fE_TSc = (*muIter)->GetAmplitude();

    // skip this det until the lower bound of the time window
    while ((currentLaBr3pulse != pulses_LaBr3.end()) 
      //&& (*currentLaBr3pulse)->GetTime() + 7500 < ft_TSc - fTimeWindow )
      && (*currentLaBr3pulse)->GetTime() < ft_TSc - fTimeWindow )
        currentLaBr3pulse++;

    AnalysedPulseList::iterator iterLaBr3;
    iterLaBr3 = currentLaBr3pulse;
    while (iterLaBr3 != pulses_LaBr3.end())
    {
      //if ((*iterLaBr3)->GetTime() + 7.5e3< ft_TSc + fTimeWindow )
      if ((*iterLaBr3)->GetTime() < ft_TSc + fTimeWindow )
      {
        ft_LaBr3.push_back((*iterLaBr3)->GetTime());
        fE_LaBr3.push_back((*iterLaBr3)->GetAmplitude());
      }
      iterLaBr3++;
    }

    // DetectorLoop(SiR2S) 
    // DetectorLoop(SiL2S)
    // DetectorLoop(SiR2F) 
    // DetectorLoop(SiL2F)
    // finally fill the tree
    mutree->Fill();
    ClearDetectorVector(LaBr3);

  }

  return 0;
}

#undef DetectorLoop
#undef DeclarePulseList
#undef ClearDetectorVector

bool SimpleMuonEvent::IsMuonHit(TAnalysedPulse * TScPulse)
{
  return (TScPulse->GetAmplitude() > fThreshold_TSc);
}

AnalysedPulseList SimpleMuonEvent::GetDetectorPulse(string detname)
{
  AnalysedPulseList pulse_list;

  for (SourceAnalPulseMap::const_iterator sourceIt2 = gAnalysedPulseMap.begin();
      sourceIt2 != gAnalysedPulseMap.end(); ++sourceIt2)
  {
    if (sourceIt2->first.Channel() != detname) {
      continue;
    }
    pulse_list = sourceIt2->second;
  }
  return pulse_list;
}

ALCAP_REGISTER_MODULE(SimpleMuonEvent, time_window, coin_window,
    pileup_protection_window, musc_threshold, debug)
