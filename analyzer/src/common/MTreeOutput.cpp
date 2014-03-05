/********************************************************************\

  Name:         MTreeOutput
  Created by:   Peter Winter

  Contents:     Module to generate a ROOT tree from the data in gData

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TROOT.h>
#include <TPluginManager.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TVacuumData.h"

/*-- Module declaration --------------------------------------------*/

INT  MTreeOutput_init(void);
INT  MTreeOutput_exit(void);
INT  MTreeOutput(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;
extern TVacuumData* gVacuum;
extern char *gMiasTreeOutputFileName;

static TFile *fTreeFile = NULL;
static TTree *fEventTree = NULL;
static TBranch *fEventBranch = NULL;
static TTree *fSetupTree = NULL;
static TBranch *fSetupBranch = NULL;
static TTree *fVacuumTree = NULL;
static TBranch *fVacuumBranch = NULL;

ANA_MODULE MTreeOutput_module =
{
  "MTreeOutput",                 /* module name           */
  "Peter Winter",                /* author                */
  MTreeOutput,                   /* event routine         */
  NULL,                          /* BOR routine           */
  NULL,                          /* EOR routine           */
  MTreeOutput_init,              /* init routine          */
  MTreeOutput_exit,              /* exit routine          */
  NULL,                          /* parameter structure   */
  0,                             /* structure size        */
  NULL,                          /* initial parameters    */
};

extern TROOT* gROOT;
static bool PLUGIN_LOADED = false;

/** This method initializes the tree file and tree
  */
INT MTreeOutput_init()
{
  if (!PLUGIN_LOADED) {
    if(gROOT->GetPluginManager()->FindHandler("TVirtualStreamerInfo") == NULL)
      gROOT->GetPluginManager()->AddHandler("TVirtualStreamerInfo","*","TStreamerInfo","RIO","TStreamerInfo()");
    PLUGIN_LOADED = true;
  }
    
  if(gMiasTreeOutputFileName){
    fTreeFile = TFile::Open(gMiasTreeOutputFileName, "recreate");
    if(!fTreeFile || fTreeFile->IsZombie()){
      printf("Could not open tree file %s!\n", gMiasTreeOutputFileName);
      return ANA_SKIP;
    }  
  }
  fTreeFile->cd();
  
  fEventTree = new TTree("EventTree","All readout channels");
  fEventTree->SetAutoSave(300000000); // autosave when 300 Mbyte written.
  fEventTree->SetMaxVirtualSize(300000000); // 300 Mbyte
  int split = 1;
  int bufsize = 64000;
  Int_t branchstyle = 1;

  if (split < 0) {branchstyle = 0; split = -1-split;}
  TTree::SetBranchStyle(branchstyle);

  fEventBranch = fEventTree->Branch("Event", "TGlobalData", &gData, bufsize, split);
  fEventBranch->SetAutoDelete(kFALSE);

  // The TTree with the setup information
  fSetupTree = new TTree("SetupTree","All setup information");
  fSetupTree->SetAutoSave(300000000); // autosave when 300 Mbyte written.
  fSetupTree->SetMaxVirtualSize(300000000); // 300 Mbyte

  fSetupBranch = fSetupTree->Branch("Setup", "TSetupData", &gSetup, bufsize, split);
  fSetupBranch->SetAutoDelete(kFALSE);
  
  fSetupTree->Fill();

  // The TTree with the Vacuum information
  fVacuumTree = new TTree("VacuumTree","All setup information");
  fVacuumTree->SetAutoSave(300000000); // autosave when 300 Mbyte written.
  fVacuumTree->SetMaxVirtualSize(300000000); // 300 Mbyte

  fVacuumBranch = fVacuumTree->Branch("Vacuum", "TVacuumData", &gVacuum, bufsize, split);
  fVacuumBranch->SetAutoDelete(kFALSE);
  
  return SUCCESS;
}

/** This method writes the data in gData into the branches of the tree  */
INT MTreeOutput(EVENT_HEADER *pheader, void *pevent)
{
  fEventTree->Fill();
  fVacuumTree->Fill();
  return SUCCESS;
}

/** This method writes the tree and closes the tree file  */
INT MTreeOutput_exit()
{
  if(fTreeFile){
    fTreeFile->Write();
    fTreeFile->Close();
  }

  return SUCCESS;
}

