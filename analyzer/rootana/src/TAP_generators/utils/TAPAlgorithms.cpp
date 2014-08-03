#include "TAPAlgorithms.h"
#include "TSetupData.h"
#include "SetupNavigator.h"

#include <iostream>
#include <algorithm>
#include <cmath>

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
  double pedestal = SetupNavigator::Instance()->GetPedestal(bankname);
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

double Algorithm::ConstantFractionTime::operator() (const TPulseIsland* tpi) {
  std::string bankname = tpi->GetBankName();

  std::vector<int> samps = tpi->GetSamples();
  std::vector<int>::iterator b = samps.begin(), e = samps.end();

  double ped = SetupNavigator::Instance()->GetPedestal(bankname);
  int pol = TSetupData::Instance()->GetTriggerPolarity(bankname);
  unsigned int max_adc_value = std::pow(2, TSetupData::Instance()->GetNBits(bankname)) - 1;

  std::vector<int>::iterator m = pol > 0 ? std::max_element(b, e) : std::min_element(b, e);
  unsigned int amp = *m;
  unsigned int thresh = pol > 0 ? (unsigned int)(th_frac*(double)(max_adc_value - ped) + ped) : (unsigned int)((1.-th_frac)*ped);
  unsigned int cf = pol > 0 ? (unsigned int)(constant_fraction*(double)(amp-ped)) + ped : (unsigned int)((double)(ped-amp)*(1.-constant_fraction) + amp);

  double t;
  if ((pol > 0 ? amp > thresh : amp < thresh) && (pol > 0 ? amp < max_adc_value : amp > 0)) {
    std::vector<int>::iterator c = m;
    while ((pol > 0 ? *--m > (int)cf : *--m < (int)cf) && m != b);
    if (*(m+1) == *m)
      t = (double)(m-b);
    else
      t = (double)((int)cf - *m)/(double)(*(m+1) - *m) + (double)(m-b);
  }

  double time = tpi->GetClockTickInNs() * (t + (double)tpi->GetTimeStamp()) - TSetupData::Instance()->GetTimeShift(tpi->GetBankName());

  return time;
}
