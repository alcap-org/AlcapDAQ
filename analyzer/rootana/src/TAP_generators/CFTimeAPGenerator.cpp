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

  // Get the parameter values we want from the modules file
  //fConstantFraction = opts->GetDouble("constfrac", 0.1);
  fConstantFractionParam = 0.2;
  if (fConstantFractionParam <= 0. || fConstantFractionParam >=100.)
    throw OptionsError();

}

int CFTimeAPGenerator::ProcessPulses(const PulseIslandList& pulseList,
				     AnalysedPulseList& analysedList) {

  // Get the channel and bank name
  IDs::channel channel = GetChannel();
  std::string bankname = TSetupData::Instance()->GetBankName(channel.str());

  // Get the variables we want from TSetupData/SetupNavigator
  double pedestal = SetupNavigator::Instance()->GetPedestal(channel);
  int trigger_polarity = TSetupData::Instance()->GetTriggerPolarity(bankname);
  int max_adc_value = std::pow(2, TSetupData::Instance()->GetNBits(bankname)) - 1;
  double clock_tick_in_ns = TSetupData::Instance()->GetClockTick(bankname);
  double time_shift = TSetupData::Instance()->GetTimeShift(bankname);

  // Set-up the algorithms
  fConstantFractionTime = new Algorithm::ConstantFractionTime(pedestal, trigger_polarity, max_adc_value, clock_tick_in_ns, time_shift, fConstantFractionParam);

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
