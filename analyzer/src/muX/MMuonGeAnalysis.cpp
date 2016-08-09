/********************************************************************\

Name:         MMuonGeAnalysis
Created by:   Frederik

Contents:     Combines TGeHits and TMuonHits to construct Ge-Muon, Ge-Ge, and Ge-Ge-Muon histograms
              after a first pass, should do a run dependent energy calibration, and optimise the e resolution (cut on pedestal?)

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
#include "TGeHit.h"
#include "AlCap.h"
#include <vector> 
#include <algorithm>

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MMuonGeAnalysis_init(void);
INT  MMuonGeAnalysis_bor(INT);
INT  MMuonGeAnalysis(EVENT_HEADER*, void*);

//utility methods
void MakeGeVsMuonTimeHists(std::vector<TMuonHit>* muons, std::vector<TGeHit>* gehits, std::vector<TGeHitTDC>* gehitsTDC);
void MakeGeVsGeTimeHists(std::vector<TGeHit>* gehits);
void MakeTripleCoincidenceHists(std::vector<TMuonHit>* muons, std::vector<TGeHit>* gehits);


extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

using std::cout;
using std::cerr;
using std::cin;
using std::endl;

using namespace AlCap;

//declare histograms here
//
//  * Germanium energy versus muon time
//  * ge1-ge2 coincidence spectra
//  * triple coincidence spectra
//  * coincident ge energy versus cuts

//E calibration
//Ge1
float a_ge1 = 1.6676e-01;
float b_ge1 = -7.720+01;
//Ge2
float a_ge2 = 1.7168e-01;
float b_ge2 = -1.328e+02;

//Wide Window
double tWide = 50000;
//Narrow Window
double tNarrow = 2000.;
//Coincidence Window
double tCoinc = 50;
//offset
double offSet = -15080.;

//Ge coincidence window;
double tDiffGeLow = -400.;
double tDiffGeHigh = 200.;

namespace {
  TH2 *hGe1VersusTime;
  TH2 *hGe2VersusTime;
  TH2 *hGe1VersusTimeGoodHit;
  TH2 *hGe2VersusTimeGoodHit;
  TH2 *hGe1VersusTimeGoodHitPP;
  TH2 *hGe2VersusTimeGoodHitPP;
  TH1* hmuSCHitAutoCorrelationPP;
  TH1* hmuSCHitTimeDifferences;
  TH1 *hmuonHitTimesbis;
  TH2* hGe1EnergyVersusGe1Ge2TDiff;
  TH2* hGe2EnergyVersusGe1Ge2TDiff;
  TH2* hGe1EnergyVsGe2Energy;
  TH2* hGe1EnergyVsGe2EnergyGoodHits;
  TH3* hGe1EnergyVsGe2EnergyVsMuonTime;
  TH3* hGe1EnergyVsGe2EnergyVsMuonTimePP;
  TH3* hGe1EnergyVsGe2EnergyGoodHitsVsMuonTimePP;
}

ANA_MODULE MMuonGeAnalysis_module =
{
	"MMuonGeAnalysis",                    /* module name           */
	"Frederik",              /* author                */
	MMuonGeAnalysis,                      /* event routine         */
	MMuonGeAnalysis_bor,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MMuonGeAnalysis_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};


/** This method initializes histograms.
*/

INT MMuonGeAnalysis_init()
{
  //
  //  Init histos
  //
  
  float tLow = -tNarrow;
  float tHigh = +tNarrow;
  int nBins = int(2*tNarrow/10.);
  
  hGe1VersusTime = new TH2F("hGe1VersusTime","Ge1 energy versus time, no cuts on hits ; Ge1 energy (keV) ; tDiff (ns) ",1500.,0.,3000.,nBins,tLow,tHigh);
  hGe2VersusTime = new TH2F("hGe2VersusTime","Ge2 energy versus time, no cuts on hits ; Ge2 energy (keV) ; tDiff (ns) ",1500.,0.,3000.,nBins,tLow,tHigh);
  
  hGe1VersusTimeGoodHit = new TH2F("hGe1VersusTimeGoodHit","Ge1 energy versus time, good ge hits ; Ge1 energy (keV) ; tDiff (ns) ",1500.,0.,3000.,nBins,tLow,tHigh);
  hGe2VersusTimeGoodHit = new TH2F("hGe2VersusTimeGoodHit","Ge2 energy versus time, good ge hits ; Ge2 energy (keV) ; tDiff (ns) ",1500.,0.,3000.,nBins,tLow,tHigh);
  
  hGe1VersusTimeGoodHitPP = new TH2F("hGe1VersusTimeGoodHitPP","Ge1 energy versus time, good ge hits, PP on the muon ; Ge1 energy (keV) ; tDiff (ns) ",1500.,0.,3000.,nBins,tLow,tHigh);
  hGe2VersusTimeGoodHitPP = new TH2F("hGe2VersusTimeGoodHitPP","Ge2 energy versus time, good ge hits, PP on the muon ; Ge2 energy (keV) ; tDiff (ns) ",1500.,0.,3000.,nBins,tLow,tHigh);
  
  hmuSCHitAutoCorrelationPP = new TH1F("hmuSCHitAutoCorrelationPP","muSCHi nearest neighbour after PP; tDiff(ns)",2000,-2000,2000);
  
    
  hmuSCHitTimeDifferences = new TH1F("hmuSCHitTimeDifferences","tDiff between consequetive muSCHi hits ; tDiff (ns)", 100,-50000,50000);

  hmuonHitTimesbis = new TH1I("hmuonHitTimesbis","muon hit times; time(ns)", 1000, 0., 100000000);
  
  hGe1EnergyVersusGe1Ge2TDiff = new TH2F("hGe1EnergyVersusGe1Ge2TDiff","Ge1 versus Ge1-Ge2 time; energy (keV); time (ns)",1500,0.,3000.,nBins/10,tLow,tHigh);
  hGe2EnergyVersusGe1Ge2TDiff = new TH2F("hGe2EnergyVersusGe1Ge2TDiff","Ge2 versus Ge1-Ge2 time; energy (keV); time (ns)",1500,0.,3000.,nBins/10,tLow,tHigh);
  
  hGe1EnergyVsGe2Energy = new TH2F("hGe1EnergyVsGe2Energy","Energy Ge1 Versus Energy Ge2, time coincidence; Ge1 Energy (keV); Ge2 Energy (keV)",1500,0.,3000.,1500,0.,3000.);
  
  hGe1EnergyVsGe2EnergyGoodHits = new TH2F("hGe1EnergyVsGe2EnergyGoodHits","Energy Ge1 Versus Energy Ge2, time coincidence; Ge1 Energy (keV); Ge2 Energy (keV)",1500,0.,3000.,1500,0.,3000.);
  
  hGe1EnergyVsGe2EnergyVsMuonTime = new TH3F("hGe1EnergyVsGe2EnergyVsMuonTime","Energy Ge1 Versus Energy Ge2, time coincidence, Vs Muon Tme; Ge1 Energy (keV); Ge2 Energy (keV); muonTime - Ge1Time",1500,0.,3000.,1500,0.,3000.,40,-1000,1000); // big histo!
  
    hGe1EnergyVsGe2EnergyVsMuonTimePP = new TH3F("hGe1EnergyVsGe2EnergyVsMuonTimePP","Energy Ge1 Versus Energy Ge2, time coincidence, Vs Muon Time (PP); Ge1 Energy (keV); Ge2 Energy (keV); muonTime - Ge1Time",1500,0.,3000.,1500,0.,3000.,40,-1000,1000); // big histo!
  
  hGe1EnergyVsGe2EnergyGoodHitsVsMuonTimePP = new TH3F("hGe1EnergyVsGe2EnergyGoodHitsVsMuonTimePP","Energy Ge1 Versus Energy Ge2, time coincidence, Vs Muon Tme (Good Ge hits + muon PP); Ge1 Energy (keV); Ge2 Energy (keV); muonTime - Ge1Time",1500,0.,3000.,1500,0.,3000.,40,-1000,1000); // big histo!
  return SUCCESS;
  
}

// Resets the histograms at the beginning of each run so that the online display updates
INT MMuonGeAnalysis_bor(INT run_number)
{
  return SUCCESS;

}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MMuonGeAnalysis(EVENT_HEADER *pheader, void *pevent)
{
  // Get the event number
  int midas_event_number = pheader->serial_number;
  
  //get hits from gData
  std::vector<TMuonHit>* muonHits = gData->GetMuonHits();
  std::vector<TGeHit>* geHits = gData->GetGeHits(); 
  std::vector<TGeHitTDC>* geHitsTDC = gData->GetGeHitsTDC();
  
  for(int iGe = 0; iGe < geHits->size(); iGe++ )
  {
    //if(geHits->at(iGe).GetChannel()==1) cout << " fff " << endl;
  }
  
  
  std::sort(geHits->begin(), geHits->end(),TGeHit::TimeSortHits());
  std::sort(muonHits->begin(), muonHits->end(),TMuonHit::TimeSortHits());


  MakeGeVsMuonTimeHists(muonHits,geHits,geHitsTDC);
  MakeGeVsGeTimeHists(geHits);
  MakeTripleCoincidenceHists(muonHits,geHits);
  
 
  
  //////////////////////////////////////
  return SUCCESS;
}

void MakeGeVsMuonTimeHists(std::vector<TMuonHit>* muons, std::vector<TGeHit>* gehits, std::vector<TGeHitTDC>* gehitsTDC)
{
  int geSize = gehits->size();
  int muonSize = muons->size();
  //std::cout << " muon hit size " << muonSize << endl;
  
  int ilowest = 0;
  
  for(int iMuon = 0; iMuon < muonSize; iMuon++)
  {
    double timeMuon = muons->at(iMuon).GetTime();
    hmuonHitTimesbis->Fill(timeMuon);
    
    if(iMuon>0) { double timebefore = muons->at(iMuon-1).GetTime(); hmuSCHitTimeDifferences->Fill(timeMuon-timebefore) ; }      
    
    if(iMuon > 0 && iMuon < muonSize-1 && muons->at(iMuon).IsPPMuonWithLo())
    {
      if(muons->at(iMuon-1).IsPPMuonWithLo()) hmuSCHitAutoCorrelationPP->Fill(timeMuon-muons->at(iMuon-1).GetTime());
      if(muons->at(iMuon+1).IsPPMuonWithLo()) hmuSCHitAutoCorrelationPP->Fill(timeMuon-muons->at(iMuon+1).GetTime());
    }
    
    for(int iGe = ilowest; iGe < geSize; iGe++ )
    {
      double timeGe = gehits->at(iGe).GetTime();
      timeGe = timeGe + offSet; // correction for the large TPC-WFD time offset
      double tDiff = timeMuon - timeGe;
      int channel = gehits->at(iGe).GetChannel(); 
      double energy; 
      if(channel==1) { energy = gehits->at(iGe).GetEnergy(a_ge1,b_ge1);  }
      if(channel==2) energy = gehits->at(iGe).GetEnergy(a_ge2,b_ge2);
      
      if(tDiff > tWide) //the muon time is still to far ahead of the Ge hit  
      {
        ilowest =iGe;
        continue;
      }
      if(tDiff < -tWide) { break; } //jumped out of the window

      if( fabs(tDiff) < tWide ) //ge hit found for muon-ge histogram 
      {
        if(channel==1) { hGe1VersusTime->Fill(energy,tDiff); }
        if(channel==2) hGe2VersusTime->Fill(energy,tDiff);
        
        if(channel==1 && gehits->at(iGe).GoodHit() ) hGe1VersusTimeGoodHit->Fill(energy,tDiff);
        if(channel==2 && gehits->at(iGe).GoodHit() ) hGe2VersusTimeGoodHit->Fill(energy,tDiff);
        
        if(channel==1 && gehits->at(iGe).GoodHit() && muons->at(iMuon).IsPPMuon() ) hGe1VersusTimeGoodHitPP->Fill(energy,tDiff);
        if(channel==2 && gehits->at(iGe).GoodHit() && muons->at(iMuon).IsPPMuon() ) hGe2VersusTimeGoodHitPP->Fill(energy,tDiff);
      }
    } //end ge loop    
  } //end muon loop
  
}

void MakeGeVsGeTimeHists(std::vector<TGeHit>* gehits)
{
  
}

void MakeTripleCoincidenceHists(std::vector<TMuonHit>* muons, std::vector<TGeHit>* gehits)
{

  int geSize = gehits->size();
  int muonSize = muons->size();
  int ilowestGe = 0;
  int ilowestMuon = 0;
  
  for(int iGe1 = 0; iGe1 < geSize; iGe1++)
  {
    double geTime1 = gehits->at(iGe1).GetTime();
    int channel1 = gehits->at(iGe1).GetChannel();
    
    double energy1 = 0.;
    double energy2 = 0.;
    
    if(channel1==1) energy1 = gehits->at(iGe1).GetEnergy(a_ge1,b_ge1);
    if(channel1==2) energy2 = gehits->at(iGe1).GetEnergy(a_ge2,b_ge2);
    
    for(int iGe2 = ilowestGe; iGe2 < geSize; iGe2++)
    {
      double geTime2 = gehits->at(iGe2).GetTime();
      int channel2 = gehits->at(iGe2).GetChannel();
      
      double tDiff = geTime1-geTime2;      
      
      if(tDiff > tWide)
      {
        ilowestGe = iGe2;
        continue;
      }
      
      if(tDiff < -tWide) break;
      
      if( fabs(tDiff) < tWide)
      {
        //ge ge coincidences
        if( channel1 != channel2 )
        {
          if(channel2==1) energy1 = gehits->at(iGe2).GetEnergy(a_ge1,b_ge1);
          if(channel2==2) energy2 = gehits->at(iGe2).GetEnergy(a_ge2,b_ge2);
          
          if(channel1 == 1)
          {
            hGe1EnergyVersusGe1Ge2TDiff->Fill(energy1,tDiff);
            if(tDiff > tDiffGeLow && tDiff < tDiffGeHigh)
            {
              hGe1EnergyVsGe2Energy->Fill(energy1,energy2);
              if(gehits->at(iGe1).GoodHit() && gehits->at(iGe2).GoodHit()) hGe1EnergyVsGe2EnergyGoodHits->Fill(energy1,energy2);
            }
          }
          if(channel2 == 1)
          {
            hGe2EnergyVersusGe1Ge2TDiff->Fill(energy2,-tDiff);
            //avoid double counting, so use channel 1 as a coincidence trigger
            //if(-tDiff > tDiffGeLow && -tDiff < tDiffGeHigh)
            //{
            //  hGe1EnergyVsGe2Energy->Fill(energy1,energy2);
            //  if(gehits->at(iGe1).GoodHit() && gehits->at(iGe2).GoodHit()) hGe1EnergyVsGe2EnergyGoodHits->Fill(energy1,energy2);
            //}
          }
          
          //triple coincidences
          for(int iMuon = ilowestMuon; iMuon < muonSize; iMuon++)
          {
            double muonTime = muons->at(iMuon).GetTime()-offSet;
            double tDiffMuon = muonTime-geTime1;
                        
            if(tDiffMuon < -tWide) // muon hits to far before the germanium hit
            {
              ilowestMuon = iMuon;
              continue;
            }
            if(tDiffMuon > tWide) //jumped over the Ge hit
            {
              break;
            }
            if(fabs(tDiffMuon) < tWide ) //event of interest
            {
              //cout << " tDiff = " << tDiffMuon << endl;
              if(channel1 == 1)
              {
                if(tDiff > tDiffGeLow && tDiff < tDiffGeHigh)
                {
                  hGe1EnergyVsGe2EnergyVsMuonTime->Fill(energy1,energy2,tDiffMuon);
                  if(muons->at(iMuon).IsPPMuon()) hGe1EnergyVsGe2EnergyVsMuonTimePP->Fill(energy1,energy2,tDiffMuon);
                  if(gehits->at(iGe1).GoodHit() && gehits->at(iGe2).GoodHit() && muons->at(iMuon).IsPPMuon()) hGe1EnergyVsGe2EnergyGoodHitsVsMuonTimePP->Fill(energy1,energy2,tDiffMuon);
                }
              }
              
            }
          }
        }
      } 
    }
  }
  
}


