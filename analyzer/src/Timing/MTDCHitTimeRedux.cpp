////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTDCHitTime
/// \ingroup lldq
/// \author John Quirk
/// updated version ("redux"): Cole Kampa
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
#include <TH1F.h>
#include <TDirectory.h>
#include "TFile.h"
#include "TApplication.h"
#include "TNtuple.h"

/* AlCap includes */
#include "AlCap.h"
#include "TGlobalData.h"
#include "TSetupData.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
//static INT MTDCHitTimeRedux_init(void);
//static INT MTDCHitTimeRedux(EVENT_HEADER*, void*);
//static INT MTDCHitTimeRedux_eor(INT);
INT MTDCHitTimeRedux_init(void);
INT MTDCHitTimeRedux(EVENT_HEADER*, void*);
INT MTDCHitTimeRedux_eor(INT);
INT MTDCHitTimeRedux_BookHistograms(); // do this in init?

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

// ntuple and histogram definitions
//std::map<std::string, TNtuple*> Ntuple_map;
//std::map<std::string, TH1F*> hTDC_map, hRollover_map;
//std::map<int, TNtuple*> nTDC;
//std::map<int, TH1*> vhTDCHitTime;

//std::map<int, TNtuple*> nTDC;
//std::map<int, TH1*> vhTDCHitTime;

std::map<std::string, TNtuple*> nTDC;
std::map<std::string, TH1*> vhTDCHitTime;

bool TDCHitTimeRedux_firstEvent = true;
//int block_TDC_Redux = 0;

using namespace AlCap;
namespace {
  TDirectory* DIR;
  //TH1* vhTDCHitTime[NCHANTDC];
  //TNtuple* nTDC[NCHANTDC];
  string TDCBANKS[NCHANTDC];
}

ANA_MODULE MTDCHitTimeRedux_module =
{
  "MTDCHitTime",    /* module name           */
  "John R Quirk",   /* author                */
  MTDCHitTimeRedux,      /* event routine         */
  NULL,             /* BOR routine           */
  MTDCHitTimeRedux_eor,  /* EOR routine           */
  MTDCHitTimeRedux_init, /* init routine          */
  NULL,             /* exit routine          */
  NULL,             /* parameter structure   */
  0,                /* structure size        */
  NULL,             /* initial parameters    */
};

INT MTDCHitTimeRedux_init() {
  return SUCCESS;
}

/*
INT MTDCHitTimeRedux_init() {
  printf("MTDCHitTimeRedux_init\n");
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
    //TNtuple* ntuple = new TNtuple(ntupname.c_str(), ntuptitle.c_str(), "TDCHitTick:TDCHitTime");
    TNtuple* ntuple = new TNtuple(ntupname.c_str(), ntuptitle.c_str(), "TDCHitTick:TDCHitTime:Block:iHit");
    //nTDC[ich] = ntuple;
    nTDC[bank] = ntuple;
  }
  cwd->cd();
  return SUCCESS;
}
*/

INT MTDCHitTimeRedux_eor(INT run_number) {
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

INT MTDCHitTimeRedux_BookHistograms() {
  //printf("MTDCHitTimeRedux_init\n");
  TDirectory* cwd = gDirectory;
  DIR = gDirectory->mkdir("TDCHitTimeRedux");
  DIR->cd();

  // Create a histogram and ntuple for each detector
  /*  
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
    //TNtuple* ntuple = new TNtuple(ntupname.c_str(), ntuptitle.c_str(), "TDCHitTick:TDCHitTime");
    TNtuple* ntuple = new TNtuple(ntupname.c_str(), ntuptitle.c_str(), "TDCHitTick:TDCHitTime:Block:iHit");
    //nTDC[ich] = ntuple;
    nTDC[bank] = ntuple;
  }
  */
  const map< string, vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;

  int ich = 0;
  for (map<string, vector<int64_t>>::const_iterator mIter = tdc_map.begin(); mIter != tdc_map.end(); mIter++) {
    //const string bank = mIter->first;
    //const string det = gSetup->GetDetectorName(bank);
    string bank = mIter->first;
    string det = gSetup->GetDetectorName(bank);
    TDCBANKS[ich] = bank;
    char name[64], title[128];
    //sprintf(name, "hTDCHitTime_%s_%s", bank, det.c_str());
    //sprintf(title, "TDC block hit times %s;Time (ns)", det.c_str());
    sprintf(name, "hTDCHitTime_%s_%s", bank.c_str(), det.c_str());
    sprintf(title, "TDC block hit times %s;Time (ns)", det.c_str());
    vhTDCHitTime[bank] = new TH1F(name, title, 120.e6/25.e2, 0., 120.e6);
    vhTDCHitTime[bank]->Sumw2();
    // ntuple
    string ntupname = "ntup_" + det + "_" + bank;
    string ntuptitle = "ntuple for " + det;
    //TNtuple* ntuple = new TNtuple(ntupname.c_str(), ntuptitle.c_str(), "TDCHitTick:TDCHitTime:Block:iHit");
    TNtuple* ntuple = new TNtuple(ntupname.c_str(), ntuptitle.c_str(), "TDCHitTick:TDCHitTime:Event:iHit");
    // turn off autosave
    ntuple->SetAutoSave(0);
    nTDC[bank] = ntuple;
    ich++;
  }

  cwd->cd();
  return SUCCESS;

}

INT MTDCHitTimeRedux(EVENT_HEADER *pheader, void *pevent) {
  // Get the event number
  int midas_event_number = pheader->serial_number;  
  //block_TDC_Redux++;
  if(TDCHitTimeRedux_firstEvent == true){
    MTDCHitTimeRedux_BookHistograms();
    TDCHitTimeRedux_firstEvent = false;
  }

  const map< string, vector<int64_t> >& tdc_map = gData->fTDCHitsToChannelMap;

  /*
  for (int ich = 0; ich < NCHANTDC; ++ich) {
    char bank[5]; sprintf(bank, "T4%02d", ich);
    //if (!tdc_map.count(TDCBANKS[ich])) continue;
    const vector<int64_t>& hits = tdc_map.at(TDCBANKS[ich]);
    for (unsigned int i = 0; i < hits.size(); ++i) {
      //const float vals [2] = {hits[i], TICKTDC*hits[i]};
      float fhit = static_cast<float>(hits[i]);
      float ftime = fTICKTDC*static_cast<float>(hits[i]);
      float fi = static_cast<float>(i+1);
      float fBlock = static_cast<float>(block_TDC_Redux);
      //const float vals [4] = {hits[i], TICKTDC*hits[i], block_TDC, i+1};
      const float vals [4] = {fhit, ftime, fBlock, fi};
      //vhTDCHitTime[ich]->Fill(TICKTDC*hits[i]);
      //vhTDCHitTime[ich]->Fill(vals[1]);
      //nTDC[ich]->Fill(vals);
      //vhTDCHitTime[bank]->Fill(vals[1]);
      vhTDCHitTime[bank]->Fill(TICKTDC*hits[i]);
      nTDC[bank]->Fill(vals);
    }
  */

  for (map<string, vector<int64_t>>::const_iterator mIter = tdc_map.begin(); mIter != tdc_map.end(); mIter++) {
    const string bank = mIter->first;
    //const string det = gSetup->GetDetectorName(bank);   
    //if (!tdc_map.count(TDCBANKS[ich])) continue;
    const vector<int64_t>& hits = mIter->second;
    int iHit=0;
    //int fTimestamp;

    for (vector<int64_t>::const_iterator hIter = hits.begin(); hIter != hits.end(); hIter++){
      iHit++;
      //const float vals [4] = {*hIter, fTICKTDC*(*hIter), block_TDC_Redux, iHit};
      //const float vals [4] = {*hIter, fTICKTDC*(*hIter), midas_event_number, iHit};
      const float vals [4] = {*hIter, TICKTDC*(*hIter), midas_event_number, iHit};
      //const float vals [2] = {hits[i], TICKTDC*hits[i]};
      //float fhit = static_cast<float>(hits[i]);
      //float ftime = fTICKTDC*static_cast<float>(hits[i]);
      //float fi = static_cast<float>(i+1);
      //float fBlock = static_cast<float>(block_TDC_Redux);
      //const float vals [4] = {hits[i], TICKTDC*hits[i], block_TDC, i+1};
      //const float vals [4] = {fhit, ftime, fBlock, fi};
      //vhTDCHitTime[ich]->Fill(TICKTDC*hits[i]);
      //vhTDCHitTime[ich]->Fill(vals[1]);
      //nTDC[ich]->Fill(vals);
      //vhTDCHitTime[bank]->Fill(vals[1]);
      vhTDCHitTime[bank]->Fill(TICKTDC*(*hIter));
      nTDC[bank]->Fill(vals);
    }

  }

  return SUCCESS;
}

/// @}
