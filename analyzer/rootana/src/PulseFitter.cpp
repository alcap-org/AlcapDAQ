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
#include "TH2.h"

#include "TF1.h"
#include "TMath.h"

#include "TROOT.h"
#include "TStyle.h"

#include "TCanvas.h"
#include "TLine.h"
#include "TLegend.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

static bool verbose = false;

static int entry_counter = 1;

static TH2F* h2DTemplate = NULL;
static TH1F* hTemplate = NULL;
static double anchor_time = 0;

PulseFitter::PulseFitter(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){
  
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders
  gStyle->SetPalette(1);
  
  dir->cd("/");
}

PulseFitter::~PulseFitter(){

	dir->cd(); // cd into the module's directory
	
	// Now get the template out
	// Want <ADC>(t)
	hTemplate = new TH1F("hTemplate", "hTemplate", h2DTemplate->GetXaxis()->GetXmax(), h2DTemplate->GetXaxis()->GetXmin(), h2DTemplate->GetXaxis()->GetXmax());
	//std::cout << "Template: " << std::endl;
	
	for (int iBin = 1; iBin <= h2DTemplate->GetNbinsX(); iBin++) {
		
		double avg_ADC = 0.0;
		int n_entries = 0;
		
		for (int jBin = 1; jBin <= h2DTemplate->GetNbinsY(); jBin++) {
			
/*			if (h2DTemplate->GetBinContent(iBin, jBin) != 0) {
			std::cout << "iBin = " << iBin << ", jBin = " << jBin << std::endl;
			std::cout << "GetBin(iBin, jBin) = " << h2DTemplate->GetBin(iBin, jBin) << std::endl;
			std::cout << "GetBinCenter(y-axis) = " << h2DTemplate->GetYaxis()->GetBinCenter(jBin) << std::endl;
			std::cout << "GetBinContent = " << h2DTemplate->GetBinContent(iBin, jBin) << std::endl;
			}
*/		
			avg_ADC += h2DTemplate->GetYaxis()->GetBinCenter(jBin) * h2DTemplate->GetBinContent(iBin, jBin);
			n_entries += h2DTemplate->GetBinContent(iBin, jBin);
		
		}
		avg_ADC /= n_entries;
		
		hTemplate->Fill(iBin, avg_ADC);
	
	}
	std::cout << std::endl;

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
	  	
	  	if (strcmp((*pulseIter)->GetBankName().c_str(), "Nh80") == 0)
	  		GaussianFit(*pulseIter, pulseIter - pulses.begin());
	  	
	  }
  }
  entry_counter++;
  
  return 0;
}

void PulseFitter::GaussianFit(TPulseIsland* pulse, int pulse_number) {
	
	// Get the samples
	std::vector<int> theSamples = pulse->GetSamples();
	  	
	// Create the histogram
	std::stringstream histname;
	histname << "Entry" << entry_counter << "_" << pulse->GetBankName() << "_Pulse" << pulse_number;
	  	
	int bin_min = 0; int bin_max = theSamples.size() * pulse->GetClockTickInNs(); int n_bins = theSamples.size();
	TH1F* hPulse = new TH1F(histname.str().c_str(), histname.str().c_str(), n_bins, bin_min, bin_max);
	  	
	hPulse->GetXaxis()->SetTitle("time [ns]");
	hPulse->GetYaxis()->SetTitle("ADC Value");
	  	
	// Fill the histogram
	for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
		hPulse->Fill( (sampleIter - theSamples.begin()) * pulse->GetClockTickInNs(), *sampleIter);
	}
	  	
	std::stringstream canvasname;
	canvasname << "Canvas_" << histname.str();
	TCanvas* c1 = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());
	  	
	TF1* gaussian = new TF1("gaus", "[0]*TMath::Gaus(x, [1], [2], 0)", hPulse->GetXaxis()->GetXmin()+500,hPulse->GetXaxis()->GetXmax()-500);
	gaussian->SetParameter(0, 250);
	gaussian->SetParameter(1, 1500);
	gaussian->SetParameter(2, 1500);
	  	
	gaussian->SetLineColor(kRed);
	gaussian->SetLineWidth(2);
	hPulse->Fit("gaus", "QR");
	  		  	
	  	
	// Create the histogram if it's not been created
	if (h2DTemplate == NULL) {
		h2DTemplate = new TH2F("h2DTemplate", "h2DTemplate", 
	  				hPulse->GetXaxis()->GetXmax(),hPulse->GetXaxis()->GetXmin(),hPulse->GetXaxis()->GetXmax(), 
	  				hPulse->GetMaximum()+500,hPulse->GetMinimum(),hPulse->GetMaximum()+500);
	}
	// Add the fit to the template
	for (int iBin = 1; iBin <= h2DTemplate->GetNbinsX(); iBin++) {
	  		
		double time_shift = 0;
	  	// Get the time shift
	  	if (anchor_time == 0) {
	  		// Have this pulse as the anchor
	  		anchor_time = gaussian->GetMaximumX(); // anchor to the maximum value of the function
	  	}
	  	else {
	  		double this_time = gaussian->GetMaximumX();
	  		time_shift = this_time - anchor_time;
	  	}
	  		
	  	h2DTemplate->Fill(iBin - time_shift, gaussian->Eval(iBin));
	  	
	}
	
	
	/*	  	TF1* skew_gaussian = new TF1("skew-gaus", "2*[0]*TMath::Gaus(x, [1], [2], 0)*(0.5*(1 + TMath::Erf( ([3]*x)/sqrt(2) )))", hPulse->GetXaxis()->GetXmin(),hPulse->GetXaxis()->GetXmax());
	  	skew_gaussian->SetParameter(0, 250);
	  	skew_gaussian->SetParameter(1, 1500);
	  	skew_gaussian->SetParameter(2, 1500);
	  	skew_gaussian->SetParameter(3, 0);
	  	
	  	skew_gaussian->SetLineWidth(2);
	  	skew_gaussian->SetLineColor(kBlue);
	  	hPulse->Fit("skew-gaus", "Q");
	  	
	  	hPulse->Draw();
	  	skew_gaussian->Draw("SAMES");
	  	gaussian->Draw("SAMES");
	  	myfunc->Draw("SAMES");
	  	
	  	TLegend* legend = new TLegend(0.5, 0.85, 0.7, 0.75, "");
	  	legend->SetBorderSize(0);
        legend->SetTextSize(0.04);
        legend->SetFillColor(kWhite);
        legend->AddEntry(gaussian, "Gaussian", "l");
        legend->AddEntry(skew_gaussian, "Skewed Gaussian", "l");
        legend->AddEntry(myfunc, "Convoluted Landau & Gaussian", "l");
        legend->Draw();
        
	  	c1->Update();
	  	c1->Write();
	  	
//	  	std::cout << "Difference in parameters:\tdelta_p0 = " << skew_gaussian->GetParameter(0) - gaussian->GetParameter(0)
//	  				<< "\tdelta_p1 = " << skew_gaussian->GetParameter(1) - gaussian->GetParameter(1)
//	  				<< "\tdelta_p2 = " << skew_gaussian->GetParameter(2) - gaussian->GetParameter(2)
//	  				<< "\tdelta_p3 = " << skew_gaussian->GetParameter(3) - 0 << std::endl;

	  	*/
}
