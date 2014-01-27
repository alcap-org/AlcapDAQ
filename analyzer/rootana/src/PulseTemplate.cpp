#include "PulseTemplate.h"
#include "HistogramFitFCN.h"

#include "TH1.h"
#include "TH1D.h"


PulseTemplate::PulseTemplate(int nSigma = 3, int refine = 100) : fTemplate(NULL), fFitter(NULL), fNPulses(0),
								 fNSigma(nSigma), fRefine(refine), fNBins(0),
								 fSumX(0.), fSumX2(0.), fSum2X(0.),
								 fAverageChange(0.), fClockTick(0) {
  HistogramFitFCN* fcn = new HistogramFitFCN();
  fFitter = new TFitterMinuit(3); //  Three (3) parameters to modify (amplitude, time, pedestal)
  fFitter->SetMinuitFCN(fcn);
}


PulseTemplate::~PulseTemplate() {
  delete fTemplate;
  delete fFitter; // Fitter should own HistogramFitFCN and destroy it according to documentation
}

double PulseTemplate::AddPulse(TPulseIsland* pulse, double shift) {
  // Function to average in pulse with template
  // Input--
  // pulse:     Pulse to average in
  // shift:     Bin shift (timing offset of peak)
  // Output--
  // change:    Sum of squares of change from old template to new per bin
  TH1D* old_template;
  double norm;
  double peak;
  double sigma;
  double pol;
  double ped;
  double change;
  int nsampes;
  std::vector<int> samples;
  std::vector<double> reshaped_pulse;

  old_template = (TH1D*)fTemplate->Clone();
  pol = (double)pulse->GetTriggerPolarity();
  ped = (double)pulse->GetPedestal();
  samples = pulse->GetSamples();
  nsamps = samples.size();
  // Get peak value for normalization
  peak = 0;
  norm = pol * (double)samples[0];
  for (int i = 1; i < nsamps; ++i) {
    if ((double)samples[i] * pol > norm) {
      peak = (double)i;
      norm = (double)samples[i];
    }
  }
  // Get sigma for scaling
  sigma = 0.;
  for (int i = 0; i < nsamps; ++i)
    sigma += std::pow((double)(samples[i]*i)*pol - peak, 2.);
  sigma = std::sqrt(sigma) / (double)nsamps;

  // If this is the first pulse, setup some variables
  if (fNPulses == 0) {
    fClockTick = pulse->GetClockTickInNs() / (double)fRefine;
    fNBins = (int)(2. * (double)fNSigma * sigma) * fRefine;
    fTemplate = new TH1D("template", "Template", fNBins, -(double)fNSigma, (double)fNSigma);
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
  double mag_old = old_template->Integral(1, fNBins);
  double mag_new = fTemplate->Integral(1,fNBins);
  double dot_product = 0.;
  for (int i = 1; i <= fNBins; ++i)
    dot_product += old_template->GetBinContent(i) * fTemplate->GetBinContent(i);
  double cos_similarity = dot_product / (mag_old * mag_new);
  if (fNPulses > 0)
    fAverageChange = ((double)(fNPulses - 1) * fAverageSimilarity + (1 - cos_similarity))/(double)fNPulses;

  // Recalculate values for correlation coefficient
  fSumX = 0.;
  fSumX2 = 0.;
  int c = 0;
  for (int i = 1; i <= fNBins; ++i) {
    c = fTemplate->GetBinContent(i);
    fSumX += c;
    fSsumX2 += std::pow(c,2.);
  }
  fSum2X = std::pow(fSumX, 2.);

  // Keep track of the number of pulses
  ++fNPulse;

  return fAverageChange;
}

double PulseTemplate::Correlation(TPulseIsland* pulse, double& ped, double& amp, double& time) {
  // Fits by finding the maximum correlation coefficient of template with pulse
  // Input--
  // pulse:    Pulse to fit
  // Output--
  // ped:      Pedestal of fit
  // amp:      Amplitude of fit
}

double PulseTemplate::Chi2Fit(TPulseIsland* pulse, double& ped, double& amp, double& time) {
  // First make a histogram out of the pulse with the same bin width as the template,
  // Then pass to fitter.
  // Returns ped in units of ADC counts, amp in... scale units(?), and time in units
  // of bins since the beginning of pulse
  std::vector<int> samples = pulse.GetSamples();
  int nel = samples.size();
  TH1D* rebinned_pulse = new TH1D("pulse2fit", "pulse2fit", nel * fRefine, -0.5, nel - 0.5);
  for (int i = 0; i < nel; ++i)
    for (int j = 1; j <= fRefine, ++j)
      rebinned_pulse.SetBinContent(i * fRefine + j, samples.at(i));
  time *= fRefine;

  // Prepare for minimizations
  fFitter->Clear();
  HistogramFitFCN* fcn = (HistogramFitFCN*)fFitter->GetMinuitFCN();
  fcn->SetH1(fTemplate);
  fcn->SetH2(rebinned_pulse);
  fFitter->SetParameter(0, "Pedestal", ped, 0.1, 0, 0);
  fFitter->SetParameter(1, "Amplitude", amp, 0.1, 0, 0);
  fFitter->SetParameter(2, "Time", time, 1., 0, 0); // Timing should have step size no smaller than binning,
                                                    // *IF* the fourth argument is step size this is okay,
                                                    // or later implement some interpolation method, note
                                                    // *DERIVATIVES* at bounderies of interpolation may cause
                                                    // problems since MIGRAD (the default method) relies on
                                                    // these heavily.
  fFitter->CreateMinimizer();

  // Minimize and notify if there was a problem
  int status = fFitter->Minimize();
  if (status != 0)
    std::cout << "ERROR: Problem with fit (" << status << ")!" << std::endl;

  // Get parameters and return chi2 of fit
  ped  = fFitter->GetParameter(0);
  amp = fFitter->GetParameter(1);
  time = fFitter->GetParameter(2) / (double)fRefine;
  fcn = (HistogramFitFCN*)fFitter->GetMinuitFCN();
  std::vector<double> params;
  params.push_back(ped);
  params.push_back(amp);
  params.push_back(time);

  return (*fcn)(params);
}

int PulseTemplate::GetNumberOfPulses() {
  retrun fNPulses;
}
