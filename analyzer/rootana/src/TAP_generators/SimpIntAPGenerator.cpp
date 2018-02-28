#include "TAPGeneratorFactory.h"
#include "SimpIntAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include "SetupNavigator.h"
#include <iostream>
using std::cout;
using std::endl;

SimpIntAPGenerator::SimpIntAPGenerator(TAPGeneratorOptions* opts):
  TVAnalysedPulseGenerator("SimpInt", opts),
  fSimpleIntegral(SetupNavigator::Instance()->GetPedestal(GetChannel()), 
		  TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(GetChannel().str()))) {

}

int SimpIntAPGenerator::ProcessPulses( 
	  const PulseIslandList& pulseList, AnalysedPulseList& analysedList)
{
  TAnalysedPulse* outPulse;

  for(PulseIslandList::const_iterator pIt = pulseList.begin(); pIt != pulseList.end(); pIt++)
    {    
      // We will need new calibration parameters from integral method
      //      energy = fECalibSlope* integral + fECalibOffset;
      double integral = fSimpleIntegral(*pIt);

      outPulse=MakeNewTAP(pIt-pulseList.begin(), 0);
      outPulse->SetIntegral(integral);
      //      outPulse->SetEnergy(energy);
      // Add the pulse to the TAP list
      analysedList.push_back(outPulse);
    }


  // returning 0 tells the caller that we were successful, return non-zero if not
  return 0;
}

// Similar to the modules, this macro registers the generator with
// MakeAnalysedPulses. The first argument is compulsory and gives the name of
// this generator. All subsequent arguments will be used as names for arguments
// given directly within the modules file.  See the github wiki for more.
//
// NOTE: for TAP generators OMIT the APGenerator part of the class' name
ALCAP_TAP_GENERATOR(SimpInt);
