#include "ModulesManager.h"
#include "TemplateCreator.h"
#include "TemplateFitter.h"
#include <iostream>
#include <utility>
#include <cmath>
#include "RegisterModule.inc"

#include "TemplateArchive.h"
#include "TH2.h"

using std::cout;
using std::endl;
using std::string;

TemplateCreator::TemplateCreator(modules::options* opts):
   FillHistBase("TemplateCreator",opts),fOptions(opts){
	dir->cd("/");
}

TemplateCreator::~TemplateCreator(){
}

int TemplateCreator::BeforeFirstEntry(TGlobalData* gData,TSetupData *setup){
    return 0;
}

int TemplateCreator::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){

  // Loop over each detector
  string bankname, detname;
  PulseIslandList_t thePulseIslands;
  BankPulseList_t::const_iterator it;

  // Open the template archive, ready for saving
  TemplateArchive* archive = new TemplateArchive("templates.root");

  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); it++){
    bankname = it->first;
    detname = gSetup->GetDetectorName(bankname);

    // Get the TPIs
    thePulseIslands = it->second;
    if (thePulseIslands.size() == 0) continue; // no pulses here...

    // Create the template fitter and the template histogram itself
    TH1D* hTemplate = NULL;
    TemplateFitter* template_fitter = new TemplateFitter();

    // Now loop through the TPIs and create the templates
    for (PulseIslandList_t::const_iterator pulseIter = thePulseIslands.begin(); pulseIter != thePulseIslands.end(); ++pulseIter) {
      
      // If this is the first pulse, just add it directly to the template
      if (pulseIter == thePulseIslands.begin()) {
	AddPulseToTemplate(hTemplate, *pulseIter, 0); // want 0 as the time shift
	continue;
      }

      // Make an initial guess at the parameters
      double amplitude = 0;
      double time = 0;
      InitialParameterGuess(*pulseIter, amplitude, time);

      std::cout << "Amplitude = " << amplitude << ", time = " << time << std::endl;

      // Fit the pulse to the template

      // Then we will add the fitted pulse to the template

      // When the fit has converged, we stop
    }

    // Save the template
    //    archive->SaveTemplate(hTemplate);
  }

  delete archive; // close the template archive file
  return 0;
}


// InitialParameterGuess
// -- makes an initial guess at the parameters from the TPulseIsland
void TemplateCreator::InitialParameterGuess(const TPulseIsland* pulse, double& amplitude, double& time) {

  // Just use the TPI's methods for the time being
  amplitude = pulse->GetAmplitude();
  time = pulse->GetPulseTime();

}

// AddPulseToTemplate()
// -- adds the given pulse to the given template with the given time shift
// Input--
// pulse:     Pulse to average in
// shift:     Bin shift (timing offset of peak)
void TemplateCreator::AddPulseToTemplate(TH1D* hTemplate, TPulseIsland* pulse, double shift) {

  double norm;
  double peak;
  double sigma;
  std::vector<double> rectified_samples, reshaped_pulse;

  double polarity = (double)pulse->GetTriggerPolarity();
  double pedestal = pulse->GetPedestal(0);
  std::vector<int> samples = pulse->GetSamples();
  pedestal = (double)(samples[0]+samples[1]+samples[2]+samples[3]) / 4.; /*** TEMPERARY PEDESTAL ***/
  int n_samples = samples.size();
  for (int i = 0; i < n_samples; ++i)
    rectified_samples.push_back(polarity*((double)samples[i]-pedestal));
  // Get peak value for normalization
  peak = 0.;
  norm = polarity * (double)samples[0];
  for (int i = 1; i < n_samples; ++i) {
    if ((double)samples[i] * polarity > norm) {
      peak = (double)i;
      norm = (double)samples[i];
    }
  }
  norm *= polarity;
  // Get sigma for scaling
  sigma = 0.;
  for (int i = 0; i < n_samples; ++i) {
    std::cout << i << "\t" << sigma << "," << rectified_samples[i] << "\t";
    if (i % 10 == 0)
      std::cout << std::endl;
    sigma += rectified_samples[i] * std::pow(i - peak, 2.);
  }
  sigma = std::sqrt(std::abs(sigma)) / (double)n_samples;

  // If this is the first pulse (i.e. if the template doesn't exist yet), setup some variables
  if (hTemplate == NULL) {
    fClockTick = pulse->GetClockTickInNs() / (double)fRefine;
    fNBins = (int)(2. * (double)fNSigma * sigma) * fRefine;
    std::cout << "Making histogram: " << fNBins << " " << fNSigma << " " << sigma << " " << n_samples << " " << pedestal << std::endl;
    TString str = "template_";
    str += pulse->GetBankName();
    fTemplate = new TH1D(str, "Template", fNBins, -(double)fNSigma, (double)fNSigma);
  }

  // Reshape pulse
  // To go from course binning somehwere in the pulse
  // to fine binning in the template, just use
  // a linear f(x)=mx+b
  int pulse_index;
  double m = 2. * (double)fNSigma * sigma / (double)fNBins;
  double b = (peak + shift) - (double)fNSigma * sigma;
  for (int i = 0; i < fNBins; ++i) {
    pulse_index = (int)((double)i * m + b + 0.5); // The half shift is for bin-centering since int floors the double
    reshaped_pulse.push_back((double)samples[pulse_index] / norm);
  }

  // If this is the first pulse added to the template, smooth it.
  // In C++11, we'll have direct access to the reshaped pulse's
  // data array with vector.data(), but now we have to use
  // a somewhat circuitous route. TH1::SmoothArray takes an array
  // of doubles to smooth as an argument.
  if (fNPulses == 0) {
    unsigned int l = reshaped_pulse.size();
    double *reshaped_pulse_array = new double[l];
    for (unsigned int i = 0; i < l; ++i)
      reshaped_pulse_array[i] = reshaped_pulse[i];
    TH1::SmoothArray(reshaped_pulse.size(), reshaped_pulse_array, 1);
    for (unsigned int i = 0; i < l; ++i)
      reshaped_pulse.at(i) = reshaped_pulse_array[i];
    delete [] reshaped_pulse_array;
  }

  // Add in pulse to template
  // Histogram indexing is off by one (index 0 is the underflow bin)
  // Hence the (i+1), which really corresponds to the ith element
  // in the reshaped_pulse array.
  double x, x_old;
  double e, e_old;
  for (int i = 0; i < fNBins; ++i) {
    x_old = fTemplate->GetBinContent(i+1);
    e_old = fTemplate->GetBinError(i+1);
    x = (double)fNPulses * x_old + reshaped_pulse[i];
    x /= (double)(fNPulses + 1);
    e = (double)(fNPulses - 1) * std::pow(e_old, 2.) + ((double)reshaped_pulse[i] - x_old) * ((double)reshaped_pulse[i] - x);
    e = std::sqrt(e / fNPulses);
    fTemplate->SetBinContent(i+1, x);
    fTemplate->SetBinError(i+1, e);
  }

  // Find the change and average it with previous changes.
  // That is the dot product of the last template with
  // the new one subtracted from 1.
  if (fNPulses > 0) {
    double mag_old = hTemplate->Integral(1, fNBins);
    double mag_new = fTemplate->Integral(1,fNBins);
    double dot_product = 0.;
    for (int i = 1; i <= fNBins; ++i)
      dot_product += hTemplate->GetBinContent(i) * fTemplate->GetBinContent(i);
    double cos_similarity = dot_product / (mag_old * mag_new);
    fConvergence = ((double)(fNPulses - 1) * fConvergence + (1 - cos_similarity))/(double)fNPulses;
  }

  // Recalculate values for correlation coefficient
  fSumX = 0.;
  fSumX2 = 0.;
  int c = 0;
  for (int i = 1; i <= fNBins; ++i) {
    c = fTemplate->GetBinContent(i);
    fSumX += c;
    fSumX2 += std::pow(c, 2.);
  }
  fSum2X = std::pow(fSumX, 2.);

  // Keep track of the number of pulses
  ++fNPulses;
}

ALCAP_REGISTER_MODULE(TemplateCreator);
