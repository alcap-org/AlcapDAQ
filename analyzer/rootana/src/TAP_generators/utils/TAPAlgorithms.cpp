#include "TAPAlgorithms.h"
#include "TSetupData.h"

#include <iostream>
#include <algorithm>

double Algorithm::MaxBinAmplitude::operator() (const TPulseIsland* tpi) {

  // Get the samples and get an iterator ready to find the peak sample
  std::vector<int> pulseSamples = tpi->GetSamples();  
  std::vector<int>::iterator peak_sample_pos;

  // Get the trigger polarity
  std::string bankname = tpi->GetBankName();
  int trigger_polarity = TSetupData::Instance()->GetTriggerPolarity(bankname);

  // Find the position of the peak samples
  if (trigger_polarity == 1)
    peak_sample_pos = std::max_element(pulseSamples.begin(), pulseSamples.end());
  else if (trigger_polarity == -1)
    peak_sample_pos = std::min_element(pulseSamples.begin(), pulseSamples.end());
  else
    std::cout << "Algorithm::MaxBinAmplitude: Trigger Polarity for channel " << bankname << " is not +- 1!" << std::endl;

  // Now calculate the amplitude
  double pedestal = TSetupData::Instance()->GetPedestal(bankname);
  double amplitude = trigger_polarity*(*peak_sample_pos - pedestal);

  return amplitude;
}

double Algorithm::MaxBinTime::operator() (const TPulseIsland* tpi) {

  // Get the samples and get an iterator ready to find the peak sample
  std::vector<int> pulseSamples = tpi->GetSamples();  
  std::vector<int>::iterator peak_sample_pos;

  // Get the trigger polarity
  std::string bankname = tpi->GetBankName();
  int trigger_polarity = TSetupData::Instance()->GetTriggerPolarity(bankname);

  // Find the position of the peak samples
  if (trigger_polarity == 1)
    peak_sample_pos = std::max_element(pulseSamples.begin(), pulseSamples.end());
  else if (trigger_polarity == -1)
    peak_sample_pos = std::min_element(pulseSamples.begin(), pulseSamples.end());
  else
    std::cout << "Algorithm::MaxBinTime: Trigger Polarity for channel " << bankname << " is not +- 1!" << std::endl;

  // Now calculate the time
  double clock_tick_in_ns = TSetupData::Instance()->GetClockTick(bankname);
  double time_shift = TSetupData::Instance()->GetTimeShift(bankname);
  double time = ((tpi->GetTimeStamp() + (peak_sample_pos - pulseSamples.begin())) * clock_tick_in_ns) - time_shift;

  return time;
}
