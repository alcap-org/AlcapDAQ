#include "TAPGeneratorFactory.h"
#include "MaxBinNoShiftAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include "EventNavigator.h"
#include "SetupNavigator.h"

#include <algorithm>
#include <cmath>

// IsTimeOrdered()
// -- Returns true of the first pulse is before the second
// Static so that only this file sees it
// Useful for sorting the pulses
static bool IsTimeOrdered(TAnalysedPulse* a, TAnalysedPulse* b) {
  // Want a to be before b
  return ( a->GetTime() < b->GetTime() );
}

//======================================================================

static bool OverThreshold(const TPulseIsland* tpi, double thr_pct) {
  const std::vector<int>::const_iterator beg = tpi->GetSamples().begin();
  const std::vector<int>::const_iterator end = tpi->GetSamples().end();
  const int max = (int)std::pow(2.,TSetupData::Instance()->GetNBits(tpi->GetBankName())) - 1;
  const int pol = TSetupData::Instance()->GetTriggerPolarity(tpi->GetBankName());
  const int ped = SetupNavigator::Instance()->GetPedestal(TSetupData::Instance()->GetDetectorName(tpi->GetBankName()));
  const int thr = pol > 0 ? thr_pct * (max - ped) + (double)ped : (1. - thr_pct) * ped;
  return pol > 0 ? *std::max_element(beg, end) > thr : *std::min_element(beg, end) < thr;
}


//----------------------------------------------------------------------
int MaxBinNoShiftAPGenerator::ProcessPulses(const PulseIslandList& pulseList,
                                     AnalysedPulseList& analysedList)
{
  double amplitude, time;
  TAnalysedPulse* outPulse;

  // Get the various variables we need from TSetupData/SetupNavigator
  std::string bankname = pulseList[0]->GetBankName();
  fMaxBinAmplitude.pedestal = SetupNavigator::Instance()->GetPedestal(TSetupData::Instance()->GetDetectorName(bankname));

  fMaxBinAmplitude.trigger_polarity = TSetupData::Instance()->GetTriggerPolarity(bankname);
  fMaxBinTime.trigger_polarity = fMaxBinAmplitude.trigger_polarity;

  fMaxBinTime.clock_tick_in_ns = TSetupData::Instance()->GetClockTick(bankname);
  fMaxBinTime.time_shift = 0.;


  for (PulseIslandList::const_iterator pulseIter = pulseList.begin(); pulseIter != pulseList.end(); pulseIter++) {
    if(!OverThreshold(*pulseIter, fThresholdPercent))
      continue;
    amplitude = fMaxBinAmplitude(*pulseIter);
    time = fMaxBinTime(*pulseIter);
    
    // Make the TAnalysedPulse pulse
    outPulse=MakeNewTAP(pulseIter-pulseList.begin());
    outPulse->SetAmplitude(amplitude);
    outPulse->SetTime(time);
    //    outPulse->SetEnergy(energy); //AE (3rd Aug 2014): Commenting this out for the time being since we don't really know how we're doing calibration. The equation used before was:   energy = fECalibSlope * amplitude + fECalibOffset;
    // Add the pulse into the list
    analysedList.push_back(outPulse);
    
  }
  std::sort(analysedList.begin(), analysedList.end(), IsTimeOrdered);
  return 0;
}
ALCAP_TAP_GENERATOR(MaxBinNoShift,threshold_percent_of_range);
