#include "TAPGeneratorFactory.h"
#include "MaxBinAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include "EventNavigator.h"
#include "SetupNavigator.h"
#include <algorithm>

// IsTimeOrdered()
// -- Returns true of the first pulse is before the second
// Static so that only this file sees it
// Useful for sorting the pulses
static bool IsTimeOrdered(TAnalysedPulse* a, TAnalysedPulse* b) {
  // Want a to be before b
  return ( a->GetTime() < b->GetTime() );
}

//======================================================================
MaxBinAPGenerator::MaxBinAPGenerator(TAPGeneratorOptions* opts)
  : TVAnalysedPulseGenerator("MaxBinAPGenerator", opts),
    fMaxBinAmplitude(SetupNavigator::Instance()->GetPedestal(GetChannel()), 
		     TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(GetChannel().str()))),
    fMaxBinTime(TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(GetChannel().str())),
		TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(GetChannel().str())),
		opts->GetDouble("time_shift", TSetupData::Instance()->GetTimeShift(TSetupData::Instance()->GetBankName(GetChannel().str())))) {

}


//----------------------------------------------------------------------
int MaxBinAPGenerator::ProcessPulses(const PulseIslandList& pulseList,
                                     AnalysedPulseList& analysedList)
{

  double amplitude, time;
  TAnalysedPulse* outPulse;

  for (PulseIslandList::const_iterator pulseIter = pulseList.begin(); pulseIter != pulseList.end(); pulseIter++) {
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
ALCAP_TAP_GENERATOR(MaxBin);
