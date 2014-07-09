#include "TestSmoothing.h"
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

std::map<std::string, TH2F*> NGSmooth_plots;
int SmoothingCount = 0;

TestSmoothing::TestSmoothing(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName)
  {
    dir->cd("/");
  } 

TestSmoothing::~TestSmoothing()
{ }

int TestSmoothing::ProcessEntry(TGlobalData *gData, TSetupData *gSetup)
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


      if(NGSmooth_plots.find(keyname) == NGSmooth_plots.end())
        {
         std::string histname2 = "h" + detname + "_ratio";
         std::string histtitle2 = "Plot of tail ratiofor the " + detname + " detector";
         TH2F* hNGRatio = new TH2F(histname2.c_str(), histtitle2.c_str(), 300, 0, 0.2, 3000, 0, 20);
         hNGRatio->GetXaxis()->SetTitle("Ratio");
         hNGRatio->GetYaxis()->SetTitle("ADC count");
         NGSmooth_plots[keyname] = hNGRatio;
        }



      std::vector<TPulseIsland*> pulses = mapIter->second;

      if(pulses.size() == 0)
	continue;

      for(std::vector<TPulseIsland*>::iterator pIter = pulses.begin(); pIter != pulses.end(); pIter++)
	{

	  std::vector<int> samples = (*pIter)->GetSamples();

	  //int length = std::distance(samples.begin(), samples.end());


  
	  SmoothingCount += 1;
	  std::stringstream ss;
	  ss<<SmoothingCount;
	  std::string histname1 = "signal_"  + ss.str();
	  std::string histtitle1 = "Plot of unsmoothed signal " + ss.str();
	  TH1F* hpulse = new TH1F(histname1.c_str(), histtitle1.c_str(), 150, 0, 150);
	  hpulse->GetXaxis()->SetTitle("time");
	  hpulse->GetYaxis()->SetTitle("ADC Count");


	  std::string histname2 = "smSignal_" + ss.str();
	  std::string histtitle2 = "Plot of smoothed signal " + ss.str();
	  TH1F* hpulse2 = new TH1F(histname2.c_str(), histtitle2.c_str(), 150, 0, 300);
	  hpulse2->GetXaxis()->SetTitle("time");
	  hpulse2->GetYaxis()->SetTitle("ADC count");


	  //need to get pedestal first

	  int sum = 0, pcount = 10;
	  float pedestal = 0, sigma = 0, sum2 = 0;

	  for(std::vector<int>::iterator sIt = samples.begin(); sIt != samples.begin() + pcount; sIt++)
	    sum += (*sIt);
	  pedestal = sum / pcount;

	  for(std::vector<int>::iterator sIt = samples.begin(); sIt != samples.begin() + pcount; sIt++)
	    sum2 = ((*sIt) - pedestal) * ((*sIt) - pedestal);
	  sigma = sqrt(sum2 / (pcount-1));

	  //now to get the times and integrals for the original signal

	  int tstart = 0, tstop = 0, tpeak = 0, ttail = 0;
	  float peak = 0, samp = 0, threshold = 3 * sigma;
	  double fullInt = 0, tailInt = 0, ratio = 0;

	  if(threshold <= 5)
	    threshold = 5;

	  for(std::vector<int>::iterator sIt = samples.begin(); sIt != samples.end(); sIt++)
	    {

	      samp = (*sIt) - pedestal;
	      int time = std::distance(samples.begin(),sIt);

	      //make our histograms while we're at it

	      hpulse->Fill(time, samp);

	      if((time%3) ==0)
		hpulse2->Fill(time * 3, samp);
	      else if((time%3) == 1)
		hpulse2->Fill((time *3)+1,  samp);
	      else
		hpulse2->Fill((time*3)+2, samp);


	    }


	  peak = hpulse->GetMaximum();
	  tpeak = hpulse->GetMaximumBin();

		
	  //currently I have no means of fitting peak values past 4098
	  //values above 4098 are tagged with an overflow giving a value
	  //of 8096

	  if(peak > 5000)
	    {
	      delete hpulse;
	      delete hpulse2;
	      SmoothingCount -= 1;
	      continue;
	    }

	  hpulse2->Smooth(5);

	  //now we can get the integrals for the original waveform

	  fullInt = hpulse->Integral(tpeak - 3, tpeak + 20);
	  tailInt = hpulse->Integral(tpeak + 5, tpeak + 20);

	  ratio = tailInt / fullInt;

	  //now we need to look through the smoothed sample
	  //this will have to be done some other way as the iterator
	  //method does not quite work.

	  int tsmstart = 0, tsmtail = 0,tsmpeak = 0, tsmstop = 0;
	  float smpeak = 0;
	  double smFullInt = 0, smTailInt = 0, smRatio = 0;

	  smpeak = hpulse2->GetMaximum();
	  tsmpeak = hpulse2->GetMaximumBin();

	  smFullInt = hpulse2->Integral(tsmpeak - 9, tsmpeak + 60);
	  smTailInt = hpulse2->Integral(tsmpeak + 15, tsmpeak + 60);
	  smRatio = smTailInt / smFullInt;


	  double Energy = smpeak * 0.0056433;

	  NGSmooth_plots[keyname]->Fill(smRatio, Energy);

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
	  delete hpulse2;





	}  //end pulse loop

    }  //end map loop

  return 0;
}  //end function


