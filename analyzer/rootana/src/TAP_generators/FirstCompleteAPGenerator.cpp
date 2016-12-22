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
			TSetupData::GetDownSampling(GetBank().c_str(), SetupNavigator::Instance()->GetRunNumber()),
      opts->GetDouble("constant_fraction")),
  fSimpleIntegral(SetupNavigator::Instance()->GetPedestal(GetChannel()),
		  TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(GetChannel().str()))),
  fPulseCandidateFinder(NULL) {
  if(opts->GetBool("use_pcf",false)){
     if(opts->Debug()) opts->SetOption("debug","true");
     fPulseCandidateFinder=new PulseCandidateFinder(GetChannel().str(), opts);
  }
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

    // Loop over all the TPIs given to us
    for (PulseIslandList::const_iterator i_tpi=pulseList.begin();
            i_tpi!=pulseList.end(); ++i_tpi){
        int tpi_ID = i_tpi - pulseList.begin();
        if(fPulseCandidateFinder) MakeTAPsWithPCF(tpi_ID,*i_tpi,analysedList);
        else AnalyseOneTpi(tpi_ID, *i_tpi,analysedList);
    }
    return 0;
}

void FirstCompleteAPGenerator::MakeTAPsWithPCF(int tpi_ID, const TPulseIsland* original_tpi, AnalysedPulseList& analysedList){
    // Look for more than one pulse on the TPI
    fPulseCandidateFinder->FindPulseCandidates(original_tpi);
    fPulseCandidateFinder->GetPulseCandidates(fSubPulses);

    // now loop over all sub-pulses
    for(PulseIslandList::const_iterator i_tpi=fSubPulses.begin(); i_tpi!=fSubPulses.end(); ++i_tpi){
        AnalyseOneTpi( tpi_ID, *i_tpi,analysedList);
    }
}

void FirstCompleteAPGenerator::AnalyseOneTpi(int tpi_ID, const TPulseIsland* tpi, AnalysedPulseList& analysedList){
    if(tpi->GetPulseLength() < 14) return;

    // Analyse each TPI
    double amplitude=fMaxBinAmplitude(tpi);
    double time=fConstantFractionTime(tpi);
    double integral=fSimpleIntegral(tpi);

    // Now that we've found the information we were looking for make a TAP to
    // hold it.  This method makes a TAP and sets the parent TPI info.  It needs
    // the index of the parent TPI in the container as an argument
    TAnalysedPulse* tap = MakeNewTAP(tpi_ID);
    tap->SetAmplitude(amplitude);
    tap->SetTime(time);
    tap->SetIntegral(integral);

    // Finally add the new TAP to the output list
    analysedList.push_back(tap);
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

ALCAP_TAP_GENERATOR(FirstComplete,constant_fraction,no_time_shift, use_pcf);
