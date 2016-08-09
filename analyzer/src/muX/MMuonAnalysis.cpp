/********************************************************************\

Name:         MMuonAnalysis
Created by:   Frederik

Contents:     Base Analysis module for muon entrances. Makes TMuonHits, set pile up flags and makes histograms.

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <cmath>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TMuonHit.h"
#include "AlCap.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MMuonAnalysis_init(void);
INT  MMuonAnalysis_bor(INT);
INT  MMuonAnalysis(EVENT_HEADER*, void*);

//utility methods

int RawHitsAnalysis(const vector<TPulseIsland*>* pulses,int channel, int ev_nr, std::vector<float>* ped);
int MatchEandT(const vector<TPulseIsland*>* Epulses,const vector<TPulseIsland*>* Tpulses,int Echannel, int Tchannel,std::vector<float>* ped);
std::vector<double> GetTDCTimes(const string& bank);
void MakeTCorrelationHists(std::vector<double>* Hi, std::vector<double>* Lo,TH1* hist);
void MakeHits(std::vector<double>* Hi, std::vector<double>* Lo, std::vector<double>* A);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;



static std::vector<std::string> left_thin_banknames;
static std::vector<std::string> right_thin_banknames;

//const double pedestals[4];
double tDiffLo = -1000.; //for time correlation histograms
double tDiffHi = 1000.;

double coincHiLo = 8.; //+- window to match Hi and Lo
double ppWindow = 1000.;// +- window for pile up protection (lifetime of muZ is O(200ns))
double ppWindowWide = 4000.;// +- wide window for pile up protection

using std::cout;
using std::cerr;
using std::cin;
using std::endl;

using namespace AlCap;

//declare histograms here, don't now why
namespace {
  TH1 *hMuonHitStats;
  TH1 *hmuSCHimuSCLoTDiff;
  TH1 *hmuSCHiAutoCorrelation;
  TH1 *hmuSCHimuSCATDiff;
  TH1 *hmuonHitTimes;
  

}

ANA_MODULE MMuonAnalysis_module =
{
	"MMuonAnalysis",                    /* module name           */
	"Frederik",              /* author                */
	MMuonAnalysis,                      /* event routine         */
	MMuonAnalysis_bor,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MMuonAnalysis_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};


/** This method initializes histograms.
*/

INT MMuonAnalysis_init()
{
  //
  //  Init histos
  //
  
 
  //stats histogram
  hMuonHitStats = new TH1I("hMuonHitStats","Muon hit stats",16,0.5,16.5);
  hMuonHitStats->GetXaxis()->SetBinLabel(1,"MuSC Hi triggers");
  hMuonHitStats->GetXaxis()->SetBinLabel(2,"MuSC Lo triggers");
  hMuonHitStats->GetXaxis()->SetBinLabel(3,"MuSCA triggers");
  hMuonHitStats->GetXaxis()->SetBinLabel(4,"Hit objects");
  hMuonHitStats->GetXaxis()->SetBinLabel(5,"Hi (muon) Hits");
  hMuonHitStats->GetXaxis()->SetBinLabel(6,"Hi PP Hits");
  hMuonHitStats->GetXaxis()->SetBinLabel(7,"Hi & A PP Hits");
  hMuonHitStats->GetXaxis()->SetBinLabel(8,"Hi & A & Lo PP Hits");
  hMuonHitStats->GetXaxis()->SetBinLabel(9,"good entrances");
  hMuonHitStats->GetXaxis()->SetBinLabel(10,"Wide Hi PP Hits");
  hMuonHitStats->GetXaxis()->SetBinLabel(11,"Wide Hi & A PP Hits");
  hMuonHitStats->GetXaxis()->SetBinLabel(12,"Wide Hi & A & Lo PP Hits");
  
  
  
  
  hmuSCHimuSCLoTDiff = new TH1F("hmuSCHimuSCLoTDiff","muSCHi muSCLo TDiff; tDiff(ns)",2000,tDiffLo,tDiffHi);
  hmuSCHimuSCATDiff = new TH1F("hmuSCHimuSCATDiff","muSCHi muSCA TDiff; tDiff(ns)",2000,tDiffLo,tDiffHi);
  hmuSCHiAutoCorrelation = new TH1F("hmuSCHiAutoCorrelation","muSCHi Autocorrelation; tDiff(ns)",2000,tDiffLo,tDiffHi);
  //TF1 *hmuSCHiAutoCorrelation;
  //TF1 *hmuSCHimuSCATDiff;
  
  hmuonHitTimes = new TH1I("hmuonHitTimes","muon hit times; time(ns)", 1000, 0., 100000000);

  return SUCCESS;
}

// Resets the histograms at the beginning of each run so that the online display updates
INT MMuonAnalysis_bor(INT run_number)
{
  return SUCCESS;

}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MMuonAnalysis(EVENT_HEADER *pheader, void *pevent)
{
  // Get the event number
  int midas_event_number = pheader->serial_number;

 //detector names
  const string muSCHi_name("T402");
  const string muSCLo_name("T408");
  const string muSCA_name("T409");
  
  vector<double> muSCHi = GetTDCTimes(muSCHi_name);
  vector<double> muSCLo = GetTDCTimes(muSCLo_name);
  vector<double> muSCA = GetTDCTimes(muSCA_name);
  
  hMuonHitStats->SetBinContent(1,muSCHi.size() + hMuonHitStats->GetBinContent(1) );
  hMuonHitStats->SetBinContent(2,muSCLo.size()+ hMuonHitStats->GetBinContent(2) );
  hMuonHitStats->SetBinContent(3,muSCA.size() + hMuonHitStats->GetBinContent(3) );
  
 
  MakeTCorrelationHists(&muSCHi,&muSCLo,hmuSCHimuSCLoTDiff);
  MakeTCorrelationHists(&muSCHi,&muSCA,hmuSCHimuSCATDiff);
  MakeTCorrelationHists(&muSCHi,&muSCHi,hmuSCHiAutoCorrelation);
  
  MakeHits(&muSCHi,&muSCLo,&muSCA);
  
  
  //////////////////////////////////////
  return SUCCESS;
}


/*int RawHitsAnalysis(const vector<TPulseIsland*>* pulses,int channel, int ev_nr, std::vector<float>* ped)
{
return 1;
}
*/
//copy from John, 
std::vector<double> GetTDCTimes(const string& bank) 
{
  
  vector<double> times;
  const double tick = AlCap::TICKTDC;
  const map<string, vector<int64_t> >& hit_map = gData->fTDCHitsToChannelMap;
  if (!hit_map.count(bank)) return times;
  
  const vector<int64_t>& hits = hit_map.at(bank);
  times.reserve(hits.size());
  
  for (int ihit = 0; ihit < hits.size(); ++ihit){ times.push_back(tick*hits[ihit]); }
  return times;
}


void MakeTCorrelationHists(std::vector<double>* Hi, std::vector<double>* Lo, TH1* hist)
{
  //Correlation hits
  int ilowest = 0;
  for(unsigned int iHi = 0; iHi < Hi->size(); iHi++)
  {
    double time1 = Hi->at(iHi);

    for(unsigned int iLo = ilowest; iLo < Lo->size(); iLo++ )
    {
      double time2 = Lo->at(iLo);
      double tDiff = time1-time2;
      if(tDiff > tDiffHi) 
      {
        ilowest =iLo;
        continue;
      }
      if(tDiff < tDiffLo)//Don't loop over times which are out of range of the TDiff histogram
      {
        break;
      }
      hist->Fill(tDiff); //std::cout << " tDiff " << tDiff << std::endl;
    }
  }
  return;
}

//Take the 'Lo' as a seed. If there is a matching Hi, take the time of the Hi as the timestamp. 
//The muSCA pileup flage is set as a second step
void MakeHits(std::vector<double>* Hi, std::vector<double>* Lo, std::vector<double>* A)
{
  int ilowest = 0;
  int ilowestA = 0;
  
  gData->ClearMuonHits();
  
  //std::cout << " about to make " << Lo->size() <<  " hits " << std::endl;
  //muSCLo is the seed, match with Hi, and set Lo and Hi pile up flags
  for(unsigned int iLo = 0; iLo < Lo->size(); iLo++)
  {
    TMuonHit hit(iLo);
    double timeLo = Lo->at(iLo);
    hit.SetTime(timeLo);
    if(timeLo < ppWindow) hit.SetBookEnd(true);
    if(iLo == Lo->size()-1) hit.SetBookEnd(true); //allways flag the last one, don`t know where I can get the end of block time
    
    //Lo Pile up flags
    if(iLo > 0) // check the preceeding hit
    {
      if( fabs( Lo->at(iLo-1) - Lo->at(iLo) ) < ppWindow  )  hit.SetMuSCLoPileUp(true);
      if( fabs( Lo->at(iLo-1) - Lo->at(iLo) ) < ppWindowWide  )  hit.SetMuSCLoPileUpWide(true);
    }
    if(iLo < Lo->size() - 1)//check against the next hit
    {
      if( fabs( Lo->at(iLo) - Lo->at(iLo + 1) ) < ppWindow )  hit.SetMuSCLoPileUp(true);
      if( fabs( Lo->at(iLo) - Lo->at(iLo + 1) ) < ppWindowWide )  hit.SetMuSCLoPileUpWide(true);
    }  

    
    for(unsigned int iHi = ilowest; iHi < Hi->size(); iHi++ )
    {
      double timeHi = Hi->at(iHi);
      double tDiff = timeLo - timeHi;
      
      if(tDiff > ppWindowWide) //the Lo time is still to far ahead of the Hi  
      {
        ilowest =iHi;
        continue;
      }
      if(tDiff < -ppWindowWide) { break; } //jumped out of the window
      if( fabs(tDiff) < coincHiLo ) 
      {
        hit.SetLowNotHi(false); //there is a coincident Hi, so it's a propper muon
        hit.SetTime(Hi->at(iHi)); // Take the Hi time if available
      }
      else if ( fabs(tDiff) < ppWindow ) hit.SetMuSCHiPileUp(true);  //add an artificial dead time
      if ( fabs(tDiff) < ppWindowWide && fabs(tDiff) > 40. ) hit.SetMuSCHiPileUpWide(true);  //add an artificial dead time
      
    } //end Hi loop
    
    for(unsigned int iA = ilowestA; iA < A->size(); iA++ ) // do the muSCA's in parallel
    {
      double timeA = A->at(iA);
      double tDiff = timeLo - timeA;
      
      if(tDiff > ppWindowWide) 
      {
        ilowestA =iA;
        continue;
      }
      if(tDiff < -ppWindowWide) { break; }
      if ( fabs(tDiff) < ppWindow ) hit.SetMuSCAPileUp(true);  
      if ( fabs(tDiff) < ppWindowWide ) hit.SetMuSCAPileUpWide(true);  //the 'if' should be redundant
    } //end A loop
           
    //Set Hit
    gData->AddMuonHit(hit);
    
    hmuonHitTimes->Fill(hit.GetTime());
    
    //Set Hit Stats
    
    hMuonHitStats->Fill(4);
    if(hit.IsMuon())
    {
      hMuonHitStats->Fill(5);
      if(!hit.MuSCHiPileUp()) 
      {
        hMuonHitStats->Fill(6);
        if(!hit.MuSCAPileUp())
        {
          hMuonHitStats->Fill(7);
          if(!hit.MuSCLoPileUp()) hMuonHitStats->Fill(8);
        }
      }
      
      if(!hit.MuSCHiPileUpWide()) 
      {
        hMuonHitStats->Fill(10);
        if(!hit.MuSCAPileUpWide())
        {
          hMuonHitStats->Fill(11);
          if(!hit.MuSCLoPileUpWide()) hMuonHitStats->Fill(12);
        }
      }
      
    }
    if(hit.IsPPMuon()) hMuonHitStats->Fill(9);
  } //en Lo loop
    
}
