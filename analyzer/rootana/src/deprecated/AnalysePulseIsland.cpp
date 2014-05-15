#define USE_PRINT_OUT 

#include "AnalysePulseIsland.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <sstream>

#include "TAnalysedPulse.h"
static int pulse_counter = 0;
using std::string;
using std::map;
using std::vector;
using std::pair;

extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;

AnalysePulseIsland::AnalysePulseIsland(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){  
  dir->cd("/");
}

AnalysePulseIsland::~AnalysePulseIsland(){  
}

int AnalysePulseIsland::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

   //std::cout << "Size of gData " << gData->fPulseIslandToChannelMap.size() << std::endl;
  for(map_iterator mapIter = gData->fPulseIslandToChannelMap.begin(); mapIter != gData->fPulseIslandToChannelMap.end(); mapIter++){
    pulse_counter = 0;
    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);
    std::vector<TPulseIsland*> thePulses = mapIter->second;
    //std::cout<<"Analysing detector: "<<detname<<std::endl;

    if (thePulses.size() == 0)
      continue; // no pulses here...

    // Loop over the TPulseIslands and analyse the TPulseIslands into TAnalysedPulses
    std::vector<TAnalysedPulse*> analysedPulses;

    for (std::vector<TPulseIsland*>::iterator pulseIter = thePulses.begin(); pulseIter != thePulses.end(); pulseIter++) {

      // For the time being assume one TAnalysedPulse per TPulseIsland
      double amplitude = 0;
      double time = 0;
      double integral = 0;
      double energy = 0.;

      // Find all the pulses on the island first...
      //      std::vector<TPulseIsland*> allPulses = FindPulses(*pulseIter);
      //      std::cout << "Number of Pulses for " << detname << " on this Island: " << allPulses.size() << std::endl;
      // Now loop through all these pulses and create TAnalysedPulses
      //      for (std::vector<TPulseIsland*>::iterator allPulseIter = allPulses.begin(); allPulseIter != allPulses.end(); ++allPulseIter) {

	// If this is a slow pulse
	if ( TSetupData::IsSlow(detname) ) {
	  GetAllParameters_MaxBin( gSetup, *pulseIter, amplitude, time, integral, energy);
	} else if ( TSetupData::IsFast(detname)) {
	  GetAllParameters_MaxBin( gSetup, *pulseIter, amplitude, time, integral, energy);
	} else {
	  GetAllParameters_MaxBin( gSetup, *pulseIter, amplitude, time, integral, energy);
	}


	TAnalysedPulse* analysedPulse = new TAnalysedPulse(amplitude, time, integral, energy, detname);
	analysedPulses.push_back(analysedPulse);
	//      }
    }    
    std::sort(analysedPulses.begin(), analysedPulses.end(), IsTimeOrdered);
    gAnalysedPulseMap[detname] = analysedPulses;
  }

  return 0;
}

// GetAllParameters_MaxBin()
// -- Gets all the parameters for the pulse using the max bin method
void AnalysePulseIsland::GetAllParameters_MaxBin(TSetupData* gSetup, const TPulseIsland* pulse,
						 double& amplitude, double& time, double& integral, double& energy) {

  std::string bankname = pulse->GetBankName();
  double pedestal = gSetup->GetPedestal(bankname);
  int trigger_polarity = gSetup->GetTriggerPolarity(bankname);
  double eCalib_slope = gSetup->GetADCSlopeCalib(bankname);
  double eCalib_offset = gSetup->GetADCOffsetCalib(bankname);

  // First find the position of the peak
  std::vector<int> pulseSamples = pulse->GetSamples();
  int peak_sample_value = 0;
  int peak_sample_pos = 0;
  for (std::vector<int>::const_iterator sampleIter = pulseSamples.begin(); sampleIter != pulseSamples.end(); sampleIter++) {
    
    int this_height = trigger_polarity*(*(sampleIter) - pedestal);
    if ( this_height > peak_sample_value ) {
      peak_sample_value = this_height;
      peak_sample_pos = sampleIter - pulseSamples.begin();
    }
  }

  // Now assign the parameters
  amplitude = peak_sample_value;
  time = ((pulse->GetTimeStamp() + peak_sample_pos) * gSetup->GetClockTick(bankname)) - gSetup->GetTimeShift(bankname);
  integral = 0;
  energy = eCalib_slope * amplitude + eCalib_offset;
}

// IsTimeOrdered()
// -- Returns tru of the first pulse is before the second
bool IsTimeOrdered(TAnalysedPulse* a, TAnalysedPulse* b) {

  // Want a to be before b
  return ( a->GetTime() < b->GetTime() );

}

// FindPulses()
// -- Finds all pulses on an island and returns the vector of sub pulses
std::vector<TPulseIsland*> AnalysePulseIsland::FindPulses(TPulseIsland* theIsland) {

  // Get the output ready
  std::vector<TPulseIsland*> output;

  // Things we need from the oldpulse island
  std::string bankname = theIsland->GetBankName();
  std::vector<int> theSamples = theIsland->GetSamples();
  int pedestal = TSetupData::Instance()->GetPedestal(bankname);
  int trigger_polarity = TSetupData::Instance()->GetTriggerPolarity(bankname);
  int RMS = 20; // hard-coded for the time being

  // Histograms
  std::stringstream histname;
  histname << bankname << "new_pulses_" << pulse_counter;
  ++pulse_counter;
  //  TH1F* new_pulses = new TH1F(histname.str().c_str(), histname.str().c_str(), theSamples.size(),0,theSamples.size());
  histname << "_old";
  //  TH1F* old_pulse = new TH1F(histname.str().c_str(), histname.str().c_str(), theSamples.size(),0,theSamples.size());

  // What we need for the new pulses
  int timestamp = 0;
  std::vector<int> newSamples;

  // Loop through the samples
  bool start_pulse = false;
  for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {
  
    // Get the current height (taking into account pulse polarity)
    int height = trigger_polarity*(*(sampleIter) - pedestal);
    //    old_pulse->Fill(sampleIter - theSamples.begin(), *sampleIter);

    if (start_pulse == false) { // if we haven't found a pulse yet then see if we do
      if (height > 0 + RMS) {
	timestamp = sampleIter - theSamples.begin();
	start_pulse = true;
      }
    }

    if (start_pulse == true) {
      // see if the pulse has ended
      if (height < 0) {
	start_pulse = false; // the pulse is over

	// Add the TPulseIsland
	output.push_back(new TPulseIsland(timestamp, newSamples, bankname));

	// Clear the old information
	newSamples.clear();
	timestamp = 0;
      }
      else {
	newSamples.push_back(*sampleIter);
	//	new_pulses->Fill(sampleIter - theSamples.begin(), *sampleIter);
      }
    }
  }

  return output;
}
