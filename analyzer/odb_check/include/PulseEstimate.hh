/***********************************
Class: PulseEstimate
Author: John R. Quirk

This class takes data quality histograms from an
alcapana production and predicts what certain
correct ODB values are.
***********************************/

#ifndef PULSEESTIMATE_H__
#define PULSEESTIMATE_H__

#include "TH1.h"

class PulseEstimate {

private:
  int fPedestal;
  int fPolarity;
  int fOffset;

public:
  PulseEstimate();

  /* Estimates parameters of shapes histogram */
  /*
    Pedestal: Looks for the most common ADC value
    Polarity: Looks at the max and min ADC value
              and whichever is further from the
              estimated pedestal sets the polarity.
    Offset:   Looks for peak in timing correlation
              between MuSC and this detector
  */
  void Estimate(TH1* pulses, TH1* timing);
  int GetPedestal() const;
  int GetPolarity() const;
  int GetOffset() const;
};

#endif
