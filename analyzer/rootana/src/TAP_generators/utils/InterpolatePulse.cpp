#include "InterpolatePulse.h"

#include <TH1D.h>
#include <stdexcept>
#include "TPulseIsland.h"
#include "TSetupData.h"
#include "SetupNavigator.h"
#include <algorithm> //std::generate_n

TH1D* functions::InterpolatePulse(
    const TPulseIsland* pulse, std::string histname,
    std::string histtitle, bool interpolate, int refine, bool ped_subtract) {

  // Get a few things first
  const std::string& bankname = pulse->GetBankName();
  const std::string detname = TSetupData::Instance()->GetDetectorName(bankname);
  const std::vector<int>& theSamples = pulse->GetSamples();
  int n_samples = theSamples.size();
  int n_bins = refine*n_samples; // number of bins in the template
  int n_bits = TSetupData::Instance()->GetNBits(bankname);
  double max_adc_value = std::pow(2, n_bits);
  int trigger_polarity = TSetupData::Instance()->GetTriggerPolarity(bankname);
  
  // Create the higher resolution histogram
  TH1D* hist = new TH1D(histname.c_str(), histtitle.c_str(), n_bins, 0, n_samples);

  double pedestal = SetupNavigator::Instance()->GetPedestal(detname);
  double pedestal_error = SetupNavigator::Instance()->GetNoise(detname);

  // Go through the bins in the high-resolution histogram
  // NB sample numbers go grom 0 to n-1 and bins go from 1 to n
  for (int i = 0; i < n_bins; ++i) {
    int bin = i+1; // bins go from 1 to n rather than 0 to n-1
    int sample_number = i / refine;
    double remainder = i % refine;
    double sample_value;

    // We may want to interpolate between the samples in the samples vector
    if (interpolate) {
      try {
        sample_value = theSamples.at(sample_number) 
                     + (remainder / refine)*(theSamples.at(sample_number+1) 
                                                     - theSamples.at(sample_number));
      }
      catch (const std::out_of_range& oor) { // if we'll be going out of range of the samples vector
        sample_value = theSamples.at(sample_number);
      }
    }
    else {
      sample_value = theSamples.at(sample_number);
    }

    if (sample_value >= max_adc_value-1) {
      hist->SetBinError( bin, max_adc_value); // set a large error bar so that template fitting can work
    }
    else {
      hist->SetBinError( bin, pedestal_error);
    }
    // Set the bin contents and bin error
    if (ped_subtract) {
      sample_value -= pedestal;
    }
    hist->SetBinContent( bin, trigger_polarity*sample_value);
  }
  hist->SetDirectory(0);

  return hist;
}

namespace{
  struct unique_n{
    int operator() () { return current+=step; }
    int current;
    const int step;
    unique_n(int b,int s):current(b),step(s){}
  };
}

void functions::FillBinLabels(double* labels, int size, int start, int increment){
   unique_n uniq(start,increment);
   std::generate_n(labels,size, uniq);
}
