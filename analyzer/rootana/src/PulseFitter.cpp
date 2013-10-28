// PulseFitter.cpp
// Author: Andrew Edmonds
// Created: 25th Oct 2013
// -- Pulse fitter module

#include "PulseFitter.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>

#include <sstream>
#include <cmath>

#include "TH1.h"

#include "TF1.h"

#include "TCanvas.h"
#include "TLine.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

static bool verbose = false;

static int entry_counter = 1;

PulseFitter::PulseFitter(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){
  dir->cd("/");
}

PulseFitter::~PulseFitter(){
}

int PulseFitter::ProcessEntry(TGlobalData *gData){
  typedef map<string, vector<TPulseIsland*> > TStringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;
  typedef vector<TPulseIsland*>::iterator pulse_iterator;
  typedef vector<int>::iterator int_iterator;

  vector<TPulseIsland*> pulses;
  
  for(map_iterator iter = gData->fPulseIslandToChannelMap.begin();
      iter != gData->fPulseIslandToChannelMap.end(); iter++) {
    	
      if (strcmp((iter->first).data(), "Pulses") == 0)
      	pulses = iter->second;
	  
	  for (pulse_iterator pulseIter = pulses.begin(); pulseIter != pulses.end(); pulseIter++) {
	  	
	  	if (verbose)
	  		std::cout << "Entry: " << entry_counter << " Bank: " << (*pulseIter)->GetBankName() << ", Pulse: " << pulseIter - pulses.begin() << std::endl;
	  	
	  	if (strcmp((*pulseIter)->GetBankName().c_str(), "Nh80") != 0)
	  		continue;
	  		
	  	// Get the samples
	  	std::vector<int> theSamples = (*pulseIter)->GetSamples();
	  	
	  	// Create the histogram
	  	std::stringstream histname;
	  	histname << "Entry" << entry_counter << "_" << (*pulseIter)->GetBankName() << "_Pulse" << pulseIter - pulses.begin();
	  	
	  	int bin_min = 0; int bin_max = theSamples.size(); int n_bins = bin_max;
	  	TH1F* hPulse = new TH1F(histname.str().c_str(), histname.str().c_str(), n_bins, bin_min, bin_max);
	  	
	  	// Fill the histogram
	  	for (int_iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
	  		hPulse->Fill(sampleIter - theSamples.begin(), *sampleIter);
	  	}
	  	
	  	TF1* gaussian = new TF1("gaus", "[0]*TMath::Gaus(x, [1], [2], 0)", hPulse->GetXaxis()->GetXmin(),hPulse->GetXaxis()->GetXmax());
	  	gaussian->SetParameter(0, 250);
	  	gaussian->SetParameter(1, 20);
	  	gaussian->SetParameter(2, 20);
	  	hPulse->Fit("gaus");
	  }
  }
  entry_counter++;
  
  return 0;
}
