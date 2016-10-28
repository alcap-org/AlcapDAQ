#include "TAPGeneratorFactory.h"
#include "MovingAverageFilterAPGenerator.h"
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
#include <algorithm>

MovingAverageFilterAPGenerator::MovingAverageFilterAPGenerator(TAPGeneratorOptions* opts):
  TVAnalysedPulseGenerator("MovingAverageFilter",opts),
  fPulseCandidateFinder(NULL) 
{
  fWindowWidth = opts->GetInt("window_width");
  fConstantFraction = opts->GetDouble("constant_fraction");

  if(opts->GetBool("use_pcf",true))
  { 
    if(opts->Debug()) opts->SetOption("debug","true");
    fPulseCandidateFinder=new PulseCandidateFinder(GetChannel().str(), opts);
  }
}

MovingAverageFilterAPGenerator::~MovingAverageFilterAPGenerator(){
  // delete all sub-pulses
  for(PulseIslandList::iterator i_tpi=fSubPulses.begin(); i_tpi!=fSubPulses.end(); ++i_tpi){ 
    delete *i_tpi;
  }
}

int MovingAverageFilterAPGenerator::ProcessPulses( 
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

void MovingAverageFilterAPGenerator::MakeTAPsWithPCF(int tpi_ID, const TPulseIsland* original_tpi, AnalysedPulseList& analysedList){
  // Look for more than one pulse on the TPI
  fPulseCandidateFinder->FindPulseCandidates(original_tpi);
  fPulseCandidateFinder->GetPulseCandidates(fSubPulses);

  // now loop over all sub-pulses
  for(PulseIslandList::const_iterator i_tpi=fSubPulses.begin(); i_tpi!=fSubPulses.end(); ++i_tpi){
    AnalyseOneTpi( tpi_ID, *i_tpi,analysedList);
  }
}

void MovingAverageFilterAPGenerator::AnalyseOneTpi(int tpi_ID, const TPulseIsland* tpi, AnalysedPulseList& analysedList){
  if(tpi->GetPulseLength() < 14) return;
  if (fWindowWidth >= tpi->GetPulseLength() || fWindowWidth < 2) {
    return;
  }

  // Prepare the filtered TPI
  std::vector<int> orig_samples = tpi->GetSamples();
  std::vector<double> mavg_samples;

  int first_mavg_sample = 0;
  for (int isample = 0; isample < fWindowWidth; ++isample) {
    first_mavg_sample += orig_samples.at(isample);
  }
  mavg_samples.push_back(first_mavg_sample);
  for (unsigned int isample = 1;
      isample < orig_samples.size() - fWindowWidth + 1; ++isample) {
    mavg_samples.push_back(mavg_samples.back() 
        + orig_samples.at(isample + fWindowWidth - 1) 
        - orig_samples.at(isample - 1));
  }
  
  for (unsigned int isample = 0; isample < mavg_samples.size(); isample++)
  {
    mavg_samples.at(isample) = mavg_samples.at(isample)/fWindowWidth;
  }

  // can't create a TPI cause it accepts only integer samples
 
  // Analyse each TPI
  // double amplitude=fMaxBinAmplitude(tpi);
  // double time=fConstantFractionTime(tpi);
  // double integral=fSimpleIntegral(tpi);
  double integral = 1.;
  // Analyse the filtered waveform
  double ped = 0.;
  // double integral = 0.;
  // for (unsigned int isample = 0; isample < mavg_samples.size(); ++isample) {
  // mavg_samples.at(isample) = ped - mavg_samples.at(isample);
  // }
  std::vector<double>::const_iterator peak_sample_pos =
    std::min_element(mavg_samples.begin(), mavg_samples.end());
  double amplitude = 
    SetupNavigator::Instance()->GetPedestal(GetChannel()) - *peak_sample_pos;
  int time = peak_sample_pos - mavg_samples.begin() + fWindowWidth - 1;

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

void MovingAverageFilterAPGenerator::DrawPulse(int original, int pulse_timestamp, int n_pulse_samples){

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

ALCAP_TAP_GENERATOR(MovingAverageFilter, window_width, constant_fraction, no_time_shift, use_pcf);
