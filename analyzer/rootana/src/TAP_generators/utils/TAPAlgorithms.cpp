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

  // Find the position of the peak samples
  if (trigger_polarity == 1)
    peak_sample_pos = std::max_element(pulseSamples.begin(), pulseSamples.end());
  else if (trigger_polarity == -1)
    peak_sample_pos = std::min_element(pulseSamples.begin(), pulseSamples.end());

  // Now calculate the amplitude
  double amplitude = trigger_polarity*(*peak_sample_pos - pedestal);

  return amplitude;
}

double Algorithm::MaxBinTime::operator() (const TPulseIsland* tpi) {

  // Get the samples and get an iterator ready to find the peak sample
  std::vector<int> pulseSamples = tpi->GetSamples();  
  std::vector<int>::iterator peak_sample_pos;

  // Find the position of the peak samples
  if (trigger_polarity == 1)
    peak_sample_pos = std::max_element(pulseSamples.begin(), pulseSamples.end());
  else if (trigger_polarity == -1)
    peak_sample_pos = std::min_element(pulseSamples.begin(), pulseSamples.end());

  // Now calculate the time
  double time = ((tpi->GetTimeStamp() + (peak_sample_pos - pulseSamples.begin())) * clock_tick_in_ns) - time_shift;

  return time;
}

double Algorithm::ConstantFractionTime::operator() (const TPulseIsland* tpi) {

  const std::vector<int>& samps = tpi->GetSamples();
  std::vector<int>::const_iterator b = samps.begin(), e = samps.end();

  std::vector<int>::const_iterator m = trigger_polarity > 0 ? std::max_element(b, e) : std::min_element(b, e);
  unsigned int amp = *m;
  unsigned int thresh = trigger_polarity > 0 ? (unsigned int)(th_frac*(double)(max_adc_value - pedestal) + pedestal) : (unsigned int)((1.-th_frac)*pedestal);
  unsigned int cf = trigger_polarity > 0 ? (unsigned int)(constant_fraction*(double)(amp-pedestal)) + pedestal : (unsigned int)((double)(pedestal-amp)*(1.-constant_fraction) + amp);

  double t;
  if ((trigger_polarity > 0 ? amp > thresh : amp < thresh) && (trigger_polarity > 0 ? amp < max_adc_value : amp > 0)) {
    std::vector<int>::const_iterator c = m;
    while ((trigger_polarity > 0 ? *--m > (int)cf : *--m < (int)cf) && m != b);
    if (*(m+1) == *m)
      t = (double)(m-b);
    else
      t = (double)((int)cf - *m)/(double)(*(m+1) - *m) + (double)(m-b);
  }

  double time = clock_tick_in_ns * (t + (double)tpi->GetTimeStamp() - time_shift);

  return time;
}

double Algorithm::SimpleIntegral::operator() (const TPulseIsland* tpi) {
  const std::vector<int>& samples = tpi->GetSamples();
  
  double length = samples.size();
  double tempint = 0;
  for(std::vector<int>::const_iterator sIt = samples.begin(); sIt != samples.end(); sIt++)
    tempint += *sIt;
  
  double integral = trigger_polarity * (tempint - (pedestal * length));

  return integral;
}
