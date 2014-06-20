#include "PlotRMSNoise.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"

#include <iostream>
#include <cmath>
using std::cout;
using std::endl;

PlotRMSNoise::PlotRMSNoise(modules::options* opts):
   BaseModule("PlotRMSNoise",opts){

  // Do something with opts here.  Has the user specified any
  // particular configuration that you want to know?
  // For example, perhaps this module wants an axis range:
}

PlotRMSNoise::~PlotRMSNoise(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int PlotRMSNoise::BeforeFirstEntry(TGlobalData* gData,TSetupData *setup){
  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----PlotRMSNoise::BeforeFirstEntry(): I'm debugging!"<<endl;
  }

  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int PlotRMSNoise::ProcessEntry(TGlobalData* gData,TSetupData *setup){

  // Prepare a few variables
  std::string bankname, detname;
  PulseIslandList thePulseIslands;
  StringPulseIslandMap::const_iterator it;

  // Loop over each detector
  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); ++it){
    
    // Get the bank and detector names for this detector
    bankname = it->first;
    detname = setup->GetDetectorName(bankname);

    // Get the TPIs
    thePulseIslands = it->second;
    if (thePulseIslands.size() == 0) continue; // no pulses here..

    // Create the histogram that will store all the average RMS noises from each event
    if (fRMSNoiseHistograms.find(detname) == fRMSNoiseHistograms.end()) {
      int max_value = 200;
      int n_bins = max_value*10;
      std::string histname = "fRMSNoiseHistogram_" + detname;
      std::string histtitle = "Average RMS Noises of each Event in " + detname;
      TH1D* histogram = new TH1D(histname.c_str(), histtitle.c_str(), n_bins, 0, max_value);
      histogram->GetXaxis()->SetTitle("Average RMS Noise of Event [ADC]");
      histogram->GetYaxis()->SetTitle("Number of Events");
      fRMSNoiseHistograms[detname] = histogram;
    }

    TH1D* rms_noise_histogram = fRMSNoiseHistograms[detname];

    // Loop through all the pulses
    for (PulseIslandList::iterator pulseIter = thePulseIslands.begin(); pulseIter != thePulseIslands.end(); ++pulseIter) {

      const std::vector<int>& theSamples = (*pulseIter)->GetSamples();

      double sum = 0;
      int n_samples = 5;
      for (int iSample = 0; iSample < n_samples; ++iSample) {
	sum += theSamples.at(iSample);
      }
      double mean = sum / n_samples;
      
      double sum_of_deviations_squared = 0;
      for (int iSample = 0; iSample < n_samples; ++iSample) {
	sum_of_deviations_squared += (theSamples.at(iSample) - mean)*(theSamples.at(iSample) - mean);
      }
      double RMS = std::sqrt(sum_of_deviations_squared);

      rms_noise_histogram->Fill(RMS);
    } // end loop through pulses
  } // end loop through detectors

  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int PlotRMSNoise::AfterLastEntry(TGlobalData* gData,TSetupData *setup){

  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----PlotRMSNoise::AfterLastEntry(): I'm debugging!"<<endl;
  }

  // Print the channel, bank and mean of the RMS noise (i.e. the 1-sigma value)
  std::cout << "Channel\tBank\t1-sigma" << std::endl;
  for (std::map<std::string, TH1D*>::iterator noiseHistIter = fRMSNoiseHistograms.begin(); noiseHistIter != fRMSNoiseHistograms.end(); ++noiseHistIter) {
    std::string detname = noiseHistIter->first;
    std::string bankname = setup->GetBankName(detname);
    TH1D* rms_noise_histogram = noiseHistIter->second;

    double sigma = rms_noise_histogram->GetMean(); // NB that the histogram is a plot of the RMSs already
    std::cout << detname << "\t" << bankname << "\t" << sigma << std::endl;
  }

  return 0;
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(PlotRMSNoise,x_max);
