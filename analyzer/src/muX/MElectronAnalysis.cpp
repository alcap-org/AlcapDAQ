/********************************************************************\

Name:         MElectronAnalysis
Created by:   Frederik

Contents:     Base Analysis module for Electron entrances. Makes TElectronHits, set pile up flags and makes histograms.

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
#include "TeSCHit.h"
#include "AlCap.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MElectronAnalysis_init(void);
INT  MElectronAnalysis_bor(INT);
INT  MElectronAnalysis(EVENT_HEADER*, void*);

//utility methods


std::vector<double> GeteSCTDCTimes(const string& bank);
void MakeeSCTCorrelationHists(std::vector<double>* Lo, std::vector<double>* H,TH1* hist);
void MakeHits(std::vector<double>* Lo, std::vector<double>* Hi, int channel);

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;



//const double pedestals[4];
double eSCtDiffLo = -1000.; //for time correlation histograms
double eSCtDiffHi = 1000.;

double eSCcoincHiLo = 8.; //+- window to match Hi and Lo


using std::cout;
using std::cerr;
using std::cin;
using std::endl;

using namespace AlCap;

//declare histograms here, don't now why
namespace {
  TH1 *hElectronHitStats;
  TH2 *hElectronHitTimes;
  TH1* heSC1HiLoTDiff;
  TH1* heSC2HiLoTDiff;
  

}

ANA_MODULE MElectronAnalysis_module =
{
	"MElectronAnalysis",                    /* module name           */
	"Frederik",              /* author                */
	MElectronAnalysis,                      /* event routine         */
	MElectronAnalysis_bor,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MElectronAnalysis_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};


/** This method initializes histograms.
*/

INT MElectronAnalysis_init()
{
  //
  //  Init histos
  //
  
 
  //stats histogram
  hElectronHitStats = new TH1I("hElectronHitStats","Electron hit stats",16,0.5,16.5);
  hElectronHitStats->GetXaxis()->SetBinLabel(1,"eSC 1 Lo triggers");
  hElectronHitStats->GetXaxis()->SetBinLabel(2,"eSC 1 Hi triggers");
  hElectronHitStats->GetXaxis()->SetBinLabel(3,"eSC 2 Lo triggers");
  hElectronHitStats->GetXaxis()->SetBinLabel(4,"eSC 2 Hi triggers");
  hElectronHitStats->GetXaxis()->SetBinLabel(5,"eSC 1 HiandLo hits");
  hElectronHitStats->GetXaxis()->SetBinLabel(6,"eSC 2 HiandLo hits");
  hElectronHitStats->GetXaxis()->SetBinLabel(7,"eSC 1 LoNotHi hits");
  hElectronHitStats->GetXaxis()->SetBinLabel(8,"eSC 2 LoNotHi hits");

  
  
  hElectronHitTimes = new TH2I("hElectronHitTimes","Electron hit times; time(ns)", 1000, 0., 100000000,4,0.5,4.5);
  hElectronHitTimes->GetYaxis()->SetBinLabel(1,"eSC1Lo");
  hElectronHitTimes->GetYaxis()->SetBinLabel(2,"eSC1Hi");
  hElectronHitTimes->GetYaxis()->SetBinLabel(3,"eSC2Lo");
  hElectronHitTimes->GetYaxis()->SetBinLabel(4,"eSC2Hi");
  
  heSC1HiLoTDiff = new TH1F("heSC1HiLoTDiff","Hi and Lo eSC T correlation; time(ns)",2000,-1000,1000);
  heSC2HiLoTDiff = new TH1F("heSC2HiLoTDiff","Hi and Lo eSC T correlation; time(ns)",2000,-1000,1000);

  return SUCCESS;
}

// Resets the histograms at the beginning of each run so that the online display updates
INT MElectronAnalysis_bor(INT run_number)
{
  return SUCCESS;
}


INT MElectronAnalysis(EVENT_HEADER *pheader, void *pevent)
{
  // Get the event number
  int midas_event_number = pheader->serial_number;

 //detector names
  const string eSC1Lo_name("T412");
  const string eSC1Hi_name("T413");
  const string eSC2Lo_name("T414");
  const string eSC2Hi_name("T415");
  
  vector<double> eSC1Lo = GeteSCTDCTimes(eSC1Lo_name);
  vector<double> eSC1Hi = GeteSCTDCTimes(eSC1Hi_name);
  vector<double> eSC2Lo = GeteSCTDCTimes(eSC2Lo_name);
  vector<double> eSC2Hi = GeteSCTDCTimes(eSC2Hi_name);

  
  hElectronHitStats->SetBinContent(1,eSC1Lo.size() + hElectronHitStats->GetBinContent(1) );
  hElectronHitStats->SetBinContent(2,eSC1Hi.size() + hElectronHitStats->GetBinContent(2) );
  hElectronHitStats->SetBinContent(3,eSC2Lo.size() + hElectronHitStats->GetBinContent(3) );
  hElectronHitStats->SetBinContent(4,eSC2Hi.size() + hElectronHitStats->GetBinContent(4) );

  for(unsigned int i = 0; i < eSC1Lo.size(); i++)
  {
    hElectronHitTimes->Fill(eSC1Lo.at(i),1);
  }
  
  for(unsigned int i = 0; i < eSC2Lo.size(); i++)
  {
    hElectronHitTimes->Fill(eSC2Lo.at(i),3);
  }
  
  for(unsigned int i = 0; i < eSC1Hi.size(); i++)
  {
    hElectronHitTimes->Fill(eSC1Hi.at(i),2);
  }
  
  for(unsigned int i = 0; i < eSC2Hi.size(); i++)
  {
    hElectronHitTimes->Fill(eSC2Hi.at(i),4);
  }
  
  
 
  MakeeSCTCorrelationHists(&eSC1Lo,&eSC1Hi,heSC1HiLoTDiff);
  MakeeSCTCorrelationHists(&eSC2Lo,&eSC2Hi,heSC2HiLoTDiff);
  
  gData->CleareSCHits();
  
  MakeHits(&eSC1Hi,&eSC1Lo,1); 
  MakeHits(&eSC2Hi,&eSC2Lo,2);
  
  
  //////////////////////////////////////
  return SUCCESS;
}



//copy from John, 
std::vector<double> GeteSCTDCTimes(const string& bank) 
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
	

void MakeeSCTCorrelationHists(std::vector<double>* Hi, std::vector<double>* Lo, TH1* hist)
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
      if(tDiff > eSCtDiffHi) 
      {
        ilowest =iLo;
        continue;
      }
      if(tDiff < eSCtDiffLo)//Don't loop over times which are out of range of the TDiff histogram
      {
        break;
      }
      hist->Fill(tDiff); //std::cout << " tDiff " << tDiff << std::endl;
    }
  }
  return;
}

//Take the 'Lo' as a seed. If there is a matching Hi, take the time of the Hi as the timestamp. 

void MakeHits(std::vector<double>* Hi, std::vector<double>* Lo,int channel)
{
    unsigned int iLowest = 0;
    
    for(unsigned int iLo = 0; iLo < Lo->size(); iLo++)
    {
      double timeLo = Lo->at(iLo);
      bool foundHi = false;
      
      for(unsigned int iHi = iLowest; iHi < Hi->size(); iHi++)
      {
         double timeHi = Hi->at(iHi);
         double tDiff = timeLo-timeHi;
         if(tDiff > eSCtDiffHi) 
         {
           iLowest = iHi;
           continue;
         }
         if(tDiff < eSCtDiffLo)//Don't loop over times which are out of range of the TDiff histogram
         {
           break;
         }
         if(fabs(tDiff) < eSCcoincHiLo)
         {
           TeSCHit hit(channel);
           foundHi = true;
           hit.SetLowTime(timeLo);
           hit.SetHighTime(timeHi);    
           gData->AddeSCHit(hit);
           if(channel == 1) hElectronHitStats->Fill(5);
           if(channel == 2) hElectronHitStats->Fill(6);
         }
      }
      
      if(!foundHi)
      {
           TeSCHit hit(channel);
           hit.SetLowTime(timeLo);
           gData->AddeSCHit(hit);
           if(channel == 1) hElectronHitStats->Fill(7);
           if(channel == 2) hElectronHitStats->Fill(8);
      }
    }
}
