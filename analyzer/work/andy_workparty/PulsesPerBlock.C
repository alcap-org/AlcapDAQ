// PulsesPerBlock.C
// -- A ROOT macro that plots the number of TPulseIsland* in each 100ms block

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>


#include "TFile.h"
#include "TTree.h"
#include "TH1.h"

#include "../../src/common/TPulseIsland.h"

void PulsesPerBlock(std::string filename) {
  
  TFile* file = new TFile(filename.c_str());
  TTree* tree = (TTree*) file->Get("EventTree");
  TFile* out_file = new TFile("PulsesPerBlock.root", "RECREATE");

  map<string, vector<TPulseIsland*> > theMap;

  tree->SetMakeClass(1); // needed this to get theMap out for some reason
  tree->SetBranchAddress("fPulseIslandToChannelMap", &theMap);

  // Vector of histograms that will be stored later
  std::vector<TH1F*> pulsesPerBlockHists;

  // Loop through the entries in the tree
  for (int iEntry = 0; iEntry < tree->GetEntries(); iEntry++) {
    tree->GetEntry(iEntry);

    int element = 0; // the element of the histogram vector that we would want
    // Loop through the banks
    for (map<string, vector<TPulseIsland*> >::iterator mapIter = theMap.begin();
	 mapIter != theMap.end();
	 mapIter++) {

      // If on the first entry, then create the histogram for each channel
      if (iEntry == 0) {
	std::stringstream histname;
	histname << "Number of pulses per block in channel " << mapIter->first;
	TH1F* hist1 = new TH1F(histname.str().c_str(), histname.str().c_str(), 100,0,100);
	hist1->GetXaxis()->SetTitle("Number of TPulseIsland*");
	hist1->GetYaxis()->SetTitle("Arbitrary Unit");
	pulsesPerBlockHists.push_back(hist1);
      }

      vector<TPulseIsland*> theVector = theMap[mapIter->first];
      if (!theVector.empty()) {
	std::cout << "Element = " << element << std::endl;
	pulsesPerBlockHists[element]->Fill(theVector.size()); // fill the histogram with the number of pulses in this block
      } // end if theVector.empty() 

      element++;
    } // end for mapIter
  } // end for iEntry

  for (std::vector<TH1F*>::iterator it = pulsesPerBlockHists.begin(); it != pulsesPerBlockHists.end(); it++) {
    (*it)->Write();
  }
  out_file->Close();
}
