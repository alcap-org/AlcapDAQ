#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <map>
#include "utils.h"

#include "FillHistBase.h"
#include "SimpleHistograms.h"
#include "MyModule.h"

#include "TTree.h"
#include "TBranch.h"
#include "TFile.h"
#include "TGlobalData.h"
#include "TSetupData.h"

void help_command_line(char *my_name);
bool isNumber(char *c);
int check_arguments();
int analyze_command_line (int argc, char **argv);
void *root_event_loop(void *arg = NULL);

ARGUMENTS arguments = {"","",0,0,-1};

static TTree *tree = NULL;
static TTree *InfoTree = NULL;
static TBranch *br = NULL;
static TBranch *InfoBr = NULL;
static TGlobalData *g_event;
static TSetupData *s_data;

static int n_fillhist = 0;  // keeps track of the total number of modules
static FillHistBase **fillhists;

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
  InfoTree->Print();
  if(!InfoTree) {
    printf("Could not find InfoTree. Exiting.\n");
    treefile->Close();
    return 1;
  }
  s_data = 0;
  InfoBr = InfoTree->GetBranch("Setup");
  InfoBr->SetAddress(&s_data);
  InfoTree->GetEntry(0);
  std::map<std::string, std::string>::iterator it_info;
  for (it_info=s_data->fBankToDetectorMap.begin();it_info!=s_data->fBankToDetectorMap.end();++it_info){
  printf("%s => %s\n",it_info->first.c_str(),it_info->second.c_str());
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
  fillhists = new FillHistBase *[20]; // increase if more than 20 modules
  n_fillhist = 0;  // number of modules (global variable)
  fillhists[n_fillhist++] = new SimpleHistograms("SimpleHistograms");
  fillhists[n_fillhist++] = new MyModule("MyModule");
  
  fileOut->cd();
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
  printf("there are %d entries\n",(int)nentries);

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

  for (Long64_t jentry=start; jentry<stop;jentry++) {
    if(g_event){
      g_event->Clear("C");
      br->SetAddress(&g_event);
    }
    
    // Let's get the next event
    nb = tree->GetEntry(jentry);

    int q = 0;
    
    for(int i=0; i < n_fillhist; i++) {
      //printf("processing fillhists[%d]\n",i);      
      q = fillhists[i]->ProcessGenericEntry(g_event);
      if(q) break;
    }
    if(q) printf("q was non-zero when jentry was %d\n",(Int_t)jentry);
    if(q) break;
  }

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
