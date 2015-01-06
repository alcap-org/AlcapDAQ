//#define USE_PRINT_OUT 

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <ctype.h>
#include <map>
#include <string>
#include "utils.h"

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
#include "PlotNEnergy.h"
#include "ScintInt.h"
#include "NGammaInt.h"
#include "NGammaInt2.h"
#include "NGammaSmInt.h"
#include "NGammaLanInt.h"
#include "NGIntTCut.h"
#include "NGIntTCut2.h"
#include "TestSmoothing.h"
#include "TestFit.h"
#include "TestTiming.h"
#include "TestVeto.h"
#include "TestFGA.h"
#include "TestVetoMuscCoinc.h"
#include "TestVetoMuscTiming.h"

#include "TTree.h"
#include "TBranch.h"
#include "TFile.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "ProcessCorrectionFile.h"



void help_command_line(char *my_name);
bool isNumber(char *c);
int check_arguments();
int analyze_command_line (int argc, char **argv);
void *root_event_loop(void *arg = NULL);
void ClearGlobalData(TGlobalData*);
int GetRunNumber(char* input_file);

ARGUMENTS arguments = {"","","",0,0,-1};

static TTree *tree = NULL;
static TTree *InfoTree = NULL;
static TBranch *br = NULL;
static TBranch *InfoBr = NULL;
static TGlobalData *g_event;
static TSetupData *s_data;
static char correction_file[256];

std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;
std::map<std::string, std::vector<TDetectorPulse*> > gDetectorPulseMap;

static int n_fillhist = 0;  // keeps track of the total number of modules
static FillHistBase **fillhists;

TSetupData* TSetupData::Instance()
{
  return s_data;
}

TGlobalData* TGlobalData::Instance()
{
  return g_event;
}

int main(int argc, char **argv){
  int ret = analyze_command_line (argc, argv);
  if(!ret) return 1;
  
  ret = check_arguments();
  if(!ret) return 1;
  
  char *infilename = arguments.infile;
  char *outfilename = arguments.outfile;
  
  TFile *treefile = new TFile(infilename);
  if(!treefile->IsOpen()) {
    printf("Failed to open file.  Exiting.\n");
    delete treefile;
    return 1;
  }

  //Info Tree  
  InfoTree = (TTree *)treefile->Get("SetupTree");
  if(!InfoTree) {
    printf("Could not find InfoTree. Exiting.\n");
    treefile->Close();
    return 1;
  }
  InfoTree->Print();
  s_data = 0;
  InfoBr = InfoTree->GetBranch("Setup");
  InfoBr->SetAddress(&s_data);
  InfoTree->GetEntry(0);
  // Now that we've loaded the TSetupData for this run check if there are any
  // suggested replacemets for the wiremap data
  CheckSetupData(s_data, correction_file);
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

  //Event Tree
  tree = (TTree *)treefile->Get("EventTree");
  if(!tree) {
    printf("Could not find EventTree. Exiting.\n");
    treefile->Close();
    return 1;
  }
  g_event = 0; // initialization to zero is important
  br = tree->GetBranch("Event");
  br->SetAddress(&g_event);

  // Let's open the output file for histograms etc.
  TFile *fileOut = new TFile(outfilename, "RECREATE");
  if(!fileOut->IsOpen()){
    printf("Could not open ROOT output file %s\n", outfilename);
    return 1;
  }
  fileOut->cd();

  // Now let's setup all the analysis modules we want
  fillhists = new FillHistBase *[50]; // increase if more than 20 modules
  n_fillhist = 0;  // number of modules (global variable)
  fillhists[n_fillhist++] = new AnalysePulseIsland("AnalysePulseIsland");
  fillhists[n_fillhist++] = new PlotAmplitude("PlotAmplitude");
  //fillhists[n_fillhist++] = new PlotNEnergy("PlotNEnergy");
  //fillhists[n_fillhist++] = new PlotTime("PlotTime");
  //fillhists[n_fillhist++] = new TestSmoothing("Smoothing Tests");
  //fillhists[n_fillhist++] = new TestVeto("Veto Tests");
  //fillhists[n_fillhist++] = new TestVetoMuscCoinc("Veto Coinc Tests");
  //fillhists[n_fillhist++] = new TestVetoMuscTiming("Veto Timing Tests");
  //fillhists[n_fillhist++] = new TestFGA("FGA Tests");
  //fillhists[n_fillhist++] = new TestTiming("Timing Tests");


  fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_Ge-S_Ge-F", "Ge-S","Ge-F");
  //fillhists[n_fillhist++] = new ScintInt("Neutron Integrals");
  fillhists[n_fillhist++] = new NGammaInt("PSD");
  //fillhists[n_fillhist++] = new NGammaInt2("PSD_2");
  //fillhists[n_fillhist++] = new NGammaSmInt("Smoothed PSD");
  //fillhists[n_fillhist++] = new NGammaLanInt("Landau Fitted PSD");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_SiL1-1_SiL2_slow", "SiL1-1-S","SiL2-S");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_SiL1-2_SiL2_slow", "SiL1-2-S","SiL2-S");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_SiL1-3_SiL2_slow", "SiL1-3-S","SiL2-S");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_SiL1-4_SiL2_slow", "SiL1-4-S","SiL2-S");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_SiL1-1_SiL2_fast", "SiL1-1-F","SiL2-F");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_SiL1-2_SiL2_fast", "SiL1-2-F","SiL2-F");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_SiL1-3_SiL2_fast", "SiL1-3-F","SiL2-F");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_SiL1-4_SiL2_fast", "SiL1-4-F","SiL2-F");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_SiR1-1_SiR2_slow", "SiR1-1-S","SiR2-S");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_SiR1-2_SiR2_slow", "SiR1-2-S","SiR2-S");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_SiR1-3_SiR2_slow", "SiR1-3-S","SiR2-S");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_SiR1-4_SiR2_slow", "SiR1-4-S","SiR2-S");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_SiR1-1_SiR2_fast", "SiR1-1-F","SiR2-F");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_SiR1-2_SiR2_fast", "SiR1-2-F","SiR2-F");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_SiR1-3_SiR2_fast", "SiR1-3-F","SiR2-F");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_SiR1-4_SiR2_fast", "SiR1-4-F","SiR2-F");

  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_Ge-S", "muSc","Ge-S");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_Ge-F", "muSc","Ge-F");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiL1-1-S", "muSc","SiL1-1-S");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiL1-2-S", "muSc","SiL1-2-S");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiL1-3-S", "muSc","SiL1-3-S");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiL1-4-S", "muSc","SiL1-4-S");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiL1-1-F", "muSc","SiL1-1-F");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiL1-2-F", "muSc","SiL1-2-F");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiL1-3-F", "muSc","SiL1-3-F");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiL1-4-F", "muSc","SiL1-4-F");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiL2S", "muSc","SiL2-S");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiL2F", "muSc","SiL2-F");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiR1-1-S", "muSc","SiR1-1-S");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiR1-2-S", "muSc","SiR1-2-S");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiR1-3-S", "muSc","SiR1-3-S");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiR1-4-S", "muSc","SiR1-4-S");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiR1-1-F", "muSc","SiR1-1-F");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiR1-2-F", "muSc","SiR1-2-F");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiR1-3-F", "muSc","SiR1-3-F");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiR1-4-F", "muSc","SiR1-4-F");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiR2S", "muSc","SiR2-S");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_SiR2F", "muSc","SiR2-F");
  fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_NDet", "muSc","NDet");
  fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_NDet2", "muSc","NDet2");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_MuSc_ScVe", "muSc","ScVe");
  //fillhists[n_fillhist++] = new PlotAmpVsTDiff("PlotAmpVsTDiff_ScVe_MuSc", "ScVe","muSc");
  //fillhists[n_fillhist++] = new CoincidenceCut("CoincidenceCut_MuSc-GeF", "muSc","Ge-F", -100,100);
  //fillhists[n_fillhist++] = new CoincidenceCut("CoincidenceCut_MuSc-NDet", "muSc","NDet", 400,1000);
  //fillhists[n_fillhist++] = new CoincidenceCut("CoincidenceCut_MuSc-NDet2", "muSc","NDet2", 400,1000);
  fillhists[n_fillhist++] = new NGIntTCut("PSD_PromptTCut", "muSc", -40, 40);
  fillhists[n_fillhist++] = new NGIntTCut("PSD_PbTCut", "muSc", 40, 400);
  fillhists[n_fillhist++] = new NGIntTCut("PSD_AlTCut", "muSc", 400, 4000);
  fillhists[n_fillhist++] = new NGIntTCut("PSD_WideTCut", "muSc", -1000, 8000);
  //fillhists[n_fillhist++] = new NGIntTCut2("PSD AfterCut_2", "muSc", 300, 1700);
  //fillhists[n_fillhist++] = new PlotAmplitude("PlotAmplitude_AfterCut");
  //fillhists[n_fillhist++] = new MakeMuonEvents("MakeMuonEvents",s_data);
  //fillhists[n_fillhist++] = new EvdE("EvdE");
  //fillhists[n_fillhist++] = new CreateDetectorPulse("CreateDetectorPulse");

  fileOut->cd();
  
  // do the main loop
  root_event_loop();

  fileOut->Write();
  fileOut->Close();
  treefile->Close();

  return 0;
}

void *root_event_loop(void *arg){
/*************************************************************************\
| Loop over tree entries and call histogramming modules.                  |
\*************************************************************************/
  //printf("in the root_event_loop\n");
  Long64_t nentries = tree->GetEntriesFast();
  printf("There are %d entries\n",(int)nentries);
  std::cout<<"Processing file, which may take a while.  Have patience young padwan.."<<std::endl;

  Int_t nbytes = 0, nb = 0;

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
    br->SetAddress(&g_event);
  }
  
  nb = tree->GetEntry(start);
  int q = 0;
  for (int i=0; i < n_fillhist; i++) {
    q |= fillhists[i]->BeforeFirstEntry(g_event);
    //if (q) break;
    // q = fillhists[i]->ProcessGenericEntry(g_event);
    //if (q) break;
  }
  if(q) {
    printf("q was non-zero when preprocessing first entry (%d)\n",
	   (Int_t)start);
    return NULL;
  }

  q = 0;
  //process entries
  for (Long64_t jentry=start; jentry<stop;jentry++) {
    if(g_event){
      g_event->Clear("C");
      ClearGlobalData(g_event);
      br->SetAddress(&g_event);
    }
    
    if (jentry%100 == 0) {
      printf("Completed %d events out of %d \n", jentry, stop);
    }
    // Let's get the next event
    nb = tree->GetEntry(jentry);

    for(int i=0; i < n_fillhist; i++) {
      //printf("processing fillhists[%d]\n",i);      
      PrintOut(i<<": Now processing "<<fillhists[i]->GetName()<<std::endl);
      q |= fillhists[i]->ProcessGenericEntry(g_event,s_data);
      //if(q) break;
    }
    if(q){
      printf("q was non-zero when jentry was %d\n",(Int_t)jentry);
      break;
    }
  }

  //post-process on last entry
  q = 0;
  for(int i=0; i < n_fillhist; i++) {
    q |= fillhists[i]->AfterLastEntry(g_event);

  }
  if (q) printf("q was non-zero during post-processing last entry (%d)\n",
		(Int_t)(stop-1));
  
  return NULL;
}

void help_command_line(char *my_name){
  fprintf (stderr,"\nusage: %s  [flags] \n\n",my_name);
  fprintf (stderr,"valid options are:\n");
  fprintf (stderr,"  -i <filename>\t\t Input root tree file.\n");
  fprintf (stderr,"  -o <filename>\t\t Output root tree file.\n");  
  fprintf (stderr,"  -n <count>\t\t Analyze only <count> events.\n");
  fprintf (stderr,"     <first> <last>\t Analyze only events from "
	   "<first> to <last>.\n");
  fprintf (stderr,"  -r <PSI run number>\t\t Run number specification for the shrubs.\n");
  fprintf (stderr,"\n");
  return;
}

bool isNumber(char *c){
  for(int i=0; i<strlen(c); i++){
    if(!isdigit(c[i])) return false;
  }
  return true;
}

int check_arguments(){
  if(arguments.stop >0){
    if(arguments.stop <= arguments.start){
      printf("ERROR: Cannot process events from %d to %d because %d<=%d\n",
	     arguments.start, arguments.stop, arguments.stop, arguments.start);
      return 0;
    }
  }

  if(strcmp(arguments.infile,"")==0){
    printf("ERROR: Empty input file name. Did you specify the -i option?\n");
    return 0;
  }  
  if(strcmp(arguments.outfile,"")==0){
    printf("ERROR: Empty output file name. Did you specify the -o option?\n");
    return 0;
  }  
  if(arguments.run==-1){
          // No run number has been set, obtain it from the filename
          arguments.run=GetRunNumber(arguments.infile);
  }
  if(strcmp(arguments.correction_file,"")==0){
    sprintf(correction_file,"wiremap_corrections/correct%ld.dat",arguments.run);
  }else sprintf(correction_file,arguments.correction_file);
  return 1;
}

int analyze_command_line (int argc, char **argv){
  if(argc==1) goto usage;
  if(strcmp(argv[1],"--help")==0) goto usage;
  
  for(int i=1; i<argc; /* incrementing of i done in the loop! */){
    if(argv[i][0] != '-'){
      printf("ERROR: Wrong argument %s\n",argv[i]);
      goto usage;
    }
    else{
      if(strlen(&argv[i][1]) == 1){
	switch(argv[i][1]){
	case 'i':
	  if(i+1 < argc){
	    arguments.infile = argv[i+1];
	    i+=2;
	  }
	  else{
	    printf("ERROR: No argument for input file specified\n");
	    goto usage;
	  }
	  break;
	case 'o':
	  if(i+1 < argc){
	    arguments.outfile = argv[i+1];
	    i+=2;
	  }
	  else{
	    printf("ERROR: No argument for input file specified\n");
	    goto usage;
	  }
	  break;
        case 's':
	  if(i+1 < argc){
	    arguments.correction_file = argv[i+1];
	    i+=2;
	  }
	  else{
	    printf("ERROR: No argument for TSetupData correction file specified\n");
	    goto usage;
	  }
	  break;
	case 'n':
	  if(i+1 < argc){
	    if(isNumber(argv[i+1])){
	      arguments.start = atoi(argv[i+1]);
	      i+=2;
	    }
	    else{
	      printf("ERROR: Argument %s for option -n is not a number\n",argv[i+1]);
	      goto usage;
	    }
	    if(i < argc && argv[i][0]!='-'){
	      if(isNumber(argv[i])){
		arguments.stop = atoi(argv[i]);
		i+=1;
	      }
	      else{
		printf("ERROR: Argument %s for option -n is not a number\n", argv[i]);
		goto usage;
	      }
	    }
	  }
	  else{
	    printf("ERROR: No argument for number of processed events specified\n");
	    goto usage;
	  }
	  break;
	case 'r':
	  if(i+1 < argc){
	    if(isNumber(argv[i+1])){
	      arguments.run = atoi(argv[i+1]);
	      i+=2;
	    }
	    else{
	      printf("ERROR: Argument %s for option -r is not a number\n",argv[i+1]);
	      goto usage;
	    }
	  }
	  break;
	default:
	  printf("ERROR: Argument %s not recognized\n",argv[i]);
	  goto usage;
	}
      }
      else{
	
      }
    }
  }

  return 1;

 usage:
  help_command_line(argv[0]);
  
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
    for(int i=0; i<pulse_vector.size(); i++){
      delete pulse_vector[i];
      pulse_vector[i] = NULL;
    }
    pulse_vector.clear();
  }


  for(std::map<std::string, std::vector<TAnalysedPulse*> >::iterator mapIter = gAnalysedPulseMap.begin(); mapIter != gAnalysedPulseMap.end(); mapIter++) {

    // The iterator is pointing to a pair<string, vector<TPulseIsland*> >
    std::vector<TAnalysedPulse*> pulse_vector= mapIter->second;
    for(int i=0; i<pulse_vector.size(); i++){
      delete pulse_vector[i];
      pulse_vector[i] = NULL;
    }
    pulse_vector.clear();
  }
  gAnalysedPulseMap.clear();

  for(std::map<std::string, std::vector<TDetectorPulse*> >::iterator mapIter = gDetectorPulseMap.begin(); mapIter != gDetectorPulseMap.end(); mapIter++) {
    // The iterator is pointing to a pair<string, vector<TPulseIsland*> >
    std::vector<TDetectorPulse*> pulse_vector= mapIter->second;
    for(int i=0; i<pulse_vector.size(); i++){
      delete pulse_vector[i];
      pulse_vector[i] = NULL;
    }
    pulse_vector.clear();
  }
  gDetectorPulseMap.clear();
}

int GetRunNumber(char* input_file){
        std::string run_number=input_file;
        std::cout<<run_number<<std::endl;
        //remove the extension
        run_number=run_number.substr(0,run_number.find_last_of('.'));
        std::cout<<run_number<<std::endl;
        //find the last numbers
        run_number=run_number.substr(run_number.find_last_not_of("01234566789")+1);
        std::cout<<run_number<<std::endl;
        std::cout<<atoi(run_number.c_str())<<std::endl;
        return atoi(run_number.c_str());
}
