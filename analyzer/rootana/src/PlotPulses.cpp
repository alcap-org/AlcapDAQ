// PlotPulses.cpp
// Author: Andrew Edmonds
// Created: 19/09/2013
// -- A module that plots the calibrated pulse and the pedestal subtracted pulse

#include "PlotPulses.h"
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
static int entry_counter = 1;


PlotPulses::PlotPulses(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){

  dir->cd("/");
}

PlotPulses::~PlotPulses() {
}

int PlotPulses::ProcessEntry(TGlobalData *gData){
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;
  typedef vector<TPulseIsland*>::iterator island_iterator;
  typedef vector<int>::iterator int_iterator;


  for(map_iterator iter = gData->fPulseIslandToChannelMap.begin();
      iter != gData->fPulseIslandToChannelMap.end(); iter++){
      
    // Get the vector of TPulseIslands
    std::vector <TPulseIsland*> theIslands = iter->second;
    
    // Go through the TPulseIslands
    for (island_iterator islandIter = theIslands.begin(); islandIter != theIslands.end(); islandIter++) {
  		std::stringstream histname;
  		histname << "Entry" << entry_counter << "Bank" << iter->first << "_Island" << islandIter - theIslands.begin();;
		
		// Plot the pulse as it is
		TH1F* hPulse = new TH1F(histname.str().c_str(), histname.str().c_str(), 100,0,100);
  		hPulse->GetXaxis()->SetTitle("Clock Ticks (since time stamp)");
  		hPulse->GetYaxis()->SetTitle("Sample Value");
  
 		std::vector<int> theSamples = (*islandIter)->GetSamples();
 		
 		// Loop through the samples
  		int sampleCounter = 0;
  		for (int_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
  		
  			hPulse->Fill(sampleCounter, *sampleIter);
  			sampleCounter++;
  		}
  		
  		// Now calibrate the pulse with 1 clock tick = clickTimeInNs()
  		TH1F* hCalib = (TH1F*) Calibrate(hPulse, 1, (*islandIter)->GetClockTickInNs());
  		hCalib->GetXaxis()->SetTitle("time / ns");
  	
  		// Now remove the pedestal
  		TH1F* hPedSub = (TH1F*) RemovePedestal(hCalib);
  	}
  }
  
  entry_counter++;
  return 0;
}

// Calibrate()
// -- Takes a calibration point (i.e. x --> new_x) and calculates the scaling factor
// -- Reassigns the old bin contents to new bin centers
TH1* PlotPulses::Calibrate(TH1* hist, double x, double new_x) {

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
TH1* PlotPulses::RemovePedestal(TH1* hist) {

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
