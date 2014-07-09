#include "TestTiming.h"
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
#include <sstream>

using namespace std;
using std::string;
using std::map;
using std::vector;
using std::pair;

//std::map<std::string, TH1F*> signals;
//std::map<std::string, TH1F*> smSignals;

int TimingFCount = 0;
int TimingSCount = 0;
//std::map<std::string, TH1F*> timing_plots;
//std::map<std::string, TH1F*> length_plots;

TestTiming::TestTiming(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName)
  {
    dir->cd("/");
  } 

TestTiming::~TestTiming()
{ }

int TestTiming::ProcessEntry(TGlobalData *gData, TSetupData *gSetup)
{
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  for(map_iterator mapIter = gData->fPulseIslandToChannelMap.begin(); mapIter != gData->fPulseIslandToChannelMap.end(); mapIter++)
    {
      std::string bankname = mapIter->first;
      std::string detname = gSetup->GetDetectorName(bankname);
      std::string keyname = mapIter->first + GetName();

      if((detname != "Ge-F") && (detname != "Ge-S"))
	continue;
      
      /*
      if(timing_plots.find(keyname) == timing_plots.end() )
	{
	  std::string histname1 = detname + " times";
	  std::string histtitle1 = detname + " times";
	  TH1F* htiming = new TH1F(histname1.c_str(), histtitle1.c_str(), 200000, 0, 25000000);
	  htiming->GetXaxis()->SetTitle("Time");
	  timing_plots[keyname] = htiming;
	}


      if(length_plots.find(keyname) == length_plots.end() )
	{
	  
	  std::string histname1 = detname + " lengths";
	  std::string histtitle1 = detname + " lengths";
	  TH1F* hlength = new TH1F(histname1.c_str(), histtitle1.c_str(), 65, 0, 65);
	  hlength->GetXaxis()->SetTitle("Length (samples)");
	  length_plots[keyname] = hlength;
	}
      */

      std::vector<TPulseIsland*> pulses = mapIter->second;

      if(pulses.size() == 0)
	continue;

      for(std::vector<TPulseIsland*>::iterator pIter = pulses.begin(); pIter != pulses.end(); pIter++)
	{

	  std::vector<int> samples = (*pIter)->GetSamples();

	  int length = std::distance(samples.begin(), samples.end());


          if((TimingFCount < 50) && (length > 1500) && (detname == "Ge-F"))
	    {

	      TimingFCount += 1;
	      std::stringstream ss;
	      ss<<TimingFCount;
	      std::string histname1 = "signal_" +  detname + "_" + ss.str();
	      std::string histtitle1 = "Plot of signal " + ss.str() + "_" + detname;
	      TH1F* hpulse = new TH1F(histname1.c_str(), histtitle1.c_str(), length, 0, length);
	      hpulse->GetXaxis()->SetTitle("time");
	      hpulse->GetYaxis()->SetTitle("ADC Count");


	      //we need to get the timestamp

	      float timestamp = (*pIter)->GetTimeStamp();
	      float time = timestamp * gSetup->GetClockTick(bankname);
	      float samp = 0;

	      for(std::vector<int>::iterator sIt = samples.begin(); sIt != samples.end(); sIt++)
		{

		  samp = (*sIt);
		  int pos = std::distance(samples.begin(), sIt);

		  //make our histogram while we're at it

		  hpulse->Fill(pos, samp);

		}
		

	      //timing_plots[keyname]->Fill(timestamp);
	      //length_plots[keyname]->Fill(length);



	      /*	     
	      
	      if(std::distance(pulses.begin(), pIter) <= 1)
		continue;
	      std::vector<int> psamples = (*(pIter-1))->GetSamples();
	      int plength = std::distance(psamples.begin(), psamples.end());

	      std::string histname2 = "psignal_" +  detname + "_" + ss.str();
	      std::string histtitle2 = "Plot of signal " + ss.str() + "_"+ detname + " precursor";
	      TH1F* hpulse2 = new TH1F(histname2.c_str(), histtitle2.c_str(), plength, 0, plength);
	      hpulse2->GetXaxis()->SetTitle("time");
	      hpulse2->GetYaxis()->SetTitle("ADC Count");


	      //we need to get the timestamp

	      float ptimestamp = (*(pIter-1))->GetTimeStamp();
	      float ptime = ptimestamp * gSetup->GetClockTick(bankname);
	      int psamp = 0;

	      for(std::vector<int>::iterator sIt = psamples.begin(); sIt != psamples.end(); sIt++)
		{

		  psamp = (*sIt);
		  int pos = std::distance(psamples.begin(), sIt);

		  //make our histogram while we're at it

		  hpulse2->Fill(pos, psamp);

		}
		

	      timing_plots[keyname]->Fill(ptimestamp);
	      length_plots[keyname]->Fill(plength);
	      */
	      

	      // need to output here
	      /*
	      cout << "for odd pulse number " << ss.str() << "_" << detname <<  " Presample :"<< endl;
	      cout << "Timestamp " << ptimestamp << "   time " << ptime << "    Length " << plength << endl;
	      

	      cout << "for odd pulse number " << ss.str() << "_" << detname << " :"<< endl;
	      cout << "Timestamp " << timestamp << "   time " << time << "    Length " << length << endl;
	      */


	    }   // end count loop


          if((TimingSCount < 50) && (length > 1500) && (detname == "Ge-S"))
	    {

	      TimingSCount += 1;
	      std::stringstream ss;
	      ss<<TimingSCount;
	      std::string histname1 = "signal_" +  detname + "_" + ss.str();
	      std::string histtitle1 = "Plot of signal " + ss.str() + "_" + detname;
	      TH1F* hpulse = new TH1F(histname1.c_str(), histtitle1.c_str(), length, 0, length);
	      hpulse->GetXaxis()->SetTitle("time");
	      hpulse->GetYaxis()->SetTitle("ADC Count");


	      //we need to get the timestamp

	      float timestamp = (*pIter)->GetTimeStamp();
	      float time = timestamp * gSetup->GetClockTick(bankname);
	      float samp = 0;

	      for(std::vector<int>::iterator sIt = samples.begin(); sIt != samples.end(); sIt++)
		{

		  samp = (*sIt);
		  int pos = std::distance(samples.begin(), sIt);

		  //make our histogram while we're at it

		  hpulse->Fill(pos, samp);

		}
		

	      //timing_plots[keyname]->Fill(timestamp);
	      //length_plots[keyname]->Fill(length);

	    }//end count loop



	  
	}  //end pulse loop

    }  //end map loop

  return 0;
}  //end function


