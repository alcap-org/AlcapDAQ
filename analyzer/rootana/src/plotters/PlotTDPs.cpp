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
   BaseModule("PlotTDPs",opts){
}

PlotTDPs::~PlotTDPs(){
}

int PlotTDPs::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){

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
        title=" of TDPs coming from ";
        title+=i_source->first.str();
        if(Debug()) {
            cout<<"Made histogram called "<<name<<" with title "<< title<<endl;
        }

        // Make a histogram of the relative amplitudes
        tmp.amplitudes=new TH2F((name+"_amp").c_str(),("Amplitudes "+title).c_str(), 100,0,1000,100,0,1000);
        tmp.amplitudes->SetBit(TH1::kCanRebin);

        // Make a histogram of the time difference between pulses
        tmp.time_diff=NULL;

        // Add tmp to the list of TDPs to draw
        fPlotsList[i_source->first]=tmp;
    }

  return 0;
}

int PlotTDPs::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

    // For each TDP source of interest (defined in BeforeFirstEntry)
    const DetectorPulseList* pulseList;
    SourceDetPulseMap::const_iterator i_pulse_list;
    for(PlotsList_t::iterator i_source=fPlotsList.begin();
            i_source!=fPlotsList.end(); ++i_source){
        // Get the desired pulse list
        i_pulse_list=gDetectorPulseMap.find(i_source->first);
        if(i_pulse_list== gDetectorPulseMap.end()){
            cout<<"Unable to find TDP list for source, "<<i_source->first<<endl;
            return 1;
        }
        pulseList=&i_pulse_list->second;

        // Loop over the pulse list
        for(DetectorPulseList::const_iterator i_pulse=pulseList->begin();
                i_pulse!=pulseList->end(); ++i_pulse){

            // Fill histogram of the relative amplitudes
            i_source->second.amplitudes->Fill(
                    (*i_pulse)->GetAmplitude(TDetectorPulse::kFast),
                    (*i_pulse)->GetAmplitude(TDetectorPulse::kSlow)
                    );

            // Fill histogram of the time difference between pulses
        }
    }

    return 0;
}

int PlotTDPs::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
    // Draw and save all histograms
    for(PlotsList_t::iterator i_source=fPlotsList.begin();
            i_source!=fPlotsList.end(); ++i_source){
        i_source->second.amplitudes->Draw();
    }

  return 0;
}

ALCAP_REGISTER_MODULE(PlotTDPs);
