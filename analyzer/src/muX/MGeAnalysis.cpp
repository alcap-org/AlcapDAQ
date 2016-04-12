/********************************************************************\

Name:         MGeAnalysis
Created by:   Frederik

Contents:     Base Analysis module for HPGe detector. Makes TGeHIts and histograms.

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
#include "TPulseIsland.h"
#include "TGeHit.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

/*-- Module declaration --------------------------------------------*/
INT  MGeAnalysis_init(void);
INT  MGeAnalysis_bor(INT);
INT  MGeAnalysis(EVENT_HEADER*, void*);

//utility methods

int RawHitsAnalysis(const vector<TPulseIsland*>* pulses,int channel, int ev_nr, std::vector<float>* ped);
int MatchEandT(const vector<TPulseIsland*>* Epulses,const vector<TPulseIsland*>* Tpulses,int Echannel, int Tchannel,std::vector<float>* ped);


extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static std::vector<std::string> left_thin_banknames;
static std::vector<std::string> right_thin_banknames;

//const double pedestals[4];

using std::cout;
using std::cerr;
using std::cin;
using std::endl;

//declare histograms here, don't now why
namespace {
  TH2 *hGeE;
  TH2 *hGe1EVsT;
  TH2 *hGe2EVsT;
  TH1 *hGeHitStats;
  TH1 *hTriggersVsBlock;
  TH2 *hGePed;
  TH3 *hGeShape;
  TH2* hGe1MaxSampleVsE;
  TH2* hGe2MaxSampleVsE;
  TH1 *hGeHitTimes;
  TH2 *hGeHitTimeVersusAmplitude;
}

ANA_MODULE MGeAnalysis_module =
{
	"MGeAnalysis",                    /* module name           */
	"Frederik",              /* author                */
	MGeAnalysis,                      /* event routine         */
	MGeAnalysis_bor,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MGeAnalysis_init,                 /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};


/** This method initializes histograms.
*/

INT MGeAnalysis_init()
{
  //
  //  Init histos
  //
  
  //basic pulse height spectra
  hGeE = new TH2F("hGeE","Ge pulse height spectra; Pulse amplitude (ADC); ",17000,0.,17000.,4,0.5,4.5);
  hGeE->GetYaxis()->SetBinLabel(1, "Ge1, slow channel");
  hGeE->GetYaxis()->SetBinLabel(2, "Ge2, slow channel");
  hGeE->GetYaxis()->SetBinLabel(3, "Ge1, fast channel");
  hGeE->GetYaxis()->SetBinLabel(4, "Ge2, fast channel");
  
  //pedestal histograms
  hGePed = new TH2F("hGePed","Ge pedestals (first n samples); channel ",600,400.,1000.,4,0.5,4.5);
  hGePed->GetYaxis()->SetBinLabel(1, "Ge1, slow channel");
  hGePed->GetYaxis()->SetBinLabel(2, "Ge2, slow channel");
  hGePed->GetYaxis()->SetBinLabel(3, "Ge1, fast channel, -14000");
  hGePed->GetYaxis()->SetBinLabel(4, "Ge2, fast channel, -14000");


  //Time correlation spectra
  hGe1EVsT= new TH2F("hGe1EVsT","Time correlation between fast and slow signal of Ge1; tDiff (ns); Pulse Height (ADC)", 10000,-50000.,+50000.,17000,0,17000);
  hGe2EVsT = new TH2F("hGe1EVsT","Time correlation between fast and slow signal of Ge2; tDiff (ns); Pulse Height (ADC)", 10000,-50000.,+50000.,17000,0,17000);
  
  //stats histogram
  hGeHitStats = new TH1I("hGeHitStats","Ge hit stats",16,0.5,16.5);
  hGeHitStats->GetXaxis()->SetBinLabel(1,"Ge1 slow triggers");
  hGeHitStats->GetXaxis()->SetBinLabel(2,"Ge2 slow triggers");
  hGeHitStats->GetXaxis()->SetBinLabel(3,"Ge1 fast triggers");
  hGeHitStats->GetXaxis()->SetBinLabel(4,"Ge2 fast triggers");
  hGeHitStats->GetXaxis()->SetBinLabel(5,"Ge1 hits");
  hGeHitStats->GetXaxis()->SetBinLabel(6,"Ge2 hits");
  hGeHitStats->GetXaxis()->SetBinLabel(7,"PP Ge1 hits");
  hGeHitStats->GetXaxis()->SetBinLabel(8,"PP Ge2 hits");
  hGeHitStats->GetXaxis()->SetBinLabel(9,"Ge1 EvsT Match hits");
  hGeHitStats->GetXaxis()->SetBinLabel(10,"Ge2 EvsT Match hits");
  hGeHitStats->GetXaxis()->SetBinLabel(11,"Ge1 E shape ok");
  hGeHitStats->GetXaxis()->SetBinLabel(12,"Ge2 E shape ok");

  
  hTriggersVsBlock = new TH1I("hTriggersVsBlock","Ge triggers vs Block Nr; Block Nr; # raw Ge pulses",10000,0.5,10000.5);
  
  //Amplitude versus integral
  hGeShape = new TH3F("hGeShape","Amplitude versus integral; Amplitude ; Integral; ",8500,0,17000,1400,0,1400000,4,0.5,4.5);
  hGeShape->GetZaxis()->SetBinLabel(1,"Ge1 slow hits");
  hGeShape->GetZaxis()->SetBinLabel(2,"Ge2 slow hits");
  hGeShape->GetZaxis()->SetBinLabel(3,"Ge1 fast hits");
  hGeShape->GetZaxis()->SetBinLabel(4,"Ge2 fast hits");  
  
  hGe1EVsT = new TH2F("hGe1EVsT","Amplitude E Vs T channel ; Amplitude (E) ; Amplitude (T)", 8500, 0., 17000., 1700, 0., 17000.);
  hGe2EVsT = new TH2F("hGe2EVsT","Amplitude E Vs T channel ; Amplitude (E) ; Amplitude (T)", 8500, 0., 17000., 1700, 0., 17000.);
  
  hGe1MaxSampleVsE = new TH2F("hGe1MaxSampleVsE"," Max Sample fast channel versus amplitude slow channel ; Max Sample; Amplitude",400,0,400,8500,0.,17000.);
  hGe2MaxSampleVsE = new TH2F("hGe2MaxSampleVsE"," Max Sample fast channel versus amplitude slow channel ; Max Sample; Amplitude",400,0,400,8500,0.,17000.);
  
  /*std::map<std::string, std::string> bank_to_detector_map = gSetup->fBankToDetectorMap;
  for(std::map<std::string, std::string>::iterator mapIter = bank_to_detector_map.begin(); mapIter != bank_to_detector_map.end(); mapIter++) { 

      std::string bankname = mapIter->first;
      std::string detname = gSetup->GetDetectorName(bankname);
    }*/
    
  hGeHitTimes = new TH1I("hGeHitTimes","germanium hit times; time(ns)", 1000, 0., 100000000);

  hGeHitTimeVersusAmplitude = new TH2F("hGeHitTimeVersusAmplitude","Hit time within the island versus E pulse amplitude; time (ns) ; Pulse Height (ADC)",100,0,5000,170,0,17000);

  return SUCCESS;
}

// Resets the histograms at the beginning of each run so that the online display updates
INT MGeAnalysis_bor(INT run_number)
{
  return SUCCESS;

}

/** This method processes one MIDAS block, producing a vector
 * of TOctalFADCIsland objects from the raw Octal FADC data.
 */
INT MGeAnalysis(EVENT_HEADER *pheader, void *pevent)
{
  // Get the event number
  int midas_event_number = pheader->serial_number;

 //detector names, fast and slow channel of the two Ge detectors
  const string ge1E_name("D401");
  const string ge2E_name("D402");
  
  const string ge1T_name("D403");
  const string ge2T_name("D404");
  
  // Some typedefs
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  // Fetch a reference to the gData structure that stores a map
  // of (bank_name, vector<TPulseIsland*>) pairs
  
  TStringPulseIslandMap& pulse_islands_map = gData->fPulseIslandToChannelMap;
  std::map<std::string, std::vector<TPulseIsland*> >& pulses_map = gData->fPulseIslandToChannelMap;
  
  const vector<TPulseIsland*>& ge1E_pulses = pulses_map.at(ge1E_name);
  const vector<TPulseIsland*>& ge2E_pulses = pulses_map.at(ge2E_name);
  const vector<TPulseIsland*>& ge1T_pulses = pulses_map.at(ge1T_name);
  const vector<TPulseIsland*>& ge2T_pulses = pulses_map.at(ge2T_name);
  
  int nPulses = ge1E_pulses.size();
  hTriggersVsBlock->SetBinContent(midas_event_number,nPulses);
  
  //container
  std::vector<float> blockPedestal;
  for(int i = 0 ; i < 4; i++) blockPedestal.push_back(0.);
 // blockPedestal->clear();

  //First loop over the HPGe pulses, no correlations yet
  RawHitsAnalysis(&ge1E_pulses,1,midas_event_number,&blockPedestal);
  RawHitsAnalysis(&ge2E_pulses,2,midas_event_number,&blockPedestal);
  RawHitsAnalysis(&ge1T_pulses,3,midas_event_number,&blockPedestal);
  RawHitsAnalysis(&ge2T_pulses,4,midas_event_number,&blockPedestal);
  
  gData->ClearGeHits();
  MatchEandT(&ge1E_pulses,&ge1T_pulses,1,3,&blockPedestal);
  MatchEandT(&ge2E_pulses,&ge2T_pulses,2,4,&blockPedestal);
	

  //////////////////////////////////////
  return SUCCESS;
}

int RawHitsAnalysis(const vector<TPulseIsland*>* pulses,int channel, int ev_nr, std::vector<float>* ped)
{
  //cout << "number of pulses : " << pulses->size() << endl;
  int size = pulses->size();
  float sum = 0.;
  
  for(int i = 0; i < size; i++)
  {
    hGeHitStats->Fill(channel);
       
    double amplitude = pulses->at(i)->GetPulseHeight();
    double pedestal = pulses->at(i)->GetPedestal(10);
    double integral = pulses->at(i)->GetPulseIntegral();
    
    hGeE->Fill(amplitude,channel);
    hGeShape->Fill(amplitude,integral,channel);
    //if(channel >2) cout << " amplitude " << amplitude << " integral " << integral << endl;
    
    if(channel > 2) pedestal = pedestal - 14000;
    hGePed->Fill(pedestal,channel);
    
    sum+=pedestal;
  }
  
  ped->at(channel-1)=sum/(1.*size);
  
  return 1;
}

//this routing will make the GeHit Objects
int MatchEandT(const vector<TPulseIsland*>* Epulses,const vector<TPulseIsland*>* Tpulses,int Echannel, int Tchannel, std::vector<float>* ped)
{
  int sizeE = Epulses->size();
  int sizeT = Tpulses->size(); 
  
  if(sizeE!=sizeT) 
  {
    cout << "Number of triggers don`t match !!! " << endl;
    return 0; 
  }
  
  for(int i = 0; i < sizeE; i++)
  {
    double Eamplitude = Epulses->at(i)->GetPulseHeight();
    double Tamplitude = Tpulses->at(i)->GetPulseHeight();
    int peakSample = Tpulses->at(i)->GetPeakSample();
    double Epedestal = Epulses->at(i)->GetPedestal();
    double TPedestal = Tpulses->at(i)->GetPedestal();

    
    if(Echannel ==1)
    {
      hGe1EVsT->Fill(Eamplitude,Tamplitude);
      hGe1MaxSampleVsE->Fill(peakSample,Eamplitude);
    }
    if(Echannel==2)
    {
      hGe2EVsT->Fill(Eamplitude,Tamplitude);
      hGe2MaxSampleVsE->Fill(peakSample,Eamplitude);
    }
    
    //Make hits, a E channel hit above threshold triggers a Ge hit object
    
    TGeHit hit(Echannel);
    bool acceptHit = hit.SetEPulse(Epulses->at(i));
    hit.SetTPulse(Tpulses->at(i));
   
    if(acceptHit)
    {
      //Set Flags (pile up, pulse quality, ... )
      double timeInIsland = hit.SearchTPulses(); 
      hit.PulseShapeAnalysis();
      hGeHitTimeVersusAmplitude->Fill(timeInIsland,Eamplitude);
      
      //if(timeInIsland<1) { cout << "pulse size" << Tpulses->at(i)->GetPulseLength() << "  " << hit.Shape() << " " << hit.MatchEVsT() << endl; } 
      //Set previously calculated pedestals
      if(Echannel ==1)
      {
        hit.SetEPedestal(ped->at(0));
        hit.SetTPedestal(ped->at(2));
      }
      if(Echannel==2)
      {
        hit.SetEPedestal(ped->at(1));
        hit.SetTPedestal(ped->at(3));
      }
             
      //Set Hit
      gData->AddGeHit(hit);
            
      //get info from the hits
      hGeHitStats->Fill(4+Echannel);
      if(!hit.SecondPulse()) hGeHitStats->Fill(6+Echannel);
      if(hit.MatchEVsT()) hGeHitStats->Fill(8+Echannel);
      if(hit.Shape()) hGeHitStats->Fill(10+Echannel);
      hGeHitTimes->Fill(hit.GetTime()); 
    }
        
    //
   
  }
  
  
  

  return 1;
}

