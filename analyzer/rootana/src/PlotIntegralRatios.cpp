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
    fShiftFull=opts->GetDouble("shift_full",0);
    fShiftDifference=opts->GetDouble("shift_diff",0);
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
            //tmp.ratio->SetDirectory(GetDirectory("ratios"));

            tmp.ratio_zoomed=new TH1F(
                    modules::parser::ToCppValid("h"+tmp.src.str()+"_ratio_zoomed").c_str(),
                    ("Zoomed Ratio of integrals for "+tmp.src.str()).c_str(),
                    2000,0.1,0.25);
            tmp.ratio_zoomed->SetXTitle("Ratio of total to tail integral");
            //tmp.ratio_zoomed->SetDirectory(GetDirectory("ratios"));

            tmp.full_integral=new TH1F(
                    modules::parser::ToCppValid("h"+tmp.src.str()+"_integral").c_str(),
                    ("Integrals for "+tmp.src.str()).c_str(),
                    1000,0,-1);
            tmp.full_integral->SetXTitle("Total integral");
            //tmp.full_integral->SetDirectory(GetDirectory("ratios"));

//            tmp.assymetry=new TH1F(
//                    modules::parser::ToCppValid("h"+tmp.src.str()+"_assymetry").c_str(),
//                    ("Assymetry (first - second)/(first+second) for "+tmp.src.str()).c_str(),
//                    200,0,-1);
//            tmp.assymetry->SetXTitle("Assymetry of total to tail integral");
////            tmp.assymetry->SetDirectory(GetDirectory("assymetry"));
//
//            tmp.assymetry2d=new TH2F(
//                    modules::parser::ToCppValid("h"+tmp.src.str()+"_assymetry2d").c_str(),
//                    ("Assymetry (first - second) vs (first+second) for "+tmp.src.str()).c_str(),
//                    200,0,-1,200,0,-1);
//            tmp.assymetry2d->SetXTitle("Total integral");
//            tmp.assymetry2d->SetYTitle("Difference between first and tail");
////            tmp.assymetry2d->SetDirectory(GetDirectory("assymetry"));

            tmp.full_v_tail=new TH2F(
                    modules::parser::ToCppValid("h"+tmp.src.str()+"_2d").c_str(),
                    ("Integrals for "+tmp.src.str()).c_str(),
                    400,0,-1,400,0,-1);
            tmp.full_v_tail->SetXTitle("tail integral");
            tmp.full_v_tail->SetYTitle("full integral");
            //tmp.full_v_tail->SetDirectory(GetDirectory("miscs"));

            tmp.full_v_ratio=new TH2F(
                    modules::parser::ToCppValid("h"+tmp.src.str()+"_integral_v_ratio").c_str(),
                    ("Integral vs ratio of tail to integral for "+tmp.src.str()).c_str(),
                    400,0,-3,400,0,-1);
            tmp.full_v_ratio->SetXTitle("ratio of tail to full integrals");
            tmp.full_v_ratio->SetYTitle("full integral");
            //tmp.full_v_ratio->SetDirectory(GetDirectory("miscs"));

            tmp.diff_v_ratio=new TH2F(
                    modules::parser::ToCppValid("h"+tmp.src.str()+"_diff_v_ratio").c_str(),
                    ("Difference vs ratio of tail to integral for "+tmp.src.str()).c_str(),
                    400,0,-1,400,0,-1);
            tmp.diff_v_ratio->SetXTitle("ratio of tail to full integrals");
            tmp.diff_v_ratio->SetYTitle("differente of tail to full integrals");
            //tmp.diff_v_ratio->SetDirectory(GetDirectory("miscs"));

            //tmp.mean_vs_min=new TH2F(
            //        modules::parser::ToCppValid("h"+tmp.src.str()+"_mean_vs_min").c_str(),
            //        ("mean vs min values for "+tmp.src.str()).c_str(),
            //        200,0,-1,200,0,-1);
            //tmp.mean_vs_min->SetXTitle("minimum value");
            //tmp.mean_vs_min->SetYTitle("mean value");
            //tmp.mean_vs_min->SetDirectory(GetDirectory("min_max_mean"));

            //tmp.mean_vs_max=new TH2F(
            //        modules::parser::ToCppValid("h"+tmp.src.str()+"_mean_vs_max").c_str(),
            //        ("mean vs max values for "+tmp.src.str()).c_str(),
            //        200,0,-1,200,0,-1);
            //tmp.mean_vs_max->SetXTitle("maximum value");
            //tmp.mean_vs_max->SetYTitle("mean value");
            //tmp.mean_vs_max->SetDirectory(GetDirectory("min_max_mean"));

            //tmp.mean=new TH1F(
            //        modules::parser::ToCppValid("h"+tmp.src.str()+"_mean").c_str(),
            //        ("mean value for "+tmp.src.str()).c_str(),
            //        400,0,-1);
            //tmp.mean->SetXTitle("mean value");
            //tmp.mean->SetDirectory(GetDirectory("min_max_mean"));

            //tmp.max=new TH1F(
            //        modules::parser::ToCppValid("h"+tmp.src.str()+"_max").c_str(),
            //        ("max value for "+tmp.src.str()).c_str(),
            //        400,0,-1);
            //tmp.max->SetXTitle("max value");
            //tmp.max->SetDirectory(GetDirectory("min_max_mean"));

            //tmp.min=new TH1F(
            //        modules::parser::ToCppValid("h"+tmp.src.str()+"_min").c_str(),
            //        ("min value for "+tmp.src.str()).c_str(),
            //        400,0,-1);
            //tmp.min->SetXTitle("min value");
            //tmp.min->SetDirectory(GetDirectory("min_max_mean"));

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
            double diff=full-2*tail-fShiftDifference;
            i_source->ratio->Fill(ratio);
            i_source->ratio_zoomed->Fill(ratio);
            i_source->full_integral->Fill(full);
            i_source->full_v_tail->Fill(tail,full);
            i_source->full_v_ratio->Fill(ratio,full);
            i_source->diff_v_ratio->Fill(ratio,diff);
            //i_source->assymetry->Fill(diff/(full-fShiftFull));
            //i_source->assymetry2d->Fill(full,(full-2*tail));

            //double mean=pulse->GetMean();
            //double min=pulse->GetMin();
            //double max=pulse->GetMax();
            //i_source->mean->Fill(mean);
            //i_source->max->Fill(max);
            //i_source->min->Fill(min);
            //i_source->mean_vs_min->Fill(min,mean);
            //i_source->mean_vs_max->Fill(max,mean);
        }

    }
  return 0;
}

int PlotIntegralRatios::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
  return 0;
}

ALCAP_REGISTER_MODULE(PlotIntegralRatios,source,shift_full, shift_diff);
