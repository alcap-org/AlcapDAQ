#include "PulseEstimate.hh"

#include "TH2.h"
#include "TH1D.h"


PulseEstimate::PulseEstimate() : fPedestal(0), fPolarity(0) {}

void PulseEstimate::Estimate(TH2* pulses) {
  TH1D* proj_y = pulses->ProjectionY("_py",1);
  fPedestal = (int)proj_y->GetBinCenter(proj_y->GetMaximumBin());
  int min = 0, max = 0;
  bool min_found = false, max_found = false;
  int nbins = proj_y->GetNbinsX();
  for (int i = 1; i < nbins; ++i) {
    if (!min_found && proj_y->GetBinContent(i) > 0.) {
      min_found = true;
      min = i;
    }
    if (!max_found && proj_y->GetBinContent(nbins - i) > 0.) {
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
}

int PulseEstimate::GetPedestal() const {
  return fPedestal;
}

int PulseEstimate::GetPolarity() const {
  return fPolarity;
}
