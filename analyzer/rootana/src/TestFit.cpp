#include "TestFit.h"
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
#include "TH1.h"
#include "TH2.h"
#include "TSpline.h"
#include <sstream>

using namespace std;
using std::string;
using std::map;
using std::vector;
using std::pair;

std::map<std::string, TH1F*> NGPed_plots;
std::map<std::string, TH1F*> NGChi_plots;
std::map<std::string, TH2F*> NGPSD_plots, NGChivRatio_plots, NGChivAmp_plots;

int FitCount = 0;

TestFit::TestFit(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName)
  {
    dir->cd("/");
  } 

TestFit::~TestFit()
{ }

int TestFit::ProcessEntry(TGlobalData *gData, TSetupData *gSetup)
{
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  for(map_iterator mapIter = gData->fPulseIslandToChannelMap.begin(); mapIter != gData->fPulseIslandToChannelMap.end(); mapIter++)
    {
      std::string bankname = mapIter->first;
      std::string detname = gSetup->GetDetectorName(bankname);
      std::string keyname = mapIter->first + GetName();

      if(detname != "NDet")
	continue;

      if(NGChivAmp_plots.find(keyname) == NGChivAmp_plots.end())
        {
         std::string histname2 = "h" + detname + "_ChivAmp";
         std::string histtitle2 = "Plot of Chi Squared vs Amplitude";
         TH2F* hNGRatio = new TH2F(histname2.c_str(), histtitle2.c_str(), 1000, 100, 1100, 2832, 0, 2831);
         hNGRatio->GetXaxis()->SetTitle("Chi Squared");
         hNGRatio->GetYaxis()->SetTitle("Amplitude");
         NGChivAmp_plots[keyname] = hNGRatio;
        }

      if(NGChivRatio_plots.find(keyname) == NGChivRatio_plots.end())
        {
         std::string histname2 = "h" + detname + "_ChivRat";
         std::string histtitle2 = "Plot of Chi Squared vs Integral Ratio";
         TH2F* hNGRatio = new TH2F(histname2.c_str(), histtitle2.c_str(), 1000, 100, 1100, 300, 0, 0.35);
         hNGRatio->GetXaxis()->SetTitle("Chi Squared");
         hNGRatio->GetYaxis()->SetTitle("Ratio");
         NGChivRatio_plots[keyname] = hNGRatio;
        }


      if(NGPSD_plots.find(keyname) == NGPSD_plots.end())
        {
         std::string histname2 = "h" + detname + "_ratio";
         std::string histtitle2 = "Plot of Integral Ratio";
         TH2F* hNGRatio = new TH2F(histname2.c_str(), histtitle2.c_str(), 2832, 0, 16.16, 300, 0, 0.35);
         hNGRatio->GetXaxis()->SetTitle("Energy (MeVee)");
         hNGRatio->GetYaxis()->SetTitle("Integral Ratio");
         NGPSD_plots[keyname] = hNGRatio;
        }

      if(NGPed_plots.find(keyname) == NGPed_plots.end())
        {
         std::string histname2 = "h" + detname + "_Ped";
         std::string histtitle2 = "Plot of the pedestals";
         TH1F* hNGPed = new TH1F(histname2.c_str(), histtitle2.c_str(), 4000, 300, 800);
         hNGPed->GetXaxis()->SetTitle("Pedestal");
         hNGPed->GetYaxis()->SetTitle("Count");
         NGPed_plots[keyname] = hNGPed;
        }

      if(NGChi_plots.find(keyname) == NGChi_plots.end())
        {
         std::string histname2 = "h" + detname + "_Chi2";
         std::string histtitle2 = "Plot of Chi Square";
         TH1F* hNGChi = new TH1F(histname2.c_str(), histtitle2.c_str(), 600, 150, 750);
         hNGChi->GetXaxis()->SetTitle("Chi Squared");
         hNGChi->GetYaxis()->SetTitle("Count");
         NGChi_plots[keyname] = hNGChi;
        }



      std::vector<TPulseIsland*> pulses = mapIter->second;

      if(pulses.size() == 0)
	continue;

      for(std::vector<TPulseIsland*>::iterator pIter = pulses.begin(); pIter != pulses.end(); pIter++)
	{

	  std::vector<int> samples = (*pIter)->GetSamples();

	  int length = std::distance(samples.begin(), samples.end());

	  

	  FitCount += 1;
	  std::stringstream ss;
	  ss<<FitCount;
	  
	  std::string histname1 = "signal_"  + ss.str();
	  std::string histtitle1 = "Plot of signal " + ss.str() + " with fit";
	  TH1F* hpulse = new TH1F(histname1.c_str(), histtitle1.c_str(), length, 0, length);
	  hpulse->GetXaxis()->SetTitle("time");
	  hpulse->GetYaxis()->SetTitle("ADC Count");
	  

	  //need to get pedestal first

	  int sum = 0, pcount = 15;
	  float pedestal = 0;

	  for(std::vector<int>::iterator sIt = samples.begin(); sIt != samples.begin() + pcount; sIt++)
	    sum += (*sIt);
	  pedestal = sum / pcount;
	  


	  int tpeak = 0;// ttail = 4, tstop = 0;
	  float peak = 0, samp = 0;
      


	  for(std::vector<int>::iterator sIt = samples.begin(); sIt != samples.end(); sIt++)
	    {

	      samp = (*sIt) - pedestal;
	      int time = std::distance(samples.begin(),sIt);

	      //if((sIt + 1) == samples.end())
	      //tstop = time;

	      hpulse->Fill(time, samp);

	    }
	

	  peak = hpulse->GetMaximum();
	  tpeak = hpulse->GetMaximumBin();

	  Double_t fullInt = hpulse->Integral(tpeak - 3, tpeak + 20);
	  Double_t tailInt = hpulse->Integral(tpeak + 5, tpeak + 20);
	  Double_t ratio = tailInt / fullInt;
	  Double_t energy = (float) (peak+15.2)/177.2;
  


	
	  //currently I have no means of fitting peak values past 4098
	  //values above 4098 are tagged with an overflow giving a value
	  //of 8096

	  if(peak > 5000)
	    {
	      delete hpulse;
	      /*
	      if(FitCount < 20)
		FitCount -= 1;
	      */
	      continue;
	    }

	  //we will attempt a  fit using a landau plus a constant 	  
	    
	  TF1* fPulse= new TF1("land", "landau(0) + [3]", 15, 150);
	  fPulse->SetParameter(0, peak * 2);
	  fPulse->SetParameter(2, tpeak);
	  fPulse->SetParameter(1, 10/peak);
	  fPulse->SetParameter(3, pedestal);

	  hpulse->Fit(fPulse, "Q", "", 15, 60);

	  Double_t land_height = fPulse->GetParameter(0);
	  Double_t land_mpv = fPulse->GetParameter(1);
	  Double_t land_sigma = fPulse->GetParameter(2);
	  Double_t fit_ped = fPulse->GetParameter(3);

	  Double_t Chi2 = fPulse->GetChisquare();

	  
	  delete hpulse;
	  delete fPulse;


	  NGChi_plots[keyname]->Fill(Chi2);
	  NGPed_plots[keyname]->Fill(fit_ped);
	  NGPSD_plots[keyname]->Fill(energy, ratio);

	  NGChivAmp_plots[keyname]->Fill(Chi2, peak);
	  NGChivRatio_plots[keyname]->Fill(Chi2, ratio);
	  
	  /*	  
	  if(FitCount > 20)
	    {
	      delete hpulse;
	      continue;
	    }    
	  
	  */
	  // need to output here
	  /*
	  cout << "for pulse number " << ss << " :"<< endl;
	  cout << "Pedestal " << pedestal << "   sigma " << sigma << "   threshold " << threshold << endl;
	  */

	  /*
		  cout << "quartiles   : " << q1 << "   " << median << "   " << q3 << "  " << skew <<  endl;
	  */

	  
	}  //end pulse loop

    }  //end map loop

  return 0;
}  //end function


