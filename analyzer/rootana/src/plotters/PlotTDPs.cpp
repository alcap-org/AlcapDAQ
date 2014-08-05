#include "PlotTDPs.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TDetectorPulse.h"
#include "ModulesOptions.h"
#include "ModulesNavigator.h"
#include "ModulesParser.h"
#include "definitions.h"
#include "IdSource.h"
#include "MakeDetectorPulses.h"
#include "debug_tools.h"

#include "TH2F.h"
#include "TH1F.h"

#include <iostream>
#include <sstream>
#include <string>
#include <utility> // std::rel_ops
using std::cout;
using std::endl;

using namespace std::rel_ops;

extern SourceDetPulseMap gDetectorPulseMap;

PlotTDPs::PlotTDPs(modules::options* opts):
   BaseModule("PlotTDPs",opts),fCutHists(false){
       fCutHists=!opts->GetFlag("no_cut_histos");
       fFastCut=opts->GetDouble("fast_ch_cut",0);
       fSlowCut=opts->GetDouble("slow_ch_cut",0);
}

PlotTDPs::~PlotTDPs(){
}

int PlotTDPs::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
    // Change into this modules directory
    GetDirectory()->cd();

    // Make sure we've got MakeDetectorPulses being used
    MakeDetectorPulses* makeTDPs=modules::navigator::Instance()->GetModule<MakeDetectorPulses>("MakeDetectorPulses");
    if(!makeTDPs){
        cout<<"It's meaningless to use the PlotTDPs module without MakeDetectorPulses as well"<<endl;
        return 1;
    }
    // Make sure MakeDetectorPulses is called before PlotTDPs
    if(!modules::navigator::Instance()->Before("MakeDetectorPulses",GetName())){
        cout<<"MakeDetectorPulses must be used before PlotTDPs"<<endl;
        return 2;
    }

    // Init loop variables
    IDs::generator passThruId=makeTDPs->GetPassThruGeneratorID();
    Detector_t tmp;
    std::string name;
    std::string title;
    std::stringstream full_title;

    // Loop over all TDP sources
    for(SourceDetPulseMap::const_iterator i_source=gDetectorPulseMap.begin();
            i_source!= gDetectorPulseMap.end(); ++i_source){

        // Ignore TDP sources that use the pass-through generator (so only have
        // one valid input channel, like a scintillator or the muScA)
        if(i_source->first==passThruId){
            cout<<"Skipping TDP source, '"<<i_source->first<<"' as it was made with the pass-through generator"<<endl;
            continue;
        }

        // make the name and title for the histogram
        name='h'+i_source->first.str();
        modules::parser::ToCppValid(name);
        title=" coming from ";
        title+=i_source->first.str();
        if(Debug()) {
            cout<<"Made histogram called "<<name<<" with title "<< title<<endl;
        }

        // Make a histogram of the relative amplitudes
        tmp.amplitudes=new TH2F((name+"_amp").c_str(),("Amplitudes of TDPs "+title).c_str(), 100,0,1000,100,0,1000);
        tmp.amplitudes->SetBit(TH1::kCanRebin);
        tmp.amplitudes->SetXTitle("Amplitude in Fast channel");
        tmp.amplitudes->SetYTitle("Amplitude in Slow channel");

        // Histogram amplitudes for pulses not in either channel
        tmp.fast_only_amps=new TH1F((name+"_fast_only_amp").c_str(),
                ("Amplitudes of hits with no corresponding slow pulse "+title).c_str(), 200,0,-1);
        tmp.fast_only_amps->SetBit(TH1::kCanRebin);
        tmp.fast_only_amps->SetXTitle("Amplitude in Fast channel");

        // Histogram amplitudes for pulses not in either channel
        tmp.slow_only_amps=new TH1F((name+"_slow_only_amp").c_str(),
                ("Amplitudes of hits with no corresponding fast pulse "+title).c_str(), 200,0,-1);
        tmp.slow_only_amps->SetBit(TH1::kCanRebin);
        tmp.slow_only_amps->SetXTitle("Amplitude in Slow channel");

        // Make a histogram of the time difference between pulses
        full_title.str("Time difference vs. fast amplitude for pulses ");
        full_title<<title;
        tmp.time_diff_fast=new TH2F((name+"_fast_amp_tdiff").c_str(),full_title.str().c_str(),200,0,-1,200,0,-1);
        tmp.time_diff_fast->SetBit(TH1::kCanRebin);
        tmp.time_diff_fast->SetXTitle("t_{Fast} - t_{Slow}");
        tmp.time_diff_fast->SetYTitle("Amplitude in Fast channel");

        full_title.str("Time difference vs. slow amplitude for pulses ");
        full_title<<title;
        tmp.time_diff_slow=new TH2F((name+"_slow_amp_tdiff").c_str(),full_title.str().c_str(),200,0,-1,200,0,-1);
        tmp.time_diff_slow->SetBit(TH1::kCanRebin);
        tmp.time_diff_slow->SetXTitle("t_{Fast} - t_{Slow}");
        tmp.time_diff_slow->SetYTitle("Amplitude in Slow channel");

        if(fCutHists){
            // Histogram amplitudes for fast pulses with a slow pulse cut
            full_title.str("");
            full_title<<"Amplitudes of fast pulses for slow pulses with amplitudes above ";
            full_title<<fFastCut<<title;
            tmp.slow_amps_fast_cut=new TH1F((name+"_slow_amp_fast_cut").c_str(),
                    full_title.str().c_str(), 200,0,-1);
            tmp.slow_amps_fast_cut->SetBit(TH1::kCanRebin);
            tmp.slow_amps_fast_cut->SetXTitle("Amplitude in Slow channel");

            // Histogram amplitudes for slow pulses with a fast pulse cut
            full_title.str("");
            full_title<<"Amplitudes of slow pulses for fast pulses with amplitudes above ";
            full_title<<fSlowCut<<title;
            tmp.fast_amps_slow_cut=new TH1F((name+"_fast_amps_slow_cut").c_str(),
                    full_title.str().c_str(), 200,0,-1);
            tmp.fast_amps_slow_cut->SetBit(TH1::kCanRebin);
            tmp.fast_amps_slow_cut->SetXTitle("Amplitude in Fast channel");
        } else{
            tmp.fast_amps_slow_cut=NULL;
            tmp.slow_amps_fast_cut=NULL;
        }

        // Add tmp to the list of TDPs to draw
        fPlotsList[i_source->first]=tmp;
    }

    // Change back to the top level directory
    GetDirectory()->cd("/");
  return 0;
}

int PlotTDPs::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

    // For each TDP source of interest (defined in BeforeFirstEntry)
    const DetectorPulseList* pulseList;
    SourceDetPulseMap::const_iterator i_pulse_list;
    double fast_amp, slow_amp, fast_time, slow_time;
    Detector_t plots;
    for(PlotsList_t::iterator i_source=fPlotsList.begin();
            i_source!=fPlotsList.end(); ++i_source){
        // Get the desired pulse list
        i_pulse_list=gDetectorPulseMap.find(i_source->first);
        if(i_pulse_list== gDetectorPulseMap.end()){
            cout<<"Unable to find TDP list for source, "<<i_source->first<<endl;
            return 1;
        }
        pulseList=&i_pulse_list->second;
        plots=i_source->second;

        // Loop over the pulse list
        for(DetectorPulseList::const_iterator i_pulse=pulseList->begin();
                i_pulse!=pulseList->end(); ++i_pulse){

            // get the amplitudes for each pulse
            fast_amp=(*i_pulse)->GetAmplitude(TDetectorPulse::kFast);
            slow_amp=(*i_pulse)->GetAmplitude(TDetectorPulse::kSlow);
            fast_time=(*i_pulse)->GetTime(TDetectorPulse::kFast);
            slow_time=(*i_pulse)->GetTime(TDetectorPulse::kSlow);

            // If one of the channels is the default value then fill the
            // histogram for channels that weren't matched
            if(fast_amp==definitions::DefaultValue){
                plots.slow_only_amps->Fill(slow_amp);
            }else if(slow_amp==definitions::DefaultValue){
                plots.fast_only_amps->Fill(fast_amp);
            }else {
                // Both channels saw hits, fill the 2d plot
                plots.amplitudes->Fill(fast_amp ,slow_amp);
                plots.time_diff_fast->Fill(fast_amp ,slow_time - fast_time);
                plots.time_diff_slow->Fill(slow_amp ,slow_time - fast_time);
            }

            if(!fCutHists) continue;
            // Fill the cut histograms
            if(fast_amp> fFastCut && slow_amp!=definitions::DefaultValue){
                plots.slow_amps_fast_cut->Fill(slow_amp);
            }
            if(slow_amp> fSlowCut && fast_amp!=definitions::DefaultValue){
                plots.fast_amps_slow_cut->Fill(fast_amp);
            }

        }
    }

    return 0;
}

int PlotTDPs::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
    // Draw and save all histograms
    for(PlotsList_t::iterator i_source=fPlotsList.begin();
            i_source!=fPlotsList.end(); ++i_source){
        i_source->second.amplitudes->Draw();
        i_source->second.slow_only_amps->Draw();
        i_source->second.fast_only_amps->Draw();
        i_source->second.fast_amps_slow_cut->Draw();
        i_source->second.slow_amps_fast_cut->Draw();
        i_source->second.time_diff_fast->Draw();
        i_source->second.time_diff_slow->Draw();
    }

  return 0;
}

ALCAP_REGISTER_MODULE(PlotTDPs,slow_ch_cut,fast_ch_cut);
