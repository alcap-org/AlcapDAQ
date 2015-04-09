///////////////////////////////////////////////////////
//  NGIntTCut        for use in rootana for Alcap    //
//                                                   //
//  Damien Alexander                                 // 
//                                                   //
//  A Module to Discriminate neutron signals from    //
//  gamma signals following a time cut using the     //
//  ratio of the full integral to the tail integral. //
///////////////////////////////////////////////////////

//#define USE_PRINT_OUT 1

#include "NGIntTCut.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <cmath>

#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "TH2F.h"
#include "TH1.h"
#include <sstream>

using std::string;
using std::map;
using std::vector;
using std::pair;

extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;

std::map<std::string, TH2F*> NGCDisc_plots;
std::map<std::string, TH2F*> NGCRatio_plots;
std::map<std::string, TH1F*> NGNCEnergy_plots, NGGCEnergy_plots;
std::map<std::string, TH1D*> NGC1D_plots;

NGIntTCut::NGIntTCut(char *HistogramDirectoryName, std::string detnameA, double start_window, double stop_window) :
  FillHistBase(HistogramDirectoryName)
  {
    fDetNameA = detnameA;
    fStartWindow = start_window;
    fStopWindow = stop_window;
    fHistName = HistogramDirectoryName;
    dir->cd("/");
  }

NGIntTCut::~NGIntTCut()
{
}

int NGIntTCut::ProcessEntry(TGlobalData *gData, TSetupData *gSetup)
{

  std::vector<TAnalysedPulse*> detAPulses = gAnalysedPulseMap[fDetNameA];
 

  //loop through detectors
  for(std::map<std::string, std::vector<TAnalysedPulse*> >::iterator detIter = gAnalysedPulseMap.begin(); detIter != gAnalysedPulseMap.end(); detIter++)
  {
    std::string detname = detIter->first;
    std::string keyname = detIter->first + GetName();
    
    std::vector<TAnalysedPulse*>::iterator currentDetAPulse = detAPulses.begin();

    //  I'm skipping anything not a neutron detector
    if((detname != "NDet") && (detname != "NDet2"))
      continue; 


    float ratioMax = 0.35;


    //initialize the histograms.  /////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////


    if(NGCDisc_plots.find(keyname) == NGCDisc_plots.end())
    {
 
      std::string histname = "h" + detname + "_discrimination";
      std::string histtitle = "Plot of pulse integrals in the " + detname + " detector";
      TH2F* hNGCDisc = new TH2F(histname.c_str(), histtitle.c_str(), 3001, 0, 12500, 1001, 0, 6000);
      hNGCDisc->GetXaxis()->SetTitle("full integral (adc counts)");
      hNGCDisc->GetYaxis()->SetTitle("tail integral (adc counts)");
      NGCDisc_plots[keyname] = hNGCDisc;
    }


    if(NGCRatio_plots.find(keyname) == NGCRatio_plots.end())
    {

     std::string histname2 = "h" + detname + "_ratio";
     std::string histtitle2 = "Plot of pulse integral ratio for the " + detname + " detector";
     TH2F* hNGCRatio = new TH2F(histname2.c_str(), histtitle2.c_str(), 2001, 0, 15, 300, 0, ratioMax);
     hNGCRatio->GetYaxis()->SetTitle("integral ratio");
     hNGCRatio->GetXaxis()->SetTitle("Energy (MeVee)");
     NGCRatio_plots[keyname] = hNGCRatio;
    }
 

    int Emax1 = 0;
    if(detname == 'NDet')
      Emax1 = 15.99;
    else if(detname == 'NDet2')
      Emax1 = 10.53;

 if(NGNCEnergy_plots.find(keyname) == NGNCEnergy_plots.end())
      {
	std::string histname6 = "h" + detname + "_Amplitude";
	std::string histtitle6 = "Plot of Neutron Amplitudes";
	TH1F* hNGNCEnergy = new TH1F(histname6.c_str(), histtitle6.c_str(), 2832, 0, Emax1);
	hNGNCEnergy->GetXaxis()->SetTitle("Energy (MeVee)");
	hNGNCEnergy->GetYaxis()->SetTitle("Count");
	NGNCEnergy_plots[keyname] = hNGNCEnergy;
      }

 if(NGGCEnergy_plots.find(keyname) == NGGCEnergy_plots.end())
      {
	std::string histname6 = "h" + detname + "_AmplitudeG";
	std::string histtitle6 = "Plot of Gamma Amplitudes";
	TH1F* hNGGCEnergy = new TH1F(histname6.c_str(), histtitle6.c_str(), 2832, 0, Emax1);
	hNGGCEnergy->GetXaxis()->SetTitle("Energy (MeVee)");
	hNGGCEnergy->GetYaxis()->SetTitle("Count");
	NGGCEnergy_plots[keyname] = hNGGCEnergy;
      }



 ///////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////

 //  Analyze the waves


    std::vector<TAnalysedPulse*> pulses = detIter->second;

   
    for(std::vector<TAnalysedPulse*>::iterator pIter = pulses.begin(); pIter != pulses.end(); ++pIter)
    {
      double energy = 0;
      double peak = (*pIter)->GetAmplitude();
      double fullInt = (*pIter)->GetIntegral();
      double tailInt = (*pIter)->GetTIntegral();
      double ratio = (*pIter)->GetRatio();

      int pileup = 15000; //in ns
      double timeB = 0;
      double timeA = 0;
      bool coincidence = false;

      if(peak > 5000)
	continue;

      //Scale to MeVee
      if(detname == "NDet")
	energy = (float) (peak+15.2)/177.2;
      if(detname == "NDet2")
	energy = (float) (peak+15)/269.1;

      //get cf time
      timeB = (*pIter)->GetTime();

      // search for timing coincidence

      for(std::vector<TAnalysedPulse*>::iterator pIterA = currentDetAPulse; pIterA != detAPulses.end(); ++pIterA)
	{

	  timeA = (*pIterA)->GetTime();

	  if(timeA > timeB + fStopWindow)
	    {
	      coincidence = false;
	      break;  //we passed the time window, no coincidence found
	    }


	  if(pIterA != detAPulses.begin() && timeA - (*(pIterA - 1))->GetTime() < pileup){
	   
	    continue; //pre-pulse pileup protection (skip pulse)
	    
	  }
	  if(pIterA+1 != detAPulses.end() &&  (*(pIterA + 1))->GetTime() - timeA < pileup){
	
	    continue; //post-pulse pileup protection (skip pulse)
	    
	  }


          double t_diff = timeB - timeA;

	  if((t_diff > fStartWindow) && (t_diff < fStopWindow))
	    {
	      coincidence = true;
	      break;
	    }


	  currentDetAPulse = pIterA;
	}   // end timing coincidence search

      /////////////////////////////////


      if(coincidence == false)
	  continue;  //cut on timing
     

      //filling Histograms

      NGCDisc_plots[keyname]->Fill(fullInt, tailInt);
      NGCRatio_plots[keyname]->Fill(energy, ratio);


      float temp1 = 0.09415 + 0.03783/energy - 0.00557/(energy * energy);
      float temp2 = 0.10815 + 0.03814/energy - 0.00591/(energy * energy);


      if((detname == "NDet") && (ratio > temp1)) 
	NGNCEnergy_plots[keyname]->Fill(energy);
      if((detname == "NDet") && (ratio < temp1)) 
	NGGCEnergy_plots[keyname]->Fill(energy);      
      if((detname == "NDet2") && (ratio > temp2))
	NGNCEnergy_plots[keyname]->Fill(energy);
      if((detname == "NDet2") && (ratio < temp2))
	NGGCEnergy_plots[keyname]->Fill(energy);


 
    }//close pulse loop

  }//close map loop

  return 0;
}


int NGIntTCut::AfterLastEntry(TGlobalData *gData){

  std::string directory = "/" + std::string(fHistName);
  dir-> cd(directory.c_str());
  /*
  for(map<string, vector<TPulseIsland*> >::iterator mapIter = gData->fPulseIslandToChannelMap.begin(); mapIter != gData->fPulseIslandToChannelMap.end(); mapIter++)
  {
    std::string bankname = mapIter->first;
    std::string keyname = mapIter->first + GetName();
    std::string detname;

    if(bankname == "Nf81")
      detname = "NDet2";
    if(bankname == "Ng81")
      detname = "NDet";

    double scale = 2000/11;
    
    //  I'm skipping the other detectors at this point.
    if((detname != "NDet") && (detname != "NDet2"))
      continue;  

    for(int energyIt = 0; energyIt < 22; energyIt++){
      float energy = (float) energyIt / 2;
      int energyBin1 = (energy*scale)+1;
      int energyBin2 = (energy + 0.5)*scale;

      std::stringstream ss;
      std::stringstream st;
      ss << energy + 0.5;
      st << energy;
 
      std::string histname = "h" + detname + "_ratio_" + ss.str();
      std::string histtitle = "Plot of Ratios for the cut " + st.str() + " to " + ss.str() + " MeV";
      
      NGC1D_plots[keyname] = NGCRatio_plots[keyname]->ProjectionY(histname.c_str(), energyBin1, energyBin2);
      NGC1D_plots[keyname]->SetTitle(histtitle.c_str());
      
    }
  }
  */
  return 0;
  
}
