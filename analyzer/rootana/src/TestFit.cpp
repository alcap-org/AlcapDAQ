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

//std::map<std::string, TH1F*> signals;
//std::map<std::string, TH1F*> smSignals;

std::map<std::string, TH2F*> NGFit_plots;
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
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  for(map_iterator mapIter = gData->fPulseIslandToChannelMap.begin(); mapIter != gData->fPulseIslandToChannelMap.end(); mapIter++)
    {
      std::string bankname = mapIter->first;
      std::string detname = gSetup->GetDetectorName(bankname);
      std::string keyname = mapIter->first + GetName();

      if(detname != "NDet")
	continue;


      if(NGFit_plots.find(keyname) == NGFit_plots.end())
        {
         std::string histname2 = "h" + detname + "_ratio";
         std::string histtitle2 = "Plot of Skewness for the " + detname + " detector";
         TH2F* hNGRatio = new TH2F(histname2.c_str(), histtitle2.c_str(), 300, -1.5, 3.5, 3000, 0, 15);
         hNGRatio->GetXaxis()->SetTitle("Skew");
         hNGRatio->GetYaxis()->SetTitle("Count");
         NGFit_plots[keyname] = hNGRatio;
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
	  Double_t energy = peak * 0.00564;


	  Double_t Skew = 0, Sigma = 0, sumSigma = 0, sum2 = 0, sum3 = 0;
	  Double_t n = 25, x = 0, mean = 0, median = 0, q1 = 0, q3 = 0;
	  Double_t Ex2 = 0, temp = 0, sumSkew = 0, skew = 0;

	  for(int i = tpeak - 4; i < tpeak - 4 + n; i++)
	    {
	      x= hpulse->GetBinContent(i);
	      sum2 += x;
	      mean += i*x;
	      Ex2 += i*i*x;
	    }

	  mean /= sum2;
	  Ex2 /= sum2;
	  double temp3 = 0;
	  for(int i = tpeak - 4; i < tpeak - 4 + n; i++)
	    {
	      x = hpulse->GetBinContent(i);
	      double temp2 = 0.25 * sum2;
	      sum3 += x;
	      if((sum3 > temp2) && (q1 == 0))
		{
		  temp3 =  (sum3 - temp2)/x;
		  q1 = i + temp3;
		}
	      if((sum3 > (temp2 * 2)) && (median == 0))
		{
		  temp3 = (sum3 - (temp2 * 2))/x;
		  median = i + temp3;
		}
	      if((sum3 > (temp2 * 3))  && (q3 == 0))
		{
		  temp3 = (sum3 - (temp2 * 3))/x;
		  q3 = i + temp3;
		}


	      temp = x - mean;
	      sumSigma += temp * temp;
	      sumSkew += temp * temp * temp;

	    }

	  Sigma = sqrt(sumSigma/(n-1));
	  Skew = sumSkew * n / ((n-1) * (n-2) * Sigma * Sigma * Sigma);
	  skew = ((q3 - median) - (median - q1)) / (q3 - q1);
	  



	
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

	  //we will attempt a  fit using an exponential (plus a constant if the fit looks bad)
	  
	  //double_t Constant = 0, Sigma = 0, Mode = 0, Skew = 0;
	  //double pi = 3.1415926535897;

	  /*
	    
	  TF1* fPulse= new TF1("fPulse", "[0] *(([1]* sqrt(2 * pi))^-1) * exp((-(x-[2])^2)/(2*([2]^2))) * (1 + TMath::Erf([3]*(x - [2])/([1] * sqrt(2))))", 0., 150.);
	  fPulse->SetParameter(0, peak * 2);
	  fPulse->SetParameter(2, tpeak);
	  fPulse->SetParameter(1, 10/peak);
	  fPulse->SetParameter(3, 6);

	  hpulse->Fit("fPulse", "Q", "", tpeak - 3, tpeak + 20);


	  Constant = fPulse->GetParameter(0);
	  Sigma =fPulse->GetParameter(1);
	  Mode = fPulse->GetParameter(2);
	  Skew = fPulse->GetParameter(3);
	  */
	  
	  delete hpulse;
	  /*
	  if((ratio < 0.1) && (energy > 2.0)) 
	    continue;
	  
	  if((ratio < 0.125) && (energy > 0.8) && (energy < 2.0))
	    continue;
	  
	  if((ratio < 0.15) && (energy < 0.8))
	    continue;
	  */





	  NGFit_plots[keyname]->Fill(Skew, energy);
	  
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


