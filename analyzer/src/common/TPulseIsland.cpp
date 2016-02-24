#include "TPulseIsland.h"

#include <cmath>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <numeric>

#include "TF1.h"
#include "TSetupData.h"

using std::vector;
using std::string;

TPulseIsland::TPulseIsland() : fSamples(), fTimeStamp(0), fBankName(""), fTDCTime(-1), fPSD_parameter(-1), fVetoPulse(false), fPileupPulse(false), fDoublePulse(false) {
}

TPulseIsland::TPulseIsland(int timestamp, const vector<int>::const_iterator& first,
        const vector<int>::const_iterator& last, string bank_name) :
  fSamples(first,last), fTimeStamp(timestamp), fBankName(bank_name), fTDCTime(-1), fPSD_parameter(-1), fVetoPulse(false), fPileupPulse(false), fDoublePulse(false) {
}

TPulseIsland::TPulseIsland(int timestamp, const vector<int>& samples_vector, string bank_name) :
  fSamples(samples_vector), fTimeStamp(timestamp), fBankName(bank_name), fTDCTime(-1), fPSD_parameter(-1), fVetoPulse(false), fPileupPulse(false), fDoublePulse(false) {
}

void TPulseIsland::Reset(Option_t* o) {
  fTimeStamp = 0;
  fSamples.clear();
  fBankName = "";
  fTDCTime = -1;
  fPSD_parameter = -1;
  fVetoPulse = false;
  fPileupPulse = false;
  fDoublePulse = false;
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

  double pedestal = GetPedestal(10);
  int peak_sample_element = GetPeakSample();

  return ( GetTriggerPolarity()*(fSamples.at(peak_sample_element) - pedestal) );
}

double TPulseIsland::GetPulseTime() const {
  return fTimeStamp * GetClockTickInNs();
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

int TPulseIsland::GetPulseIntegral() const {
  return GetTriggerPolarity() *
    std::accumulate(fSamples.begin(), fSamples.end(),
		    fSamples.size()*GetPedestal(10));
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

double TPulseIsland::GetFitMax() const {
  double pedestal = GetPedestal(10);
  int nSamp = fSamples.size();
  int polarity = GetTriggerPolarity();

  TH1F* hpulse = new TH1F("hpulse", "waveform", nSamp, 0, nSamp);
  for(int i = 0; i < nSamp; i++)
    hpulse->SetBinContent(i, polarity *(fSamples.at(i) - pedestal));

  int peak = GetPeakSample();
  TF1* fit1 = new TF1("fit1", "gaus", 0, nSamp);
  fit1->SetParameters(fSamples.at(peak), peak, 2);
  if(peak < 2 || peak > nSamp - 2){
    delete hpulse;
    delete fit1;
    return 0;
  }
  hpulse->Fit("fit1", "Q", "", peak-2, peak+2);

  double max = fit1->GetParameter(0);
  delete hpulse;
  delete fit1;

  return max;
}



double TPulseIsland::GetIntegral() const {
  std::string det = TSetupData::Instance()->GetDetectorName(fBankName);
  int polarity = TSetupData::Instance()->GetTriggerPolarity(fBankName);
  int nSamp = fSamples.size();
  float pedBegin = 0, pedEnd = 0, pedBeginSamp = 0, pedEndSamp = 0;
  float pedSlope = 0, pedInt = 0, pedMid = 0;
  float integral = 0, integral_ps = 0;

  //integrate the pedestal
  for(std::vector<int>::const_iterator pulse_begin = fSamples.begin() + 1; pulse_begin != fSamples.begin() + 9; pulse_begin++){
    pedBegin += (*pulse_begin);
    pedBeginSamp += 1;
  }
  for(std::vector<int>::const_iterator pulse_end = fSamples.end() - 9; pulse_end != fSamples.end(); pulse_end++){
    pedEnd += (*pulse_end);
    pedEndSamp += 1;
  }
  pedBegin /= pedBeginSamp;
  pedEnd /= pedEndSamp;
  pedSlope = (pedEnd - pedBegin)/(float)nSamp;
  pedMid = pedSlope * ((float)nSamp/2) + pedBegin;
  if(det == "TSc"){
    pedBegin = GetPedestal(4);
    pedMid = pedBegin; pedEnd = pedBegin;   
  }
  pedInt = (float)(nSamp-1)/6 * (pedBegin + (4*pedMid) + pedEnd);

  //integrate the wave
  int nInt = 0;
  int lastSamp = 0;
  for(int i = 0; i+2 < nSamp; i += 2){
    integral += (fSamples.at(i) + (4*fSamples.at(i+1)) + fSamples.at(i+2))/3;
    nInt += 2;
    lastSamp = i+2;
    }
  if(nSamp % 2 == 0){
    integral += 0.5 * (fSamples.at(lastSamp) + fSamples.at(lastSamp + 1));
    nInt ++;
    lastSamp++;
  }
  integral_ps = polarity * (integral - pedInt);

  return integral_ps;
}
