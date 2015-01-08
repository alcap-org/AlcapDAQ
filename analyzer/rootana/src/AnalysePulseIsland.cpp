#define USE_PRINT_OUT 

#include "AnalysePulseIsland.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <algorithm>

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
  //typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  //typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
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
      double tintegral = 0.;
      double energy = 0.;
      double ratio = 0.;

      // If this is a slow pulse
      if ( TSetupData::IsSlow(detname) ) {
	GetAllParameters_MBCFT( gSetup, *pulseIter, amplitude, time, integral, tintegral, energy, ratio);
      } else if ( TSetupData::IsFast(detname)) {
	GetAllParameters_MBCFT( gSetup, *pulseIter, amplitude, time, integral, tintegral, energy, ratio);
      } else {
	GetAllParameters_MBCFT( gSetup, *pulseIter, amplitude, time, integral, tintegral, energy, ratio);
      }


      TAnalysedPulse* analysedPulse = new TAnalysedPulse(amplitude, time, integral, tintegral, energy, ratio, detname);
      analysedPulses.push_back(analysedPulse);
    }    
    std::sort(analysedPulses.begin(), analysedPulses.end(), IsTimeOrdered);
    gAnalysedPulseMap[detname] = analysedPulses;
  }

  return 0;
}

// GetAllParameters_MaxBin()
// -- Gets all the parameters for the pulse using the max bin method
void AnalysePulseIsland::GetAllParameters_MaxBin(TSetupData* gSetup, const TPulseIsland* pulse,
						 double& amplitude, double& time, double& integral, double& tintegral, double& energy, double& ratio) {

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
  tintegral = 0;
  energy = eCalib_slope * amplitude + eCalib_offset;
  ratio = 0;
}

void AnalysePulseIsland::GetAllParameters_MBCFT(TSetupData* gSetup, const TPulseIsland* pulse,
						double& amplitude, double& time, double& integral, double& tintegral, double& energy, double& ratio) {

  float constant_fraction = 0.50;
  std::string bankname = pulse->GetBankName();
  double pedestal = gSetup->GetPedestal(bankname);
  int trigger_polarity = gSetup->GetTriggerPolarity(bankname);
  double eCalib_slope = gSetup->GetADCSlopeCalib(bankname);
  double eCalib_offset = gSetup->GetADCOffsetCalib(bankname);
  double clock_tick_in_ns = gSetup->GetClockTick(bankname);
  double time_shift = gSetup->GetTimeShift(bankname);

  std::string detname = gSetup->GetDetectorName(bankname);
  if(detname == "NDet")
    time_shift += 192;
  if(detname == "NDet2")
    time_shift += 34;

  double fullInt = 0, tailInt = 0;
  double tStart = -3, tTail = 5, tStop = 20;


  // First find the position of the peak
  const std::vector<int>& samps = pulse->GetSamples();
  const std::vector<int>::const_iterator b = samps.begin(), e = samps.end();

  std::vector<int>::const_iterator m = trigger_polarity > 0 ? std::max_element(b, e) : std::min_element(b, e);

  for(std::vector<int>::const_iterator i = m-4; i < m+20; ++i){
    int ph = *i - (int)pedestal;
    fullInt += std::abs(ph);
    if(i >= m+5){
      tailInt += std::abs(ph);
    }
  }
      
  const int amp = *m;
  const unsigned int cf = trigger_polarity > 0 ?
      (unsigned int)(constant_fraction*(double)(amp-pedestal)) + pedestal :
      (unsigned int)((double)(pedestal-amp)*(1.-constant_fraction) + amp);
  while (m != b && (trigger_polarity > 0 ? *--m > (int)cf : *--m < (int)cf));
  double dx = (double)(m-b);
  if (*(m+1) != *m)
    dx += (double)((int)cf - *m)/(double)(*(m+1) - *m);


  // Now assign the parameters
  amplitude = amp;
  time = (dx + (double)pulse->GetTimeStamp()) * clock_tick_in_ns - time_shift;
  integral = fullInt;
  tintegral = tailInt;
  energy = eCalib_slope * amplitude + eCalib_offset;
  ratio = tailInt / fullInt;
}



bool IsTimeOrdered(TAnalysedPulse* a, TAnalysedPulse* b) {

  // Want a to be before b
  return ( a->GetTime() < b->GetTime() );

}
