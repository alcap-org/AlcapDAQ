#include "TAPGeneratorFactory.h"
#include "FirstCompleteAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include "SetupNavigator.h"
#include "EventNavigator.h"
#include "ExportPulse.h"
#include "ModulesParser.h"
#include "debug_tools.h"

#include <iostream>
#include <cmath>
#include <sstream>

FirstCompleteAPGenerator::FirstCompleteAPGenerator(TAPGeneratorOptions* opts):
  TVAnalysedPulseGenerator("FirstComplete",opts),
  fMaxBinAmplitude(SetupNavigator::Instance()->GetPedestal(GetChannel()), 
		   TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(GetChannel().str()))),
  fConstantFractionTime(SetupNavigator::Instance()->GetPedestal(GetChannel()), 
			TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(GetChannel().str())),
			TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(GetChannel().str())),
			opts->GetBool("no_time_shift", false) ? 0. : SetupNavigator::Instance()->GetCoarseTimeOffset(GetSource()),
			opts->GetDouble("constant_fraction")), 
  fSimpleIntegral(SetupNavigator::Instance()->GetPedestal(GetChannel()), 
		  TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(GetChannel().str()))),
  fPulseCandidateFinder(new PulseCandidateFinder(GetChannel().str(), opts)) {
}

FirstCompleteAPGenerator::~FirstCompleteAPGenerator(){
    // delete all sub-pulses
    for(PulseIslandList::iterator i_tpi=fSubPulses.begin(); i_tpi!=fSubPulses.end(); ++i_tpi){ 
        delete *i_tpi;
    }
}

int FirstCompleteAPGenerator::ProcessPulses( 
        const PulseIslandList& pulseList,
        AnalysedPulseList& analysedList){

    // The variables that this generator will be filling
    double amplitude, time, integral;

    TAnalysedPulse* tap;
    // Loop over all the TPIs given to us
    for (PulseIslandList::const_iterator original_tpi=pulseList.begin();
            original_tpi!=pulseList.end(); original_tpi++){

      // Look for more than one pulse on the TPI
        fPulseCandidateFinder->FindPulseCandidates(*original_tpi);
        int n_pulse_candidates = fPulseCandidateFinder->GetNPulseCandidates();
        fPulseCandidateFinder->GetPulseCandidates(fSubPulses);


        for(PulseIslandList::const_iterator i_tpi=fSubPulses.begin(); i_tpi!=fSubPulses.end(); ++i_tpi){
            // Skip small pulses.  This must be at least 1 to skip empty pulses
	  //            PulseIslandList::const_iterator i_tpi=original_tpi;
            if((*i_tpi)->GetPulseLength() < 14) continue;

            // Analyse each TPI
            amplitude=fMaxBinAmplitude(*i_tpi);
            time=fConstantFractionTime(*i_tpi);
            integral=fSimpleIntegral(*i_tpi);

            // Now that we've found the information we were looking for make a TAP to
            // hold it.  This method makes a TAP and sets the parent TPI info.  It needs
            // the index of the parent TPI in the container as an argument
            tap = MakeNewTAP(original_tpi-pulseList.begin());
            tap->SetAmplitude(amplitude);
            tap->SetTime(time);
            tap->SetIntegral(integral);

            // Finally add the new TAP to the output list
            analysedList.push_back(tap);

        } // loop over all sub-pulses


    } // loop over all original TPIs

    return 0;
}

void FirstCompleteAPGenerator::DrawPulse(int original, int pulse_timestamp, int n_pulse_samples){

  /*    if( ExportPulse::Instance())
        ExportPulse::Instance()->AddToExportList(GetChannel().str(), original);
    else return;
  */
    for (std::vector<TPulseIsland*>::const_iterator subPulseIter = fSubPulses.begin();
            subPulseIter != fSubPulses.end(); ++subPulseIter) {
        std::stringstream histname;
        histname << "hSubPulse_" << GetChannel() << "_Event" 
            << EventNavigator::Instance().EntryNo() <<"_Pulse" << original
            << "_SubPulse" << subPulseIter - fSubPulses.begin();

        const std::vector<int>& sub_pulse_samples = (*subPulseIter)->GetSamples();

        TH1D* hPulse = new TH1D(modules::parser::ToCppValid(histname.str()).c_str(),
                histname.str().c_str(), n_pulse_samples,0,n_pulse_samples);
        hPulse->SetLineColor(kMagenta);

        int delay = (*subPulseIter)->GetTimeStamp() - pulse_timestamp;
        for (std::vector<int>::const_iterator subPulseSampleIter = sub_pulse_samples.begin();
                subPulseSampleIter != sub_pulse_samples.end(); ++subPulseSampleIter) {
            hPulse->Fill( (subPulseSampleIter - sub_pulse_samples.begin()) + delay, (*subPulseSampleIter) );
        }
    }
}

ALCAP_TAP_GENERATOR(FirstComplete,constant_fraction,no_time_shift);
