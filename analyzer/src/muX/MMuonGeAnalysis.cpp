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
void MakeGeVsMuonTimeHists(std::vector<TMuonHit>* muons, std::vector<TGeHitTDC>* gehits);
void MakeElectronVsMuonTimeHists(std::vector<TMuonHit>* muons,std::vector<TeSCHit>* electrons);
void MakeElectronVsGeTimeHists(std::vector<TGeHitTDC>* gehits,std::vector<TeSCHit>* electrons);
void MakeGeVsGeTimeHists(std::vector<TGeHitTDC>* gehits);
void MakeTripleCoincidenceHists(std::vector<TMuonHit>* muons, std::vector<TGeHitTDC>* gehits);


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
float a_ge1 = 1.66765e-01;
float b_ge1 = -7.720e+01;
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
double tDiffGeHigh = 400.;

//electron veto on ge detector window
double ge1ElLow = 10.;
double ge1ElHigh = 180.;
double ge2ElLow = 30.;
double ge2ElHigh = 200.;

namespace {
  TH2 *hGe1VersusTime;
  TH2 *hGe2VersusTime;
  TH2 *hGe1VersusTimeGoodHit;
  TH2 *hGe2VersusTimeGoodHit;
  TH2 *hGe1VersusTimeGoodHitPP;
  TH2 *hGe2VersusTimeGoodHitPP;
  TH2 *hGe1VersusTimeGoodHitPPElectronVeto;
  TH2 *hGe2VersusTimeGoodHitPPElectronVeto;
  TH2 *hGe1VersusTimeGoodHitPPNoBLR;
  TH2 *hGe2VersusTimeGoodHitPPNoBLR;
  TH2 *hGe1VersusTimeGoodHitPPElectronOpposite;
  TH2 *hGe2VersusTimeGoodHitPPElectronOpposite;
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
  TH3* hGe1EnergyVsGe2EnergyGoodHitsElectronVetoVsMuonTimePP;
  TH1* hTripleStats;
  TH2* hElectronMuonTDiff;
  TH2* hElectronGeTDiff;
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
  
    hGe1VersusTimeGoodHitPPNoBLR = new TH2F("hGe1VersusTimeGoodHitPPNoBLR","Ge1 energy versus time, good ge hits, PP on the muon, no extra BLR ; Ge1 energy (keV) ; tDiff (ns) ",1500.,0.,3000.,nBins,tLow,tHigh);
  hGe2VersusTimeGoodHitPPNoBLR = new TH2F("hGe2VersusTimeGoodHitPPNoBLR","Ge2 energy versus time, good ge hits, PP on the muon, no extra BLR ; Ge2 energy (keV) ; tDiff (ns) ",1500.,0.,3000.,nBins,tLow,tHigh);
  
  hGe1VersusTimeGoodHitPPElectronVeto = new TH2F("hGe1VersusTimeGoodHitPPElectronVeto","Ge1 energy versus time, good ge hits, PP on the muon, veto from electron detector ; Ge1 energy (keV) ; tDiff (ns) ",1500.,0.,3000.,nBins,tLow,tHigh);
  hGe2VersusTimeGoodHitPPElectronVeto = new TH2F("hGe2VersusTimeGoodHitPPElectronVeto","Ge2 energy versus time, good ge hits, PP on the muon, veto from electron detector ; Ge2 energy (keV) ; tDiff (ns) ",1500.,0.,3000.,nBins,tLow,tHigh);
  
    hGe1VersusTimeGoodHitPPElectronOpposite = new TH2F("hGe1VersusTimeGoodHitPPElectronOpposite","Ge1 energy versus time, good ge hits, PP on the muon,electron hit in the opposing detector ; Ge1 energy (keV) ; tDiff (ns) ",1500.,0.,3000.,nBins,tLow,tHigh);
  hGe2VersusTimeGoodHitPPElectronOpposite = new TH2F("hGe2VersusTimeGoodHitPPElectronOpposite","Ge2 energy versus time, good ge hits, PP on the muon, electron hit in the opposing detector ; Ge2 energy (keV) ; tDiff (ns) ",1500.,0.,3000.,nBins,tLow,tHigh);
  
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
  
  hGe1EnergyVsGe2EnergyGoodHitsElectronVetoVsMuonTimePP = new TH3F("hGe1EnergyVsGe2EnergyGoodHitsElectronVetoVsMuonTimePP","Energy Ge1 Versus Energy Ge2, time coincidence, Vs Muon Tme (Good Ge hits + muon PP); Ge1 Energy (keV); Ge2 Energy (keV); muonTime - Ge1Time",1500,0.,3000.,1500,0.,3000.,40,-1000,1000); // big histo!
  
    //stats histogram
  hTripleStats = new TH1I("hTripleStats","Ge-muon hit stats",20,0.5,20.5);
  hTripleStats->GetXaxis()->SetBinLabel(1,"muon hits");
  hTripleStats->GetXaxis()->SetBinLabel(2,"Ge1 hits");
  hTripleStats->GetXaxis()->SetBinLabel(3,"Ge2 hits");
  hTripleStats->GetXaxis()->SetBinLabel(4,"Ge1 Low not Hi E coinc");
  hTripleStats->GetXaxis()->SetBinLabel(5,"Ge2 Low not Hi E coinc");
  hTripleStats->GetXaxis()->SetBinLabel(6,"Ge1 Hi not Low E coinc");
  hTripleStats->GetXaxis()->SetBinLabel(7,"Ge2 Hi not Low E coinc");
  hTripleStats->GetXaxis()->SetBinLabel(8,"Ge1 Hi and Low E coinc");
  hTripleStats->GetXaxis()->SetBinLabel(9,"Ge2 Hi and Low E coinc");
  hTripleStats->GetXaxis()->SetBinLabel(10,"Ge1 E veto");
  hTripleStats->GetXaxis()->SetBinLabel(11,"Ge2 E veto");
  
  
  
  hElectronMuonTDiff = new TH2F("hElectronMuonTDiff","Electron Detector Muon time difference; Muon - Electron Time diff (ns)",1000,-tNarrow,tNarrow,4,0.5,4.5);
  hElectronMuonTDiff->GetYaxis()->SetBinLabel(1,"eSC1 Hi and Lo");
  hElectronMuonTDiff->GetYaxis()->SetBinLabel(2,"eSC2 Hi and Lo");
  hElectronMuonTDiff->GetYaxis()->SetBinLabel(3,"eSC1 Lo not Hi");
  hElectronMuonTDiff->GetYaxis()->SetBinLabel(4,"eSC2 Lo not Hi");
  
  hElectronGeTDiff = new TH2F("hElectronGeTDiff","Electron Detector Ge time difference; Germanium - Electron Time diff (ns)",1000,-tNarrow,tNarrow,6,0.5,6.5);
  hElectronGeTDiff->GetYaxis()->SetBinLabel(1,"eSC1 Hi and Lo");
  hElectronGeTDiff->GetYaxis()->SetBinLabel(2,"eSC2 Hi and Lo");
  hElectronGeTDiff->GetYaxis()->SetBinLabel(3,"eSC1 Lo not Hi");
  hElectronGeTDiff->GetYaxis()->SetBinLabel(4,"eSC2 Lo not Hi");
  hElectronGeTDiff->GetYaxis()->SetBinLabel(5,"eSC1 Ge2");
  hElectronGeTDiff->GetYaxis()->SetBinLabel(6,"eSC2 Ge1");

  
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
  std::vector<TeSCHit>* eSCHits = gData->GeteSCHits();
  
  for(int iGe = 0; iGe < geHits->size(); iGe++ )
  {
    if(geHits->at(iGe).GetChannel()==1) hTripleStats->Fill(2);
    if(geHits->at(iGe).GetChannel()==2) hTripleStats->Fill(3);
  }
  
  
  std::sort(geHits->begin(), geHits->end(),TGeHit::TimeSortHits());
  std::sort(geHitsTDC->begin(), geHitsTDC->end(),TGeHitTDC::TimeSortHits());
  std::sort(muonHits->begin(), muonHits->end(),TMuonHit::TimeSortHits());
  std::sort(eSCHits->begin(), eSCHits->end(),TeSCHit::TimeSortHits());


  MakeElectronVsMuonTimeHists(muonHits,eSCHits);
  MakeElectronVsGeTimeHists(geHitsTDC,eSCHits);//sets electron veto flag for the ge detectors, so has to be called first
  MakeGeVsMuonTimeHists(muonHits,geHitsTDC);
  MakeGeVsGeTimeHists(geHitsTDC);
  MakeTripleCoincidenceHists(muonHits,geHitsTDC);
  
 
  
  //////////////////////////////////////
  return SUCCESS;
}

void MakeGeVsMuonTimeHists(std::vector<TMuonHit>* muons,  std::vector<TGeHitTDC>* gehits)
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
      if(muons->at(iMuon+1).IsPPMuonWithLo()	) hmuSCHitAutoCorrelationPP->Fill(timeMuon-muons->at(iMuon+1).GetTime());
    }
    
    for(int iGe = ilowest; iGe < geSize; iGe++ )
    {
      double timeGe = gehits->at(iGe).GetTime();
      //timeGe = timeGe + offSet; // correction for the large TPC-WFD time offset
      double tDiff = timeMuon - timeGe;
      int channel = gehits->at(iGe).GetChannel(); 
      double energy;
      double energyNoBLR;
      
      if(channel==1) { energy = gehits->at(iGe).GetEnergy(a_ge1,b_ge1);}
      if(channel==2) { energy = gehits->at(iGe).GetEnergy(a_ge2,b_ge2);}
      
      if(channel==1) { energyNoBLR = gehits->at(iGe).GetEnergy(a_ge1,b_ge1,true);}
      if(channel==2) { energyNoBLR = gehits->at(iGe).GetEnergy(a_ge2,b_ge2,true);}
      
      if(tDiff > tWide) //the muon time is still to far ahead of the Ge hit  
      {
        ilowest =iGe;
        continue;
      }
      if(tDiff < -tWide) { break; } //jumped out of the window

      if( fabs(tDiff) < tWide ) //ge hit found for muon-ge histogram 
      {
        if(channel==1) { hGe1VersusTime->Fill(energy,tDiff); }
        if(channel==2) { hGe2VersusTime->Fill(energy,tDiff);}
        
        //std::cout << "ped correction " << gehits->at(iGe).GetPedestalCorrection() << " channel : " << channel << std::endl;
        
        if(channel==1 && gehits->at(iGe).GoodHit() ) hGe1VersusTimeGoodHit->Fill(energy,tDiff);
        if(channel==2 && gehits->at(iGe).GoodHit() ) hGe2VersusTimeGoodHit->Fill(energy,tDiff);
        
        if(channel==1 && gehits->at(iGe).GoodHit() && muons->at(iMuon).IsPPMuon() ) hGe1VersusTimeGoodHitPP->Fill(energy,tDiff);
        if(channel==2 && gehits->at(iGe).GoodHit() && muons->at(iMuon).IsPPMuon() ) hGe2VersusTimeGoodHitPP->Fill(energy,tDiff);
        
        if(channel==1 && gehits->at(iGe).GoodHit() && muons->at(iMuon).IsPPMuon() ) hGe1VersusTimeGoodHitPPNoBLR->Fill(energyNoBLR,tDiff);
        if(channel==2 && gehits->at(iGe).GoodHit() && muons->at(iMuon).IsPPMuon() ) hGe2VersusTimeGoodHitPPNoBLR->Fill(energyNoBLR,tDiff);
        
        if(channel==1 && gehits->at(iGe).GoodHit() && muons->at(iMuon).IsPPMuon() && !gehits->at(iGe).GetElectronCoincidence() ) hGe1VersusTimeGoodHitPPElectronVeto->Fill(energy,tDiff);
        if(channel==2 && gehits->at(iGe).GoodHit() && muons->at(iMuon).IsPPMuon() && !gehits->at(iGe).GetElectronCoincidence() ) hGe2VersusTimeGoodHitPPElectronVeto->Fill(energy,tDiff);
        
        if(channel==1 && gehits->at(iGe).GoodHit() && muons->at(iMuon).IsPPMuon() && gehits->at(iGe).GetOppositeElectronCoincidence() ) hGe1VersusTimeGoodHitPPElectronOpposite->Fill(energy,tDiff);
        if(channel==2 && gehits->at(iGe).GoodHit() && muons->at(iMuon).IsPPMuon() && gehits->at(iGe).GetOppositeElectronCoincidence() ) hGe2VersusTimeGoodHitPPElectronOpposite->Fill(energy,tDiff);
        
      }
    } //end ge loop    
  } //end muon loop
  
}

void MakeGeVsGeTimeHists(std::vector<TGeHitTDC>* gehits)
{
  
}

void MakeElectronVsMuonTimeHists(std::vector<TMuonHit>* muons,std::vector<TeSCHit>* electrons)
{
  for(unsigned int iMuon = 0; iMuon < muons->size(); iMuon++)
  {
    double timeMuon = muons->at(iMuon).GetTime();
    unsigned int iLowest = 0;
    
    for(unsigned int iElectron = iLowest; iElectron < electrons->size(); iElectron++)
    {
      double timeElectron = electrons->at(iElectron).GetTime();
      int channel = electrons->at(iElectron).GetChannel();
      double tDiff = timeMuon - timeElectron;
      
      if(tDiff > tNarrow)
      {
        iLowest = iElectron;
        continue;
      }
      if(tDiff < -tNarrow) break;
      
      if(electrons->at(iElectron).LowAndHi()) 
      {
        hElectronMuonTDiff->Fill(tDiff,0+channel);
      }
      
      if(electrons->at(iElectron).LowNotHi()) 
      {
        hElectronMuonTDiff->Fill(tDiff,2+channel);
      }                  
    }
    
  }
}



void MakeElectronVsGeTimeHists(std::vector<TGeHitTDC>* gehits,std::vector<TeSCHit>* electrons)
{
  for(unsigned int iGe = 0; iGe < gehits->size(); iGe++)
  {
    double timeGe = gehits->at(iGe).GetTime();
    int geChannel = gehits->at(iGe).GetChannel();
    
    unsigned int iLowest = 0;
    
    for(unsigned int iElectron = iLowest; iElectron < electrons->size(); iElectron++)
    {
      double timeElectron = electrons->at(iElectron).GetTime();
      int elChannel = electrons->at(iElectron).GetChannel();
      double tDiff = timeGe - timeElectron;
      
      if(tDiff > tNarrow)
      {
        iLowest = iElectron;
        continue;
      }
      if(tDiff < -tNarrow) break;
      
      if(geChannel == elChannel)
      {
        if(electrons->at(iElectron).LowAndHi()) 
        {
          hElectronGeTDiff->Fill(tDiff,0+elChannel);
          hTripleStats->Fill(7+geChannel);
 
        }
        if(electrons->at(iElectron).LowNotHi()) 
        {
          hTripleStats->Fill(3+geChannel);
          hElectronGeTDiff->Fill(tDiff,2+elChannel);
        }
        if(electrons->at(iElectron).HiNotLow())
        { 
          hTripleStats->Fill(5+geChannel);        
        }
        double low, high;
        if( geChannel==1 ) { low = ge1ElLow; high = ge1ElHigh; }
        if( geChannel==2 ) { low = ge2ElLow; high = ge2ElHigh; }
        if(tDiff > low && tDiff < high) {  gehits->at(iGe).SetElectronCoincidence(true); hTripleStats->Fill(9+geChannel); }//cout << "set veto, tDiff " << tDiff << endl; }
          //else cout << "no veto" << endl;
                  
      }
      if(geChannel != elChannel)
      {
        hElectronGeTDiff->Fill(tDiff,4+geChannel);
        double low, high;
        if( geChannel==1 ) { low = ge1ElLow; high = ge1ElHigh; }
        if( geChannel==2 ) { low = ge2ElLow; high = ge2ElHigh; }
        if(tDiff > low && tDiff < high) {  gehits->at(iGe).SetOppositeElectronCoincidence(true); }
      }
      
    }
                
  }
      
}


void MakeTripleCoincidenceHists(std::vector<TMuonHit>* muons, std::vector<TGeHitTDC>* gehits)
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
      
      if( fabs(tDiff) < tNarrow)
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
            double muonTime = muons->at(iMuon).GetTime();//-offSet;
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
                  if(gehits->at(iGe1).GoodHit() && gehits->at(iGe2).GoodHit() && muons->at(iMuon).IsPPMuon() && ( !gehits->at(iGe2).GetElectronCoincidence() || !gehits->at(iGe1).GetElectronCoincidence() ) ) hGe1EnergyVsGe2EnergyGoodHitsElectronVetoVsMuonTimePP->Fill(energy1,energy2,tDiffMuon);
                }
              }              
            }
          }
        }
      } 
    }
  }
  
}


