#include "TAPGeneratorFactory.h"
#include "CFTimeAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include "SetupNavigator.h"

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
	TVAnalysedPulseGenerator("CFTimeAPGenerator",opts){
  //fConstantFraction = opts->GetDouble("constfrac", 0.1);
  fConstantFraction = 0.2;
  if (fConstantFraction <= 0. || fConstantFraction >=100.)
    throw OptionsError();
}

int CFTimeAPGenerator::ProcessPulses(const PulseIslandList& pulseList,
				     AnalysedPulseList& analysedList) {

  // Get the variables we want from TSetupData/SetupNavigator
  std::string bankname = pulseList[0]->GetBankName();
  double pedestal = SetupNavigator::Instance()->GetPedestal(TSetupData::Instance()->GetDetectorName(bankname));
  int trigger_polarity = TSetupData::Instance()->GetTriggerPolarity(bankname);
  int max_adc_value = std::pow(2, TSetupData::Instance()->GetNBits(bankname)) - 1;
  double clock_tick_in_ns = TSetupData::Instance()->GetClockTick(bankname);
  double time_shift = TSetupData::Instance()->GetTimeShift(bankname);

  fConstantFractionTime = new Algorithm::ConstantFractionTime(pedestal, trigger_polarity, max_adc_value, clock_tick_in_ns, time_shift, fConstantFraction);

  for (unsigned int iTPI = 0; iTPI < pulseList.size(); ++iTPI) {
    TPulseIsland* tpi = pulseList.at(iTPI);

    double time = fConstantFractionTime->Process(tpi);

    TAnalysedPulse* tap = MakeNewTAP(iTPI);
    tap->SetTime(time);
    analysedList.push_back(tap);
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
