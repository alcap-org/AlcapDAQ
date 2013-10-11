// PulseFinder.cpp
// Author: Andrew Edmonds
// Created: 10th Oct 2013
// -- Pulse finder module based on the method described in elog:43 of the analysis-run09 elog

#include "PulseFinder.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>

#include <sstream>
#include <cmath>

#include "TH1.h"

#include "TCanvas.h"
#include "TLine.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

static bool verbose = true;
static int entry_counter = 1;

PulseFinder::PulseFinder(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){
  dir->cd("/");
}

PulseFinder::~PulseFinder(){
}

int PulseFinder::ProcessEntry(TGlobalData *gData){
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;
  typedef vector<TPulseIsland*>::iterator island_iterator;
  typedef vector<int>::iterator int_iterator;

  vector<TPulseIsland*> islands;

  for(map_iterator iter = gData->fPulseIslandToChannelMap.begin();
      iter != gData->fPulseIslandToChannelMap.end(); iter++){
    if(strcmp((iter->first).data(), "Nhe0") == 0){
      islands = iter->second;
    }   
  }

  for (island_iterator islandIter = islands.begin(); islandIter != islands.end(); islandIter++) {
  
  	if (verbose)
  		std::cout << "Entry " << entry_counter << " Island " << islandIter - islands.begin() << std::endl;
  		
  	// Get the samples
  	std::vector<int> theSamples = (*islandIter)->GetSamples();
  	
  	
  	// Create the histogram
  	std::stringstream histname;
  	histname << "Entry" << entry_counter << "_Island" << islandIter - islands.begin();
  	
  	int bin_min = 0; int bin_max = theSamples.size(); int n_bins = bin_max;
  	TH1F* hIsland = new TH1F(histname.str().c_str(), histname.str().c_str(), n_bins, bin_min, bin_max);
  	
  	
  	// Fill the histogram
  	for (int_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
  		hIsland->Fill(sampleIter - theSamples.begin(), *sampleIter);
  	}
  	
  	
  	// Get the pedestal and RMS
  	double pedestal = 0; double RMS = 0;
  	GetPedestalAndRMS(theSamples, pedestal, RMS);
  	
  	std::stringstream canvasname;
  	canvasname << "Entry" << entry_counter << "_Island" << islandIter - islands.begin() << "_Canvas";
  	TCanvas* c1 = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());
  	
  	hIsland->Draw();
  	
  	
  	// Plot a line on the histogram of the pedestal and RMS
  	TLine* pedestal_line = new TLine(bin_min, pedestal, bin_max, pedestal);
  	pedestal_line->SetLineColor(kRed);
    pedestal_line->SetLineStyle(1);
    pedestal_line->Draw();
    
  	TLine* upper_rms_line = new TLine(bin_min, pedestal+RMS, bin_max, pedestal+RMS);
  	upper_rms_line->SetLineColor(kRed);
    upper_rms_line->SetLineStyle(7);
    upper_rms_line->Draw();
    
  	TLine* lower_rms_line = new TLine(bin_min, pedestal-RMS, bin_max, pedestal-RMS);
  	lower_rms_line->SetLineColor(kRed);
    lower_rms_line->SetLineStyle(7);
    lower_rms_line->Draw();
    
  	c1->Update();
  	c1->Write();
    
    if (verbose) {
    	std::cout << "Pedestal Line: (" << pedestal_line->GetX1() << "," << pedestal_line->GetY1() 
    				<< ") to (" << pedestal_line->GetX2() << "," << pedestal_line->GetY2() << ")" << std::endl;
    	std::cout << "Upper RMS Line: (" << upper_rms_line->GetX1() << "," << upper_rms_line->GetY1() 
    				<< ") to (" << upper_rms_line->GetX2() << "," << upper_rms_line->GetY2() << ")" << std::endl;
    	std::cout << "Lower RMS Line: (" << lower_rms_line->GetX1() << "," << lower_rms_line->GetY1() 
    				<< ") to (" << lower_rms_line->GetX2() << "," << lower_rms_line->GetY2() << ")" << std::endl;
    }
    
    
    
    // Loop through the samples and find the start of the pulse
    // (when sample[i] < pedestal and sample[i+2] < pedestal - 5*RMS)
    // NB assuming negative pulses
    int pulse_counter = 0;
    bool pulse_found = false;
    std::vector<TH1F*> hPulseHists;
    
    // NB end two elements before the end so that the iterator isn't inspecting some random bit of memory
    for (int_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end()-2; sampleIter++) {
    
  		if ( *(sampleIter) < pedestal && *(sampleIter+2) < (pedestal - 5*RMS) && *(sampleIter+1) < *(sampleIter) && pulse_found == false) {
  			// Found a pulse
  			if (verbose) {
  				std::cout << "Start of pulse found!" << std::endl;
  				std::cout << "Sample Value = " << *(sampleIter) << ", element = " << sampleIter - theSamples.begin() << std::endl;
  				std::cout << "Sample Value + 2 = " << *(sampleIter+2) << ", element = " << sampleIter + 2 - theSamples.begin() << std::endl;
  			}
  			
  			// Create a new histogram to store the found pulse
  			std::stringstream pulsehistname;
  			pulsehistname << "Entry" << entry_counter << "_Island" << islandIter - islands.begin() << "_Pulse" << pulse_counter;
  			pulse_counter++;
  			
  			TH1F* hPulse = new TH1F(pulsehistname.str().c_str(), pulsehistname.str().c_str(), n_bins, bin_min, bin_max);
  			hPulse->SetLineColor(kMagenta);
  			
  			hPulseHists.push_back(hPulse); 			
  			pulse_found = true;
  		}
  		
  		// If a pulse has been found, fill the last histogram in the vector (since that should be the most recent pulse)
  		if (pulse_found == true) {
  			(*(hPulseHists.end()-1))->Fill(sampleIter - theSamples.begin(), *sampleIter);
  			
  			if (verbose)
  				std::cout << "Pulse filled at " << sampleIter - theSamples.begin() << " with sample value " << *sampleIter << std::endl;
  			
  			// Check to see if we are at the end of the pulse
  			if (*(sampleIter) > pedestal)
  				pulse_found = false; // no longer have a pulse
  		}
  	} 
    	
    if (verbose) // line break between islands
  		std::cout << std::endl;

  }
  
  entry_counter++;
  return 0;
}


// GetPedestalAndRMS()
// -- Gets the pedestal as the mean of the first 10 bins
// -- and the RMS (i.e. standard deviation, sigma)
void PulseFinder::GetPedestalAndRMS(std::vector<int> samples, double& pedestal, double& RMS) {

  int nBinsForMean = 10; // the number of bins to use for the mean

  // Get the mean
  pedestal = 0.0;
  for (int i = 0; i < nBinsForMean; i++) {
    pedestal += samples[i];
  }
  pedestal /= nBinsForMean;
  
  if (verbose)
  	std::cout << "Pedestal: " << pedestal << std::endl;
  
  // Get the standard deviation
  RMS = 0.0;
  for (int i = 0; i < nBinsForMean; i++) {
    RMS += (samples[i] - pedestal)*(samples[i] - pedestal);
    if (verbose)
    	std::cout << "RMS step " << i << ": " << RMS << ", sample: " << samples[i] << std::endl;
  }
  RMS /= nBinsForMean;
  RMS = std::sqrt(RMS);
  
  if (verbose)
  	std::cout << "RMS: " << RMS << std::endl;
}
