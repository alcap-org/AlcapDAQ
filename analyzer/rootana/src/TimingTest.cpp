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
static TH1 *hC7_times = 0;
static int C7_counter = 1;

static TH1 *hC6_times = 0;
static int C6_counter = 1;


TimingTest::TimingTest(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){
  
  hC7_times = new TH1F("hC7_times", "hC7_times", 200, 0, 200);
  hC6_times = new TH1F("hC6_times", "hC6_times", 200, 0, 200);

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

  vector<TPulseIsland*> C7_islands, C6_islands, B7_islands, A7_islands, B5_islands;

  for(map_iterator iter = gData->fPulseIslandToChannelMap.begin();
      iter != gData->fPulseIslandToChannelMap.end(); iter++){
    if(strcmp((iter->first).data(), "Nhe0") == 0){
      C7_islands = iter->second;
    }
    else if(strcmp((iter->first).data(), "Nge0") == 0){
      C6_islands = iter->second;
    } 
    else if(strcmp((iter->first).data(), "Nhc0") == 0){
      B7_islands = iter->second;
    }  
    else if(strcmp((iter->first).data(), "Nh80") == 0){
      A7_islands = iter->second;
    }  
    else if(strcmp((iter->first).data(), "Nfc0") == 0){
      B5_islands = iter->second;
    }
  }
  
  // Loop through the FADC C Ch 7 pulse islands and plot the pulse
  for (island_iterator islandIter = C7_islands.begin(); islandIter != C7_islands.end(); islandIter++) {
  	std::stringstream histname;
  	histname << "C7_Number" << C7_counter << "_Island" << islandIter - C7_islands.begin();;
  	C7_counter++;
  	
  	TH1F* hC7 = new TH1F(histname.str().c_str(), histname.str().c_str(), 100,0,100);
  	hC7->GetXaxis()->SetTitle("Clock Ticks (since time stamp)");
  	hC7->GetYaxis()->SetTitle("Sample Value");
  
 	std::vector<int> theSamples = (*islandIter)->GetSamples();
	  
  	// Loop through the samples
  	int sampleCounter = 0;
  	for (int_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
  		
  		hC7->Fill(sampleCounter, *sampleIter);
  		sampleCounter++;
  	}
  	
  	// Now calibrate the pulse with 1 clock tick = clickTimeInNs()
  	TH1F* hCalib = (TH1F*) Calibrate(hC7, 1, (*islandIter)->GetClockTickInNs());
  	hCalib->GetXaxis()->SetTitle("time / ns");
  	
  	// Now remove the pedestal
  	TH1F* hPedSub = (TH1F*) RemovePedestal(hCalib);
  	
  	hC7_times->Fill(GetPulseTime(hPedSub));
  }
  
  // Loop through the FADC C Ch 6 pulse islands and plot the pulse
  for (island_iterator islandIter = C6_islands.begin(); islandIter != C6_islands.end(); islandIter++) {
  	std::stringstream histname;
  	histname << "C6_Number" << C6_counter << "_Island" << islandIter - C6_islands.begin();;
  	C6_counter++;
  	
  	TH1F* hC6 = new TH1F(histname.str().c_str(), histname.str().c_str(), 100,0,100);
  	hC6->GetXaxis()->SetTitle("Clock Ticks (since time stamp)");
  	hC6->GetYaxis()->SetTitle("Sample Value");
  
 	std::vector<int> theSamples = (*islandIter)->GetSamples();
	  
  	// Loop through the samples
  	int sampleCounter = 0;
  	for (int_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
  		
  		hC6->Fill(sampleCounter, *sampleIter);
  		sampleCounter++;
  	}
  	
  	// Now calibrate the pulse with 1 clock tick = clickTimeInNs()
  	TH1F* hCalib = (TH1F*) Calibrate(hC6, 1, (*islandIter)->GetClockTickInNs());
  	hCalib->GetXaxis()->SetTitle("time / ns");
  	
  	// Now remove the pedestal
  	TH1F* hPedSub = (TH1F*) RemovePedestal(hCalib);
  	
  	hC6_times->Fill(GetPulseTime(hPedSub));
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

// RemovePedestal()
// -- Assumes the pedestal is the mean of the first five bins
// -- Subtracts this from all the values
TH1* TimingTest::RemovePedestal(TH1* hist) {

  std::stringstream pedSubHistname;
  pedSubHistname << "PedSub_" << hist->GetName();

  double low_val = hist->GetBinLowEdge(1);
  double high_val = hist->GetBinLowEdge(hist->GetXaxis()->GetNbins() + 1);
  int n_bins = hist->GetXaxis()->GetNbins();

  // Create the pedestal subtracted histogram
  TH1F* hPedSub = new TH1F(pedSubHistname.str().c_str(), pedSubHistname.str().c_str(), n_bins,low_val,high_val);
  hPedSub->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
  hPedSub->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());  

  // Loop through the first few bins and take the mean value for the pedestal
  double pedestal = 0.0;
  int nBinsForMean = 5;
  for (int iBin = 1; iBin <= nBinsForMean; iBin++) {
    pedestal += hist->GetBinContent(iBin);
  }
  pedestal /= nBinsForMean;

  // Loop through the bins (NB bin numbering starts at 1!)
  for (int iBin = 1; iBin <= hist->GetXaxis()->GetNbins(); iBin++) {
    double binValue = hist->GetBinContent(iBin);
    float binCenter = hist->GetBinCenter(iBin);

    if (binValue != 0) { // make sure the bin's not empty
      hPedSub->Fill(binCenter, binValue - pedestal);
    }
  }

  return hPedSub;
}

// GetPulseTime()
// - Returns the time of the pulse height
// - NB assuming a negative pulse
double TimingTest::GetPulseTime(TH1* pulse_hist) {
  
  double pulse_time = pulse_hist->GetBinCenter(pulse_hist->GetMinimumBin());
  return pulse_time;
}
