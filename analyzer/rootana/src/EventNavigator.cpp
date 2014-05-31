#include "EventNavigator.h"

//C++/STL
#include <iostream>
#include <string>
#include <cmath>

//ROOT
#include "TFile.h"
#include "TTree.h"

//Local
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "TMuonEvent.h"
#include "debug_tools.h"


TGlobalData* TGlobalData::Instance()
{
  return g_event;
}

//======================================================================
EventNavigator* EventNavigator::fInstance = 0x0;

//----------------------------------------------------------------------
EventNavigator& EventNavigator::Instance()
{
  if (!fInstance) fInstance = new EventNavigator();
  return *fInstance;
}

//----------------------------------------------------------------------
EventNavigator::EventNavigator()
{
  //no-op
}

//----------------------------------------------------------------------
Bool_t EventNavigator::ConnectInput(const char* input_file_name,
					Bool_t read_only)
{
  //For now we  dont support identical output files
  if (read_only == false){
    std::cout << "WARN:  " << "re-writable files not supported!"
	      << std::endl;
    read_only = true;
  }


  //Not very careful about tidying up after failures or repeat attempts
  //here.  Probably doesen't matter since the program is likely to
  //abort shortly afterward.  Revisit if that is not true.

  //Open the file
  fInput = TFile::Open(input_file_name, (read_only ? "READ" : "UPDATE"));
  if (!fInput || fInput->IsZombie())  return false;
  //if (!read_only && !fOutput) fOutput = fInput;

  //Look for setup tree
  TTree* setup_tree = 0x0;
  fInput->GetObject("SetupTree", setup_tree);
  if (!setup_tree || setup_tree->GetEntriesFast() == 0) return false;

  //Look for data tree
  TTree* event_tree = 0x0;
  fInput->GetObject("EventTree", event_tree);
  if (!event_tree || event_tree->GetEntriesFast() == 0) return false;

  //All set
  fSetupTree = setup_tree;
  fEventTree = event_tree;
  return true;
}


//----------------------------------------------------------------------
Bool_t EventNavigator::ConnectOutputFile(const char* output_file_name,
					 Bool_t append)
{
  //Not very careful about tidying up after failures or repeat attempts
  //here.  Probably doesen't matter since the program is likely to
  //abort shortly afterward.  Revisit if that is not true.
  TFile* prev_output = fOutput;

  fOutput = TFile::Open(output_file_name, (append ? "UPDATE" : "RECREATE"));
  if (!fOutput || fOutput->IsZombie()) return false; 
  
  return true;
}


extern void ClearGlobalData(TGlobalData* data);
//----------------------------------------------------------------------
void EventNavigator::CopyTree()
{
  //DEBUG_PRINT
  Long64_t nentries = fEventTree->GetEntries();
  //nentries = 100;
  TGlobalData* event = new TGlobalData();
  fEventTree->SetBranchAddress("Event", &event);
  fEventTree->GetDirectory()->Print();

  //DEBUG_PRINT

  fOutput->ls();
  gFile->ls();
  TTree* out_tree = new TTree("output_tree", "out");//fEventTree->CloneTree(0);
  TTree* out_tree2 = new TTree("friend", "Friendly tree");
  out_tree->SetName("ouput_event");
  out_tree2->SetName("friendly_event");
  Int_t something; Int_t something_else; Float_t ratio;
  out_tree->Branch("Event", &event);
  out_tree->Branch("demo",&something,"demo/I");
  out_tree->SetAutoSave(1000000);
  out_tree->AddFriend(out_tree2);
  out_tree2->Branch("mode",&something_else,"mode/I");
  out_tree2->Branch("R",&ratio,"R/F");
  out_tree2->SetAutoSave(1000000);
  
  Long64_t i;
  DEBUG::check_clock();
  for (/*Long64_t*/ i=0; i < nentries; ++i){
    //std::cout << "on entry" << i << "/" << nentries << std::endl;
    //fEventTree->GetEntry(20);
    fEventTree->GetEntry(i);
    something = i % 25;
    something_else = i % 7;
    ratio = (1. * something) / (1+something_else);
    out_tree->Fill();
    out_tree2->Fill();
    //out_tree->AutoSave();
    out_tree2->AutoSave();
    
    if ( i ==    1 || i ==    2 || i ==    5 ||
	 i ==   10 || i ==   20 || i ==   50 ||
	 i ==  100 || i ==  200 || i ==  500 ||
	 i % 1000 == 0 ) {
      std::cout << "Event " << i  
		<< ": \t" << DEBUG::check_clock().str
		<< "\t" << DEBUG::check_mem().str 
		<< std::endl;
      
    }
    //std::cout << "map size: "<<event->fPulseIslandToChannelMap.size() << std::endl;
    typedef std::vector<TPulseIsland*> PI_list_t;
    typedef std::map<std::string, PI_list_t> PI2CM_t;
    typedef PI2CM_t::iterator PI2C_iter_t;
    int npi  =0; int npi2 = 0;
    for (PI2C_iter_t it = event->fPulseIslandToChannelMap.begin();
	 it != event->fPulseIslandToChannelMap.end(); ++it){
      npi += it->second.size();
      PI_list_t& bank = it->second;
      for (int pui = 0; pui < bank.size() ; ++pui){
	delete bank[pui];
      }
      bank.clear();
      npi2 +=  it->second.size();
      //typedef std::vector<int> PIdata_t; 
      //PIdata_t samples = 
      //for (int s =  
      
    }
    //std::cout << "N islands: "<< npi << std::endl;
    //std::cout << "After del: "<< npi2 << std::endl;
    
    
    //event->Clear();
    //ClearGlobalData(event);
    fEventTree->Clear();
    out_tree->Clear();
    out_tree2->Clear();
  }
  std::cout << nentries << " Entries" << std::endl;
  delete event;
  out_tree->Print();
  out_tree->AutoSave();
  out_tree2->Print();
  out_tree2->AutoSave();
  
  fOutput->ls();
  fOutput->Close();
  //DEBUG_PRINT

}
