#include "PulseEstimate.hh"

#include "TH1.h"

PulseEstimate::PulseEstimate() : fPedestal(0), fPolarity(0) {}

void PulseEstimate::Estimate(TH1* pulses, TH1* timing) {
  /* Get pedestal */
  if (pulses) {
    // First reduce range slightly; specifically Ge preamp reset caused flatlining
    static const int nBinsIgnore = 5; // This number is arbitrary. Logically it should be 1, maybe 2, but 5 can't hurt
    int nbins = pulses->GetXaxis()->GetNbins();
    pulses->GetXaxis()->SetRange(nBinsIgnore, nbins - nBinsIgnore);
    // Get the pedestal
    fPedestal = (int)pulses->GetBinCenter(pulses->GetMaximumBin());
    // Reset bin range
    pulses->GetXaxis()->SetRange();

    /* Get polarity */
    int min = 0, max = 0;
    bool min_found = false, max_found = false;
    // Again, ignore 5 bin on each side to get rid of flatlines
    for (int i = nBinsIgnore; i <= nbins - nBinsIgnore; ++i) {
      if (!min_found && pulses->GetBinContent(i) > 0.) {
	min_found = true;
	min = i;
      }
      if (!max_found && pulses->GetBinContent(nbins - i) > 0.) {
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
    fPedestal = 0;
    fPolarity = 1;
  }

  /* Get Timing */
  // Rebin until we have plenty of samples at the peak
  if (timing) {
    int nRebins = 0;
    static int min_time_correlation_height = 10;
    while (timing->GetBinContent(timing->GetMaximumBin()) < min_time_correlation_height && nRebins++ < 3)
      timing->Rebin();
    fOffset = timing->GetBinCenter(timing->GetMaximumBin());
  } else {
    // If no timing, default to zero offset
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
