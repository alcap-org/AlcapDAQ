// TimingTest.cpp
// Author: Andrew Edmonds
// Created: 12/09/2013
// -- The timing test analysis for run 356 (elog:116)
// Banks:
// -- FADC C, Ch 7 (Bank: Nhe0) = 2ns delay
// -- FADC C, Ch 6 (Bank: Nge0) = 5ns delay
// -- FADC B, Ch 7 (Bank: Nhc0) = 5ns delay
// -- FADC A. Ch 7 (Bank: Nh80) = 1ns + 5ns delay
// -- FADC B, Ch 5 (Bank: Nfc0) = 1ns + 2ns delay

#include "TimingTest.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <cmath>
#include <map>
#include <utility>

#include "TH1.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

static bool verbose = false;
//static TH1 *hCCh7 = 0;

static int CCh7_counter = 1;


TimingTest::TimingTest(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){

  dir->cd("/");
}

TimingTest::~TimingTest() {
}

int TimingTest::ProcessEntry(TGlobalData *gData){
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;
  typedef vector<TPulseIsland*>::iterator island_iterator;
  typedef vector<int>::iterator int_iterator;

  vector<TPulseIsland*> C_Ch7_islands, C_Ch6_islands, B_Ch7_islands, A_Ch7_islands, B_Ch5_islands;

  for(map_iterator iter = gData->fPulseIslandToChannelMap.begin();
      iter != gData->fPulseIslandToChannelMap.end(); iter++){
    if(strcmp((iter->first).data(), "Nhe0") == 0){
      C_Ch7_islands = iter->second;
    }
    else if(strcmp((iter->first).data(), "Nge0") == 0){
      C_Ch6_islands = iter->second;
    } 
    else if(strcmp((iter->first).data(), "Nhc0") == 0){
      B_Ch7_islands = iter->second;
    }  
    else if(strcmp((iter->first).data(), "Nh80") == 0){
      A_Ch7_islands = iter->second;
    }  
    else if(strcmp((iter->first).data(), "Nfc0") == 0){
      B_Ch5_islands = iter->second;
    }
  }
  
  // Loop through the FADC C Ch 7 pulse islands and plot the pulse
  for (island_iterator islandIter = C_Ch7_islands.begin(); islandIter != C_Ch7_islands.end(); islandIter++) {
  	std::stringstream histname;
  	histname << "FADC-C_Ch-7_Number" << CCh7_counter << "_Island" << islandIter - C_Ch7_islands.begin();;
  	CCh7_counter++;
  	
  	TH1F* hCCh7 = new TH1F(histname.str().c_str(), histname.str().c_str(), 100,0,100);
  	hCCh7->GetXaxis()->SetTitle("Clock Ticks (since time stamp)");
  	hCCh7->GetYaxis()->SetTitle("Sample Value");
  
 	std::vector<int> theSamples = (*islandIter)->GetSamples();
	  
  	// Loop through the samples
  	int sampleCounter = 0;
  	for (int_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
  		
  		hCCh7->Fill(sampleCounter, *sampleIter);
  		sampleCounter++;
  	}
  	
  	// Now calibrate the pulse with 1 clock tick = clickTimeInNs()
  	TH1F* hCalib = (TH1F*) Calibrate(hCCh7, 1, (*islandIter)->GetClockTickInNs());
  	hCalib->GetXaxis()->SetTitle("time / ns");
  	
/*  	// Now remove the pedestal
  	TH1F* hPedSub = (TH1F*) RemovePedestal(hCalib);
  	
  	// Get the pulse integral
  	fast_pulse_integrals.push_back(GetPulseIntegral(hPedSub));*/
  }
  
  return 0;
}

// Calibrate()
// -- Takes a calibration point (i.e. x --> new_x) and calculates the scaling factor
// -- Reassigns the old bin contents to new bin centers
TH1* TimingTest::Calibrate(TH1* hist, double x, double new_x) {

  std::stringstream calibHistname;
  calibHistname << "Calib_" << hist->GetName();

  double scaling_factor = new_x / x;
  double low_val = hist->GetBinLowEdge(1) * scaling_factor;
  double high_val = hist->GetBinLowEdge(hist->GetXaxis()->GetNbins() + 1) * scaling_factor;
  double n_bins = hist->GetXaxis()->GetNbins();

  TH1F* hCalib  = new TH1F(calibHistname.str().c_str(), calibHistname.str().c_str(), n_bins, low_val, high_val);

  hCalib->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
  hCalib->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());


  // Now fill the calibrated histogram
  for (int iBin = 1; iBin <= hist->GetXaxis()->GetNbins(); iBin++) {
    double binCenter = hist->GetBinCenter(iBin) * scaling_factor;
    double binContent = hist->GetBinContent(iBin);

    if (binContent != 0) // ignore empty bins
      hCalib->Fill(binCenter, binContent);

  }
  return hCalib;
}
