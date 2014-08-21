#include "TAPGeneratorFactory.h"
#include "CFTimeMBAmpAPGenerator.h"
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

CFTimeMBAmpAPGenerator::CFTimeMBAmpAPGenerator(TAPGeneratorOptions* opts) :
	TVAnalysedPulseGenerator("CFTimeMBAmpAPGenerator",opts) {

  // Get the parameters we want from the modules file
  // This is required in the modules file by giving it an invalid default value.
  fConstantFractionParam = opts->GetDouble("constant_fraction", -0.10);
  if (fConstantFractionParam <= 0.00 || fConstantFractionParam >=1.00)
    throw OptionsError();

  fDontShiftTime = opts->GetBool("no_time_shift", false);
}

int CFTimeMBAmpAPGenerator::ProcessPulses(const PulseIslandList& pulseList,
				     AnalysedPulseList& analysedList) {


  // Get the channel and bankname
  IDs::channel channel = GetChannel();
  std::string bankname = TSetupData::Instance()->GetBankName(channel.str());

  // Get the variables we want from TSetupData/SetupNavigator
  double pedestal = SetupNavigator::Instance()->GetPedestal(channel);
  int trigger_polarity = TSetupData::Instance()->GetTriggerPolarity(bankname);
  int max_adc_value = std::pow(2, TSetupData::Instance()->GetNBits(bankname)) - 1;
  double clock_tick_in_ns = TSetupData::Instance()->GetClockTick(bankname);
  double time_shift;
  if (fDontShiftTime)
    time_shift = 0;
  else
    time_shift = SetupNavigator::Instance()->GetCoarseTimeOffset(channel.str());

  // Set-up the algorithms
  fConstantFractionTime = new Algorithm::ConstantFractionTime(pedestal, trigger_polarity, max_adc_value, clock_tick_in_ns, time_shift, fConstantFractionParam);
  fMaxBinAmplitude = new Algorithm::MaxBinAmplitude(pedestal, trigger_polarity);

  for (unsigned int iTPI = 0; iTPI < pulseList.size(); ++iTPI) {
    TPulseIsland* tpi = pulseList.at(iTPI);

    double time = fConstantFractionTime->Process(tpi);
    double amplitude  = fMaxBinAmplitude->Process(tpi);

    TAnalysedPulse* tap = MakeNewTAP(iTPI);
    tap->SetTime(time);
    tap->SetAmplitude(amplitude);
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
ALCAP_TAP_GENERATOR(CFTimeMBAmp,constant_fraction,no_time_shift);
