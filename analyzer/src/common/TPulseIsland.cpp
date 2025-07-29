#include "TPulseIsland.h"

#include <cmath>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <numeric>

#include "TF1.h"
#include "TSetupData.h"

//extern TSetupData* gSetup;

/* AlCap includes */
#include "tOffsets.h"

using std::vector;
using std::string;

TPulseIsland::TPulseIsland() : gSetup(nullptr), fSamples(), fTimeStamp(0), fTimeStampCF(-1), fBankName(""), fTDCTime(-1), fPSD_parameter(-1), fVetoPulse(false), fPileupPulse(false), fDoublePulse(false), fEnergy(0) {
}

TPulseIsland::TPulseIsland(int timestamp, const vector<int>::const_iterator& first,
        const vector<int>::const_iterator& last, string bank_name, TSetupData* setup) :
  gSetup(setup), fSamples(first,last), fTimeStamp(timestamp), fTimeStampCF(-1), fBankName(bank_name), fTDCTime(-1), fWFDTime(-1), fPSD_parameter(-1), fVetoPulse(false), fPileupPulse(false), fDoublePulse(false), fEnergy(0) {
}

TPulseIsland::TPulseIsland(int timestamp, const vector<int>& samples_vector, string bank_name, TSetupData* setup) :
  gSetup(setup), fSamples(samples_vector), fTimeStamp(timestamp), fTimeStampCF(-1), fBankName(bank_name), fTDCTime(-1), fWFDTime(-1), fPSD_parameter(-1), fVetoPulse(false), fPileupPulse(false), fDoublePulse(false), fEnergy(0) {
}

// CF time bool
TPulseIsland::TPulseIsland(int timestamp, const vector<int>& samples_vector, string bank_name, bool do_CF, TSetupData* setup) :
  gSetup(setup), fSamples(samples_vector), fTimeStamp(timestamp), fTimeStampCF(-1), fBankName(bank_name), fTDCTime(-1), fWFDTime(-1), fPSD_parameter(-1), fVetoPulse(false), fPileupPulse(false), fDoublePulse(false), fEnergy(0) {

    if (do_CF) {
      string detname = gSetup->GetDetectorName(bank_name);
      SetTimeStampCF(0.5, detname);
    }

}

void TPulseIsland::Reset(Option_t* o) {
  fTimeStamp = 0;
  fTimeStampCF = -1;
  fSamples.clear();
  fBankName = "";
  fTDCTime = -1;
  fWFDTime = -1;
  fPSD_parameter = -1;
  fVetoPulse = false;
  fPileupPulse = false;
  fDoublePulse = false;
  fEnergy = 0;
}

// GETTERS
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

double TPulseIsland::GetPulseTimeCF() const {
  return fTimeStampCF * GetClockTickInNs();
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

double TPulseIsland::GetEnergyAmp(int amp) const{
  std::string det = TSetupData::Instance()->GetDetectorName(fBankName);
  double energy = 0;
  if(det == "NdetD"){ energy = ((double)amp * 0.0003999) + 0.008234;  }
  if(det == "NdetU"){ energy = ((double)amp * 0.0004015) + 0.009037;  }
  if(det == "GeCHEH"){ energy = ((double)amp * 0.0001517) - 0.0003119;  }
  if(det == "GeCHEL"){ energy = ((double)amp * 0.0003838) - 0.000629;    }
  if(det == "LaBr3"){ energy = ((double)amp * 0.00163022) - 0.00836618;  } 
  if(det == "GeCHT"){ energy = (double)amp;  }
  return energy;
}


double TPulseIsland::GetEnergyInt(double Int) const{
  std::string det = TSetupData::Instance()->GetDetectorName(fBankName);
  double energy = 0;
  if(det == "NdetD"){ energy = (0.0000686 * (double)Int) + 0.02117;  }
  if(det == "NdetU"){ energy = (0.00006766 * (double)Int) + 0.02358;  }
  if(det == "GeCHEH"){ energy = (0.000002022 * (double)Int) + 0.001323;  }
  if(det == "GeCHEL"){ energy = (0.000005112 * (double)Int) + 0.00150;  }
  if(det == "LaBr3"){ energy = 0.0000823 * (double)Int;  } 
  if(det == "GeCHT"){ energy = Int;  }
  return energy;
}

double TPulseIsland::GetEnergyFit(double fit) const{
  std::string det = TSetupData::Instance()->GetDetectorName(fBankName);
  float energy = 0;
  if(det == "NdetD"){ energy = (fit * 0.0003914) + 0.02889;  }
  if(det == "NdetU"){ energy = (fit * 0.0004381) + 0.02568;  }
  if(det == "GeCHEH"){ energy = (fit * 0.0001518) + 0.000299;  }
  if(det == "GeCHEL"){ energy = (fit * 0.0003841) - 0.0003155;    }
  if(det == "LaBr3"){ energy = (fit * 0.00163022) - 0.00836618;  } 
  if(det == "GeCHT"){ energy = fit;  }
  return energy;
}

// SETTERS
void TPulseIsland::SetTimeStampCF(double frac, string detname) {
  // FIXME! I think this is the same for each detector but may want an explicit check
  //const int pretrigger = 40;
  //const double pretrigger = 40;
  // get pedestal, amp, polarity
  const double ped = GetPedestal(10);
  const int pol = GetTriggerPolarity();
  const std::vector<int> samples = fSamples; // ADC indexed by tick number
  double amp = GetAmplitude();
  double cf = frac * amp; // threshold value

  // loop until hitting threhsold
  int t1 = 0;
  int t2 = 1;
  double s1 = (double)pol * ((double)samples[t1] - ped);
  double s2 = (double)pol * ((double)samples[t2] - ped);

  // loop until we bracket the cf value
  while (t2 < (int)samples.size() && s2 < cf) {
    ++t1;
    ++t2;
    s1 = (double)pol * ((double)samples[t1] - ped);
    s2 = (double)pol * ((double)samples[t2] - ped);
  }

  // interpolate
  double t_CF;
  if (t2 >= (int)samples.size() || s2 == s1) {
    t_CF = (double)t1;
  } else {
    // diff from cf threshold and value at leading edge, divided by slope gives interpolation correction
    t_CF = (cf - s1) / (s2 - s1) + (double)t1;
  }

  // report timestamp w.r.t. DAQ fTimeStamp, which is triggered at absolute value of the pulse
  //fTimeStampCF = fTimeStamp + t_CF;
  // correct for pretrigger
  // FIXME! I think this is the same for each detector but may want an explicit check
  //fTimeStampCF = fTimeStamp - (double)AlCap::pretrigger + t_CF;
  // double pretrigger = AlCap::fDetectorToPretriggerMap[detname]; // DOES NOT EXPLICITLY CHECK THAT KEY EXISTS!
  double pretrigger = AlCap::fDetectorToPretriggerMap.at(detname); // throws out_of_range if key is missing
  fTimeStampCF = fTimeStamp - pretrigger + t_CF;

}
