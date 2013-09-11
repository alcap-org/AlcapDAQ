// FastSlowPulseAnalysis.cpp
// Author: Andrew Edmonds
// Created: 10/09/2013
// -- The fast / slow pulse analysis for runs 403 and 406 (elog:112)

#include "FastSlowPulseAnalysis.h"
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
static TH1 *hFastPulseIntegrals = 0;
static TH1 *hSlowPulseHeights = 0;
static TH1 *hFastPulseIntegralsCalib = 0;
static TH1 *hSlowPulseHeightsCalib = 0;

static int fast_pulse_counter = 1;
static int slow_pulse_counter = 1;


FastSlowPulseAnalysis::FastSlowPulseAnalysis(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){

  dir->cd("/");
}

FastSlowPulseAnalysis::~FastSlowPulseAnalysis() {
  dir->cd(); // cd into the module's directory
	
  // Sort the vectors into ascending order to make it easier to decide where to put the histogram limits
  std::sort(slow_pulse_heights.begin(), slow_pulse_heights.end());
  std::sort(fast_pulse_integrals.begin(), fast_pulse_integrals.end());

  // Pulse heights from the slow pulse
  int low_bin_val = *(slow_pulse_heights.begin());
  int high_bin_val = *(slow_pulse_heights.end() - 1);

  hSlowPulseHeights = new TH1F("hSlowPulseHeights", "hSlowPulseHeights", 50, low_bin_val, high_bin_val);

  for (std::vector<double>::iterator it = slow_pulse_heights.begin(); it != slow_pulse_heights.end(); it++) {
    hSlowPulseHeights->Fill(*it);
  }

  hSlowPulseHeights->GetXaxis()->SetTitle("Pulse Height / FADC value");
  hSlowPulseHeights->GetYaxis()->SetTitle("Number of Pulses");
  
  // Pulse integrals from the fast pulse
  low_bin_val = *(fast_pulse_integrals.begin());
  high_bin_val = *(fast_pulse_integrals.end() - 1);

  hFastPulseIntegrals = new TH1F("hFastPulseIntegrals", "hFastPulseIntegrals", 100, low_bin_val, high_bin_val);

  for (std::vector<double>::iterator it = fast_pulse_integrals.begin(); it != fast_pulse_integrals.end(); it++) {
    hFastPulseIntegrals->Fill(*it);
  }

  hFastPulseIntegrals->GetXaxis()->SetTitle("Pulse Integral / FADC value");
  hFastPulseIntegrals->GetYaxis()->SetTitle("Number of Pulses");
	
  // Calibrate the energy by taking the mean of the current histogram and scaling it to the pre-amp output
  double preAmpOutputInMeV = 3.8;
  double pulse_height_mean = hSlowPulseHeights->GetMean();

  hSlowPulseHeightsCalib = (TH1F*) Calibrate(hSlowPulseHeights, pulse_height_mean, preAmpOutputInMeV);

  hSlowPulseHeightsCalib->GetXaxis()->SetTitle("Energy / MeV");
  hSlowPulseHeightsCalib->GetYaxis()->SetTitle("Number of Pulses");
  
  // Now for the fast pulse
  double pulse_integral_mean = hFastPulseIntegrals->GetMean();

  hFastPulseIntegralsCalib = (TH1F*) Calibrate(hFastPulseIntegrals, pulse_integral_mean, preAmpOutputInMeV);

  hFastPulseIntegralsCalib->GetXaxis()->SetTitle("Energy / MeV");
  hFastPulseIntegralsCalib->GetYaxis()->SetTitle("Number of Pulses");
 
  dir->cd("/");
}

int FastSlowPulseAnalysis::ProcessEntry(TGlobalData *gData){
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;
  typedef vector<TPulseIsland*>::iterator island_iterator;
  typedef vector<int>::iterator int_iterator;

  vector<TPulseIsland*> fast_pulse_islands, slow_pulse_islands;

  for(map_iterator iter = gData->fPulseIslandToChannelMap.begin();
      iter != gData->fPulseIslandToChannelMap.end(); iter++){
    if(strcmp((iter->first).data(), "Ng80") == 0){
      fast_pulse_islands = iter->second;
    }
    else if(strcmp((iter->first).data(), "Nh80") == 0){
      slow_pulse_islands = iter->second;
    } 
  }
  
  // Loop through the fast pulse islands and plot the pulse
  for (island_iterator islandIter = fast_pulse_islands.begin(); islandIter != fast_pulse_islands.end(); islandIter++) {
  	std::stringstream histname;
  	histname << "FastPulse" << fast_pulse_counter << "_Island" << islandIter - fast_pulse_islands.begin();;
  	fast_pulse_counter++;
  	TH1F* hFastPulse = new TH1F(histname.str().c_str(), histname.str().c_str(), 100,0,100);
  	hFastPulse->GetXaxis()->SetTitle("Clock Ticks (since time stamp)");
  	hFastPulse->GetYaxis()->SetTitle("Sample Value");
  
 	std::vector<int> theSamples = (*islandIter)->GetSamples();
	  
  	// Loop through the samples
  	int sampleCounter = 0;
  	for (int_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
  		
  		hFastPulse->Fill(sampleCounter, *sampleIter);
  		sampleCounter++;
  	}
  	
  	// Now calibrate the pulse with 1 clock tick = clickTimeInNs()
  	TH1F* hCalib = (TH1F*) Calibrate(hFastPulse, 1, (*islandIter)->GetClockTickInNs());
  	hCalib->GetXaxis()->SetTitle("time / ns");
  	
  	// Now remove the pedestal
  	TH1F* hPedSub = (TH1F*) RemovePedestal(hCalib);
  	
  	// Get the pulse integral
  	fast_pulse_integrals.push_back(GetPulseIntegral(hPedSub));
  }
  
  // Loop through the slow pulse islands and plot the pulse
  for (island_iterator islandIter = slow_pulse_islands.begin(); islandIter != slow_pulse_islands.end(); islandIter++) {
  	std::stringstream histname;
  	histname << "SlowPulse" << slow_pulse_counter << "_Island" << islandIter - slow_pulse_islands.begin();;
  	slow_pulse_counter++;
  	TH1F* hSlowPulse = new TH1F(histname.str().c_str(), histname.str().c_str(), 200,0,200);
  	hSlowPulse->GetXaxis()->SetTitle("Clock Ticks (since time stamp)");
  	hSlowPulse->GetYaxis()->SetTitle("Sample Value");
  
 	std::vector<int> theSamples = (*islandIter)->GetSamples();
	  
  	// Loop through the samples
  	int sampleCounter = 0;
  	for (int_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
  		
  		hSlowPulse->Fill(sampleCounter, *sampleIter);
  		sampleCounter++;
  	}
  	
  	// Now calibrate the pulse with 1 clock tick = clickTimeInNs()
  	TH1F* hCalib = (TH1F*) Calibrate(hSlowPulse, 1, (*islandIter)->GetClockTickInNs());
  	hCalib->GetXaxis()->SetTitle("time / ns");
  	
  	// Now remove the pedestal
  	TH1F* hPedSub = (TH1F*) RemovePedestal(hCalib);
  	
  	// Get the pulse height
  	slow_pulse_heights.push_back(GetPulseHeight(hPedSub));
  }
  
  return 0;
}

// Calibrate()
// -- Takes a calibration point (i.e. x --> new_x) and calculates the scaling factor
// -- Reassigns the old bin contents to new bin centers
TH1* FastSlowPulseAnalysis::Calibrate(TH1* hist, double x, double new_x) {

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
TH1* FastSlowPulseAnalysis::RemovePedestal(TH1* hist) {

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

// GetPulseHeight()
// - Returns the peak height of the histogram
// - NB assuming a negative pulse
double FastSlowPulseAnalysis::GetPulseHeight(TH1* pulse_hist) {
  
  double peak_height = std::fabs(pulse_hist->GetMinimum());
  return peak_height;
}


// GetPulseIntegral()
// - Return the integral of the pulse
// - NB assumes the whole x-axis is part of the pulse (not a problem if the pulse has been pedestal subtracted)
double FastSlowPulseAnalysis::GetPulseIntegral(TH1* pulse_hist) {

  double integral = std::fabs(pulse_hist->Integral(0,100));
  return integral;
}
