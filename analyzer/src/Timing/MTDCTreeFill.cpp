////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTDCTreeFill
/// \ingroup lldq
/// \author Cole Kampa
////////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <utility>
#include <cmath>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
//#include <TH1F.h>
#include <TDirectory.h>
#include "TFile.h"
#include "TApplication.h"
//#include "TNtuple.h"
#include <TTree.h>
#include <TBranch.h>

/* AlCap includes */
#include "AlCap.h"
#include "TGlobalData.h"
#include "TSetupData.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT MTDCTreeFill_init(void);
INT MTDCTreeFill(EVENT_HEADER*, void*);
INT MTDCTreeFill_eor(INT);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

// tree definition
extern TFile *fTreeFile;
static TTree *fTDCTree = NULL;
//static std::map<std::string, TBranch *> tTDCDetectorToBranchMap;
static TBranch * fTDCBranch = NULL;

// ntuple and histogram definitions
//std::map<std::string, TNtuple*> Ntuple_map;
//std::map<std::string, TH1F*> hTDC_map, hRollover_map;
//std::map<int, TNtuple*> nTDC;
//std::map<int, TH1*> vhTDCHitTime;

//std::map<int, TNtuple*> nTDC;
//std::map<int, TH1*> vhTDCHitTime;

//std::map<std::string, TNtuple*> nTDC;
//std::map<std::string, TH1*> vhTDCHitTime;

//bool TDCHitTimeRedux_firstEvent = true;
//map< string, vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;
//map< string, vector<int64_t> >& tdc_map;

using namespace AlCap;
namespace {
  //TDirectory* DIR;
  //TH1* vhTDCHitTime[NCHANTDC];
  //TNtuple* nTDC[NCHANTDC];
  string TDCBANKS[NCHANTDC];
}


ANA_MODULE MTDCTreeFill_module =
{
  "MTDCTreeFill",    /* module name           */
  "Cole Kampa",   /* author                */
  MTDCTreeFill,      /* event routine         */
  NULL,             /* BOR routine           */
  MTDCTreeFill_eor,  /* EOR routine           */
  MTDCTreeFill_init, /* init routine          */
  NULL,             /* exit routine          */
  NULL,             /* parameter structure   */
  0,                /* structure size        */
  NULL,             /* initial parameters    */
};

INT MTDCTreeFill_init() {
  printf("MTDCTreeFill_init\n");

  //const map< string, vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;
  map< string, vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;
  //tdc_map = gData->fTDCHitsToChannelMap;
  // FIXME! want to store all detectors, but start with one as a test
  /*  
  const map< string, vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;
  vector<int64_t*
  for (int ich = 0; ich < NCHANTDC; ++ich) {
    char bank[5]; sprintf(bank, "T4%02d", ich);
    const string det = gSetup->GetDetectorName(bank);
    //TDCBANKS[ich] = bank;
    if det == "TNdetD" {

    }
  }
  */

  fTreeFile->cd();

  fTDCTree = new TTree("TDCTree", "Hit times in TDCs (V1920)");
  fTDCTree->SetAutoSave(300000000); // autosave when 300 Mbyte written.
  fTDCTree->SetMaxVirtualSize(300000000); // 300 Mbyte
  
  int split = 1; // how deep of a split of branches
  //int split = 2; // how deep of a split of branches
  int bufsize = 64000;

  // test on a single channel
  //TW
  fTDCBranch = fTDCTree->Branch("TDC_TSync", "std::vector<int64_t>", &tdc_map["T401"], bufsize, split); // 556 pulser -- 55.6 hits per block

  //fTDCBranch = fTDCTree->Branch("TDC", "std::map< std::string, std::vector<int64_t> >", &tdc_map, bufsize, split);
  fTDCBranch->SetAutoDelete(kFALSE);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
  /*
  TDirectory* cwd = gDirectory;
  DIR = gDirectory->mkdir("TDCHitTimeRedux");
  DIR->cd();

  // Create a histogram and ntuple for each detector
  for (int ich = 0; ich < NCHANTDC; ++ich) {
    char bank[5]; sprintf(bank, "T4%02d", ich);
    const string det = gSetup->GetDetectorName(bank);
    TDCBANKS[ich] = bank;
    char name[64], title[128];
    sprintf(name, "hTDCHitTime_%s_%s", bank, det.c_str());
    sprintf(title, "TDC block hit times %s;Time (ns)", det.c_str());
    //vhTDCHitTime[ich] = new TH1F(name, title, 120.e6/25.e2, 0., 120.e6);
    //vhTDCHitTime[ich]->Sumw2();
    vhTDCHitTime[bank] = new TH1F(name, title, 120.e6/25.e2, 0., 120.e6);
    vhTDCHitTime[bank]->Sumw2();
    // ntuple
    // stores: TDC, NRollover
    string ntupname = "ntup_" + det + "_" + bank;
    string ntuptitle = "ntuple for " + det;
    //TNtuple* ntuple = new TNtuple(ntupname.c_str(), ntuptitle.c_str(), "Integral:Amplitude:iMax:nSamp:SIntegral:LIntegral:SIntegral_pedsub:LIntegral_pedsub:IntegralRatio:status_IntegralRatio:Polarity:pedBegin:pedEnd:pedX0:pedSlope:pedInt:pedIntS:pedIntL");
    //TNtuple* ntuple = new TNtuple(ntupname.c_str(), ntuptitle.c_str(), "TDCHitTick:TDCHitTime:NRollover");
    // FIXME! need to include rollover count in TGlobalData during TDC processing.
    TNtuple* ntuple = new TNtuple(ntupname.c_str(), ntuptitle.c_str(), "TDCHitTick:TDCHitTime");
    //nTDC[ich] = ntuple;
    nTDC[bank] = ntuple;
  }
  cwd->cd();
  */
  return SUCCESS;
}

INT MTDCTreeFill_eor(INT run_number) {
  //TDirectory* cwd = gDirectory;
  //DIR->cd();
  // FIXME! Do I want this normalization? If I do, probably want to make it optional. i.e. NMuRun is initialized to 0, and should be meaningless in calibration runs.
  /*
  for (int ich = 0; ich < NCHANTDC; ++ich) {
    TH1* h0 = vhTDCHitTime[ich];
    string name(h0->GetName());
    string title(h0->GetTitle());
    name += "_normalized";
    title += " (normalized)";
    TH1* h = (TH1*)h0->Clone(name.c_str());
    h->SetTitle(title.c_str());
    h->Scale(1./gData->NMuRun());
  }
  */
  //cwd->cd();
  return SUCCESS;
}

//INT MTDCHittimeRedux_BookHistograms() {
//}

INT MTDCTreeFill(EVENT_HEADER *pheader, void *pevent) {
  //tdc_map = gData->fTDCHitsToChannelMap;
  fTDCTree->Fill();
  
  /*  
  const map< string, vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;

  for (int ich = 0; ich < NCHANTDC; ++ich) {
    char bank[5]; sprintf(bank, "T4%02d", ich);
    if (!tdc_map.count(TDCBANKS[ich])) continue;
    const vector<int64_t>& hits = tdc_map.at(TDCBANKS[ich]);
    for (int i = 0; i < hits.size(); ++i) {
      const float vals [2] = {hits[i], TICKTDC*hits[i]};
      //vhTDCHitTime[ich]->Fill(TICKTDC*hits[i]);
      //vhTDCHitTime[ich]->Fill(vals[1]);
      //nTDC[ich]->Fill(vals);
      vhTDCHitTime[bank]->Fill(vals[1]);
      nTDC[bank]->Fill(vals);
    }
  }
  */

  return SUCCESS;
}

/// @}
