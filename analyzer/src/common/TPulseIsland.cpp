#include "TPulseIsland.h"

#include <cmath>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <numeric>

#include "TF1.h"

using std::vector;
using std::string;

TPulseIsland::TPulseIsland() : fSamples(), fTimeStamp(0), fBankName("") {
}

TPulseIsland::TPulseIsland(int timestamp, const vector<int>::const_iterator& first,
        const vector<int>::const_iterator& last, string bank_name) :
  fSamples(first,last), fTimeStamp(timestamp), fBankName(bank_name) {
}

TPulseIsland::TPulseIsland(int timestamp, const vector<int>& samples_vector, string bank_name) :
  fSamples(samples_vector), fTimeStamp(timestamp), fBankName(bank_name) {
}

void TPulseIsland::Reset(Option_t* o) {
  fTimeStamp = 0;
  fSamples.clear();
  fBankName = "";
}

// GetAmplitude()
// -- Gets the amplitude of the pulse
double TPulseIsland::GetAmplitude() const {

  if (TSetupData::Instance()->IsFast(fBankName))
    return GetFastPulseAmplitude();
  else
    return GetSlowPulseAmplitude();

}

// GetFastPulseAmplitude()
// -- Gets the amplitude for the fast pulse
// -- Anyone can play around with algorithms in here provided they are on their own branch
double TPulseIsland::GetFastPulseAmplitude() const {

  double pedestal = GetPedestal(10);
  int peak_sample_element = GetPeakSample();

  return ( GetTriggerPolarity()*(fSamples.at(peak_sample_element) - pedestal) );
}

double TPulseIsland::GetSlowPulseAmplitude() const {

  double pedestal = GetPedestal(10);
  int peak_sample_element = GetPeakSample();

  return ( GetTriggerPolarity()*(fSamples.at(peak_sample_element) - pedestal) );
}

double TPulseIsland::GetPulseHeight() const {

  //double pedestal = GetPedestal(10);
  int peak_sample_element = GetPeakSample();

  return ( (fSamples.at(peak_sample_element) ) );
}

double TPulseIsland::GetPulseTime() const {
  return fTimeStamp * GetClockTickInNs();
}

double TPulseIsland::GetCFTime(double frac) const {	
  const int ped = GetPedestal(10), pol = GetTriggerPolarity();

  const std::vector<int>::const_iterator
    b = fSamples.begin(), e = fSamples.end();
  std::vector<int>::const_iterator m = pol > 0 ?
    std::max_element(b, e) : std::min_element(b, e);
  const int amp = *m;

  const double cf = pol > 0 ?
    frac*(amp-ped) + ped :
    (ped-amp)*(1.-frac) + amp;

  if (pol > 0) {
    while (m != b && *--m > cf){};
  } else {
    while (m != b && *--m < cf){};
  }

  double dx = m-b;
  if (*(m+1) != *m)
    dx += (cf - *m)/(*(m+1) - *m);

  return (dx + fTimeStamp);
}

// GetPulseWaveform()
// -- Fills a histogram with all the samples and returns it
TH1I* TPulseIsland::GetPulseWaveform(std::string histname, std::string histtitle) const {

  TH1I* hWaveform = new TH1I(histname.c_str(), histtitle.c_str(), fSamples.size(),0,fSamples.size());

  // Loop over the samples and fill the histogram
  for (std::vector<int>::const_iterator sampleIter = fSamples.begin(); sampleIter != fSamples.end(); sampleIter++) {
    hWaveform->Fill(sampleIter - fSamples.begin(), *sampleIter);
  }

  return hWaveform;
}

double TPulseIsland::GetPulseIntegral() const {
  int pedestal = GetPedestal(); //printf(" ped %d pol %d\n", pedestal,GetTriggerPolarity());
  double sum=0.;
  for(int i = 0; i < fSamples.size(); i++)
  {
    sum = sum + GetTriggerPolarity()*(fSamples.at(i)-pedestal); 
  }
//  return GetTriggerPolarity() * std::accumulate(fSamples.begin(), fSamples.end(),  pedestal);
  //printf("polarity %d, sum %f , pedestal %d , PulseHeight %f\n", GetTriggerPolarity(), sum, pedestal,GetPulseHeight()); 
  return sum;
}


int TPulseIsland::GetPeakSample() const {

  std::vector<int>::const_iterator peak;
  if(GetTriggerPolarity() == 1)
    peak = std::max_element(fSamples.begin(), fSamples.end());
  else
    peak = std::min_element(fSamples.begin(), fSamples.end());

  return peak - fSamples.begin();
}

double TPulseIsland::GetPedestal(int nPedSamples) const {
  double ped = 0.0;

  //old way, set manually:
  //ped = TSetupData::Instance()->GetPedestal(fBankName);

  //loop over a few ped samples from the beginning of the waveform:
  double sum = 0.0;
  for (std::vector<int>::const_iterator sampleIter = fSamples.begin(); sampleIter<fSamples.begin()+nPedSamples; sampleIter++) {
     sum += *sampleIter;
  }
  ped = sum/nPedSamples;

  return ped;
}

