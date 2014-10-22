#include "SavePulses.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "ModulesNavigator.h"
#include "ModulesParser.h"
#include "definitions.h"
#include "IdSource.h"
#include "TAnalysedPulse.h"
#include "TAPGeneratorFactory.h"

#include "TTree.h"
#include "TClonesArray.h"

#include <iostream>
using std::cout;
using std::endl;

extern SourceAnalPulseMap gAnalysedPulseMap;

SavePulses::SavePulses(modules::options* opts):
   BaseModule("SavePulses",opts,false){
       fTree=new TTree("TAP_tree","Tree storing all TAPs made in rootana");
}

SavePulses::~SavePulses(){
}

int SavePulses::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){

    // Check we're after MakeAnalysedPulses
    if(!modules::navigator::Instance()->Before("MakeAnalysedPulses",GetName())){
        cout<<"SavePulses: Error: I need to be run after MakeAnalysedPulses"<<endl;
        return 1;
    }

    // Loop over gAnalysedPulseMap and add a branch for each source
    TClonesArray* array;
    std::string name;
    for (SourceAnalPulseMap::const_iterator i_source = gAnalysedPulseMap.begin();
            i_source != gAnalysedPulseMap.end(); ++i_source) {
        std::string tap_type=TAPGeneratorFactory::Instance()->GetTAPType(i_source->first);
        array=new TClonesArray(tap_type.c_str());
        fArrays[i_source->first]=array;
        name=i_source->first.str();
        modules::parser::ToCppValid(name);
        fTree->Branch(name.c_str(), array);
    }

  return 0;
}

int SavePulses::ProcessEntry(TGlobalData* gData,const TSetupData *setup){
    const AnalysedPulseList* pulseList;
    TClonesArray* array;
    TObject* tap;
    for (SourceAnalPulseMap::const_iterator i_source = gAnalysedPulseMap.begin();
            i_source != gAnalysedPulseMap.end(); ++i_source) {
        pulseList=&i_source->second;
        array=fArrays[i_source->first];
        for (AnalysedPulseList::const_iterator i_pulse = pulseList->begin();
                i_pulse != pulseList->end(); ++i_pulse) {
            tap= array->ConstructedAt(i_pulse-pulseList->begin());
            (*i_pulse)->Copy(*tap);
        }
    }
    fTree->Fill();
    for(SourceToClonesArrayMap::iterator i_array=fArrays.begin();
            i_array!=fArrays.end(); ++i_array){
        i_array->second->Clear("C");
    }
  return 0;
}

int SavePulses::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
    FlyWeight<IDs::source,TAnalysedPulse::Tag>::SaveProxyList(GetDirectory(),"TAP_sources");
  return 0;
}

ALCAP_REGISTER_MODULE(SavePulses);
