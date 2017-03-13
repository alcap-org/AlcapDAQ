/*Standard includes */
#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <utility>
#include <cmath>
#include <algorithm>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TROOT.h>
#include <TPluginManager.h>
#include <TTree.h>
#include <TBranch.h>
#include<TFile.h>
#include <TDirectory.h>

/* AlCap includes */
#include "AlCap.h"
#include "TGlobalData.h"
#include "TSetupData.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
static INT MTCorrTree_init(void);
static INT MTCorrTree(EVENT_HEADER*, void*);
INT MTCorrTree_bor(int);
INT MTCorrTree_eor(int);
float MTCorrTree_Threshold(std::string);


extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using namespace AlCap;
namespace {
  std::string WFDBANKS[NCRATE][MAXNCHANWFD];
  std::string TDCBANKS[NCRATE][MAXNCHANWFD];

  TTree* vvtTCorrTree = NULL;
  TTree* vvtSetupTree = NULL;
  TBranch *vSetupBranch, *detBranch, *pedBranch, *eFitBranch, *eIntBranch, *PSDBranch, *tdiffBranch, *vetoBranch, *pileupBranch, *overflowBranch, *dPulseBranch;

  TH1D* vhTCorrTree_Count_TSc;
  TH1D* vhTCorrTree_TSc_Amp;
  const double TIME_LOW = -2e3, TIME_HIGH = 8e3;
  double TScCount;

  TFile *treeFile = NULL;
  char bank[5], det[10];
  std::string detName;
  double eFit, eInt, PSD_param, tdiff;
  float pedestal;
  bool veto, dPulse, pileup, overflow, threshold;

  char *treeFileName = "Analysis.root";
}


ANA_MODULE MTCorrTree_module =
{
  "MTCorrTree",        /* module name           */
  "Damien Alexander",  /* author                */
  MTCorrTree,          /* event routine         */
  MTCorrTree_bor,      /* BOR routine           */
  MTCorrTree_eor,      /* EOR routine           */
  MTCorrTree_init,     /* init routine          */
  NULL,                /* exit routine          */
  NULL,                /* parameter structure   */
  0,                   /* structure size        */
  NULL,                /* initial parameters    */
};

extern TROOT* gROOT;
static bool PLUGIN_LOADED = false;


INT MTCorrTree_init() {

  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("TCorrTree")->cd();
  //select channels and corresponding vetos
  char histname[64];   char histtitle[64]; 

  sprintf(histname, "hTCorrTest_TSc_Amp");
  sprintf(histtitle, "TSc Pulse Amplitudes");
  vhTCorrTree_TSc_Amp = new TH1D(histname, histtitle, 15000, 0, 15000);
  vhTCorrTree_TSc_Amp->GetXaxis()->SetTitle("TSc pulse Amplitudes");
  vhTCorrTree_TSc_Amp->GetYaxis()->SetTitle("Count");

  sprintf(histname, "hTCorrTest_Count_TSc");
  sprintf(histtitle, "Number of counts in TSc");
  vhTCorrTree_Count_TSc = new TH1D(histname, histtitle, 1455, 5975, 7430);
  vhTCorrTree_Count_TSc->GetXaxis()->SetTitle("RunNumber");
  vhTCorrTree_Count_TSc->GetYaxis()->SetTitle("Number of valid pulses");

  cwd->cd();
  ////          init Setup Tree        /////
  //not likely necessary, but just in case
  /*
  if (!PLUGIN_LOADED) {
    if(gROOT->GetPluginManager()->FindHandler("TVirtualStreamerInfo") == NULL)
      gROOT->GetPluginManager()->AddHandler("TVirtualStreamerInfo","*","TStreamerInfo","RIO","TStreamerInfo()");
    PLUGIN_LOADED = true;
  }
  
  //std::string filename = "Analysis";
  //sprintf(treeFileName, "%s", filename.c_str());
  if(treeFileName){
    treeFile = TFile::Open(treeFileName, "recreate");
    if(!treeFile || treeFile->IsZombie()){
      printf("Could not open tree file %s!\n", treeFileName);
      return ANA_SKIP;
    }  
  }
  treeFile->cd();
  */
  int split = 1;
  int bufsize = 64000;
  Int_t branchstyle = 1;

  if(split < 0) {branchstyle = 0; split = -1 - split;}
  TTree::SetBranchStyle(branchstyle);

  vvtSetupTree = new TTree("SetupTree", "All Setup information");
  vvtSetupTree->SetAutoSave(300000000);
  vvtSetupTree->SetMaxVirtualSize(300000000);

  vSetupBranch = vvtSetupTree->Branch("Setup", "TSetupData", &gSetup, bufsize, split);
  vSetupBranch->SetAutoDelete(kFALSE);

  vvtSetupTree->Fill();

  vvtTCorrTree = new TTree("vvtTCorrTree", "Reduced Data Tree");
  vvtTCorrTree->SetAutoSave(300000000);
  vvtTCorrTree->SetMaxVirtualSize(300000000);
  
  detBranch = vvtTCorrTree->Branch("DetName", &det, "det/C");
  pedBranch = vvtTCorrTree->Branch("Pedestal", &pedestal, "pedestal/F");
  eFitBranch = vvtTCorrTree->Branch("FitEnergy", &eFit, "eFit/D");
  eIntBranch = vvtTCorrTree->Branch("IntegralEnergy", &eInt, "eInt/D");
  PSDBranch = vvtTCorrTree->Branch("PSD", &PSD_param, "PSD_param/D");
  tdiffBranch = vvtTCorrTree->Branch("TimeDifference", &tdiff, "tdiff/D");
  vetoBranch = vvtTCorrTree->Branch("VetoFlag", &veto, "veto/O");
  pileupBranch = vvtTCorrTree->Branch("PileupFlag", &pileup, "pileup/O");
  overflowBranch = vvtTCorrTree->Branch("OverflowFlag", &overflow, "overflow/O");
  dPulseBranch = vvtTCorrTree->Branch("DoublePulseFlag", &dPulse, "dPulse/O");

  detBranch->SetAutoDelete(kFALSE);
  pedBranch->SetAutoDelete(kFALSE);
  eFitBranch->SetAutoDelete(kFALSE);
  eIntBranch->SetAutoDelete(kFALSE);
  PSDBranch->SetAutoDelete(kFALSE);
  tdiffBranch->SetAutoDelete(kFALSE);
  vetoBranch->SetAutoDelete(kFALSE);
  pileupBranch->SetAutoDelete(kFALSE);
  overflowBranch->SetAutoDelete(kFALSE);
  dPulseBranch->SetAutoDelete(kFALSE);




  //file is created and saved in eor

  for(int icrate = 0; icrate < NCRATE; ++icrate){
  
    for(int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      
      char bank[5]; sprintf(bank, "D%d%02d", icrate, ich);
      std::string detname = gSetup->GetDetectorName(bank);

      if(detname == "NdetD" || detname == "NdetU" /*|| det == "LaBr3"*/){
	WFDBANKS[icrate][ich] = bank;
	//TDCBANKS[icrate][ich] = gSetup->GetBankName("T" + det);
      }
      else if(detname == "GeCHEH" || detname == "GeCHEL"){
	WFDBANKS[icrate][ich] = bank;
	//TDCBANKS[icrate][ich] = gSetup->GetBankName("TGeCHT");
      }
      else if(detname == "TSc") {
	WFDBANKS[icrate][ich] = bank;
	//TDCBANKS[icrate][ich] = gSetup->GetBankName("TTSc");
      }
      else{
	WFDBANKS[icrate][ich] = " ";
	//TDCBANKS[icrate][ich] = "";
	continue;  // ignoring SYNC pulses, Rollover, LYSO, GeCHT, and empty
      }    
    }
  }
  cwd->cd();
  return SUCCESS;
}

INT MTCorrTree_eor(int run_number){
  int bin = vhTCorrTree_Count_TSc->FindBin(run_number);
  vhTCorrTree_Count_TSc->SetBinContent(bin, TScCount);


  vvtTCorrTree->Write();
  vvtSetupTree->Write();
  return 0;
}

INT MTCorrTree_bor(int run_number){
  TScCount = 0;
  return 0;
}

INT MTCorrTree(EVENT_HEADER *pheader, void *pevent) {
  const map< string, vector<TPulseIsland*> >& wfd_map = gData->fPulseIslandToChannelMap;

  std::string ref_bank = gSetup->GetBankName("TSc");
  if(!wfd_map.count(ref_bank)){
      printf("MTCorrTree: No reference hits TSc!\n");
      return SUCCESS;
  }
  const std::vector<TPulseIsland*>& ref_hits = wfd_map.at(ref_bank);

  for(int icrate = 0; icrate < NCRATE; ++icrate){
    for(int ich = 0; ich < NCHANWFD[icrate]; ++ich) {
      if(WFDBANKS[icrate][ich] == " ") continue;
      if(!wfd_map.count(WFDBANKS[icrate][ich])) continue;
      std::string detname=gSetup->GetDetectorName(WFDBANKS[icrate][ich]);
      //detName = detname;
      sprintf(det, "%s", detname.c_str());
      //det = (char[10])detname.c_str();
      if(detname != "NdetD" && detname != "NdetU" && detname != "TSc" && detname != "GeCHEH" && detname != "GeCHEL") continue;

      const int polarity = gSetup->GetTriggerPolarity(WFDBANKS[icrate][ich]);
      const int nBits = gSetup->GetNBits(WFDBANKS[icrate][ich]);
      const int max_adc = std::pow(2, nBits) - 1;

      const std::vector<TPulseIsland*>& pulses = wfd_map.at(WFDBANKS[icrate][ich]);

      //two loops, pulse loop
      int t0 = 0;
      for(int p = 0; p < pulses.size(); ++p){
	//initialize tree events
	overflow=false; dPulse=false; veto=false; pileup=false; threshold = false;
	pedestal = 0; PSD_param = -1; eFit = 0; eInt = 0; tdiff = -9999;

	//setup variables
	pedestal = pulses[p]->GetPedestal(8);
	if(detname == "TSc") pedestal = pulses[p]->GetPedestal(4);
	const std::vector<int>& samples = pulses[p]->GetSamples();
	const int nSamp = samples.size();
	if(nSamp < 8) continue;

	if(*(std::min_element( samples.begin(), samples.end() )) <= 0) overflow = true;
	if(*(std::max_element( samples.begin(), samples.end() )) >= max_adc-1) overflow = true;


	veto = pulses[p]->GetVetoPulse(); //vetoed
	pileup = pulses[p]->GetPileupPulse(); //Pileup Protection
	dPulse = pulses[p]->GetDoublePulse(); //double pulsing

	float integral_ps = pulses[p]->GetIntegral();
	float max = pulses[p]->GetPulseHeight();
	float threshold = MTCorrTree_Threshold(detname);
	float fit_max = max;
	if(detname != "TSc") fit_max = pulses[p]->GetFitMax();
	
	PSD_param = pulses[p]->GetPSDParameter();
	if(detname != "NdetD" && detname != "NdetU") PSD_param = -1;

	//pulse detail checks
	if(max < polarity*(threshold-pedestal)) threshold = true;
	
	eInt = pulses[p]->GetEnergyInt(integral_ps);
	eFit = pulses[p]->GetEnergyFit(fit_max);

	if(detname == "TSc"){
	  vhTCorrTree_TSc_Amp->Fill(max);
	  TScCount++;
	  tdiff = -9999;
	  vvtTCorrTree->Fill();
	  continue;
	}

	//loop over TTSc times
	if(pulses[p]->GetTDCTime() < 0){vvtTCorrTree->Fill(); continue; }
	for(int t = t0; t<ref_hits.size(); ++t){
	  if(ref_hits[t]->GetTDCTime() < 0) continue;
	  if(ref_hits[t]->GetPileupPulse() ) continue;
	  if(ref_hits[t]->GetVetoPulse() ) continue;
	  float dt = TICKTDC*(pulses[p]->GetTDCTime() - ref_hits[t]->GetTDCTime());
	  if(dt < TIME_LOW) break;
	  else if(dt < TIME_HIGH) {tdiff = dt; break;}
	  else ++t0;
	}
	vvtTCorrTree->Fill();

      }
    }
  }

  return SUCCESS;
} 
  

float MTCorrTree_Threshold(std::string detname){
  float thresh = 0;
  if(detname == "GeCHEH") thresh = 1500;
  else if(detname == "GeCHEL") thresh = 1050;
  else if(detname == "NdetD") thresh = 15450;
  else if(detname == "NdetU") thresh = 15510;
  else if(detname == "LaBr3") thresh = 15645;
  else if(detname == "TSc") thresh = 15650;
  else thresh = 0;
  return thresh;
}

