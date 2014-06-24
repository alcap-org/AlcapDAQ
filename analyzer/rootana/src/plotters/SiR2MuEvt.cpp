//#define USE_PRINT_OUT 

#include "SiR2MuEvt.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <cmath>
#include "definitions.h"

#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "RegisterModule.inc"

using std::string;
using std::map;
using std::vector;
using std::pair;

extern StringAnalPulseMap gAnalysedPulseMap;

SiR2MuEvt::SiR2MuEvt(char *HistogramDirectoryName): 
  BaseModule(HistogramDirectoryName)
{ 
  fThreshold_muSc = 240;
  fTimeMargin = 3e3; 
  fPileupProtectionTWindow = 15e3;

  mutree = new TTree("mutree", "mutree");
  mutree->Branch("E_muSc", &fE_muSc);
  mutree->Branch("t_muSc", &ft_muSc);
  mutree->Branch("E_SiR2_S", &fE_SiR2_S);
  mutree->Branch("t_SiR2_S", &ft_SiR2_S);
  mutree->Branch("E_SiR2_F", &fE_SiR2_F);
  mutree->Branch("t_SiR2_F", &ft_SiR2_F);
  dir->cd("/");
}

SiR2MuEvt::SiR2MuEvt(modules::options* opts) : 
  BaseModule( (opts->GetString("0")).c_str() ) 
{
  fThreshold_muSc = 240;
  fTimeMargin = 4e3; 
  fPileupProtectionTWindow = 15e3;

  mutree = new TTree("mutree", "mutree");
  mutree->Branch("E_muSc", &fE_muSc);
  mutree->Branch("t_muSc", &ft_muSc);
  mutree->Branch("E_SiR2_S", &fE_SiR2_S);
  mutree->Branch("t_SiR2_S", &ft_SiR2_S);
  mutree->Branch("E_SiR2_F", &fE_SiR2_F);
  mutree->Branch("t_SiR2_F", &ft_SiR2_F);
  dir->cd("/");
}

SiR2MuEvt::~SiR2MuEvt()
{  
}

int SiR2MuEvt::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){

  std::string fDetNameA  = "muSc";
  std::string fDetNameB = "SiR2-S";
  std::string fDetNameC = "SiR2-F";
  // Get the detA and detB pulses ready but make sure they exist first
  AnalysedPulseList detA_pulses;
  AnalysedPulseList detB_pulses;
  AnalysedPulseList detC_pulses;

  if (gAnalysedPulseMap.find(fDetNameA) == gAnalysedPulseMap.end()) {
    if (Debug())
    std::cout << fDetNameA << " pulses not found" << std::endl;
  }
  else {
    detA_pulses = gAnalysedPulseMap[fDetNameA];
  }

  if (gAnalysedPulseMap.find(fDetNameB) == gAnalysedPulseMap.end()) {
    if (Debug())
    std::cout << fDetNameB << " pulses not found" << std::endl;
  }
  else {
    detB_pulses = gAnalysedPulseMap[fDetNameB];
  }

  if (gAnalysedPulseMap.find(fDetNameC) == gAnalysedPulseMap.end()) {
    if (Debug())
    std::cout << fDetNameC << " pulses not found" << std::endl;
  }
  else {
    detC_pulses = gAnalysedPulseMap[fDetNameC];
  }

  // want to keep track of how far we are through the detA pulses
  AnalysedPulseList::iterator currentDetAPulse = detA_pulses.begin(); 
  AnalysedPulseList::iterator currentDetBPulse = detB_pulses.begin(); 
  AnalysedPulseList::iterator currentDetCPulse = detC_pulses.begin(); 

  // Loop through the muSc pulses
  for (AnalysedPulseList::iterator detAPulseIter = currentDetAPulse; 
      detAPulseIter != detA_pulses.end(); ++detAPulseIter) 
  {
    if ((*detAPulseIter)->GetAmplitude()>=fThreshold_muSc)
    {
      ft_muSc = (*detAPulseIter)->GetTime();
      fE_muSc = (*detAPulseIter)->GetAmplitude();
      // Skip detB pulses until ft_muSc
      while ((currentDetBPulse != detB_pulses.end()) 
          &&((*currentDetBPulse)->GetTime() < (ft_muSc - fTimeMargin)))
        currentDetBPulse++;

      // Loop through the detB pulses
      AnalysedPulseList::iterator detBPulseIter = currentDetBPulse;
      AnalysedPulseList::iterator detCPulseIter = currentDetCPulse;
      // if this is the last detA pulse, don't check for upper bound
      if (detAPulseIter == detA_pulses.end()-1)
      {
        while (detBPulseIter != detB_pulses.end())
        {
          double detB_time = (*detBPulseIter)->GetTime();
          double detB_amplitude = (*detBPulseIter)->GetAmplitude();
          fE_SiR2_S.push_back(detB_amplitude);
          ft_SiR2_S.push_back(detB_time);
          detBPulseIter++;
        }
        while (detCPulseIter != detC_pulses.end())
        {
          double detC_time = (*detCPulseIter)->GetTime();
          double detC_amplitude = (*detCPulseIter)->GetAmplitude();
          fE_SiR2_F.push_back(detC_amplitude);
          ft_SiR2_F.push_back(detC_time);
          detCPulseIter++;
        }
      }
      else
       // not the last pulse, check the next
      {
        double ft_muSc_next = (*(detAPulseIter + 1))->GetTime();
        // check that the next pulse is not in to pile up protection window
        if (ft_muSc_next - ft_muSc > fPileupProtectionTWindow)
        {
          while ((detBPulseIter != detB_pulses.end()) &&
              ((*detBPulseIter)->GetTime()<(ft_muSc_next - fTimeMargin)))
          {
            double detB_time = (*detBPulseIter)->GetTime();
            double detB_amplitude = (*detBPulseIter)->GetAmplitude();
            fE_SiR2_S.push_back(detB_amplitude);
            ft_SiR2_S.push_back(detB_time);
            detBPulseIter++;
          }
          while ((detCPulseIter != detC_pulses.end()) &&
              ((*detCPulseIter)->GetTime()<(ft_muSc_next - fTimeMargin)))
          {
            double detC_time = (*detCPulseIter)->GetTime();
            double detC_amplitude = (*detCPulseIter)->GetAmplitude();
            fE_SiR2_F.push_back(detC_amplitude);
            ft_SiR2_F.push_back(detC_time);
            detCPulseIter++;
          }
        }
      }

      // Filling tree, clear vectors for next event
      mutree->Fill();
      fE_SiR2_S.clear();
      ft_SiR2_S.clear();
      fE_SiR2_F.clear();
      ft_SiR2_F.clear();
    } 
  }
  return 0;
}

bool SiR2MuEvt::IsMuonHit(TAnalysedPulse * muScPulse)
{
  return (muScPulse->GetAmplitude()>240);
}

ALCAP_REGISTER_MODULE(SiR2MuEvt)
