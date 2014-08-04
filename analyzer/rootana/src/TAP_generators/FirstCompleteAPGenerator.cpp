#include "TAPGeneratorFactory.h"
#include "FirstCompleteAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include "SetupNavigator.h"
#include "ExportPulse.h"

#include <iostream>
#include <cmath>
#include <sstream>

using std::cout;
using std::endl;

FirstCompleteAPGenerator::FirstCompleteAPGenerator(TAPGeneratorOptions* opts):
  TVAnalysedPulseGenerator("FirstComplete",opts), fOpts(opts){
	// Do things to set up the generator here. 

}

int FirstCompleteAPGenerator::ProcessPulses( 
		const PulseIslandList& pulseList,
		AnalysedPulseList& analysedList){
    // Do something here that takes the TPIs in the PulseIslandList and
    // fills the list of TAPS

  // Create a PulseCandidateFinder
  fPulseCandidateFinder = new PulseCandidateFinder(GetChannel().str(), fOpts);

  // The variables that this generator will be filling
  double amplitude, time, integral;

  // Get the relevant TSetupData/SetupNavigator variables for the algorithms
  std::string bankname = pulseList[0]->GetBankName();

  fMaxBinAmplitude.pedestal = fConstantFractionTime.pedestal = fSimpleIntegral.pedestal = SetupNavigator::Instance()->GetPedestal(bankname);
  fMaxBinAmplitude.trigger_polarity = fConstantFractionTime.trigger_polarity = fSimpleIntegral.trigger_polarity = TSetupData::Instance()->GetTriggerPolarity(bankname);
  fConstantFractionTime.max_adc_value = std::pow(2, TSetupData::Instance()->GetNBits(bankname)) - 1;
  fConstantFractionTime.clock_tick_in_ns = TSetupData::Instance()->GetClockTick(bankname);
  fConstantFractionTime.time_shift = TSetupData::Instance()->GetTimeShift(bankname);

  TAnalysedPulse* tap;
  // Loop over all the TPIs given to us
  for (PulseIslandList::const_iterator tpi=pulseList.begin();
       tpi!=pulseList.end(); tpi++){

    // Look for more than one pulse on the TPI
    fPulseCandidateFinder->FindPulseCandidates(*tpi);
    int n_pulse_candidates = fPulseCandidateFinder->GetNPulseCandidates();
    std::cout << "FirstCompleteAPGenerator: " << GetChannel().str() << ": n_pulse_candidates = " << n_pulse_candidates  << std::endl;

    const std::vector<int>& pulse_samples = (*tpi)->GetSamples();
    int n_pulse_samples = pulse_samples.size();
    int pulse_timestamp = (*tpi)->GetTimeStamp();

    if (Debug() && n_pulse_candidates > 1 ) {
      ExportPulse::Instance()->AddToExportList(GetChannel().str(), tpi-pulseList.begin());

      const std::vector<TPulseIsland*>& sub_pulses = fPulseCandidateFinder->GetPulseCandidates();
      for (std::vector<TPulseIsland*>::const_iterator subPulseIter = sub_pulses.begin(); subPulseIter != sub_pulses.end(); ++subPulseIter) {
	std::stringstream histname;
	histname << "hSubPulse_ " << GetChannel().str() << "_Pulse" << tpi-pulseList.begin() << "_SubPulse" << subPulseIter - sub_pulses.begin();

	const std::vector<int>& sub_pulse_samples = (*subPulseIter)->GetSamples();
	int n_sub_pulse_samples = sub_pulse_samples.size();

	TH1D* hPulse = new TH1D(histname.str().c_str(), histname.str().c_str(), n_pulse_samples,0,n_pulse_samples);
	hPulse->SetLineColor(kMagenta);

	int delay = (*subPulseIter)->GetTimeStamp() - pulse_timestamp;
	for (std::vector<int>::const_iterator subPulseSampleIter = sub_pulse_samples.begin(); subPulseSampleIter != sub_pulse_samples.end(); ++subPulseSampleIter) {
	  hPulse->Fill( (subPulseSampleIter - sub_pulse_samples.begin()) + delay, (*subPulseSampleIter) );
	}
      }
    }

    // Analyse each TPI
    amplitude=fMaxBinAmplitude(*tpi);
    time=fConstantFractionTime(*tpi);
    integral=fSimpleIntegral(*tpi);

    // Now that we've found the information we were looking for make a TAP to
    // hold it.  This method makes a TAP and sets the parent TPI info.  It needs
    // the index of the parent TPI in the container as an argument
    tap = MakeNewTAP(tpi-pulseList.begin());
    tap->SetAmplitude(amplitude);
    tap->SetTime(time);
    tap->SetIntegral(integral);

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
ALCAP_TAP_GENERATOR(FirstComplete);
