#include "TAPGeneratorFactory.h"
#include "NeutronAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include "SetupNavigator.h"
#include "EventNavigator.h"
#include "ExportPulse.h"
#include "ModulesParser.h"
#include "debug_tools.h"
#include "TIntegralRatioAnalysedPulse.h"

#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <sstream>

using std::cout;
using std::endl;

NeutronAPGenerator::NeutronAPGenerator(TAPGeneratorOptions* opts):
  TVAnalysedPulseGenerator("Neutron", opts),
  fStartTailAsFraction(opts->GetBool("Start_tail_as_fraction", false)),
  fStartIntegral(opts->GetInt("start_int", "x>=0")),
  fStopIntegral(opts->GetInt("stop_int", "x>=0")),
  fStartTail(opts->GetDouble("start_tail", fStartTailAsFraction? "x<1 && x>0": Form("x>%g", fStartIntegral))),
  fPedestal(SetupNavigator::Instance()->GetPedestal(GetChannel())),
  fPolarity(TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(GetChannel().str()))),
  fConstantFractionTime(SetupNavigator::Instance()->GetPedestal(GetChannel()), 
			TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(GetChannel().str())),
			TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(GetChannel().str())),
			opts->GetBool("no_time_shift", false) ? 0. : SetupNavigator::Instance()->GetCoarseTimeOffset(GetSource()),
			opts->GetDouble("constant_fraction")), 

  fMaxBinAmplitude(fPedestal, fPolarity),
  fIntegralRatioAlgo(fStartIntegral, fStartTail, fStopIntegral, fPolarity, fPedestal) {
}

NeutronAPGenerator::~NeutronAPGenerator(){
  //delete all sub-pulses
  for(PulseIslandList::iterator i_tpi=fSubPulses.begin(); i_tpi!=fSubPulses.end(); ++i_tpi){
    delete *i_tpi;
  }
}

int NeutronAPGenerator::ProcessPulses(
         const PulseIslandList& pulseList,
	 AnalysedPulseList& analysedList){

  //Loop over TPIs
  for(PulseIslandList::const_iterator i_tpi = pulseList.begin();
      i_tpi!=pulseList.end(); ++i_tpi){
    int tpi_ID = i_tpi - pulseList.begin();

    if((*i_tpi)->GetPulseLength() < 14) return 1;

    int start_tail;

    if(fStartTailAsFraction){
      start_tail=fStartTail*(*i_tpi)->GetPulseLength();
      fIntegralRatioAlgo.SetTailStart(start_tail);
    }

  

    double amplitude = fMaxBinAmplitude(*i_tpi),
      time = fConstantFractionTime(*i_tpi);
    int tpeak = fMaxBinAmplitude.GetTime();

    fIntegralRatioAlgo.SetHeadStart(25);
    fIntegralRatioAlgo.SetTailStop(tpeak + 20);

    if(!fStartTailAsFraction)
      fIntegralRatioAlgo.SetTailStart(tpeak + 5);

    //construct TAP with values
    TIntegralRatioAnalysedPulse* tap = MakeNewTAP<TIntegralRatioAnalysedPulse>(tpi_ID);
    tap->SetAmplitude(amplitude);
    tap->SetTime(time);
    tap->SetIntegral(fIntegralRatioAlgo.GetTotal());
    tap->SetIntegralSmall(fIntegralRatioAlgo.GetTail());
    tap->SetIntegralRatio(fIntegralRatioAlgo.GetRatio());

    //add TAP to output list
    analysedList.push_back(tap);

  }
  return 0;
}




void NeutronAPGenerator::DrawPulse(int original, int pulse_timestamp, int n_pulse_samples){
  for(std::vector<TPulseIsland*>::const_iterator subPulseIter = fSubPulses.begin(); subPulseIter != fSubPulses.end(); ++subPulseIter) {
    std::stringstream histname;
    histname << "hSubPulse" << GetChannel() << "_Event"
	     <<EventNavigator::Instance().EntryNo() << "_Pulse" << original
	     << "_SubPulse" << subPulseIter - fSubPulses.begin();

    const std::vector<int>& sub_pulse_samples = (*subPulseIter)->GetSamples();

    TH1D* hPulse = new TH1D(modules::parser::ToCppValid(histname.str()).c_str(), histname.str().c_str(), n_pulse_samples, 0, n_pulse_samples);
    hPulse->SetLineColor(kMagenta);

    int delay = (*subPulseIter)->GetTimeStamp() - pulse_timestamp;
    for(std::vector<int>::const_iterator subPulseSampleIter = sub_pulse_samples.begin(); subPulseSampleIter != sub_pulse_samples.end(); ++subPulseSampleIter) {
      hPulse->Fill( (subPulseSampleIter - sub_pulse_samples.begin()) + delay, (*subPulseSampleIter) ) ;
    }
  }
}

const char* NeutronAPGenerator::TapType(){
  return TIntegralRatioAnalysedPulse::Class()->GetName();
}


ALCAP_TAP_GENERATOR(Neutron, constant_fraction, no_time_shift, start_int, start_tail, stop_int, start_tail_as_fraction, use_pcf);
