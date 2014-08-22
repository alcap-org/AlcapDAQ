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
  TVAnalysedPulseGenerator("CFTimeMBAmpAPGenerator",opts),
  fConstantFractionTime(SetupNavigator::Instance()->GetPedestal(GetChannel()), 
			TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(GetChannel().str())),
			std::pow(2, TSetupData::Instance()->GetNBits(TSetupData::Instance()->GetBankName(GetChannel().str()))) - 1,
			TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(GetChannel().str())),
			opts->GetDouble("time_shift", TSetupData::Instance()->GetTimeShift(TSetupData::Instance()->GetBankName(GetChannel().str()))),
			opts->GetDouble("constant_fraction", -0.10)), 
  fMaxBinAmplitude(SetupNavigator::Instance()->GetPedestal(GetChannel()), 
		   TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(GetChannel().str()))) {

  // Get the parameters we want from the modules file
  // This is required in the modules file by giving it an invalid default value.
  double constant_fraction_param = opts->GetDouble("constant_fraction", -0.10);
  if (constant_fraction_param <= 0.00 || constant_fraction_param >=1.00)
    throw OptionsError();
}

int CFTimeMBAmpAPGenerator::ProcessPulses(const PulseIslandList& pulseList,
				     AnalysedPulseList& analysedList) {

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
ALCAP_TAP_GENERATOR(CFTimeMBAmp,constant_fraction,no_time_shift);
