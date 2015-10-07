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
INT  MNeutronCut_init(void);
INT  MNeutronCut(EVENT_HEADER*, void*);
INT  MNeutronCut_eor(INT);
INT  MNeutronCut_BookHistograms();

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

std::map<std::string,TH2F*> NeutronUncut_map, NeutronCut_map;
std::map<std::string,TH2F*> NeutronUncutE_map, NeutronCutE_map;
std::map<std::string, TH1F*> NeutronIntegral_map, NeutronEnergy_map;

bool NeutronCut_firstEvent = true;

ANA_MODULE MNeutronCut_module =
{
	"MNeutronCut",                 /* module name           */
	"Damien Alexander",            /* author                */
	MNeutronCut,                   /* event routine         */
	NULL,                          /* BOR routine           */
	MNeutronCut_eor,               /* EOR routine           */
	MNeutronCut_init,              /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/////////////////////////////////////////////////////////////////



INT MNeutronCut_init(){
  return SUCCESS;
}

INT MNeutronCut_BookHistograms() 
{
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("NeutronCut")->cd();

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

    int max_bin = max_adc * n_samples/24;
    float max_E = 7.0;
    if(detname == "NdetU") max_E = 7.5;

    
    std::string histname = "h" + detname + "Uncut";
    std::string histtitle = "Integral Ratio vs Integral for " + detname + " before cut";
    TH2F* hNdetUncut = new TH2F(histname.c_str(), histtitle.c_str(), max_adc, 0, max_bin, 600, 0, 2.5);
    hNdetUncut->GetYaxis()->SetTitle("Integral Ratio");
    hNdetUncut->GetXaxis()->SetTitle("Integral (pedestal subtracted)");
    NeutronUncut_map[bankname] = hNdetUncut;
    
    
    histname = "h" + detname + "Cut";
    histtitle = "Integral Ratio vs Energy for " + detname + "After Cut";
    TH2F* hNdetCut = new TH2F(histname.c_str(), histtitle.c_str(), max_adc, 0, max_bin, 600, 0, 2.5);
    hNdetCut->GetYaxis()->SetTitle("Integral Ratio");
    hNdetCut->GetXaxis()->SetTitle("Energy (MeVee)");
    NeutronCut_map[bankname] = hNdetCut;

    histname = "h" + detname + "UncutEnergy";
    histtitle = "Integral Ratio vs Energy for " + detname + " before cut";
    TH2F* hNdetUncutE = new TH2F(histname.c_str(), histtitle.c_str(), max_adc, 0, 7.5, 600, 0, 2.5);
    hNdetUncutE->GetYaxis()->SetTitle("Integral Ratio");
    hNdetUncutE->GetXaxis()->SetTitle("Energy (MeVee)");
    NeutronUncutE_map[bankname] = hNdetUncutE;
    
    
    histname = "h" + detname + "CutEnergy";
    histtitle = "Integral Ratio vs Energy for " + detname + "After Cut";
    TH2F* hNdetCutE = new TH2F(histname.c_str(), histtitle.c_str(), max_adc, 0, 7.5, 600, 0, 2.5);
    hNdetCutE->GetYaxis()->SetTitle("Integral Ratio");
    hNdetCutE->GetXaxis()->SetTitle("Energy (MeVee)");
    NeutronCutE_map[bankname] = hNdetCutE;

    histname = "h" + detname + "NeutronIntegral";
    histtitle = "Pedestal Subtracted Integral Distribution for " + detname + " Neutrons";
    TH1F* hNeutronIntegral = new TH1F(histname.c_str(), histtitle.c_str(), max_adc, 0, max_bin);
    hNeutronIntegral->GetXaxis()->SetTitle("Integral (ADC Count)");
    hNeutronIntegral->GetYaxis()->SetTitle("Count (arbitrary)");
    NeutronIntegral_map[bankname] = hNeutronIntegral;

    histname = "h" + detname + "NeutronEnergy";
    histtitle = "Energy Distribution for " + detname + " Neutrons";
    TH1F* hNeutronEnergy = new TH1F(histname.c_str(), histtitle.c_str(), max_adc, 0, max_E);
    hNeutronEnergy->GetXaxis()->SetTitle("Energy (MeVee)");
    hNeutronEnergy->GetYaxis()->SetTitle("Count (arbitrary)");
    NeutronEnergy_map[bankname] = hNeutronEnergy;

  }

  cwd->cd();
  return SUCCESS;
}

INT MNeutronCut_eor(INT run_number){
  return SUCCESS; //nothing right now
}

INT MNeutronCut(EVENT_HEADER *pheader, void *pevent)
{
  if(NeutronCut_firstEvent == true){
    MNeutronCut_BookHistograms();
    NeutronCut_firstEvent = false;
  }

  std::map<std::string, std::vector<TPulseIsland*> >& tpi_map = gData->fPulseIslandToChannelMap;

  for(std::map<std::string, std::vector<TPulseIsland*> >::iterator mIter = tpi_map.begin(); mIter != tpi_map.end(); mIter++){
    const std::string bankname = mIter->first;
    const std::string detname = gSetup->GetDetectorName(bankname);
    if(!gSetup->IsNeutron(detname)) continue;

    std::vector<TPulseIsland*>& pulses = mIter->second;
    int polarity = gSetup->GetTriggerPolarity(bankname);
    int nbits = gSetup->GetNBits(bankname);
    int max_adc = std::pow(2, nbits);
   
    float pct = 0.1;

    for(std::vector<TPulseIsland*>::iterator pIter = pulses.begin(); pIter != pulses.end(); pIter++){

      float integral = 0;
      float integral_ps = 0;
      float lInt = 0, sInt = 0, ratio = 0;
      float lIntPed = 0, sIntPed = 0;
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

      for(int j=tMax; j>0; j--){
	float pedestal = (pedSlope * j) + pedBegin;

	float tmp = pedestal - samples.at(j); //polarity -1
	if(tmp < pct * max_ps){//below CF threshold
	  //interpolate the point to CF, linear, ensure pedestal subtraction
	  float slope = 1/(samples.at(j) - samples.at(j+1)); // polarity -1
	  float h1 = (pct*max_ps)  - (tmp);
	  tLstart = j + h1*slope;
	  break;
	}
      }
      if(tLstart <= 2) continue;
      if(tLstart > nSamp - 3) continue; //late pulse
      
      float tailOffset = 10 * (integral_ps) / (857 + integral_ps);

      tSstart = (float)tMax + tailOffset;
      if(tSstart < 0) continue;

      tstop = (float)tMax + tailOffset * 6;
      if(tstop >= nSamp) continue; 

      //std::cout << tLstart << "   " << tSstart << "   " << tstop << std::endl;


      /////////////////long gate//////////////////////////////////////

      //integrate first sample (trapezoidal with linear interpolation)
      int lastSamp = 0, startInt  = tLstart +1;
      float y1 = 0, y2 = 0, y0 = 0, tdiff = 0;
      tdiff = (float)startInt - tLstart;
      if(startInt-1 > 0) y1 = samples.at(startInt -1); 
      if(startInt < nSamp) y2 = samples.at(startInt);
      y0 = y1 + (y2-y1) * tdiff;
      lInt += tdiff*(y0 + samples.at(startInt))/2;
      lIntPed += tdiff * (pedSlope*(tSstart + startInt) + 2*pedBegin)/2;
      //integrate bulk, Simpson's rule
      for(int i = tLstart+1; i+2 <= tstop; i += 2){
	lInt += (samples.at(i) + (4*samples.at(i+1)) + samples.at(i+2))/3;
	lIntPed += 2 * (pedSlope*(i+1) + pedBegin);
	lastSamp = i+2;
      }
      //assume pulse tail not at endpoint, linearly interpolate last point
      tdiff = tstop - (float)lastSamp;
      y1 = samples.at(lastSamp);
      y2 = 0;
      if(lastSamp +1 < nSamp) y2 = samples.at(lastSamp +1);
      y0 = y1 + (y2-y1)*(tdiff);
      lInt += tdiff*(samples.at(lastSamp) + y0)/2;
      lIntPed += tdiff * (pedSlope*(tstop + lastSamp) + 2* pedBegin)/2;
      lInt = polarity * (lInt - lIntPed);

      ///////////////short gate///////////////////////////////////

      //integrate first, trapezoidal with linear interpolation
      startInt = tSstart + 1;
      tdiff = (float)startInt - tSstart;
      if(startInt-1 > 0) y1 = samples.at(startInt -1); 
      if(startInt < nSamp) y2 = samples.at(startInt);
      y0 = y1 + (y2-y1) * tdiff;
      sInt += tdiff*(y0 + samples.at(startInt))/2;
      sIntPed += tdiff * (pedSlope*(tSstart + startInt) + 2*pedBegin)/2;
      //integrate bulk, Simpson's rule
      for(int i = tSstart+1; i+2 <= tstop; i += 2){
	sInt += (samples.at(i) + (4*samples.at(i+1)) + samples.at(i+2))/3;
	sIntPed += 2 * (pedSlope*(i+1) + pedBegin);
	lastSamp = i+2;
      }
      //integrate last, trapezoidal with linear interpolation
      tdiff = tstop - (float)lastSamp;
      y1 = samples.at(lastSamp); 
      if(lastSamp+1 < nSamp) y2 = samples.at(lastSamp + 1);
      y0 = y1 + (y2-y1)*(tdiff);
      sInt += tdiff*(samples.at(lastSamp) + y0)/2;
      sIntPed += tdiff * (pedSlope*(tstop + lastSamp) + 2*pedBegin)/2;
      sInt = polarity * (sInt - sIntPed);

      /////////////////////////////////////////////////////////

      if(overflow) continue;


      //fill the histograms
      ratio = sInt/lInt;

      float energy=0, ratio_t = 0;
      if(detname == "NdetD"){
	energy = 0.02358 + 0.00006766 * integral_ps;
	ratio_t = ratio/(0.08482 + 3.951/sqrt(integral_ps) + 73.88/integral_ps);
      }
      if(detname == "NdetU"){
	energy = 0.02117 + 0.0000686 * integral_ps;
	ratio_t = ratio/(0.08944 + 6.653/sqrt(integral_ps) - 64/12/integral_ps);
      }


      NeutronUncut_map[bankname]->Fill(integral_ps, ratio_t);
      NeutronUncutE_map[bankname]->Fill(energy, ratio_t);
      
      if(ratio_t < 1){//perform cut (want neutrons)
	pulses.erase(pIter);
	if(std::distance(pulses.begin(), pIter) > 1) pIter--;
	continue;
      }
    
      NeutronCut_map[bankname]->Fill(integral_ps, ratio_t);
      NeutronCutE_map[bankname]->Fill(energy, ratio_t);

      NeutronIntegral_map[bankname]->Fill(integral_ps);
      NeutronEnergy_map[bankname]->Fill(energy);
      
    }
  }

  return SUCCESS;
}
