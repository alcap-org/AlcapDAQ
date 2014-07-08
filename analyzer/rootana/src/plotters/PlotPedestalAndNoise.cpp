#include "PlotPedestalAndNoise.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"

#include <iostream>
#include <cmath>
#include <sstream>
#include <fstream>
using std::cout;
using std::endl;

#include <TSQLiteServer.h>
#include <TSQLiteResult.h>
#include <TSQLiteRow.h>

PlotPedestalAndNoise::PlotPedestalAndNoise(modules::options* opts):
   BaseModule("PlotPedestalAndNoise",opts){

  // Do something with opts here.  Has the user specified any
  // particular configuration that you want to know?
  // For example, perhaps this module wants an axis range:
  fNSamples = opts->GetInt("n_samples", 5);
  fExportSQL = opts->GetBool("export_sql", false);
}

PlotPedestalAndNoise::~PlotPedestalAndNoise(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int PlotPedestalAndNoise::BeforeFirstEntry(TGlobalData* gData,TSetupData *setup){
  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----PlotPedestalAndNoise::BeforeFirstEntry(): I'm debugging!"<<endl;
  }

  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int PlotPedestalAndNoise::ProcessEntry(TGlobalData* gData,TSetupData *setup){

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
    if (fPedestalVsNoiseHistograms.find(detname) == fPedestalVsNoiseHistograms.end()) {
      int n_bits = TSetupData::Instance()->GetNBits(bankname);
      int x_min = 0;
      int x_max = std::pow(2, n_bits);
      int n_bins_x = (x_max - x_min)/10;

      int y_min = 0;
      int y_max = 200;
      int n_bins_y = (y_max - y_min)*10;

      std::string histname = "fPedestalVsNoiseHistogram_" + detname;
      std::stringstream histtitle;
      histtitle << "Plot of the Pedestal vs Noise (mean and RMS of first " << fNSamples << " samples) in " << detname;

      TH2D* histogram = new TH2D(histname.c_str(), histtitle.str().c_str(), n_bins_x,x_min,x_max, n_bins_y,y_min,y_max);
      histogram->GetXaxis()->SetTitle("Pedestal [ADC]");
      histogram->GetYaxis()->SetTitle("Noise [ADC]");
      fPedestalVsNoiseHistograms[detname] = histogram;
    }

    TH2D* pedestal_vs_noise_histogram = fPedestalVsNoiseHistograms[detname];

    // Loop through all the pulses
    for (PulseIslandList::iterator pulseIter = thePulseIslands.begin(); pulseIter != thePulseIslands.end(); ++pulseIter) {

      const std::vector<int>& theSamples = (*pulseIter)->GetSamples();

      double sum = 0;
      for (int iSample = 0; iSample < fNSamples; ++iSample) {
	sum += theSamples.at(iSample);
      }
      double mean = sum / fNSamples;
      
      double sum_of_deviations_squared = 0;
      for (int iSample = 0; iSample < fNSamples; ++iSample) {
	sum_of_deviations_squared += (theSamples.at(iSample) - mean)*(theSamples.at(iSample) - mean);
      }
      double RMS = std::sqrt(sum_of_deviations_squared / fNSamples);

      pedestal_vs_noise_histogram->Fill(mean, RMS);
    } // end loop through pulses
  } // end loop through detectors

  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int PlotPedestalAndNoise::AfterLastEntry(TGlobalData* gData,TSetupData *setup){

  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----PlotPedestalAndNoise::AfterLastEntry(): I'm debugging!"<<endl;
  }

  if (fExportSQL) {
    // Get the SQLite database file
    TSQLiteServer* server = new TSQLiteServer("sqlite://pedestals-and-noises.sqlite");

    std::stringstream query; 
    std::string tablename = "pedestals_and_noises";
    if (server) {
      // Create the pedestals_and_noises table if it doesn't already exist
      query << "CREATE TABLE IF NOT EXISTS " << tablename << " (channel STRING PRIMARY KEY, bank STRING, pedestal FLOAT, noise FLOAT)";
      server->Exec(query.str().c_str());
      query.str(""); // clear the stringstream after use

      // Now loop through the histograms and record the channel, bank and mean and RMS of first fNSamples to the SQLite table
      for (std::map<std::string, TH2D*>::iterator histIter = fPedestalVsNoiseHistograms.begin(); histIter != fPedestalVsNoiseHistograms.end(); ++histIter) {
	std::string detname = histIter->first;
	std::string bankname = setup->GetBankName(detname);
	TH2D* pedestal_vs_noise_histogram = histIter->second;
      
	double pedestal = pedestal_vs_noise_histogram->GetMean(1);
	double noise = pedestal_vs_noise_histogram->GetMean(2);

	// See if this row already exists (result will equal 1 if it does)
	query << "SELECT COUNT(*) FROM " << tablename << " WHERE channel=\'" << detname << "\'";
	TSQLiteResult* result = (TSQLiteResult*) server->Query(query.str().c_str());  // get the result of this query
	TSQLiteRow* row = (TSQLiteRow*) result->Next(); // get the first row
	query.str(""); // clear the stringstream after use
      
	std::string n_table_entries = row->GetField(0);

	if (n_table_entries == "0") {
	  // insert a new set of values
	  query << "INSERT INTO " << tablename << " VALUES (\"" << detname << "\", \"" << bankname << "\", " << pedestal << ", " << noise << ");"; // insert the values
	  server->Exec(query.str().c_str());
	  query.str(""); // clear the stringstream after use
	}
	else if (n_table_entries == "1") {
	  // just update the values that are already there
	  query << "UPDATE " << tablename << " SET bank=\'" << bankname << "\', pedestal=" << pedestal << ", noise=" << noise << " WHERE channel=\'" << detname << "\';";
	  server->Exec(query.str().c_str());
	  query.str(""); // clear the stringstream after use
	}

	// Need to delete the TSQLiteResult and TSQLiteRow like the ROOT documentation says so
	delete result;
	delete row;
      }
    }
    else {
      std::cout << "Error: Couldn't connect to SQLite database" << std::endl;
    }
  }
  
  return 0;
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(PlotPedestalAndNoise,x_max);
