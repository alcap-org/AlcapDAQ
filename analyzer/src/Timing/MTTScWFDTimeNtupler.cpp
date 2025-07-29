////////////////////////////////////////////////////////////////////////////////
/// \defgroup MTTScWFDTimeNtupler
/// \ingroup Timing
/// \author Cole Kampa
///
///////////////////////////////////////////////////////////////////////////////

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <cmath>
#include <algorithm>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>
#include <TDirectory.h>
#include "TF1.h"
#include "TFile.h"
#include "TApplication.h"
#include "TROOT.h"
//#include "TMath.h"
//#include "Math/Interpolator.h"
#include "TNtuple.h"

/* AlCap includes */
#include "AlCap.h"
#include "tOffsets.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using namespace AlCap;
//using namespace std;
namespace {
  // ntuples for timing of different detectors
  std::map<std::string, TNtuple*> Ntuple_map;
  // ntuple for number of hits in the event from TTSc
  TNtuple* Ntuple_Nhits;
  bool firstEvent = true;
  std::string TTSCBANK;
  std::string TVSCBANK;
  // time offset maps
  std::map<std::string, double> detADC_min_TScTDC_map{{"NdetD", detADC_min_TScTDC[0]}, {"NdetU", detADC_min_TScTDC[1]}, {"GeCHEH", detADC_min_TScTDC[2]}};
  std::map<std::string, double> detTDC_min_TScTDC_map{{"TNdetDV", detTDC_min_TScTDC[2]}, {"TNdetUV", detTDC_min_TScTDC[3]}, {"TVSc", detTDC_min_TScTDC[4]}, {"TGeV", detTDC_min_TScTDC[6]}};
  // FIXME! Target dependent PU cut?
  // double PU=10000.; // +-10 micros -- charged emission value; Damien's thesis value
  double PU=5000.; // +-5 micros
  //double PU=3500.; // +-3.5 micros ~ 4 lifetimes (Al)
  //double PU=2000.; // +-2 micros
  // PUV
  double PUV=PU; // same as TTSc
  // different from TTSc?
}

/*-- Module declaration --------------------------------------------*/
INT  MTTScWFDTimeNtupler_init(void);
INT  MTTScWFDTimeNtupler(EVENT_HEADER*, void*);
INT  MTTScWFDTimeNtupler_eor(INT);
INT  MTTScWFDTimeNtupler_BookHistograms();
// pileup
// TSc
std::vector<int64_t> TTSc_Pileup_Cut(const std::vector<int64_t>&, double);
// VTSc
std::vector<int64_t> TVSc_Pileup_Cut(const std::vector<int64_t>&, const std::vector<int64_t>&, double, double);
// find nearest TTSc hit times -- 4 vals: [tnear_positive, tnear_negative, tnear_positive-1, tnear_negative+1]
// WFD hit time that is passed in should already have the appropriate time offset applied.
std::vector<double> WFD_find_nearest_TTSc(double, const std::vector<int64_t>&);
// find nearest veto hit times -- 2 vals: [tnear_positive, tnear_negative]
// WFD hit time that is passed in should already have the appropriate time offset applied.
std::vector<double> WFD_find_nearest_Veto(double, const std::vector<int64_t>&);
// find nearest sync pulse (ADC)
// for vetoing pulses coincident with sync pulses in Ge
// WFD_time is raw fTimeStamp (no offsets, we are looking in the same crate)
// Sync_pulses is raw fTimeStamp of sync hits from that crate
std::vector<double> WFD_find_nearest_Sync(double, const std::vector<TPulseIsland*>&, const double);
std::vector<double> WFD_find_nearest_TSync(double, const std::vector<int64_t>&, double);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

ANA_MODULE MTTScWFDTimeNtupler_module =
{
	"MTTScWFDTimeNtupler",                  /* module name           */
	"Cole Kampa",            /* author                */
	MTTScWFDTimeNtupler,                    /* event routine         */
	NULL,                          /* BOR routine           */
	MTTScWFDTimeNtupler_eor,                /* EOR routine           */
	MTTScWFDTimeNtupler_init,               /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/////////////////////////////////////////////////////////////////

INT MTTScWFDTimeNtupler_init(){
  return SUCCESS;
}

INT MTTScWFDTimeNtupler_BookHistograms()
{
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("TTScWFDTimeNtupler")->cd();

  const std::map<std::string, std::vector<TPulseIsland*> >& tpi_map = gData->fPulseIslandToChannelMap;

  // loop through ADC pulses
  for(std::map<std::string, std::vector<TPulseIsland*> >::const_iterator mIter = tpi_map.begin(); mIter != tpi_map.end(); mIter++){
    std::string bankname = mIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);

    //if (detname != "NdetD" && detname != "NdetU") continue;
    if (detname != "NdetD" && detname != "NdetU" && detname != "GeCHEH") continue;

    // ntuple
    // stores: Integral, Amplitude
    std::string ntupname = "ntup_" + detname + "_" + bankname;
    std::string ntuptitle = "ntuple for " + detname;
    // define ntuple
    // dev notes: Amplitude not strictly necessary, but wanting to check when comparing to EnergyPlots ntuple.
    TNtuple* ntuple = new TNtuple(ntupname.c_str(), ntuptitle.c_str(), "Event:iPulse:Amplitude:TSWFDtick:TSWFD_raw:TSWFD_raw_noCF:iSync:SyncOff:TSWFD:TSWFD_toff_TTSc:TTSc0p:TTSc0n:TTSc1p:TTSc1n:TTSc0pPU:TTSc0nPU:TTSc1pPU:TTSc1nPU:TTSc0pPUV:TTSc0nPUV:TTSc1pPUV:TTSc1nPUV:TSWFD_toff_Veto:TVeto0p:TVeto0n:Sync0p:Sync0n:TSync0p:TSync0n");
    // turn off autosave
    ntuple->SetAutoSave(0);
    Ntuple_map[bankname] = ntuple;
    
  }
  // setup ntuple for Nhits
  std::string nname = "ntup_NHits_TTSc";
  std::string ntitle = "ntuple for N Hits in TTSc";
  TNtuple* ntuple_hits = new TNtuple(nname.c_str(), ntitle.c_str(), "Event:N_TTSc:N_TVSc:N_TTSc_PU:N_TTSc_PUV");
  ntuple_hits->SetAutoSave(0);
  Ntuple_Nhits = ntuple_hits;
  // extra setup
  // bankname for TSc
  TTSCBANK = gSetup->GetBankName("TTSc");
  TVSCBANK = gSetup->GetBankName("TVSc");

  cwd->cd();
  return SUCCESS;
}

INT MTTScWFDTimeNtupler_eor(INT run_number)
{
  return SUCCESS;  // currently nothing to do here
}

INT MTTScWFDTimeNtupler(EVENT_HEADER *pheader, void *pevent)
{
  // DEBUG
  // std::cout << "in MTTScWFDTimeNtupler()" << std::endl;

  // Get the event number
  int midas_event_number = pheader->serial_number;
  if(firstEvent == true){
    MTTScWFDTimeNtupler_BookHistograms();
    firstEvent = false;
  }

  const std::map<std::string, std::vector<TPulseIsland*> >& tpi_map = gData->fPulseIslandToChannelMap;
  const std::map<std::string, std::vector<int64_t>>& tdc_map = gData->fTDCHitsToChannelMap;

  // grab appropriate TDC hit vectors for comparison to WFD pulses
  // TTSc
  const std::vector<int64_t>& TTSc_hits = tdc_map.at(TTSCBANK);
  // TVSc
  const std::vector<int64_t>& TVSc_hits = tdc_map.at(TVSCBANK);
  // pileup TSc
  const std::vector<int64_t>& TTSc_hits_PU = TTSc_Pileup_Cut(TTSc_hits, PU);
  // pileup VSc
  const std::vector<int64_t>& TTSc_hits_PUV = TVSc_Pileup_Cut(TTSc_hits_PU, TVSc_hits, detTDC_min_TScTDC_map.at("TVSc"), PUV);
  // grab veto in the loop

  // fill Nhits ntuple
  //TNtuple* ntuple_hits = new TNtuple("ntup_NHits_TTSc".c_str(), "ntuple for N Hits in TTSc".c_str(), "Event:N_TTSc:N_TVSc:N_TTSc_PU:N_TTSc_PUV");
  //ntuple_hits->SetAutoSave(0);
  //Ntuple_Nhits = ntuple_hits;
  const float vals_hits [5] = {midas_event_number, TTSc_hits.size(), TVSc_hits.size(), TTSc_hits_PU.size(), TTSc_hits_PUV.size()};
  Ntuple_Nhits->Fill(vals_hits);

  for(std::map<std::string, std::vector<TPulseIsland*> >::const_iterator mIter = tpi_map.begin(); mIter != tpi_map.end(); mIter++){
    const std::string bankname = mIter->first;
    const std::string icrate_str = bankname.substr(1, 1);
    const int icrate = std::stoi(icrate_str);
    const std::string detname = gSetup->GetDetectorName(bankname);
    const std::vector<TPulseIsland*>& pulses = mIter->second;
    // WFD tick time
    const double TICKWFD_crate = TICKWFD[icrate];
    // Sync pulses
    const std::string detsync = "SyncCrate" + icrate_str;
    const std::string banksync = gSetup->GetBankName(detsync);
    const std::vector<TPulseIsland*>& Sync_pulses = tpi_map.at(banksync);
    // TSync hits
    const std::string dettsync = "TSync";
    const std::string banktsync = gSetup->GetBankName(dettsync);
    const std::vector<int64_t>& TSync_hits = tdc_map.at(banktsync);
    // SyncPulse
    // toff = synctdc - syncwfd, so wfd+toff puts time on equal footing to TDC.
    // note: if index=-1, toff ~ 0 (closest numerical float value to zero)
    const double toff = gData->fTDCSynchronizationPulseOffset[icrate];
    // index = -1 is the flag for failed sync
    const int iSync = gData->fTDCSynchronizationPulseIndex[icrate];

    if (detname != "NdetD" && detname != "NdetU" && detname != "GeCHEH") continue;

    // veto information
    std::string dn_v;
    if (detname == "GeCHEH") {
      dn_v = "Ge";
    } else {
      dn_v = detname;
    }
    const std::string vdetname = "T" + dn_v + "V";
    const std::string vbankname = gSetup->GetBankName(vdetname);
    // hits
    const std::vector<int64_t>& veto_hits = tdc_map.at(vbankname);

    int iPulse=0;
    //int Timestamp_tick;
    double Timestamp_tick, Timestamp_tick_noCF;
    double fTimestamp_raw, fTimestamp_raw_noCF, fTimestamp, fTimestamp_TTSc, fTimestamp_Veto;
    double Amplitude;

    for(std::vector<TPulseIsland*>::const_iterator pIter = pulses.begin(); pIter != pulses.end(); pIter++){
      iPulse++;
      // pulse length too short, continue (matches filter in EnergyPlotsRedux)
      // get the samples
      // const std::vector<int>& samples = (*pIter)->GetSamples();
      // if(samples.size() < 8) continue;
      const int samples_size = (*pIter)->GetSamples().size();
      if(samples_size < 8) continue;
      // pulse time
      // CF for Ge
      if ((*pIter)->HasCFTime()) {
        Timestamp_tick = (*pIter)->GetTimeStampCF();
        Timestamp_tick_noCF = (double)(*pIter)->GetTimeStamp();
        // convert to ns
        fTimestamp_raw = TICKWFD_crate*Timestamp_tick;
        fTimestamp_raw_noCF = TICKWFD_crate*Timestamp_tick_noCF;
      } else {
        Timestamp_tick = (double)(*pIter)->GetTimeStamp();
        Timestamp_tick_noCF = Timestamp_tick;
        // convert to ns
        fTimestamp_raw = TICKWFD_crate*Timestamp_tick;
        fTimestamp_raw_noCF = fTimestamp_raw;
      }
      // add SyncPulseOffset (all times are w.r.t. TDC)
      fTimestamp = fTimestamp_raw + toff;
      Amplitude = (*pIter)->GetPulseHeight();
      // correct WFD timestamp for TTSc offset
      // dt = detADC - TScTDC
      fTimestamp_TTSc = fTimestamp - detADC_min_TScTDC_map.at(detname);
      // find nearest TTSc
      std::vector<double> TTSc_vals = WFD_find_nearest_TTSc(fTimestamp_TTSc, TTSc_hits);
      // with pileup (TTSc only)
      std::vector<double> TTSc_vals_PU = WFD_find_nearest_TTSc(fTimestamp_TTSc, TTSc_hits_PU);
      // with pileup (TTSc+TVSc)
      std::vector<double> TTSc_vals_PUV = WFD_find_nearest_TTSc(fTimestamp_TTSc, TTSc_hits_PUV);
      // correct WFD timestamp for TTSc offset
      // dt = detTDC - TScTDC
      fTimestamp_Veto = fTimestamp - detTDC_min_TScTDC_map.at(vdetname);
      // find nearest veto
      // FIXME! Need to adjust veto_hits to match TTSc time?
      std::vector<double> veto_vals = WFD_find_nearest_Veto(fTimestamp_Veto, veto_hits);
      // find nearest sync pulse (Ge cut)
      // ADC
      //std::vector<double> Sync_vals = WFD_find_nearest_Sync(fTimestamp_raw, Sync_pulses, TICKWFD_crate);
      std::vector<double> Sync_vals = WFD_find_nearest_Sync(fTimestamp_raw_noCF, Sync_pulses, TICKWFD_crate);
      // TDC
      std::vector<double> TSync_vals = WFD_find_nearest_TSync(fTimestamp_raw_noCF+toff, TSync_hits, toff);
      // find nearest Veto (positive and negative)
      // find nearest det TDC
      // --> if this appears to work well (do some studies), should consider make another version of the ntupler that looks for nearest w.r.t. det TDC, not ADC -- e.g. MTTScTDCTimeNtupler.cpp

      // fill the ntuple
      //TNtuple* ntuple = new TNtuple(ntupname.c_str(), ntuptitle.c_str(), "Event:iPulse:Amplitude:TSWFDtick:TSWFD_raw:TSWFD_raw_noCF:iSync:SyncOff:TSWFD:TSWFD_toff_TTSc:TTSc0p:TTSc0n:TTSc1p:TTSc1n:TTSc0pPU:TTSc0nPU:TTSc1pPU:TTSc1nPU:TTSc0pPUV:TTSc0nPUV:TTSc1pPUV:TTSc1nPUV:TSWFD_toff_Veto:TVeto0p:TVeto0n:Sync0p:Sync0n:TSync0p:TSync0n");
      const float vals [29] = {midas_event_number, iPulse, Amplitude, Timestamp_tick, fTimestamp_raw, fTimestamp_raw_noCF, iSync, toff, fTimestamp, fTimestamp_TTSc, TTSc_vals[0], TTSc_vals[1], TTSc_vals[2], TTSc_vals[3], TTSc_vals_PU[0], TTSc_vals_PU[1], TTSc_vals_PU[2], TTSc_vals_PU[3], TTSc_vals_PUV[0], TTSc_vals_PUV[1], TTSc_vals_PUV[2], TTSc_vals_PUV[3], fTimestamp_Veto, veto_vals[0], veto_vals[1], Sync_vals[0], Sync_vals[1], TSync_vals[0], TSync_vals[1]};
      Ntuple_map[bankname]->Fill(vals);
    }
  }

  return SUCCESS;
}

std::vector<int64_t> TTSc_Pileup_Cut(const std::vector<int64_t>& TTSc_hits, double PU) {
  std::vector<int64_t> TTSc_hits_PU;
  TTSc_hits_PU.reserve(TTSc_hits.size());
  // if (TTSc_hits.size() < 3) {
  //   for (int i = 0; i < TTSc_hits.size(); ++i) {
  //     hit_time0
  //   }
  // } return TTSc_hits;
  double hit_time0, hit_time1, hit_timem1;
  int64_t hit_tick0;
  // loop through TTSc_hits
  for (int i = 0; i < TTSc_hits.size(); ++i) {
    //hit_time0 = TICKTDC * TTSc_hits[i];
    hit_tick0 = TTSc_hits[i];
    hit_time0 = TICKTDC * hit_tick0;
    if (i == 0) {
      hit_timem1 = -1e10;
    }
    else {
      hit_timem1 = TICKTDC * TTSc_hits[i-1];
    }
    if (i < TTSc_hits.size()-1) {
      hit_time1 = TICKTDC * TTSc_hits[i+1];

    }
    else {
      hit_time1 = 1e10;
    }
    // check for PU
    bool PU_1, PU_m1;
    PU_1 = fabs(hit_time1 - hit_time0) < PU;
    PU_m1 = fabs(hit_time0 - hit_timem1) < PU;
    // if pileup, skip
    if (PU_1 || PU_m1) continue;
    // else we add to the accepted pulses
    //TTSc_hits_PU.push_back(hit_time0);
    // save the hit tick, not time
    TTSc_hits_PU.push_back(hit_tick0);
  }

  return TTSc_hits_PU;
}

std::vector<int64_t> TVSc_Pileup_Cut(const std::vector<int64_t>& TTSc_hits, const std::vector<int64_t>& TVSc_hits, double toff_TVSc,double PUV) {
  std::vector<int64_t> TTSc_hits_PUV;
  TTSc_hits_PUV.reserve(TTSc_hits.size());
  double hit_time0, vhit_time, dt, dt_min;
  int64_t hit_tick0;
  // loop through TTSc_hits
  for (int i = 0; i < TTSc_hits.size(); ++i) {
    // TTSc hit
    hit_tick0 = TTSc_hits[i];
    hit_time0 = TICKTDC * hit_tick0;
    dt_min = 1e10;
    // loop through TVSc_hits
    for (int j = 0; j < TVSc_hits.size(); ++j) {
      // TVSc time
      // with offset correction
      // dt = detTDC - TScTDC
      vhit_time = TICKTDC * TVSc_hits[j] - toff_TVSc;
      // check if closer in time
      dt = fabs(vhit_time - hit_time0);
      if (dt < dt_min) dt_min=dt;
    }
    // check if we should keep the TSc hit
    if (dt_min < PUV) continue;
    // add to list
    TTSc_hits_PUV.push_back(hit_tick0);
  }

  return TTSc_hits_PUV;
}

std::vector<double> WFD_find_nearest_TTSc(double WFD_time, const std::vector<int64_t>& TTSc_hits) {
  double t0p=1e10, t0n=-1e10, t1p=1e10, t1n=-1e10;
  double dt, dt_p_best=1e10, dt_n_best=-1e10;
  std::vector<double> TTSc_vals;
  TTSc_vals.reserve(4);
  //int64_t hit;
  double hit_time;
  // loop through the TTSc_hits
  for (int i = 0; i < TTSc_hits.size(); ++i) {
    //hit = TTSc_hits[i];
    hit_time = TICKTDC * TTSc_hits[i];
    //dt = WFD_time - (double)hit;
    //hit = (double)TTSc_hits[i];
    dt = WFD_time - hit_time;
    if (dt > 0.) {
      // check if better than t0p
      if (dt < dt_p_best) {
        dt_p_best = dt;
        t0p = hit_time;
        // store previous hit as t1p -- depends if we are at the start of the vector
        if (i > 0) {
          t1p = TTSc_hits[i-1];
        }
        // else is not strictly necessary. leaving for clarity.
        else {
          t1p = 1e10;
        }
      }
    }
    // dt < 0.
    else {
      // check if better than t0n
      if (dt > dt_n_best) {
        dt_n_best = dt;
        t0n = hit_time;
        // store next hit as t1n -- depends if we are at the end of the vector
        if (i < TTSc_hits.size()-1) {
          t1n = TTSc_hits[i+1];
        }
        // else is not strictly necessary. leaving for clarity.
        else {
          t1n = -1e10;
        }
      }
    }
  }
  // fill the vector
  TTSc_vals.push_back(t0p);
  TTSc_vals.push_back(t0n);
  TTSc_vals.push_back(t1p);
  TTSc_vals.push_back(t1n);
  return TTSc_vals;
}

std::vector<double> WFD_find_nearest_Veto(double WFD_time, const std::vector<int64_t>& veto_hits) {
  double t0p=1e10, t0n=-1e10;
  double dt, dt_p_best=1e10, dt_n_best=-1e10;
  std::vector<double> veto_vals;
  veto_vals.reserve(2);
  //int64_t hit;
  double hit_time;
  // loop through the veto_hits
  for (int i = 0; i < veto_hits.size(); ++i) {
    hit_time = TICKTDC * veto_hits[i];
    dt = WFD_time - hit_time;
    if (dt > 0.) {
      // check if better than t0p
      if (dt < dt_p_best) {
        dt_p_best = dt;
        t0p = hit_time;
      }
    }
    // dt < 0.
    else {
      // check if better than t0n
      if (dt > dt_n_best) {
        dt_n_best = dt;
        t0n = hit_time;
      }
    }
  }
  // fill the vector
  veto_vals.push_back(t0p);
  veto_vals.push_back(t0n);
  return veto_vals;
}

// for vetoing pulses coincident with sync pulses in Ge
// WFD_time is raw fTimeStamp (no offsets, we are looking in the same crate)
// Sync_pulses is raw fTimeStamp of sync hits from that crate
std::vector<double> WFD_find_nearest_Sync(double WFD_time, const std::vector<TPulseIsland*>& Sync_pulses, const double TICKWFD_crate) {
  double t0p=1e10, t0n=-1e10;
  double dt, dt_p_best=1e10, dt_n_best=-1e10;
  std::vector<double> Sync_vals;
  Sync_vals.reserve(2);
  //int64_t hit;
  double hit_time;
  // loop through Sync pulses
  for(std::vector<TPulseIsland*>::const_iterator pIter = Sync_pulses.begin(); pIter != Sync_pulses.end(); pIter++){
    //iPulse++;
    // use CF
    // double Timestamp_tick;
    // if ((*pIter)->HasCFTime()) {
    //   Timestamp_tick = (*pIter)->GetTimeStampCF();
    // } else {
    //   Timestamp_tick = (double)(*pIter)->GetTimeStamp();
    // }
    // don't use CF
    double Timestamp_tick = (double)(*pIter)->GetTimeStamp();
    //
    hit_time = TICKWFD_crate * Timestamp_tick;
    dt = WFD_time - hit_time;
    // dt > 0.
    if (dt > 0.) {
      // check if better than t0p
      if (dt < dt_p_best) {
        dt_p_best = dt;
        t0p = hit_time;
      }
    }
    // dt <= 0.
    else {
      // check if better than t0n
      if (dt > dt_n_best) {
        dt_n_best = dt;
        t0n = hit_time;
      }
    }
  }
  // fill the vector
  Sync_vals.push_back(t0p);
  Sync_vals.push_back(t0n);
  return Sync_vals;
}

std::vector<double> WFD_find_nearest_TSync(double WFD_time, const std::vector<int64_t>& TSync_hits, double toff) {
  double t0p=1e10, t0n=-1e10;
  double dt, dt_p_best=1e10, dt_n_best=-1e10;
  std::vector<double> TSync_vals;
  TSync_vals.reserve(2);
  //int64_t hit;
  double hit_time;
  // loop through TSync hits
  for (int i = 0; i < TSync_hits.size(); ++i) {
    hit_time = TICKTDC * TSync_hits[i];
    dt = WFD_time - hit_time;
    // dt > 0.
    if (dt > 0.) {
      // check if better than t0p
      if (dt < dt_p_best) {
        dt_p_best = dt;
        t0p = hit_time;
      }
    }
    // dt < 0.
    else {
      // check if better than t0n
      if (dt > dt_n_best) {
        dt_n_best = dt;
        t0n = hit_time;
      }
    }
  }
  // fill the vector
  TSync_vals.push_back(t0p - toff);
  TSync_vals.push_back(t0n - toff);
  return TSync_vals;
}
