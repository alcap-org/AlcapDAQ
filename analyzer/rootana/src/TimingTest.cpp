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
static TH1 *hC6_times = 0;


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
  
 	std::vector<int> theSamples = (*islandIter)->GetSamples();
  	
  	// Remove the pedestal
  	std::vector<double> thePedSubSamples = RemovePedestal(theSamples);
  	
  	hC7_times->Fill(GetPulseTime(thePedSubSamples) * (*islandIter)->GetClockTickInNs());
  }
  
  // Loop through the FADC C Ch 6 pulse islands and plot the pulse
  for (island_iterator islandIter = C6_islands.begin(); islandIter != C6_islands.end(); islandIter++) {
  
 	std::vector<int> theSamples = (*islandIter)->GetSamples();
  	
  	// Remove the pedestal
  	std::vector<double> thePedSubSamples = RemovePedestal(theSamples);
  	
  	hC6_times->Fill(GetPulseTime(thePedSubSamples) * (*islandIter)->GetClockTickInNs());
  }
  
  return 0;
}

// RemovePedestal()
// -- Assumes the pedestal is the mean of the first five bins
// -- Subtracts this from all the values
std::vector<double> TimingTest::RemovePedestal(std::vector<int> samples) {

  std::vector<double> pedSubSamples;

  // Loop through the first few elements and take the mean value for the pedestal
  double pedestal = 0.0;
  int nBinsForMean = 5;
  for (int i = 0; i < nBinsForMean; i++) {
    pedestal += samples[i];
  }
  pedestal /= nBinsForMean;

  // Loop through the elements
  for (std::vector<int>::iterator it = samples.begin(); it != samples.end(); it++) {
    pedSubSamples.push_back( (*it) - pedestal);
  }

  return pedSubSamples;
}

// GetPulseTime()
// - Returns the time of the pulse height
// - Finds the minimum value of the sample and returns the element number
double TimingTest::GetPulseTime(std::vector<double> samples) {
  
  std::vector<double>::iterator theElement = std::min_element(samples.begin(), samples.end());
  double pulse_height = *theElement;
  double element_number = theElement - samples.begin();
  
  return element_number;
}
