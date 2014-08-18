#include "TAPGeneratorFactory.h"
#include "UncalibratedAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include <iostream>
using std::cout;
using std::endl;

UncalibratedAPGenerator::UncalibratedAPGenerator(TAPGeneratorOptions* opts):
	TVAnalysedPulseGenerator("Uncalibrated",opts){
	// Do things to set up the generator here. 
}

int UncalibratedAPGenerator::ProcessPulses( 
		const PulseIslandList& pulseList,
		AnalysedPulseList& analysedList){
    // Do something here that takes the TPIs in the PulseIslandList and
    // fills the list of TAPS

  // The variables we are going to fill
  double time;
  
  // Get the relevant TSetupData/SetupNavigator variables for the algorithms
  std::string bankname = pulseList[0]->GetBankName();

  fNoShiftTime.clock_tick_in_ns = TSetupData::Instance()->GetClockTick(bankname);


  // Loop over all the TPIs given to us
  TAnalysedPulse* tap;
  for (PulseIslandList::const_iterator tpi=pulseList.begin();
       tpi!=pulseList.end(); tpi++){

    // Analyse each TPI
    time=fNoShiftTime(*tpi);

    // Now that we've found the information we were looking for make a TAP to
    // hold it.  This method makes a TAP and sets the parent TPI info.  It needs
    // the index of the parent TPI in the container as an argument
    tap = MakeNewTAP(tpi-pulseList.begin());
    tap->SetTime(time);
    // Finally add the new TAP to the output list
    analysedList.push_back(tap);
   }
	
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
ALCAP_TAP_GENERATOR(Uncalibrated);
