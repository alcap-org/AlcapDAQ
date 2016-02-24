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
INT  MPSDFGA_init(void);
INT  MPSDFGA(EVENT_HEADER*, void*);
//INT  MPSDFGA_eor(INT);
INT  MPSDFGA_BookHistograms();

extern HNDLE hDB;
extern TGlobalData* gData;
extern TSetupData* gSetup;

std::map<std::string,TH2F*> NdetFGA_map, NdetFGAE_map;


bool PSDFGA_firstEvent = true;
//int PSDFGA_counter = 0, PSDFGA2_counter = 0, PSDFGA3_counter = 0;

ANA_MODULE MPSDFGA_module =
{
	"MPSDFGA",                     /* module name           */
	"Damien Alexander",            /* author                */
	MPSDFGA,                       /* event routine         */
	NULL,                          /* BOR routine           */
	NULL,                          /* EOR routine           */
	MPSDFGA_init,                  /* init routine          */
	NULL,                          /* exit routine          */
	NULL,                          /* parameter structure   */
	0,                             /* structure size        */
	NULL,                          /* initial parameters    */
};

/////////////////////////////////////////////////////////////////



INT MPSDFGA_init(){
  return SUCCESS;
}

INT MPSDFGA_BookHistograms() 
{
  TDirectory* cwd = gDirectory;
  gDirectory->mkdir("PSDFGA")->cd();

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


    std::string histname = "h" + detname + "_FGA";
    std::string histtitle = "FGA PSD for " + detname + " with tail end ";
    TH2F* hNdetFGA = new TH2F(histname.c_str(), histtitle.c_str(), max_adc/4, 0, max_adc, 800, 0, 0.9);
    hNdetFGA->GetYaxis()->SetTitle("1 - (F(1)/F(0))^2");
    hNdetFGA->GetXaxis()->SetTitle("Pulse Height (adc count)");
    NdetFGA_map[bankname] = hNdetFGA;


    histname = "h" + detname + "_FGAE";
    histtitle = "FGA PSD vs Energy for " + detname + " with tail end ";
    TH2F* hNdetFGAE = new TH2F(histname.c_str(), histtitle.c_str(), max_adc/4, 0, 6.75, 800, 0, 0.9);
    hNdetFGAE->GetYaxis()->SetTitle("1 - (F(1)/F(0))^2");
    hNdetFGAE->GetXaxis()->SetTitle("Energy (MeVee)");
    NdetFGAE_map[bankname] = hNdetFGAE;

  }

  cwd->cd();
  //gDirectory->mkdir("PSDFGA_Unknown");
  //gDirectory->mkdir("PSDFGA_Neutron");
  //gDirectory->mkdir("PSDFGA_Gamma");
  return SUCCESS;
}

INT MPSDFGA_eor(INT run_number)
{
  return SUCCESS;
}

INT MPSDFGA(EVENT_HEADER *pheader, void *pevent)
{
  if(PSDFGA_firstEvent == true){
    MPSDFGA_BookHistograms();
    PSDFGA_firstEvent = false;
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
   

    for(std::vector<TPulseIsland*>::const_iterator pIter = pulses.begin(); pIter != pulses.end(); pIter++){

      float integral = 0;
      float integral_ps = 0;
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

      float max_ps = polarity * (max - pedBegin); // polarity -1

      if(overflow) continue;

      //////////FGA Algorithm//////////////////////////////////

      int length = 80;
      if(tMax + length > samples.size()) continue;
      float sum1 = 0, sumcos = 0, sumsin = 0;
      float avg1 = 0, avg0 = 0;
      float tmp = 0, diff = 0, energy = 0;
      float coef = 2 * 3.14159 / length;

      for(int i = tMax; i < tMax + length; i++){
	tmp = (polarity * (samples.at(i) - pedBegin) ) / max_ps;
	sum1 += tmp;
	sumcos += tmp * cos(coef * i);
	sumsin += tmp * sin(coef * i);
      }

      avg1 = sqrt(sumcos * sumcos + sumsin * sumsin);
      avg0 = abs(sum1);

      diff = avg0 - avg1;
      float ratiosq = 1 - ( (avg1 * avg1) / (avg0 * avg0) );

      //fill the histograms

      if(detname == "NdetD") energy = 0.008234 + 0.0003999 * max_ps;
	
      if(detname == "NdetU") energy = 0.009037 + 0.0004015 * max_ps;

      NdetFGA_map[bankname]->Fill(max_ps, ratiosq);
      NdetFGAE_map[bankname]->Fill(energy, ratiosq);
      

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
