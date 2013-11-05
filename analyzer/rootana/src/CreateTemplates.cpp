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

static const int n_slow_pulse_banks = 1;
static const int n_fast_pulse_banks = 1;
static string slow_pulse_banknames[n_slow_pulse_banks] = {"Nh80"};
static string fast_pulse_banknames[n_fast_pulse_banks] = {"Ng80"};

static TH2F* h2DTemplateSlow[n_slow_pulse_banks] = {NULL};
static TH2F* h2DTemplateFast[n_fast_pulse_banks] = {NULL};

static double anchor_time_slow = 0;
static double anchor_amp_slow = 0;
static double anchor_time_fast = 0;
static double anchor_amp_fast = 0;

static TH1F* hPseudotime[n_fast_pulse_banks] = {NULL};
static TH1F* hTau[n_fast_pulse_banks] = {NULL};

static TH2F* hAMaxVsPsi[n_fast_pulse_banks] = {NULL};
static TH1F* hAvgAMaxVsPsi[n_fast_pulse_banks] = {NULL};

static TH1F* hTemplateSlow[n_slow_pulse_banks] = {NULL};
static TH1F* hTemplateFast[n_fast_pulse_banks] = {NULL};

CreateTemplates::CreateTemplates(char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){
  
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders
  gStyle->SetPalette(1);
  
  dir->cd("/");
}

CreateTemplates::~CreateTemplates(){

	dir->cd(); // cd into the module's directory
	
	// Loop through the slow pulse banks and get the final pulse template
	for (int iBank = 0; iBank < n_slow_pulse_banks; iBank++) {
		
		std::stringstream histname;
		histname << "hTemplateSlow_" << slow_pulse_banknames[iBank];
		hTemplateSlow[iBank] = new TH1F(histname.str().c_str(), histname.str().c_str(), h2DTemplateSlow[iBank]->GetNbinsX(), h2DTemplateSlow[iBank]->GetXaxis()->GetXmin(), h2DTemplateSlow[iBank]->GetXaxis()->GetXmax());
		
		std::stringstream histtitle;
	  	histtitle << "Template for the Slow Pulses from bank " << slow_pulse_banknames[iBank];
		hTemplateSlow[iBank]->SetTitle(histtitle.str().c_str());
		hTemplateSlow[iBank]->GetXaxis()->SetTitle("time [ns]");
		hTemplateSlow[iBank]->GetYaxis()->SetTitle("ADC value");
		
		Plot1DTemplateHistogram(h2DTemplateSlow[iBank], hTemplateSlow[iBank]);
	}
		
	
	// For fast pulse banks:
	// Loop through the fast pulse banks and calculate the final distributions and constants we need
	for (int iBank = 0; iBank < n_fast_pulse_banks; iBank++) {
	
		// Calculate the constant to get tau(psi)
		double constant = CalculatePseudotimeConstant(hPseudotime[iBank], 6.25); // TODO: find a way to read 6.25 from somewhere
		
		// Plot the tau distribution
		std::stringstream histname;
		histname << "hTau_" << fast_pulse_banknames[iBank];
		hTau[iBank] = new TH1F(histname.str().c_str(), histname.str().c_str(), 100,0,TMath::PiOver2());
		histname.str("");
		
		std::stringstream histtitle;
	  	histtitle << "Tau disttributions as a function of #psi for bank " << fast_pulse_banknames[iBank];
	  	hTau[iBank]->SetTitle(histtitle.str().c_str());
		hTau[iBank]->GetXaxis()->SetTitle("#psi");
		hTau[iBank]->GetYaxis()->SetTitle("#tau(#psi)");
		histtitle.str("");
	  	
		PlotTauDistribution(hTau[iBank], hPseudotime[iBank], constant);
		
		
		// Plot the average a_max distribution
		histname << "hAvgMVsPsi_" << fast_pulse_banknames[iBank];
		hAvgAMaxVsPsi[iBank] = new TH1F(histname.str().c_str(), histname.str().c_str(), 100,0,TMath::PiOver2());
		histname.str("");
		
	  	histtitle << "The average a_{max} distribution for bank " << fast_pulse_banknames[iBank];
		hAvgAMaxVsPsi[iBank]->SetTitle(histtitle.str().c_str());
		hAvgAMaxVsPsi[iBank]->GetXaxis()->SetTitle("#psi");
		hAvgAMaxVsPsi[iBank]->GetYaxis()->SetTitle("<a>_{max}");
		histtitle.str("");
		
		PlotAverageAMaxDistribution(hAMaxVsPsi[iBank], hAvgAMaxVsPsi[iBank]);
		
		// Calculate the constant, k for R(psi)
		double k = CalculateRConstant(hAvgAMaxVsPsi[iBank]);
		
		
		// Loop through all the fast pulses again and plot the 2D template
		for (int iEntry = 1; iEntry <= entry_counter; iEntry++) {
			std::stringstream histname;
			histname << "Entry" << iEntry << "_" << fast_pulse_banknames[iBank] << "_Pulse0"; // TODO: Get correct pulse numbers
			TH1F* hFastPulse = (TH1F*) dir->Get(histname.str().c_str());
			histname.str("");
		
			if (hFastPulse == NULL)
				continue; // skip the loop if there's not a fast pulse in this entry
			
			double a_max, t_max, A, delta_t;
			GetFastPulsePseudotimeVariables(hFastPulse, a_max, t_max, A, delta_t, 6.25, iBank, constant, k);
			
			// Create the 2D template histogram if it's not been created yet
			if (h2DTemplateFast[iBank] == NULL) {
			
				histname << "h2DTemplateFast_" << fast_pulse_banknames[iBank];
				h2DTemplateFast[iBank] = new TH2F(histname.str().c_str(), histname.str().c_str(),
	  						hFastPulse->GetNbinsX(),hFastPulse->GetXaxis()->GetXmin(),hFastPulse->GetXaxis()->GetXmax(), 
	  						hFastPulse->GetMaximum()+500,hFastPulse->GetMinimum(),hFastPulse->GetMaximum()+500);
	  			
	  			std::stringstream histtitle;
	  			histtitle << "Fast Pulse 2D Template Histogram for bank " << fast_pulse_banknames[iBank];
	  			h2DTemplateFast[iBank]->SetTitle(histtitle.str().c_str());
	  			h2DTemplateFast[iBank]->GetXaxis()->SetTitle("time [ns]");
	  			h2DTemplateFast[iBank]->GetYaxis()->SetTitle("ADC value");
			}
			
			// Fill the 2D template histogram
			Fill2DTemplateHistogram(hFastPulse, h2DTemplateFast[iBank], a_max, t_max, A, delta_t);
		}
		
		// Now get the final template for the fast pulse
		histname << "hTemplateFast_" << fast_pulse_banknames[iBank];
		hTemplateFast[iBank] = new TH1F(histname.str().c_str(), histname.str().c_str(), h2DTemplateFast[iBank]->GetNbinsX(), h2DTemplateFast[iBank]->GetXaxis()->GetXmin(), h2DTemplateFast[iBank]->GetXaxis()->GetXmax());
		
	  	histtitle << "Template for the Fast Pulses from bank " << fast_pulse_banknames[iBank];
		hTemplateFast[iBank]->SetTitle(histtitle.str().c_str());
		hTemplateFast[iBank]->GetXaxis()->SetTitle("time [ns]");
		hTemplateFast[iBank]->GetYaxis()->SetTitle("ADC value");
		
		Plot1DTemplateHistogram(h2DTemplateFast[iBank], hTemplateFast[iBank]);
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
	  	
	  	// Compare the bank names against all the slow pulse banks
	  	for (int iBank = 0; iBank < n_slow_pulse_banks; iBank++) {
	  		if (strcmp((*pulseIter)->GetBankName().c_str(), slow_pulse_banknames[iBank].c_str()) == 0) {
	  		
	  			// Plot the pulse in a histogram and store the a_max and t_max values
	  			double a_max, t_max;
	  			TH1F* hPulse = PlotPulse(*pulseIter, pulseIter - pulses.begin(), a_max, t_max);
	  			
	  			// Fit a Gaussian to the histogram
	  			TF1* fit = FitGaussian(hPulse);
	  			
	  			// Get the true amplitude and delta_t values from the fit
				double A = fit->GetMaximum();
				double delta_t = t_max - fit->GetMaximumX();
				
				
				// Create the histogram if it's not been created
				if (h2DTemplateSlow[iBank] == NULL) {
					std::stringstream histname;
					histname << "h2DTemplateSlow_" << slow_pulse_banknames[iBank];
					h2DTemplateSlow[iBank] = new TH2F(histname.str().c_str(), histname.str().c_str(), 
	  											hPulse->GetNbinsX(),hPulse->GetXaxis()->GetXmin(),hPulse->GetXaxis()->GetXmax(), 
	  											hPulse->GetMaximum()+500,hPulse->GetMinimum(),hPulse->GetMaximum()+500);
	  				
	  				std::stringstream histtitle;
	  				histtitle << "Slow Pulse 2D Template Histogram for bank " << slow_pulse_banknames[iBank];
	  				h2DTemplateSlow[iBank]->SetTitle(histtitle.str().c_str());
	  				h2DTemplateSlow[iBank]->GetXaxis()->SetTitle("time [ns]");
	  				h2DTemplateSlow[iBank]->GetYaxis()->SetTitle("ADC value");
				}
				
				// Fill the 2D template histogram
				Fill2DTemplateHistogram(hPulse, h2DTemplateSlow[iBank], a_max, t_max, A, delta_t);
	  		}
	  	}
	  	
	  	for (int iBank = 0; iBank < n_fast_pulse_banks; iBank++) {
	  		if (strcmp((*pulseIter)->GetBankName().c_str(), fast_pulse_banknames[iBank].c_str()) == 0) {
	  		
	  			// Plot the pulse in a histogram and store the a_max and t_max values
	  			double a_max, t_max;
	  			TH1F* hPulse = PlotPulse(*pulseIter, pulseIter - pulses.begin(), a_max, t_max);
	  			
	  			// Create the pseudotime histogram if it doesn't currently exist
				if (hPseudotime[iBank] == NULL) { // create the histogram
					std::stringstream histname;
					histname << "hPseudotime_" << fast_pulse_banknames[iBank];
					hPseudotime[iBank] = new TH1F(histname.str().c_str(), histname.str().c_str(), 100,0,TMath::PiOver2());
					
					std::stringstream histtitle;
	  				histtitle << "Pseudotime Distribution for bank " << fast_pulse_banknames[iBank];
	  				hPseudotime[iBank]->SetTitle(histtitle.str().c_str());
	  				hPseudotime[iBank]->GetXaxis()->SetTitle("#psi");
	  				hPseudotime[iBank]->GetYaxis()->SetTitle("N");
				}
				
				// Create the a_max vs psi distribution if it doesn't currently exist
				if (hAMaxVsPsi[iBank] == NULL) { // create the histogram
					std::stringstream histname;
					histname << "hAMaxVsPsi_" << fast_pulse_banknames[iBank];
					hAMaxVsPsi[iBank] = new TH2F(histname.str().c_str(), histname.str().c_str(), 100,0,TMath::PiOver2(), 1000,0,1000);
					
					std::stringstream histtitle;
	  				histtitle << "Distribution of a_{max} vs #psi for bank " << fast_pulse_banknames[iBank];
	  				hAMaxVsPsi[iBank]->SetTitle(histtitle.str().c_str());
	  				hAMaxVsPsi[iBank]->GetXaxis()->SetTitle("#psi");
	  				hAMaxVsPsi[iBank]->GetYaxis()->SetTitle("a_{max}");
				}
				
				// Fill the psuedotime distribution, f(psi) and the max value vs pseudotime distribution, a_max(psi)
	  			FillPseudotimeDistributions(hPulse, hPseudotime[iBank], hAMaxVsPsi[iBank]);
	  		}
	  	}
	  	
	  }
  }
  entry_counter++;
  
  return 0;
}

// Plot the pulse in a histogram for later use (fitting, etc.) and record a_max and t_max
TH1F* CreateTemplates::PlotPulse(TPulseIsland* pulse, int pulse_number, double& a_max, double& t_max) {

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
	a_max = 0;
	t_max = 0;
	for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); sampleIter++) {
	
		if ( (*sampleIter) > a_max) {
			a_max = (*sampleIter);
			t_max = (sampleIter - theSamples.begin()) * pulse->GetClockTickInNs();
		}
		
		hPulse->Fill( (sampleIter - theSamples.begin()) * pulse->GetClockTickInNs(), *sampleIter);
	}
	
	return hPulse;
}

// Fit a Gaussian to the given histogram and return the function
TF1* CreateTemplates::FitGaussian(TH1F* hPulse) {
	
	// Create a Gaussian and fit it to the histogram given
	TF1* gaussian = new TF1("gaus", "[0]*TMath::Gaus(x, [1], [2], 0)", hPulse->GetXaxis()->GetXmin()+500,hPulse->GetXaxis()->GetXmax()-500);
	gaussian->SetParameter(0, 250);
	gaussian->SetParameter(1, 1500);
	gaussian->SetParameter(2, 1500);
	
	gaussian->SetLineColor(kRed);
	gaussian->SetLineWidth(2);
	hPulse->Fit("gaus", "QR");
	  		  	
	return gaussian;
}

// FIll the 2D template histogram
void CreateTemplates::Fill2DTemplateHistogram(TH1F* hPulse, TH2F* h2DTemplate, double a_max, double t_max, double A, double delta_t) {

	// Normalise the data and plot it in the 2D histogram
	for (int iBin = 1; iBin <= hPulse->GetNbinsX(); iBin++) {
	
		double ADC = hPulse->GetBinContent(iBin);
		double time = iBin * hPulse->GetBinWidth(1);
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
	
		//h2DTemplate->Fill(time - time_shift, ADC * amp_scale_factor);
		h2DTemplate->Fill(time, ADC);
	}
}

// Plot the 1D template histogram, <ADC>(t)
void CreateTemplates::Plot1DTemplateHistogram(TH2F* h2DTemplate, TH1F* h1DTemplate) {

	for (int iBin = 1; iBin <= h2DTemplate->GetNbinsX(); iBin++) {
		double avg_ADC = 0.0;
		int n_entries = 0;
		
		for (int jBin = 1; jBin <= h2DTemplate->GetNbinsY(); jBin++) {
			
			avg_ADC += h2DTemplate->GetYaxis()->GetBinCenter(jBin) * h2DTemplate->GetBinContent(iBin, jBin);
			n_entries += h2DTemplate->GetBinContent(iBin, jBin);
		
		}
		
		if (n_entries != 0) {
			avg_ADC /= n_entries;	
			h1DTemplate->SetBinContent(iBin, avg_ADC);
		}
	
	}
}

// Get the plots we need to fill on a pulse-by-pulse basis for the pseuodtime method
void CreateTemplates::FillPseudotimeDistributions(TH1F* hPulse, TH1F* hPseudotime, TH2F* hAMaxVsPsi) {
	
	// Get the amplitude and bin number of the maximum sample
	int a_max = hPulse->GetMaximum();
	int a_max_bin = hPulse->GetMaximumBin();
	
	// Get the amplitudes of the samples before and after the maximum
	int a_p = hPulse->GetBinContent(a_max_bin - 1); // the amplitude of the previous sample
	int a_n = hPulse->GetBinContent(a_max_bin + 1); // the amplitude of the next sample
	    
	double del_p = std::abs(a_max - a_p);
	double del_n = std::abs(a_max - a_n);
	
	// Calculate pseusotime
	double psi = std::atan2(del_p, del_n);
	
	// Fill the supplied histograms
	hPseudotime->Fill(psi);
	hAMaxVsPsi->Fill(psi, a_max);
}

// Calculate the constant in the function tau(psi)
// tau(psi) = constant * integral(0->psi)(f(psi))
// for psi=pi/2, tau = ClockTickInNs() ==> constant = ClockTickInNs() / integral(0->pi/2)f(psi)
double CreateTemplates::CalculatePseudotimeConstant(TH1F* hPseudotime, double clock_tick) {
	
	return clock_tick / hPseudotime->Integral("width");
}

void CreateTemplates::PlotTauDistribution(TH1F* hTau, TH1F* hPseudotime, double constant) {

	for (int iBin = 1; iBin <= hPseudotime->GetNbinsX(); iBin++) {
		hTau->SetBinContent(iBin, constant * hPseudotime->Integral(1, iBin, "width"));
	}
}

// Plot the average a_max distribution as a function of psi
void CreateTemplates::PlotAverageAMaxDistribution(TH2F* hAMaxDistribution, TH1F* hAverageAMaxDistribution) {

	for (int iBin = 1; iBin <= hAMaxDistribution->GetNbinsX(); iBin++) {
		
		double avg_a_max = 0.0;
		int n_entries = 0;
		
		for (int jBin = 1; jBin <= hAMaxDistribution->GetNbinsY(); jBin++) {
			
			avg_a_max += hAMaxDistribution->GetYaxis()->GetBinCenter(jBin) * hAMaxDistribution->GetBinContent(iBin, jBin);
			n_entries += hAMaxDistribution->GetBinContent(iBin, jBin);
		
		}
		if (n_entries != 0) {
			avg_a_max /= n_entries;
			hAverageAMaxDistribution->SetBinContent(iBin, avg_a_max);
		}
	}
}

// Get the constant, k for the function R(psi) 
// by assuming R_max = 1 and using <M>(psi) = k*R(psi)
double CreateTemplates::CalculateRConstant(TH1F* hAvgAMaxVsPsi) {

	// Get the maximum <M> which is the same as k
	return hAvgAMaxVsPsi->GetMaximum();
}

// Get the a_max, t_max, A and delta_t values for the fast pulse using the pseudotime calculation
void CreateTemplates::GetFastPulsePseudotimeVariables(TH1F* hFastPulse, double& a_max, double& t_max, double& A, double& delta_t, double clock_tick, int iBank, double constant, double k) {

	// Get the maximum samples and the time bin this is in
	a_max = hFastPulse->GetMaximum();
	int a_max_bin = hFastPulse->GetMaximumBin();
	t_max = a_max_bin * clock_tick;
	
	// Calculate psi for the pulse given
	int a_p = hFastPulse->GetBinContent(a_max_bin - 1); // the amplitude of the previous sample
	int a_n = hFastPulse->GetBinContent(a_max_bin + 1); // the amplitude of the next sample
		    
	double del_p = std::abs(a_max - a_p);
	double del_n = std::abs(a_max - a_n);
	    
	double psi = std::atan2(del_p, del_n);
		
		
	// Calculate R(psi)
	double R = hAvgAMaxVsPsi[iBank]->GetBinContent(hAvgAMaxVsPsi[iBank]->FindBin(psi)) / k;
		
	// Calculate tau(psi)
	double tau = hTau[iBank]->GetBinContent(hTau[iBank]->FindBin(psi));
		
	// Finally get A and delta_t
	A = a_max / R;
	delta_t = -0.5 + tau;
}
