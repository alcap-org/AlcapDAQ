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
#include "TDetectorPulse.h"
#include "TH2F.h"
#include "TH1.h"
#include <sstream>

using std::string;
using std::map;
using std::vector;
using std::pair;



std::map<std::string, TH2F*> NGDisc_plots;
std::map<std::string, TH2F*> NGRatio_plots;
std::map<std::string, TH1F*> NG1D_plot_05, NG1D_plot_10, NG1D_plot_15, NG1D_plot_20, NG1D_plot_25, NG1D_plot_30, NG1D_plot_35, NG1D_plot_40, NG1D_plot_50, NG1D_plot_60, NG1D_plot_70, NG1D_plot_80, NG1D_plot_90, NG1D_plot_100, NG1D_plot_150;
std::map<std::string, TH1F*> NGEnergy_plots;
//neutCount = 0, gammaCount = 0;

NGammaInt::NGammaInt(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName)
  {
    dir->cd("/");
  }

NGammaInt::~NGammaInt()
{
}

int NGammaInt::ProcessEntry(TGlobalData *gData, TSetupData *gSetup)
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


    float ratioMax = 0.2, tailcount = 12;



    //initialize the histograms.  ///////////////////////////////


    if(NGDisc_plots.find(keyname) == NGDisc_plots.end())
    {
 
      std::string histname = "h" + detname + "_discrimination";
      std::string histtitle = "Plot of pulse integrals in the " + detname + " detector";
      TH2F* hNGDisc = new TH2F(histname.c_str(), histtitle.c_str(), 3000, 0, 12500, 1000, 0, 6000);
      hNGDisc->GetXaxis()->SetTitle("full integral (adc counts)");
      hNGDisc->GetYaxis()->SetTitle("tail integral (adc counts)");
      NGDisc_plots[keyname] = hNGDisc;
    }



    if(NGRatio_plots.find(keyname) == NGRatio_plots.end())
    {

     std::string histname2 = "h" + detname + "_ratio";
     std::string histtitle2 = "Plot of pulse integral ratio for the " + detname + " detector";
     TH2F* hNGRatio = new TH2F(histname2.c_str(), histtitle2.c_str(), 300, 0, ratioMax, 2000, 0, 15);
     hNGRatio->GetXaxis()->SetTitle("integral ratio");
     hNGRatio->GetYaxis()->SetTitle("Energy (MeVee)");
     NGRatio_plots[keyname] = hNGRatio;
    }
 
    // there may be some easier way of doing this, but for the limited
    // number of histograms, this suffices.  

    /* 
    if(NG1D_plots.find(keyname) == NG1D_plots.end())
      {
	for(int E = 1; E < 10; E++)
	  {
	    float Ep = E/2, Epp = (E-1)/2;
	    std::stringstream ss, st;
	    ss<<Ep;
	    st<<Epp;
	    std::string histname1 = "h" + detname + "_FoM_" + ss.str();
	    std::string histtitle1 = "Plot of Ratios for Cut " + st.str() + " - " + ss.str() + " MeV";
	    TH1F* hNG1D[E] = new TH1F(histname1.c_str(), histtitle1.c_str(), 300,0,0.45);
	    hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	    hNG1D->GetYaxis()->SetTitle("Count");
	    NG1D_plots[keyname] = hNG1D[E];
	  }
      }
    */

    if(NG1D_plot_05.find(keyname) == NG1D_plot_05.end())
      {
	std::string histname1 = "h" + detname + "_FoM_05";
	std::string histtitle1 = "Plot of Ratios for Cut 0.0 - 0.5 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NG1D_plot_05[keyname] = hNG1D;
      }


    if(NG1D_plot_10.find(keyname) == NG1D_plot_10.end())
      {
	std::string histname1 = "h" + detname + "_FoM_10";
	std::string histtitle1 = "Plot of Ratios for Cut 0.5 - 1.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NG1D_plot_10[keyname] = hNG1D;
      }

    if(NG1D_plot_15.find(keyname) == NG1D_plot_15.end())
      {
	std::string histname1 = "h" + detname + "_FoM_15";
	std::string histtitle1 = "Plot of Ratios for Cut 1.0 - 1.5MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NG1D_plot_15[keyname] = hNG1D;
      }


    if(NG1D_plot_20.find(keyname) == NG1D_plot_20.end())
      {
	std::string histname1 = "h" + detname + "_FoM_20";
	std::string histtitle1 = "Plot of Ratios for Cut 1.5 - 2.0MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NG1D_plot_20[keyname] = hNG1D;
      }

    if(NG1D_plot_25.find(keyname) == NG1D_plot_25.end())
      {
	std::string histname1 = "h" + detname + "_FoM_25";
	std::string histtitle1 = "Plot of Ratios for Cut 2.0 - 2.5MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NG1D_plot_25[keyname] = hNG1D;
      }

    if(NG1D_plot_30.find(keyname) == NG1D_plot_30.end())
      {
	std::string histname1 = "h" + detname + "_FoM_30";
	std::string histtitle1 = "Plot of Ratios for Cut 2.5 - 3.0MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NG1D_plot_30[keyname] = hNG1D;
      }

    if(NG1D_plot_35.find(keyname) == NG1D_plot_35.end())
      {
	std::string histname1 = "h" + detname + "_FoM_35";
	std::string histtitle1 = "Plot of Ratios for Cut 3.0 - 3.5MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NG1D_plot_35[keyname] = hNG1D;
      }

    if(NG1D_plot_40.find(keyname) == NG1D_plot_40.end())
      {
	std::string histname1 = "h" + detname + "_FoM_40";
	std::string histtitle1 = "Plot of Ratios for Cut 3.5 - 4.0MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NG1D_plot_40[keyname] = hNG1D;
      }

    if(NG1D_plot_50.find(keyname) == NG1D_plot_50.end())
      {
	std::string histname1 = "h" + detname + "_FoM_50";
	std::string histtitle1 = "Plot of Ratios for Cut 4.0 - 5.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NG1D_plot_50[keyname] = hNG1D;
      }


    if(NG1D_plot_60.find(keyname) == NG1D_plot_60.end())
      {
	std::string histname1 = "h" + detname + "_FoM_60";
	std::string histtitle1 = "Plot of Ratios for Cut 5.0 - 6.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NG1D_plot_60[keyname] = hNG1D;
      }

    if(NG1D_plot_70.find(keyname) == NG1D_plot_70.end())
      {
	std::string histname1 = "h" + detname + "_FoM_70";
	std::string histtitle1 = "Plot of Ratios for Cut 6.0 - 7.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NG1D_plot_70[keyname] = hNG1D;
      }


    if(NG1D_plot_80.find(keyname) == NG1D_plot_80.end())
      {
	std::string histname1 = "h" + detname + "_FoM_80";
	std::string histtitle1 = "Plot of Ratios for Cut 7.0 - 8.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NG1D_plot_80[keyname] = hNG1D;
      }

    if(NG1D_plot_90.find(keyname) == NG1D_plot_90.end())
      {
	std::string histname1 = "h" + detname + "_FoM_90";
	std::string histtitle1 = "Plot of Ratios for Cut 8.0 - 9.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NG1D_plot_90[keyname] = hNG1D;
      }

    if(NG1D_plot_100.find(keyname) == NG1D_plot_100.end())
      {
	std::string histname1 = "h" + detname + "_FoM_100";
	std::string histtitle1 = "Plot of Ratios for Cut 9.0 - 10.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NG1D_plot_100[keyname] = hNG1D;
      }

    if(NG1D_plot_150.find(keyname) == NG1D_plot_150.end())
      {
	std::string histname1 = "h" + detname + "_FoM_150";
	std::string histtitle1 = "Plot of Ratios for Cut 10.0 - 15.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NG1D_plot_150[keyname] = hNG1D;
      }






 if(NGEnergy_plots.find(keyname) == NGEnergy_plots.end())
      {
	std::string histname6 = "h" + detname + "_Amplitude";
	std::string histtitle6 = "Plot of Neutron Amplitudes";
	TH1F* hNGEnergy = new TH1F(histname6.c_str(), histtitle6.c_str(), 2832, 0, 16.16);
	hNGEnergy->GetXaxis()->SetTitle("Energy (MeVee)");
	hNGEnergy->GetYaxis()->SetTitle("Count");
	NGEnergy_plots[keyname] = hNGEnergy;
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
      float peak = 0, energy = 0, tstart = 0, tstop = 0, ttail = 0, tpeak = 0;
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

      // Fill the histograms


      NGDisc_plots[keyname]->Fill(fullInt, tailInt);
      NGRatio_plots[keyname]->Fill(ratio, energy);

      if(energy < 0.5)
	NG1D_plot_05[keyname]->Fill(ratio);
      if((energy > 0.5) && (energy < 1.0))
	NG1D_plot_10[keyname]->Fill(ratio);	  
      if((energy > 1.0) && (energy < 1.5))
	NG1D_plot_15[keyname]->Fill(ratio);
      if((energy > 1.5) && (energy < 2.0))
	NG1D_plot_20[keyname]->Fill(ratio);
      if((energy > 2.0) && (energy < 2.5))
	NG1D_plot_25[keyname]->Fill(ratio);
      if((energy > 2.5) && (energy < 3.0))
	NG1D_plot_30[keyname]->Fill(ratio);
      if((energy > 3.0) && (energy < 3.5))
	NG1D_plot_35[keyname]->Fill(ratio);
      if((energy > 3.5) && (energy < 4.0))
	NG1D_plot_40[keyname]->Fill(ratio);
      if((energy > 4.0) && (energy < 5.0))
	NG1D_plot_50[keyname]->Fill(ratio);	  
      if((energy > 5.0) && (energy < 6.0))
	NG1D_plot_60[keyname]->Fill(ratio);
      if((energy > 6.0) && (energy < 7.0))
	NG1D_plot_70[keyname]->Fill(ratio);
      if((energy > 7.0) && (energy < 8.0))
	NG1D_plot_80[keyname]->Fill(ratio);
      if((energy > 8.0) && (energy < 9.0))
	NG1D_plot_90[keyname]->Fill(ratio);
      if((energy > 9.0) && (energy < 10.0))
	NG1D_plot_100[keyname]->Fill(ratio);
      if((energy > 10.0) && (energy < 15.0))
	NG1D_plot_150[keyname]->Fill(ratio);


      float temp1 = -0.0161 + 0.0827/(sqrt(energy));
      float temp2 = 0.0171 + 0.0259/energy + 0.0406/(energy*energy);


      if((detname == "NDet") && (ratio > temp1)) 
	NGEnergy_plots[keyname]->Fill(energy);      
      if((detname == "NDet2") && (ratio > temp2))
	NGEnergy_plots[keyname]->Fill(energy);




      //cuts should go here

      /////////////////////////////////////////////////////////
      //   now we need to figure out why those signals are   //
      //   showing up weird.  start with condition check,    //
      //   then build histogram or graph (which one?)        //
      //   from our pulse.                                   //
      /////////////////////////////////////////////////////////
      
      /* 
      if((ratio >= 0.08) && (ratio < 0.09) && (neutCount < 10)  && (detname == "NDet") && (peak > 495) && (peak < 500))
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
	      hPulse->Fill(t, *(sIt)- 1265);
            } 


	}

       
      if((ratio < 0.05) && (ratio >= 0.03) && (gammaCount < 10) && (detname == "NDet") && (peak < 500) && (peak > 495))
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
              hPulse2->Fill(t1 , (*sIt) - 1265);
       

            } 

	}
      */
      



    }//close pulse loop

  }//close map loop

  return 0;
}
