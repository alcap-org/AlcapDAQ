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
  
  int bin_min = 0; int bin_max = 50; int n_bins = bin_max;

  vector<TPulseIsland*> islands;

  for(map_iterator iter = gData->fPulseIslandToChannelMap.begin();
      iter != gData->fPulseIslandToChannelMap.end(); iter++){
    if(strcmp((iter->first).data(), "Nhe0") == 0){
      islands = iter->second;
    }   
  }

  for (island_iterator islandIter = islands.begin(); islandIter != islands.end(); islandIter++) {
  
  	// Create the histogram
  	std::stringstream histname;
  	histname << "Entry" << entry_counter << "_Island" << islandIter - islands.begin();
  	TH1F* hIsland = new TH1F(histname.str().c_str(), histname.str().c_str(), n_bins, bin_min, bin_max);
  	
  	// Get the samples
  	std::vector<int> theSamples = (*islandIter)->GetSamples();
  	
  	// Fill the histogram
  	for (int_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
  		hIsland->Fill(sampleIter - theSamples.begin(), *sampleIter);
  	}
  	
  	// Get the pedestal and RMS
  	double pedestal = 0; double RMS = 0;
  	GetPedestalAndRMS(theSamples, pedestal, RMS);
  	
  	std::stringstream canvasname;
  	canvasname << "Canvas_Entry" << entry_counter << "_Island" << islandIter - islands.begin();
  	TCanvas* c1 = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());
  	
  	hIsland->Draw();
  	
  	// Plot a line on the histogram of the pedestal and RMS
  	TLine* pedestal_line = new TLine(bin_min, pedestal, bin_max, pedestal);
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
