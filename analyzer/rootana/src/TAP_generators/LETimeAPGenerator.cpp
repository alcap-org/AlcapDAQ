#include "TAPGeneratorFactory.h"
#include "LETimeAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include <iostream>
using std::cout;
using std::endl;

LETimeAPGenerator::LETimeAPGenerator(TAPGeneratorOptions* opts):
	TVAnalysedPulseGenerator("LETime",opts){
	// Do things to set up the generator here. 
}

int LETimeAPGenerator::ProcessPulses( 
		const PulseIslandList& pulseList,
		AnalysedPulseList& analysedList){
	// Do something here that takes the TPIs in the PulseIslandList and
	// fills the list of TAPS
	
	// Generators have a Debug method similar to modules
	if(Debug()){
    // They also have a unique ID, retrievable by GetSource and
		// a GetChannel method to get the ID of just the channel
    cout<<"Now running source: "<<GetSource().str()<<" which looks for TAPs on "
        "channel: "<<GetChannel().str()<<'\n';
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
ALCAP_TAP_GENERATOR(LETime);
