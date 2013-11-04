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
static double anchor_time_slow = 0;
static double anchor_amp_slow = 0;
static double anchor_time_fast = 0;
static double anchor_amp_fast = 0;

static TH1F* hPseudotime = NULL;
static TH1F* hTau = NULL;

static TH2F* hMVsPsi = NULL;
static TH1F* hAvgMVsPsi = NULL;

static TH1F* hTemplateSlow = NULL;

static const int n_fast_pulse_banks = 1;
static const int n_slow_pulse_banks = 1;
static string fast_pulse_banknames[n_fast_pulse_banks] = {"Ng80"};
static string slow_pulse_banknames[n_slow_pulse_banks] = {"Nh80"};

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
		hTemplateSlow = new TH1F("hTemplateSlow", "hTemplateSlow", h2DTemplateSlow->GetNbinsX(), h2DTemplateSlow->GetXaxis()->GetXmin(), h2DTemplateSlow->GetXaxis()->GetXmax());
		
		hTemplateSlow->SetTitle("Template for the Slow Pulses");
		hTemplateSlow->GetXaxis()->SetTitle("time [ns]");
		hTemplateSlow->GetYaxis()->SetTitle("ADC value");
	
		for (int iBin = 1; iBin <= h2DTemplateSlow->GetNbinsX(); iBin++) {
		
			double avg_ADC = 0.0;
			int n_entries = 0;
		
			for (int jBin = 1; jBin <= h2DTemplateSlow->GetNbinsY(); jBin++) {
			
				avg_ADC += h2DTemplateSlow->GetYaxis()->GetBinCenter(jBin) * h2DTemplateSlow->GetBinContent(iBin, jBin);
				n_entries += h2DTemplateSlow->GetBinContent(iBin, jBin);
		
			}
			avg_ADC /= n_entries;
			
			hTemplateSlow->SetBinContent(iBin, avg_ADC);
	
		}
	}
	
	// Now that we have all the pulses from all the entries, complete the fast pulse pseudotime template
	PseudotimeTemplate();
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
	  	
	  	// Compare the bank names against all the slow pulse banks
	  	for (int iBankName = 0; iBankName < n_slow_pulse_banks; iBankName++) {
	  		if (strcmp((*pulseIter)->GetBankName().c_str(), slow_pulse_banknames[iBankName].c_str()) == 0)
	  			GaussianFit(*pulseIter, pulseIter - pulses.begin());
	  	}
	  	
	  	for (int iBankName = 0; iBankName < n_fast_pulse_banks; iBankName++) {
	  		if (strcmp((*pulseIter)->GetBankName().c_str(), fast_pulse_banknames[iBankName].c_str()) == 0)
	  			PseudotimeDistributions(*pulseIter, pulseIter - pulses.begin());
	  	}
	  	
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
	
	// Normalise the data and plot it in the 2D histogram
	for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
		
		double ADC = (*sampleIter);
		double time = (sampleIter - theSamples.begin()) * pulse->GetClockTickInNs();
		double time_shift = 0;
		double amp_scale_factor = 0;
		
		if (anchor_time_slow == 0) {
	  		// Have this pulse as the anchor
	  		anchor_time_slow = t_max + delta_t; // anchor to the maximum value of the function
	  	}
	  	time_shift = t_max - anchor_time_slow;
	  	
	  	if (anchor_amp_slow == 0) {
	  		// Have this pulse as the anchor
	  		anchor_amp_slow = A;
	  	}
	  	amp_scale_factor = a_max / anchor_amp_slow;
		
		h2DTemplateSlow->Fill(time - time_shift, ADC * amp_scale_factor);
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

// Get the plots we need to fill on a pulse-by-pulse basis
void CreateTemplates::PseudotimeDistributions(TPulseIsland* pulse, int pulse_number) {
	
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
	
	// Calculate pseudotime and plot its distribution
	// Get the amplitudes of the samples before and after the maximum
	int a_p = theSamples[max_sample_position - 1]; // the amplitude of the previous sample
	int a_n = theSamples[max_sample_position + 1]; // the amplitude of the next sample
	    
	double del_p = std::abs(max_sample_value - a_p);
	double del_n = std::abs(max_sample_value - a_n);
	    
	double psi = std::atan2(del_p, del_n); // pseudotime
	
	hPseudotime->Fill(psi);
	
	
	// Plot the M vs Psi distribution
	if (hMVsPsi == NULL) { // create the histogram
		hMVsPsi = new TH2F("hMVsPsi", "M vs #psi", 100,0,TMath::PiOver2(), 1000,0,1000);
	}
	hMVsPsi->Fill(psi, max_sample_value);
}

void CreateTemplates::PseudotimeTemplate() {
	
	double fast_pulse_clocktick = 6.25;
	
	// Calculate the constant in the function tau(psi)
	// tau(psi) = constant * integral(0->psi)(f(psi))
	// for psi=pi/2, tau = ClockTickInNs() ==> constant = ClockTickInNs() / integral(0->pi/2)f(psi)
	
	double constant = fast_pulse_clocktick / hPseudotime->Integral("width");
	
	// Plot tau(psi)
	int n_bins = 100;
	hTau = new TH1F("hTau", "#tau(#psi)", n_bins,0,TMath::PiOver2());
	for (int iBin = 1; iBin <= hPseudotime->GetNbinsX(); iBin++) {
		hTau->SetBinContent(iBin, constant * hPseudotime->Integral(1, iBin, "width"));
	}
	
	// Plot the average value of M (max sample) as a function of psi
	hAvgMVsPsi = new TH1F("hAvgMVsPsi", "<M>(#psi)", n_bins,0,TMath::PiOver2());
	
	for (int iBin = 1; iBin <= hMVsPsi->GetNbinsX(); iBin++) {
		
		double avg_M = 0.0;
		int n_entries = 0;
		
		for (int jBin = 1; jBin <= hMVsPsi->GetNbinsY(); jBin++) {
			
			avg_M += hMVsPsi->GetYaxis()->GetBinCenter(jBin) * hMVsPsi->GetBinContent(iBin, jBin);
			n_entries += hMVsPsi->GetBinContent(iBin, jBin);
		
		}
		if (n_entries != 0) {
			avg_M /= n_entries;
			hAvgMVsPsi->SetBinContent(iBin, avg_M);
		}
	}
	
	// Get R(psi) by assuming R_max = 1 and using <M>(psi) = k*R(psi)
	// Get the maximum <M> which is the same as k
	double k = hAvgMVsPsi->GetMaximum();
	
	// No we have everything, so loop through all the fast pulses again to get the true amplitude and delta_t

	for (int iEntry = 1; iEntry <= entry_counter; iEntry++) {
		std::stringstream histname;
		histname << "Entry" << iEntry << "_" << fast_pulse_banknames[0] << "_Pulse0";
		TH1F* hFastPulse = (TH1F*) dir->Get(histname.str().c_str());
		
		if (hFastPulse == NULL)
			continue; // skip the loop if there's not a fast pulse in this entry
			
		// Calculate psi for this pulse
		double a_max = hFastPulse->GetMaximum();
		double a_max_bin = hFastPulse->GetMaximumBin();
		double t_max = a_max_bin * fast_pulse_clocktick;
		
		int a_p = hFastPulse->GetBinContent(a_max_bin - 1); // the amplitude of the previous sample
		int a_n = hFastPulse->GetBinContent(a_max_bin + 1); // the amplitude of the next sample
		    
		double del_p = std::abs(a_max - a_p);
		double del_n = std::abs(a_max - a_n);
	    
		double psi = std::atan2(del_p, del_n); // pseudotime
		
		
		// Calculate R(psi)
		double R = hAvgMVsPsi->GetBinContent(hAvgMVsPsi->FindBin(psi)) / k;
		
		// Calculate tau(psi)
		double tau = hTau->GetBinContent(hTau->FindBin(psi));
		
		// Finally get A and delta_t
		double A = a_max / R;
		double delta_t = -0.5 + tau;
		
		// Create the histogram if it's not been created
		if (h2DTemplateFast == NULL) {
			h2DTemplateFast = new TH2F("h2DTemplateFast", "h2DTemplateFast", 
	  					hFastPulse->GetNbinsX(),hFastPulse->GetXaxis()->GetXmin(),hFastPulse->GetXaxis()->GetXmax(), 
	  					hFastPulse->GetMaximum()+500,hFastPulse->GetMinimum(),hFastPulse->GetMaximum()+500);
	  	
	  		h2DTemplateFast->SetTitle("Fast Pulse 2D Template Histogram");
	  		h2DTemplateFast->GetXaxis()->SetTitle("time [ns]");
	  		h2DTemplateFast->GetYaxis()->SetTitle("ADC value");
		}
	
		// Normalise the data and plot it in the 2D histogram
		for (int iBin = 1; iBin <= hFastPulse->GetNbinsX(); iBin++) {
		
			double ADC = hFastPulse->GetBinContent(iBin);
			double time = iBin * fast_pulse_clocktick;
			double time_shift = 0;
			double amp_scale_factor = 0;
		
			if (anchor_time_fast == 0) {
	  			// Have this pulse as the anchor
	  			anchor_time_fast = t_max + delta_t; // anchor to the maximum value of the function
	  		}
	  		time_shift = t_max - anchor_time_fast;
	  	
	  		if (anchor_amp_fast == 0) {
	  			// Have this pulse as the anchor
	  			anchor_amp_fast = A;
	  		}
	  		amp_scale_factor = a_max / anchor_amp_fast;
		
			h2DTemplateFast->Fill(time - time_shift, ADC * amp_scale_factor);
		}
		
		if (verbose) {
			std::cout << "Fast Pulse: " << std::endl;
			std::cout << "a_max = " << a_max << "\tt_max = " << t_max << std::endl;
			std::cout << "psi = " << psi << "\tR = " << R << "\ttau = " << tau << std::endl;
			std::cout << "A = " << A << "\tdelta_t = " << delta_t << std::endl << std::endl;
		}
	}
}
