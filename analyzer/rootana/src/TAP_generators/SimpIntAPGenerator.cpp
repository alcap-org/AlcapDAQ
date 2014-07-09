#include "TAPGeneratorFactory.h"
#include "SimpIntAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include <iostream>
using std::cout;
using std::endl;


int SimpIntAPGenerator::ProcessPulses( 
	  const PulseIslandList& pulseList, AnalysedPulseList& analysedList)
{

  SetBankInfo(pulseList.at(0)->GetBankName());

  double integral, energy;
  TAnalysedPulse* outPulse;

  for(PulseIslandList::const_iterator pIt = pulseList.begin(); pIt != pulseList.end(); pIt++)
    {
      integral = 0;
      energy = 0;
      std::vector<int> samples = (*pIt)->GetSamples();

      double length = samples.size();
      double tempint = 0;
      for(std::vector<int>::iterator sIt = samples.begin(); sIt != samples.end(); sIt++)
	  tempint += *sIt;

      integral = fTriggerPolarity * (tempint - (fPedestal * length));
    

      // We will need new calibration parameters from integral method
      energy = fECalibSlope* integral + fECalibOffset;
  
      outPulse=MakeNewTAP(pIt-pulseList.begin());
      outPulse->SetIntegral(integral);
      outPulse->SetEnergy(energy);
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
