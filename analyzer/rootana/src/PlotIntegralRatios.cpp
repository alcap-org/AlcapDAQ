#include "PlotIntegralRatios.h"
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

PlotIntegralRatios::PlotIntegralRatios(modules::options* opts):
   BaseModule("PlotIntegralRatios",opts),
    fSource(opts->GetString("source","SiR2-*#IntegralRatio#any")){
}

PlotIntegralRatios::~PlotIntegralRatios(){
}

int PlotIntegralRatios::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
    SourcePlots_t tmp;
    for(SourceAnalPulseMap::const_iterator i_source = gAnalysedPulseMap.begin();
            i_source!=gAnalysedPulseMap.end(); ++i_source){
        if(i_source->first.matches(fSource)){
            tmp.src=i_source->first;

            tmp.ratio=new TH1F(
                    modules::parser::ToCppValid("h"+tmp.src.str()+"_ratio").c_str(),
                    ("Ratio of integrals for "+tmp.src.str()).c_str(),
                    1000,0,1);
            tmp.ratio->SetXTitle("Ratio of total to tail integral");

            tmp.ratio_zoomed=new TH1F(
                    modules::parser::ToCppValid("h"+tmp.src.str()+"_ratio_zoomed").c_str(),
                    ("Zoomed Ratio of integrals for "+tmp.src.str()).c_str(),
                    2000,0.1,0.25);
            tmp.ratio_zoomed->SetXTitle("Ratio of total to tail integral");

            tmp.assymetry=new TH1F(
                    modules::parser::ToCppValid("h"+tmp.src.str()+"_assymetry").c_str(),
                    ("Assymetry (first - second)/(first+second) for "+tmp.src.str()).c_str(),
                    200,0,-1);
            tmp.assymetry->SetXTitle("Assymetry of total to tail integral");

            tmp.assymetry2d=new TH2F(
                    modules::parser::ToCppValid("h"+tmp.src.str()+"_assymetry2d").c_str(),
                    ("Assymetry (first - second) vs (first+second) for "+tmp.src.str()).c_str(),
                    200,0,-1,200,0,-1);
            tmp.assymetry2d->SetXTitle("Total integral");
            tmp.assymetry2d->SetYTitle("Difference between first and tail");

            tmp.full_v_tail=new TH2F(
                    modules::parser::ToCppValid("h"+tmp.src.str()+"_2d").c_str(),
                    ("Integrals for "+tmp.src.str()).c_str(),
                    100,0,-1,100,0,-1);
            tmp.full_v_tail->SetXTitle("tail integral");
            tmp.full_v_tail->SetYTitle("full integral");
            fSourcesToPlot.push_back(tmp);
        }
    }
    if(fSourcesToPlot.empty()){
        cout<<"PlotIntegralRatios:BeforeFirstEntry: Error: No sources in gAnalysedPulseMap match "<<fSource<<endl;
        return 1;
    }
  return 0;
}

int PlotIntegralRatios::ProcessEntry(TGlobalData* gData,const TSetupData *setup){
    TIntegralRatioAnalysedPulse* pulse=0;
    for(SourceList_t::const_iterator i_source=fSourcesToPlot.begin();
            i_source!=fSourcesToPlot.end(); ++i_source){
        const AnalysedPulseList& pulseList=gAnalysedPulseMap.at(i_source->src);
        for(AnalysedPulseList::const_iterator i_pulse=pulseList.begin();
                i_pulse!=pulseList.end(); ++i_pulse){
            if(i_pulse==pulseList.begin()){
                pulse=dynamic_cast<TIntegralRatioAnalysedPulse*>(*i_pulse);
                if(!pulse) {
                    cout<<"PlotIntegralRatios::ProcessEntry: Error: Pulses in "<<i_source->src
                        <<" aren't of type TIntegralRatioAnalysedPulse"<<endl;
                    return 2;
                }
            }else{
                pulse=static_cast<TIntegralRatioAnalysedPulse*>(*i_pulse);
            }
            double ratio=pulse->GetIntegralRatio();
            double full=pulse->GetIntegral();
            double tail=pulse->GetIntegralSmall();
            i_source->ratio->Fill(ratio);
            i_source->ratio_zoomed->Fill(ratio);
            i_source->full_v_tail->Fill(full,tail);
            i_source->assymetry->Fill((full-2*tail)/full);
            i_source->assymetry2d->Fill((full-2*tail),full);
        }

    }
  return 0;
}

int PlotIntegralRatios::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
  return 0;
}

ALCAP_REGISTER_MODULE(PlotIntegralRatios,source);
