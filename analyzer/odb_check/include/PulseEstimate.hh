#ifndef PULSEESTIMATE_H__
#define PULSEESTIMATE_H__

#include "TH2.h"

class PulseEstimate {
private:
  int fPedestal;
  int fPolarity;
public:
  PulseEstimate();
  /* Estimates parameters of shapes histogram */
  /*
    Pedestal: Looks for the most common ADC value
    Polarity: Looks at the max and min ADC value
              and whichever is further from the
              estimated pedestal sets the polarity.
  */
  void Estimate(TH2* pulses);
  int GetPedestal() const;
  int GetPolarity() const;
};

#endif
