#include "PlotTPI_PedestalAndNoise.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"

#include "SetupNavigator.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
using std::cout;
using std::endl;

#include <TSQLiteServer.h>
#include <TSQLiteResult.h>
#include <TSQLiteRow.h>


PlotTPI_PedestalAndNoise::PlotTPI_PedestalAndNoise(modules::options* opts):
  BaseModule("PlotTPI_PedestalAndNoise",opts),
  fNSamples(opts->GetInt("n_samples")),
  fExportSQL(opts->GetBool("export_sql", false)) {
}

PlotTPI_PedestalAndNoise::~PlotTPI_PedestalAndNoise(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int PlotTPI_PedestalAndNoise::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----PlotTPI_PedestalAndNoise::BeforeFirstEntry(): I'm debugging!"<<endl;
  }

  return 0;
}



// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int PlotTPI_PedestalAndNoise::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

  // Prepare a few variables
  std::string bankname, detname;
  PulseIslandList thePulseIslands;
  StringPulseIslandMap::const_iterator it;


  // Loop over each detector
  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); ++it){

    // Get the bank and detector names for this detector. If bank==det, then
    // channel was disconnected.
    bankname = it->first;
    detname = setup->GetDetectorName(bankname);
    if (bankname == detname) continue;

    // Get the TPIs
    thePulseIslands = it->second;
    if (thePulseIslands.size() == 0) continue; // no pulses here..

    // Create the histograms if they don't exist for RMS noise and pedestals.
    if (fPedestalVsNoiseHistograms.count(detname) == 0)
      BookHistogram(bankname, detname);


    TH2D* pedestal_vs_noise_histogram = fPedestalVsNoiseHistograms[detname];

    // Loop through all the pulses
    for (PulseIslandList::iterator pulseIter = thePulseIslands.begin(); pulseIter != thePulseIslands.end(); ++pulseIter) {

      const std::vector<int>& theSamples = (*pulseIter)->GetSamples();
      int   limit = std::min<int>(fNSamples, theSamples.size());
      double sum = std::accumulate(theSamples.begin(), theSamples.begin()+limit, 0.);
      double mean = sum / limit;

      double sum_of_deviations_squared = 0;
      for (int iSample = 0; iSample < limit; ++iSample) {
          sum_of_deviations_squared += (theSamples.at(iSample) - mean)*(theSamples.at(iSample) - mean);
      }
      double stdev = std::sqrt(sum_of_deviations_squared / limit);

      pedestal_vs_noise_histogram->Fill(mean, stdev);
    } // end loop through pulses
  } // end loop through detectors

  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int PlotTPI_PedestalAndNoise::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----PlotTPI_PedestalAndNoise::AfterLastEntry(): I'm debugging!"<<endl;
  }

  if (fExportSQL) {

    // Now loop through the histograms and record the channel, mean and RMS of first fNSamples
    for (std::map<std::string, TH2D*>::iterator histIter = fPedestalVsNoiseHistograms.begin(); histIter != fPedestalVsNoiseHistograms.end(); ++histIter) {
      std::string detname = histIter->first;
      IDs::channel channel(detname);

      TH2D* pedestal_vs_noise_histogram = histIter->second;

      double pedestal = pedestal_vs_noise_histogram->GetMean(1);
      double noise = pedestal_vs_noise_histogram->GetMean(2);

      SetupNavigator::Instance()->SetPedestalAndNoise(channel, pedestal, noise);
    }
  }

  return 0;
}

void PlotTPI_PedestalAndNoise::BookHistogram(const std::string& bank,
                                             const std::string& det) {
  int n_bits = TSetupData::Instance()->GetNBits(bank);
  int x_min = 0;
  int x_max = std::pow(2, n_bits);
  int n_bins_x = 100;

  int y_min = 0;
  int y_max = 200;
  int n_bins_y = (y_max - y_min)*5;

  std::string histname = "fPedestalVsNoiseHistogram_" + det;
  std::stringstream histtitle;
  histtitle << "Plot of the Pedestal vs Noise (mean and RMS of first "
            << fNSamples << " samples) in " << det;

  TH2D* histogram = new TH2D(histname.c_str(), histtitle.str().c_str(),
                             n_bins_x, x_min, x_max,
                             n_bins_y, y_min, y_max);
  histogram->GetXaxis()->SetTitle("Pedestal [ADC]");
  histogram->GetYaxis()->SetTitle("Noise [ADC]");
  fPedestalVsNoiseHistograms[det] = histogram;
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(PlotTPI_PedestalAndNoise,n_samples,export_sql);
