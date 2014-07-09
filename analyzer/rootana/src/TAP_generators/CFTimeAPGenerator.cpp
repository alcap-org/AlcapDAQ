#include "TAPGeneratorFactory.h"
#include "CFTimeAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"

#include <numeric>
#include <algorithm>
#include <cmath>
#include <exception>


class OptionsError : public std::exception {
  const char* what() const throw () {
    return "Bad MODULES option";
  }
};

CFTimeAPGenerator::CFTimeAPGenerator(TAPGeneratorOptions* opts):
	TVAnalysedPulseGenerator("CFTime",opts){
  //fConstantFraction = opts->GetDouble("constfrac", 0.1);
  fConstantFraction = 0.2;
  if (fConstantFraction <= 0. || fConstantFraction >=100.)
    throw OptionsError();
}

int CFTimeAPGenerator::ProcessPulses(const PulseIslandList& pulseList,
				     AnalysedPulseList& analysedList) {
  static const double th_frac = 0.25;

  TSetupData* setup = TSetupData::Instance();
  unsigned int max = std::pow(2, setup->GetNBits(pulseList[0]->GetBankName())) - 1;

  int pol = pulseList[0]->GetTriggerPolarity();
  for (unsigned int iTPI = 0; iTPI < pulseList.size(); ++iTPI) {
    TPulseIsland* tpi = pulseList.at(iTPI);
    std::vector<int> samps = tpi->GetSamples();
    std::vector<int>::iterator b = samps.begin(), e = samps.end();
    unsigned int ped = std::accumulate(b, b + 5, 0)/5.;
    std::vector<int>::iterator m = pol > 0 ? std::max_element(b, e) : std::min_element(b, e);
    unsigned int amp = *m;
    unsigned int thresh = pol > 0 ? (unsigned int)(th_frac*(double)(max - ped) + ped) : (unsigned int)((1.-th_frac)*ped);
    unsigned int cf = pol > 0 ? (unsigned int)(fConstantFraction*(double)(amp-ped)) + ped : (unsigned int)((double)(ped-amp)*(1.-fConstantFraction) + amp);
    if ((pol > 0 ? amp > thresh : amp < thresh) && (pol > 0 ? amp < max : amp > 0)) {
      std::vector<int>::iterator c = m;
      while ((pol > 0 ? *--m > (int)cf : *--m < (int)cf) && m != b);
      double t;
      if (*(m+1) == *m)
	t = (double)(m-b);
      else
      t = (double)((int)cf - *m)/(double)(*(m+1) - *m) + (double)(m-b);
      TAnalysedPulse* tap = MakeNewTAP(iTPI);
      tap->SetTime(tpi->GetClockTickInNs() * (t + (double)tpi->GetTimeStamp()) - TSetupData::Instance()->GetTimeShift(tpi->GetBankName()));
      analysedList.push_back(tap);
    }
  }

  return 0;
}

// Similar to the modules, this macro registers the generator with
// MakeAnalysedPulses. The first argument is compulsory and gives the name of
// this generator. All subsequent arguments will be used as names for arguments
// given directly within the modules file.  See the github wiki for more.
//
// NOTE: for TAP generators OMIT the APGenerator part of the class' name
ALCAP_TAP_GENERATOR(CFTime);
