// PlotSamples.C
// -- A ROOT macro that plots the samples in each TPulseIsland

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"

#include "../../src/common/TPulseIsland.h"

void PlotSamples(std::string filename) {
  
  //  gSystem->Load("libDataClasses.so");
  
  TFile* file = new TFile(filename.c_str());
  TTree* tree = (TTree*) file->Get("EventTree");
  TFile* out_file = new TFile("output.root", "RECREATE");

  map<string, vector<TPulseIsland*> > theMap;

  tree->SetMakeClass(1); // needed this to get theMap out for some reason
  tree->SetBranchAddress("fPulseIslandToChannelMap", &theMap);

  std::vector<TH1F*> theHistograms;

  // Loop through the entries in the tree
  for (int iEntry = 0; iEntry < tree->GetEntries(); iEntry++) {
    
    tree->GetEntry(iEntry);

    // Loop through the banks
    for (map<string, vector<TPulseIsland*> >::iterator mapIter = theMap.begin();
	 mapIter != theMap.end();
	 mapIter++) {
      
      vector<TPulseIsland*> theVector = theMap[mapIter->first];
      if (!theVector.empty()) {

	// Loop through the TPulseIslands
	for (vector<TPulseIsland*>::iterator islandIter = theVector.begin();
	     islandIter != theVector.end();
	     islandIter++) {

	  std::stringstream histname;
	  histname << "Entry" << iEntry << "_" << (*islandIter)->GetBankName() << "_Island" << islandIter - theVector.begin();
	  TH1F* hist1 = new TH1F(histname.str().c_str(), histname.str().c_str(), 100,0,100);
		  
	  std::vector<int> theSamples = (*islandIter)->GetSamples();

	  // Loop through the samples
	  int sampleCounter = 0;
	  for (std::vector<int>::iterator sampleIter = theSamples.begin();
	       sampleIter != theSamples.end();
	       sampleIter++) {
	    
	    hist1->Fill(sampleCounter, *sampleIter);
	    sampleCounter++;
	  } // end for sampleIter

	  theHistograms.push_back(hist1);
	} // end for islandIter
      } // end if theVector.empty() 
    } // end for mapIter
  } // end for iEntry

  for (std::vector<TH1F*>::iterator iter = theHistograms.begin();
       iter != theHistograms.end();
       iter++) {
    (*iter)->Write();
  }

  out_file->Close();
}
