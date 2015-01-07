#include "TestVeto.h"
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

std::map<std::string, TH1F*> vetoSignals;

int VetoCount = 0;

TestVeto::TestVeto(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName)
  {
    dir->cd("/");
  } 

TestVeto::~TestVeto()
{ }

int TestVeto::ProcessEntry(TGlobalData *gData, TSetupData *gSetup)
{

  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  for(map_iterator mapIter = gData->fPulseIslandToChannelMap.begin(); mapIter != gData->fPulseIslandToChannelMap.end(); mapIter++)
    {
      std::string bankname = mapIter->first;
      std::string detname = gSetup->GetDetectorName(bankname);
      std::string keyname = mapIter->first + GetName();

      if(detname != "ScVe")
	continue;


      std::vector<TPulseIsland*> pulses = mapIter->second;

      if(pulses.size() == 0)
	continue;

      for(std::vector<TPulseIsland*>::iterator pIter = pulses.begin(); pIter != pulses.end(); pIter++)
	{

	  std::vector<int> samples = (*pIter)->GetSamples();

	  //int length = std::distance(samples.begin(), samples.end());

	  if(VetoCount > 50)
	    continue; 
  
	  VetoCount += 1;
	  std::stringstream ss;
	  ss<<VetoCount;
	  std::string histname1 = "signal_"  + ss.str();
	  std::string histtitle1 = "Plot of veto signal " + ss.str();
	  TH1F* hpulse = new TH1F(histname1.c_str(), histtitle1.c_str(), samples.size(), 0, samples.size());
	  hpulse->GetXaxis()->SetTitle("time");
	  hpulse->GetYaxis()->SetTitle("ADC Count");

	  int sum = 0, pcount = 10;
	  float pedestal = 0;

	  for(std::vector<int>::iterator sIt = samples.begin(); sIt != samples.begin() + pcount; sIt++)
	    sum += (*sIt);
	  pedestal = sum / pcount;


	  for(std::vector<int>::iterator sIt = samples.begin(); sIt != samples.end(); sIt++)
	    {

	      int time = std::distance(samples.begin(),sIt), samp = (*sIt);


	      //make our histograms

	      hpulse->Fill(time, samp);

	    }

	  int peak = hpulse->GetMinimum();

	  if((pedestal - peak) < 10)
	    {
	      delete hpulse;
	      VetoCount -= 1;
	    }
	  
	  //need to output the data
	  /*
	  cout << "for pulse number " << ss << " :"<< endl;
	  cout << "Pedestal " << pedestal << "   sigma " << sigma << "  Threshold " << threshold << endl;

	  cout << "For the original pulse :" << endl;
	  cout << "Peak " << peak << "   Start time " << tstart << "   Tail start " << ttail << endl;
	  cout << "End time " << tstop << "   Full Integral " << fullInt << "   Tail Integral " << tailInt << "   Ratio " << ratio << endl;
	  */

	  //delete hpulse;
	  //delete hpulse2;
	  




	}  //end pulse loop

    }  //end map loop

  return 0;
}  //end function


