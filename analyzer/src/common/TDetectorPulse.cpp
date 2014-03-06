#include "TDetectorPulse.h"

#include <cmath>
#include <sstream>
#include <cstdlib>

#include "TF1.h"

using std::vector;
using std::string;

TDetectorPulse::TDetectorPulse()
{
  Reset();
}

TDetectorPulse::TDetectorPulse(TAnalysedPulse* fast_pulse, TAnalysedPulse* slow_pulse, std::string det_name)
{
  Reset();
  fFastPulse = fast_pulse;
  fSlowPulse = slow_pulse;
  fDetName = det_name;
}

void TDetectorPulse::Reset(Option_t* o)
{
  fFastPulse = NULL;
  fSlowPulse = NULL;
  fDetName = "";
}
