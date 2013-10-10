// PulseFinder.cpp
// Date: 10th Oct 2013
// Author: Andrew Edmonds
// Comments: Pulse finder module based on the method described in elog:43 of the analysis-run09 elog

#include "PulseFinder.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>

#include "TH1.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

static bool verbose = false;
static TH1 *hPulse = 0;

PulseFinder::PulseFinder(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){
  hPulse = new TH1F("hPulse", "The pulse found", 15, -1.5, 13.5);
  dir->cd("/");
}

PulseFinder::~PulseFinder(){
}

int PulseFinder::ProcessEntry(TGlobalData *gData){
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;
  typedef vector<TPulseIsland*>::iterator island_iterator;

  vector<TPulseIsland*> islands;

  for(map_iterator iter = gData->fPulseIslandToChannelMap.begin();
      iter != gData->fPulseIslandToChannelMap.end(); iter++){
    if(strcmp((iter->first).data(), "Nhe0") == 0){
      islands = iter->second;
    }   
  }

  for (island_iterator islandIter = islands.begin(); islandIter != islands.end(); islandIter++) {
  
  	// Get pedestal (mean of first 10 bins)
  	std::vector<int> theSamples = (*islandIter)->GetSamples();
  	double pedestal = GetPedestal(theSamples);
  	std::cout << pedestal << std::endl;
  }


  return 0;
}


// GetPedestal()
// -- Gets the pedestal as the mean of the first 10 bins
double PulseFinder::GetPedestal(std::vector<int> samples) {

  // Loop through the first few elements and take the mean value for the pedestal
  double pedestal = 0.0;
  int nBinsForMean = 10;
  for (int i = 0; i < nBinsForMean; i++) {
    pedestal += samples[i];
  }
  pedestal /= nBinsForMean;
  
  return pedestal;
}
