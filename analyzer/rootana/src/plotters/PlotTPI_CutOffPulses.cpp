#include "PlotTPI_CutOffPulses.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"

#include "SetupNavigator.h"

#include <iostream>
#include <cmath>
#include <sstream>
#include <fstream>
#include <algorithm>
using std::cout;
using std::endl;

#include <TSQLiteServer.h>
#include <TSQLiteResult.h>
#include <TSQLiteRow.h>

#include "ExportPulse.h"

PlotTPI_CutOffPulses::PlotTPI_CutOffPulses(modules::options* opts):
  BaseModule("PlotTPI_CutOffPulses",opts),
  fNMaxExportPulses(opts->GetInt("n_max_export_pulses", 20)),
  fNExportPulses(0), fPulsesPlottedPerDetectorHist(NULL) {
}

PlotTPI_CutOffPulses::~PlotTPI_CutOffPulses(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int PlotTPI_CutOffPulses::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----PlotTPI_CutOffPulses::BeforeFirstEntry(): I'm debugging!"<<endl;
  }

  if(!ExportPulse::Instance()){
    std::cout<<"PlotTPI_CutOffPulses: Error: You need to run with the ExportPulse module to be able to export pulses"<<std::endl;
    return 1;
  }

  int n_bins = gData->fPulseIslandToChannelMap.size();
  fPulsesPlottedPerDetectorHist = new TH1F("fPulsesPlottedPerDetectorHist", "Number of Pulses with the final sample as the largest", n_bins,0,n_bins);
  fPulsesPlottedPerDetectorHist->SetXTitle("Detector Name");
  fPulsesPlottedPerDetectorHist->SetYTitle("Number of \"Odd\" Pulses");
  fPulsesPlottedPerDetectorHist->Sumw2();

  fPulsesPerDetectorHist = new TH1F("fPulsesPerDetectorHist", "Number of Pulses that were looked at", n_bins,0,n_bins);
  fPulsesPerDetectorHist->SetXTitle("Detector Name");
  fPulsesPerDetectorHist->SetYTitle("Number of Pulses");
  fPulsesPerDetectorHist->Sumw2();

  // Set the bin labels
  std::string bankname, detname;
  PulseIslandList thePulseIslands;
  StringPulseIslandMap::const_iterator it;

  // Loop over each detector
  int bin_counter = 1;
  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); ++it){
    
    // Get the bank and detector names for this detector
    bankname = it->first;
    detname = setup->GetDetectorName(bankname);
    fPulsesPlottedPerDetectorHist->GetXaxis()->SetBinLabel(bin_counter, detname.c_str());
    fPulsesPerDetectorHist->GetXaxis()->SetBinLabel(bin_counter, detname.c_str());
    ++bin_counter;
  }

  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int PlotTPI_CutOffPulses::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

  // Prepare a few variables
  std::string bankname, detname;
  PulseIslandList thePulseIslands;
  StringPulseIslandMap::const_iterator it;


  // Loop over each detector
  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); ++it){
    
    // Get the bank and detector names for this detector
    bankname = it->first;
    detname = setup->GetDetectorName(bankname);

    int trigger_polarity = TSetupData::Instance()->GetTriggerPolarity(bankname);

    // Get the TPIs
    thePulseIslands = it->second;
    if (thePulseIslands.size() == 0) continue; // no pulses here..

    // Loop through all the pulses
    for (PulseIslandList::iterator pulseIter = thePulseIslands.begin(); pulseIter != thePulseIslands.end(); ++pulseIter) {

      const std::vector<int>& theSamples = (*pulseIter)->GetSamples();
      std::vector<int>::const_iterator peak_sample_pos;

      if (trigger_polarity == 1)
	peak_sample_pos = std::max_element(theSamples.begin(), theSamples.end());
      else if (trigger_polarity == -1)
	peak_sample_pos = std::min_element(theSamples.begin(), theSamples.end());

      fPulsesPerDetectorHist->Fill(detname.c_str(), 1);

      if (peak_sample_pos == theSamples.end()-1) {
	if(fNExportPulses <= fNMaxExportPulses) {
	  ExportPulse::Instance()->AddToExportList(detname, pulseIter-thePulseIslands.begin());
	  fPulsesPlottedPerDetectorHist->Fill(detname.c_str(), 1);
	  ++fNExportPulses;
	}
	else {
	  std::cout << "PlotTPI_CutOffPulses: Reached limit of " << fNMaxExportPulses << " pulses exported. Quitting..." << std::endl;
	  return 1;
	}
      }

    } // end loop through pulses
  } // end loop through detectors

  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int PlotTPI_CutOffPulses::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----PlotTPI_CutOffPulses::AfterLastEntry(): I'm debugging!"<<endl;
  }

  fPulsesPlottedPerDetectorHist_Normalised = (TH1F*) fPulsesPlottedPerDetectorHist->Clone("fPulsesPlottedPerDetectorHist_Normalised");
  fPulsesPlottedPerDetectorHist_Normalised->SetTitle("Fraction of Pulses with the final sample as the largest");
  fPulsesPlottedPerDetectorHist_Normalised->SetYTitle("Fraction of \"Odd\" Pulses");

  fPulsesPlottedPerDetectorHist_Normalised->Divide(fPulsesPerDetectorHist);

  return 0;
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(PlotTPI_CutOffPulses,n_max_export_pulses);
