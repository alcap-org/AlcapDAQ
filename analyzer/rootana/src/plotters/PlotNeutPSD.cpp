#include "PlotNeutPSD.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "TIntegralRatioAnalysedPulse.h"

#include <TH1F.h>
#include <TH2F.h>

#include "debug_tools.h"

#include <iostream>
using std::cout;
using std::endl;

extern SourceAnalPulseMap gAnalysedPulseMap;

PlotNeutPSD::PlotNeutPSD(modules::options* opts):
  BaseModule("PlotNeutPSD", opts),
  fSource(opts->GetString("source", "NDet-*#IntegralRatio#any")){
}

PlotNeutPSD::~PlotNeutPSD(){}

int PlotNeutPSD::BeforeFirstEntry(TGlobalData* gData, const TSetupData *setup){
  SourcePlots_t tmp;
  for(SourceAnalPulseMap::const_iterator i_source = gAnalysedPulseMap.begin(); i_source != gAnalysedPulseMap.end(); ++i_source){
    if(i_source->first.matches(fSource)){
      tmp.src=i_source->first;

      tmp.ratio = new TH1F(modules::parser::ToCppValid("h"+tmp.src.str()+"_ratio").c_str(), ("Ratio of integrals for "+tmp.src.str()).c_str(), 2000, 0.1, 0.25);
      tmp.ratio->SetXTitle("Ratio of total to tail integral");

      tmp.ratio_zoomed = new TH1F(modules::parser::ToCppValid("h"+tmp.src.str()+"_ratio_zoomed").c_str(), ("Zoomed Ratio of integrals for "+tmp.src.str()).c_str(), 2000, 0.1, 0.25);
      tmp.ratio_zoomed->SetXTitle("Ratio of total to tail integral");

      tmp.full_v_tail = new TH2F(modules::parser::ToCppValid("h"+tmp.src.str()+"_2d").c_str(), ("Integrals for "+tmp.src.str()).c_str(), 400, 0, -1, 400, 0, -1);
      tmp.full_v_tail->SetXTitle("tail integral");
      tmp.full_v_tail->SetYTitle("full integral");

      tmp.full_v_ratio = new TH2F(modules::parser::ToCppValid("h"+tmp.src.str()+"_integral_v_ratio").c_str(), ("Integral vs ratio of tail to integral for "+tmp.src.str()).c_str(), 400, 0, -3, 400, 0, -1);
      tmp.full_v_ratio->SetXTitle("ratio of tail to full integrals");
      tmp.full_v_ratio->SetYTitle("full integral");
      

      //initialize 1D energy range histograms


      fSourcesToPlot.push_back(tmp);
    }
  }
  if(fSourcesToPlot.empty()){
    cout<<"PlotNeutPSD:BeforeFirstEntry: Error: No sources in gAnalysedPulseMap match " << fSource << endl;
    return 1;
  }
  return 0;
}

int PlotNeutPSD::ProcessEntry(TGlobalData* gData, const TSetupData *setup){
  TIntegralRatioAnalysedPulse* pulse=0;
  for(SourceList_t::const_iterator i_source=fSourcesToPlot.begin(); i_source!=fSourcesToPlot.end(); ++i_source){
    const AnalysedPulseList& pulseList=gAnalysedPulseMap.at(i_source->src);
    for(AnalysedPulseList::const_iterator i_pulse=pulseList.begin(); i_pulse!=pulseList.end(); ++i_pulse){
      if(i_pulse==pulseList.begin()){
	pulse=dynamic_cast<TIntegralRatioAnalysedPulse*>(*i_pulse);
	if(!pulse) {
	  cout<<"PlotNeutPSD::ProcessEntry: Error: Pulses in "<<i_source->src <<" aren't of type TIntegralRatioAnalysedPulse"<<endl;
	  return 2;
	}
      }
      else{
	pulse = static_cast<TIntegralRatioAnalysedPulse*>(*i_pulse);
      }
      double ratio = pulse->GetIntegralRatio();
      double full = pulse->GetIntegral();
      double tail = pulse->GetIntegralSmall();

      i_source->ratio->Fill(ratio);
      i_source->ratio_zoomed->Fill(ratio);
      i_source->full_v_tail->Fill(tail, full);
      i_source->full_v_ratio->Fill(ratio, full);

  
      //At this point I should add my energy range 1D histograms
    }
  }
  return 0;
}

int PlotNeutPSD::AfterLastEntry(TGlobalData* gData, const TSetupData *setup){
  return 0;
}

ALCAP_REGISTER_MODULE(PlotNeutPSD, source);
