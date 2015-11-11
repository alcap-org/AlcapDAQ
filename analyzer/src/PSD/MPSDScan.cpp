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
INT  MPSDScan_init(void);
INT  MPSDScan(EVENT_HEADER*, void*);
INT  MPSDScan_eor(INT);
INT  MPSDScan_BookHistograms();

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

std::map<int, TH2F*> NdetDRatio_map, NdetURatio_map;
std::map<int, TH1D*> NdetDFoM_map, NdetUFoM_map;

bool PSD_firstEvent = true;
int PSD_counter = 0;

ANA_MODULE MPSDScan_module =
{
	"MPSDScan",                  /* module name           */
	"Damien Alexander",            /* author                */
	MPSDScan,                    /* event routine         */
	NULL,                          /* BOR routine           */
	MPSDScan_eor,                /* EOR routine           */
	MPSDScan_init,               /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/////////////////////////////////////////////////////////////////



INT MPSDScan_init(){
  return SUCCESS;
}

INT MPSDScan_BookHistograms() 
{
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("PSDScan")->cd();

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

    int max_bin = max_adc * n_samples/30;

    for(int i = 0; i<9; i++){
      std::stringstream ss;
      ss << i+4;

      if(detname == "NdetD"){
	std::string histname = "hNdetDRatio" + ss.str();
	std::string histtitle = "Integral Ratio vs Integral for NdetD with tail start " + ss.str();
	TH2F* hNdetDRatio = new TH2F(histname.c_str(), histtitle.c_str(), max_adc, 0, max_bin, 700, 0, 0.6);
	hNdetDRatio->GetYaxis()->SetTitle("Integral Ratio");
	hNdetDRatio->GetXaxis()->SetTitle("Integral (pedestal subtracted)");
	NdetDRatio_map[i] = hNdetDRatio;
      }

      else if(detname == "NdetU"){
	std::string histname = "hNdetURatio" + ss.str();
	std::string histtitle = "Integral Ratio vs Integral for NdetU with tail start " + ss.str();
	TH2F* hNdetURatio = new TH2F(histname.c_str(), histtitle.c_str(), max_adc, 0, max_bin, 700, 0, 0.6);
	hNdetURatio->GetYaxis()->SetTitle("Integral Ratio");
	hNdetURatio->GetXaxis()->SetTitle("Integral (pedestal subtracted)");
	NdetURatio_map[i] = hNdetURatio;
      }
    }
  }

  cwd->cd();
  return SUCCESS;
}

INT MPSDScan_eor(INT run_number)
{
  gDirectory->mkdir("PSDScan/NdetD_FoM")->cd();
  //add in FoM projections( or would pulse height based selection
  //make more sense?)  need to consider this closely
  for(std::map<int, TH2F*>::iterator mIt = NdetDRatio_map.begin(); mIt != NdetDRatio_map.end(); mIt++){
    std::stringstream ss1;
    int cut = std::distance(NdetDRatio_map.begin(), mIt);
    ss1 << cut + 4;
    for(int i = 0; i < 16; i++){
      int iBin1 = i * 1000, iBin2 = ((i + 1) * 1000) - 1;
      float rateD = NdetDRatio_map[cut]->GetXaxis()->GetNbins()/ NdetDRatio_map[cut]->GetXaxis()->GetXmax();
      std::stringstream st1, st2, ss2;
      ss2 << i;
      st1 << (int)((float)iBin1/rateD);
      st2 << (int)((float)iBin2/rateD);
    
      std::string histname = "hNdetD_FoM_" + ss1.str() + "_" + ss2.str();
      std::string histtitle = "FoM Projection for NdetD, short gate " + ss1.str() + " from " + st1.str() + " to " + st2.str();
    
      NdetDFoM_map[i] = NdetDRatio_map[cut]->ProjectionY(histname.c_str(), iBin1, iBin2);
      NdetDFoM_map[i]->SetTitle(histtitle.c_str());
    }
  }
  gDirectory->cd("..");

  gDirectory->mkdir("PSDScan/NdetU_FoM")->cd();
  for(std::map<int, TH2F*>::iterator mIt = NdetURatio_map.begin(); mIt != NdetURatio_map.end(); mIt++){
    std::stringstream ss1;
    int cut = std::distance(NdetURatio_map.begin(), mIt);
    ss1 << cut + 4;
    for(int i = 0; i < 16; i++){
      int iBin1 = i * 1000, iBin2 = ((i + 1) * 1000) - 1;
      float rateU = NdetURatio_map[cut]->GetXaxis()->GetNbins()/ NdetURatio_map[cut]->GetXaxis()->GetXmax();
      std::stringstream st1, st2, ss2;
      ss2 << i;
      st1 << (int)((float)iBin1/rateU);
      st2 << (int)((float)iBin2/rateU);
    
      std::string histname = "hNdetU_FoM_" + ss1.str() + "_" + ss2.str();
      std::string histtitle = "FoM Projection for NdetU, short gate " + ss1.str() + " bins " + st1.str() + " to " + st2.str();
    
      NdetUFoM_map[i] = NdetURatio_map[cut]->ProjectionY(histname.c_str(), iBin1, iBin2);
      NdetUFoM_map[i]->SetTitle(histtitle.c_str());
    }
  }
  gDirectory->cd("..");

  return SUCCESS;  // currently nothing to do here
}

INT MPSDScan(EVENT_HEADER *pheader, void *pevent)
{
  if(PSD_firstEvent == true){
    MPSDScan_BookHistograms();
    PSD_firstEvent = false;
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
   
    float pct = 0.2;

    for(std::vector<TPulseIsland*>::const_iterator pIter = pulses.begin(); pIter != pulses.end(); pIter++){

      float integral = 0;
      float integral_ps = 0;
      float lInt = 0, sInt[9], ratio = 0;
      float lIntPed = 0, sIntPed[9];
      int tLstart = 0, tstop = 0;
      int tSstart[9];
      bool overflow = false;
      int max = std::pow(2, nbits), tMax = 0;
      float pedBegin = 0, pedEnd = 0, pedBeginSamp = 0, pedEndSamp = 0;
      float pedSlope = 0, pedInt = 0, pedMid = 0;

      for(int i = 0; i < 9; i++){
        sInt[i] = 0;
	tSstart[i] = 0;
	sIntPed[i] = 0;
      }

      const std::vector<int>& samples = (*pIter)->GetSamples();
      const int nSamp = samples.size();

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
      pedInt = nSamp/6 * (pedBegin + (4*pedMid) + pedEnd);

      //do each of these three integrations separately,

      //full integral       
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
      if(nSamp % 2 == 1) //need to include last point
	integral += 0.5 * (samples.at(nSamp-1) + samples.at(nSamp));
      integral_ps = polarity * (integral - pedInt);

      float max_ps = ((pedSlope * tMax) + pedBegin) - max; // polarity -1


      //set times
      for(int j=tMax; j>0; j--){
	float pedestal = (pedSlope * j) + pedBegin;

	float tmp = pedestal - samples.at(j); //polarity -1
	if(tmp < 0.3 * max_ps){//below CF threshold
	  tLstart = j;
	  break;
	}//interpolation comes later
      }
      if(tLstart <= 0){//early pulse
	tLstart = 0; overflow = true;
      }
      for(int i = 0; i < 9; i++){
	tSstart[i] = tMax + i + 4;
	tstop = tMax + (i + 4) * 6;
      }
      if(tstop >= nSamp){ //technically late starting pulse
	tstop =nSamp -1; overflow = true; 
      }


      //long gate
      for(int i = tLstart; i+2 <= tstop; i += 2){
	lInt += (samples.at(i) + (4*samples.at(i+1)) + samples.at(i+2))/3;
	lIntPed += 2 * (pedSlope*(i+1) + pedBegin);
      }
      if((tstop - (int)tLstart) % 2 == 1){
	lInt += 0.5 * (samples.at(tstop - 1) + samples.at(tstop));
	lIntPed += pedSlope * ((float)tstop - 0.5) + pedBegin;
      }
      lInt = polarity * (lInt - lIntPed);

      //short gate
      for(int j=0; j< 9; j++){
	for(int i = tSstart[j]; i+2 <= tstop; i += 2){
	  sInt[j] += (samples.at(i) + (4*samples.at(i+1)) + samples.at(i+2))/3;
	  sIntPed[j] += 2 * (pedSlope*(i+1) + pedBegin);
	}
	if((tstop - (int)tSstart[j]) % 2 == 1){
	  sInt[j] += 0.5 *(samples.at(tstop - 1) + samples.at(tstop));
	  sIntPed[j] += pedSlope * ((float)tstop - 0.5) + pedBegin;
	}
	sInt[j] = polarity * (sInt[j] - sIntPed[j]);
      }

      if(overflow) continue;


      //fill the histograms
      for(int j = 0; j< 9; j++){
	ratio = sInt[j]/lInt;
	if(detname == "NdetD") NdetDRatio_map[j]->Fill(integral_ps, ratio);
	if(detname == "NdetU") NdetURatio_map[j]->Fill(integral_ps, ratio);
      }

      //Plot some questionable pulses
      /*
      if(PSD_counter < 20 && integral_ps > 375000){
	std::stringstream ss;
	ss << PSD_counter;
	std::string histname = "hIPulse" + ss.str();
	std::string histtitle = "High Integral Waveform" + ss.str();
	TH1F* hIPulse = new TH1F(histname.c_str(), histtitle.c_str(), intSamp, 0, intSamp);
	for(int j = 0; j < intSamp; j++)
	  hIPulse->Fill(j, yout[j]);
      PSD_counter++;

      }
      */
    }
  }

  return SUCCESS;
}
