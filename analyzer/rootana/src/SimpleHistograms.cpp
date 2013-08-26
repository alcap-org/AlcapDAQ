#include "SimpleHistograms.h"
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
static TH1 *hNumberOfNoCoincidences = 0;

SimpleHistograms::SimpleHistograms(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){
  hNumberOfNoCoincidences = new TH1F("hNumberOfNoCoincidences", "Number of no coincidences found", 15, -1.5, 13.5);
}

SimpleHistograms::~SimpleHistograms(){
}

int SimpleHistograms::ProcessEntry(TGlobalData *gData){
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  vector<TPulseIsland*> islands1, islands2;

  for(map_iterator iter = gData->fPulseIslandToChannelMap.begin();
      iter != gData->fPulseIslandToChannelMap.end(); iter++){
    if(strcmp((iter->first).data(), "Nhe0") == 0){
      islands1 = iter->second;
    } 
    if(strcmp((iter->first).data(), "Nhc0") == 0){
      islands2 = iter->second;
    }     
  }

  int last = 0;
  int NumberOfNoCoincidences = 0;

  if(verbose){
    for(unsigned int i = 0; i < islands1.size(); ++i){
      std::cout << "i = " << i << ": " <<  islands1[i]->GetTimeStamp() * islands1[i]->GetClockTickInNs() << "\n";
    }
    for(unsigned int i = 0; i < islands2.size(); ++i){
      std::cout << "i = " << i << ": " <<  islands2[i]->GetTimeStamp() * islands2[i]->GetClockTickInNs() << "\n";
    }
  }

  for(unsigned int i = 0; i < islands1.size(); ++i){
    double t1 = islands1[i]->GetTimeStamp() * islands1[i]->GetClockTickInNs();
    bool foundCoincidence = false;
    for(unsigned int j = last; j < islands2.size(); ++j){
      double t2 = islands2[j]->GetTimeStamp() * islands2[j]->GetClockTickInNs();
      double tdiff = t2 - t1;

      if(tdiff < -200.){
	last = j;
	continue;
      }
      if(tdiff > 200.){
	break;
      }
      // Found a coincidence
      foundCoincidence = true;      
    }
    // Check if we found a coincidence
    if(!foundCoincidence){
      ++NumberOfNoCoincidences;
      printf("Found %d tree entries with no coincidence.\n", NumberOfNoCoincidences);
    }
    hNumberOfNoCoincidences->Fill(NumberOfNoCoincidences);
  }


  return 0;
}
