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
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;


  std::vector<TAnalysedPulse*> detAPulses = gAnalysedPulseMap[fDetNameA];
 

  //loop through detectors
  for(map_iterator mapIter = gData->fPulseIslandToChannelMap.begin(); mapIter != gData->fPulseIslandToChannelMap.end(); mapIter++)
  {
    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);
    std::string keyname = mapIter->first + GetName();
    
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
      TH2F* hNGCDisc = new TH2F(histname.c_str(), histtitle.c_str(), 3000, 0, 12500, 1000, 0, 6000);
      hNGCDisc->GetXaxis()->SetTitle("full integral (adc counts)");
      hNGCDisc->GetYaxis()->SetTitle("tail integral (adc counts)");
      NGCDisc_plots[keyname] = hNGCDisc;
    }


    if(NGCRatio_plots.find(keyname) == NGCRatio_plots.end())
    {

     std::string histname2 = "h" + detname + "_ratio";
     std::string histtitle2 = "Plot of pulse integral ratio for the " + detname + " detector";
     TH2F* hNGCRatio = new TH2F(histname2.c_str(), histtitle2.c_str(), 2000, 0, 11, 300, 0, ratioMax);
     hNGCRatio->GetYaxis()->SetTitle("integral ratio");
     hNGCRatio->GetXaxis()->SetTitle("Energy (MeVee)");
     NGCRatio_plots[keyname] = hNGCRatio;
    }
 


 if(NGNCEnergy_plots.find(keyname) == NGNCEnergy_plots.end())
      {
	std::string histname6 = "h" + detname + "_Amplitude";
	std::string histtitle6 = "Plot of Neutron Amplitudes";
	TH1F* hNGNCEnergy = new TH1F(histname6.c_str(), histtitle6.c_str(), 2832, 0, 16.16);
	hNGNCEnergy->GetXaxis()->SetTitle("Energy (MeVee)");
	hNGNCEnergy->GetYaxis()->SetTitle("Count");
	NGNCEnergy_plots[keyname] = hNGNCEnergy;
      }

 if(NGGCEnergy_plots.find(keyname) == NGGCEnergy_plots.end())
      {
	std::string histname6 = "h" + detname + "_AmplitudeG";
	std::string histtitle6 = "Plot of Gamma Amplitudes";
	TH1F* hNGGCEnergy = new TH1F(histname6.c_str(), histtitle6.c_str(), 2832, 0, 16.16);
	hNGGCEnergy->GetXaxis()->SetTitle("Energy (MeVee)");
	hNGGCEnergy->GetYaxis()->SetTitle("Count");
	NGGCEnergy_plots[keyname] = hNGGCEnergy;
      }



 ///////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////

 //  Analyze the waves


    std::vector<TPulseIsland*> pulses = mapIter->second;

    if(pulses.size() == 0)
      continue;          // skip if no pulse

   
    for(std::vector<TPulseIsland*>::iterator pIter = pulses.begin(); pIter != pulses.end(); pIter++)
    {
      std::vector<int> samples = (*pIter)->GetSamples();
    
     
      float sum = 0, pedestal = 0, peak = 0, energy = 0, tpeak = 0;
      int count = 10;
      float timeB = 0, timeA = 0;
      bool coincidence = false;
      double fullInt = 0, tailInt = 0;
      double ratio = 0;  
      int pileup = 15000; //in ns


      float constant_fraction = 0.50;
      //int trigger_polarity = gSetup->GetTriggerPolarity(bankname);
      double clock_tick_in_ns = gSetup->GetClockTick(bankname);
      double time_shift = gSetup->GetTimeShift(bankname);


      //corrections to time shifts
      if(detname == "NDet")
	time_shift += 192;
      if(detname == "NDet2")
	time_shift += 34;



      for(std::vector<int>::iterator sampIt = samples.begin(); sampIt != samples.begin() + count; sampIt++)
	sum += (*sampIt);
      
      pedestal = sum / count;




      const std::vector<int>::const_iterator b = samples.begin(), e = samples.end();

      //std::vector<int>::const_iterator m = trigger_polarity > 0 ? std::max_element(b, e) : std::min_element(b, e); // find peak position

      std::vector<int>::const_iterator m = std::max_element(b, e);

      peak = *m - pedestal;
      tpeak = m-b;

      if(peak > 5000)
	{
	  continue; // overflow cut
	}


      //Fill values for our integrals

      for(std::vector<int>::const_iterator samp = m-3; samp != m+20; samp++){
	fullInt += *(samp) - pedestal;
	if(samp >= m+5){
	  tailInt += *(samp) - pedestal;
	}
      }
     

      ratio = tailInt / fullInt;


      //Scale to MeV
      if(detname == "NDet")
	energy = (peak+15.2)/177.2;
      if(detname == "NDet2")
	energy = (peak+15)/269.1;



      //search for constant_fraction time
      const unsigned int cf = (constant_fraction*(double)(peak-pedestal)) + pedestal;

      while(m != b && (*--m > (int)cf));
      double dx = (double)(m-b); // determine how many clock ticks we are in
      if(*(m+1) != *m)
	dx += (double)((int)cf - *m)/(double)(*(m+1) - *m);;
         //calculate shift to actual cf, linear interpolation
         //replace with spline interpolation time permitting (to get lower cf)

    //get cf time
      timeB = (dx + (double)(*pIter)->GetTimeStamp()) * clock_tick_in_ns - time_shift;
      //timeB = (tpeak + (*pIter)->GetTimeStamp()) * clock_tick_in_ns - time_shift;

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


      float temp1 = 0.09579 + 0.02932/energy - 0.00385/(energy * energy);
      float temp2 = 0.10864 + 0.03444/energy - 0.00645/(energy*energy);


      if((detname == "NDet") && (ratio > temp1)) 
	NGNCEnergy_plots[keyname]->Fill(energy);
      if((detname == "NDet") && (ratio < temp1)) 
	NGGCEnergy_plots[keyname]->Fill(energy);      
      if((detname == "NDet2") && (ratio > temp2))
	NGNCEnergy_plots[keyname]->Fill(energy);
      if((detname == "NDet2") && (ratio < temp2))
	NGGCEnergy_plots[keyname]->Fill(energy);




      //plot waveforms


     
      
      if((ratio >= 0.06) && (ratio < 0.085) && (neutCount < 20)  && (detname == "NDet") && (energy > 3.5) && (energy < 4.0))
	{
          //make histogram here
          neutCount += 1;
	  std::stringstream ss;
          ss << neutCount;
	  std::string histname3 = "h" + ss.str() + "_npulse ";
	  std::string histtitle3 = "Plot of gamma pulse " + ss.str() + "w/ time cut";
          TH1F* hPulse = new TH1F(histname3.c_str(), histtitle3.c_str(),150, 0, 150);
          hPulse->GetXaxis()->SetTitle("time");
          hPulse->GetYaxis()->SetTitle("ADC count");
	  hPulse->SetFillStyle(0);

          for(std::vector<int>::iterator sIt = samples.begin(); sIt != samples.end(); sIt++)
	    {
	      int t = std::distance(samples.begin(), sIt);	      
	      hPulse->Fill(t, *(sIt)-1265);
            } 


	}

       
      if((ratio > 0.06) && (ratio < 0.085) && (gammaCount < 20) && (detname == "NDet") && (energy < 5.0) && (energy > 4.5))
	{
          //make histogram here
          gammaCount += 1;
	  std::stringstream ss;
          ss << gammaCount;
	  std::string histname4 = "h" + ss.str() + "_gpulse ";
	  std::string histtitle4 = "Plot of gamma pulse " + ss.str() + "w/o time cuts";
          TH1F* hPulse2 = new TH1F(histname4.c_str(), histtitle4.c_str(),150, 0, 150);
          hPulse2->GetXaxis()->SetTitle("time");
          hPulse2->GetYaxis()->SetTitle("ADC count");

          for(std::vector<int>::iterator sIt = samples.begin(); sIt != samples.end(); sIt++)
	    {
	      int t1 = std::distance(samples.begin(), sIt);
              hPulse2->Fill(t1 , (*sIt)-1265);
       

            } 

	}
      

 
    }//close pulse loop

  }//close map loop

  return 0;
}

int NGIntTCut::AfterLastEntry(TGlobalData *gData){

  std::string directory = "/" + std::string(fHistName);
  dir-> cd(directory.c_str());

  for(map<string, vector<TPulseIsland*> >::iterator mapIter = gData->fPulseIslandToChannelMap.begin(); mapIter != gData->fPulseIslandToChannelMap.end(); mapIter++)
  {
    std::string bankname = mapIter->first;
    std::string keyname = mapIter->first + GetName();
    std::string detname;

    if(bankname == "Nf81")
      detname = "NDet2";
    if(bankname == "Ng81")
      detname = "NDet";

    double scale = 2000/15;
    
    //  I'm skipping the other detectors at this point.
    if((detname != "NDet") && (detname != "NDet2"))
      continue;  

    for(int energyIt = 0; energyIt < 30; energyIt++){
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
      NGC1D_plots[keyname] = NGCRatio_plots[keyname]->ProjectionY(histname.c_str(), energyBin1, energyBin2);
      NGC1D_plots[keyname]->SetTitle(histtitle.c_str());
  
    }
  }
  
}
