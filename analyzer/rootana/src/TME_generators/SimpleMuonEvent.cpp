#include "SimpleMuonEvent.h"
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
#include "RegisterModule.inc"
#include "ModulesParser.h"

using std::string;
using std::map;
using std::vector;
using std::pair;
using modules::parser::GetOneWord;
using modules::parser::GetDouble;


extern StringAnalPulseMap gAnalysedPulseMap;

SimpleMuonEvent::SimpleMuonEvent(modules::options* opts):
  BaseModule( (opts->GetString("0")).c_str() ) 
{ 
  dir->cd("/"); 

  fDetNames["GeS"] = "Ge-S";
  fDetNames["muSc"] = "muSc";
  fDetNames["SiR2S"] = "SiR2-S";
  fDetNames["SiL2S"] = "SiL2-S";
  fDetNames["SiR2F"] = "SiR2-F";
  fDetNames["SiL2F"] = "SiL2-F";
  fDetNames["SiL11S"] = "SiL1-1-S";
  fDetNames["SiL12S"] = "SiL1-2-S";
  fDetNames["SiL13S"] = "SiL1-3-S";
  fDetNames["SiL14S"] = "SiL1-4-S";
  fDetNames["SiR11S"] = "SiR1-1-S";
  fDetNames["SiR12S"] = "SiR1-2-S";
  fDetNames["SiR13S"] = "SiR1-3-S";
  fDetNames["SiR14S"] = "SiR1-4-S";

  fTimeWindow = 
    (opts->GetString("1")!= "") ? GetDouble(opts->GetString("1")) : 10.e3;

  fCoinWindow = 
    (opts->GetString("2")!= "") ? GetDouble(opts->GetString("2")) : 1.e3;

  fPileupProtectionWindow = 
    (opts->GetString("3")!= "") ? GetDouble(opts->GetString("3")) : 15.e3;

  fThreshold_muSc = 
    (opts->GetString("4")!= "") ? GetDouble(opts->GetString("4")) : 240.;
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
  DeclareBranch(muSc)
  DeclareBranch(SiR2S)
  DeclareBranch(SiL2S)
  DeclareBranch(SiR2F)
  DeclareBranch(SiL2F)

  DeclareBranch(GeS)
  DeclareBranch(SiR11S)
  DeclareBranch(SiR12S)
  DeclareBranch(SiR13S)
  DeclareBranch(SiR14S)

  DeclareBranch(SiL11S)
  DeclareBranch(SiL12S)
  DeclareBranch(SiL13S)
  DeclareBranch(SiL14S)

  return 0;
}
#undef DeclareBranch

#define DeclarePulseList(detector)\
  tmpstr = #detector;\
  AnalysedPulseList  pulses_##detector = GetDetectorPulse(fDetNames[tmpstr]);

#define DetectorLoop(det)\
  while ((current ## det ## pulse != pulses_ ## det.end()) \
      && (*current ## det ## pulse)->GetTime()  < ft_muSc - fTimeWindow )\
        current ## det ## pulse++;\
  AnalysedPulseList::iterator iter ## det;\
  iter ## det = current ## det ## pulse;\
  while (iter ## det != pulses_ ## det.end())\
  {\
    if ((*iter ## det)->GetTime() < ft_muSc + fTimeWindow )\
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
  DeclarePulseList(muSc)
  DeclarePulseList(SiR2S)
  DeclarePulseList(SiL2S)
  DeclarePulseList(SiR2F)
  DeclarePulseList(SiL2F)

  DeclarePulseList(GeS)
  DeclarePulseList(SiR11S)
  DeclarePulseList(SiR12S)
  DeclarePulseList(SiR13S)
  DeclarePulseList(SiR14S)

  DeclarePulseList(SiL11S)
  DeclarePulseList(SiL12S)
  DeclarePulseList(SiL13S)
  DeclarePulseList(SiL14S)

  // Make a list of muon hits: over threshold
  AnalysedPulseList muCandidates;

  for (AnalysedPulseList::iterator muScIter = pulses_muSc.begin(); 
      muScIter != pulses_muSc.end(); ++muScIter) 
  {
    if (IsMuonHit(*muScIter))
      muCandidates.push_back(*muScIter);
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
  AnalysedPulseList::iterator currentGeSpulse = pulses_GeS.begin();
  AnalysedPulseList::iterator currentSiR2Spulse = pulses_SiR2S.begin();
  AnalysedPulseList::iterator currentSiL2Spulse = pulses_SiL2S.begin();
  AnalysedPulseList::iterator currentSiR2Fpulse = pulses_SiR2F.begin();
  AnalysedPulseList::iterator currentSiL2Fpulse = pulses_SiL2F.begin();
  AnalysedPulseList::iterator currentSiR11Spulse = pulses_SiR11S.begin();
  AnalysedPulseList::iterator currentSiR12Spulse = pulses_SiR12S.begin();
  AnalysedPulseList::iterator currentSiR13Spulse = pulses_SiR13S.begin();
  AnalysedPulseList::iterator currentSiR14Spulse = pulses_SiR14S.begin();
  AnalysedPulseList::iterator currentSiL11Spulse = pulses_SiL11S.begin();
  AnalysedPulseList::iterator currentSiL12Spulse = pulses_SiL12S.begin();
  AnalysedPulseList::iterator currentSiL13Spulse = pulses_SiL13S.begin();
  AnalysedPulseList::iterator currentSiL14Spulse = pulses_SiL14S.begin();

  // Loop over muon hits
  for (AnalysedPulseList::iterator muIter = muHits.begin(); 
      muIter+1 < muHits.end(); ++muIter)
  {
    ft_muSc = (*muIter)->GetTime();
    fE_muSc = (*muIter)->GetAmplitude();

    // skip this det until the lower bound of the time window
    while ((currentGeSpulse != pulses_GeS.end()) 
      //&& (*currentGeSpulse)->GetTime() + 7500 < ft_muSc - fTimeWindow )
      && (*currentGeSpulse)->GetTime() < ft_muSc - fTimeWindow )
        currentGeSpulse++;

    AnalysedPulseList::iterator iterGeS;
    iterGeS = currentGeSpulse;
    while (iterGeS != pulses_GeS.end())
    {
      //if ((*iterGeS)->GetTime() + 7.5e3< ft_muSc + fTimeWindow )
      if ((*iterGeS)->GetTime() < ft_muSc + fTimeWindow )
      {
        ft_GeS.push_back((*iterGeS)->GetTime());
        fE_GeS.push_back((*iterGeS)->GetAmplitude());
      }
      iterGeS++;
    }

    DetectorLoop(SiR2S) 
    DetectorLoop(SiL2S)
    DetectorLoop(SiR2F) 
    DetectorLoop(SiL2F)
    DetectorLoop(SiR11S)
    DetectorLoop(SiR12S)
    DetectorLoop(SiR13S)
    DetectorLoop(SiR14S)
    DetectorLoop(SiL11S)
    DetectorLoop(SiL12S)
    DetectorLoop(SiL13S)
    DetectorLoop(SiL14S)
    // finally fill the tree
    mutree->Fill();
    ClearDetectorVector(GeS);
    ClearDetectorVector(SiR2S);
    ClearDetectorVector(SiL2S)
    ClearDetectorVector(SiR2F);
    ClearDetectorVector(SiL2F)
    ClearDetectorVector(SiR11S)
    ClearDetectorVector(SiR12S)
    ClearDetectorVector(SiR13S)
    ClearDetectorVector(SiR14S)
    ClearDetectorVector(SiL11S)
    ClearDetectorVector(SiL12S)
    ClearDetectorVector(SiL13S)
    ClearDetectorVector(SiL14S)

  }

  return 0;
}

#undef DetectorLoop
#undef DeclarePulseList
#undef ClearDetectorVector

bool SimpleMuonEvent::IsMuonHit(TAnalysedPulse * muScPulse)
{
  return (muScPulse->GetAmplitude()>240);
}

AnalysedPulseList SimpleMuonEvent::GetDetectorPulse(string detname)
{
  AnalysedPulseList pulse_list;
  if (gAnalysedPulseMap.find(detname) == gAnalysedPulseMap.end()) {
    if (Debug())
      std::cout << detname << " pulses not found" << std::endl;
  }
  else {
      pulse_list =  gAnalysedPulseMap[detname];
  }
  return pulse_list;
}

ALCAP_REGISTER_MODULE(SimpleMuonEvent)
