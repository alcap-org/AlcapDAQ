#include "SiR2Target.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TAnalysedPulse.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <cmath>

#include "TH1I.h"

extern std::map< std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;

SiR2Target::SiR2Target(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName) {

  ftCoincidence = 2000.;
  fhSpectrum = new TH1I("hSiR2Spectrum","Spectrum of Silicon Target", 26000, 0., 26000.);

  gDirectory->cd("/");
}

SiR2Target::~SiR2Target(){
}

int SiR2Target::ProcessEntry(TGlobalData *gData, TSetupData *gSetup) {

  std::vector<TAnalysedPulse*>* pulses_musc = NULL;
  std::vector<TAnalysedPulse*>* pulses_targ = NULL;
  std::vector<TAnalysedPulse*>* pulses_thick = NULL;
  std::vector<TAnalysedPulse*>* pulses_thin1 = NULL;
  std::vector<TAnalysedPulse*>* pulses_thin2 = NULL;
  std::vector<TAnalysedPulse*>* pulses_thin3 = NULL;
  std::vector<TAnalysedPulse*>* pulses_thin4 = NULL;

  // Find the pulses if they exist
  if (gAnalysedPulseMap.count("muSc") == 0) {
    std::cout << "INFO: No beam counter hits" << std::endl;
    return 0;
  } else if (gAnalysedPulseMap.count("SiR2-S") == 0) {
    std::cout << "INFO: No target hits" << std::endl;
    return 0;
  } else if (gAnalysedPulseMap.count("SiL2-S") == 0) {
    std::cout << "INFO: No thick external hits" << std::endl;
    return 0;
  }
  pulses_musc = &gAnalysedPulseMap.at("muSc");
  pulses_targ = &gAnalysedPulseMap.at("SiR2-S");
  pulses_thick = &gAnalysedPulseMap.at("SiL2-S");
  
  if (gAnalysedPulseMap.count("SiL1-1-S"))
    pulses_thin1 = &gAnalysedPulseMap.at("SiL1-1-S");
  if (gAnalysedPulseMap.count("SiL1-2-S"))
    pulses_thin2 = &gAnalysedPulseMap.at("SiL1-2-S");
  if (gAnalysedPulseMap.count("SiL1-3-S"))
    pulses_thin3 = &gAnalysedPulseMap.at("SiL1-3-S");
  if (gAnalysedPulseMap.count("SiL1-4-S"))
    pulses_thin4 = &gAnalysedPulseMap.at("SiL1-4-S");
  if(!(pulses_thin1 || pulses_thin2 || pulses_thin3 || pulses_thin4)) {
    std::cout << "INFO: No thin external hits" << std::endl;
    return 0;
  }

  std::vector<TAnalysedPulse*>::iterator iMusc;
  std::vector<TAnalysedPulse*>::iterator iTarg;
  std::vector<TAnalysedPulse*>::iterator iThick;
  std::vector<TAnalysedPulse*>::iterator iThin1;
  std::vector<TAnalysedPulse*>::iterator iThin2;
  std::vector<TAnalysedPulse*>::iterator iThin3;
  std::vector<TAnalysedPulse*>::iterator iThin4;

  iMusc = pulses_musc->begin();
  iTarg = pulses_targ->begin();
  iThick = pulses_thick->begin();
  if (pulses_thin1)
    iThin1 = pulses_thin1->begin();
  if (pulses_thin2)
    iThin2 = pulses_thin2->begin();
  if (pulses_thin3)
    iThin3 = pulses_thin3->begin();
  if (pulses_thin4)
    iThin4 = pulses_thin4->begin();

  double e, t;
  for (; iMusc != pulses_musc->end(); ++iMusc) {
    e = 0.;
    t = (*iMusc)->GetTime();
    // Advance target and external detectors to time window, and
    // then check that we haven't run out of pulses.
    for (; iTarg != pulses_targ->end(); ++iTarg)
      if ((*iTarg)->GetTime() - t >= -ftCoincidence)
	break;
    for (; iThick != pulses_thick->end(); ++iThick)
      if ((*iThick)->GetTime() - t >= -ftCoincidence)
	break;
    if (pulses_thin1)
      for (; iThin1 != pulses_thin1->end(); ++iThin1)
	if ((*iThin1)->GetTime() - t >= -ftCoincidence)
	  break;
    if (pulses_thin2)
      for (; iThin2 != pulses_thin2->end(); ++iThin2)
	if ((*iThin2)->GetTime() - t >= -ftCoincidence)
	  break;
    if (pulses_thin3)
      for (; iThin3 != pulses_thin3->end(); ++iThin3)
	if ((*iThin3)->GetTime() - t >= -ftCoincidence)
	  break;
    if (pulses_thin4)
      for (; iThin4 != pulses_thin4->end(); ++iThin4)
	if ((*iThin4)->GetTime() - t >= -ftCoincidence)
	  break;
    // Check we didn't run out of pulses
    if (iTarg == pulses_targ->end()) {
      break;
    } else if ((*iTarg)->GetTime() - t < ftCoincidence) {
      e = (*iTarg)->GetEnergy();
      ++iTarg;
    } else {
      continue;
    }
    if (iThick == pulses_thick->end()) {
      break;
    } else if ((*iThick)->GetTime() - t < ftCoincidence) {
      e += (*iThick)->GetEnergy();
      ++iThick;
    } else {
      continue;
    }
    bool pulse_found = false;
    bool out_of_pulses = true;
    if (pulses_thin1) {
      if (iThin1 != pulses_thin1->end()) {
	out_of_pulses = false;
	if ((*iThin1)->GetTime() - t < ftCoincidence) {
	  e += (*iThin1)->GetEnergy();
	  ++iThin1;
	  pulse_found = true;
	}
      }
    }
    if (pulses_thin2) {
      if (iThin2 != pulses_thin2->end()) {
	out_of_pulses = false;
	if ((*iThin2)->GetTime() - t < ftCoincidence) {
	  e += (*iThin2)->GetEnergy();
	  ++iThin2;
	  pulse_found = true;
	}
      }
    }
    if (pulses_thin3) {
      if (iThin3 != pulses_thin3->end()) {
	out_of_pulses = false;
	if ((*iThin3)->GetTime() - t < ftCoincidence) {
	  e += (*iThin3)->GetEnergy();
	  ++iThin3;
	  pulse_found = true;
	}
      }
    }
    if (pulses_thin4) {
      if (iThin4 != pulses_thin4->end()) {
	out_of_pulses = false;
	if ((*iThin4)->GetTime() - t < ftCoincidence) {
	  e += (*iThin4)->GetEnergy();
	  ++iThin4;
	  pulse_found = true;
	}
      }
    }
    if (out_of_pulses)
      break;
    else if (!pulse_found)
      continue;
    // Fill the energy
    fhSpectrum->Fill(e);
  }
  return 0;
}
