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

#include "TH2I.h"

extern std::map< std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;

SiR2Target::SiR2Target(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName) {

  double nbinsx      = 2000;
  double nbinsy      = 1000;
  double max_e_thick = 20000.;
  double max_e_thin  = 10000.;

  fhEvdE = new TH2I("hSiR2EvdE", "E vs dE; E_{arm} + dE_{arm} + E_{targ}; ",
		    nbinsx, 0., max_e_thick,
		    nbinsy, 0., max_e_thin);
  // Set the coincidence times, assuming TAnalyzedPulse time offsets were at least nominally correct
  ftCoinc_Musc_Stop[0] = 0.; ftCoinc_Musc_Stop[1] = 500.;
  ftCoinc_Stop_Decay[0] = 500.; ftCoinc_Stop_Decay[1] = 5000.;
  ftCoinc_Decay_ArmHit[0] = 0.; ftCoinc_Decay_ArmHit[1] = 500.;

  gDirectory->cd("/");
}

SiR2Target::~SiR2Target(){
}

int SiR2Target::ProcessEntry(TGlobalData *gData, TSetupData *gSetup) {

  /////////////////////////////////////////////
  /*** Get TAnalyzed Pulse Vector Pointers ***/
  /////////////////////////////////////////////
  std::vector<bool> thin_mask(4, false);  // To keep track of which thin quadrants have hits
  char thin_name[16]; // For grabbing the name of the thin quadrants based on the naming pattern
  int iSec;           // A common iterator
  // Check first to make sure at least the minimum number of pulses we need exist
  if (gAnalysedPulseMap.count("muSc") == 0)
    return 0;
  else if (gAnalysedPulseMap.count("SiR2-S") == 0)
    return 0;
  else if (gAnalysedPulseMap.count("SiL2-S") == 0)
    return 0;
  else {
    bool found = false;
    for (iSec = 0; iSec < 4; ++iSec) {
      sprintf(thin_name, "SiL1-%1d-S", iSec + 1);
      if (gAnalysedPulseMap.count(thin_name) > 0) {
	found = true;
	thin_mask.at(iSec) = true;
      }
    }
    if (!found)
      return 0;
  }

  // Now grab the addresses of those vectors
  std::vector<TAnalysedPulse*>* pulses_musc;
  std::vector<TAnalysedPulse*>* pulses_targ;
  std::vector<std::vector<TAnalysedPulse*>*> pulses_thin(4, NULL);
  std::vector<TAnalysedPulse*>* pulses_thick;

  pulses_musc = &gAnalysedPulseMap.at("muSc");
  pulses_targ = &gAnalysedPulseMap.at("SiR2-S");
  for (iSec = 0; iSec < 4; ++iSec) {
    if (thin_mask.at(iSec)) {
      sprintf(thin_name, "SiL1-%1d-S", iSec + 1);
      pulses_thin.at(iSec) = &gAnalysedPulseMap.at(thin_name);
    } else {
      pulses_thin.at(iSec) = NULL;
    }
  }
  pulses_thick = &gAnalysedPulseMap.at("SiL2-S");

  ////////////////////////////////
  /*** Prepare loop iterators ***/
  ////////////////////////////////
  std::vector<TAnalysedPulse*>::iterator cMusc;
  std::vector<TAnalysedPulse*>::iterator cTarg;
  std::vector<std::vector<TAnalysedPulse*>::iterator> cThin(4);
  std::vector<TAnalysedPulse*>::iterator cThick;

  cMusc = pulses_musc->begin();
  cTarg = pulses_targ->begin();
  for (iSec = 0; iSec < 4; ++iSec)
    if (thin_mask.at(iSec))
      cThin.at(iSec) = pulses_thin.at(iSec)->begin();
  cThick = pulses_thick->begin();

  //////////////////////
  /*** Sanity checks ***/
  //////////////////////
  if (pulses_musc->size() == 0) {
    std::cout << "WARNING: muSc analyzed pulse vector exists but has size zero (skipping event)." << std::endl;
    return 0;
  } else if (pulses_targ->size() == 0) {
    std::cout << "WARNING: SiR2-S analyzed pulse vector exists but has size zero (skipping event)." << std::endl;
    return 0;
  } else if (pulses_thick->size() == 0) {
    std::cout << "WARNING: SiL2-S analyzed pulse vector exists but has size zero (skipping event)." << std::endl;
    return 0;
  } else {
    for (iSec = 0; iSec < 4; ++iSec) {
      if (thin_mask.at(iSec)) {
	if (pulses_thin.at(iSec)->size() == 0) {
	  std::cout << "WARNING: SiL1-" << iSec + 1 << "-S analyzed pulse vector exists but has size zero (skipping event)." << std::endl;
	  return 0;
	}
      }
    }
  }

  //////////////////////////
  /*** Loop Bookkeeping ***/
  //////////////////////////
  // Energy
  double e_Decay;
  double e_Thin;
  double e_Thick;
  double e_Total;
  // Time
  double t_Musc;
  double t_Stop;
  double t_Decay;
  std::vector<double> t_Thin(4);
  double t_Thick;
  // Pulse abundance
  bool out_of_pulses = false;
  std::vector<bool> out_of_thin_pulses(4);
  std::vector<bool> valid_thin_pulse(4);
  for (iSec = 0; iSec < 4; ++iSec)
    if (thin_mask.at(iSec))
      out_of_thin_pulses.at(iSec) = false;
    else
      out_of_thin_pulses.at(iSec) = true;
  //////////////////////////////////////////////////
  /*** Loop through pulses to find coincidences ***/
  ///////////////////l///////////////////////////////
  int n = 0;
  for (; cMusc != pulses_musc->end(); ++cMusc) {
    ++n;
    // Reset all bookkeeping to zero
    t_Musc = 0.;
    t_Stop = 0.;
    t_Decay = 0.; e_Decay = 0.;
    for (iSec = 0; iSec < 4; ++iSec) {
      t_Thin.at(iSec) = 0.;
      valid_thin_pulse.at(iSec) = false;
    }
    e_Thin = 0.;
    e_Thick = 0; t_Thick = 0;
    e_Total = 0.;
    // Catch the target pulses up to the MuSc_Stop coincidence window
    t_Musc = (*cMusc)->GetTime();
    while ((*cTarg)->GetTime() - t_Musc < ftCoinc_Musc_Stop[0]) {
      ++cTarg;
      // Check to see if we have a pulses left in the target
      if (cTarg == pulses_targ->end()) {
	out_of_pulses = true;
	break;
      }
    }
    if (out_of_pulses)
      break;
    // Check if the next pulse in the target is a stop
    t_Stop = (*cTarg)->GetTime();
    if (t_Stop - t_Musc >= ftCoinc_Musc_Stop[1])
      continue;
    // Catch the target pulses up to the Decay_Stop coincidence window
    ++cTarg;
    if (cTarg == pulses_targ->end()) {
      out_of_pulses = true;
      break;
    }
    while ((*cTarg)->GetTime() - t_Stop < ftCoinc_Stop_Decay[0]) {
      ++cTarg;
      if (cTarg == pulses_targ->end()) {
	std::cout << "Ran out of target pulses (decays) after " << n << " muSc hits." << std::endl;
	out_of_pulses = true;
	break;
      }
    }
    if (out_of_pulses)
      break;
    // Check if the next pulse in the target is a decay
    t_Decay = (*cTarg)->GetTime();
    if (t_Decay - t_Stop >= ftCoinc_Stop_Decay[1])
      continue;
    // Catch the thin and thick pulses up to the Stop_ArmHit coincidence window
    // if we're not out of them yet
    for (iSec = 0; iSec < 4; ++iSec) {
      if (out_of_thin_pulses.at(iSec))
	continue;
      while ((*cThin.at(iSec))->GetTime() - t_Decay < ftCoinc_Decay_ArmHit[0]) {
	++cThin.at(iSec);
	if (cThin.at(iSec) == pulses_thin.at(iSec)->end()) {
	  out_of_thin_pulses.at(iSec) = true;
	  std::cout << "Ran out of thin (" << iSec << ") pulses after " << n << " muSc hits." << std::endl;
	  break;
	}
      }
    }
    if (out_of_thin_pulses.at(0) && out_of_thin_pulses.at(1) &&
	out_of_thin_pulses.at(2) && out_of_thin_pulses.at(3))
      out_of_pulses = true;
    if (out_of_pulses)
      break;
    while ((*cThick)->GetTime() - t_Decay < ftCoinc_Decay_ArmHit[0]) {
      ++cThick;
      if (cThick == pulses_thick->end()) {
	std::cout << "Ran out of thick pulses after " << n << " muSc hits." << std::endl;
	out_of_pulses = true;
	break;
      }
    }
    if (out_of_pulses)
      break;
    for (iSec = 0; iSec < 4; ++iSec) {
      if (out_of_thin_pulses.at(iSec))
	continue;
      t_Thin.at(iSec) = (*cThin.at(iSec))->GetTime();
    }
    t_Thick = (*cThick)->GetTime();
    for (iSec = 0; iSec < 4; ++iSec) {
      if (out_of_thin_pulses.at(iSec))
	continue;
      if (t_Thin.at(iSec) - t_Decay < ftCoinc_Decay_ArmHit[1])
	valid_thin_pulse.at(iSec) = true;
    }
    if (!(valid_thin_pulse.at(0) || valid_thin_pulse.at(1) || valid_thin_pulse.at(2) || valid_thin_pulse.at(3)))
      continue;
    if (t_Thick - t_Decay >= ftCoinc_Decay_ArmHit[1])
      continue;
    // At this point we've identified we have some good pulses.
    // Now to get the energies and fill the hitograms
    e_Decay = (*cTarg)->GetEnergy();
    for (iSec = 0; iSec < 4; ++iSec)
      if (valid_thin_pulse.at(iSec))
	e_Thin += (*cThin.at(iSec))->GetEnergy();
    e_Thick = (*cThick)->GetEnergy();
    e_Total = e_Decay + e_Thin + e_Thick;
    fhEvdE->Fill(e_Total, e_Thin + e_Thick);
    std::cout << "Filled..." << std::endl;
  }

  return 0;
}
