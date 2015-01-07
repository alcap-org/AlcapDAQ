// PlotTime.C
// -- A ROOT macro that plots the time of each pulse island

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"

#include "../../src/common/TPulseIsland.h"

void PlotTime(std::string filename) {
  
  TFile* file = new TFile(filename.c_str());
  TTree* tree = (TTree*) file->Get("EventTree");
  TFile* out_file = new TFile("PlotTime.root", "RECREATE");

  map<string, vector<TPulseIsland*> > theMap;

  tree->SetMakeClass(1); // needed this to get theMap out for some reason
  tree->SetBranchAddress("fPulseIslandToChannelMap", &theMap);

  // Loop through the entries in the tree
  for (int iEntry = 0; iEntry < tree->GetEntries(); iEntry++) {
    
    tree->GetEntry(iEntry);

    // Loop through the banks
    for (map<string, vector<TPulseIsland*> >::iterator mapIter = theMap.begin();
	 mapIter != theMap.end();
	 mapIter++) {
      
      std::stringstream histname;
      histname << "Block" << iEntry << "_" << mapIter->first;
      TH1F* hist1 = new TH1F(histname.str().c_str(), histname.str().c_str(), 100,0, 100);
      
      vector<TPulseIsland*> theVector = theMap[mapIter->first];
      if (!theVector.empty()) {

	// Loop through the TPulseIslands
	for (vector<TPulseIsland*>::iterator islandIter = theVector.begin();
	     islandIter != theVector.end();
	     islandIter++) {
	  //	  std::cout << (*islandIter)->GetClockTickInNs() << ", " << (*islandIter)->GetTimeStamp() << ", " << (*islandIter)->GetClockTickInNs() * (*islandIter)->GetTimeStamp() << std::endl;
	  hist1->Fill((*islandIter)->GetClockTickInNs() * (*islandIter)->GetTimeStamp() / 1E6);
	} // end for islandIter

	hist1->GetXaxis()->SetTitle("time / ms");
	hist1->GetYaxis()->SetTitle("Arbitrary Unit");
	hist1->Write();
      } // end if theVector.empty() 
    } // end for mapIter
  } // end for iEntry

  out_file->Close();
}
