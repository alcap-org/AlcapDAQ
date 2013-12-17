#include "TAnalysedPulse.h"

#include <cmath>
#include <sstream>
#include <cstdlib>

#include "TF1.h"

using std::vector;
using std::string;

TAnalysedPulse::TAnalysedPulse()
{
  Reset();
}

TAnalysedPulse::TAnalysedPulse(double amplitude, double time, double integral, double energy, std::string det_name)
{
  Reset();
  fAmplitude = amplitude;
  fTime = time;
  fIntegral = integral;
  fEnergy = energy;
  fDetName = det_name;
}

void TAnalysedPulse::Reset(Option_t* o)
{
  fAmplitude = 0;
  fTime = 0;
  fIntegral = 0;
  fEnergy = 0.;
  fDetName = "";
}
