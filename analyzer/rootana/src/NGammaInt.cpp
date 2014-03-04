///////////////////////////////////////////////////////
//  NGammaInt        for use in rootana for Alcap    //
//                                                   //
//  Damien Alexander                                 // 
//                                                   //
//  A Module to Discriminate neutron signals from    //
//  gamma signals in a neutron detector, using the   //
//  ratio of the full integral to the tail integral. //
///////////////////////////////////////////////////////

#define USE_PRINT_OUT 1

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>

#include "TH1.h"

static TH1F* hNeutrons1;
static TH1F* hNeutrons2;
static TH2L* hNGDisc1;
static TH2L* hNGDisc2;          //I need to split these up some more
static TH1F* hNGRatio1;         //I probably need either NGDisc or ratio
static TH1F* hNGRatio2;         //not both


NGammaInt::NGammaInt(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName)
  {
    dir->cd("/");
  }

NGammaInt::~NGammaInt()
{
}

int NGammaInt::ProcessEntry(TGlobalData *gdata, TSetupData *gSetup)
{
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  for(map_iterator mapIter = gData->fPulseIslandToChannelMap.begin(); mapIter != gData->fPulseIslandToChannelMap.end(); mapIter++)
  {
    std::string bankname = mapIter->first;
    std::string detname = gSetup->GetDetectorName(bankname);
    
    //  I'm skipping the other detectors at this point.
    if((detname != "NDet") || (detname != "NDet2"))
      continue;         
   
    std::vector<TPulseIsland*> pulses = mapIter->second;

    if(pulses.size() == 0)
      continue;          // skip if no pulse
   
    for(std::vector<TPulseIsland*>::iterator pIter = pulses.begin(); pIter != pulses.end(); pIter++)
    {
      std::vector<int> samples = *pIter->GetSamples();
    
      std::vector<int>::iterator tstart, tstop;
      int peak, dec_count, inc_count;
      long integral, late_integral;
      bool double_check;
  


      for(std::vector<int>::iterator sIter = samples.begin(); sIter != samples.end(); sIter++) 
      {

        if(sIter == samples.begin())
	{
          tstart = 0;
          tstop = 0;
          peak = 0;
          double_check = 0;
	}


	//need to write in a threshold value and still need pedestal subtraction


	if((*sIter > threshold) && (tstart == 0)) 
	  tstart = sIter;      //determining the waveform start time

        if((tstart != 0) && (*sIter < threshold) && (tstop == 0))
	  tstop = sIter;       //determine the end of the waveform

	if((tstart != 0) && (*sIter > peak))
	  peak = *sIter;        //find peak from max height (revisit)

        // all three of these need more proper treatment later.
        // For the height I should fit a landau curve
        // to the waveform.  This similarly can be used to find
        // a more accurate start time (time at 20% maxon rising edge?
        // that would need another iteration of the samples)


        //check for overlapping double peaks.  look for a declining
        //trend followed by a rising trend.  This is touchy and
        //depends heavily on which pulse we're using.  The neutron
        //pulses are short so the rising trend needs to be very short

        if((tstart != 0) && (double_check == 0) && (tstop == 0))
	{                                                
          if((*sIter < *(sIter - 1)) && (dec_count < 2))  
	    dec_count += 1;    //if started and on a falling edge, count
          if((*sIter > *(sIter - 1)) && (dec_count < 2))
            dec_count = 0;     //to stop false falling edges require 3 in a row
          if((dec_count == 2) && (*sIter > *(sIter - 1)))
            inc_count += 1;    //now look for a rising edge before pulse ends
          if((inc_count < 1) && (dec_count == 2) && (*sIter < *(sIter-1)))
            inc_count = 0;     //require 2 in a row to rise after falling begins
          if((inc_count == 1) && (dec_count == 2))
	  {
            double_check = 1;
	    break;         // need to skip if found
	  }
	}

        if((tstop != 0) && (*sIter > threshold))
	{
	  double_check = 1;    // checking for separated double peaks
	  break;            //need to skip if found  I may not need the bool
	}




        //starting the integration at this point

        if((tstart != 0) && (*sIter > minThreshold))
          fullInt += *sIter;    //adding the value to the full interal

	if((tstart != 0) && (*sIter < peak) && (*sIter > minThreshold))
	  tailInt += *sIter;    //adding the value to the tail integral

        
      }  // closing the sample loop    

      //at this point we need to skip the pulse if double_peak
      if(double_check == 1)
        continue;



      ratio = fullInt/tailInt;    
      if(bankname == NDet)
      {
        hNGDisc1->Fill(fullInt, tailInt);
        hNGRatio1->Fill(ratio);       //fill the histograms for NDet
      }
      if(bankname == NDet2)
      {
        hNGDisc2->Fill(fullInt, tailInt);
        hNGRatio2->Fill(ratio);      
      }
    }//close pulse loop
  }//close map loop
}

//this is not done and I still need to check syntax, but it's a start.       




//be sure to define you class in a .h file to include

//define pedestal for each signal and threshold
//use saved value for now, there will be an updated version eventually
//consider a separate improved code for the pedestal and pedestal subtraction

