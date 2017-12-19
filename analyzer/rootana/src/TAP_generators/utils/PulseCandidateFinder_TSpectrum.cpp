#include "PulseCandidateFinder_TSpectrum.h"
#include "SetupNavigator.h"
#include "definitions.h"
#include "debug_tools.h"
#include "AlcapExcept.h"

#include <iostream>
#include <iterator>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cmath>

#include <TSQLiteServer.h>
#include <TSQLiteResult.h>
#include <TSQLiteRow.h>

MAKE_EXCEPTION(PulseCandidateFinder_TSpectrum, Base);
MAKE_EXCEPTION(UnspecifiedChannel,PulseCandidateFinder_TSpectrum);

/// The constructor just sets all the parameter values
PulseCandidateFinder_TSpectrum::PulseCandidateFinder_TSpectrum():
    fChannel(IDs::kErrorDetector),
    fTSpectrum(new TSpectrum()) {
}

PulseCandidateFinder_TSpectrum::PulseCandidateFinder_TSpectrum(std::string detname, modules::options* opts):
    fTSpectrum(new TSpectrum()) {

  SetChannel(detname);
}

void PulseCandidateFinder_TSpectrum::SetChannel(const std::string& detname){
  fChannel=detname;
}

/// FindPulseCandidates()
/// Finds the pulse candidates on the given TPulseIsland
// Pass in hPulse if you already have a histogram of the pulse (e.g. ExportPulse)
void PulseCandidateFinder_TSpectrum::FindPulseCandidates(const TPulseIsland* pulse, TH1* hPulse) {

    if(!fChannel.isValid()){
        std::cout<<"Channel for PCF is not set, make sure to use "
                   "SetChannel() if you used the defualt constructor"<<std::endl;
        throw Except::UnspecifiedChannel();
    }

  // Clear the vector of pulse candidate locations first since it will still contain them from previous pulses
  fPulseCandidateLocations.clear();
  fPulseCandidatePeakLocations.clear();

  fPulseIsland = pulse;

  // Make a histogram of the pulse
  std::string bankname = TSetupData::Instance()->GetBankName(fChannel.str());
  int downsampling = TSetupData::GetDownSampling(bankname.c_str(), SetupNavigator::Instance()->GetRunNumber());
  size_t num_samples = fPulseIsland->GetPulseLength();
  double min=0;
  double max=num_samples*TSetupData::Instance()->GetClockTick(bankname)*downsampling;
  double pedestal = SetupNavigator::Instance()->GetPedestal(fChannel.str());

  if (!hPulse) {
    hPulse = new TH1F("Pulse", "", num_samples,min,max);
    hPulse->GetXaxis()->SetNoExponent(true);
    hPulse->SetDirectory(0);
    hPulse->SetXTitle("pulse time [ns]");
    hPulse->SetYTitle("pedestal subtracted [ADC]");
    
    size_t bin=0;
    for ( size_t i=0;i <(size_t)fPulseIsland->GetPulseLength(); ++i) {
      bin=i+1;
      hPulse->SetBinContent(bin, fPulseIsland->GetSamples().at(i)-pedestal);
      hPulse->SetBinError(bin, 0);//pedestal_error);
    }
  }

  // Check if this pulse overflowed the digitiser
  //  if (!CheckDigitiserOverflow()) {
  double sigma = 2;
  double threshold = 0.05;
  fTSpectrum->Search(hPulse, sigma, "noMarkov nobackground nodraw", threshold);

  if (fTSpectrum->GetNPeaks() == 0) {
    //    fTSpectrum->Search(hPulse, sigma, "nodraw", threshold);
    std::cout << "PulseCandidateFinder_TSpectrum: No pulses found!!" << std::endl;
  }

  // Just use empty locations because we only want this to count the peaks...
  for (int i_peak = 0; i_peak < fTSpectrum->GetNPeaks(); ++i_peak) {
    PeakLocation peak_location;
    peak_location.time = *(fTSpectrum->GetPositionX() + i_peak);
    peak_location.amplitude = *(fTSpectrum->GetPositionY() + i_peak);
    //    std::cout << "Adding peak location t = " << peak_location.time << ", A = " << peak_location.amplitude << std::endl;
    fPulseCandidatePeakLocations.push_back(peak_location);

    Location location;
    location.start = 0;
    location.stop = hPulse->GetNbinsX();
    fPulseCandidateLocations.push_back(location);
  }
}

PulseCandidateFinder_TSpectrum::~PulseCandidateFinder_TSpectrum() {
}

/// GetPulseCandidates()
/// Fills a vector of the TPulseIslands of the pulse candidates that have been found
void PulseCandidateFinder_TSpectrum::GetPulseCandidates(std::vector<TPulseIsland*>& pulse_candidates)const {

  // Loop through the pulse candidate locations
  std::vector<int>::const_iterator start_new, stop_new;
  for (std::vector<Location>::const_iterator locationIter = fPulseCandidateLocations.begin();
          locationIter != fPulseCandidateLocations.end(); ++locationIter) {
    
    // Get iterator to the start locations
    start_new=fPulseIsland->GetSamples().begin();
    std::advance(start_new,locationIter->start);

    // Get iterator to the stop locations
    stop_new=fPulseIsland->GetSamples().begin();
    std::advance(stop_new,locationIter->stop);

    // Get the bank name and work out what the new timestamp will be
    std::string bankname = fPulseIsland->GetBankName();
    int new_timestamp = fPulseIsland->GetTimeStamp() + locationIter->start;

    // Make the new TPI
    int index=locationIter-fPulseCandidateLocations.begin();
    if((int)pulse_candidates.size() <= index || !pulse_candidates.at(index)){
        TPulseIsland* pulse_island = new TPulseIsland(new_timestamp, start_new, stop_new, bankname);
        pulse_candidates.push_back(pulse_island);
    }else{
        TPulseIsland* pulse_island=pulse_candidates.at(index);
        pulse_island->SetSamples(start_new,stop_new);
        pulse_island->SetTimeStamp(new_timestamp);
        pulse_island->SetBankName(bankname);
    }
  }
  for(unsigned int i=fPulseCandidateLocations.size();i< pulse_candidates.size();++i){
      pulse_candidates.at(i)->Reset();
  }
}

bool PulseCandidateFinder_TSpectrum::CheckDigitiserOverflow() {
  const std::vector<int>& samples = fPulseIsland->GetSamples();
  unsigned int n_samples = samples.size();

  std::string bankname = fPulseIsland->GetBankName();
  int n_bits = TSetupData::Instance()->GetNBits(bankname);
  double max_adc_value = std::pow(2, n_bits);
  bool overflowed = false;

  // Loop through the samples
  for (unsigned int i = 0; i < n_samples; ++i) {
    if (samples[i] >= max_adc_value) {
      overflowed = true;
      break;
    }
  }

  return overflowed;
}
