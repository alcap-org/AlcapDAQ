/********************************************************************\

Name:         MMuSCTimeDifferences
Created by:   Peter Winter

Contents:     A module to fill histograms for time differences for muSC hits
              versus each other detector hits to see timing offsets

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <iostream>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH2.h>
#include <TDirectory.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MMuSCTimeDifferences_init(void);
INT  MMuSCTimeDifferences(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

ANA_MODULE MMuSCTimeDifferences_module =
{
	"MMuSCTimeDifferences",        /* module name           */
	"Peter Winter",                /* author                */
	MMuSCTimeDifferences,          /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MMuSCTimeDifferences_init,     /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

static TH1 **hTime;
static TH2 **hAmplitudeVsTime;
static TH1 **hTime_PP;
static TH2 **hAmplitudeVsTime_PP;
static int *last;
static std::vector<std::string> detectors;
static int nr_detectors; 

int FillCoincidenceTimeHistogram(int tMusc, vector<TPulseIsland*> pulses, 
				 int start, TH1 *h, TH2 *hAmp, double t_low, double t_high,
				 bool PP, TH1 *h_PP, TH2 *hAmp_PP);

bool IsPileupProtected(double t,const std::vector<TPulseIsland*>& p, double window, int *lPP);

/** This method initializes histograms.
*/
INT MMuSCTimeDifferences_init()
{
  gDirectory->mkdir("MuSC_TimingCorrelations")->cd();
  // Let's find all detector names other than muSC
  for(std::map<std::string, std::string>::iterator it = gSetup->fBankToDetectorMap.begin();
      it != gSetup->fBankToDetectorMap.end(); ++it){
    if(it->first != std::string("") && it->first != std::string("blank")){
      if(it->second != std::string("muSc")) detectors.push_back(it->second);
    }
  }
  std::cout << "Number of detectors fo Time Correlations: " << detectors.size() << std::endl;
  std::cout << "Size of Bank to Detector map: " << gSetup->fBankToDetectorMap.size() << std::endl;
  std::cout << "Size of TPI map: " << gData->fPulseIslandToChannelMap.size() << std::endl;

  nr_detectors = detectors.size();
  hTime = new TH1*[nr_detectors];
  hTime_PP = new TH1*[nr_detectors];
  hAmplitudeVsTime = new TH2*[nr_detectors];
  hAmplitudeVsTime_PP = new TH2*[nr_detectors];
  last = new int[nr_detectors];
  
  for(int i = 0; i<nr_detectors; ++i){
    last[i] = 0;

    std::string hName("hMuSC_");
    hName += detectors[i];
    hName += "_Timediff";
    std::string hTitle("Tdiff for ");
    hTitle += detectors[i];
    hTitle += " - muSC";
    hTime[i] = new TH1F(hName.c_str(), hTitle.c_str(), 500, -10000, 10000);
    hTime[i]->GetXaxis()->SetTitle("Time difference [ns]");
    hName += "_PP";
    hTitle += " with PP";
    hTime_PP[i] = new TH1F(hName.c_str(), hTitle.c_str(), 500, -10000, 10000);
    hTime_PP[i]->GetXaxis()->SetTitle("Time difference [ns]");
       
    hName = "hMuSC_";
    hName += detectors[i];
    hName += "_AmplitudeVsTdiff";
    hTitle = "Amplitude versus tdiff for ";
    hTitle += detectors[i];
    hTitle += " - muSC";
    hAmplitudeVsTime[i] = new TH2F(hName.c_str(), hTitle.c_str(), 250, -5000, 5000,
				   250, 0, 1000);
    hAmplitudeVsTime[i]->GetXaxis()->SetTitle("Time difference [ns]");
    hAmplitudeVsTime[i]->GetYaxis()->SetTitle("Amplitude");
    hName += "_PP";
    hTitle += " with PP";
    hAmplitudeVsTime_PP[i] = new TH2F(hName.c_str(), hTitle.c_str(), 250, -5000, 5000,
				      250, 0, 1000);
    hAmplitudeVsTime_PP[i]->GetXaxis()->SetTitle("Time difference [ns]");
    hAmplitudeVsTime_PP[i]->GetYaxis()->SetTitle("Amplitude");
  }
  gDirectory->cd("/");

  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MMuSCTimeDifferences(EVENT_HEADER *pheader, void *pevent){
  double t_low = -10000;
  double t_high = 10000;

  // Some typedefs
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  
  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  TStringPulseIslandMap& pulse_islands_map =
    gData->fPulseIslandToChannelMap;
  
  // Let's loop over the muSC times
  std::string str_MuSC = gSetup->GetBankName("muSc");
  int lPP = 0;
  for(int i=0; i<nr_detectors; ++i) last[i] = 0;
  
  for(std::vector<TPulseIsland*>::iterator it_Musc = pulse_islands_map[str_MuSC].begin(); 
      it_Musc != pulse_islands_map[str_MuSC].end(); ++it_Musc){
    
    double tMusc = (*it_Musc)->GetPulseTime();
    
    bool PP = IsPileupProtected(tMusc, pulse_islands_map[str_MuSC], 10000, &lPP);
    
    for(int i=0; i<nr_detectors; ++i){
      last[i] = FillCoincidenceTimeHistogram(tMusc, pulse_islands_map[gSetup->GetBankName(detectors[i])], 
					     last[i], hTime[i], hAmplitudeVsTime[i], t_low, t_high,
					     PP, hTime_PP[i], hAmplitudeVsTime_PP[i]);
    }
  }
  
  return SUCCESS;
}

int FillCoincidenceTimeHistogram(int tMusc, std::vector<TPulseIsland*> pulses, 
				 int start, TH1 *h, TH2 *hAmp, double t_low, double t_high,
				 bool PP, TH1 *h_PP, TH2 *hAmp_PP)
{
  for(int p = start; p < (int)pulses.size(); ++p){
    
    double tDiff = pulses[p]->GetPulseTime() - tMusc;
    if(tDiff < t_low){
      start = p;
      continue;
    }
    if(tDiff > t_high) break;
    h->Fill(tDiff);
    hAmp->Fill(tDiff, pulses[p]->GetPulseHeight());
    if(PP){
      h_PP->Fill(tDiff);
      hAmp_PP->Fill(tDiff, pulses[p]->GetPulseHeight());
    }
  }

  return start;
}

bool IsPileupProtected(double t,const std::vector<TPulseIsland*>& p, double window, int *lPP){
  if(t < window || t > 1.12E8-window) return false; // very basic bookending
  for(int i = *lPP; i < p.size(); ++i){
    double t2 = p[i]->GetPulseTime();
    if(t2 < t-window){
      *lPP = i;
    }
    if(t2 > t+window) break;
    if(t != t2 && t2 > t-window && t2 < t+window) return false;
  }
  return true;
}

