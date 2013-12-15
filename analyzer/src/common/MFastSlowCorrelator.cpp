/********************************************************************\

Name:         MFastSlowCorrelator
Created by:   Ran Hong

Contents:     A module to find correlated slow pulse for a given fast pulse,
              and plot the height of them in a 2D plot 

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>

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
INT  MFastSlowCorrelator_init(void);
INT  MFastSlowCorrelator(EVENT_HEADER*, void*);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

ANA_MODULE MFastSlowCorrelator_module =
{
	"MFastSlowCorrelator",         /* module name           */
	"Ran Hong",                    /* author                */
	MFastSlowCorrelator,           /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MFastSlowCorrelator_init,      /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

static TH1 **hTime;
static TH2 **hAmplitudeCorrelation;
static TH1 **hSlowCounts;
static TH1 **hTime_PP;
static TH2 **hAmplitudeCorrelation_PP;
static int *last;
static std::vector<std::string> FastDetectors;
static std::vector<std::string> SlowDetectors;
static int nr_fast_detectors; 
static int nr_slow_detectors;
static int nr_detectors;

int FillCoincidenceTimeHistogram(double tFast,double aFast, vector<TPulseIsland*> pulses, 
				 int start, TH1 *h, TH2 *hAmp, double t_low, double t_high,
				 bool PP, TH1 *h_PP, TH2 *hAmp_PP, TH1 *hSC);

bool IsPileupProtected(double t, std::vector<TPulseIsland*> p, double window, int *lPP);

/** This method initializes histograms.
*/
INT MFastSlowCorrelator_init()
{
  gDirectory->mkdir("FastSlowCorrelations")->cd();
  // Let's pair fast and slow channels
  for(std::map<std::string, std::string>::iterator it = gSetup->fBankToDetectorMap.begin();
      it != gSetup->fBankToDetectorMap.end(); ++it){
    if(it->first != std::string("") && it->first != std::string("blank")){
      if (it->second.compare("SiR1-sum-F")==0) continue;
      size_t StrPos = it->second.find("-F");
      if(StrPos!=std::string::npos){
	FastDetectors.push_back(it->second);
//	printf("%s\n",it->second.c_str());
	std::string paired_slow = it->second.substr(0,StrPos);
	paired_slow+="-S";
	for(std::map<std::string, std::string>::iterator it_slow = gSetup->fBankToDetectorMap.begin();
	    it_slow != gSetup->fBankToDetectorMap.end(); ++it_slow){
	  if(it_slow->first != std::string("") && it_slow->first != std::string("blank")){
	    if(it_slow->second.compare(paired_slow)==0){
	      SlowDetectors.push_back(it_slow->second);
//	      printf("%s\n",it_slow->second.c_str());
	    }
	  }
	}
      }
    }
  }

  nr_fast_detectors = FastDetectors.size();
  nr_slow_detectors = SlowDetectors.size();
  if (nr_slow_detectors != nr_fast_detectors){
    printf("Fast and Slow channel numbers don't match!\n");
    return 1;
  }
  nr_detectors = nr_slow_detectors;

  hTime = new TH1*[nr_detectors];
  hTime_PP = new TH1*[nr_detectors];
  hAmplitudeCorrelation = new TH2*[nr_detectors];
  hAmplitudeCorrelation_PP = new TH2*[nr_detectors];
  hSlowCounts = new TH1*[nr_detectors];
  last = new int[nr_detectors];
  
  std::string hName;
  std::string hTitle;

  for(int i = 0; i<nr_detectors; ++i){
    last[i] = 0;

    hName = std::string("h_");
    hName += FastDetectors[i];
    hName += "_"; 
    hName += SlowDetectors[i];
    hName += "_TDiff";
    hTitle = std::string("Tdiff for ");
    hTitle += SlowDetectors[i];
    hTitle += " - ";
    hTitle += FastDetectors[i];
    hTime[i] = new TH1F(hName.c_str(), hTitle.c_str(), 500, -10000, 10000);
    hTime[i]->GetXaxis()->SetTitle("Time difference [ns]");
    hName += "_PP";
    hTitle += " with PP";
    hTime_PP[i] = new TH1F(hName.c_str(), hTitle.c_str(), 500, -10000, 10000);
    hTime_PP[i]->GetXaxis()->SetTitle("Time difference [ns]");
    
    hName = std::string("h_");
    hName += FastDetectors[i];
    hName += "_"; 
    hName += SlowDetectors[i];
    hName += "Amplitude_Correlation";
    hTitle = std::string("Tdiff for ");
    hTitle += SlowDetectors[i];
    hTitle += " - ";
    hTitle += FastDetectors[i];
    printf("%s %s\n",hName.c_str(),hTitle.c_str());
    hAmplitudeCorrelation[i] = new TH2F(hName.c_str(), hTitle.c_str(), 250, 0, 1000,
				   250, 0, 1000);
    hAmplitudeCorrelation[i]->GetXaxis()->SetTitle("Fast Amplitude");
    hAmplitudeCorrelation[i]->GetYaxis()->SetTitle("Slow Amplitude");
    hName += "_PP";
    hTitle += " with PP";
    hAmplitudeCorrelation_PP[i] = new TH2F(hName.c_str(), hTitle.c_str(), 250, 0, 1000,
				      250, 0, 1000);
    hAmplitudeCorrelation_PP[i]->GetXaxis()->SetTitle("Fast Amplitude");
    hAmplitudeCorrelation_PP[i]->GetYaxis()->SetTitle("Slow Amplitude");

    hName = std::string("h_");
    hName += "SlowCounts_";
    hName += FastDetectors[i];
    hTitle = std::string("SlowCounts correspond to ");
    hTitle += FastDetectors[i];
    hSlowCounts[i] = new TH1I(hName.c_str(),hTitle.c_str(),10,0,10);
  }
  gDirectory->cd("/");
  printf("Initialization Complete!\n");
  return SUCCESS;
}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MFastSlowCorrelator(EVENT_HEADER *pheader, void *pevent){
  double t_low = -10000;
  double t_high = 10000;

  // Some typedefs
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  
  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  TStringPulseIslandMap& pulse_islands_map =
    gData->fPulseIslandToChannelMap;
  
  // Let's loop over fast channel times
  int lPP = 0;

  for(int i=0; i<nr_detectors; ++i) last[i] = 0;
  for (int i=0; i<nr_detectors; ++i){
    std::string FastBank = gSetup->GetBankName(FastDetectors[i]);
    std::string SlowBank = gSetup->GetBankName(SlowDetectors[i]);
    lPP = 0;
    for(std::vector<TPulseIsland*>::iterator it_Fast = pulse_islands_map[FastBank].begin(); 
	it_Fast != pulse_islands_map[FastBank].end(); ++it_Fast){

      double tFast = (*it_Fast)->GetPulseTime();
      double aFast = (*it_Fast)->GetPulseHeight();

      bool PP = IsPileupProtected(tFast, pulse_islands_map[FastBank], 10000, &lPP);

      last[i] = FillCoincidenceTimeHistogram(tFast,aFast,pulse_islands_map[SlowBank], 
	    last[i], hTime[i], hAmplitudeCorrelation[i], t_low, t_high,
	    PP, hTime_PP[i], hAmplitudeCorrelation_PP[i],hSlowCounts[i]);
    }
  }
  
  
  return SUCCESS;
}

int FillCoincidenceTimeHistogram(double tFast,double aFast, std::vector<TPulseIsland*> pulses, 
				 int start, TH1 *h, TH2 *hAmp, double t_low, double t_high,
				 bool PP, TH1 *h_PP, TH2 *hAmp_PP, TH1 *hSC)
{
  int i=0;
  for(int p = start; p < (int)pulses.size(); ++p){
    
    double tDiff = pulses[p]->GetPulseTime() - tFast;
    if(tDiff < t_low){
      start = p;
      continue;
    }
    if(tDiff > t_high) break;
    h->Fill(tDiff);
    hAmp->Fill(aFast, pulses[p]->GetPulseHeight());
    i++;
    if(PP){
      h_PP->Fill(tDiff);
      hAmp_PP->Fill(aFast, pulses[p]->GetPulseHeight());
    }
  }
  hSC->Fill(i);
  return start;
}

bool IsPileupProtected(double t, std::vector<TPulseIsland*> p, double window, int *lPP){
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

