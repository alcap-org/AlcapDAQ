//#define USE_PRINT_OUT 

#include <cstdio>
#include <string>
//#include <cctype>
#include <map>

#include "CommandLine.h" // Provides analyze_command_line()

#include "ModulesReader.h"
#include "ModulesFactory.h"
#include "ModulesNavigator.h"
#include "FillHistBase.h"

#include "TTree.h"
#include "TBranch.h"
#include "TFile.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "TMuonEvent.h"
//#include "ProcessCorrectionFile.h" // Provides CheckSetupData()

#include "TAnalysedPulseMapWrapper.h"

// Forward declaration of functions ======================
Int_t Main_event_loop(TTree* dataTree,ARGUMENTS& arguments);
void ClearGlobalData(TGlobalData*);
TTree* GetTree(TFile* inFile, const char* t_name);
Int_t PrepareAnalysedPulseMap(TFile* fileOut);
Int_t PrepareSingletonObjects(const ARGUMENTS&);

static TGlobalData *g_event=NULL;
static TFile *gInFile=NULL;

// Temporary botch to let ExportPulse module know what the current entry
// number is.  I'm assuming Phill's Event Navigator will provide this
// functionality, so I'll remove this at that point.
Long64_t* gEntryNumber;
Long64_t* gTotalEntries;

TAnalysedPulseMapWrapper *gAnalysedPulseMapWrapper=NULL;
static TTree *gAnalysedPulseTree = NULL;
TBranch *gAnalysedPulseBranch = NULL;

StringAnalPulseMap gAnalysedPulseMap;
StringDetPulseMap gDetectorPulseMap;
MuonEventList gMuonEvents;

TGlobalData* TGlobalData::Instance()
{
  return g_event;
}

int main(int argc, char **argv){
//load_config_file("MODULES.txt");

  // Parse the command line
  ARGUMENTS arguments;
  int ret = analyze_command_line (argc, argv,arguments);
  if(ret!=0) return ret;
  printf("Starting event");

  // Open the input tree file
  gInFile = new TFile(arguments.infile);
  if(!gInFile->IsOpen()) {
    printf("Failed to open input file, '%s'.  Exiting.\n",arguments.infile);
    delete gInFile;
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
  //CheckSetupData(s_data, arguments.correction_file);

  //Event Tree
  TTree* eventTree = GetTree(gInFile,"EventTree");
  if(!eventTree) return 1;
  eventTree->SetBranchAddress("Event",&g_event);

  // Let's open the output file for analysis data, histograms and so on.
  TFile *fileOut = new TFile(arguments.outfile, "RECREATE");
  if(!fileOut->IsOpen()){
    printf("Could not open ROOT output file %s\n", arguments.outfile);
    return 1;
  }
  fileOut->cd();

  // Now let's setup all the analysis modules we want
  // NOTE: This has to be done after the output file was opened else the
  // modules wont have a directory to store things to.
  ret= modules::navigator::Instance()->LoadConfigFile(arguments.mod_file);
  if(ret!=0) {
     printf("Problem setting up analysis modules.\n");
     return ret;
  }
  
  // Setup the analysed pulse map to store / read in the pulses
  ret = PrepareAnalysedPulseMap(fileOut);
  if(ret!=0) {
     printf("Problem preparing analysed pulse map.");
     return ret;
  }

  // Finally let's do the main loop
  fileOut->cd();
  Main_event_loop(eventTree,arguments);

  // and finish up
  fileOut->cd();
  gAnalysedPulseTree->Write();
  fileOut->Write();
  fileOut->Close();
  gInFile->Close();

  return 0;
}

Int_t Main_event_loop(TTree* dataTree,ARGUMENTS& arguments){
/*************************************************************************\
| Loop over tree entries and call histogramming modules.                  |
\*************************************************************************/
  Long64_t nentries = dataTree->GetEntriesFast();
  printf("There are %d entries\n",(int)nentries);
  std::cout<<"Processing file, which may take a while. "
     "Have patience young padawan.."<<std::endl;

  //set up the input data
  if (g_event){
    g_event->Clear("C");
    dataTree->SetBranchAddress("Event",&g_event);
  }

  // How many entries should we loop over?
  Long64_t start = 0;
  Long64_t stop = nentries;
  if(arguments.stop>0){
    if(arguments.start>0)
      start = (Long64_t)(arguments.start-1);
    if((Long64_t)arguments.stop < nentries)
      stop = (Long64_t)arguments.stop;
  }
  else if((Long64_t)arguments.start < nentries && arguments.start > 0){
    stop = (Long64_t)arguments.start;
  }
  gTotalEntries=&stop;
  // wind the file on to the first event
  dataTree->GetEntry(start);

  // Get the first and last module to run with
  modules::list::iterator first_module = modules::navigator::Instance()->Begin();
  modules::list::iterator last_module = modules::navigator::Instance()->End();
  modules::list::iterator it_mod;

  int q = 0;
  for (it_mod=first_module; it_mod != last_module; it_mod++) {
    q |= it_mod->second->BeforeFirstEntry(g_event, TSetupData::Instance());
  }
  if(q) {
    printf("Error while preprocessing first entry (%d)\n",(Int_t)start);
    return q;
  }

  q = 0;
  //process entries
  Long64_t jentry;
  gEntryNumber=&jentry;
  for ( jentry=start; jentry<stop;jentry++) {
    if(g_event){
      g_event->Clear("C");
      ClearGlobalData(g_event);
      dataTree->SetBranchAddress("Event",&g_event);
    }
    
    if (jentry%100 == 0) {
      printf("Completed %lld events out of %lld\n", jentry, stop);
    }
    // Let's get the next event
    dataTree->GetEntry(jentry);

    for (it_mod=first_module; it_mod != last_module; it_mod++) {
      q |= it_mod->second->ProcessGenericEntry(g_event,TSetupData::Instance());
      //if(q) break;
    }
    if(q){
      printf("q was non-zero when jentry was %lld\n",jentry);
      break;
    }

    gAnalysedPulseMapWrapper->SetMap(gAnalysedPulseMap);
    //gAnalysedPulseMapWrapper->ShowInfo();
    gAnalysedPulseTree->Fill();
  }

  //post-process on last entry
  q = 0;
  for (it_mod=first_module; it_mod != last_module; it_mod++) {
    q |= it_mod->second->AfterLastEntry(g_event,TSetupData::Instance());
  }
  if (q) {
     printf("Error during post-processing last entry (%lld)\n",(stop-1));
      return q;
  }
  
  printf("Finished processing data normally\n");
  return 0;
}

void ClearGlobalData(TGlobalData* data)
{
  // We could put this into TGlobalData::Clear(), but we need
  // to be sure that's okay at the alcapana level. That is, if
  // Clear() deletes the TPulseIsland objects, but g_event doesn't
  // own the pulses, they would be deleted later. A solution is to
  // be sure that TGlobalData isn't called in alcapana, or ensure
  // that g_event owns the pulse islands at that level.
  StringPulseIslandMap::iterator mapIter;
  StringPulseIslandMap::iterator mapEnd = data->fPulseIslandToChannelMap.end();
  for(mapIter = data->fPulseIslandToChannelMap.begin(); mapIter != mapEnd; mapIter++) {
    // The iterator is pointing to a pair<string, vector<TPulseIsland*> >
    std::vector<TPulseIsland*> pulse_vector= mapIter->second;
    for(size_t i=0; i<pulse_vector.size(); i++){
      delete pulse_vector[i];
      pulse_vector[i] = NULL;
    }
    pulse_vector.clear();
  }


  for(StringAnalPulseMap::iterator mapIter=gAnalysedPulseMap.begin();
     mapIter != gAnalysedPulseMap.end(); mapIter++) {

    // The iterator is pointing to a pair<string, vector<TPulseIsland*> >
    AnalysedPulseList pulse_vector= mapIter->second;
    for(size_t i=0; i<pulse_vector.size(); i++){
      delete pulse_vector[i];
      pulse_vector[i] = NULL;
    }
    pulse_vector.clear();
  }
  gAnalysedPulseMap.clear();

  for(StringDetPulseMap::iterator mapIter = gDetectorPulseMap.begin(); mapIter != gDetectorPulseMap.end(); mapIter++) {
    // The iterator is pointing to a pair<string, vector<TPulseIsland*> >
    std::vector<TDetectorPulse*> pulse_vector= mapIter->second;
    for(size_t i=0; i<pulse_vector.size(); i++){
      delete pulse_vector[i];
      pulse_vector[i] = NULL;
    }
    pulse_vector.clear();
  }
  gDetectorPulseMap.clear();
}

TTree* GetTree(TFile* inFile, const char* t_name)
{
   TTree* InfoTree = (TTree *)inFile->Get(t_name);
   if(!InfoTree) {
      printf("Unable to find TTree '%s' in %s.\n",t_name,inFile->GetName());
      return NULL;
   }
   InfoTree->Print();

   return InfoTree;
}

TSetupData* TSetupData::Instance()
{
  static TSetupData *s_data=NULL;

  // if s_data is already setup then we can just return it immediately
  if(s_data) return s_data;

  // Check we have a valid tree for the setup data
  TTree* setupTree= GetTree(gInFile, "SetupTree");
  if(!setupTree) return NULL;

  // Hook up the branch in the file to s_data and load in the data
  setupTree->SetBranchAddress("Setup",&s_data);
  if(!s_data) {
     printf("Unable to find TSetupData branch 'Setup' in the file");
     return NULL;
  }
  setupTree->GetEntry(0);

  return s_data;
}

void PrintSetupData(TSetupData* s_data){
     if(!s_data) return;
  // print things out
  std::map<std::string, std::string>::iterator it_info;
  printf("### TSetupData ###\n");
  printf("Bank  Detector Name  Clock Tick (ns)  Pedestal (ADC)  Trigger Polarity  Time Shift (ns)  No. of Bits\n");
  printf("----  -------------  ---------------  --------------  ----------------  ---------------  -----------\n");
  for (it_info=s_data->fBankToDetectorMap.begin();it_info!=s_data->fBankToDetectorMap.end();++it_info){
		 std::string bankname = it_info->first;
		 std::string detname = s_data->GetDetectorName(bankname);
		 float clockTick = s_data->GetClockTick(bankname);
		 int pedestal = s_data->GetPedestal(bankname);
		 int trig_pol = s_data->GetTriggerPolarity(bankname);
		 double time_shift = s_data->GetTimeShift(bankname);
		 int n_bits = s_data->GetNBits(bankname);
		 printf("%s  %s\t\t%f\t%d\t\t%d\t\t%f\t\t%d\n",bankname.c_str(), detname.c_str(), clockTick, pedestal, trig_pol, time_shift,n_bits);
  }
}

Int_t PrepareAnalysedPulseMap(TFile* fileOut){
	// TAnalysedMapWrapper
   gAnalysedPulseMapWrapper = new TAnalysedPulseMapWrapper(gAnalysedPulseMap);

   int split = 1;
   int bufsize = 64000;
   Int_t branchstyle = 1;

   if (split < 0) {branchstyle = 0; split = -1-split;}
   TTree::SetBranchStyle(branchstyle);

   gAnalysedPulseTree = new TTree("AnalysedPulseTree", "AnalysedPulseTree");
   gAnalysedPulseTree->SetAutoSave(100000000);
   gAnalysedPulseTree->SetMaxVirtualSize(100000000);
   gAnalysedPulseBranch = gAnalysedPulseTree->Branch("AnalysedPulseMapWrapper", 
                                                     "TAnalysedPulseMapWrapper", &gAnalysedPulseMapWrapper, bufsize, split);
   gAnalysedPulseBranch->SetAutoDelete(kFALSE);
   return 0;
}

Int_t PrepareSingletonObjects(const ARGUMENTS&){
   // Set up the modules navigator
   if(!modules::navigator::Instance()) return 1;

   // Make sure the instance of TSetupData get's loaded 
   // (though in practice we can do this lazily)
   //TSetupData* s_data = TSetupData::Instance();
   if(!TSetupData::Instance()) return 2;

   return 0;
}
