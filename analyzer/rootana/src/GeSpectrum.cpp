#include "GeSpectrum.h"
#include "RegisterModule.inc"
#include "TMuonEvent.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "SetupNavigator.h"
#include "ExportPulse.h"
#include "PulseCandidateFinder.h"

#include "TH1I.h"
#include "TH2D.h"
#include "TF1.h"
#include "TDirectory.h"

#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <climits>
#include <string>
#include <sstream>

const IDs::channel GeSpectrum::fGe(IDs::kGe, IDs::kNotApplicable);
const IDs::channel GeSpectrum::fGeEnergy(IDs::kGe, IDs::kSlow);
const IDs::channel GeSpectrum::fMuSc(IDs::kMuSc, IDs::kNotApplicable);

extern MuonEventList gMuonEvents;

GeSpectrum::GeSpectrum(modules::options* opts) :
  BaseModule("GeSpectrum",opts),
  fHist_TimeEnergy(NULL) {

  fMinTime = opts->GetDouble("time_axis_min", -2e4);
  fMaxTime = opts->GetDouble("time_axis_max", 2e4);
  fTimeBinWidth = opts->GetDouble("time_bin_width", 100);

  TDirectory* cwd = TDirectory::CurrentDirectory();
  dir->cd();

  cwd->cd();
}

GeSpectrum::~GeSpectrum(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int GeSpectrum::BeforeFirstEntry(TGlobalData* gData, const TSetupData *setup){

  double energy_bin_width = 0.1;//SetupNavigator::Instance()->GetAdcToEnergyGain(fGeEnergy); // get the minimum energy bin width from the calibration - we can rebin later
  double max_energy = 2000;
  double min_energy = 0;
  int n_energy_bins = (max_energy - min_energy) / energy_bin_width;

  int n_time_bins = (fMaxTime - fMinTime) / fTimeBinWidth;
  fHist_TimeEnergy   = new TH2D("hTimeEnergy", "Energy of Gammas vs Time Difference to Central Muon (PP);Time [ns];Energy [keV];Counts", n_time_bins,fMinTime,fMaxTime, n_energy_bins, min_energy, max_energy);

  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int GeSpectrum::ProcessEntry(TGlobalData* gData, const TSetupData *setup){

  // Loop over each TME
  for(MuonEventList::const_iterator i_tme=gMuonEvents.begin();
      i_tme!=gMuonEvents.end(); ++i_tme){

    if ( (*i_tme)->HasMuonPileup() ) {
      continue;
    }

    double muon_arrival_time = (*i_tme)->GetTime();

    int source_index=(*i_tme)->GetSourceIndex(fGe);
    while(source_index>-1){
      const IDs::source& source=(*i_tme)->GetSource(source_index);
      int n_tdps=(*i_tme)->NumPulses(source);

      for(int i_tdp=0; i_tdp<n_tdps; ++i_tdp){
	const TDetectorPulse* tdp=(*i_tme)->GetPulse(source,i_tdp);
	double ge_time = tdp->GetTime(TDetectorPulse::kFast);
	double ge_energy = tdp->GetEnergy(TDetectorPulse::kSlow);

	fHist_TimeEnergy->Fill(muon_arrival_time - ge_time, ge_energy);
      }
      source_index=(*i_tme)->GetSourceIndex(fGe,source_index+1);
    }
  }

  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int GeSpectrum::AfterLastEntry(TGlobalData* gData, const TSetupData *setup){
  return 0;
}

ALCAP_REGISTER_MODULE(GeSpectrum, musc_cf, gef_gen, gef_cfg, gef_cf);
