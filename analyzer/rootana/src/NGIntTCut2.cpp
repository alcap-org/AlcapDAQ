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

#include "NGIntTCut2.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <cmath>

#include "TAnalysedPulse.h"
#include "TH2F.h"
#include "TH1.h"
#include <sstream>

using std::string;
using std::map;
using std::vector;
using std::pair;

extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;

std::map<std::string, TH2F*> NGC2Disc_plots;
std::map<std::string, TH2F*> NGC2Ratio_plots;
std::map<std::string, TH1F*> NGNC2Energy_plots, NGGC2Energy_plots;
std::map<std::string, TH1D*> NGC21D_plots;

NGIntTCut2::NGIntTCut2(char *HistogramDirectoryName, std::string detnameA, double start_window, double stop_window) :
  FillHistBase(HistogramDirectoryName)
  {
    fDetNameA = detnameA;
    fStartWindow = start_window;
    fStopWindow = stop_window;
    fHistName = HistogramDirectoryName;
    dir->cd("/");
  }

NGIntTCut2::~NGIntTCut2()
{
}

int NGIntTCut2::ProcessEntry(TGlobalData *gData, TSetupData *gSetup)
{

  std::vector<TAnalysedPulse*> detAPulses = gAnalysedPulseMap[fDetNameA];
 

  //loop through detectors
  for (std::map<std::string, std::vector<TAnalysedPulse*> >::iterator detIter = gAnalysedPulseMap.begin(); detIter != gAnalysedPulseMap.end(); detIter++) {

    std::string detname = detIter->first;
    std::string keyname = detIter->first + GetName();

    
    std::vector<TAnalysedPulse*>::iterator currentDetAPulse = detAPulses.begin();

    //  I'm skipping anything not a neutron detector
    if((detname != "NDet") && (detname != "NDet2"))
      continue; 


    float ratioMax = 0.35;


    //initialize the histograms.  /////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////


    if(NGC2Disc_plots.find(keyname) == NGC2Disc_plots.end())
    {
 
      std::string histname = "h" + detname + "_discrimination";
      std::string histtitle = "Plot of pulse integrals in the " + detname + " detector";
      TH2F* hNGCDisc = new TH2F(histname.c_str(), histtitle.c_str(), 3000, 0, 12500, 1000, 0, 6000);
      hNGCDisc->GetXaxis()->SetTitle("full integral (adc counts)");
      hNGCDisc->GetYaxis()->SetTitle("tail integral (adc counts)");
      NGC2Disc_plots[keyname] = hNGCDisc;
    }


    if(NGC2Ratio_plots.find(keyname) == NGC2Ratio_plots.end())
    {

     std::string histname2 = "h" + detname + "_ratio";
     std::string histtitle2 = "Plot of pulse integral ratio for the " + detname + " detector";
     TH2F* hNGCRatio = new TH2F(histname2.c_str(), histtitle2.c_str(), 2000, 0, 11, 300, 0, ratioMax);
     hNGCRatio->GetYaxis()->SetTitle("integral ratio");
     hNGCRatio->GetXaxis()->SetTitle("Energy (MeVee)");
     NGC2Ratio_plots[keyname] = hNGCRatio;
    }
 

    int Emax1 = 0;
    if(detname == "NDet")
      Emax1 = 15.99;
    else if(detname == "NDet2")
      Emax1 = 10.53;

    if(NGNC2Energy_plots.find(keyname) == NGNC2Energy_plots.end())
      {
	std::string histname6 = "h" + detname + "_Amplitude";
	std::string histtitle6 = "Plot of Neutron Amplitudes";
	TH1F* hNGNCEnergy = new TH1F(histname6.c_str(), histtitle6.c_str(), 2832, 0, Emax1);
	hNGNCEnergy->GetXaxis()->SetTitle("Energy (MeVee)");
	hNGNCEnergy->GetYaxis()->SetTitle("Count");
	NGNC2Energy_plots[keyname] = hNGNCEnergy;
      }

    if(NGGC2Energy_plots.find(keyname) == NGGC2Energy_plots.end())
      {
	std::string histname6 = "h" + detname + "_AmplitudeG";
	std::string histtitle6 = "Plot of Gamma Amplitudes";
	TH1F* hNGGCEnergy = new TH1F(histname6.c_str(), histtitle6.c_str(), 2832, 0, Emax1);
	hNGGCEnergy->GetXaxis()->SetTitle("Energy (MeVee)");
	hNGGCEnergy->GetYaxis()->SetTitle("Count");
	NGGC2Energy_plots[keyname] = hNGGCEnergy;
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

      float timeB = (*pIter)->GetTime();
      float timeA = 0;
      bool coincidence = false;

      int pileup = 15000; //in ns


      if(peak > 5000)
	{
	  continue; // overflow cut
	}


      //Scale to MeV
      if(detname == "NDet")
	energy = (peak+15.2)/177.2;
      if(detname == "NDet2")
	energy = (peak+15)/269.1;

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

      NGC2Disc_plots[keyname]->Fill(fullInt, tailInt);
      NGC2Ratio_plots[keyname]->Fill(energy, ratio);


      float temp1 = 0.09579 + 0.02932/energy - 0.00385/(energy * energy);
      float temp2 = 0.10864 + 0.03444/energy - 0.00645/(energy*energy);


      if((detname == "NDet") && (ratio > temp1)) 
	NGNC2Energy_plots[keyname]->Fill(energy);
      if((detname == "NDet") && (ratio < temp1)) 
	NGGC2Energy_plots[keyname]->Fill(energy);      
      if((detname == "NDet2") && (ratio > temp2))
	NGNC2Energy_plots[keyname]->Fill(energy);
      if((detname == "NDet2") && (ratio < temp2))
	NGGC2Energy_plots[keyname]->Fill(energy);

      

 
    }//close pulse loop

  }//close map loop

  return 0;
}

int NGIntTCut2::AfterLastEntry(TGlobalData *gData){

  std::string directory = "/" + std::string(fHistName);
  dir-> cd(directory.c_str());

  for (std::map<std::string, std::vector<TAnalysedPulse*> >::iterator detIter = gAnalysedPulseMap.begin(); detIter != gAnalysedPulseMap.end(); detIter++) {

    std::string detname = detIter->first;
    std::string keyname = detIter->first + GetName();

    double scale = 2000/15;
    
    //  I'm skipping the other detectors at this point.
    if((detname != "NDet") && (detname != "NDet2"))
      continue;  

    for(int energyIt = 0; energyIt < 20; energyIt++){
      float energy = (float) energyIt / 2;
      int energyBin1 = 0, energyBin2 = 0;
      if(detname == "NDet"){
	energyBin1 = (energy*scale)+1;
	energyBin2 = ((energy + 0.5)*scale) ;
      }
      if(detname == "NDet2"){
	energyBin1 = (energy*scale)+1;
	energyBin2 = ((energy+0.5)*scale) ;
      }
     

      std::stringstream ss;
      std::stringstream st;
      ss << energy + 0.5;
      st << energy;
 
      std::string histname = "h" + detname + "_ratio_" + ss.str();
      std::string histtitle = "Plot of Ratios for the cut " + st.str() + " to " + ss.str() + " MeV";
      NGC21D_plots[keyname] = NGC2Ratio_plots[keyname]->ProjectionY(histname.c_str(), energyBin1, energyBin2);
      NGC21D_plots[keyname]->SetTitle(histtitle.c_str());
  
    }
  }
  
}
