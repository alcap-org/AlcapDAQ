#include "TAPGeneratorFactory.h"
#include "MaxBinAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include "EventNavigator.h"
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

//----------------------------------------------------------------------
void MaxBinAPGenerator::SetBankInfo(std::string bankname){
  const TSetupData* fSetup = EventNavigator::Instance().GetSetupData();
  fBankname=bankname;
  fPedestal = fSetup->GetPedestal(bankname);
  fTriggerPolarity = fSetup->GetTriggerPolarity(bankname);
  fECalibSlope = fSetup->GetADCSlopeCalib(bankname);
  fECalibOffset = fSetup->GetADCOffsetCalib(bankname);
  fClockTick = fSetup->GetClockTick(bankname);
  fTimeShift = fSetup->GetTimeShift(bankname);
  fDetName=fSetup->GetDetectorName(bankname);
}


//----------------------------------------------------------------------
int MaxBinAPGenerator::ProcessPulses(const PulseIslandList& pulseList,
                                     AnalysedPulseList& analysedList)
{
  SetBankInfo(pulseList.at(0)->GetBankName());
  
  double amplitude, time, integral, energy;
  TAnalysedPulse* outPulse;
  for (PulseIslandList::const_iterator pulseIter = pulseList.begin(); pulseIter != pulseList.end(); pulseIter++) {
    amplitude = 0;
    time = 0;
    integral = 0;
    energy = 0.;
    // Assume one TAnalysedPulse per TPulseIsland
    GetAllParameters_MaxBin(*pulseIter,amplitude,time,integral,energy);
    
    // Make the TAnalysedPulse pulse
    outPulse=MakeNewTAP(pulseIter-pulseList.begin());
    outPulse->SetAmplitude(amplitude);
    outPulse->SetTime(time);
    outPulse->SetEnergy(energy);
    // Add the pulse into the list
    analysedList.push_back(outPulse);
    
  }
  std::sort(analysedList.begin(), analysedList.end(), IsTimeOrdered);
  return 0;
}

//----------------------------------------------------------------------
void MaxBinAPGenerator::GetAllParameters_MaxBin(const TPulseIsland* pulse,
                                                double& amplitude, double& time,
                                                double& integral, double& energy)
{
  // First find the position of the peak
  std::vector<int> pulseSamples = pulse->GetSamples();  
  std::vector<int>::iterator peak_sample_pos;
  if (fTriggerPolarity == 1)
    peak_sample_pos = std::max_element(pulseSamples.begin(), pulseSamples.end());
  else
    peak_sample_pos = std::min_element(pulseSamples.begin(), pulseSamples.end());

  // Now assign the parameters
  amplitude = fTriggerPolarity*(*peak_sample_pos - fPedestal);
  time = ((pulse->GetTimeStamp() + (peak_sample_pos - pulseSamples.begin())) * fClockTick) - fTimeShift;
  integral = 0;
  energy = fECalibSlope * amplitude + fECalibOffset;
}

ALCAP_TAP_GENERATOR(MaxBin);
