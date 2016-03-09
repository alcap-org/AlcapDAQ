/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <ios>

/* MIDAS includes */
#include "midas.h"

/* ROOT includes */
#include <TH1.h>
#include <TH2.h>
#include <TDirectory.h>
#include "TFile.h"
#include "TApplication.h"
#include "TROOT.h"
//#include "TMath.h"
//#include "Math/Interpolator.h"

/* AlCap includes */
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TPulseIsland.h"

using namespace std;

/*-- Module declaration --------------------------------------------*/
INT  MPSDIntTest_init(void);
INT  MPSDIntTest(EVENT_HEADER*, void*);
INT  MPSDIntTest_eor(INT);
INT  MPSDIntTest_BookHistograms();

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

std::map<std::string,TH1F*> NeutronBaseline_map, GammaBaseline_map, AltBaseline_map, AltTimestamp_map;
std::map<std::string,TH1I*> NeutronCount_map, GammaCount_map, AltCount_map;
std::map<std::string, TH2F*> StdPSD_map, PedestalPSD_map, BaselinePSD_map;
//std::map<int, TH1D*> NdetDIFoMS_map, NdetUIFoMS_map;
static std::map<std::string, std::ofstream*> waveforms;

bool PSDIntTest_firstEvent = true;
int PSDIntTestND_count = 0, PSDIntTestGD_count = 0, PSDIntTestAD_count = 0;
int PSDIntTestNU_count = 0, PSDIntTestGU_count = 0, PSDIntTestAU_count = 0;

extern TH1F* hDQ_TDCCheck_nMuons;

ANA_MODULE MPSDIntTest_module =
{
	"MPSDIntTest",                 /* module name           */
	"Damien Alexander",            /* author                */
	MPSDIntTest,                   /* event routine         */
	NULL,                          /* BOR routine           */
        MPSDIntTest_eor,               /* EOR routine           */
	MPSDIntTest_init,              /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/////////////////////////////////////////////////////////////////



INT MPSDIntTest_init(){
  return SUCCESS;
}

INT MPSDIntTest_BookHistograms() 
{
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("PSDIntTest")->cd();

  const std::map<std::string, std::vector<TPulseIsland*> >& tpi_map = gData->fPulseIslandToChannelMap;

  for(std::map<std::string, std::vector<TPulseIsland*> >::const_iterator mIter = tpi_map.begin(); mIter != tpi_map.end(); mIter++){
    std::string bankname = mIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);
    if(!gSetup->IsNeutron(detname)) continue; //neutron only

    const std::vector<TPulseIsland*>& pulses = mIter->second;
    std::vector<TPulseIsland*>::const_iterator firstPulse = pulses.begin();
    if(pulses.size() <= 1) continue;
    firstPulse++; // just in case
    std::vector<int> samples = (*firstPulse)->GetSamples();
    int n_samples = samples.size();
    const int max_adc = std::pow(2, gSetup->GetNBits(bankname));

    int max_bin = max_adc * n_samples/20;

    
    std::string histname = "h" + detname + "_Neutron";
    std::string histtitle = detname + " Neutrons over 1.5 MeV";
    TH1I* hNeutronCount = new TH1I(histname.c_str(), histtitle.c_str(), 1000, 0, 0.001);
    hNeutronCount->GetYaxis()->SetTitle("");
    hNeutronCount->GetXaxis()->SetTitle("Count");
    NeutronCount_map[bankname] = hNeutronCount;

    histname = "h" + detname + "_Alt";
    histtitle = detname + " Alternate Waveforms over 1.5 MeV";
    TH1I* hAltCount = new TH1I(histname.c_str(), histtitle.c_str(), 1000, 0, 0.00001);
    hAltCount->GetYaxis()->SetTitle("");
    hAltCount->GetXaxis()->SetTitle("Count");
    AltCount_map[bankname] = hAltCount;

    histname = "h" + detname + "_Gamma";
    histtitle = "Baseline for " + detname + " Gammas over 1.5 MeV";
    TH1I* hGammaCount = new TH1I(histname.c_str(), histtitle.c_str(), 1000, 0, 0.01);
    hGammaCount->GetYaxis()->SetTitle("");
    hGammaCount->GetXaxis()->SetTitle("Count");
    GammaCount_map[bankname] = hGammaCount;

    ////Baselines

    histname = "h" + detname + "_NeutronBase";
    histtitle = "Baseline for " + detname + " Neutrons";
    TH1F* hNeutronBaseline = new TH1F(histname.c_str(), histtitle.c_str(), 700, max_adc - 700, max_adc);
    hNeutronBaseline->GetYaxis()->SetTitle("Count");
    hNeutronBaseline->GetXaxis()->SetTitle("Baseline(adc count)");
    NeutronBaseline_map[bankname] = hNeutronBaseline;

    histname = "h" + detname + "_AltBase";
    histtitle = "Baseline for " + detname + " Alternate Waveformss";
    TH1F* hAltBaseline = new TH1F(histname.c_str(), histtitle.c_str(), 700, max_adc - 700, max_adc);
    hAltBaseline->GetYaxis()->SetTitle("Count");
    hAltBaseline->GetXaxis()->SetTitle("Baseline(adc count)");
    AltBaseline_map[bankname] = hAltBaseline;

    histname = "h" + detname + "_GammaBase";
    histtitle = "Baseline for " + detname + " Gammas";
    TH1F* hGammaBaseline = new TH1F(histname.c_str(), histtitle.c_str(), 700, max_adc - 700, max_adc);
    hGammaBaseline->GetYaxis()->SetTitle("Count");
    hGammaBaseline->GetXaxis()->SetTitle("Baseline(adc count)");
    GammaBaseline_map[bankname] = hGammaBaseline;

    ////PSD plots

    histname = "h" + detname + "_PSD";
    histtitle = "Ratio vs energy for " + detname + " original pedestal";
    TH2F* hStdPSD = new TH2F(histname.c_str(), histtitle.c_str(), max_adc/2, 0, 6.5, 300, 0, 0.5);
    hStdPSD->GetYaxis()->SetTitle("Ratio");
    hStdPSD->GetXaxis()->SetTitle("Energy (MeVee)");
    StdPSD_map[bankname] = hStdPSD;

    histname = "h" + detname + "_PedPSD";
    histtitle = "Ratio vs energy for " + detname + " averaged pedestal";
    TH2F* hPedestalPSD = new TH2F(histname.c_str(), histtitle.c_str(), max_adc/2, 0, 6.5, 300, 0, 0.5);
    hPedestalPSD->GetYaxis()->SetTitle("Ratio");
    hPedestalPSD->GetXaxis()->SetTitle("Energy (MeVee)");
    PedestalPSD_map[bankname] = hPedestalPSD;

    histname = "hDQ_IslandTimestamp_" + detname + "_" + bankname;
    histtitle = "Distribution of time stamps in " + detname;
    TH1F* hDQ_Histogram = new TH1F(histname.c_str(), histtitle.c_str(), n_bins, 0, bin_max);
    hDQ_Histogram->GetXaxis()->SetTitle("Time Stamp [ns]");
    hDQ_Histogram->GetYaxis()->SetTitle("Number of TPulseIslands");
    AltTimestamp_map[bankname] = hDQ_Histogram;

/*
    histname = "h" + detname + "_BaselinePSD";
    histtitle = "Ratio vs energy for " + detname + " static Baseline";
    TH2F* hBaselinePSD = new TH2F(histname.c_str(), histtitle.c_str(), max_adc/2, 0, 6.5, 300, 0, 0.5);
    hBaselinePSD->GetYaxis()->SetTitle("Ratio");
    hBaselinePSD->GetXaxis()->SetTitle("Energy (MeVee)");
    BaselinePSD_map[bankname] = hBaselinePSD; 
*/
  }

  cwd->cd();

  return SUCCESS;
}

INT MPSDIntTest_eor(INT run_number)
{

  float nMuons = hDQ_TDCCheck_nMuons->GetEntries();
  const std::map<std::string, std::vector<TPulseIsland*> >& tpi_map = gData->fPulseIslandToChannelMap;

  for(std::map<std::string, std::vector<TPulseIsland*> >::const_iterator mIter = tpi_map.begin(); mIter != tpi_map.end(); mIter++){
    const std::string bankname = mIter->first;
    const std::string detname = gSetup->GetDetectorName(bankname);
    if(!gSetup->IsNeutron(detname)) continue;

    if(detname == "NdetD"){
      NeutronCount_map[bankname]->Fill(PSDIntTestND_count/nMuons);
      GammaCount_map[bankname]->Fill(PSDIntTestGD_count/nMuons);
      AltCount_map[bankname]->Fill(PSDIntTestAD_count/nMuons);
    }
    if(detname == "NdetU"){
      NeutronCount_map[bankname]->Fill(PSDIntTestNU_count/nMuons);
      GammaCount_map[bankname]->Fill(PSDIntTestGU_count/nMuons);
      AltCount_map[bankname]->Fill(PSDIntTestAU_count/nMuons);
    }
  }

  return SUCCESS;
}

INT MPSDIntTest(EVENT_HEADER *pheader, void *pevent)
{
  if(PSDIntTest_firstEvent == true){
    MPSDIntTest_BookHistograms();
    PSDIntTest_firstEvent = false;
  }

  const std::map<std::string, std::vector<TPulseIsland*> >& tpi_map = gData->fPulseIslandToChannelMap;

  for(std::map<std::string, std::vector<TPulseIsland*> >::const_iterator mIter = tpi_map.begin(); mIter != tpi_map.end(); mIter++){
    const std::string bankname = mIter->first;
    const std::string detname = gSetup->GetDetectorName(bankname);
    if(!gSetup->IsNeutron(detname)) continue;

    const std::vector<TPulseIsland*>& pulses = mIter->second;
    int polarity = gSetup->GetTriggerPolarity(bankname);
    int nbits = gSetup->GetNBits(bankname);
    int max_adc = std::pow(2, nbits);
   
    float pct = 0.1;

    for(std::vector<TPulseIsland*>::const_iterator pIter = pulses.begin(); pIter != pulses.end(); pIter++){

      float integral = 0;
      float integral_ps = 0;

      //previous method, linear pedestal
      float lInt1 = 0, sInt1 = 0, ratio1 = 0;
      float lIntPed1 = 0, sIntPed1 = 0;
      //simple pedestal, average of first 16 samples
      float lInt2 = 0, sInt2 = 0, ratio2 = 0;
      float lIntPed2 = 0, sIntPed2 = 0;
      //static pedestal, average pedestal value for data
      float lInt3 = 0, sInt3 = 0, ratio3 = 0;
      float lIntPed3 = 0, sIntPed3 = 0;

      float tLstart = 0, tSstart = 0, tstop = 0;
      bool overflow = false;
      int max = std::pow(2, nbits), tMax = 0;
      float pedBegin = 0, pedEnd = 0, pedBeginSamp = 0, pedEndSamp = 0;
      float pedSlope = 0, pedInt = 0, pedMid = 0;


      const std::vector<int>& samples = (*pIter)->GetSamples();
      const int nSamp = samples.size();

      //////////////Pedestal /////////////////////////////////////

      for(std::vector<int>::const_iterator pulse_begin = samples.begin() + 1; pulse_begin != samples.begin() + 9; pulse_begin++){
	pedBegin += (*pulse_begin);
	pedBeginSamp += 1;
      }
      for(std::vector<int>::const_iterator pulse_end = samples.end() - 9; pulse_end != samples.end(); pulse_end++){
	pedEnd += (*pulse_end);
	pedEndSamp += 1;
      }
      pedBegin /= pedBeginSamp;
      pedEnd /= pedEndSamp;
      pedSlope = (pedEnd - pedBegin)/(float)nSamp;

      //integrate the pedestal
      pedMid = pedSlope * ((float)nSamp/2) + pedBegin;
      if(detname == "TSc"){
	pedBegin = (samples.at(0) + samples.at(1) + samples.at(2) + samples.at(3))/4;
	pedMid = pedBegin; pedEnd = pedBegin;   
      }
      pedInt = (float)(nSamp-1)/6 * (pedBegin + (4*pedMid) + pedEnd);

      //do each of these three integrations separately,

      //////////////full integral/////////////////////////////////
       
      for(int i = 0; i+2 < nSamp; i += 2){
	if((samples.at(i) == 0 || samples.at(i+1) == 0) || (samples.at(i) == max_adc || samples.at(i+1) == max_adc)){ 
	  overflow = true; break;
	}
	integral += (samples.at(i) + (4*samples.at(i+1)) + samples.at(i+2))/3;
	if(samples.at(i) < max){
	  max = samples.at(i); tMax = i;
	}
	if(samples.at(i+1) < max){
	  max = samples.at(i+1); tMax = i+1;
	}
      }
      if(nSamp % 2 == 0) //need to include last point
	integral += 0.5 * (samples.at(nSamp-2) + samples.at(nSamp-1));
      integral_ps = polarity * (integral - pedInt);

      float max_ps = ((pedSlope * tMax) + pedBegin) - max; // polarity -1


      ///////////////////set times///////////////////////////////////

      tLstart = tMax - 7;
      if(tLstart <= 2) continue;
      if(tLstart > nSamp - 3) continue; //late pulse
      
      tSstart = tMax + 9;
      if(tSstart < 0) continue;

      tstop = tMax + 81;
      if(tstop >= nSamp) continue; 

      float pedestal = (*pIter)->GetPedestal(16);
      /*
      float baseline = 0;
      if(detname == "NdetD")
	baseline = 15837;
      if(detname == "NdetU")
	baseline = 15886;
      */

      /////////////////long gate//////////////////////////////////////

      int lastSamp = 0;
      //integrate bulk, Simpson's rule
      for(int i = tLstart; i+2 <= tstop; i += 2){
	lInt1 += (samples.at(i) + (4*samples.at(i+1)) + samples.at(i+2))/3;
	lInt2 += (samples.at(i) + (4*samples.at(i+1)) + samples.at(i+2))/3;
	//lInt3 += (samples.at(i) + (4*samples.at(i+1)) + samples.at(i+2))/3;
	lIntPed1 += 2 * (pedSlope*(i+1) + pedBegin);
	lIntPed2 += 2 * pedestal;
	//lIntPed3 += 2 * baseline; 
	lastSamp = i+2;
      }
      //assume pulse tail not at endpoint, linearly interpolate last point
      if(lastSamp != tstop){
	lInt1 += samples.at(tstop);
	lInt2 += samples.at(tstop);
	//lInt3 += samples.at(tstop);
	lIntPed1 += pedSlope*(tstop) + pedBegin;
	lIntPed2 += pedestal;
	//lIntPed3 += baseline;
      }

      lInt1 = polarity * (lInt1 - lIntPed1);
      lInt2 = polarity * (lInt2 - lIntPed2);
      //lInt3 = polarity * (lInt3 - lIntPed3);

      ///////////////short gate///////////////////////////////////

      lastSamp = 0;
      //integrate bulk, Simpson's rule
      for(int i = tSstart; i+2 <= tstop; i += 2){
	sInt1 += (samples.at(i) + (4*samples.at(i+1)) + samples.at(i+2))/3;
	sInt2 += (samples.at(i) + (4*samples.at(i+1)) + samples.at(i+2))/3;
	//sInt3 += (samples.at(i) + (4*samples.at(i+1)) + samples.at(i+2))/3;
	sIntPed1 += 2 * (pedSlope*(i+1) + pedBegin);
	sIntPed2 += 2 * pedestal;
	//sIntPed3 += 2 * baseline;
	lastSamp = i+2;
      }
      //integrate last, trapezoidal with linear interpolation
      if(lastSamp != tstop){
	sInt1 += samples.at(tstop);
	sInt2 += samples.at(tstop);
	//sInt3 += samples.at(tstop);
	sIntPed1 += pedSlope*(tstop) + pedBegin;
	sIntPed2 += pedestal;
	//sIntPed3 += baseline;
      }

      sInt1 = polarity * (sInt1 - sIntPed1);
      sInt2 = polarity * (sInt2 - sIntPed2);
      //sInt3 = polarity * (sInt3 - sIntPed3);

      /////////////////////////////////////////////////////////

      if(overflow) continue;


      //fill the histograms
      ratio1 = sInt1/lInt1;
      ratio2 = sInt2/lInt2;
      //ratio3 = sInt3/lInt3;

      float energy=0;
      if(detname == "NdetD"){
	energy = 0.008234 + 0.0003999 * max_ps;
      }
      if(detname == "NdetU"){
	energy = 0.009037 + 0.0004015 * max_ps;
      }

      if(energy > 1.5 && ratio1 > 0.23){
	int timestamp = (*pIter)->GetTimeStamp();
	AltBaseline_map[bankname]->Fill(pedestal);
	AltTimestamp_map[bankname]->Fill(timestamp);
      else if((energy > 1.5 && ratio1 > 0.13) || (energy < 1.5 && ratio1 > 0.17))
	NeutronBaseline_map[bankname]->Fill(pedestal);
      else if((energy > 1.5 && ratio1 < 0.13) || (energy < 1.5 && ratio1 < 0.16))
	GammaBaseline_map[bankname]->Fill(pedestal);

      StdPSD_map[bankname]->Fill(energy, ratio1);
      PedestalPSD_map[bankname]->Fill(energy, ratio2);
      //BaselinePSD_map[bankname]->Fill(energy, ratio3);

      if(energy < 1.5) continue;
      else if(detname == "NdetD"){
	if(ratio1 <= 0.16 && ratio1 > 0.09) PSDIntTestGD_count++;
	else if (ratio1 <= 0.26 && ratio1 > 0.16) PSDIntTestND_count++;
	else if (ratio1 <= 0.35 && ratio1 > 0.26) PSDIntTestAD_count++;
      }
      else if(detname == "NdetU"){
	if(ratio1 <= 0.17 && ratio1 > 0.1) PSDIntTestGU_count++;
	else if (ratio1 <= 0.27 && ratio1 > 0.17) PSDIntTestNU_count++;
	else if (ratio1 <= 0.37 && ratio1 > 0.27) PSDIntTestAU_count++;
      }


    }
  }

  return SUCCESS;
}
