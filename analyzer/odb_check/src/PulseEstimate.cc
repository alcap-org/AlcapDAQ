#include "PulseEstimate.hh"

#include "TH1.h"
#include "TH2.h"

#include <iostream>

PulseEstimate::PulseEstimate() : fPedestal(0), fPolarity(1), fOffset(0) {}

void PulseEstimate::Estimate(TH2* pulses, TH1* timing) {
  /* Get pedestal and polarity */
  if (pulses && pulses->GetEntries()) {
    int nPreSamples = 10; // Number of presamples to project
    // Project out first few samples to get pedestal
    // Project everything to get polarity
    TH1* pulses_proj_ped = pulses->ProjectionY("_ped", 1, nPreSamples);
    TH1* pulses_proj_pol = pulses->ProjectionY("_pol");

    /* Get pedestal */
    // First reduce range slightly; specifically Ge preamp reset caused flatlining
    // We want to miss underflow/overflow due to this
    static const int nBinsIgnore = 5; // This number is arbitrary. Logically it should be 1, maybe 2, but 5 can't hurt
    int nbins = pulses_proj_ped->GetXaxis()->GetNbins();
    pulses_proj_ped->GetXaxis()->SetRange(nBinsIgnore, nbins - nBinsIgnore);
    // Get the pedestal
    fPedestal = (int)pulses_proj_ped->GetBinCenter(pulses_proj_ped->GetMaximumBin());
    // Reset bin range
    pulses_proj_ped->GetXaxis()->SetRange();

    /* Get polarity */
    int min = 0, max = 0;
    bool min_found = false, max_found = false;
    // Again, ignore 5 bin on each side to get rid of flatlines
    for (int i = nBinsIgnore; i <= nbins - nBinsIgnore; ++i) {
      if (!min_found && pulses_proj_pol->GetBinContent(i) > 0.) {
	min_found = true;
	min = i;
      }
      if (!max_found && pulses_proj_pol->GetBinContent(nbins - i) > 0.) {
	max_found = true;
	max = nbins - i;
      }
      if (min_found && max_found)
	break;
    }
    if (max - fPedestal > fPedestal - min)
      fPolarity = 1;
    else
      fPolarity = -1;
  } else {
    // If no pulses, default to positive polarity and zero pedestal
    std::cout << "PulseEstimate ERROR: Cannot estimate nonexistent or empty shapes histogram!" << std::endl;
    fPedestal = 0;
    fPolarity = 1;
  }

  /* Get Timing */
  // Rebin until we have plenty of samples at the peak
  if (timing && timing->GetEntries()) {
    int nRebins = 0;
    static int min_time_correlation_height = 10;
    while (timing->GetBinContent(timing->GetMaximumBin()) < min_time_correlation_height && nRebins++ < 3)
      timing->Rebin();
    fOffset = timing->GetBinCenter(timing->GetMaximumBin());
  } else {
    // If no timing, default to zero offset
    std::cout << "PulseEstimate ERROR: Cannot estimate nonexistent or empty timing histogram!" << std::endl;
    fOffset = 0;
  }
}

int PulseEstimate::GetPedestal() const {
  return fPedestal;
}

int PulseEstimate::GetPolarity() const {
  return fPolarity;
}

int PulseEstimate::GetOffset() const {
  return fOffset;
}
