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
  // This is required in the modules file by giving it an invalid default value.
  fConstantFractionTime.constant_fraction = opts->GetDouble("constant_fraction", -0.10);
  fDontShiftTime = opts->GetBool("no_time_shift", false);
  if (fConstantFractionTime.constant_fraction <= 0.00 || fConstantFractionTime.constant_fraction >=1.00) {
    throw OptionsError();
}

int CFTimeMBAmpAPGenerator::ProcessPulses(const PulseIslandList& pulseList,
				     AnalysedPulseList& analysedList) {


  // Get the variables we want from TSetupData/SetupNavigator
  std::string bankname = pulseList[0]->GetBankName();
  fConstantFractionTime.pedestal = SetupNavigator::Instance()->GetPedestal(TSetupData::Instance()->GetDetectorName(bankname));
  fConstantFractionTime.trigger_polarity = TSetupData::Instance()->GetTriggerPolarity(bankname);
  fConstantFractionTime.max_adc_value = std::pow(2, TSetupData::Instance()->GetNBits(bankname)) - 1;
  fConstantFractionTime.clock_tick_in_ns = TSetupData::Instance()->GetClockTick(bankname);
  if (fDontShiftTime)
    fConstantFractionTime.time_shift = 0;
  else
    fConstantFractionTime.time_shift = TSetupData::Instance()->GetTimeShift();

  fMaxBinAmplitude.pedestal = fConstantFractionTime.pedestal;
  fMaxBinAmplitude.trigger_polarity = fConstantFractionTime.trigger_polarity;


  for (unsigned int iTPI = 0; iTPI < pulseList.size(); ++iTPI) {
    TPulseIsland* tpi = pulseList.at(iTPI);

    double time = fConstantFractionTime(tpi);
    double amplitude  = fMaxBinAmplitude(tpi);

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
ALCAP_TAP_GENERATOR(CFTimeMBAmp,constant_fraction);
