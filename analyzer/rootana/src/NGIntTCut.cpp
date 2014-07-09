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
std::map<std::string, TH1F*> NGC1D_plot_04, NGC1D_plot_06, NGC1D_plot_08, NGC1D_plot_10, NGC1D_plot_12, NGC1D_plot_14, NGC1D_plot_16, NGC1D_plot_18, NGC1D_plot_20, NGC1D_plot_35, NGC1D_plot_50, NGC1D_plot_70, NGC1D_plot_90, NGC1D_plot_110, NGC1D_plot_150;
std::map<std::string, TH1F*> NGCEnergy_plots;

NGIntTCut::NGIntTCut(char *HistogramDirectoryName, std::string detnameA, double start_window, double stop_window) :
  FillHistBase(HistogramDirectoryName)
  {
    fDetNameA = detnameA;
    fStartWindow = start_window;
    fStopWindow = stop_window;
    dir->cd("/");
  }

NGIntTCut::~NGIntTCut()
{
}

int NGIntTCut::ProcessEntry(TGlobalData *gData, TSetupData *gSetup)
{
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
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
    if((detname != "NDet") && (detname != "NDet2") && (detname!= "LiquidSc"))
      continue; 


    float ratioMax = 0.2, tailcount = 12;


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
     TH2F* hNGCRatio = new TH2F(histname2.c_str(), histtitle2.c_str(), 300, 0, ratioMax, 2000, 0, 10);
     hNGCRatio->GetXaxis()->SetTitle("integral ratio");
     hNGCRatio->GetYaxis()->SetTitle("Energy (MeVee)");
     NGCRatio_plots[keyname] = hNGCRatio;
    }
 
    // there may be some easier way of doing this.  Need to find 

    if(NGC1D_plot_04.find(keyname) == NGC1D_plot_04.end())
      {
	std::string histname1 = "h" + detname + "_FoM_04";
	std::string histtitle1 = "Plot of Ratios for Cut 0.0 - 0.4 MeV";
	TH1F* hNGC1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNGC1D->GetXaxis()->SetTitle("Integral Ratio");
	hNGC1D->GetYaxis()->SetTitle("Count");
	NGC1D_plot_04[keyname] = hNGC1D;
      }


    if(NGC1D_plot_06.find(keyname) == NGC1D_plot_06.end())
      {
	std::string histname1 = "h" + detname + "_FoM_06";
	std::string histtitle1 = "Plot of Ratios for Cut 0.4 - 0.6 MeV";
	TH1F* hNGC1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNGC1D->GetXaxis()->SetTitle("Integral Ratio");
	hNGC1D->GetYaxis()->SetTitle("Count");
	NGC1D_plot_06[keyname] = hNGC1D;
      }

    if(NGC1D_plot_08.find(keyname) == NGC1D_plot_08.end())
      {
	std::string histname1 = "h" + detname + "_FoM_08";
	std::string histtitle1 = "Plot of Ratios for Cut 0.6 - 0.8 MeV";
	TH1F* hNGC1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNGC1D->GetXaxis()->SetTitle("Integral Ratio");
	hNGC1D->GetYaxis()->SetTitle("Count");
	NGC1D_plot_08[keyname] = hNGC1D;
      }


    if(NGC1D_plot_10.find(keyname) == NGC1D_plot_10.end())
      {
	std::string histname1 = "h" + detname + "_FoM_10";
	std::string histtitle1 = "Plot of Ratios for Cut 0.8 - 1.0MeV";
	TH1F* hNGC1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNGC1D->GetXaxis()->SetTitle("Integral Ratio");
	hNGC1D->GetYaxis()->SetTitle("Count");
	NGC1D_plot_10[keyname] = hNGC1D;
      }

    if(NGC1D_plot_12.find(keyname) == NGC1D_plot_12.end())
      {
	std::string histname1 = "h" + detname + "_FoM_12";
	std::string histtitle1 = "Plot of Ratios for Cut 1.0 - 1.2 MeV";
	TH1F* hNGC1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNGC1D->GetXaxis()->SetTitle("Integral Ratio");
	hNGC1D->GetYaxis()->SetTitle("Count");
	NGC1D_plot_12[keyname] = hNGC1D;
      }

    if(NGC1D_plot_14.find(keyname) == NGC1D_plot_14.end())
      {
	std::string histname1 = "h" + detname + "_FoM_14";
	std::string histtitle1 = "Plot of Ratios for Cut 1.2 - 1.4 MeV";
	TH1F* hNGC1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNGC1D->GetXaxis()->SetTitle("Integral Ratio");
	hNGC1D->GetYaxis()->SetTitle("Count");
	NGC1D_plot_14[keyname] = hNGC1D;
      }

    if(NGC1D_plot_16.find(keyname) == NGC1D_plot_16.end())
      {
	std::string histname1 = "h" + detname + "_FoM_16";
	std::string histtitle1 = "Plot of Ratios for Cut 1.4 - 1.6 MeV";
	TH1F* hNGC1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNGC1D->GetXaxis()->SetTitle("Integral Ratio");
	hNGC1D->GetYaxis()->SetTitle("Count");
	NGC1D_plot_16[keyname] = hNGC1D;
      }

    if(NGC1D_plot_18.find(keyname) == NGC1D_plot_18.end())
      {
	std::string histname1 = "h" + detname + "_FoM_18";
	std::string histtitle1 = "Plot of Ratios for Cut 1.6 - 1.8MeV";
	TH1F* hNGC1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNGC1D->GetXaxis()->SetTitle("Integral Ratio");
	hNGC1D->GetYaxis()->SetTitle("Count");
	NGC1D_plot_18[keyname] = hNGC1D;
      }



    if(NGC1D_plot_20.find(keyname) == NGC1D_plot_20.end())
      {
	std::string histname1 = "h" + detname + "_FoM_20";
	std::string histtitle1 = "Plot of Ratios for Cut 1.8 - 2.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGC1D_plot_20[keyname] = hNG1D;
      }


    if(NGC1D_plot_35.find(keyname) == NGC1D_plot_35.end())
      {
	std::string histname1 = "h" + detname + "_FoM_35";
	std::string histtitle1 = "Plot of Ratios for Cut 2.0 - 3.5 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGC1D_plot_35[keyname] = hNG1D;
      }

    if(NGC1D_plot_50.find(keyname) == NGC1D_plot_50.end())
      {
	std::string histname1 = "h" + detname + "_FoM_50";
	std::string histtitle1 = "Plot of Ratios for Cut 3.5 - 5.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGC1D_plot_50[keyname] = hNG1D;
      }


    if(NGC1D_plot_70.find(keyname) == NGC1D_plot_70.end())
      {
	std::string histname1 = "h" + detname + "_FoM_70";
	std::string histtitle1 = "Plot of Ratios for Cut 5.0 - 7.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGC1D_plot_70[keyname] = hNG1D;
      }

    if(NGC1D_plot_90.find(keyname) == NGC1D_plot_90.end())
      {
	std::string histname1 = "h" + detname + "_FoM_90";
	std::string histtitle1 = "Plot of Ratios for Cut 8.0 - 9.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGC1D_plot_90[keyname] = hNG1D;
      }

    if(NGC1D_plot_110.find(keyname) == NGC1D_plot_110.end())
      {
	std::string histname1 = "h" + detname + "_FoM_110";
	std::string histtitle1 = "Plot of Ratios for Cut 9.0 - 11.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGC1D_plot_110[keyname] = hNG1D;
      }

    if(NGC1D_plot_150.find(keyname) == NGC1D_plot_150.end())
      {
	std::string histname1 = "h" + detname + "_FoM_150";
	std::string histtitle1 = "Plot of Ratios for Cut 11.0 - 15.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGC1D_plot_150[keyname] = hNG1D;
      }




 if(NGCEnergy_plots.find(keyname) == NGCEnergy_plots.end())
      {
	std::string histname6 = "h" + detname + "_Amplitude";
	std::string histtitle6 = "Plot of Neutron Amplitudes";
	TH1F* hNGCEnergy = new TH1F(histname6.c_str(), histtitle6.c_str(), 2832, 0, 16.16);
	hNGCEnergy->GetXaxis()->SetTitle("Energy (MeVee)");
	hNGCEnergy->GetYaxis()->SetTitle("Count");
	NGCEnergy_plots[keyname] = hNGCEnergy;
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
    
     
      float sum = 0, pedestal = 0, peak = 0, energy = 0;
      int count = 10;
      float ttail = 0, tstart = 0, tstop = 0, tpeak = 0;
      float timeB = 0, timeA = 0;
      bool coincidence = false;
      double fullInt = 0, tailInt = 0;
      double ratio = 0;  
      
      TH1F* hpulse = new TH1F("signal", "Plot of pulse shape", 150, 0, 150);
      hpulse->GetXaxis()->SetTitle("time");
      hpulse->GetYaxis()->SetTitle("ADC Count");



      for(std::vector<int>::iterator sampIt = samples.begin(); sampIt != samples.begin() + count; sampIt++)
	sum += (*sampIt);
      
      pedestal = sum / count;




      for(std::vector<int>::iterator sIter = samples.begin(); sIter != samples.end(); sIter++) 
	{


	  float samp = (*sIter) - pedestal;
	  int time = std::distance(samples.begin(), sIter);
  
	  //if((sIter+1) == samples.end())
	  //tstop = time;

	  //histogram our pulse
	  hpulse->Fill(time, samp);
	}



      peak = hpulse->GetMaximum();
      tpeak = hpulse->GetMaximumBin();
     

      if(detname == "NDet")
	energy = peak * 0.00575;
      if(detname == "NDet2")
	energy = peak * 0.00399;


      /***************************/
      // if(energy < 1.349)
      //tailcount = 3.774 + 2.968*energy;
      //if(energy >= 1.349)
      tailcount = 6.88 + 1.96*log(energy);

      int tcount = tailcount;

      

      /***************************/

      tstart = tpeak - 3;
      ttail = tpeak + tcount;
      tstop = ttail + 10;
      int trstop = tstop;

      float rem = (tailcount - tcount);


      if(peak > 5000)
	{
	  delete hpulse;
	  continue;
	}


      fullInt = hpulse->Integral(tstart, trstop-1);
      tailInt = hpulse->Integral(ttail+1, trstop-1);
      tailInt += (1-rem) * hpulse->GetBinContent(ttail);
      fullInt += rem * hpulse->GetBinContent(trstop);
      tailInt += rem * hpulse->GetBinContent(trstop);


      ratio = tailInt / fullInt;

      delete hpulse;

      // search for timing coincidence

      for(std::vector<TAnalysedPulse*>::iterator pIterA = currentDetAPulse; pIterA != detAPulses.end(); ++pIterA)
	{

	  timeB = (((*pIter)->GetTimeStamp() + tpeak) * gSetup->GetClockTick(bankname)) - gSetup->GetTimeShift(bankname);
	  timeA = (*pIterA)->GetTime();
          double t_diff = timeB - timeA;

	  if((t_diff > fStartWindow) && (t_diff < fStopWindow))
	    {
	      coincidence = true;
	      break;
	    }

	  if(timeA > timeB)
	    {
	      coincidence = false;
	      break;
	    }

	  currentDetAPulse = pIterA;
	}   // end timing coincidence search




      /////////////////////////////////


      if(coincidence == false)
	  continue;  //cut on timing
     


      //filling Histograms



      NGCDisc_plots[keyname]->Fill(fullInt, tailInt);
      NGCRatio_plots[keyname]->Fill(ratio, energy);

      if(energy < 0.4)
	NGC1D_plot_04[keyname]->Fill(ratio);
      if((energy > 0.4) && (energy < 0.6))
	NGC1D_plot_06[keyname]->Fill(ratio);	  
      if((energy > 0.6) && (energy < 0.8))
	NGC1D_plot_08[keyname]->Fill(ratio);
      if((energy > 0.8) && (energy < 1.0))
	NGC1D_plot_10[keyname]->Fill(ratio);
      if((energy > 1.0) && (energy < 1.2))
	NGC1D_plot_12[keyname]->Fill(ratio);
      if((energy > 1.2) && (energy < 1.4))
	NGC1D_plot_14[keyname]->Fill(ratio);
      if((energy > 1.4) && (energy < 1.6))
	NGC1D_plot_16[keyname]->Fill(ratio);
      if((energy > 1.6) && (energy < 1.8))
	NGC1D_plot_18[keyname]->Fill(ratio);
      if((energy > 1.8) && (energy < 2.0))
	NGC1D_plot_20[keyname]->Fill(ratio);	  
      if((energy > 2.0) && (energy < 3.5))
	NGC1D_plot_35[keyname]->Fill(ratio);
      if((energy > 3.5) && (energy < 5.0))
	NGC1D_plot_50[keyname]->Fill(ratio);
      if((energy > 5.0) && (energy < 7.0))
	NGC1D_plot_70[keyname]->Fill(ratio);
      if((energy > 7.0) && (energy < 9.0))
	NGC1D_plot_90[keyname]->Fill(ratio);
      if((energy > 9.0) && (energy < 11.0))
	NGC1D_plot_110[keyname]->Fill(ratio);
      if((energy > 11.0) && (energy < 15.0))
	NGC1D_plot_150[keyname]->Fill(ratio);


      float temp1 = -0.0161 + 0.0827/(sqrt(energy));
      float temp2 = 0.0171 + 0.0259/energy + 0.0406/(energy*energy);


      if((detname == "NDet") && (ratio > temp1)) 
	NGCEnergy_plots[keyname]->Fill(energy);      
      if((detname == "NDet2") && (ratio > temp2))
	NGCEnergy_plots[keyname]->Fill(energy);




 
    }//close pulse loop

  }//close map loop

  return 0;
}
