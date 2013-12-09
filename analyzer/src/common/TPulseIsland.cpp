#include "TPulseIsland.h"

#include <cmath>
#include <sstream>
#include <cstdlib>

#include "TF1.h"

using std::vector;
using std::string;

TPulseIsland::TPulseIsland()
{
  Reset();
}

TPulseIsland::TPulseIsland(
  int timestamp, const vector<int>& samples_vector,
  double clock_tick_in_ns, double adc_value_in_MeV, string bank_name,int pol)
{
  Reset();
  fTimeStamp = timestamp;
  fSamples = samples_vector;
  fClockTickInNs = clock_tick_in_ns;
  fADCValueInMeV = adc_value_in_MeV;
  fBankName = bank_name;
}

void TPulseIsland::Reset(Option_t* o)
{
  fTimeStamp = 0;
  fSamples.clear();
  fClockTickInNs = 0.0;
  fBankName = "";
}

// GetPulseHeight()
// -- Gets the position of the peak from GetPeakSample() and then returns the pulse height
double TPulseIsland::GetPulseHeight() const {
  
  double pedestal = GetPedestal(10);
  int peak_sample_element = GetPeakSample();
  /*  std::stringstream histname;
  histname << "waveform" << std::rand()%1000;
  TH1I* hWaveform  = GetPulseWaveform(histname.str().c_str(), histname.str().c_str()); // get the histogram for fitting purposes

  TF1* gaussian = new TF1("gaus", "[0]*TMath::Gaus(x, [1], [2], 0) + [3]", peak_sample_element-10, peak_sample_element+10);
  gaussian->SetParameter(0, -1000);
  gaussian->SetParameter(1, hWaveform->GetMean());
  gaussian->SetParameter(2, hWaveform->GetRMS());
  gaussian->FixParameter(3, pedestal);
                
  gaussian->SetLineColor(kRed);
  gaussian->SetLineWidth(2);
  hWaveform->Fit("gaus", "QR");
  
  delete hWaveform;*/
  // Go through the samples and get the samples with the largest difference between it and the pedestal
  // (should take into account both positive and negative pulses)

  return ( GetTriggerPolarity()*GetBoardPolarity()*(fSamples.at(peak_sample_element) - pedestal) * fADCValueInMeV);
}

// GetPulseTime()
// -- Gets the position of the peak from GetPeakSample() and adds that number of elements to the time stamp
// -- It also calibrates the time to ns using fClockTickInNs
double TPulseIsland::GetPulseTime() const {

  return (fTimeStamp + GetPeakSample()) * fClockTickInNs;
}

// GetPulseWaveform()
// -- Fills a histogram with all the samples and returns it
TH1I* TPulseIsland::GetPulseWaveform(std::string histname, std::string histtitle) const {

  TH1I* hWaveform = new TH1I(histname.c_str(), histtitle.c_str(), 200,0,200);

  // Loop over the samples and fill the histogram
  for (std::vector<int>::const_iterator sampleIter = fSamples.begin(); sampleIter != fSamples.end(); sampleIter++) {
    hWaveform->Fill(sampleIter - fSamples.begin(), *sampleIter);
  }

  return hWaveform;
}


// GetPeakSample()
// -- Goes through the samples and finds the element with the largest difference from the pedestal
//    so it should take into account both positive and negative pulses
// -- It returns its position in the fSamples vector
int TPulseIsland::GetPeakSample() const {

  double pedestal = GetPedestal(10);
  int trigger_polarity=GetTriggerPolarity();
  int board_polarity=GetBoardPolarity();
  int peak_sample_value = trigger_polarity*board_polarity*pedestal;
  int peak_sample_pos = 0;
  printf("Bank Name: %s\n", fBankName.c_str());
  printf("Trigger Pol: %d, Board Pol: %d\n", trigger_polarity, board_polarity);
  for (std::vector<int>::const_iterator sampleIter = fSamples.begin(); sampleIter != fSamples.end(); sampleIter++) {
  
    int this_height = trigger_polarity*board_polarity*(*(sampleIter) - pedestal);
    if ( this_height > peak_sample_value ) {
      peak_sample_value = this_height;
      peak_sample_pos = sampleIter - fSamples.begin();
    }
    printf("Current Height: %d, Peak Height: %d\n", this_height, peak_sample_value);
  }

  return peak_sample_pos;
}

// GetPedestal()
// -- Calculates the pedestal for this TPulseIsland using the given number of bins
double TPulseIsland::GetPedestal(int nPedSamples) const {

  // Hard-coding pedestals for the time being....
  if ( fBankName == "CeUH" )
    return 8100;
  else if ( fBankName == "CfUH" )
    return 8680;
  else
    return 2750;  // Fixed pedestal

  if (nPedSamples > fSamples.size())
    nPedSamples = 2;

  double pedestal = 0;
  for (int iSample = 0; iSample < nPedSamples; iSample++) {
    pedestal += fSamples.at(iSample);
  }

  pedestal /= nPedSamples;

  return pedestal;
}
