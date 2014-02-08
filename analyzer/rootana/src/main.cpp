//#define USE_PRINT_OUT 

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <ctype.h>
#include <map>

#include "utils.h" // Provides analyze_command_line()

// Modules list
#include "FillHistBase.h"
#include "AnalysePulseIsland.h"
#include "CheckCoincidence.h"
#include "MakeMuonEvents.h"
#include "CreateDetectorPulse.h"
#include "PlotAmplitude.h"
#include "PlotTime.h"
#include "CoincidenceCut.h"
#include "EvdE.h"
#include "PlotAmpVsTDiff.h"

#include "TTree.h"
#include "TBranch.h"
#include "TFile.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "ProcessCorrectionFile.h" // Provides CheckSetupData()

#include "TAnalysedPulseMapWrapper.h"

// Forward declaration of functions ======================
Int_t Main_event_loop(TTree* dataTree);
void ClearGlobalData(TGlobalData*);
TTree* GetTree(TFile* inFile, const char* t_name);
Int_t PrepareAnalysedPulseMap(TFile* fileOut);
Int_t PrepareModules();

static TGlobalData *g_event=NULL;
static TFile *gInFile=NULL;

TAnalysedPulseMapWrapper *gAnalysedPulseMapWrapper=NULL;
static TTree *gAnalysedPulseTree = NULL;
TBranch *gAnalysedPulseBranch = NULL;

std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;
std::map<std::string, std::vector<TDetectorPulse*> > gDetectorPulseMap;

static int n_fillhist = 0;  // keeps track of the total number of modules
static FillHistBase **fillhists;

TGlobalData* TGlobalData::Instance()
{
  return g_event;
}

int main(int argc, char **argv){
  ARGUMENTS arguments;
  int ret = analyze_command_line (argc, argv,arguments);
  if(ret!=0) return ret;
  
  gInFile = new TFile(arguments.infile);
  if(!gInFile->IsOpen()) {
    printf("Failed to open input file, '%s'.  Exiting.\n",arguments.infile);
    delete gInFile;
    return 1;
  }

  // Make sure the instance of TSetupData get's loaded 
  // (though in practice we can do this lazily)
  TSetupData* s_data = TSetupData::Instance();

  // Now that we've loaded the TSetupData for this run check if there are any
  // suggested replacements for the wiremap data
  CheckSetupData(s_data, arguments.correction_file);

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

  // Setup the analysed pulse map to store / read in the pulses
  ret = PrepareAnalysedPulseMap(fileOut);
  if(ret!=0) {
     printf("Problem preparing analysed pulse map.");
     return ret;
  }

  // Now let's setup all the analysis modules we want
  ret= PrepareModules();
  if(ret!=0) {
     printf("Problem setting up analysis modules.");
     return ret;
  }
  
  // Finally let's do the main loop
  fileOut->cd();
  Main_event_loop(eventTree);

  // and finish up
  gAnalysedPulseTree->Write();
  fileOut->Write();
  fileOut->Close();
  gInFile->Close();

  return 0;
}

Int_t Main_event_loop(TTree* dataTree,ARGUMENTS arguments){
/*************************************************************************\
| Loop over tree entries and call histogramming modules.                  |
\*************************************************************************/
  Long64_t nentries = dataTree->GetEntriesFast();
  printf("There are %d entries\n",(int)nentries);
  std::cout<<"Processing file, which may take a while. "
     "Have patience young padawan.."<<std::endl;

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
  
  //preprocess first event
  if (g_event){
    g_event->Clear("C");
    dataTree->SetBranchAddress("Event",&g_event);
  }

  dataTree->GetEntry(start);
  int q = 0;
  for (int i=0; i < n_fillhist; i++) {
    q |= fillhists[i]->BeforeFirstEntry(g_event);
    //if (q) break;
    // q = fillhists[i]->ProcessGenericEntry(g_event);
    //if (q) break;
  }
  if(q) {
    printf("Error while preprocessing first entry (%d)\n",(Int_t)start);
    return q;
  }

  q = 0;
  //process entries
  for (Long64_t jentry=start; jentry<stop;jentry++) {
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

    for(int i=0; i < n_fillhist; i++) {
      //printf("processing fillhists[%d]\n",i);      
      PrintOut(i<<": Now processing "<<fillhists[i]->GetName()<<std::endl);
      q |= fillhists[i]->ProcessGenericEntry(g_event,TSetupData::Instance());
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
  for(int i=0; i < n_fillhist; i++) {
    q |= fillhists[i]->AfterLastEntry(g_event);
  }
  if (q) {
     printf("Error during post-processing last entry (%lld)\n",(stop-1));
  return q;
  }
  
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
  typedef std::map<std::string, std::vector<TPulseIsland*> > PulseMap;
  typedef PulseMap::iterator PulseMapIt;
  PulseMapIt mapIter;
  PulseMapIt mapEnd = data->fPulseIslandToChannelMap.end();
  for(mapIter = data->fPulseIslandToChannelMap.begin(); mapIter != mapEnd; mapIter++) {
    // The iterator is pointing to a pair<string, vector<TPulseIsland*> >
    std::vector<TPulseIsland*> pulse_vector= mapIter->second;
    for(size_t i=0; i<pulse_vector.size(); i++){
      delete pulse_vector[i];
      pulse_vector[i] = NULL;
    }
    pulse_vector.clear();
  }


  for(std::map<std::string,
     std::vector<TAnalysedPulse*> >::iterator mapIter=gAnalysedPulseMap.begin();
     mapIter != gAnalysedPulseMap.end(); mapIter++) {

    // The iterator is pointing to a pair<string, vector<TPulseIsland*> >
    std::vector<TAnalysedPulse*> pulse_vector= mapIter->second;
    for(size_t i=0; i<pulse_vector.size(); i++){
      delete pulse_vector[i];
      pulse_vector[i] = NULL;
    }
    pulse_vector.clear();
  }
  gAnalysedPulseMap.clear();

  for(std::map<std::string, std::vector<TDetectorPulse*> >::iterator mapIter = gDetectorPulseMap.begin(); mapIter != gDetectorPulseMap.end(); mapIter++) {
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
  TTree* setupTree= GetTree(gInFile, "Setup");
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

Int_t PrepareModules(){

  fillhists = new FillHistBase *[50]; // increase if more than 20 modules
  n_fillhist = 0;  // number of modules (global variable)
	// this is needed to save analysed tree
  fillhists[n_fillhist++] = new AnalysePulseIsland("AnalysedPulseIsland");

	//fillhists[n_fillhist++] = new PlotAmplitude("PlotAmplitude");
	//fillhists[n_fillhist++] = new PlotTime("PlotTime");
	
	// EvdE
	//char foldername[256];
	//double t0 = 1000;
	//double t1 = 6000;
	//sprintf(foldername,"%s_%d_%d","EvdE",int(t0),int(t1));
	//fillhists[n_fillhist++] = new EvdE(foldername, t0,t1);
	

	// Amplitude, Xray
  //fillhists[n_fillhist++] = new 
		//CoincidenceCut("CoincidenceCut_MuSc-GeF_500ns", "muSc","Ge-F", -500,500);
  //fillhists[n_fillhist++] = new 
		//CoincidenceCut("CoincidenceCut_MuSc-GeS_500ns", "muSc","Ge-S", -500,500);
  //fillhists[n_fillhist++] = new PlotAmplitude("PlotAmplitude_500nsCut");
  return 0;
}
