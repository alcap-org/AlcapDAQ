#include "TRawAnalysis.h"

#include "TReadODB.h"
#include "TMucapData.h"

#include "TCaenCompData.h"
#include "TStackedTDC400Data.h"
#include "TNeutronRaw.h"
#include "TWFDBankReader.h"

#include <stdio.h>
#include <iostream>
using std::cout; using std::cerr; using std::endl;
#include <string>
using std::string;
#include <vector>
using std::vector;

/******* global variables **********/
extern TMucapData *gData;
extern TReadODB *gODB;
extern TPC_PARAM tpc_parameters;
/******* end global variables **********/

#ifdef STANDALONE
extern void MCaenCompProcessRaw_init();  // standalone
extern INT MCaenCompProcessRaw(EVENT_HEADER *pheader, void *pevent);  // standalone
extern void MTDC400ProcessRaw_init();  // standalone
extern INT MTDC400ProcessRaw(EVENT_HEADER *pheader, void *pevent);  // standalone
#endif

TRawAnalysis::TRawAnalysis()
{
  cout << "TRawAnalysis::TRawAnalysis()" << endl;
#ifdef STANDALONE
  MCaenCompProcessRaw_init();  // standalone
  MTDC400ProcessRaw_init();  // standalone
#endif
  
  // Create raw data objects
  gData->CaenComp = new TCaenCompData;
  gData->StackedTDC400 = new TStackedTDC400Data;
  // Make a reader for each pad.
  for(int ipad=0; ipad<tpc_parameters.kNPads; ipad++) {
    int crate = tpc_parameters.GetWFDCrate(ipad);
    int slot = tpc_parameters.GetWFDSlot(ipad);
    int ch = tpc_parameters.GetWFDChannel(ipad);
    // Could check Channel enabled here
    int dbc = tpc_parameters.GetWFDDBC(crate,slot,ch);
    int personality = tpc_parameters.GetWFDPersonality(crate,slot,ch);
    string bankname = tpc_parameters.Pad2WFDBankName(ipad);
    gData->fTPCWFDBanks.push_back(TWFDBankReader(dbc,personality,bankname));
  }
  
  // Make the TTPCIslandManager. Don't fill anything yet. 
  // The constructor looks at the number of pads, so it's
  // important that TParametersAnalysis has already been constructed.
  gData->fTPCIslands = new TTPCIslandManager();

  // Make the Ge Det. bank readers
  vector<TOctalFADCBankReader>& vGeFADCBanks = gData->fGeFADCBanks;
  for(int i=0; i<8; i++){
    char name[8];
    sprintf(name,"N%cfe", 'a'+i);
    vGeFADCBanks.push_back(TOctalFADCBankReader(name));
  }

#ifdef RUN9
  gData->NeutronBank = new TNeutronRaw;
#endif
}

TRawAnalysis::~TRawAnalysis()
{
  printf("Destructor TRawAnalysis()\n");

  delete gData->StackedTDC400;
  delete gData->CaenComp;
  delete gData->fTPCIslands;
#ifdef RUN9
  delete gData->NeutronBank;
#endif
}

INT TRawAnalysis::ProcessEvent(EVENT_HEADER *pheader, void *pevent)
{
#ifdef STANDALONE
  MCaenCompProcessRaw(pheader, pevent); // standalone
  MTDC400ProcessRaw(pheader, pevent); // standalone
#endif
  gData->CaenComp->ProcessEvent(pheader, pevent);
  gData->StackedTDC400->ProcessEvent(pheader, pevent);
  
  // Each bank reader needs to process its bank
  for(int ipad=0; ipad<tpc_parameters.kNPads; ipad++) {
    gData->fTPCWFDBanks.at(ipad).ProcessEvent(pheader,pevent);
  }

  //Now import islands into TTPCIslandManager, specializing them for the TPC
  gData->fTPCIslands->ProcessEvent();

  // Process bank readers
  for(int i=0; i<gData->fGeFADCBanks.size(); ++i){
    gData->fGeFADCBanks[i].ProcessEvent(pheader, pevent);
  }

  // Create GeDet Islands out of the neutron pulses
    

#ifdef RUN9

  gData->NeutronBank->Clear();

  gData->NeutronBank->ProcessEvent(pheader, pevent);

#endif

  return SUCCESS;
}

