#include "TAPGeneratorFactory.h"
#include "LETimeAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include <iostream>
using std::cout;
using std::endl;

LETimeAPGenerator::LETimeAPGenerator(TAPGeneratorOptions* opts):
	TVAnalysedPulseGenerator("LETime",opts){
  //fLeadingEdge = opts->GetDouble("lead_edge", 0.2);
  fLeadingEdge = 0.2;
}

int LETimeAPGenerator::ProcessPulses(const PulseIslandList& pulseList,
				     AnalysedPulseList& analysedList){
  
  return 0;
}

// Similar to the modules, this macro registers the generator with
// MakeAnalysedPulses. The first argument is compulsory and gives the name of
// this generator. All subsequent arguments will be used as names for arguments
// given directly within the modules file.  See the github wiki for more.
//
// NOTE: for TAP generators OMIT the APGenerator part of the class' name
ALCAP_TAP_GENERATOR(LETime);
