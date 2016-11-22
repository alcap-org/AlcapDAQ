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
			TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(GetChannel().str())),
			opts->GetBool("no_time_shift") ? 0. : SetupNavigator::Instance()->GetCoarseTimeOffset(GetSource()),
			TSetupData::GetDownSampling(GetBank().c_str(), SetupNavigator::Instance()->GetRunNumber()),
      opts->GetDouble("constant_fraction", -0.10)),
  fMaxBinAmplitude(SetupNavigator::Instance()->GetPedestal(GetChannel()),
		   TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(GetChannel().str()))) {
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
