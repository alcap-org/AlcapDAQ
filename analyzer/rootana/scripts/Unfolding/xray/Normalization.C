#include "XRay.h"

#include "TFile.h"
#include "TDirectoryFile.h"
#include "TH2.h"
#include "TF1.h"

#include "RooFit.h"
#include "RooAbsPdf.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"
#include "RooDataHist.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <map>

Bool_t debug = kTRUE;

//vectors to store the results of fitting and to plot the number of muons observed summary graph
std::vector<Double_t> vlResEnergy;
std::vector<Double_t> vlResNumber;
std::vector<Double_t> vlResError;
std::vector<Double_t> vhResEnergy;
std::vector<Double_t> vhResNumber;
std::vector<Double_t> vhResError;

int FillXRayInfo(XRay* xray);
RooRealVar* GetAreaUnderPeak(const char * descriptor, Double_t energy_low, Double_t energy_high, TH1* hSpectrum, XRay* xray, Double_t bkg_lower_offset=1, Double_t bkg_upper_offset=1, Double_t signal_sigma_upper_offset=10);

void CalculateNumberOfStoppedMuons(XRay *xray, RooRealVar *area, Double_t &n_stopped_muons, Double_t &n_stopped_muons_error) {
	// Factors to account for the detector effects
	std::vector<Double_t> detector_effects;
	//  detector_effects.push_back(1.04); detector_effects.push_back(1.01); // For R13 (dead time and TRP reset respectively)
	// TODO: Add detector effects for R15b
	n_stopped_muons = area->getValV() / (xray->intensity * xray->efficiency);
	for (std::vector<Double_t>::const_iterator i_factor = detector_effects.begin(); i_factor != detector_effects.end(); ++i_factor) {
		n_stopped_muons *= (*i_factor);
	}

	// ...and the uncertainty
	n_stopped_muons_error = n_stopped_muons* sqrt( (area->getError()/area->getValV())*(area->getError()/area->getValV()) +
			(xray->intensity_error/xray->intensity)*(xray->intensity_error/xray->intensity) +
			(xray->efficiency_error/xray->efficiency)*(xray->efficiency_error/xray->efficiency) );
}

// Takes a filename of a rootana output file as well as information on the timing cut and interesting x-ray
int CountXRays(TH1D *hEnergy, std::string channel = "GeLoGain",  std::string target_material="Al", std::string transition="2p-1s", Double_t window_low = 8, Double_t window_high = 8, Double_t lower_offset=1, Double_t upper_offset=1, int rebin_factor=1, Double_t signal_sigma_upper_offset=2) {
	// Apply the time cut and rebin
	hEnergy->Rebin(rebin_factor);

	// Now define the X-ray we want to look at
	XRay xray;
	xray.material = target_material;
	xray.transition = transition;
	if (FillXRayInfo(&xray) == 1) { // if we return an error code
		std::cout << "Error: Problem getting X-ray information" << std::endl;
		return 1;
	}

	// Here is the germanium effiency fit for R15b
	Double_t a, b, delta_a, delta_b, corr;
	if (channel == "GeHiGain") {
		a = 0.193; delta_a = 0.01635;
		b = -0.9437; delta_b = 0.01323;
		corr = -0.99486;
	}
	else if (channel == "GeLoGain") {
		a = 0.1792; delta_a = 0.01533;
		b = -0.9332; delta_b = 0.01332;
		corr = -0.99499;
	}
	else {
		std::cout << channel << " isn't a germanium channel. Aborting..." << std::endl;
		return 1;
	}
	TF1* ge_eff = new TF1("ge_eff", "[0]*(x^[1])");
	ge_eff->SetParameters(a, b);
	ge_eff->SetParError(0, delta_a);
	ge_eff->SetParError(1, delta_b);
	xray.efficiency = ge_eff->Eval(xray.energy);

	// Assuming uncertainty in the energy is small
	TF1* ge_eff_err = new TF1("ge_eff_err", "sqrt(x^(2*[1]) * ([2]^2 + TMath::Log(x)*[0]*[3]*([0]*[3]*TMath::Log(x) + 2*[4]*[2])))");
	ge_eff_err->SetParameters(a, b, delta_a, delta_b, corr);
	xray.efficiency_error = ge_eff_err->Eval(xray.energy);

	// Now get the area under the X-ray peak by doing a fit to the spectrum
	Double_t energy_low = xray.energy - window_low;
	Double_t energy_high = xray.energy + window_high;
	std::string descriptor = channel +  " " + target_material + " " + transition;
	RooRealVar* area = GetAreaUnderPeak(descriptor.c_str(), energy_low, energy_high, hEnergy, &xray, lower_offset, upper_offset, signal_sigma_upper_offset);
	//  std::cout << "Area under the curve = " << area->getValV() << " +- " << area->getError() << std::endl;

	// Factors to account for the detector effects
	std::vector<Double_t> detector_effects;
	//  detector_effects.push_back(1.04); detector_effects.push_back(1.01); // For R13 (dead time and TRP reset respectively)
	// TODO: Add detector effects for R15b

	// Now calculate the number of stopped muons
	Double_t n_stopped_muons = area->getValV() / (xray.intensity * xray.efficiency);
	for (std::vector<Double_t>::const_iterator i_factor = detector_effects.begin(); i_factor != detector_effects.end(); ++i_factor) {
		n_stopped_muons *= (*i_factor);
	}

	// ...and the uncertainty
	Double_t n_stopped_muons_error = n_stopped_muons* sqrt( (area->getError()/area->getValV())*(area->getError()/area->getValV()) +
			(xray.intensity_error/xray.intensity)*(xray.intensity_error/xray.intensity) +
			(xray.efficiency_error/xray.efficiency)*(xray.efficiency_error/xray.efficiency) );

	std::cout << "\033[1;31m" << "XRay: " << xray.material << " " << xray.transition << " " << xray.energy << " keV" << "\033[0m" << std::endl;
	std::cout << "\033[1;31m" << "Area = " << area->getValV() << " +/- " << area->getError() << " (" << (area->getError() / area->getValV()) * 100 << "%)" << "\033[0m" << std::endl;
	std::cout << "\033[1;31m" << "Intensity = " << xray.intensity << " +/- " << xray.intensity_error << " (" << (xray.intensity_error / xray.intensity) * 100 << "%)" << "\033[0m" << std::endl;
	std::cout << "\033[1;31m" << "Efficiency = " << xray.efficiency << " +/- " << xray.efficiency_error << " (" << (xray.efficiency_error / xray.efficiency) * 100 << "%)" << "\033[0m" << std::endl;
	std::cout << "\033[1;31m" << "Number of Stopped Muons = " << n_stopped_muons << " +- " << n_stopped_muons_error << " (" << (n_stopped_muons_error / n_stopped_muons) * 100 << "%)" << "\033[0m" << std::endl;

	if(xray.energy != 511 && xray.energy != 1460) {
		if(channel == "GeLoGain") {
			vlResEnergy.push_back(xray.energy);
			vlResNumber.push_back(n_stopped_muons);
			vlResError.push_back(n_stopped_muons_error);
		} else {
			vhResEnergy.push_back(xray.energy);
			vhResNumber.push_back(n_stopped_muons);
			vhResError.push_back(n_stopped_muons_error);
		}
	}
	return 0;
}

int FillXRayInfo(XRay* xray) {
	if (xray->material == "Al" && xray->transition == "2p-1s")  {
		std::cout << "Aluminium 2p-1s X-ray:" << std::endl;
		xray->energy = 346.828;
		xray->intensity = 0.798;
		xray->intensity_error = 0.008;
		return 0;
	}
	else if (xray->material == "Al" && xray->transition == "3p-1s") {
		std::cout << "Aluminium 3p-1s X-ray:" << std::endl;
		xray->energy = 412.87;
		xray->intensity = 0.0762;
		xray->intensity_error = 0.0015;
	} else {
		std::cout << "Error: Unknown target material" << std::endl;
		return 1;
	}
	return 0;
}

RooRealVar* GetAreaUnderPeak(const char * descriptor, Double_t energy_low, Double_t energy_high, TH1* hSpectrum, XRay* xray, Double_t bkg_lower_offset=1, Double_t bkg_upper_offset=1, Double_t signal_sigma_upper_offset=10) {
	Bool_t multiple_gaussian_backgrounds = kFALSE;
	// Keep track of the number of parameters
	int n_fit_params = 0;

	// Create the workspace and functions for fitting and create the summed pdf as we go
	RooWorkspace* ws = new RooWorkspace("ws", kTRUE);
	std::stringstream factory_cmd, sum_factory_cmd;
	sum_factory_cmd << "SUM::sum(";

	Bool_t linear = kFALSE;
	if(linear) {
		// First, the linear background
		factory_cmd << "Polynomial::pol1_bkg(edep[" << energy_low << ", " << energy_high << "], {bkg_offset[-1, 0], bkg_slope[-10, 10]})";
		ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
		sum_factory_cmd << "nbkg[0, 500000]*pol1_bkg";
		n_fit_params += 3; // bkg_offset, bkg_slope, nbkg
	} else {
		//exponential background
		factory_cmd << "Exponential::expo_bkg(edep[" << energy_low << ", " << energy_high << "], expo_slope[-1, 0.5])";
		ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
		sum_factory_cmd << "nbkg[10, 0, 500000]*expo_bkg";
		n_fit_params += 2; // expo_slope, nbkg
	}

	const char * signal_type = "gaussian";
	if(strncmp(signal_type, "gaussian", 2) == 0) {
		// Now the X-ray peak of interest
		// Now the X-ray peak of interest
		factory_cmd << "Gaussian::xraypeak_pdf(edep[" << energy_low << ", " << energy_high << "], xray_mean[" << xray->energy-3.5 << ", " << xray->energy+1.5 << "], xray_sigma[0.9, "<< signal_sigma_upper_offset<< "])"; // the x-ray peak itself
		ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
		sum_factory_cmd << ", xray_area[10, 0,500000]*xraypeak_pdf";
		n_fit_params += 3; // xray_mean, xray_sigma, xray_area
	}

	//////////BACKGROUNDS/////////
	//////////BACKGROUNDS/////////
	//////////BACKGROUNDS/////////

	// For Al 2p-1s, we also have a second peak that's a background
	if (xray->material == "Al" && xray->transition == "2p-1s") { //Pb-208 capture background to Tl-207
		factory_cmd << "Gaussian::bkgpeak_pdf(edep[" << energy_low << ", " << energy_high << "], bkg_mean[" << -2.5+351.1-bkg_lower_offset << "," << 351.1+bkg_upper_offset << "], bkg_sigma[1.05, 1.25])";
		ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
		sum_factory_cmd << ", bkg_area[6000, 0, 500000]*bkgpeak_pdf";
		n_fit_params += 3; // bkg_mean, bkg_sigma, bkg_area
	}
	// For Al 3p-1s, we also have a second peak that's a background
	if (xray->material == "Al" && xray->transition == "3p-1s") { //lead capture background
		factory_cmd << "Gaussian::bkgpeak_pdf(edep[" << energy_low << ", " << energy_high << "], bkg_mean[" << 415-bkg_lower_offset << "," << 415+bkg_upper_offset << "], bkg_sigma[1.05, 1.25])";
		ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
		sum_factory_cmd << ", bkg_area[600, 0, 250000]*bkgpeak_pdf";
		n_fit_params += 3; // bkg_mean, bkg_sigma, bkg_area
	}
	// Now create the SUM pdf
	sum_factory_cmd << ")";
	if(debug) std::cout << "\033[1;31m" << "RooFit factory string: " << sum_factory_cmd.str() << "\033[0m" << std::endl;
	ws->factory(sum_factory_cmd.str().c_str()); sum_factory_cmd.str("");
	// Import the histogram into a RooFit data hist
	RooDataHist data("data", "Ge Spectrum", (ws->argSet("edep")), hSpectrum);
	RooAbsPdf * pdf = ws->pdf("sum");
	//pdf->fitTo(data, RooFit::Extended() ); //RooFit::Extended() does an extended likelihood fit
	RooFitResult *res = pdf->fitTo(data, RooFit::Save() ); //RooFit::Save() to get RooFitResult object, which includes the covariance matrix
	if(debug) res->covarianceMatrix().Print();

	// Draw the fit
	RooPlot* Eframe = (ws->var("edep"))->frame();
	Eframe->SetName(descriptor );
	Eframe->SetTitle(Form("%s photopeak; E [keV]", descriptor) );
	data.plotOn(Eframe, RooFit::MarkerSize(1), RooFit::MarkerStyle(1) );
	//data.statOn(Eframe); 
	pdf->plotOn(Eframe, RooFit::Components("expo_bkg"), RooFit::LineStyle(kDotted) );
	pdf->plotOn(Eframe, RooFit::Components("xraypeak_pdf"), RooFit::LineStyle(kDashed), RooFit::LineColor(kBlack) );
	pdf->plotOn(Eframe, RooFit::Components("bkgpeak_pdf"), RooFit::LineStyle(kDashDotted) );
	if(multiple_gaussian_backgrounds) pdf->plotOn(Eframe, RooFit::Components("bkgpeak2_pdf"), RooFit::LineStyle(kDashDotted) );

	//Calculate number of stopped muons to display on plot`
	Double_t n_stopped_muons = 0;
	Double_t n_stopped_muons_error = 0;
	CalculateNumberOfStoppedMuons(xray, ws->var("xray_area"), n_stopped_muons, n_stopped_muons_error);

	pdf->plotOn(Eframe, RooFit::LineColor(kRed), RooFit::LineWidth(1) );
	pdf->paramOn(Eframe,
			RooFit::Layout(0.648, 0.957, 0.944),
			RooFit::Label(Form("#splitline{reduced #chi^{2} = %f}{N_{#gamma} = %.2e #pm %.2e}", Eframe->chiSquare(), n_stopped_muons, n_stopped_muons_error) )
		    );
	Eframe->getAttText()->SetTextFont(42);
	Eframe->getAttText()->SetTextSize(0.03);

	std::cout << "Goodness of fit: " << Eframe->chiSquare(n_fit_params) << std::endl;
	if(debug) Eframe->Draw();
	return ws->var("xray_area");
}

void MainProcess(const char * filename) {
	Double_t e1, t1;
	TString *channel;
	TFile* file = new TFile(filename, "READ");
	TFile *fOutputFile = new TFile("fit.root", "RECREATE");
	if (file->IsZombie()) {
		std::cout << "Error: Problem opening file " << filename << std::endl;
		return 1;
	}
	TH1D *hGeLoGain = new TH1D("hGeLoGain", "GeLoGain;E [keV];Counts/keV", 6000, 0, 6000);
	TH1D *hGeHiGain = new TH1D("hGeHiGain", "GeHiGain;E [keV];Counts/keV", 2700, 0, 2700);
	TTree *tree = (TTree *)file->Get("tree");
	tree->SetBranchAddress("e1", &e1);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("channel", &channel);
	for(Long64_t i=0; i < tree->GetEntries(); i++) {
		tree->GetEntry(i);
		if(t1 > 300 || t1 < -300) continue;
		if(channel->Contains("GeLoGain") ) {
			hGeLoGain->Fill(e1);
		} else if(channel->Contains("GeHiGain") ) {
			hGeHiGain->Fill(e1);
		}
	}

	CountXRays(hGeLoGain, "GeLoGain", "Al", "2p-1s", 10, 10, 1, 1, 1, 1.5);
	CountXRays(hGeLoGain, "GeLoGain", "Al", "3p-1s", 10, 10, 1, .5, 1, 1.5);

	CountXRays(hGeHiGain, "GeHiGain", "Al", "2p-1s", 10, 10, 1, 1, 1, 1.5);
	CountXRays(hGeHiGain, "GeHiGain", "Al", "3p-1s", 10, 10, 2, 1, 1, 1.5);
	fOutputFile->Write();
}

void Normalization(const char *filename = "/home/m-wong/data/R15b/al50.root") {
	gROOT->ProcessLine(".L Normalization.C");
        MainProcess(filename);
}
