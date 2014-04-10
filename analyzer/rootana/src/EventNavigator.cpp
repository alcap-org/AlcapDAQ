#include "EventNavigator.h"


//C++/STL
#include <iostream>
#include <string>

//ROOT
#include "TFile.h"
#include "TTree.h"

//Local

TGlobalData* TGlobalData::Instance()
{
  return g_event;
}

#define DEBUG_PRINT std::cout << "At line " << __LINE__ << std::endl;

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


//----------------------------------------------------------------------
void EventNavigator::CopyTree()
{
  DEBUG_PRINT
  Long64_t nentries = fEventTree->GetEntries();
  nentries = 100;
  TGlobalData* event = 0x0;
  fEventTree->SetBranchAddress("Event", &event);
  DEBUG_PRINT

  fOutput->ls();
  TTree* out_tree = fEventTree->CloneTree(0);
  for (Long64_t i=0; i < nentries; ++i){
    fEventTree->GetEntry(i);
    out_tree->Fill();
    if (i ==    1 || i ==    2 || i ==    5 ||
	i ==   10 || i ==   20 || i ==   50 ||
	i ==  100 || i ==  200 || i ==  500 ||
	i == 1000 || i == 2000 || i == 5000 ) std::cout << "Event " << i << std::endl;
  }
  std::cout << nentries << " ENTRIES" << std::endl;

  out_tree->Print();
  out_tree->AutoSave();
  fOutput->ls();
  fOutput->Close();
  DEBUG_PRINT

}
