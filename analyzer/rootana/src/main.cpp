/// \defgroup rootana rootana
/// \brief
/// Second stage of analysis in the AlCap framework.

/// \defgroup rootana_modules Modules
/// \ingroup rootana
/// \brief
/// Analysis modules for rootana.

/// \defgroup tap_gen Analyzed Pulse Generators
/// \ingroup rootana
/// \brief
/// TAnalysedPulse generators used by rootana.

/// \defgroup tdp_gen Detector Pulse Generators
/// \ingroup rootana
/// \brief
/// TDetectorPulse generators used by rootana

////////////////////////////////////////////////////////////////////////////////
/// \degroup rootana_main
/// \ingroup rootana
///
/// @{
////////////////////////////////////////////////////////////////////////////////

//#define USE_PRINT_OUT 


// C++/STL
#include <iostream>
#include <string>
//#include <cctype>
#include <map>

#include "SetupNavigator.h"
#include "definitions.h"

// ROOT
#include "TTree.h"
#include "TBranch.h"
#include "TFile.h"

// Local
#include "CommandLine.h" // Provides analyze_command_line()
#include "ModulesFactory.h"
#include "ModulesNavigator.h"
#include "BaseModule.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "TMuonEvent.h"
//#include "ProcessCorrectionFile.h" // Provides CheckSetupData()
#include "TAnalysedPulseMapWrapper.h"
#include "EventNavigator.h"

#include "debug_tools.h"

// Forward declaration of functions ======================
Int_t Main_event_loop(TTree* dataTree, ARGUMENTS& arguments);
void ClearGlobalData(TGlobalData*);
TTree* GetTree(TFile* inFile, const char* t_name);
Int_t PrepareAnalysedPulseMap(TFile* fileOut);
Int_t PrepareSingletonObjects(const ARGUMENTS&);

//TAnalysedPulseMapWrapper *gAnalysedPulseMapWrapper=NULL;
//TTree *gAnalysedPulseTree = NULL;
//TBranch *gAnalysedPulseBranch = NULL;

SourceAnalPulseMap gAnalysedPulseMap;
SourceDetPulseMap gDetectorPulseMap;
MuonEventList gMuonEvents;

int main(int argc, char **argv)
{
  //load_config_file("MODULES.txt");

  // Parse the command line
  ARGUMENTS arguments;
  int ret = analyze_command_line (argc, argv,arguments);
  if(ret!=0) return ret;
  std::cout << "Starting event" << std::endl;
  // Give the command line arguments to the navigator
  SetupNavigator::Instance()->SetCommandLineArgs(arguments);
  SetupNavigator::Instance()->CacheCalibDB();

  // Read in the configurations file
  ret= modules::navigator::Instance()->LoadConfigFile(arguments.mod_file.c_str());
  if(ret!=0) {
     std::cout<<"Error: Problem loading MODULES file"<<std::endl;
     return ret;
  }

  // Open the input tree file
  EventNavigator& navi = EventNavigator::Instance();
  if ( ! navi.ConnectInput(arguments.infile.c_str()) ){
    std::cout << "Failed to open input file '"
              << arguments.infile <<"'.  Exiting." << std::endl;
    return 1;
  }
    
  // Make an initial call to singleton objects that are very likely to be called at some point.
  ret = PrepareSingletonObjects(arguments);
  if(ret!=0) {
     std::cout<<"Error: Problem creating a singleton object..."<<std::endl;
     return ret;
  }

  // Now that we've loaded the TSetupData for this run check if there are any
  // suggested replacements for the wiremap data
  //CheckSetupData(navi.GetSetupData(), arguments.correction_file);

  //Event Tree
  TTree* eventTree = navi.GetRawTree(); 

  // Let's open the output file for analysis data, histograms and so on.
  TFile *fileOut = new TFile(arguments.outfile.c_str(), "RECREATE");
  if(!fileOut->IsOpen()){
    std::cout << "Could not open ROOT output file " 
              << arguments.outfile << std::endl;
    return 1;
  }
  modules::navigator::Instance()->SetOutFile(fileOut);

  // Now let's setup all the analysis modules we want
  // NOTE: This has to be done after the output file was opened else the
  // modules wont have a directory to store things to.
  ret= modules::navigator::Instance()->MakeModules();
  if(ret!=0) {
    std::cout << "Problem creating analysis modules." << std::endl;
    return ret;
  }
  
  // Setup the analysed pulse map to store / read in the pulses
  ret = PrepareAnalysedPulseMap(fileOut);
  if(ret!=0) {
    std::cout << "Problem preparing analysed pulse map." << std::endl;
    return ret;
  }

  // Finally let's do the main loop
  fileOut->cd();
  try {
    Main_event_loop(eventTree,arguments);
  }
  catch (std::exception& e){
    std::cout << "Terminating due to unexpected exception:\n" 
              << e.what() << std::endl; 
  }
  catch (...){
    std::cout << "Terminating due to unknown exception" << std::endl;
  }
  
  // and finish up
  fileOut->cd();
  //gAnalysedPulseTree->Write();
  fileOut->Write();
  fileOut->Close();
  navi.Close();
  SetupNavigator::Instance()->Close();
  return 0;
}

//----------------------------------------------------------------------
Int_t Main_event_loop(TTree* dataTree,ARGUMENTS& arguments)
{
  //Loop over tree entries and call histogramming modules.
  EventNavigator& enav = EventNavigator::Instance();
  Long64_t nEntries = enav.GetInputNEntries();
  std::cout << "Raw data tree contains " << nEntries 
            << " entries." << std::endl
            << "Processing file, which may take a while..."
            << std::endl;

  //set up the input data
  TGlobalData* raw_data = enav.GetRawData();
  if ( raw_data ){
    raw_data->Clear("C");
  }
  
  enav.MakeLoopSequence(arguments).Run();
  std::cout << "Finished processing data" << std::endl;
  return 0;
}


//----------------------------------------------------------------------
void ClearGlobalData(TGlobalData* data)
{
  // TODO: this comment is now gibberish.
  // We could put this into TGlobalData::Clear(), but we need
  // to be sure that's okay at the alcapana level. That is, if
  // Clear() deletes the TPulseIsland objects, but g_event doesn't
  // own the pulses, they would be deleted later. A solution is to
  // be sure that TGlobalData isn't called in alcapana, or ensure
  // that g_event owns the pulse islands at that level.  
//
//---  LoopSequence::Process now takes care of this part by invoking TGlobalData::Clear
//  StringPulseIslandMap::iterator mapIter;
//  StringPulseIslandMap::iterator mapEnd = data->fPulseIslandToChannelMap.end();
//  for(mapIter = data->fPulseIslandToChannelMap.begin(); mapIter != mapEnd; mapIter++) {
//    // The iterator is pointing to a pair<string, vector<TPulseIsland*> >
//    std::vector<TPulseIsland*>& pulse_vector= mapIter->second;
//    for(size_t i=0; i<pulse_vector.size(); i++){
//      delete pulse_vector[i];
//      pulse_vector[i] = NULL;
//    }
//    pulse_vector.clear();
//  }

    // we should not clear TAPs if we're loading them in from a previously made
    // TAP tree (which uses a TClonesArray that is responsible for deleting them )
    static bool should_delete_TAPS=!modules::navigator::Instance()->GetModule("LoadPulses");
    for(SourceAnalPulseMap::iterator mapIter=gAnalysedPulseMap.begin();
            mapIter != gAnalysedPulseMap.end(); ++mapIter) {

        // The iterator is pointing to a pair<string, vector<TPulseIsland*> >
        AnalysedPulseList& pulse_vector= mapIter->second;
        if(should_delete_TAPS){
            for(size_t i=0; i<pulse_vector.size(); i++){
                delete pulse_vector[i];
                pulse_vector[i] = NULL;
            }
        }
    pulse_vector.clear();
  }
  //  gAnalysedPulseMap.clear();

  for(SourceDetPulseMap::iterator mapIter = gDetectorPulseMap.begin();
          mapIter != gDetectorPulseMap.end(); ++mapIter) {
    // The iterator is pointing to a pair<string, vector<TPulseIsland*> >
    DetectorPulseList& pulse_vector= mapIter->second;
    for(size_t i=0; i<pulse_vector.size(); i++){
      delete pulse_vector[i];
      pulse_vector[i] = NULL;
    }
    pulse_vector.clear();
  }
  //gDetectorPulseMap.clear();

  for(MuonEventList::iterator muonEvent = gMuonEvents.begin();
          muonEvent != gMuonEvents.end(); ++muonEvent) {
      delete *muonEvent;
  }
  gMuonEvents.clear();
}


//----------------------------------------------------------------------
void PrintSetupData(TSetupData* s_data)
{
  if(!s_data) return;
  // print things out
  std::map<std::string, std::string>::iterator it_info;
  std::cout << "### TSetupData ###\n"
            << "Bank  Detector Name  Clock Tick (ns)  Pedestal (ADC)"
            << "  Trigger Polarity  Time Shift (ns)  No. of Bits\n"
            << "----  -------------  ---------------  --------------"
            << "  ----------------  ---------------  -----------" 
            << std::endl;

  for (it_info=s_data->fBankToDetectorMap.begin(); 
       it_info!=s_data->fBankToDetectorMap.end(); ++it_info){
    const std::string& bankname = it_info->first;
    std::cout << bankname << "  " 
              << s_data->GetDetectorName(bankname) << "\t\t" 
              << s_data->GetClockTick(bankname) << "\t" 
              << s_data->GetPedestal(bankname) << "\t\t" 
              << s_data->GetTriggerPolarity(bankname) << "\t\t" 
              << s_data->GetTimeShift(bankname) << "\t\t" 
              << s_data->GetNBits(bankname)
              << std::endl;
  }
}

//----------------------------------------------------------------------
Int_t PrepareAnalysedPulseMap(TFile* fileOut)
{
  // TAnalysedMapWrapper
  //gAnalysedPulseMapWrapper = new TAnalysedPulseMapWrapper(gAnalysedPulseMap);
  
  int split = 1;
  //int bufsize = 64000;
  Int_t branchstyle = 1;
  
  if (split < 0) {branchstyle = 0; split = -1-split;}
  TTree::SetBranchStyle(branchstyle);
  
  //gAnalysedPulseTree = new TTree("AnalysedPulseTree", "AnalysedPulseTree");
  //gAnalysedPulseTree->SetAutoSave(100000000);
  //gAnalysedPulseTree->SetMaxVirtualSize(100000000);
  //gAnalysedPulseBranch = gAnalysedPulseTree->Branch("AnalysedPulseMapWrapper", 
  //                                                  "TAnalysedPulseMapWrapper",
  //                                                  &gAnalysedPulseMapWrapper,
  //                                                  bufsize, split);
  //gAnalysedPulseBranch->SetAutoDelete(kFALSE);
  return 0;
}

//----------------------------------------------------------------------
Int_t PrepareSingletonObjects(const ARGUMENTS&){
  // Set up the modules navigator
  if(!modules::navigator::Instance()) return 1;
  
  // Make sure the instance of TSetupData get's loaded 
  // (though in practice we can do this lazily)
  if( !EventNavigator::Instance().GetSetupData() ) return 2;
  
  return 0;
}

/// @}
