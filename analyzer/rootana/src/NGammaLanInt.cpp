///////////////////////////////////////////////////////
//  NGammaLanInt        for use in rootana for Alcap    //
//                                                   //
//  Damien Alexander                                 // 
//                                                   //
//  A Module to Discriminate neutron signals from    //
//  gamma signals in a neutron detector, using the   //
//  ratio of the full integral to the tail integral. //
///////////////////////////////////////////////////////

//#define USE_PRINT_OUT 1

#include "NGammaLanInt.h"
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
#include "TF1.h"
#include "TH2F.h"
#include "TH1.h"
#include <sstream>

using std::string;
using std::map;
using std::vector;
using std::pair;


std::map<std::string, TH2F*> NGLanDisc_plots;
std::map<std::string, TH2F*> NGLanRatio_plots;
std::map<std::string, TH1F*> NGLan1D_plot_05, NGLan1D_plot_10, NGLan1D_plot_15, NGLan1D_plot_20, NGLan1D_plot_25, NGLan1D_plot_30, NGLan1D_plot_35, NGLan1D_plot_40;
std::map<std::string, TH1F*> NGLanEnergy_plots;

NGammaLanInt::NGammaLanInt(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName)
  {
    dir->cd("/");
  }

NGammaLanInt::~NGammaLanInt()
{
}

int NGammaLanInt::ProcessEntry(TGlobalData *gData, TSetupData *gSetup)
{
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  for(map_iterator mapIter = gData->fPulseIslandToChannelMap.begin(); mapIter != gData->fPulseIslandToChannelMap.end(); mapIter++)
  {
    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);
    std::string keyname = mapIter->first + GetName();
    
    //  I'm skipping the other detectors at this point.
    if((detname != "NDet") && (detname != "NDet2"))
      continue; 



    //initialize the histograms.  ///////////////////////////////


    if(NGLanDisc_plots.find(keyname) == NGLanDisc_plots.end())
    {
 
      std::string histname = "h" + detname + "_discrimination";
      std::string histtitle = "Plot of smoothed pulse integrals in the " + detname + " detector";
      TH2F* hNGDisc = new TH2F(histname.c_str(), histtitle.c_str(), 3000, 0, 12500, 1000, 0, 6000);
      hNGDisc->GetXaxis()->SetTitle("full integral (adc counts)");
      hNGDisc->GetYaxis()->SetTitle("tail integral (adc counts)");
      NGLanDisc_plots[keyname] = hNGDisc;
    }


    if(NGLanRatio_plots.find(keyname) == NGLanRatio_plots.end())
    {

     std::string histname2 = "h" + detname + "_ratio";
     std::string histtitle2 = "Plot of smoothed pulse integral ratio for the " + detname + " detector";
     TH2F* hNGRatio = new TH2F(histname2.c_str(), histtitle2.c_str(), 300, 0, 0.25, 2000, 0, 10);
     hNGRatio->GetXaxis()->SetTitle("integral ratio");
     hNGRatio->GetYaxis()->SetTitle("Energy (MeVee)");
     NGLanRatio_plots[keyname] = hNGRatio;
    }
 
    // there may be some easier way of doing this, but for the limited
    // number of histograms, this suffices.  

    if(NGLan1D_plot_05.find(keyname) == NGLan1D_plot_05.end())
      {
	std::string histname1 = "h" + detname + "_FoM_05";
	std::string histtitle1 = "Plot of Ratios for Cut 0.0 - 0.5 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, 0.25);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGLan1D_plot_05[keyname] = hNG1D;
      }


    if(NGLan1D_plot_10.find(keyname) == NGLan1D_plot_10.end())
      {
	std::string histname1 = "h" + detname + "_FoM_10";
	std::string histtitle1 = "Plot of Ratios for Cut 0.5 - 1.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, 0.25);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGLan1D_plot_10[keyname] = hNG1D;
      }

    if(NGLan1D_plot_15.find(keyname) == NGLan1D_plot_15.end())
      {
	std::string histname1 = "h" + detname + "_FoM_15";
	std::string histtitle1 = "Plot of Ratios for Cut 1.0 - 1.5MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, 0.25);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGLan1D_plot_15[keyname] = hNG1D;
      }


    if(NGLan1D_plot_20.find(keyname) == NGLan1D_plot_20.end())
      {
	std::string histname1 = "h" + detname + "_FoM_20";
	std::string histtitle1 = "Plot of Ratios for Cut 1.5 - 2.0MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, 0.25);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGLan1D_plot_20[keyname] = hNG1D;
      }

    if(NGLan1D_plot_25.find(keyname) == NGLan1D_plot_25.end())
      {
	std::string histname1 = "h" + detname + "_FoM_25";
	std::string histtitle1 = "Plot of Ratios for Cut 2.0 - 2.5MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, 0.25);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGLan1D_plot_25[keyname] = hNG1D;
      }

    if(NGLan1D_plot_30.find(keyname) == NGLan1D_plot_30.end())
      {
	std::string histname1 = "h" + detname + "_FoM_30";
	std::string histtitle1 = "Plot of Ratios for Cut 2.5 - 3.0MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, 0.25);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGLan1D_plot_30[keyname] = hNG1D;
      }

    if(NGLan1D_plot_35.find(keyname) == NGLan1D_plot_35.end())
      {
	std::string histname1 = "h" + detname + "_FoM_35";
	std::string histtitle1 = "Plot of Ratios for Cut 3.0 - 3.5MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, 0.25);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGLan1D_plot_35[keyname] = hNG1D;
      }

    if(NGLan1D_plot_40.find(keyname) == NGLan1D_plot_40.end())
      {
	std::string histname1 = "h" + detname + "_FoM_40";
	std::string histtitle1 = "Plot of Ratios for Cut 3.5 - 4.0MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, 0.25);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGLan1D_plot_40[keyname] = hNG1D;
      }


    if(NGLanEnergy_plots.find(keyname) == NGLanEnergy_plots.end())
      {
	std::string histname6 = "h" + detname + "_Amplitude";
	std::string histtitle6 = "Plot of Neutron Amplitudes";
	TH1F* hNGEnergy = new TH1F(histname6.c_str(), histtitle6.c_str(), 2832, 0, 16.16);
	hNGEnergy->GetXaxis()->SetTitle("Energy (MeVee)");
	hNGEnergy->GetYaxis()->SetTitle("Count");
	NGLanEnergy_plots[keyname] = hNGEnergy;
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
      float peak = 0, tstart = 0, tstop = 0, ttail = 0, tpeak = 0;
      double fullInt = 0, tailInt = 0;
      double ratio = 0, tail = 0;  


      TH1F* hpulse = new TH1F("fSignal", "Plot of pulse shape with fit", 150, 0, 150);
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

      TF1* fPulse = new TF1("fPulse", "landau", 0., 150.);
      fPulse->SetParameters(100,30,1);

      hpulse->Fit("fPulse", "Q", "", 0., 150.);

      peak = fPulse->GetMaximum(26, tstop);
      tpeak = fPulse->GetMaximumX(26, tstop);
      tail = 0.05 * peak;

      tstart = fPulse->GetX(2, 26, tpeak);
      ttail = fPulse->GetX(tail, tpeak, tstop);

      fullInt = fPulse->Integral(tstart, tstop);
      tailInt = fPulse->Integral(ttail, tstop);

      ratio = tailInt / fullInt;


      if(peak > 5000)
	{
	  delete hpulse;
	  continue;
	}

      delete hpulse;
      delete fPulse;

      // Fill the histograms

      float energy = 0;
      if(detname == "NDet")
	energy = peak * 0.00575;
      if(detname == "NDet2")
	energy = peak * 0.00399;

      NGLanDisc_plots[keyname]->Fill(fullInt, tailInt);
      NGLanRatio_plots[keyname]->Fill(ratio, energy);

      if(energy < 0.5)
	NGLan1D_plot_05[keyname]->Fill(ratio);
      if((energy > 0.5) && (energy < 1.0))
	NGLan1D_plot_10[keyname]->Fill(ratio);	  
      if((energy > 1.0) && (energy < 1.5))
	NGLan1D_plot_15[keyname]->Fill(ratio);
      if((energy > 1.5) && (energy < 2.0))
	NGLan1D_plot_20[keyname]->Fill(ratio);
      if((energy > 2.0) && (energy < 2.5))
	NGLan1D_plot_25[keyname]->Fill(ratio);
      if((energy > 2.5) && (energy < 3.0))
	NGLan1D_plot_30[keyname]->Fill(ratio);
      if((energy > 3.0) && (energy < 3.5))
	NGLan1D_plot_35[keyname]->Fill(ratio);
      if((energy > 3.5) && (energy < 4.0))
	NGLan1D_plot_40[keyname]->Fill(ratio);

      if((ratio > 0.07) && (energy > 1.0) && (energy < 2.0)) 
	NGLanEnergy_plots[keyname]->Fill(0.00571 * peak);      
      if((ratio > 0.06) && (energy > 2.0))
	NGLanEnergy_plots[keyname]->Fill(0.00571 * peak);
      

      //cuts should go here


    }//close pulse loop

  }//close map loop

  return 0;
}
