/**
 * Code is not refactors and optimal yet, but it works
 * Run with the tproof-output.root file which is generated from Parallel/TMETree.C  
 * which was generated from the original TMETrees.
 * The script looks for specific TH1D histograms which have been through the
 * -100ns < t < 100ns, 100ns < t < 4000ns, and t > 4000ns time cuts.
 * On hindsight, I should have used a TH2D EnergyTime plot...
 * ```
 * $ root -l
 * $ .L Normalization.C
 * $ Normalization("tproof-output.root"
 * ```
 * The results are stored in photon-normalization.root.
 */

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

Bool_t debug = false;

//vectors to store the results of fitting and to plot the number of muons observed summary graph
std::vector<Double_t> vlResEnergy;
std::vector<Double_t> vlResNumber;
std::vector<Double_t> vlResError;
std::vector<Double_t> vhResEnergy;
std::vector<Double_t> vhResNumber;
std::vector<Double_t> vhResError;

int FillXRayInfo(XRay* xray);
RooRealVar* GetAreaUnderPeak(const char *descriptor, Double_t energy_low, Double_t energy_high, TH1* hSpectrum, XRay* xray);

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
int CountXRays(TH1D *hEnergy, std::string channel = "GeLoGain",  std::string target_material="Al", std::string transition="2p-1s", Double_t window_size = 8, Double_t lower_offset=1, Double_t upper_offset=1, int rebin_factor=1) {
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
	Double_t energy_low = xray.energy - window_size;
	Double_t energy_high = xray.energy + window_size;
	std::string descriptor = channel +  " " + target_material + " " + transition;
	RooRealVar* area = GetAreaUnderPeak(descriptor.c_str(), energy_low, energy_high, hEnergy, &xray, lower_offset, upper_offset);
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

	std::cout << "XRay: " << xray.material << " " << xray.transition << " " << xray.energy << " keV" << std::endl;
	std::cout << "Area = " << area->getValV() << " +/- " << area->getError() << " (" << (area->getError() / area->getValV()) * 100 << "%)" << std::endl;
	std::cout << "Intensity = " << xray.intensity << " +/- " << xray.intensity_error << " (" << (xray.intensity_error / xray.intensity) * 100 << "%)" << std::endl;
	std::cout << "Efficiency = " << xray.efficiency << " +/- " << xray.efficiency_error << " (" << (xray.efficiency_error / xray.efficiency) * 100 << "%)" << std::endl;
	std::cout << "Number of Stopped Muons = " << n_stopped_muons << " +- " << n_stopped_muons_error << " (" << (n_stopped_muons_error / n_stopped_muons) * 100 << "%)" << std::endl;

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
		// For R13
		//    xray->efficiency = 5e-4;
		//    xray->efficiency_error = 0.1e-4;
		return 0;
	}
	else if (xray->material == "Al" && xray->transition == "3p-1s") {
		std::cout << "Aluminium 3p-1s X-ray:" << std::endl;
		xray->energy = 412.87;
		xray->intensity = 0.0762;
		xray->intensity_error = 0.0015;
	}
	else if (xray->material == "Al" && xray->transition == "4p-1s") {
		std::cout << "Aluminium 4p-1s X-ray:" << std::endl;
		xray->energy = 435.96;
		xray->intensity = 0.0487;
		xray->intensity_error = 0.0010;
	}
	else if (xray->material == "Al" && xray->transition == "5p-1s") {
		std::cout << "Aluminium 5p-1s X-ray:" << std::endl;
		xray->energy = 446.61;
		xray->intensity = 0.0386;
		xray->intensity_error = 0.0010;
	}
	else if (xray->material == "Al" && xray->transition == "6p-1s") {
		std::cout << "Aluminium 6p-1s X-ray:" << std::endl;
		xray->energy = 452.38;
		xray->intensity = 0.0220;
		xray->intensity_error = 0.0010;
	}
	else if (xray->material == "Al" && xray->transition == "844") {
		std::cout << "Al-27 2+-0+ gamma ray:" << std::endl;
		xray->energy = 843.74;
		xray->intensity = 0.099;
		xray->intensity_error = 0.010;
	}
	else if (xray->material == "Al" && xray->transition == "983") {
		std::cout << "Al-28 0+(972.35) to 2+(30.6382) gamma ray:" << std::endl;
		xray->energy = 983.018;
		xray->intensity = 0.0017;
		xray->intensity_error = 0.0017;
	}
	else if (xray->material == "Al" && xray->transition == "941") {
		std::cout << "Al-28 0+(972.35) to 2+(30.6382) gamma ray:" << std::endl;
		xray->energy = 941.72;
		xray->intensity = 0.023;
		xray->intensity_error = 0.0011;
	}
	//Magnesium 27 peaks
	else if (xray->material == "Mg-27" && xray->transition == "985") {
		std::cout << "Mg-27 3/2+(984.88) to 1/2+(0.0) gamma ray:" << std::endl;
		xray->energy = 984.88;
		xray->intensity = 0.032;
		xray->intensity_error = 0.002;
	}
	else if (xray->material == "Mg-27" && xray->transition == "1699") {
		std::cout << "Mg-27 5/2+(1698.52) to 1/2+(0.0) gamma ray:" << std::endl;
		xray->energy = 1698.52;
		xray->intensity = 0.02;
		xray->intensity_error = 0.002;
	}
	else if (xray->material == "Mg-27" && xray->transition == "955") {
		std::cout << "Mg-27 5/2+(1940.23) to 3/2+(984.8) gamma ray:" << std::endl;
		xray->energy = 955.42;
		xray->intensity = 0.0126;
		xray->intensity_error = 0.0006;
	}
	//Magnesium 26 peaks
	else if (xray->material == "Mg-26" && xray->transition == "2510") {
		std::cout << "Mg-26 4+(4318.89) to 2+(1808.74) gamma ray:" << std::endl;
		xray->energy = 2510.15;
		xray->intensity = 0.087;
		xray->intensity_error = 0.007;
	}
	else if (xray->material == "Mg-26" && xray->transition == "2132") {
		std::cout << "Mg-26 3+(3941.57) to 2+(1808.74) gamma ray:" << std::endl;
		xray->energy = 2132.83;
		xray->intensity = 0.013;
		xray->intensity_error = 0.002;
	}
	else if (xray->material == "Mg-26" && xray->transition == "2938") {
		std::cout << "Mg-26 2+(2938.33) to 0+(0.0) gamma ray:" << std::endl;
		xray->energy = 2938.33;
		xray->intensity = 0.011;
		xray->intensity_error = 0.002;
	}
	else if (xray->material == "Mg-26" && xray->transition == "1130") {
		std::cout << "Mg-26 2+(2938.33) to 2+(1808.74) gamma ray:" << std::endl;
		xray->energy = 1129.59;
		xray->intensity = 0.148;
		xray->intensity_error = 0.015;
	}
	else if (xray->material == "Mg-26" && xray->transition == "1808") {
		std::cout << "Mg-26 2+(1808.74) to 0+(0.0) gamma ray:" << std::endl;
		xray->energy = 1808.74;
		xray->intensity = 0.51;
		xray->intensity_error = 0.05;
	}
	//Silicon peaks
	else if (xray->material == "Si") {
		std::cout << "Silicon x-ray:" << std::endl;
		xray->transition = "2p-1s";
		xray->energy = 400.177;
		xray->intensity = 0.803;
		xray->intensity_error = 0.008;
	}
	else if (xray->material == "Ti") {
		std::cout << "Titanium x-ray:" << std::endl;
		xray->transition = "2p-1s";
		//    xray->energy = 942; // PRL V18 N26 (1967)
		xray->energy = 931; // elog:336 and Fig. 1b of the above
		xray->intensity = 0.752; 
		xray->intensity_error = 0.007;
	}
	else if (xray->material == "511") {
		std::cout << "511 keV peak:" << std::endl;
		xray->transition = "511";
		xray->energy = 511;
		xray->intensity = 1;
		xray->intensity_error = 0;
	}
	else if (xray->material == "1460") {
		std::cout << "1460 keV peak:" << std::endl;
		xray->transition = "1460";
		xray->energy = 1460;
		xray->intensity = 1;
		xray->intensity_error = 0;
	}
	else {
		std::cout << "Error: Unknown target material" << std::endl;
		return 1;
	}
	return 0;
}

RooRealVar* GetAreaUnderPeak(const char * descriptor, Double_t energy_low, Double_t energy_high, TH1* hSpectrum, XRay* xray, Double_t bkg_lower_offset=1, Double_t bkg_upper_offset=1) {

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
		sum_factory_cmd << "nbkg[0, 250000]*pol1_bkg";
		n_fit_params += 3; // bkg_offset, bkg_slope, nbkg
	} else {
		//exponential background
		factory_cmd << "Exponential::expo_bkg_pdf(edep[" << energy_low << ", " << energy_high << "], expo_slope[-1, 0.5])";
		ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
		sum_factory_cmd << "expo_area[0,250000]*expo_bkg_pdf";
		n_fit_params += 2; // expo_slope, nbkg
	}

	const char * signal_type = "gaussian";
	if(signal_type == "gaussian") {
		// Now the X-ray peak of interest
		// Now the X-ray peak of interest
		factory_cmd << "Gaussian::xraypeak_pdf(edep[" << energy_low << ", " << energy_high << "], xray_mean[" << xray->energy-2 << ", " << xray->energy+2 << "], xray_sigma[0.1, 10])"; // the x-ray peak itself
		ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
		sum_factory_cmd << ", xray_area[0,0250000]*xraypeak_pdf";
		n_fit_params += 3; // xray_mean, xray_sigma, xray_area
	} else if(signal_type == "crystal_ball") {
		// Now the X-ray peak of interest, try crystal ball
		factory_cmd << "CBShape::xraypeak_pdf(edep[" << energy_low << ", " << energy_high << "], xray_mean[" << xray->energy-2 << ", " << xray->energy+2 << "], xray_sigma[0.1, 10], xray_alpha[-3, 0], xray_n[0,1])"; // the x-ray peak itself
		ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
		sum_factory_cmd << ", xray_area[0,250000]*xraypeak_pdf";
		n_fit_params += 5; // xray_mean, xray_sigma, xray_area
	} else if(signal_type == "voigtian") {
		factory_cmd << "Voigtian::xraypeak_pdf(edep[" << energy_low << ", " << energy_high << "], xray_mean[" << xray->energy-2 << ", " << xray->energy+2 << "], xray_gamma[0, 10], xray_sigma[0.1, 10])"; // the x-ray peak itself
		ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
		sum_factory_cmd << ", xray_area[0,250000]*xraypeak_pdf";
		n_fit_params += 4; // xray_mean, xray_sigma, xray_area
	}
	// For Al 2p-1s, we also have a second peak that's a background
	if (xray->material == "Al" && xray->transition == "2p-1s") { //Pb-208 capture background to Tl-207
		factory_cmd << "Gaussian::bkgpeak_pdf(edep[" << energy_low << ", " << energy_high << "], bkg_mean[" << 350-bkg_lower_offset << "," << 350+bkg_upper_offset << "], bkg_sigma[0.1, 2])";
		ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
		sum_factory_cmd << ", bkg_area[0,250000]*bkgpeak_pdf";
		n_fit_params += 3; // bkg_mean, bkg_sigma, bkg_area
	}
	// For Al 3p-1s, we also have a second peak that's a background
	if (xray->material == "Al" && xray->transition == "3p-1s") { //lead capture background
		factory_cmd << "Gaussian::bkgpeak_pdf(edep[" << energy_low << ", " << energy_high << "], bkg_mean[" << 416.1-bkg_lower_offset << "," << 416.1+bkg_upper_offset << "], bkg_sigma[0.1, 2])";
		ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
		sum_factory_cmd << ", bkg_area[0,250000]*bkgpeak_pdf";
		n_fit_params += 3; // bkg_mean, bkg_sigma, bkg_area
	}
	// For Al 4p-1s, we also have a second peak that's a background
	if (xray->material == "Al" && xray->transition == "4p-1s") { //lead 5g-4f
		factory_cmd << "Gaussian::bkgpeak_pdf(edep[" << energy_low << ", " << energy_high << "], bkg_mean[" << 430-bkg_lower_offset << "," << 430+bkg_upper_offset << "], bkg_sigma[0.1, 2])";
		ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
		sum_factory_cmd << ", bkg_area[0,250000]*bkgpeak_pdf";
		n_fit_params += 3; // bkg_mean, bkg_sigma, bkg_area

		//		factory_cmd << "Gaussian::bkgpeak2_pdf(edep[" << energy_low << ", " << energy_high << "], bkg_mean2[" << 438-bkg_lower_offset << "," << 438+bkg_upper_offset << "], bkg_sigma2[0.1, 2])";
		//		ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
		//		sum_factory_cmd << ", bkg_area[0,250000]*bkgpeak2_pdf";
		//		n_fit_params += 3; // bkg_mean, bkg_sigma, bkg_area
	}
	// For Al 5p-1s, we also have a second peak that's a background
	if (xray->material == "Al" && xray->transition == "5p-1s") { //lead capture background
		factory_cmd << "Gaussian::bkgpeak_pdf(edep[" << energy_low << ", " << energy_high << "], bkg_mean[" << 438-bkg_lower_offset << "," << 438+bkg_upper_offset << "], bkg_sigma[0.1, 2])";
		ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
		sum_factory_cmd << ", bkg_area[0,250000]*bkgpeak_pdf";
		n_fit_params += 3; // bkg_mean, bkg_sigma, bkg_area

		factory_cmd << "Gaussian::bkgpeak2_pdf(edep[" << energy_low << ", " << energy_high << "], bkg_mean2[" << 452-bkg_lower_offset << "," << 452+bkg_upper_offset << "], bkg_sigma2[0.1, 2])";
		ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
		sum_factory_cmd << ", bkg_area[0,250000]*bkgpeak2_pdf";
		n_fit_params += 3; // bkg_mean, bkg_sigma, bkg_area
	}

	if (xray->material == "511") {
		//Si 4p-1s
		factory_cmd << "Gaussian::bkgpeak_pdf(edep[" << energy_low << ", " << energy_high << "], bkg_mean[" << 503.58-bkg_lower_offset << "," << 503.58+bkg_upper_offset << "], bkg_sigma[0.1, 2])";
		ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
		sum_factory_cmd << ", bkg_area[0,250000]*bkgpeak_pdf";
		n_fit_params += 3; // bkg_mean, bkg_sigma, bkg_area

		//Si 5p-1s
		factory_cmd << "Gaussian::bkgpeak2_pdf(edep[" << energy_low << ", " << energy_high << "], bkg_mean2[" << 515.97-bkg_lower_offset << "," << 515.97+bkg_upper_offset << "], bkg_sigma2[0.1, 2])";
		ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
		sum_factory_cmd << ", bkg_area[0,250000]*bkgpeak2_pdf";
		n_fit_params += 3; // bkg_mean, bkg_sigma, bkg_area
	}

	if (xray->material == "Al" && xray->transition == "844") {
		//		//Mg-27 843.7 activation //does not quite work, although the peak is somewhat clearly seen
		//		factory_cmd << "Gaussian::bkgpeak_pdf(edep[" << energy_low << ", " << energy_high << "], bkg_mean[" << 843.74-bkg_lower_offset << "," << 843.74+bkg_upper_offset << "], bkg_sigma[0.1, 2])";
		//		ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
		//		sum_factory_cmd << ", bkg_area[0,250000]*bkgpeak_pdf";
		//		n_fit_params += 3; // bkg_mean, bkg_sigma, bkg_area
	}
	if (xray->material == "Mg-26" && xray->transition == "2510") {
		factory_cmd << "Gaussian::bkgpeak_pdf(edep[" << energy_low << ", " << energy_high << "], bkg_mean[" << 2501-bkg_lower_offset << "," << 2501+bkg_upper_offset << "], bkg_sigma[0.1, 2])";
		ws->factory(factory_cmd.str().c_str()); factory_cmd.str("");
		sum_factory_cmd << ", bkg_area[0,250000]*bkgpeak_pdf";
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
	pdf->plotOn(Eframe, RooFit::Components("expo_bkg_pdf"), RooFit::LineStyle(kDotted) );
	pdf->plotOn(Eframe, RooFit::Components("xraypeak_pdf"), RooFit::LineStyle(kDashed) );
	pdf->plotOn(Eframe, RooFit::Components("bkgpeak_pdf"), RooFit::LineStyle(kDashDotted) );
	if(xray->material == "511") pdf->plotOn(Eframe, RooFit::Components("bkgpeak2_pdf"), RooFit::LineStyle(kDashDotted) );

	//Calculate number of stopped muons to display on plot`
	Double_t n_stopped_muons = 0;
	Double_t n_stopped_muons_error = 0;
	CalculateNumberOfStoppedMuons(xray, ws->var("xray_area"), n_stopped_muons, n_stopped_muons_error);

	pdf->plotOn(Eframe, RooFit::LineColor(kRed), RooFit::LineWidth(1) );
	pdf->paramOn(Eframe,
			RooFit::Layout(0.648, 0.957, 0.944),
			RooFit::Label(Form("#splitline{#chi^{2} / ndf = %f / %d}{N_{#gamma} = %.2e #pm %.2e}", Eframe->chiSquare(), n_fit_params, n_stopped_muons, n_stopped_muons_error) )
		    );
	Eframe->getAttText()->SetTextFont(42);
	Eframe->getAttText()->SetTextSize(0.03);

	std::cout << "Goodness of fit: " << Eframe->chiSquare(n_fit_params) << std::endl;

	if(debug) Eframe->Draw();
	return ws->var("xray_area");
}

void Normalization(const char * filename) {
	TFile* file = new TFile(filename, "READ");
	TFile *fOutputFile = new TFile("photon-normalization.root", "RECREATE");
	if (file->IsZombie()) {
		std::cout << "Error: Problem opening file " << filename << std::endl;
		return 1;
	}
	// Get the 2D time-energy histogram
	TH1D* hGeLoGain_energy = (TH1D*) file->Get("hGeLoGain_energy");
	TH1D* hGeLoGain_energy_prompt = (TH1D*) file->Get("hGeLoGain_energy_prompt");
	TH1D* hGeLoGain_energy_semiprompt = (TH1D*) file->Get("hGeLoGain_energy_semiprompt");
	TH1D* hGeLoGain_energy_delayed = (TH1D*) file->Get("hGeLoGain_energy_delayed");
	TH1D* hGeHiGain_energy = (TH1D*) file->Get("hGeHiGain_energy");
	TH1D* hGeHiGain_energy_prompt = (TH1D*) file->Get("hGeHiGain_energy_prompt");
	TH1D* hGeHiGain_energy_semiprompt = (TH1D*) file->Get("hGeHiGain_energy_semiprompt");
	TH1D* hGeHiGain_energy_delayed = (TH1D*) file->Get("hGeHiGain_energy_delayed");

	//GeLoGain Prompt
	CountXRays(hGeLoGain_energy_prompt, "GeLoGain", "511", "", 8, 0.5, 0.5, 1);
	CountXRays(hGeLoGain_energy_prompt, "GeLoGain", "Al", "2p-1s", 8, 1, 0.5, 1);
	CountXRays(hGeLoGain_energy_prompt, "GeLoGain", "Al", "3p-1s", 8, 1.5, 1);
	CountXRays(hGeLoGain_energy_prompt, "GeLoGain", "Al", "4p-1s");
	//CountXRays(hGeLoGain_energy_prompt, "GeLoGain", "Al", "5p-1s");
	
	//GeLoGain semiprompt, 100ns to 4000ns
	CountXRays(hGeLoGain_energy_semiprompt, "GeLoGain", "Al", "844", 12, 2, 3);
	CountXRays(hGeLoGain_energy_semiprompt, "GeLoGain", "Mg-26", "1808", 8); 
	CountXRays(hGeLoGain_energy_semiprompt, "GeLoGain", "Mg-26", "1130", 12); 
	//CountXRays(hGeLoGain_energy_semiprompt, "GeLoGain", "Mg-26", "2938", 12);
	CountXRays(hGeLoGain_energy_semiprompt, "GeLoGain", "Mg-26", "2132", 12); 
	CountXRays(hGeLoGain_energy_semiprompt, "GeLoGain", "Mg-26", "2510", 20); 
	CountXRays(hGeLoGain_energy_semiprompt, "GeLoGain", "Mg-27", "985", 8); 
	CountXRays(hGeLoGain_energy_semiprompt, "GeLoGain", "Mg-27", "1699", 12); 
	CountXRays(hGeLoGain_energy_semiprompt, "GeLoGain", "Mg-27", "955", 8); 

	TGraphErrors *gGeLoGain = new TGraphErrors(vlResEnergy.size(), &vlResEnergy[0], &vlResNumber[0], 0, &vlResError[0]);
	gGeLoGain->SetMarkerStyle(kOpenCircle);
	gGeLoGain->SetMarkerColor(kRed);

	//GeHiGain Prompt
	CountXRays(hGeHiGain_energy_prompt, "GeHiGain", "511", "", 8, 0.5, 0.5, 2);
	CountXRays(hGeHiGain_energy_prompt, "GeHiGain", "Al", "2p-1s", 8, 1.5, 1, 1);
	CountXRays(hGeHiGain_energy_prompt, "GeHiGain", "Al", "3p-1s", 8, 1.5, 1, 1);
	CountXRays(hGeHiGain_energy_prompt, "GeHiGain", "Al", "4p-1s");
	//CountXRays(hGeHiGain_energy_prompt, "GeHiGain", "Al", "5p-1s");

	//GeHiGain semiprompt, 100ns to 4000ns
	CountXRays(hGeHiGain_energy_semiprompt, "GeHiGain", "Al", "844", 12);
	CountXRays(hGeHiGain_energy_semiprompt, "GeHiGain", "Mg-26", "1808", 8);
	CountXRays(hGeHiGain_energy_semiprompt, "GeHiGain", "Mg-26", "1130", 12);
	//CountXRays(hGeHiGain_energy_semiprompt, "GeHiGain", "Mg-26", "2938", 12); //does not exist
	CountXRays(hGeHiGain_energy_semiprompt, "GeHiGain", "Mg-26", "2132", 12);
	CountXRays(hGeHiGain_energy_semiprompt, "GeHiGain", "Mg-26", "2510", 20, 1, 1, 2);
	CountXRays(hGeHiGain_energy_semiprompt, "GeHiGain", "Mg-27", "985", 8);
	CountXRays(hGeHiGain_energy_semiprompt, "GeHiGain", "Mg-27", "1699", 12);
	//CountXRays(hGeHiGain_energy_semiprompt, "GeHiGain", "Mg-27", "955", 8);

	//delayed spectrum, >4000ns
	CountXRays(hGeHiGain_energy_delayed, "GeHiGain", "1460", "");
	CountXRays(hGeHiGain_energy_delayed, "GeHiGain", "1460", "");

	TGraphErrors *gGeHiGain = new TGraphErrors(vhResEnergy.size(), &vhResEnergy[0], &vhResNumber[0], 0, &vhResError[0]);
	gGeHiGain->SetMarkerStyle(kOpenSquare);

	//Draw number of stopped muons vs energy, which peaks used for observing muonic x-rays
	TCanvas *summary = new TCanvas("Summary", "Summary");
	TMultiGraph *mg = new TMultiGraph("NumberOfPhotons", "Number of photons observed; E[keV];");
	mg->Add(gGeLoGain);
	mg->Add(gGeHiGain);
	TLegend *legend = new TLegend(0.63, 0.79, 0.93, 0.94);
	legend->AddEntry(gGeLoGain, "GeLoGain", "lp");
	legend->AddEntry(gGeHiGain, "GeHiGain", "lp");
	legend->Draw();
	TLatex latex;
	latex.SetTextSize(0.025);
	for(int i=0; i<vlResNumber.size(); i++) latex.DrawLatex(vlResEnergy.at(i), vlResNumber.at(i), Form("%.2e", vlResNumber.at(i) ) );
	for(int i=0; i<vhResNumber.size(); i++) latex.DrawLatex(vhResEnergy.at(i), vhResNumber.at(i), Form("%.2e", vhResNumber.at(i) ) );
	mg->Draw("AP+");
	summary->Draw();
	summary->Write();

	//For reference, the energy spectra with different time cuts
	gStyle->SetOptStat(0);
	TCanvas *timeCompare = new TCanvas("GeLoGainTimeComparison", "Time Comparison");
	hGeLoGain_energy->Draw();
	hGeLoGain_energy->SetLineColor(kBlue);
	hGeLoGain_energy_prompt->Draw("SAME");
	hGeLoGain_energy_prompt->SetLineColor(kBlack);
	hGeLoGain_energy_semiprompt->Draw("SAME");
	hGeLoGain_energy_semiprompt->SetLineColor(kGreen);
	hGeLoGain_energy_delayed->Draw("SAME");
	hGeLoGain_energy_delayed->SetLineColor(kRed);
	timeCompare->BuildLegend();
	timeCompare->Write();

	TCanvas *cGeHiGainTimeCompare = new TCanvas("GeHiGainTimeComparison", "Time Comparison");
	hGeHiGain_energy->Draw();
	hGeHiGain_energy->SetLineColor(kBlue);
	hGeHiGain_energy_prompt->Draw("SAME");
	hGeHiGain_energy_prompt->SetLineColor(kBlack);
	hGeHiGain_energy_semiprompt->Draw("SAME");
	hGeHiGain_energy_semiprompt->SetLineColor(kGreen);
	hGeHiGain_energy_delayed->Draw("SAME");
	hGeHiGain_energy_delayed->SetLineColor(kRed);
	cGeHiGainTimeCompare->BuildLegend();
	cGeHiGainTimeCompare->Write();

	fOutputFile->Write();
}
