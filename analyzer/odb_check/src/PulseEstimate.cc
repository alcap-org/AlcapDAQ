#include "PulseEstimate.hh"

#include "TH1.h"

PulseEstimate::PulseEstimate() : fPedestal(0), fPolarity(0) {}

void PulseEstimate::Estimate(TH1* pulses, TH1* timing) {
  /* Get pedestal */
  fPedestal = (int)pulses->GetBinCenter(pulses->GetMaximumBin());

  /* Get polarity */
  int min = 0, max = 0;
  bool min_found = false, max_found = false;
  int nbins = pulses->GetNbinsX();
  for (int i = 1; i < nbins; ++i) {
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

  /* Get Timing */
  fOffset = timing->GetBinCenter(timing->GetMaximumBin());
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
