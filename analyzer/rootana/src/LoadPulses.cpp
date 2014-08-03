#include "EventNavigator.h"
#include "LoadPulses.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "TAnalysedPulse.h"
#include "debug_tools.h"

#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TBranchClones.h"

#include <iostream>
#include <utility>
using std::cout;
using std::endl;

extern SourceAnalPulseMap gAnalysedPulseMap;

LoadPulses::LoadPulses(modules::options* opts):
   BaseModule("LoadPulses",opts){
       fFileName=opts->GetString("file_name");
       fTreeName=opts->GetString("tree_name","TAP_tree");
}

LoadPulses::~LoadPulses(){
}

int LoadPulses::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
    // open the file
    if(fFileName.empty()){
        cout<<"LoadPulses: Error: no input file specified "<<endl;
        return 1;
    }
    TFile* inFile=TFile::Open(fFileName.c_str(), "READ");
    if(!inFile){
        cout<<"LoadPulses: Error: Unable to open input file, "<<fFileName<<endl;
        return 2;
    }
    // open the tree
    inFile->GetObject(fTreeName.c_str(),fInTree);
    if(!fInTree){
        cout<<"LoadPulses: Error: Unable to find input tree, "<<fTreeName<<" in file "<<fFileName<<endl;
        return 3;
    }

    // Loop over each branch
    TObjArray* listOfArraysOfTAPs=fInTree->GetListOfBranches();
    TBranchClones* branch;
    IDs::source tmp_source;
        DEBUG_VALUE(listOfArraysOfTAPs->GetSize());
    for(int i_array=0;i_array<listOfArraysOfTAPs->GetSize();i_array++){
        DEBUG_VALUE(i_array);
        // Get the branch
        branch=(TBranchClones*)listOfArraysOfTAPs->At(i_array);
        if(!branch) continue;
        // Create the corrsponding source ID
        tmp_source=branch->GetName();
        DEBUG_VALUE(tmp_source);
        // Add to the list of branches
        fBranches.push_back(std::make_pair(tmp_source,branch));
        // Prepare gAnalysedPulseMap
        gAnalysedPulseMap[tmp_source];
    }

    // Get the first entry to read from
    //fCurrentEntry=EventNavigator::Instance().GetStartEntry();
    fCurrentEntry=3;
    //fInTree->SetDebug();
  return 0;
}

int LoadPulses::ProcessEntry(TGlobalData* gData,const TSetupData *setup){
    for(BranchList::iterator i_branch=fBranches.begin();
            i_branch!=fBranches.end(); ++i_branch){
        // Set the TClonesArray address
        i_branch->second->SetAddress(&fArrays[i_branch->first]);
    }

    DEBUG_VALUE(fCurrentEntry);
    fInTree->SetDebug();
    //fInTree->LoadTree(fCurrentEntry);
    fInTree->GetEntry(fCurrentEntry);
    int n_entries=0;
    SourceAnalPulseMap::iterator i_output;
    AnalysedPulseList* pulseList;
    for(SourceToClonesArrayMap::iterator i_array=fArrays.begin();
            i_array!=fArrays.end(); ++i_array){
        // Get the pulse list to put the TAPs into
        i_output=gAnalysedPulseMap.find(i_array->first);
        if(i_output==gAnalysedPulseMap.end()){
            cout<<"LoadPulses: Error: gAnalysedPulseMap doesn't contain a pulse list for "<<i_array->first<<endl;
            return 1;
        }
        pulseList=&i_output->second;
        DEBUG_VALUE(pulseList->size());

        // Loop over each TAP on file and put it into gAnalysedPulseMap;
        n_entries=i_array->second->GetEntries();
        DEBUG_VALUE(n_entries);
        TAnalysedPulse* pulse;
        for(int i_pulse=0;i_pulse<n_entries;++i_pulse){
            pulse=(TAnalysedPulse*)(i_array->second->At(i_pulse));
            pulseList->push_back(pulse);
        }
    }

    //for (SourceAnalPulseMap::const_iterator i_source = gAnalysedPulseMap.begin();
    //        i_source != gAnalysedPulseMap.end(); ++i_source) {
    //    cout<<i_source->second.size()<<" pulses for source: "<<i_source->first<<"  "<<endl;
    //}
    ++fCurrentEntry;
  return 0;
}

int LoadPulses::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
  return 0;
}

ALCAP_REGISTER_MODULE(LoadPulses,file_name,tree_name);
