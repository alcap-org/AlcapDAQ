#define USE_PRINT_OUT 

#include "AnalysePulseIsland.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>

#include "TAnalysedPulse.h"

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
    }    
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
