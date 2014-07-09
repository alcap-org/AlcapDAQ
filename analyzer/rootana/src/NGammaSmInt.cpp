///////////////////////////////////////////////////////
//  NGammaSmInt        for use in rootana for Alcap    //
//                                                   //
//  Damien Alexander                                 // 
//                                                   //
//  A Module to Discriminate neutron signals from    //
//  gamma signals in a neutron detector, using the   //
//  ratio of the full integral to the tail integral. //
///////////////////////////////////////////////////////

//#define USE_PRINT_OUT 1

#include "NGammaSmInt.h"
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


std::map<std::string, TH2F*> NGSmDisc_plots;
std::map<std::string, TH2F*> NGSmRatio_plots;
std::map<std::string, TH1F*> NGSm1D_plot_05, NGSm1D_plot_10, NGSm1D_plot_15, NGSm1D_plot_20, NGSm1D_plot_25, NGSm1D_plot_30, NGSm1D_plot_35, NGSm1D_plot_40;
std::map<std::string, TH1F*> NGSmEnergy_plots;

NGammaSmInt::NGammaSmInt(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName)
  {
    dir->cd("/");
  }

NGammaSmInt::~NGammaSmInt()
{
}

int NGammaSmInt::ProcessEntry(TGlobalData *gData, TSetupData *gSetup)
{
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  for(map_iterator mapIter = gData->fPulseIslandToChannelMap.begin(); mapIter != gData->fPulseIslandToChannelMap.end(); mapIter++)
  {
    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);
    std::string keyname = mapIter->first + GetName();
    
    //  I'm skipping the other detectors at this point.
    if((detname != "NDet") && (detname != "NDet2") && (detname != "LiquidSc"))
      continue; 


    float ratioMax = 0.6, tailRatio = 0.35;



    //initialize the histograms.  ///////////////////////////////


    if(NGSmDisc_plots.find(keyname) == NGSmDisc_plots.end())
    {
 
      std::string histname = "h" + detname + "_discrimination";
      std::string histtitle = "Plot of smoothed pulse integrals in the " + detname + " detector";
      TH2F* hNGDisc = new TH2F(histname.c_str(), histtitle.c_str(), 3000, 0, 12500, 1000, 0, 6000);
      hNGDisc->GetXaxis()->SetTitle("full integral (adc counts)");
      hNGDisc->GetYaxis()->SetTitle("tail integral (adc counts)");
      NGSmDisc_plots[keyname] = hNGDisc;
    }


    if(NGSmRatio_plots.find(keyname) == NGSmRatio_plots.end())
    {

     std::string histname2 = "h" + detname + "_ratio";
     std::string histtitle2 = "Plot of smoothed pulse integral ratio for the " + detname + " detector";
     TH2F* hNGRatio = new TH2F(histname2.c_str(), histtitle2.c_str(), 300, 0, ratioMax, 2000, 0, 15);
     hNGRatio->GetXaxis()->SetTitle("integral ratio");
     hNGRatio->GetYaxis()->SetTitle("Energy (MeVee)");
     NGSmRatio_plots[keyname] = hNGRatio;
    }
 
    // there may be some easier way of doing this, but for the limited
    // number of histograms, this suffices.  

    if(NGSm1D_plot_05.find(keyname) == NGSm1D_plot_05.end())
      {
	std::string histname1 = "h" + detname + "_FoM_05";
	std::string histtitle1 = "Plot of Ratios for Cut 0.0 - 0.5 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGSm1D_plot_05[keyname] = hNG1D;
      }


    if(NGSm1D_plot_10.find(keyname) == NGSm1D_plot_10.end())
      {
	std::string histname1 = "h" + detname + "_FoM_10";
	std::string histtitle1 = "Plot of Ratios for Cut 0.5 - 1.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGSm1D_plot_10[keyname] = hNG1D;
      }

    if(NGSm1D_plot_15.find(keyname) == NGSm1D_plot_15.end())
      {
	std::string histname1 = "h" + detname + "_FoM_15";
	std::string histtitle1 = "Plot of Ratios for Cut 1.0 - 1.5MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGSm1D_plot_15[keyname] = hNG1D;
      }


    if(NGSm1D_plot_20.find(keyname) == NGSm1D_plot_20.end())
      {
	std::string histname1 = "h" + detname + "_FoM_20";
	std::string histtitle1 = "Plot of Ratios for Cut 1.5 - 2.0MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGSm1D_plot_20[keyname] = hNG1D;
      }

    if(NGSm1D_plot_25.find(keyname) == NGSm1D_plot_25.end())
      {
	std::string histname1 = "h" + detname + "_FoM_25";
	std::string histtitle1 = "Plot of Ratios for Cut 2.0 - 2.5MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGSm1D_plot_25[keyname] = hNG1D;
      }

    if(NGSm1D_plot_30.find(keyname) == NGSm1D_plot_30.end())
      {
	std::string histname1 = "h" + detname + "_FoM_30";
	std::string histtitle1 = "Plot of Ratios for Cut 2.5 - 3.0MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGSm1D_plot_30[keyname] = hNG1D;
      }

    if(NGSm1D_plot_35.find(keyname) == NGSm1D_plot_35.end())
      {
	std::string histname1 = "h" + detname + "_FoM_35";
	std::string histtitle1 = "Plot of Ratios for Cut 3.0 - 3.5MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGSm1D_plot_35[keyname] = hNG1D;
      }

    if(NGSm1D_plot_40.find(keyname) == NGSm1D_plot_40.end())
      {
	std::string histname1 = "h" + detname + "_FoM_40";
	std::string histtitle1 = "Plot of Ratios for Cut 3.5 - 4.0MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGSm1D_plot_40[keyname] = hNG1D;
      }


 if(NGSmEnergy_plots.find(keyname) == NGSmEnergy_plots.end())
      {
	std::string histname6 = "h" + detname + "_Amplitude";
	std::string histtitle6 = "Plot of Neutron Amplitudes";
	TH1F* hNGEnergy = new TH1F(histname6.c_str(), histtitle6.c_str(), 2832, 0, 16.16);
	hNGEnergy->GetXaxis()->SetTitle("Energy (MeVee)");
	hNGEnergy->GetYaxis()->SetTitle("Count");
	NGSmEnergy_plots[keyname] = hNGEnergy;
      }



 ////////////////////////////////////////////////////////

 //  Analyze the waves


    std::vector<TPulseIsland*> pulses = mapIter->second;

    if(pulses.size() == 0)
      continue;          // skip if no pulse

   
    for(std::vector<TPulseIsland*>::iterator pIter = pulses.begin(); pIter != pulses.end(); pIter++)
    {
      std::vector<int> samples = (*pIter)->GetSamples();
    
      float sum = 0, pedestal = 0;
      int count = 5; 
      float peak = 0, tstart = 0, tstop = 0, ttail = 0;
      double fullInt = 0, tailInt = 0;
      double ratio = 0;  


      TH1F* hpulse = new TH1F("SmSignal", "Plot of pulse shape", 150, 0, 150);
      hpulse->GetXaxis()->SetTitle("time");
      hpulse->GetYaxis()->SetTitle("ADC Count");

  

      for(std::vector<int>::iterator sampIt = samples.begin(); sampIt != samples.begin() + count; sampIt++)
	sum += (*sampIt);
      
      pedestal = sum / count;


      for(std::vector<int>::iterator sIter = samples.begin(); sIter != samples.end(); sIter++)
	{
	  float samp = (*sIter) - pedestal;
	  int time = std::distance(samples.begin(), sIter);

	  if((sIter +1) == samples.end())
	    tstop = time;
 
	  hpulse->Fill(time, samp);
	}

      hpulse->Smooth(2);

      for(int i = 0; i < tstop; i++) 
      {

        float samp = hpulse->GetBinContent(i);
	float threshold = 7;
	int time = i;


	//Get the timing for our pulse
 
	if((samp > threshold) && (tstart == 0) && (time > 26)) 
	  tstart = time; 

	if((tstart != 0) && (samp > peak))
	  peak = samp;       //find peak from max height (revisit)

	if((tstart != 0) && (ttail == 0) && (samp < (tailRatio * peak)))
	  ttail = time;
       
      } //closing sample loop



      if(peak > 5000)
	{
	  delete hpulse;
	  continue;
	}


      fullInt = hpulse->Integral(tstart, tstop);
      tailInt = hpulse->Integral(ttail, tstop);

      ratio = tailInt / fullInt;

      delete hpulse;

      // Fill the histograms

      float energy = 0;
      if(detname == "NDet")
	energy = peak * 0.00575;
      if(detname == "NDet2")
	energy = peak * 0.00399;

      NGSmDisc_plots[keyname]->Fill(fullInt, tailInt);
      NGSmRatio_plots[keyname]->Fill(ratio, energy);

      if(energy < 0.5)
	NGSm1D_plot_05[keyname]->Fill(ratio);
      if((energy > 0.5) && (energy < 1.0))
	NGSm1D_plot_10[keyname]->Fill(ratio);	  
      if((energy > 1.0) && (energy < 1.5))
	NGSm1D_plot_15[keyname]->Fill(ratio);
      if((energy > 1.5) && (energy < 2.0))
	NGSm1D_plot_20[keyname]->Fill(ratio);
      if((energy > 2.0) && (energy < 2.5))
	NGSm1D_plot_25[keyname]->Fill(ratio);
      if((energy > 2.5) && (energy < 3.0))
	NGSm1D_plot_30[keyname]->Fill(ratio);
      if((energy > 3.0) && (energy < 3.5))
	NGSm1D_plot_35[keyname]->Fill(ratio);
      if((energy > 3.5) && (energy < 4.0))
	NGSm1D_plot_40[keyname]->Fill(ratio);

      if((ratio > 0.07) && (energy > 1.0) && (energy < 2.0)) 
	NGSmEnergy_plots[keyname]->Fill(0.00571 * peak);      
      if((ratio > 0.06) && (energy > 2.0))
	NGSmEnergy_plots[keyname]->Fill(0.00571 * peak);
      

      //cuts should go here


    }//close pulse loop

  }//close map loop

  return 0;
}
