#include "EventNavigator.h"

//C++/STL
#include <string>

//ROOT
#include "TFile.h"
#include "TTree.h"

//Local

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
  //noop
}

//----------------------------------------------------------------------
Bool_t EventNavigator::ConnectInputFile(const char* input_file_name,
					Bool_t read_only)
{
  //Not very careful about tidying up after failures here.  Probably
  //doesen't matter since the program is likely to abort shortly
  //afterward.  Revisit if that is not true.

  //Open the file
  fInput = TFile::Open(input_file_name, (read_only ? "READ" : "UPDATE"));
  if (!fInput || fInput->IsOpen())  return false;
  if (!read_only && !fOutput) fOutput = fInput;


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
