// CreateTemplates.cpp
// Author: Andrew Edmonds
// Created: 25th Oct 2013
// -- Pulse fitter module

#include "CreateTemplates.h"
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

static TH2F* h2DTemplateSlow = NULL;
static TH2F* h2DTemplateFast = NULL;
static double anchor_time = 0;
static double anchor_amp = 0;

static TH1F* hPseudotime = NULL;
static TH2F* hMVsPsi = NULL;

static TH1F* hTemplateSlow = NULL;

CreateTemplates::CreateTemplates(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){
  
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders
  gStyle->SetPalette(1);
  
  dir->cd("/");
}

CreateTemplates::~CreateTemplates(){

	dir->cd(); // cd into the module's directory
	
	// Now get the template out
	// Want <ADC>(t)
	if (h2DTemplateSlow != NULL) {
		hTemplateSlow = new TH1F("hTemplateSlow", "hTemplateSlow", h2DTemplateSlow->GetXaxis()->GetXmax(), h2DTemplateSlow->GetXaxis()->GetXmin(), h2DTemplateSlow->GetXaxis()->GetXmax());
		//std::cout << "Template: " << std::endl;
	
		for (int iBin = 1; iBin <= h2DTemplateSlow->GetNbinsX(); iBin++) {
		
			double avg_ADC = 0.0;
			int n_entries = 0;
		
			for (int jBin = 1; jBin <= h2DTemplateSlow->GetNbinsY(); jBin++) {
			
				avg_ADC += h2DTemplateSlow->GetYaxis()->GetBinCenter(jBin) * h2DTemplateSlow->GetBinContent(iBin, jBin);
				n_entries += h2DTemplateSlow->GetBinContent(iBin, jBin);
		
			}
			avg_ADC /= n_entries;
			
			hTemplateSlow->Fill(iBin, avg_ADC);
	
		}
		std::cout << std::endl;
	}
}

int CreateTemplates::ProcessEntry(TGlobalData *gData){
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
	  	
	  	if (strcmp((*pulseIter)->GetBankName().c_str(), "Ng80") == 0)
	  		PseudotimeFit(*pulseIter, pulseIter - pulses.begin());
	  	
	  }
  }
  entry_counter++;
  
  return 0;
}

void CreateTemplates::GaussianFit(TPulseIsland* pulse, int pulse_number) {
	
	// Get the samples
	std::vector<int> theSamples = pulse->GetSamples();
	  	
	// Create the histogram
	std::stringstream histname;
	histname << "Entry" << entry_counter << "_" << pulse->GetBankName() << "_Pulse" << pulse_number;
	  	
	int bin_min = 0; int bin_max = theSamples.size() * pulse->GetClockTickInNs(); int n_bins = theSamples.size();
	TH1F* hPulse = new TH1F(histname.str().c_str(), histname.str().c_str(), n_bins, bin_min, bin_max);
	  	
	hPulse->GetXaxis()->SetTitle("time [ns]");
	hPulse->GetYaxis()->SetTitle("ADC Value");
	  	
	// Fill the histogram and record a_max and t_max
	double a_max = 0;
	double t_max = 0;
	for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
	
		if ( (*sampleIter) > a_max) {
			a_max = (*sampleIter);
			t_max = (sampleIter - theSamples.begin()) * pulse->GetClockTickInNs();
		}
		
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
	if (h2DTemplateSlow == NULL) {
		h2DTemplateSlow = new TH2F("h2DTemplateSlow", "h2DTemplateSlow", 
	  				hPulse->GetNbinsX(),hPulse->GetXaxis()->GetXmin(),hPulse->GetXaxis()->GetXmax(), 
	  				hPulse->GetMaximum()+500,hPulse->GetMinimum(),hPulse->GetMaximum()+500);
	  	
	  	h2DTemplateSlow->SetTitle("Slow Pulse 2D Template Histogram");
	  	h2DTemplateSlow->GetXaxis()->SetTitle("time [ns]");
	  	h2DTemplateSlow->GetYaxis()->SetTitle("ADC value");
	}
	// Get the amplitude and time shift values from the fit
	double A = gaussian->GetMaximum();
	double delta_t = t_max - gaussian->GetMaximumX();
	
	// Normalise the data to A = 1, delta_t = 0
	for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
		
		double ADC = (*sampleIter);
		double time = (sampleIter - theSamples.begin()) * pulse->GetClockTickInNs();
		double time_shift = 0;
		double amp_scale_factor = 0;
		
		if (anchor_time == 0) {
	  		// Have this pulse as the anchor
	  		anchor_time = t_max + delta_t; // anchor to the maximum value of the function
	  	}
	  	time_shift = t_max - anchor_time;
	  	
	  	if (anchor_amp == 0) {
	  		// Have this pulse as the anchor
	  		anchor_amp = A;
	  	}
	  	amp_scale_factor = a_max / anchor_amp;
		
		h2DTemplateSlow->Fill(time - time_shift, ADC * amp_scale_factor);
	}
	// Add the fit to the template
/*	for (int iBin = 1; iBin <= h2DTemplateSlow->GetNbinsX(); iBin++) {
	  		
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
	  		
	  	h2DTemplateSlow->Fill(iBin - time_shift, gaussian->Eval(iBin));
	  	
	}
*/	
	
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


void CreateTemplates::PseudotimeFit(TPulseIsland* pulse, int pulse_number) {
	
	// Get the samples
	std::vector<int> theSamples = pulse->GetSamples();
	  	
	// Create the histogram
	std::stringstream histname;
	histname << "Entry" << entry_counter << "_" << pulse->GetBankName() << "_Pulse" << pulse_number;
	  	
	int bin_min = 0; int bin_max = theSamples.size() * pulse->GetClockTickInNs(); int n_bins = theSamples.size();
	TH1F* hPulse = new TH1F(histname.str().c_str(), histname.str().c_str(), n_bins, bin_min, bin_max);
	  	
	hPulse->GetXaxis()->SetTitle("time [ns]");
	hPulse->GetYaxis()->SetTitle("ADC Value");
	  	
	// Fill the histogram and remember the maximum sample
	int max_sample_value = 0;
	int max_sample_position = 0;
	
	for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
	
		if (*(sampleIter) > max_sample_value) {
	    	max_sample_value = *(sampleIter);
	    	max_sample_position = sampleIter - theSamples.begin();
	    }
	    
		hPulse->Fill( (sampleIter - theSamples.begin()) * pulse->GetClockTickInNs(), *sampleIter);
	}
	  	
	// Get the pseudotime distribution
	if (hPseudotime == NULL) { // create the histogram
		hPseudotime = new TH1F("hPseudotime", "hPseudotime", 100,0,TMath::PiOver2());
	}
	
	// Calculate pseudotime
	// Get the amplitudes of the samples before and after the maximum
	int a_p = theSamples[max_sample_position - 1]; // the amplitude of the previous sample
	int a_n = theSamples[max_sample_position + 1]; // the amplitude of the next sample
	    
	double del_p = std::abs(max_sample_value - a_p);
	double del_n = std::abs(max_sample_value - a_n);
	    
	double psi = std::atan2(del_p, del_n); // pseudotime
	
	hPseudotime->Fill(psi);
	
	// Calculate the constant in the function tau(psi)
	// tau(psi) = constant * integral(0->psi)(f(psi))
	// for psi=pi/2, tau = ClockTickInNs() ==> constant = ClockTickInNs() / integral(0->pi/2)f(psi)
	
	double constant = pulse->GetClockTickInNs() / hPseudotime->Integral("width");
	std::cout << pulse->GetClockTickInNs() << ", " << hPseudotime->Integral("width") << std::endl;
	std::cout << "Constant: " << constant << std::endl;
	
	
	// Get the pseudotime distribution
	if (hMVsPsi == NULL) { // create the histogram
		hMVsPsi = new TH2F("hMVsPsi", "hMVsPsi", 100,0,TMath::PiOver2(), 1000,0,1000);
	}
	hMVsPsi->Fill(psi, max_sample_value);
	  		  	
	  	
	// Create the histogram if it's not been created
/*	if (h2DTemplateFast == NULL) {
		h2DTemplateFast = new TH2F("h2DTemplateFast", "h2DTemplateFast", 
	  				hPulse->GetXaxis()->GetXmax(),hPulse->GetXaxis()->GetXmin(),hPulse->GetXaxis()->GetXmax(), 
	  				hPulse->GetMaximum()+500,hPulse->GetMinimum(),hPulse->GetMaximum()+500);
	}
	// Add the fit to the template
	for (int iBin = 1; iBin <= h2DTemplateFast->GetNbinsX(); iBin++) {
	  		
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
	  		
	  	h2DTemplateFast->Fill(iBin - time_shift, gaussian->Eval(iBin));	  	
	}
*/
}

