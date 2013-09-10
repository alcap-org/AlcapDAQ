#include "FastSlowPulseAnalysis.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <map>
#include <utility>

#include "TH1.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

static bool verbose = false;
static TH1 *hTest = 0;
static int fast_pulse_counter = 1;
static int slow_pulse_counter = 1;

FastSlowPulseAnalysis::FastSlowPulseAnalysis(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){
  hTest = new TH1F("hTest", "hTest", 10,0,10);
  dir->cd("/");
}

FastSlowPulseAnalysis::~FastSlowPulseAnalysis(){
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
  	TH1F* hFastPulse = new TH1F(histname.str().c_str(), histname.str().c_str(), 200,0,200);
  
 	std::vector<int> theSamples = (*islandIter)->GetSamples();
	  
  	// Loop through the samples
  	int sampleCounter = 0;
  	for (int_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
  		
  		hFastPulse->Fill(sampleCounter, *sampleIter);
  		sampleCounter++;
  	}
  }
  
  // Loop through the slow pulse islands and plot the pulse
  for (island_iterator islandIter = slow_pulse_islands.begin(); islandIter != slow_pulse_islands.end(); islandIter++) {
  	std::stringstream histname;
  	histname << "SlowPulse" << slow_pulse_counter << "_Island" << islandIter - slow_pulse_islands.begin();;
  	slow_pulse_counter++;
  	TH1F* hSlowPulse = new TH1F(histname.str().c_str(), histname.str().c_str(), 200,0,200);
  
 	std::vector<int> theSamples = (*islandIter)->GetSamples();
	  
  	// Loop through the samples
  	int sampleCounter = 0;
  	for (int_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
  		
  		hSlowPulse->Fill(sampleCounter, *sampleIter);
  		sampleCounter++;
  	}
  }
  return 0;
}

