// Resolution.C
// -- A ROOT macro that gets the resolution from the fast and slow pulses
// -- Tested with run00363.mid

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TF1.h"

#include "../../src/common/TPulseIsland.h"

// Prototypes
double GetPulseHeight(TH1* pulse_hist);
double GetPulseIntegral(TH1* pulse_hist);
TH1* RemovePedestal(TH1* hist);
TH1* Calibrate(TH1* hist, double x, double new_x);

void Resolution(std::string filename) {
  
  TFile* file = new TFile(filename.c_str());
  TTree* tree = (TTree*) file->Get("EventTree");
  TFile* out_file = new TFile("output.root", "RECREATE");

  map<string, vector<TPulseIsland*> > theMap;

  tree->SetMakeClass(1); // needed this to get theMap out for some reason
  tree->SetBranchAddress("fPulseIslandToChannelMap", &theMap);

  // vectors to store the peak heights and the integrals
  std::vector<double> peakHeights;
  std::vector<double> integrals;

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
	  TH1F* hist1 = new TH1F(histname.str().c_str(), histname.str().c_str(), 100,0,200);
		  
	  std::vector<int> theSamples = (*islandIter)->GetSamples();

	  // Loop through the samples
	  int sampleCounter = 0;
	  for (std::vector<int>::iterator sampleIter = theSamples.begin();
	       sampleIter != theSamples.end();
	       sampleIter++) {
	    
	    hist1->Fill(sampleCounter, *sampleIter);
	    sampleCounter++;
	  } // end for sampleIter
	  hist1->GetXaxis()->SetTitle("sample number");
	  hist1->GetYaxis()->SetTitle("sample value");
	  hist1->Write();

	  // Now have the TPulseIsland* with x-axis = sample number and y-axis = sample value in a histogram

	  // Calibrate the histogram
	  // convert 1 tick --> clockTickInNs
	  TH1F* calib_hist = (TH1F*) Calibrate(hist1, 1, (*islandIter)->GetClockTickInNs());
	  calib_hist->Write();
	  hist1->GetXaxis()->SetTitle("time / ns");

	  // Now remove the pedestal
	  TH1F* pedSub = (TH1F*) RemovePedestal(calib_hist);
	  pedSub->Write();

	  // if the bank name is Nge0 then it's a slow pulse
	  if ( (*islandIter)->GetBankName() == "Ng80") {
	    peakHeights.push_back(GetPulseHeight(pedSub));
	  }
	  // else if the bank name is Nhe0 then it's a fast pulse
	  else if ( (*islandIter)->GetBankName() == "Nh80") {
	    integrals.push_back(GetPulseIntegral(pedSub));
	  }
	} // end for islandIter
      } // end if theVector.empty() 
    } // end for mapIter
  } // end for iEntry

  // Sort the vectors into ascending order to make it easier to decide where to put the histogram limits
  std::sort(peakHeights.begin(), peakHeights.end());
  std::sort(integrals.begin(), integrals.end());



  // Peak heights from the slow pulse
  int low_bin_val = *(peakHeights.begin());
  int high_bin_val = *(peakHeights.end() - 1);

  TH1F* peak_height_hist = new TH1F("peak_height_hist", "peak_height_hist", 50, low_bin_val, high_bin_val);

  for (std::vector<double>::iterator it = peakHeights.begin(); it != peakHeights.end(); it++) {
    peak_height_hist->Fill(*it);
  }

  peak_height_hist->GetXaxis()->SetTitle("FADC value");
  peak_height_hist->GetYaxis()->SetTitle("number of pulses");



  low_bin_val = *(integrals.begin());
  high_bin_val = *(integrals.end() - 1);
  TH1F* integral_hist = new TH1F("integral_hist", "integral_hist", 100, low_bin_val, high_bin_val);

  for (std::vector<double>::iterator it = integrals.begin(); it != integrals.end(); it++) {
    integral_hist->Fill(*it);
  }


  integral_hist->GetXaxis()->SetTitle("integral");//energy / MeV");
  integral_hist->GetYaxis()->SetTitle("number of pulses");

  peak_height_hist->Write();
  integral_hist->Write();


  // Calibrate the energy by taking the mean of the current histogram and scaling it to the pre-amp output
  double preAmpOutputInMeV = 3.8;
  double peak_height_mean = peak_height_hist->GetMean();

  TH1F* calib_peak_height_hist = (TH1F*) Calibrate(peak_height_mean, preAmpOutputInMeV);

  calib_peak_height_hist->GetXaxis()->SetTitle("energy / MeV");
  calib_peak_height_hist->GetYaxis()->SetTitle("number of pulses");
  calib_peak_height_hist->Write();


  double integral_mean = integral_hist->GetMean();
  TH1F* calib_integral_hist = (TH1F*) Calibrate(integral_hist, integral_mean, preAmpOutputInMeV);

  calib_integral_hist->GetXaxis()->SetTitle("energy / MeV");
  calib_integral_hist->GetYaxis()->SetTitle("number of pulses");
  calib_integral_hist->Write();

  out_file->Close();
}

