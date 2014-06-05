#include "TAPGeneratorFactory.h"
#include "MaxBinAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include <algorithm>

// IsTimeOrdered()
// -- Returns tru of the first pulse is before the second
// Static so that only this file sees it
// Useful for sorting the pulses
static bool IsTimeOrdered(TAnalysedPulse* a, TAnalysedPulse* b) {
  // Want a to be before b
  return ( a->GetTime() < b->GetTime() );
}

int MaxBinAPGenerator::ProcessPulses(
      const PulseIslandList& pulseList, AnalysedPulseList& analysedList){

      SetBankInfo(pulseList[0]->GetBankName());

      double amplitude, time, integral, energy;

      for (PulseIslandList::const_iterator pulseIter = pulseList.begin(); pulseIter != pulseList.end(); pulseIter++) {
         amplitude = 0;
         time = 0;
         integral = 0;
         energy = 0.;
         // Assume one TAnalysedPulse per TPulseIsland
         GetAllParameters_MaxBin(*pulseIter,amplitude,time,integral,energy);

         // Add the pulse into the list
         analysedList.push_back(new  TAnalysedPulse(amplitude, time, integral, energy, fDetName));

      }
      std::sort(analysedList.begin(), analysedList.end(), IsTimeOrdered);
      return 0;
}

void MaxBinAPGenerator::GetAllParameters_MaxBin(const TPulseIsland* pulse,
						 double& amplitude, double& time, double& integral, double& energy) {


  // First find the position of the peak
  std::vector<int> pulseSamples = pulse->GetSamples();
  int peak_sample_value = 0;
  int peak_sample_pos = 0;
  int this_height =0;
  for (std::vector<int>::const_iterator sampleIter = pulseSamples.begin(); sampleIter != pulseSamples.end(); sampleIter++) {
    
    this_height = fTriggerPolarity*(*(sampleIter) - fPedestal);
    if ( this_height > peak_sample_value ) {
      peak_sample_value = this_height;
      peak_sample_pos = sampleIter - pulseSamples.begin();
    }
  }

  // Now assign the parameters
  amplitude = peak_sample_value;
  time = ((pulse->GetTimeStamp() + peak_sample_pos) * fClockTick) - fTimeShift;
  integral = 0;
  energy = fECalibSlope * amplitude + fECalibOffset;
}

ALCAP_TAP_GENERATOR(MaxBin);
