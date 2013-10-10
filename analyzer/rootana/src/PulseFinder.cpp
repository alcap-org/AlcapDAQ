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
  
  	// Create the histogram
  	std::stringstream histname;
  	histname << "Entry" << entry_counter << "_Island" << islandIter - islands.begin();
  	TH1F* hIsland = new TH1F(histname.str().c_str(), histname.str().c_str(), 100, 0, 100);
  	
  	// Get the samples
  	std::vector<int> theSamples = (*islandIter)->GetSamples();
  	
  	// Fill the histogram
  	for (int_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
  		hIsland->Fill(sampleIter - theSamples.begin(), *sampleIter);
  	}
  	
  	// Get the pedestal and RMS
  	double pedestal = 0; double RMS = 0;
  	GetPedestalAndRMS(theSamples, pedestal, RMS);
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
  
  if (verbose)
  	std::cout << std::endl;
}
