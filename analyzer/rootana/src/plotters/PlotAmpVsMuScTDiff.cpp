#include "PlotAmpVsMuScTDiff.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "SetupNavigator.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "TAnalysedPulse.h"
#include <cmath>
#include <stdexcept>

#include <TH2F.h>
#include <debug_tools.h>

#include <iostream>
using std::cout;
using std::endl;

extern SourceAnalPulseMap gAnalysedPulseMap;

PlotAmpVsMuScTDiff::PlotAmpVsMuScTDiff(modules::options* opts):
    BaseModule("PlotAmpVsMuScTDiff",opts){

    }

PlotAmpVsMuScTDiff::~PlotAmpVsMuScTDiff(){
}

int PlotAmpVsMuScTDiff::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
    // Find the first muSc source ID
    IDs::source muSc(IDs::kMuSc,IDs::kAnySlowFast, "*", IDs::kAnyConfig);
    for (SourceAnalPulseMap::const_iterator i_source=gAnalysedPulseMap.begin();
            i_source!=gAnalysedPulseMap.end(); i_source++){
        if(i_source->first.matches(muSc)) fMuScSources.push_back(i_source->first);
        else fNotMuScSources.push_back(i_source->first);
    }
    // Check we have at least one muSc to compare to
    if(fMuScSources.empty()){
        cout<<"No muSc sources were found so we cannot make a TDiff plot to muSc"<<endl;
        return 1;
    }
    return 0;
}

int PlotAmpVsMuScTDiff::ProcessEntry(TGlobalData* gData,const TSetupData *setup){
    Plots plots;
    AnalysedPulseList* not_muSc_pulses, *muSc_pulses;

    // for each source that's not a muSc source
    for(SourceVector::const_iterator i_not_muSc=fNotMuScSources.begin();
            i_not_muSc!=fNotMuScSources.end(); i_not_muSc++){

        // Get all pulses for i_not_muSc 
        not_muSc_pulses=&gAnalysedPulseMap[*i_not_muSc];

        // for each muSc source
        for(SourceVector::const_iterator i_muSc=fMuScSources.begin();
                i_muSc!=fMuScSources.end(); i_muSc++){

            // get or make the histogram
            try{
            plots=GetHistogram(*i_not_muSc,*i_muSc);
            }catch(const std::bad_alloc& e){
                DEBUG_PREFIX<<" Cannot allocate new histograms."<<endl;
                throw e;
            }

            // Get all pulses for i_muSc
            muSc_pulses=&gAnalysedPulseMap[*i_muSc];

            for (AnalysedPulseList::const_iterator not_muSc = not_muSc_pulses->begin();
                    not_muSc != not_muSc_pulses->end(); ++not_muSc) {
                for (AnalysedPulseList::const_iterator muSc = muSc_pulses->begin();
                        muSc != muSc_pulses->end(); ++muSc) {

                    // fill the histo
                    double arrival_time = (*not_muSc)->GetTime();
                    double muSc_time = (*muSc)->GetTime();
                    double t_diff = arrival_time - muSc_time;
                    double not_muSc_amp = (*not_muSc)->GetAmplitude();

                    plots.coarse->Fill(t_diff, not_muSc_amp);
                    plots.fine->Fill(t_diff, not_muSc_amp);

                } // end loop over muSc pulses
            } // end loop over non-muSc pulses
        } // end loop over muSc sources
    } // end loop over non-muSc sources
    return 0;
}

int PlotAmpVsMuScTDiff::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
    return 0;
}

PlotAmpVsMuScTDiff::Plots PlotAmpVsMuScTDiff::GetHistogram(const IDs::source& detector, const IDs::source& muSc){
    std::string keyname=detector.str()+ "-"+muSc.str();
    // check if we already have this plot and return it if so
    if(fPlots.find(keyname)!=fPlots.end()){
        return fPlots[keyname];
    }

    // names and axes labels
    std::string histname = "h" + keyname + "_AmpVsMuScTDiff_Coarse";
    std::string histtitle = "Amplitude vs time difference between " + detector.str() + " and " + muSc.str();
    std::string x_axis_title = "Time Difference (" + detector.str() + " - muSc) [ns]";
    std::string y_axis_title = "Amplitude from " + detector.str() + " [ADC Value]";

    // histogram ranges
    int n_bits = TSetupData::Instance()->GetNBits(SetupNavigator::Instance()->GetBank(detector.Channel()));
    double max_adc_value = std::pow(2, n_bits);

    // struct to hold return plots
    Plots plots;

    // coarse plot
    plots.coarse = new TH2F(histname.c_str(), histtitle.c_str(), 100,-50000,50000, 300,0,max_adc_value);
    plots.coarse->GetXaxis()->SetTitle(x_axis_title.c_str());
    plots.coarse->GetYaxis()->SetTitle(y_axis_title.c_str());

    // fine plot
    histname = "h" + keyname + "_AmpVsMuScTDiff_Fine";
    plots.fine = new TH2F(histname.c_str(), histtitle.c_str(), 5000,-5000,20000, 300,0,max_adc_value);
    plots.fine->GetXaxis()->SetTitle(x_axis_title.c_str());
    plots.fine->GetYaxis()->SetTitle(y_axis_title.c_str());

    // insert these plots into the list of plots
    fPlots[keyname]=plots;
    //DEBUG_VALUE(fPlots.size());

    return plots;

}

ALCAP_REGISTER_MODULE(PlotAmpVsMuScTDiff);
