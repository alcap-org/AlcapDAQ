///////////////////////////////////////////////////////
//  NGammaInt        for use in rootana for Alcap    //
//                                                   //
//  Damien Alexander                                 // 
//                                                   //
//  A Module to Discriminate neutron signals from    //
//  gamma signals in a neutron detector, using the   //
//  ratio of the full integral to the tail integral. //
///////////////////////////////////////////////////////

//#define USE_PRINT_OUT 1

#include "NGammaInt.h"
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

std::map<std::string, TH2F*> NGDisc_plots;
std::map<std::string, TH2F*> NGRatio_plots;
std::map<std::string, TH1F*> NGNEnergy_plots, NGGEnergy_plots;
std::map<std::string, TH1D*> NG1D_plots;

NGammaInt::NGammaInt(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName)
  {
    fHistName = HistogramDirectoryName;
    dir->cd("/");
  }

NGammaInt::~NGammaInt()
{
}

int NGammaInt::ProcessEntry(TGlobalData *gData, TSetupData *gSetup)
{

  for (std::map<std::string, std::vector<TAnalysedPulse*> >::iterator detIter = gAnalysedPulseMap.begin(); detIter != gAnalysedPulseMap.end(); detIter++) {

    std::string detname = detIter->first;
    std::string keyname = detIter->first + GetName();

    
    //  I'm skipping the other detectors at this point.
    if((detname != "NDet") && (detname != "NDet2") && (detname != "LiquidSc")){
      continue; 
    }

    //std::cout << bankname << "    " << detname << std::endl;

    float ratioMax = 0.35; //tailcount = 12;



    //initialize the histograms.  ///////////////////////////////


    if(NGDisc_plots.find(keyname) == NGDisc_plots.end())
    {
 
      std::string histname = "h" + detname + "_discrimination";
      std::string histtitle = "Plot of pulse integrals in the " + detname + " detector";
      TH2F* hNGDisc = new TH2F(histname.c_str(), histtitle.c_str(), 3001, 0, 12500, 1001, 0, 6000);
      hNGDisc->GetXaxis()->SetTitle("full integral (adc counts)");
      hNGDisc->GetYaxis()->SetTitle("tail integral (adc counts)");
      NGDisc_plots[keyname] = hNGDisc;
    }



    if(NGRatio_plots.find(keyname) == NGRatio_plots.end())
    {

     std::string histname2 = "h" + detname + "_ratio";
     std::string histtitle2 = "Plot of pulse integral ratio for the " + detname + " detector";
     TH2F* hNGRatio = new TH2F(histname2.c_str(), histtitle2.c_str(), 2001, 0, 15, 300, 0, ratioMax);
     hNGRatio->GetYaxis()->SetTitle("integral ratio");
     hNGRatio->GetXaxis()->SetTitle("Energy (MeVee)");
     NGRatio_plots[keyname] = hNGRatio;
    }


 if(NGNEnergy_plots.find(keyname) == NGNEnergy_plots.end())
      {
	std::string histname6 = "h" + detname + "_Amplitude";
	std::string histtitle6 = "Plot of Neutron Amplitudes";
	TH1F* hNGNEnergy = new TH1F(histname6.c_str(), histtitle6.c_str(), 2832, 0, 16.16);
	hNGNEnergy->GetXaxis()->SetTitle("Energy (MeVee)");
	hNGNEnergy->GetYaxis()->SetTitle("Count");
	NGNEnergy_plots[keyname] = hNGNEnergy;
      }

 if(NGGEnergy_plots.find(keyname) == NGGEnergy_plots.end())
      {
	std::string histname6 = "h" + detname + "_AmplitudeG";
	std::string histtitle6 = "Plot of Gamma Amplitudes";
	TH1F* hNGGEnergy = new TH1F(histname6.c_str(), histtitle6.c_str(), 2832, 0, 16.16);
	hNGGEnergy->GetXaxis()->SetTitle("Energy (MeVee)");
	hNGGEnergy->GetYaxis()->SetTitle("Count");
	NGGEnergy_plots[keyname] = hNGGEnergy;
      }


 ////////////////////////////////////////////////////////

 //  Analyze the waves


    std::vector<TAnalysedPulse*> pulses = detIter->second;

   
    for(std::vector<TAnalysedPulse*>::iterator pIter = pulses.begin(); pIter != pulses.end(); ++pIter)
    {
      double energy = 0;
      double peak = (*pIter)->GetAmplitude();
      double fullInt = (*pIter)->GetIntegral();
      double tailInt = (*pIter)->GetTIntegral();
      double ratio = (*pIter)->GetRatio();

      if(detname == "NDet")
	energy = (float) (peak+15.2)/177.2;
      if(detname == "NDet2")
	energy = (float) (peak+15)/269.1;


      if(peak > 5000)
	{
	  continue;
	}


      // Fill the histograms


      NGDisc_plots[keyname]->Fill(fullInt, tailInt);
      NGRatio_plots[keyname]->Fill(energy, ratio);

      float temp1 = 0.09415 + 0.03783/energy - 0.00557/(energy*energy);
      float temp2 = 0.10815 + 0.03814/energy - 0.00591/(energy*energy);


      if((detname == "NDet") && (ratio >= temp1) && (energy > 0.5) && (energy < 9.0)) 
	NGNEnergy_plots[keyname]->Fill(energy);      
      if((detname == "NDet") && (ratio < temp1) && (energy > 0.5) && (energy < 9.0)) 
	NGGEnergy_plots[keyname]->Fill(energy); 
      if((detname == "NDet2") && (ratio >= temp2) && (energy > 0.5) && (energy < 9.0))
	NGNEnergy_plots[keyname]->Fill(energy);
      if((detname == "NDet2") && (ratio < temp2) && (energy > 0.5) && (energy < 9.0))
	NGGEnergy_plots[keyname]->Fill(energy);



      //cuts should go here

      /////////////////////////////////////////////////////////
      //   now we need to figure out why those signals are   //
      //   showing up weird.  start with condition check,    //
      //   then build histogram or graph (which one?)        //
      //   from our pulse.                                   //
      /////////////////////////////////////////////////////////
      
      /*     
      if((ratio >= 0.12) && (ratio < 0.15) && (neutCount < 20)  && (detname == "NDet") && (peak > 495) && (peak < 500))
	{
          //make histogram here
          neutCount += 1;
	  std::stringstream ss;
          ss << neutCount;
	  std::string histname3 = "h" + ss.str() + "_npulse ";
	  std::string histtitle3 = "Plot of neutron pulse " + ss.str();
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

       
      if((ratio > 0.05) && (ratio < 0.07) && (gammaCount < 20) && (detname == "NDet") && (peak < 500) && (peak > 495))
	{
          //make histogram here
          gammaCount += 1;
	  std::stringstream ss;
          ss << gammaCount;
	  std::string histname4 = "h" + ss.str() + "_gpulse ";
	  std::string histtitle4 = "Plot of gamma pulse " + ss.str();
          TH1F* hPulse2 = new TH1F(histname4.c_str(), histtitle4.c_str(),150, 0, 150);
          hPulse2->GetXaxis()->SetTitle("time");
          hPulse2->GetYaxis()->SetTitle("ADC count");

          for(std::vector<int>::iterator sIt = samples.begin(); sIt != samples.end(); sIt++)
	    {
	      int t1 = std::distance(samples.begin(), sIt);
              hPulse2->Fill(t1 , (*sIt)-1265);
       

            } 

	}
      */
    }//close pulse loop

  }//close map loop

  return 0;
}

int NGammaInt::AfterLastEntry(TGlobalData *gData){


  std::string directory = "/" + std::string(fHistName);
  dir-> cd(directory.c_str());

  for (std::map<std::string, std::vector<TAnalysedPulse*> >::iterator detIter = gAnalysedPulseMap.begin(); detIter != gAnalysedPulseMap.end(); detIter++) {

    std::string detname = detIter->first;
    std::string keyname = detIter->first + GetName();

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
      NG1D_plots[keyname] = NGRatio_plots[keyname]->ProjectionY(histname.c_str(), energyBin1, energyBin2);
      NG1D_plots[keyname]->SetTitle(histtitle.c_str());
  
    }
  }
  return 0;
}
