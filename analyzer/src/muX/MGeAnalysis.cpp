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
#include <fstream>
#include <algorithm>

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
#include "AlCap.h"

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
int TDCTimeCorrelation(const vector<TPulseIsland*>* pulses,std::vector<double>* times,int channel,std::vector<float>* ped);
int MatchEandT(const vector<TPulseIsland*>* Epulses,const vector<TPulseIsland*>* Tpulses,int Echannel, int Tchannel,std::vector<float>* ped);
std::vector<double> GetTDCTimes_here(const string& bank); // So I can`t have methods with the same name over different modules?
float GetCorrectionFactor(double tDiff,double amp,int channel);
float BilinearInterpolation(float q11, float q12, float q21, float q22, float x1, float x2, float y1, float y2, float x, float y);


extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

static std::vector<std::string> left_thin_banknames;
static std::vector<std::string> right_thin_banknames;

//Wide Window
double tWide_Window = 50000;

//cuts for tdc-E pulse matches
double ge1_ECut = 9000; //abobe 9000 channels, a straight cut
double ge1_TCutLow = -19975;
double ge1_TCutHigh = -19670;
double ge1_a = -30.612244898;
double ge1_b = -593142.857143;

double ge1_TCutHighWide = -10900;
double ge1_TCutLowWide = -13200;

double ge2_ECut = 9000; //abobe 900 channels, a straight cut
double ge2_TCutLow = -20585;
double ge2_TCutHigh = -20140;
double ge2_a = -33.5483870968;
double ge2_b = -667661.290323;

double ge2_TCutHighWide = -11000;
double ge2_TCutLowWide = -14600;

//pc stands for pedestal correction. Stores pedestal corrections
std::vector<float> ge1_pc_energy;
std::vector<float> ge1_pc_time;
std::vector<float> ge1_pc_pedestal;

std::vector<float> ge2_pc_energy;
std::vector<float> ge2_pc_time;
std::vector<float> ge2_pc_pedestal;


//const double pedestals[4];

using std::cout;
using std::cerr;
using std::cin;
using std::endl;

//declare histograms here, don't now why
namespace {
  TH2 *hGeE;
  TH2 *hGe2EVersusTDiffHits;
  TH3 *hGe2PedestalVersusTDiffHits;
  TH3 *hGe1PedestalVersusTDiffHits;
  TH2 *hGe1EVsT;
  TH2 *hGe2EVsT;
  TH1 *hGeHitStats;
  TH1 *hTriggersVsBlock;
  TH2 *hGePed;
  TH3 *hGeShape;
  TH2* hGe1MaxSampleVsE;
  TH2* hGe2MaxSampleVsE;
  TH2 *hGeHitTimes;
  TH2 *hGeHitTimeVersusAmplitude;
  TH3* hGeEVersusTDCTDiff;
  
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
  
  hGe2EVersusTDiffHits = new TH2F("hGe2EVersusTDiffHits","Ge2 pulse height spectrum versus detector history; Pulse amplitude (ADC); Time difference (us)",4250,0.,17000.,150,0,300000);
  hGe2PedestalVersusTDiffHits = new TH3F("hGe2PedestalVersusTDiffHits","Ge2 pulse pedestal versus detector history; Pulse pedestal (ADC); Time difference (us); previous pulse amplitude (ADC)",300,400.,1000.,150,0,300000,20,0,17000);
  hGe1PedestalVersusTDiffHits = new TH3F("hGe1PedestalVersusTDiffHits","Ge1 pulse pedestal versus detector history; Pulse pedestal (ADC); Time difference (us); previous pulse amplitude (ADC)",300,400.,1000.,150,0,300000,20,0,17000);
  
  hGeEVersusTDCTDiff = new TH3F("hGeEVersusTDCTDiff","Energy versus Tdiff with TDC signal; Pulse Amplitude (ADC); TDiff (ns); Detector",170,0.,17000,5000,-25000,25000,2,0.5,2.5);
  
  
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
  hGeHitStats = new TH1I("hGeHitStats","Ge hit stats",20,0.5,20.5);
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
  hGeHitStats->GetXaxis()->SetBinLabel(13,"Ge1 TDC triggers");
  hGeHitStats->GetXaxis()->SetBinLabel(14,"Ge2 TDC triggers");
  hGeHitStats->GetXaxis()->SetBinLabel(15,"Ge1 TDC Hits");
  hGeHitStats->GetXaxis()->SetBinLabel(16,"Ge2 TDC Hits");
  hGeHitStats->GetXaxis()->SetBinLabel(17,"Ge1 TDC Hits PP");
  hGeHitStats->GetXaxis()->SetBinLabel(18,"Ge2 TDC Hits PP");


  
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
    
  hGeHitTimes = new TH2I("hGeHitTimes","germanium hit times; time(ns)", 1000, 0., 100000000,4,0.5,4.5);
  hGeShape->GetYaxis()->SetBinLabel(1,"Ge1 hits");
  hGeShape->GetZaxis()->SetBinLabel(2,"Ge2 hits");
  hGeShape->GetZaxis()->SetBinLabel(3,"Ge1 tdc hits");
  hGeShape->GetZaxis()->SetBinLabel(4,"Ge2 tdc hits");  

  hGeHitTimeVersusAmplitude = new TH2F("hGeHitTimeVersusAmplitude","Hit time within the island versus E pulse amplitude; time (ns) ; Pulse Height (ADC)",100,0,5000,170,0,17000);
  
  //read values of pedestal file
  string name = "ge1_pedestalshift.dat";
  std::ifstream file1(name.c_str());
  if(file1.is_open())
  {
    float a,b,c;
    while(file1 >> a >> b >> c)
    {
      //std::cout << "a " << a << " b " << b << " c " << c << std::endl;
      ge1_pc_energy.push_back(a);
      ge1_pc_time.push_back(b);
      ge1_pc_pedestal.push_back(c);
    }
  }
  
  file1.close();
  
  name="ge2_pedestalshift.dat";
  std::ifstream file2(name.c_str());
  if(file2.is_open())
  {
    float a,b,c;
    while(file2 >> a >> b >> c)
    {
      //std::cout << "a " << a << " b " << b << " c " << c << std::endl;
      ge2_pc_energy.push_back(a);
      ge2_pc_time.push_back(b);
      ge2_pc_pedestal.push_back(c);
    }
  }
  
  file2.close();
  

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
  
  const string ge1TDC_name("T410");
  const string ge2TDC_name("T411");
  
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
  
  vector<double> ge1TDC = GetTDCTimes_here(ge1TDC_name);
  vector<double> ge2TDC = GetTDCTimes_here(ge2TDC_name);

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
  gData->ClearGeHitsTDC();
  
  TDCTimeCorrelation(&ge1E_pulses,&ge1TDC,1,&blockPedestal);
  TDCTimeCorrelation(&ge2E_pulses,&ge2TDC,2,&blockPedestal);
  
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
  
  float previousTime = 0.;
  float previousAmplitude = 0.;
  
  float previousTimeGe1 = 0.;
  float previousAmplitudeGe1 = 0.;
  
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
    
    if(channel ==2 && pulses->at(i)->GetPulseLength()==300 )
    {
      float tDiff = pulses->at(i)->GetPulseTime()-previousTime;
      TGeHit hit = TGeHit(2);
      hit.SetEPulse(pulses->at(i));
      hit.PulseShapeAnalysis();
      if(pulses->at(i)->GetPeakSample() > 70 && pulses->at(i)->GetPeakSample() < 180 && amplitude > 3000. && previousAmplitude > 0 && pulses->at(i)->GetPulseLength()==300 && hit.Shape())
      {
        hGe2PedestalVersusTDiffHits->Fill(pedestal,tDiff,previousAmplitude);
        //printf("fill\n");
      }
      //if (previousAmplitude < 2600. ) cout << previousAmplitude << endl;
      if(amplitude > 2510. ) //determined from the hGeE spectrum. above 2500 channels hGe2 has triggered
      {
         hGe2EVersusTDiffHits->Fill(amplitude,tDiff);
         previousTime = pulses->at(i)->GetPulseTime();
         previousAmplitude = amplitude;
      }
    }
    
    if(channel ==1 && pulses->at(i)->GetPulseLength()==300 )
    {
      float tDiff = pulses->at(i)->GetPulseTime()-previousTimeGe1;
      TGeHit hit = TGeHit(1);
      hit.SetEPulse(pulses->at(i));
      hit.PulseShapeAnalysis();
      if(pulses->at(i)->GetPeakSample() > 70 && pulses->at(i)->GetPeakSample() < 180 && amplitude > 3000. && previousAmplitudeGe1 > 0 && pulses->at(i)->GetPulseLength()==300 && hit.Shape())
      {
        hGe1PedestalVersusTDiffHits->Fill(pedestal,tDiff,previousAmplitudeGe1);
        //printf("fill\n");
      }
      //if (previousAmplitude < 2600. ) cout << previousAmplitude << endl;
      if(amplitude > 2510. ) //determined from the hGeE spectrum. above 2500 channels hGe2 has triggered
      {
         previousTimeGe1 = pulses->at(i)->GetPulseTime();
         previousAmplitudeGe1 = amplitude;
      }
    }
    
    
    
    sum+=pedestal;
  }
  
  ped->at(channel-1)=sum/(1.*size);
  
  return 1;
}


int TDCTimeCorrelation(const vector<TPulseIsland*>* pulses,std::vector<double>* times,int channel, std::vector<float>* ped)
{
  
  int iLowest = 0;
  double lastETime = 0.;

  for(unsigned int iTDC = 0; iTDC < times->size(); iTDC++ )
  {
    hGeHitStats->Fill(12+channel);
    double tdcTime = times->at(iTDC);
    for(unsigned int iGe = iLowest; iGe < pulses->size(); iGe++)
    {
      
      TPulseIsland* pulse = pulses->at(iGe); 
      double slowTime = pulse->GetClockTickInNs() * ( pulse->GetTimeStamp() );
      double tDiff = tdcTime - slowTime;

      if( tDiff > tWide_Window) //tdc hit leading the ge pulse
      {
        iLowest = iGe;
        continue;
      }
      if( tDiff < -tWide_Window ) { break; }

      double energy = pulse->GetPulseHeight();
      hGeEVersusTDCTDiff->Fill(energy,tDiff,channel);
      
      
      //so now we have a candidate match
      bool makehit = false;
      
      /*if(channel == 1 && tDiff > ge1_TCutLow)
      {
        if(energy > ge1_ECut && tDiff < ge1_TCutHigh) makehit = true;
        else if(energy < ge1_a*tDiff + ge1_b) makehit = true;
      }
      
      if(channel == 2 && tDiff > ge2_TCutLow )
      {
        if(energy > ge2_ECut && tDiff < ge2_TCutHigh) makehit = true;
        else if(energy < ge1_a*tDiff + ge1_b) makehit = true;
      }*/
      
      if(channel == 1 && tDiff > ge1_TCutLowWide && tDiff < ge1_TCutHighWide) { makehit = true; }
      if(channel == 2 && tDiff > ge2_TCutLowWide && tDiff < ge2_TCutHighWide) { makehit = true; }
      
      if(makehit)
      {
        TGeHitTDC hit(channel);
        bool acceptHit = hit.SetEPulse(pulse);
        if(acceptHit)
        {
          hit.PulseShapeAnalysis();
          hit.SetTime(tdcTime);
          if(channel == 1) hit.SetEPedestal(ped->at(0));
          if(channel == 2) hit.SetEPedestal(ped->at(1));
          if(fabs(slowTime - lastETime) < 10. ) { hit.SetSecondPulse(true); }
          else {hGeHitStats->Fill(16+channel);} 
          gData->AddGeHitTDC(hit);
          hGeHitTimes->Fill(tdcTime,channel+2); 
          hGeHitStats->Fill(14+channel);
          lastETime = slowTime;
        }
      }
    }
  }
  
  //
  std::vector<TGeHitTDC>* geHitsTDC = gData->GetGeHitsTDC();
  
  double previousGe1Time = -600000;
  double previousGe2Time = -600000;
  double previousGe1Amplitude = -600000;
  double previousGe2Amplitude =-600000;
  double previousLightningTimeGe1 = -600000; //last big hit which disturbs the electronics for ~ 1ms
  double previousLightningTimeGe2 = -600000; 
  
  for(int iGe = 0; iGe < geHitsTDC->size(); iGe++)
  {
    int channel = geHitsTDC->at(iGe).GetChannel();
    double time = geHitsTDC->at(iGe).GetTime();
    double amplitude =  geHitsTDC->at(iGe).GetEPulseHeight();
    int peakSample = geHitsTDC->at(iGe).GetEPulse()->GetPeakSample();
    bool shape = geHitsTDC->at(iGe).Shape();
    int pulseLength = geHitsTDC->at(iGe).GetEPulse()->GetPulseLength();
    std::vector<int> samples = geHitsTDC->at(iGe).GetEPulse()->GetSamples();
    int lastSample = samples.at(pulseLength-1);
    
    
    if(channel ==1)
    {
      double tDiff = time - previousGe1Time;
      double tDiffLightning = time - previousLightningTimeGe1;
      if(tDiffLightning < 600000) geHitsTDC->at(iGe).SetPostLightningFlag(true);//0.6 ms window where we flag events
      
      //set pedestal correction factor
      double correction = 0.;
      if( tDiff > 0 && tDiff < 300000 && previousGe1Amplitude > 0) 
      {
          correction = GetCorrectionFactor(tDiff,previousGe1Amplitude,1); // ' correction ' is actually the pedestal from the grid file
          //std::cout << " correction " << correction << std::endl;
      }
      if(correction > 1.) geHitsTDC->at(iGe).SetPedestalCorrection( correction - geHitsTDC->at(iGe).GetEFixedPedestal() ); 
  
      //qualify the pulse as a reference for the next pulse and save the parameters
      if(peakSample > 70 && peakSample < 180 && amplitude > 3000. && pulseLength==300 && shape)
      {
        previousGe1Time = time;
        previousGe1Amplitude = amplitude;        
      }
      
      //check if the this is a "lightning" pulse
      if(lastSample > 8500) previousLightningTimeGe1 = time;
    }
    
    
    if(channel ==2)
    {
      double tDiff = time - previousGe2Time;
      double tDiffLightning = time - previousLightningTimeGe2;
      if(tDiffLightning < 600000) geHitsTDC->at(iGe).SetPostLightningFlag(true);//0.6 ms window where we flag events
      //set pedestal correction factor
      double correction = 0.;
      if( tDiff > 0 && tDiff < 300000 && previousGe2Amplitude > 0) correction = GetCorrectionFactor(tDiff,previousGe2Amplitude,2); // ' correction ' is actually the pedestal from the grid file
      if(correction > 1.) 
      {
        geHitsTDC->at(iGe).SetPedestalCorrection( correction - geHitsTDC->at(iGe).GetEFixedPedestal() ); 
        //std::cout << " correction " << correction -  geHitsTDC->at(iGe).GetEFixedPedestal() << std::endl;
      }
  
      //qualify the pulse as a reference for the next pulse and save the parameters
      if(peakSample > 70 && peakSample < 180 && amplitude > 3000. && pulseLength==300 && shape)
      {
        previousGe2Time = time;
        previousGe2Amplitude = amplitude;        
      }
      
      //check if the this is a "lightning" pulse
      if(lastSample > 8500) previousLightningTimeGe2 = time;
    }
  }
  
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
      hGeHitTimes->Fill(hit.GetTime(),Echannel); 
    }
        
    //
   
  }
  
  
  

  return 1;
}

//copy from John, 
std::vector<double> GetTDCTimes_here(const string& bank) 
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

float GetCorrectionFactor(double tDiff,double amp,int channel)
{
  std::vector<float> pc_energy;
  std::vector<float> pc_time;
  std::vector<float> pc_pedestal;
  
  if(channel ==1)
  {
    //check if there is anything there
    if(ge1_pc_energy.size() < 1) return 0.;
    pc_energy = ge1_pc_energy;
    pc_time = ge1_pc_time;
    pc_pedestal = ge1_pc_pedestal;
  }
  if(channel ==2)
  {
    //check if there is anything there
    if(ge2_pc_energy.size() < 1) return 0.;
    pc_energy = ge2_pc_energy;
    pc_time = ge2_pc_time;
    pc_pedestal = ge2_pc_pedestal;
  }
  
  //see if the tDiff and amp is in range of the grid
  float minEnergy = *min_element(pc_energy.begin(), pc_energy.end()); 
  float maxEnergy = *max_element(pc_energy.begin(), pc_energy.end());
  float minTime = *min_element(pc_time.begin(), pc_time.end()); 
  float maxTime = *max_element(pc_time.begin(), pc_time.end());
  
  if(tDiff < minTime || tDiff > maxTime || amp < minEnergy || amp > maxEnergy) return 0.;
  
  //look for the 4 corner points for the bilinear interpolation 
  //assume that the vector are order first by energy, then time
  
  int size = pc_energy.size();
  //step 1, get the index for the first energy/amp entry bigger than the input amp
  int step1_index;
  int index1, index2, index3, index4; //4 points for the lin interpolation
  for(int i = 0; i<size; i++)
  {
    step1_index = i;
    if(amp<pc_energy.at(i)) break;
  }
  
  //step 2, go down, and get the two point for the low energy
   
  for(int i = step1_index-1; i >= 0; i--)
  {
    index1 = i;
    if( tDiff > pc_time.at(i) ) break;
  }
  
  index2 = index1 +1;
  
  //and the same for the high energy range
  for(int i = step1_index; i < size; i++)
  {
    index4 = i;
    if( tDiff < pc_time.at(i) ) break;
  }
  
  index3 = index4-1;
  
  float x1 = pc_energy.at(index1);
  float x2 = pc_energy.at(index3);
  float y1 = pc_time.at(index1);
  float y2 = pc_time.at(index2);
  
  float value = BilinearInterpolation(pc_pedestal.at(index1),pc_pedestal.at(index2),pc_pedestal.at(index3),pc_pedestal.at(index4),x1,x2,y1,y2,amp,tDiff);
  //std::cout << "value " << value << "  amp " << amp << " tDiff " << tDiff <<  std::endl;
  
  return value;
}

float BilinearInterpolation(float q11, float q12, float q21, float q22, float x1, float x2, float y1, float y2, float x, float y) 
{
    float x2x1, y2y1, x2x, y2y, yy1, xx1;
    x2x1 = x2 - x1;
    y2y1 = y2 - y1;
    x2x = x2 - x;
    y2y = y2 - y;
    yy1 = y - y1;
    xx1 = x - x1;
    return 1.0 / (x2x1 * y2y1) * (
        q11 * x2x * y2y +
        q21 * xx1 * y2y +
        q12 * x2x * yy1 +
        q22 * xx1 * yy1
    );
}
