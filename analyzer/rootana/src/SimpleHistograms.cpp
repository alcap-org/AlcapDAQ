#include "SimpleHistograms.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>

#include "TH2.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

static bool verbose = false;
static TH2 *hNumberOfNoCoincidences = 0;
static TH2 *hBankSize = 0;
static vector<string> banks;

SimpleHistograms::SimpleHistograms(const char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){

  // The following are the banks we want to compare in this example
  banks.push_back("Nec0");
  banks.push_back("Nfc0");
  banks.push_back("Ngc0");
  banks.push_back("Nhc0");
  banks.push_back("Nae0");
  banks.push_back("Nbe0");
  banks.push_back("Nce0");
  banks.push_back("Nee0");
//  banks.push_back("Nfe0");
//  banks.push_back("Nge0");
//  banks.push_back("Nhe0");
  
  hBankSize = new TH2F("hBankSize", "Bank size",
		       1500, -1.5, 1498.5, banks.size(), 0.5, banks.size()+0.5);

  for(unsigned int i = 1; i<=banks.size(); ++i) hBankSize->GetYaxis()->SetBinLabel(i, banks.at(i-1).data());

  // Let's compare the number of comparisons. If N is the size of the bank vector, then we need 
  // (N-1) + (N-2) +... + 1 comparisons
  int NrComparisons = banks.size() * (banks.size()-1) / 2; 
  
  hNumberOfNoCoincidences = new TH2F("hNumberOfNoCoincidences", "Number of missing coincidences per tree entry",
				     1000, -1.5, 998.5, NrComparisons, 0.5, NrComparisons+0.5);
}

SimpleHistograms::~SimpleHistograms(){
}

int SimpleHistograms::ProcessEntry(TGlobalData *gData){
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  vector<TPulseIsland*> islands[banks.size()];
  for(unsigned int b1 = 0; b1 < banks.size(); ++b1){
    string bank = banks.at(b1);
    for(map_iterator iter = gData->fPulseIslandToChannelMap.begin();
	iter != gData->fPulseIslandToChannelMap.end(); iter++){
      if(strcmp((iter->first).data(), bank.data()) == 0){
	islands[b1] = iter->second;
      }  
    }
    hBankSize->Fill(islands[b1].size(), b1+1);
  }

  vector<TPulseIsland*> islands1, islands2;

  int count  = 1;

  for(unsigned int b1 = 0; b1 < banks.size()-1; ++b1){
    for(unsigned int b2 = b1+1; b2 < banks.size(); ++b2){
      char title[100];
      sprintf(title, "%s - %s", banks.at(b1).data(), banks.at(b2).data());
      hNumberOfNoCoincidences->GetYaxis()->SetBinLabel(count, title);

      int last = 0;
      int NumberOfNoCoincidences = 0;
      
      if(verbose){
	for(unsigned int i = 0; i < islands[b1].size(); ++i){
	  std::cout << "i = " << i << ": " <<  islands[b1][i]->GetTimeStamp() * islands[b1][i]->GetClockTickInNs() << "\n";
	}
	for(unsigned int i = 0; i < islands[b2].size(); ++i){
	  std::cout << "i = " << i << ": " <<  islands[b2][i]->GetTimeStamp() * islands[b2][i]->GetClockTickInNs() << "\n";
	}
      }
      
      for(unsigned int i = 0; i < islands[b1].size(); ++i){
	double t1 = islands[b1][i]->GetTimeStamp(); // * islands1[i]->GetClockTickInNs();
	bool foundCoincidence = false;
	for(unsigned int j = last; j < islands[b2].size(); ++j){
	  double t2 = islands[b2][j]->GetTimeStamp(); // * islands2[j]->GetClockTickInNs();
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
	}
      }
      if(verbose) 
	printf("Found %d tree entries with no coincidence for banks %s and %s.\n", NumberOfNoCoincidences, 
	       banks.at(b1).data(), banks.at(b2).data());
      hNumberOfNoCoincidences->Fill(NumberOfNoCoincidences, count++);
    }
  }

  return 0;
}
