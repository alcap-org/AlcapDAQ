#include "PlotIntegral.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "ModulesParser.h"
#include "definitions.h"
#include "TAnalysedPulse.h"
#include "IdSource.h"

#include <TH1F.h>

#include <iostream>
using std::cout;
using std::endl;

extern SourceAnalPulseMap gAnalysedPulseMap;

PlotIntegral::PlotIntegral(modules::options* opts):
   BaseModule("PlotIntegral",opts){
}

PlotIntegral::~PlotIntegral(){
}

int PlotIntegral::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
    // For each source,
    TH1F* hist=NULL;
    std::string name, title;
    const IDs::source* source=NULL;
    for(SourceAnalPulseMap::const_iterator i_source = gAnalysedPulseMap.begin();
            i_source!=gAnalysedPulseMap.end(); ++i_source){
        source=&i_source->first;

        // setup the name and title for the histogram
        name="h"+modules::parser::ToCppValid(source->str())+"_integrals";
        title="Integral of pulses coming from "+source->str();

        // make a histogram
        hist=new TH1F(name.c_str(),title.c_str(),300,0,-1);
        hist->SetXTitle("Integral of each pulse (arb. units)");

        // register histogram with list
        fPlots[*source]=hist;
    }
  return 0;
}

int PlotIntegral::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

    PlotList_t::iterator i_plot;
    const AnalysedPulseList* pulseList;
    const IDs::source* source;
    for(SourceAnalPulseMap::const_iterator i_source = gAnalysedPulseMap.begin();
            i_source!=gAnalysedPulseMap.end(); ++i_source){
        source=&i_source->first;
        pulseList=&i_source->second;

        // Find the corresponding histogram
        i_plot=fPlots.find(*source);
        if(i_plot==fPlots.end()){
            cout<<"PlotIntegral: Error: Unable to find plot for source '"<<*source<<"'"<<endl;
            return 1;
        }

        // For each TAP 
        for(AnalysedPulseList::const_iterator i_pulse=pulseList->begin();
                i_pulse!=pulseList->end();++i_pulse){
            // Fill integral histogram
            i_plot->second->Fill((*i_pulse)->GetIntegral());
        }
    }

  return 0;
}

int PlotIntegral::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
    for(PlotList_t::iterator i_plot=fPlots.begin();
            i_plot!=fPlots.end(); ++i_plot){
        i_plot->second->Draw();
    }
  return 0;
}

ALCAP_REGISTER_MODULE(PlotIntegral);
