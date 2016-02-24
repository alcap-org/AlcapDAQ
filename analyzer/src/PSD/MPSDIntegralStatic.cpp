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
INT  MPSDIntegralStatic_init(void);
INT  MPSDIntegralStatic(EVENT_HEADER*, void*);
//INT  MPSDIntegral_eor(INT);
INT  MPSDIntegralStatic_BookHistograms();

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

std::map<std::string,TH2F*> NdetRatioS_map, /*NdetNormRatioS_map,*/ NdetRatioEnergyS_map, NdetIntegrals_map, NdetIntegralE_map;
//std::map<int, TH1D*> NdetDIFoMS_map, NdetUIFoMS_map;
static std::map<std::string, std::ofstream*> waveforms;

bool PSDIntS_firstEvent = true;
int PSDIntS_counter = 0, PSDIntS2_counter = 0, PSDIntS3_counter = 0;

ANA_MODULE MPSDIntegralStatic_module =
{
	"MPSDIntegral",                /* module name           */
	"Damien Alexander",            /* author                */
	MPSDIntegralStatic,            /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MPSDIntegralStatic_init,       /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/////////////////////////////////////////////////////////////////



INT MPSDIntegralStatic_init(){
  return SUCCESS;
}

INT MPSDIntegralStatic_BookHistograms() 
{
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("PSDIntegralStatic")->cd();

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

    
    std::string histname = "h" + detname + "RatioS";
    std::string histtitle = "Integral Ratio vs Pulse Height for " + detname;
    TH2F* hNdetRatio = new TH2F(histname.c_str(), histtitle.c_str(), max_adc, 0, max_adc, 600, 0, 0.45);
    hNdetRatio->GetYaxis()->SetTitle("Integral Ratio");
    hNdetRatio->GetXaxis()->SetTitle("Pulse Height (adc count)");
    NdetRatioS_map[bankname] = hNdetRatio;

    /*
    histname = "h" + detname + "_NormRatio";
    histtitle = "Integral Ratio vs Integral for " + detname;
    TH2F* hNdetNormRatio = new TH2F(histname.c_str(), histtitle.c_str(), max_adc, 0, max_bin, 600, 0, 2.5);
    hNdetNormRatio->GetYaxis()->SetTitle("Integral Ratio");
    hNdetNormRatio->GetXaxis()->SetTitle("Integral (pedestal subtracted)");
    NdetNormRatio_map[bankname] = hNdetNormRatio;
    */
    
    histname = "h" + detname + "RatioEnergyS";
    histtitle = "Integral Ratio vs Energy for " + detname;
    TH2F* hNdetERatio = new TH2F(histname.c_str(), histtitle.c_str(), max_adc, 0, 7.5, 600, 0, 0.5);
    hNdetERatio->GetYaxis()->SetTitle("Integral Ratio");
    hNdetERatio->GetXaxis()->SetTitle("Energy (MeVee)");
    NdetRatioEnergyS_map[bankname] = hNdetERatio;


    histname = "h" + detname + "_FullvTail";
    histtitle = "Full Integral vs Tail Integral for " + detname;
    TH2F* hNdetIntegrals = new TH2F(histname.c_str(), histtitle.c_str(), 2000, 0, 100000, 1000, 0, 20000);
    hNdetIntegrals->GetYaxis()->SetTitle("Tail Integral");
    hNdetIntegrals->GetXaxis()->SetTitle("Full Integral");
    NdetIntegrals_map[bankname] = hNdetIntegrals;

    histname = "h" + detname + "_TailVEnergy";
    histtitle = "Tail Integral vs energy for " + detname;
    TH2F* hNdetEIntegral = new TH2F(histname.c_str(), histtitle.c_str(), max_adc, 0, 6.75, 1000, 0, 20000);
    hNdetEIntegral->GetYaxis()->SetTitle("Tail Integral");
    hNdetEIntegral->GetXaxis()->SetTitle("Energy (MeVee)");
    NdetIntegralE_map[bankname] = hNdetEIntegral;

    /*
    std::string outTitle = detname + ".dat";
    std::ofstream *outfile = new std::ofstream(outTitle.c_str(), std::ios::trunc);
    waveforms[bankname] = outfile;
    */
  }




  cwd->cd();
  //gDirectory->mkdir("PSDIntegralStatic_Unknown");
  //gDirectory->mkdir("PSDIntegralStatic_Neutron");
  //gDirectory->mkdir("PSDIntegralStatic_Gamma");
  return SUCCESS;
}

INT MPSDIntegral_eor(INT run_number)
{
  /*
  for(std::map<std::string, std::ofstream*>::const_iterator mIter = waveforms.begin(); mIter != waveforms.end(); mIter++){
    std::ofstream *outfile = mIter->second;
    outfile->close();
  }
  */

  /*  
  gDirectory->mkdir("PSDIntegral/NdetD_FoM");
  gDirectory->mkdir("PSDIntegral/NdetU_FoM");
  //add in FoM projections
  for(std::map<std::string, TH2F*>::iterator mIt = NdetRatio_map.begin(); mIt != NdetRatio_map.end(); mIt++){
    std::string bankname = mIt->first;
    std::string detname = gSetup->GetDetectorName(bankname);

    if(detname == "NdetD") gDirectory->cd("/PSDIntegral/NdetD_FoM");
    else if(detname == "NdetU") gDirectory->cd("/PSDIntegral/NdetU_FoM");

    for(int i = 0; i < 17; i++){
      int iBin1 = i * 1000, iBin2 = ((i + 1) * 1000) - 1;
      float rate = NdetRatio_map[bankname]->GetXaxis()->GetNbins()/ NdetRatio_map[bankname]->GetXaxis()->GetXmax();
      if(i == 16){
	iBin1 = 0; iBin2 = 16000; rate = 1;
      }
      std::stringstream st1, st2, ss2;
      ss2 << i;
      st1 << (int)((float)iBin1/rate);
      st2 << (int)((float)iBin2/rate);
    
      std::string histname = "h" + detname + "_FoM_" + ss2.str();
      std::string histtitle = "FoM Projection for " + detname + " from " + st1.str() + " to " + st2.str();
    
      if(detname == "NdetD"){
	NdetDIFoM_map[i] = NdetRatio_map[bankname]->ProjectionY(histname.c_str(), iBin1, iBin2);
	NdetDIFoM_map[i]->SetTitle(histtitle.c_str());
      }
      else if(detname == "NdetU"){
	NdetUIFoM_map[i] = NdetRatio_map[bankname]->ProjectionY(histname.c_str(), iBin1, iBin2);
	NdetUIFoM_map[i]->SetTitle(histtitle.c_str());
      }


    }
  }
  gDirectory->cd("..");
  */
  return SUCCESS;
}

INT MPSDIntegralStatic(EVENT_HEADER *pheader, void *pevent)
{
  if(PSDIntS_firstEvent == true){
    MPSDIntegralStatic_BookHistograms();
    PSDIntS_firstEvent = false;
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
      /*
      for(int j=tMax; j>0; j--){
	float pedestal = (pedSlope * j) + pedBegin;

	float tmp = pedestal - samples.at(j); //polarity -1
	if(tmp < pct * max_ps){//below CF threshold
	  //interpolate the point to CF, linear, ensure pedestal subtraction
	  float slope = 0;
	  if(samples.at(j) != samples.at(j+1))
	    slope = 1/(samples.at(j) - samples.at(j+1)); // polarity -1
	  float h1 = (pct*max_ps)  - (tmp);
	  tLstart = j + h1*slope;
	  break;
	}
      }
      */
      tLstart = tMax - 7;
      if(tLstart <= 2) continue;
      if(tLstart > nSamp - 3) continue; //late pulse
      
      //float tailOffset = 10 * (integral_ps) / (857 + integral_ps);

      //tSstart = (float)tMax + tailOffset;
      tSstart = tMax + 9;
      if(tSstart < 0) continue;

      //tstop = (float)tMax + tailOffset * 6;
      tstop = tMax + 81;
      if(tstop >= nSamp) continue; 

      //std::cout << tLstart << "   " << tSstart << "   " << tstop << std::endl;


      /////////////////long gate//////////////////////////////////////

      int lastSamp = 0;
      //integrate bulk, Simpson's rule
      for(int i = tLstart; i+2 <= tstop; i += 2){
	lInt += (samples.at(i) + (4*samples.at(i+1)) + samples.at(i+2))/3;
	lIntPed += 2 * (pedSlope*(i+1) + pedBegin);
	lastSamp = i+2;
      }
      //assume pulse tail not at endpoint, linearly interpolate last point
      if(lastSamp != tstop){
	lInt += samples.at(tstop);
	lIntPed += pedSlope*(tstop) + pedBegin;
      }

      lInt = polarity * (lInt - lIntPed);

      ///////////////short gate///////////////////////////////////

      lastSamp = 0;
      //integrate bulk, Simpson's rule
      for(int i = tSstart; i+2 <= tstop; i += 2){
	sInt += (samples.at(i) + (4*samples.at(i+1)) + samples.at(i+2))/3;
	sIntPed += 2 * (pedSlope*(i+1) + pedBegin);
	lastSamp = i+2;
      }
      //integrate last, trapezoidal with linear interpolation
      if(lastSamp != tstop){
	sInt += samples.at(tstop);
	sIntPed += pedSlope*(tstop) + pedBegin;
      }

      sInt = polarity * (sInt - sIntPed);

      /////////////////////////////////////////////////////////

      if(overflow) continue;


      //fill the histograms
      ratio = sInt/lInt;

      float energy=0; //, ratio_t = 0;
      if(detname == "NdetD"){
	energy = 0.008234 + 0.0003999 * max_ps;
	//ratio_t = ratio/(0.09880 + 283.1/integral_ps);
      }
      if(detname == "NdetU"){
	energy = 0.009037 + 0.0004015 * max_ps;
	//ratio_t = ratio/(0.1116 +  337.7/integral_ps);
      }

      NdetRatioS_map[bankname]->Fill(max_ps, ratio);
      //NdetNormRatio_map[bankname]->Fill(integral_ps, ratio_t);
      NdetRatioEnergyS_map[bankname]->Fill(energy, ratio);
      NdetIntegrals_map[bankname]->Fill(lInt, sInt);
      NdetIntegralE_map[bankname]->Fill(energy, sInt);

      (*pIter)->SetPSDParameter(ratio);


      /*
      //Plot some questionable pulses
      if(PSDIntS_counter >= 20 && PSDIntS2_counter >= 20 && PSDIntS3_counter >= 20) continue;
      
      std::ofstream *outfile = waveforms[bankname];
      if(!outfile->is_open()) cout << "MPSDIntegralStatic Error, could not open outut file " << endl;
      
      bool plot = false;
      if((max_ps > 2000 && max_ps < 3000) && (ratio > 0.25 && ratio < 0.30)) plot = true;
      if(PSDIntS_counter < 20 && detname == "NdetU" && plot){
	gDirectory->cd("PSDIntegralStatic_Unknown");
	std::stringstream ss;
	ss << PSDIntS_counter + 1;
	std::string histname = "hPSDIntPulse" + ss.str();
	std::string histtitle = "High Ratio Waveform " + ss.str();
	TH1F* hIPulse = new TH1F(histname.c_str(), histtitle.c_str(), nSamp, 0, nSamp);
	
	*outfile << "High Ratio Waveform " << ss.str() << " : " << endl;
	*outfile << "   pedestal : Integral " << pedInt << ",  PedBegin " << pedBegin << ",  PedEnd " << pedEnd << ", PedMid " << pedMid << endl; 
	*outfile << "   Integrals : Full " << lInt << ", Short " << sInt << ", ratio " << ratio << endl;
	*outfile << "   values : ";
	
	for(int j = 0; j < nSamp; j++){
	  hIPulse->Fill(j, samples.at(j));
	  *outfile << samples.at(j) << " ";
	}
	*outfile << endl << endl;
	PSDIntS_counter++;
	gDirectory->cd("..");
      }
      
      bool plot_2 = false;
      if((max_ps > 2200 && max_ps < 2300) && (ratio > 0.175 && ratio < 0.225)) plot_2 = true;
      if(PSDIntS2_counter < 20 && detname == "NdetU" && plot_2){
	gDirectory->cd("PSDIntegralStatic_Neutron");
	std::stringstream ss;
	ss << PSDIntS2_counter + 1;
	std::string histname = "hPSDIntPulse_2_" + ss.str();
	std::string histtitle = "Neutron Ratio Waveform " + ss.str();
	TH1F* hIPulse = new TH1F(histname.c_str(), histtitle.c_str(), nSamp, 0, nSamp);
	
	*outfile << "Neutron Waveform " << ss.str() << " : " << endl;
	*outfile << "   pedestal : Integral " << pedInt << ",  PedBegin " << pedBegin << ",  PedEnd " << pedEnd << ", PedMid " << pedMid << endl; 
	*outfile << "   Integrals : Full " << lInt << ", Short " << sInt << ", ratio " << ratio << endl;
	*outfile << "   values : ";
	
	for(int j = 0; j < nSamp; j++){
	  hIPulse->Fill(j, samples.at(j));
	  *outfile << samples.at(j) << " ";
	}
	*outfile << endl << endl;

	PSDIntS2_counter++;
	gDirectory->cd("..");
      }
      
      bool plot_3 = false;
      if((max_ps > 2200 && max_ps < 2300) && (ratio > 0.075 && ratio < 0.105)) plot_3 = true;
      if(PSDIntS3_counter < 20 && detname == "NdetU" && plot_3){
	gDirectory->cd("PSDIntegralStatic_Gamma");
	std::stringstream ss;
	ss << PSDIntS3_counter+1;
	std::string histname = "hPSDIntPulse_3_" + ss.str();
	std::string histtitle = "Gamma Ratio Waveform " + ss.str();
	TH1F* hIPulse = new TH1F(histname.c_str(), histtitle.c_str(), nSamp, 0, nSamp);
	
	*outfile << "Gamma Waveform " << ss.str() << " : " << endl;
	*outfile << "   pedestal : Integral " << pedInt << ",  PedBegin " << pedBegin << ",  PedEnd " << pedEnd << ", PedMid " << pedMid << endl; 
	*outfile << "   Integrals : Full " << lInt << ", Short " << sInt << ", ratio " << ratio << endl;
	*outfile << "   values : ";
	
	for(int j = 0; j < nSamp; j++){
	  hIPulse->Fill(j, samples.at(j));
	  *outfile << samples.at(j) << " ";
	}
	*outfile << endl << endl;
	PSDIntS3_counter++;
	gDirectory->cd("..");
      }
      */
    }
  }

  return SUCCESS;
}
