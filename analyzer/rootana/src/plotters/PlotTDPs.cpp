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
#include "PassThroughDPGenerator.h"
#include "SetupNavigator.h"

#include "TH2F.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TFitResult.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
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
    IDs::generator passThruId=PassThroughDPGenerator::GetStaticId();
    Detector_t tmp;
    std::string name;
    std::string title;
    std::stringstream full_title;
    IDs::channel fast_id;
    IDs::channel slow_id;
    int n_bits;
    double fast_amp_max, slow_amp_max;
    double fast_amp_min=0, slow_amp_min=0;
    double tdiff_min=-1e6, tdiff_max=1e6;

    // Loop over all TDP sources
    for(SourceDetPulseMap::const_iterator i_source=gDetectorPulseMap.begin();
            i_source!= gDetectorPulseMap.end(); ++i_source){

        // Ignore TDP sources that use the pass-through generator (so only have
        // one valid input channel, like a scintillator or the muScA)
        if(passThruId.matches(i_source->first.Generator())){
            cout<<"Skipping TDP source, '"<<i_source->first<<"' as it was made with the pass-through generator"<<endl;
            continue;
        }

        // make the name and title for the histogram
        name='h'+i_source->first.str();
        modules::parser::ToCppValid(name);
        title=" from ";
        title+=i_source->first.str();
        if(Debug()) {
            cout<<"Made histogram called "<<name<<" with title "<< title<<endl;
        }

        // Setup the Axis limits
        fast_id=i_source->first.Channel();
        fast_id.SlowFast(IDs::kFast);
        slow_id=fast_id.GetCorrespondingFastSlow();
        try{
           n_bits = setup->GetNBits(SetupNavigator::Instance()->GetBank(fast_id));
           fast_amp_max = std::pow(2, n_bits);
           n_bits = setup->GetNBits(SetupNavigator::Instance()->GetBank(slow_id));
           slow_amp_max = std::pow(2, n_bits);
        }catch(Except::InvalidDetector& e){
           cout<<"PlotTDPs::BeforeFirstEntry: Skipping channel '"<<i_source->first.Channel()
               <<"' due to an error querying the setup navigator.  This is probably casued by only 1 of "
                 "the fast/slow channels being connected for this run."<<endl;
           continue;
        }

        // Make a histogram of the relative amplitudes
        tmp.amplitudes=new TH2F((name+"_amp").c_str(),("Amplitudes of TDPs "+title).c_str(),
                150,fast_amp_min,fast_amp_max,150,slow_amp_min,slow_amp_max);
        tmp.amplitudes->SetXTitle("Amplitude in Fast channel");
        tmp.amplitudes->SetYTitle("Amplitude in Slow channel");

        // Histogram amplitudes for pulses not in either channel
        tmp.fast_only_amps=new TH1F((name+"_fast_only_amp").c_str(),
                ("Amplitudes of hits with no corresponding slow pulse "+title).c_str(), 
                200,fast_amp_min,fast_amp_max);
        tmp.fast_only_amps->SetXTitle("Amplitude in Fast channel");

        // Histogram amplitudes for pulses not in either channel
        tmp.slow_only_amps=new TH1F((name+"_slow_only_amp").c_str(),
                ("Amplitudes of hits with no corresponding fast pulse "+title).c_str(), 
                200,slow_amp_min,slow_amp_max);
        tmp.slow_only_amps->SetXTitle("Amplitude in Slow channel");

        // Histogram of amplitude ratio to show the scale factor in paired
        // channels
        tmp.scale_factor=new  TH1F((name+"_scale_factor").c_str(),
                ("Ratio of amplitudes "+title).c_str(), 
                200,0,10);
        tmp.scale_factor->SetXTitle("Ratio of amplitudes, A_{Slow} / A_{Fast}");

        // Make a histogram of the time difference between pulses
        full_title.str("Time difference vs. fast amplitude for pulses ");
        full_title<<title;
        tmp.time_diff_fast=new TH2F((name+"_fast_amp_tdiff").c_str(),full_title.str().c_str(),
				    200,tdiff_min,tdiff_max,200,fast_amp_min,fast_amp_max);
        tmp.time_diff_fast->SetXTitle("t_{Fast} - t_{Slow}");
        tmp.time_diff_fast->SetYTitle("Amplitude in Fast channel");

        full_title.str("Time difference vs. slow amplitude for pulses ");
        full_title<<title;
        tmp.time_diff_slow=new TH2F((name+"_slow_amp_tdiff").c_str(),full_title.str().c_str(),
                200,tdiff_min,tdiff_max,200,slow_amp_min,slow_amp_max);
        tmp.time_diff_slow->SetXTitle("t_{Fast} - t_{Slow}");
        tmp.time_diff_slow->SetYTitle("Amplitude in Slow channel");

        tmp.times=new TH2F((name+"_times").c_str(),("Times of pulses "+title).c_str(),
                200,0,-1,200,0,-1);
        tmp.times->SetXTitle("Time in Fast channel");
        tmp.times->SetYTitle("Time in Slow channel");

        if(fCutHists){
            // Histogram amplitudes for fast pulses with a slow pulse cut
            full_title.str("");
            full_title<<"Amplitudes in slow channel if paired to a fast pulse with amplitude above ";
            full_title<<fFastCut<<title;
            tmp.slow_amps_fast_cut=new TH1F((name+"_slow_amp_fast_cut").c_str(),
                    full_title.str().c_str(), 200,slow_amp_min,slow_amp_max);
            tmp.slow_amps_fast_cut->SetXTitle("Amplitude in Slow channel");

            // Histogram amplitudes for slow pulses with a fast pulse cut
            full_title.str("");
            full_title<<"Amplitudes in fast channel if paired to a slow pulse with amplitude above ";
            full_title<<fSlowCut<<title;
            tmp.fast_amps_slow_cut=new TH1F((name+"_fast_amps_slow_cut").c_str(),
                    full_title.str().c_str(), 200,fast_amp_min,fast_amp_max);
            tmp.fast_amps_slow_cut->SetXTitle("Amplitude in Fast channel");
        } else{
            tmp.fast_amps_slow_cut=NULL;
            tmp.slow_amps_fast_cut=NULL;
        }

        tmp.amp_status=tmp.amp_inter=tmp.amp_grad=0;

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
                plots.amplitudes->Fill(slow_amp,fast_amp);
                plots.time_diff_fast->Fill(slow_time - fast_time ,fast_amp);
                plots.time_diff_slow->Fill(slow_time - fast_time ,slow_amp);
                plots.times->Fill(slow_time, fast_time);
                plots.scale_factor->Fill(slow_amp/fast_amp);
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
        Detector_t& plots=i_source->second;
        //plots.amplitudes->Draw();
        //plots.slow_only_amps->Draw();
        //plots.fast_only_amps->Draw();
        //plots.time_diff_fast->Draw();
        //plots.time_diff_slow->Draw();
        //if(fCutHists){
        //    plots.fast_amps_slow_cut->Draw();
        //    plots.slow_amps_fast_cut->Draw();
        //}
        TProfile* profile=plots.amplitudes->ProfileX(Form("%s_fit",plots.amplitudes->GetName()));
        TFitResultPtr fit=profile->Fit("pol1","QS");
        if(((int)fit)!=0){
            cout<<"PlotTDPs: Warning: fitting to 2D amplitude plots failed with status: "
                << (int)fit <<" for "<<i_source->first<<endl;
        } else {
            plots.amp_inter=fit->Parameter(0);
            plots.amp_grad=fit->Parameter(1);
        }
        plots.amp_status=fit;
    }

    if(Debug()){
        cout<<"PlotTDPs: Fitted amplitude gradients:"<<endl;
        cout<<"   intercept\t| gradient\t| source"<<endl;
        cout<<"   ---------\t| --------\t| ------"<<endl;
        for(PlotsList_t::const_iterator i_source=fPlotsList.begin();
                i_source!=fPlotsList.end(); ++i_source){
            const Detector_t& plots=i_source->second;
            if(!plots.amp_status)
                cout<< "  "<<plots.amp_inter<<"\t| "<<  plots.amp_grad<<"\t| "<<i_source->first<<endl;
        }
    }


  return 0;
}

ALCAP_REGISTER_MODULE(PlotTDPs,slow_ch_cut,fast_ch_cut);
