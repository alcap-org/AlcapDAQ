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

TAnalysedPulse::TAnalysedPulse(double amplitude, double time, double integral, double tintegral, double energy, double ratio,  std::string det_name)
{
  Reset();
  fAmplitude = amplitude;
  fTime = time;
  fIntegral = integral;
  fTIntegral = tintegral;
  fEnergy = energy;
  fDetName = det_name;
  fRatio = ratio;
}

void TAnalysedPulse::Reset(Option_t* o)
{
  fAmplitude = 0;
  fTime = 0;
  fIntegral = 0;
  fTIntegral = 0.;
  fRatio = 0;
  fEnergy = 0.;
  fDetName = "";
}
