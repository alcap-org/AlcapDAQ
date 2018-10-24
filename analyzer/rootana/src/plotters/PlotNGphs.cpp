//quick method to discriminate and plot pulse height spectra for NDet and NDet2
//I should use methods similar to plotTAP_amplitude while incorporating timing
//cut using TDiff method (use MuSc for timing) and integral ratio method.

#include "PlotNGphs.h"

#include "definitions.h"
#include "IdChannel.h"
#include "ModulesOptions.h"
#include "RegisterModule.inc"
#include "SetupNavigator.h"
#include "TAnalysedPulse.h"
#include "TIntegralRatioAnalysedPulse.h"

#include "TH1F.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

using std::cout;
using std::endl;
using std::pair;
using std::string;
using std::vector;


extern SourceAnalPulseMap gAnalysedPulseMap;

PlotNGphs::PlotNGphs(modules::options* opts) :
  BaseModule("PlotNGphs", opts),
  ftCutMin(opts->GetDouble("tdiff_min", "x>=-10000")),
  ftCutMax(opts->GetDouble("tdiff_max", "x<=10000")),
  ftPileUp(opts->GetDouble("tpileup", 10e3)) {
  //options for timing
  //any ideas for options leading to ratio cut or energy calibration?

}

PlotNGphs::~PlotNGphs(){}

int PlotNGphs::BeforeFirstEntry(TGlobalData *gData, const TSetupData *gSetup){
  SourcePlots_t tmp;
  for(SourceAnalPulseMap::const_iterator i_source = gAnalysedPulseMap.begin(); i_source != gAnalysedPulseMap.end(); ++i_source){
    if(i_source->first.matches(fSource)){
      tmp.src=i_source->first;

      tmp.Neut = new TH1F(modules::parser::ToCppValid("h"+tmp.src.str()+"_NeutESpec").c_str(), ("Neutron Pulse Height Spectrum for "+tmp.src.str()).c_str(), 2500, 0, 2500);
      tmp.Neut->SetXTitle("Pulse Height (Pedestal Subtracted)");

      tmp.NeutE = new TH1F(modules::parser::ToCppValid("h"+tmp.src.str()+"_NeutSpec").c_str(), ("Neutron Energy Spectrum for "+tmp.src.str()).c_str(), 2500, 0, 2500);
      tmp.NeutE->SetXTitle("Energy (MeV)");


      tmp.Gam = new TH1F(modules::parser::ToCppValid("h"+tmp.src.str()+"_GamSpec").c_str(), ("Gamma Pulse Height Spectrum for "+tmp.src.str()).c_str(), 2500, 0, 2500);
      tmp.Gam->SetXTitle("Pulse Height (Pedestal Subtracted)");

      tmp.GamE = new TH1F(modules::parser::ToCppValid("h"+tmp.src.str()+"_GamESpec").c_str(), ("Gamma Energy Spectrum for "+tmp.src.str()).c_str(), 2500, 0, 2500);
      tmp.GamE->SetXTitle("Energy (MeV)");


      fSourcesToPlot.push_back(tmp);
    }
  }

  if(fSourcesToPlot.empty()){
    cout<<"PlotNGphs::BeforeFirstEntry: Error: No sources in gAnalysedPulseMap match "<< fSource << endl;
    return 1;
  }

  return 0;

}

int PlotNGphs::ProcessEntry(TGlobalData *gData, const TSetupData* gSetup){

  //Loop over each TAP list
  TIntegralRatioAnalysedPulse* pulse=0;
  for(SourceList_t::const_iterator i_source=fSourcesToPlot.begin(); i_source != fSourcesToPlot.end(); ++i_source){

    AnalysedPulseList *musc = &gAnalysedPulseMap.at(IDs::source("muSc"));
    AnalysedPulseList::iterator i_Musc;

    const AnalysedPulseList& pulseList=gAnalysedPulseMap.at(i_source->src);
    for(AnalysedPulseList::const_iterator i_pulse=pulseList.begin();
        i_pulse != pulseList.end(); ++i_pulse){

      if(i_pulse == pulseList.begin()) {
        pulse = dynamic_cast<TIntegralRatioAnalysedPulse*>(*i_pulse);
        if(!pulse) {
          cout << "PlotNGphs:ProcessEntry: Error: Pulses in " << i_source->src
               << " aren't of type TIntegralRatioAnalysedPulse" << endl;
          return 2;
        }
      } else {
        pulse = static_cast<TIntegralRatioAnalysedPulse*>(*i_pulse);
      }
      double ratio = pulse->GetIntegralRatio();
      double amp   = pulse->GetAmplitude();
      double time  = pulse->GetTime(); // need to check this and determine TDiff


      int detector=0;
      //need to determine which detector
      if(i_source->src.Channel().str() == "NDet")
        detector=1;
      if(i_source->src.Channel().str() == "NDet2")
        detector=2;
      if(detector==0) {
        cout << "PlotNGphs::ProcessEntry Error: " << i_source->src
             << " is not a neutron detector"<< endl;
        return 3;
      }


      //for now exit if no MuSc data.  need to setup alternative process later
      if(!gAnalysedPulseMap.count(IDs::source("muSc"))) {
        cout << "PlotNGphs::ProcessEntry Error : No MuSc data analyzed" << endl;
        return 4;
      }


      double tdiffTmp = time - (*i_Musc)->GetTime(), tdiff = 0;
      for(; (tdiffTmp > 0) && (i_Musc+1 < musc->end()); ++i_Musc) {
        double mtime = (*i_Musc)->GetTime();

	      //check for pileup
        if((*(i_Musc+1))->GetTime() - mtime < ftPileUp){
         ++i_Musc;
         continue;
        }

        tdiffTmp = time - (*(i_Musc+1))->GetTime();
      }
      tdiff = time - (*i_Musc)->GetTime();

      //check that the next muon isn't closer in time
      if(std::abs(tdiffTmp) < tdiff)
        tdiff = std::abs(tdiffTmp);

      i_Musc--;


      //for each detector I need time cut and ratio cut values
      //I should include some means of selecting time cut in options

      if((tdiff < ftCutMin) || (tdiff > ftCutMax))
        continue;

      double energy = 0;

      if(detector == 1){
        energy = amp * 15 - 5; //double check calibration
	      double rCut1 = 0.15; //edit later with energy relation
        if(ratio > rCut1){
          i_source->Neut->Fill(amp);
          i_source->NeutE->Fill(energy);
        } else if(ratio <= rCut1){
          i_source->Gam->Fill(amp);
          i_source->GamE->Fill(energy);
        }
      }//end detector 1

      if(detector == 2){
	      energy = amp * 15 - 5; //double check calibration
	      double rCut2 = 0.15;//edit later with energy relation
        if(ratio > rCut2){
          i_source->Neut->Fill(amp);
          i_source->NeutE->Fill(energy);
        } else if(ratio <= rCut2){
          i_source->Gam->Fill(amp);
          i_source->GamE->Fill(energy);
        }
      }//end detector 2


    }//end pulse loop

  }//end source loop
  return 0;
}

int PlotNGphs::AfterLastEntry(TGlobalData* gData, const TSetupData *gSetup){
  return 0;
}

ALCAP_REGISTER_MODULE(PlotNGphs, source, tdiff_min, tdiff_max);

