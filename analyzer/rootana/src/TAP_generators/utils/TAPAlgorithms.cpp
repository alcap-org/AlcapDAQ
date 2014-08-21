#include "TAPAlgorithms.h"
#include "TSetupData.h"
#include "SetupNavigator.h"

#include <iostream>
#include <algorithm>
#include <cmath>

#include <iostream>

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

double Algorithm::ConstantFractionTime::Process(const TPulseIsland* tpi) {

  const std::vector<int>& samps = tpi->GetSamples();
  std::vector<int>::const_iterator b = samps.begin(), e = samps.end();

  std::vector<int>::const_iterator m = trigger_polarity > 0 ? std::max_element(b, e) : std::min_element(b, e);
  const int amp = *m;
  const unsigned int cf = trigger_polarity > 0 ?
      (unsigned int)(constant_fraction*(double)(amp-pedestal)) + pedestal :
      (unsigned int)((double)(pedestal-amp)*(1.-constant_fraction) + amp);
  while (m != b && (trigger_polarity > 0 ? *--m > (int)cf : *--m < (int)cf));
  double dx = (double)(m-b);
  if (*(m+1) != *m)
    dx += (double)((int)cf - *m)/(double)(*(m+1) - *m);

  return (dx + (double)tpi->GetTimeStamp()) * clock_tick_in_ns - time_shift;
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
