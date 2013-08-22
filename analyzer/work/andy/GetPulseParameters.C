// GetPulseParameters.C
// -- Some useful functions for getting certain pulse parameters

#include <cmath>

#include "TH1.h"

// GetPulseHeight()
// - Returns the peak height of the histogram
// - NB assuming a negative pulse
double GetPulseHeight(TH1* pulse_hist) {
  
  double peak_height = std::fabs(pulse_hist->GetMinimum());
  return peak_height;
}


// GetPulseIntegral()
// - Return the integral of the pulse
// - NB assumes the whole x-axis is part of the pulse (not a problem if the pulse has been pedestal subtracted)
double GetPulseIntegral(TH1* pulse_hist) {

  double integral = std::fabs(pulse_hist->Integral(0,100));
  return integral;
}
