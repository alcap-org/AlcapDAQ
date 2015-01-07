#include "TestFGA.h"
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

using namespace std;
using std::string;
using std::map;
using std::vector;
using std::pair;

//std::map<std::string, TH1F*> signals;
//std::map<std::string, TH1F*> smSignals;

std::map<std::string, TH2F*> NGFGA_plots, NGFGA2_plots;
std::map<std::string, TH1F*> NGFGA1D_plot_05, NGFGA1D_plot_10, NGFGA1D_plot_15, NGFGA1D_plot_20, NGFGA1D_plot_25, NGFGA1D_plot_30, NGFGA1D_plot_35, NGFGA1D_plot_40, NGFGA1D_plot_50, NGFGA1D_plot_60, NGFGA1D_plot_70, NGFGA1D_plot_80, NGFGA1D_plot_90, NGFGA1D_plot_100, NGFGA1D_plot_150;
std::map<std::string, TH1F*> NGFGA1D_plot_05_2, NGFGA1D_plot_10_2, NGFGA1D_plot_15_2, NGFGA1D_plot_20_2, NGFGA1D_plot_25_2, NGFGA1D_plot_30_2, NGFGA1D_plot_35_2, NGFGA1D_plot_40_2, NGFGA1D_plot_50_2, NGFGA1D_plot_60_2, NGFGA1D_plot_70_2, NGFGA1D_plot_80_2, NGFGA1D_plot_90_2, NGFGA1D_plot_100_2, NGFGA1D_plot_150_2;
int FGACount = 0;

TestFGA::TestFGA(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName)
  {
    dir->cd("/");
  } 

TestFGA::~TestFGA()
{ }

int TestFGA::ProcessEntry(TGlobalData *gData, TSetupData *gSetup)
{

  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  for(map_iterator mapIter = gData->fPulseIslandToChannelMap.begin(); mapIter != gData->fPulseIslandToChannelMap.end(); mapIter++)
    {
      std::string bankname = mapIter->first;
      std::string detname = gSetup->GetDetectorName(bankname);
      std::string keyname = mapIter->first + GetName();

      if((detname != "NDet")&&(detname != "NDet2"))
	continue;

      float ratioMax = 2.25, ratioMax2 = 1.0;


      if(NGFGA_plots.find(keyname) == NGFGA_plots.end())
        {
         std::string histname2 = "h" + detname + "_ratio";
         std::string histtitle2 = "Plot of tail ratio for the " + detname + " detector";
         TH2F* hNGRatio = new TH2F(histname2.c_str(), histtitle2.c_str(), 300, 0, ratioMax, 3000, 0, 15);
         hNGRatio->GetXaxis()->SetTitle("Ratio");
         hNGRatio->GetYaxis()->SetTitle("ADC count");
         NGFGA_plots[keyname] = hNGRatio;
        }



    if(NGFGA1D_plot_05.find(keyname) == NGFGA1D_plot_05.end())
      {
	std::string histname1 = "h" + detname + "_FoM_05";
	std::string histtitle1 = "Plot of Ratios for Cut 0.0 - 0.5 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_05[keyname] = hNG1D;
      }


    if(NGFGA1D_plot_10.find(keyname) == NGFGA1D_plot_10.end())
      {
	std::string histname1 = "h" + detname + "_FoM_10";
	std::string histtitle1 = "Plot of Ratios for Cut 0.5 - 1.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_10[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_15.find(keyname) == NGFGA1D_plot_15.end())
      {
	std::string histname1 = "h" + detname + "_FoM_15";
	std::string histtitle1 = "Plot of Ratios for Cut 1.0 - 1.5MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_15[keyname] = hNG1D;
      }


    if(NGFGA1D_plot_20.find(keyname) == NGFGA1D_plot_20.end())
      {
	std::string histname1 = "h" + detname + "_FoM_20";
	std::string histtitle1 = "Plot of Ratios for Cut 1.5 - 2.0MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_20[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_25.find(keyname) == NGFGA1D_plot_25.end())
      {
	std::string histname1 = "h" + detname + "_FoM_25";
	std::string histtitle1 = "Plot of Ratios for Cut 2.0 - 2.5MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_25[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_30.find(keyname) == NGFGA1D_plot_30.end())
      {
	std::string histname1 = "h" + detname + "_FoM_30";
	std::string histtitle1 = "Plot of Ratios for Cut 2.5 - 3.0MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_30[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_35.find(keyname) == NGFGA1D_plot_35.end())
      {
	std::string histname1 = "h" + detname + "_FoM_35";
	std::string histtitle1 = "Plot of Ratios for Cut 3.0 - 3.5MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_35[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_40.find(keyname) == NGFGA1D_plot_40.end())
      {
	std::string histname1 = "h" + detname + "_FoM_40";
	std::string histtitle1 = "Plot of Ratios for Cut 3.5 - 4.0MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_40[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_50.find(keyname) == NGFGA1D_plot_50.end())
      {
	std::string histname1 = "h" + detname + "_FoM_50";
	std::string histtitle1 = "Plot of Ratios for Cut 4.0 - 5.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_50[keyname] = hNG1D;
      }


    if(NGFGA1D_plot_60.find(keyname) == NGFGA1D_plot_60.end())
      {
	std::string histname1 = "h" + detname + "_FoM_60";
	std::string histtitle1 = "Plot of Ratios for Cut 5.0 - 6.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_60[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_70.find(keyname) == NGFGA1D_plot_70.end())
      {
	std::string histname1 = "h" + detname + "_FoM_70";
	std::string histtitle1 = "Plot of Ratios for Cut 6.0 - 7.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_70[keyname] = hNG1D;
      }


    if(NGFGA1D_plot_80.find(keyname) == NGFGA1D_plot_80.end())
      {
	std::string histname1 = "h" + detname + "_FoM_80";
	std::string histtitle1 = "Plot of Ratios for Cut 7.0 - 8.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_80[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_90.find(keyname) == NGFGA1D_plot_90.end())
      {
	std::string histname1 = "h" + detname + "_FoM_90";
	std::string histtitle1 = "Plot of Ratios for Cut 8.0 - 9.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_90[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_100.find(keyname) == NGFGA1D_plot_100.end())
      {
	std::string histname1 = "h" + detname + "_FoM_100";
	std::string histtitle1 = "Plot of Ratios for Cut 9.0 - 10.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_100[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_150.find(keyname) == NGFGA1D_plot_150.end())
      {
	std::string histname1 = "h" + detname + "_FoM_150";
	std::string histtitle1 = "Plot of Ratios for Cut 10.0 - 15.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_150[keyname] = hNG1D;
      }

      if(NGFGA2_plots.find(keyname) == NGFGA2_plots.end())
        {
         std::string histname2 = "h" + detname + "_ratio_2";
         std::string histtitle2 = "Plot of tail ratiofor the " + detname + " detector";
         TH2F* hNGRatio = new TH2F(histname2.c_str(), histtitle2.c_str(), 300, 0, ratioMax2, 3000, 0, 15);
         hNGRatio->GetXaxis()->SetTitle("Ratio");
         hNGRatio->GetYaxis()->SetTitle("ADC count");
         NGFGA2_plots[keyname] = hNGRatio;
	}

    if(NGFGA1D_plot_05_2.find(keyname) == NGFGA1D_plot_05_2.end())
      {
	std::string histname1 = "h" + detname + "_FoM_05_2";
	std::string histtitle1 = "Plot of Ratios for Cut 0.0 - 0.5 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax2);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_05_2[keyname] = hNG1D;
      }


    if(NGFGA1D_plot_10_2.find(keyname) == NGFGA1D_plot_10_2.end())
      {
	std::string histname1 = "h" + detname + "_FoM_10_2";
	std::string histtitle1 = "Plot of Ratios for Cut 0.5 - 1.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax2);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_10_2[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_15_2.find(keyname) == NGFGA1D_plot_15_2.end())
      {
	std::string histname1 = "h" + detname + "_FoM_15_2";
	std::string histtitle1 = "Plot of Ratios for Cut 1.0 - 1.5MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax2);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_15_2[keyname] = hNG1D;
      }


    if(NGFGA1D_plot_20_2.find(keyname) == NGFGA1D_plot_20_2.end())
      {
	std::string histname1 = "h" + detname + "_FoM_20_2";
	std::string histtitle1 = "Plot of Ratios for Cut 1.5 - 2.0MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax2);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_20_2[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_25_2.find(keyname) == NGFGA1D_plot_25_2.end())
      {
	std::string histname1 = "h" + detname + "_FoM_25_2";
	std::string histtitle1 = "Plot of Ratios for Cut 2.0 - 2.5MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax2);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_25_2[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_30_2.find(keyname) == NGFGA1D_plot_30_2.end())
      {
	std::string histname1 = "h" + detname + "_FoM_30_2";
	std::string histtitle1 = "Plot of Ratios for Cut 2.5 - 3.0MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax2);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_30_2[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_35_2.find(keyname) == NGFGA1D_plot_35_2.end())
      {
	std::string histname1 = "h" + detname + "_FoM_35_2";
	std::string histtitle1 = "Plot of Ratios for Cut 3.0 - 3.5MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax2);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_35_2[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_40_2.find(keyname) == NGFGA1D_plot_40_2.end())
      {
	std::string histname1 = "h" + detname + "_FoM_40_2";
	std::string histtitle1 = "Plot of Ratios for Cut 3.5 - 4.0MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax2);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_40_2[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_50_2.find(keyname) == NGFGA1D_plot_50_2.end())
      {
	std::string histname1 = "h" + detname + "_FoM_50_2";
	std::string histtitle1 = "Plot of Ratios for Cut 4.0 - 5.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax2);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_50_2[keyname] = hNG1D;
      }


    if(NGFGA1D_plot_60_2.find(keyname) == NGFGA1D_plot_60_2.end())
      {
	std::string histname1 = "h" + detname + "_FoM_60_2";
	std::string histtitle1 = "Plot of Ratios for Cut 5.0 - 6.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax2);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_60_2[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_70_2.find(keyname) == NGFGA1D_plot_70_2.end())
      {
	std::string histname1 = "h" + detname + "_FoM_70_2";
	std::string histtitle1 = "Plot of Ratios for Cut 6.0 - 7.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax2);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_70_2[keyname] = hNG1D;
      }


    if(NGFGA1D_plot_80_2.find(keyname) == NGFGA1D_plot_80_2.end())
      {
	std::string histname1 = "h" + detname + "_FoM_80_2";
	std::string histtitle1 = "Plot of Ratios for Cut 7.0 - 8.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax2);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_80_2[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_90_2.find(keyname) == NGFGA1D_plot_90_2.end())
      {
	std::string histname1 = "h" + detname + "_FoM_90_2";
	std::string histtitle1 = "Plot of Ratios for Cut 8.0 - 9.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax2);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_90_2[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_100_2.find(keyname) == NGFGA1D_plot_100_2.end())
      {
	std::string histname1 = "h" + detname + "_FoM_100_2";
	std::string histtitle1 = "Plot of Ratios for Cut 9.0 - 10.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax2);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_100_2[keyname] = hNG1D;
      }

    if(NGFGA1D_plot_150_2.find(keyname) == NGFGA1D_plot_150_2.end())
      {
	std::string histname1 = "h" + detname + "_FoM_150_2";
	std::string histtitle1 = "Plot of Ratios for Cut 10.0 - 15.0 MeV";
	TH1F* hNG1D = new TH1F(histname1.c_str(), histtitle1.c_str(), 300, 0, ratioMax2);
	hNG1D->GetXaxis()->SetTitle("Integral Ratio");
	hNG1D->GetYaxis()->SetTitle("Count");
	NGFGA1D_plot_150_2[keyname] = hNG1D;
      }





      std::vector<TPulseIsland*> pulses = mapIter->second;

      if(pulses.size() == 0)
	continue;

      for(std::vector<TPulseIsland*>::iterator pIter = pulses.begin(); pIter != pulses.end(); pIter++)
	{

	  std::vector<int> samples = (*pIter)->GetSamples();

	  //int length = std::distance(samples.begin(), samples.end());


  
	  FGACount += 1;
	  std::stringstream ss;
	  ss<<FGACount;
	  std::string histname1 = "signal_"  + ss.str();
	  std::string histtitle1 = "Plot of signal " + ss.str();
	  TH1F* hpulse = new TH1F(histname1.c_str(), histtitle1.c_str(), samples.size(), 0, samples.size());
	  hpulse->GetXaxis()->SetTitle("time");
	  hpulse->GetYaxis()->SetTitle("ADC Count");


	  //need to get pedestal first

	  int sum = 0, pcount = 10;
	  float pedestal = 0, sigma = 0, sum2 = 0;

	  for(std::vector<int>::iterator sIt = samples.begin(); sIt != samples.begin() + pcount; sIt++)
	    {
	      sum += (*sIt);
	      sum2 += (*sIt) * (*sIt);
	    }
	  pedestal = sum / pcount;
	  sigma = (sum2/pcount) - pedestal*pedestal;

	  //now to get the times and integrals for the original signal

	  std::vector<int>::iterator tpeak;
	  float peak = 0, samp = 0, threshold = 5 * sigma;

	  if(threshold <= 5)
	    threshold = 5;

	  for(std::vector<int>::iterator sIt = samples.begin(); sIt != samples.end(); sIt++)
	    {

	      samp = (*sIt);
	      int time = std::distance(samples.begin(),sIt);

	      if((*sIt) > peak)
		{
		  peak = (*sIt);
		  tpeak = sIt;
		}
	      //make our histograms while we're at it

	      hpulse->Fill(time, samp);

	    }

	  peak -= pedestal;
	  //peak = hpulse->GetMaximum() - pedestal;
	  //tpeak = hpulse->GetMaximumBin();

		
	  //currently I have no means of fitting peak values past 4098
	  //values above 4098 are tagged with an overflow giving a value
	  //of 8096

	  if(peak > 5000)
	    {
	      delete hpulse;
	      FGACount -= 1;
	      continue;
	    }


	  float sum1 = 0, sumcos = 0, sumsin = 0;
	  float avg1 = 0, avg0 = 0;
	  const float coef = 0.2513272;
	  float temp = 0, diff = 0, energy = 0;


	  for(std::vector<int>::iterator sIt = tpeak; sIt != tpeak + 25; sIt++)
	    {
	      temp = ((*sIt) - pedestal)/ peak;
	      int n1 = std::distance(tpeak, sIt);
	      sum1 += temp;
	      sumcos += temp * cos(coef*n1);
	      sumsin += temp * sin(coef*n1);
	    }


	  avg1 = sqrt(sumcos*sumcos + sumsin*sumsin);
	  avg0 = abs(sum1);

	  diff = (avg0 - avg1);

	  float ratiosq = 1-((avg1*avg1)/(avg0*avg0));

	  if(detname == "NDet")
	    energy = peak * 0.00575;
	  if(detname == "NDet2")
	    energy = peak * 0.00399;

	  NGFGA_plots[keyname]->Fill(diff, energy);
	  NGFGA2_plots[keyname]->Fill(ratiosq, energy);


	  if(energy < 0.5)
	    NGFGA1D_plot_05[keyname]->Fill(diff);
	  if((energy > 0.5) && (energy < 1.0))
	    NGFGA1D_plot_10[keyname]->Fill(diff);	  
	  if((energy > 1.0) && (energy < 1.5))
	    NGFGA1D_plot_15[keyname]->Fill(diff);
	  if((energy > 1.5) && (energy < 2.0))
	    NGFGA1D_plot_20[keyname]->Fill(diff);
	  if((energy > 2.0) && (energy < 2.5))
	    NGFGA1D_plot_25[keyname]->Fill(diff);
	  if((energy > 2.5) && (energy < 3.0))
	    NGFGA1D_plot_30[keyname]->Fill(diff);
	  if((energy > 3.0) && (energy < 3.5))
	    NGFGA1D_plot_35[keyname]->Fill(diff);
	  if((energy > 3.5) && (energy < 4.0))
	    NGFGA1D_plot_40[keyname]->Fill(diff);
	  if((energy > 4.0) && (energy < 5.0))
	    NGFGA1D_plot_50[keyname]->Fill(diff);	  
	  if((energy > 5.0) && (energy < 6.0))
	    NGFGA1D_plot_60[keyname]->Fill(diff);
	  if((energy > 6.0) && (energy < 7.0))
	    NGFGA1D_plot_70[keyname]->Fill(diff);
	  if((energy > 7.0) && (energy < 8.0))
	    NGFGA1D_plot_80[keyname]->Fill(diff);
	  if((energy > 8.0) && (energy < 9.0))
	    NGFGA1D_plot_90[keyname]->Fill(diff);
	  if((energy > 9.0) && (energy < 10.0))
	    NGFGA1D_plot_100[keyname]->Fill(diff);
	  if((energy > 10.0) && (energy < 15.0))
	    NGFGA1D_plot_150[keyname]->Fill(diff);


	  if(energy < 0.5)
	    NGFGA1D_plot_05_2[keyname]->Fill(ratiosq);
	  if((energy > 0.5) && (energy < 1.0))
	    NGFGA1D_plot_10_2[keyname]->Fill(ratiosq);	  
	  if((energy > 1.0) && (energy < 1.5))
	    NGFGA1D_plot_15_2[keyname]->Fill(ratiosq);
	  if((energy > 1.5) && (energy < 2.0))
	    NGFGA1D_plot_20_2[keyname]->Fill(ratiosq);
	  if((energy > 2.0) && (energy < 2.5))
	    NGFGA1D_plot_25_2[keyname]->Fill(ratiosq);
	  if((energy > 2.5) && (energy < 3.0))
	    NGFGA1D_plot_30_2[keyname]->Fill(ratiosq);
	  if((energy > 3.0) && (energy < 3.5))
	    NGFGA1D_plot_35_2[keyname]->Fill(ratiosq);
	  if((energy > 3.5) && (energy < 4.0))
	    NGFGA1D_plot_40_2[keyname]->Fill(ratiosq);
	  if((energy > 4.0) && (energy < 5.0))
	    NGFGA1D_plot_50_2[keyname]->Fill(ratiosq);	  
	  if((energy > 5.0) && (energy < 6.0))
	    NGFGA1D_plot_60_2[keyname]->Fill(ratiosq);
	  if((energy > 6.0) && (energy < 7.0))
	    NGFGA1D_plot_70_2[keyname]->Fill(ratiosq);
	  if((energy > 7.0) && (energy < 8.0))
	    NGFGA1D_plot_80_2[keyname]->Fill(ratiosq);
	  if((energy > 8.0) && (energy < 9.0))
	    NGFGA1D_plot_90_2[keyname]->Fill(ratiosq);
	  if((energy > 9.0) && (energy < 10.0))
	    NGFGA1D_plot_100_2[keyname]->Fill(ratiosq);
	  if((energy > 10.0) && (energy < 15.0))
	    NGFGA1D_plot_150_2[keyname]->Fill(ratiosq);


	  //need to output the data
	  /*
	  cout << "for pulse number " << ss << " :"<< endl;
	  cout << "Pedestal " << pedestal << "   sigma " << sigma << "  Threshold " << threshold << endl;

	  cout << "For the original pulse :" << endl;
	  cout << "Peak " << peak << "   Start time " << tstart << "   Tail start " << ttail << endl;
	  cout << "End time " << tstop << "   Full Integral " << fullInt << "   Tail Integral " << tailInt << "   Ratio " << ratio << endl;
	     
	  cout << "For the smoothed pulse :" << endl;
	  cout << "Peak " << smpeak << "   Start time " << tsmstart << "   Tail start " << tsmtail << endl;
	  cout << "End time " << tstop << "   Full Integral " << smFullInt << "   Tail Integral " << smTailInt << "   Ratio " << smRatio << endl << endl;
	  */

	  delete hpulse;





	}  //end pulse loop

    }  //end map loop

  return 0;
}  //end function


