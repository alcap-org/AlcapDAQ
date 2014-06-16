#include "TemplateFitter.h"

#include "HistogramFitFCN.h"

TemplateFitter::TemplateFitter() {

  HistogramFitFCN* fcn = new HistogramFitFCN();
  fMinuitFitter = new TFitterMinuit(3); //  Three (3) parameters to modify (amplitude, time, pedestal)
  fMinuitFitter->SetMinuitFCN(fcn);
  fMinuitFitter->SetPrintLevel(-1); // set the debug level to quiet (-1=quiet, 0=normal, 1=verbose)
}

TemplateFitter::~TemplateFitter() {
}

void TemplateFitter::FitPulseToTemplate(TH1D* hTemplate, const TPulseIsland* pulse) {
  // First make a histogram out of the pulse with the same bin width as the template,
  // Then pass to fitter.
  // Returns ped in units of ADC counts, amp in... scale units(?), and time in units
  // of bins since the beginning of pulse
  std::vector<int> samples = pulse->GetSamples();
  int n_samples = samples.size();

  TH1D* hPulse = new TH1D("hPulseToFit", "hPulseToFit", n_samples, -0.5, n_samples - 0.5);
  for (int i = 0; i < n_samples; ++i) {
    hPulse->SetBinContent(i+1, samples.at(i));
  }

  // Prepare for minimizations
  fMinuitFitter->Clear();
  HistogramFitFCN* fcn = (HistogramFitFCN*)fMinuitFitter->GetMinuitFCN();
  fcn->SetH1(hTemplate);
  fcn->SetH2(hPulse);
  fMinuitFitter->SetParameter(0, "Pedestal", fPedestal, 0.1, 0, 0);
  fMinuitFitter->SetParameter(1, "Amplitude", fAmplitude, 0.1, 0, 0);
  fMinuitFitter->SetParameter(2, "Time", fTime, 1., 0, 0); // Timing should have step size no smaller than binning,
                                                    // *IF* the fourth argument is step size this is okay,
                                                    // or later implement some interpolation method, note
                                                    // *DERIVATIVES* at bounderies of interpolation may cause
                                                    // problems since MIGRAD (the default method) relies on
                                                    // these heavily.
  fMinuitFitter->CreateMinimizer();

  // Minimize and notify if there was a problem
  int status = fMinuitFitter->Minimize();
  if (status != 0)
    std::cout << "ERROR: Problem with fit (" << status << ")!" << std::endl;

  // Store the Chi2, and then we can delete the pulse
  std::vector<double> params; 
  params.push_back(fPedestal); 
  params.push_back(fAmplitude); 
  params.push_back(fTime); 
  fChi2 = (*fcn)(params);

  delete hPulse;
}

/*
double PulseTemplate::Correlation(TPulseIsland* pulse, double& ped, double& amp, double& time) {
  // Fits by finding the maximum correlation coefficient of template with pulse
  // Input--
  // pulse:    Pulse to fit
  // Output--
  // ped:      Pedestal of fit
  // amp:      Amplitude of fit
  return 0.;
}
*/
