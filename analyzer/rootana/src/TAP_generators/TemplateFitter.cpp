#include "TemplateFitter.h"

#include "TemplateFitFCN.h"

TemplateFitter::TemplateFitter() {

  //  Three (3) parameters to modify (amplitude, time, pedestal)
  fNumParameters = 3;

  // Create the fitter
  TemplateFitFCN* fcn = new TemplateFitFCN();
  fMinuitFitter = new TFitterMinuit(fNumParameters);
  fMinuitFitter->SetMinuitFCN(fcn);
}

TemplateFitter::~TemplateFitter() {
}

void TemplateFitter::FitPulse(TH1D* hTemplate, const TPulseIsland* pulse) {
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
  TemplateFitFCN* fcn = (TemplateFitFCN*)fMinuitFitter->GetMinuitFCN();
  fcn->SetH1(hTemplate);
  fcn->SetH2(hPulse);
  double ped, amp, time;
  fMinuitFitter->SetParameter(0, "Pedestal", ped, 0.1, 0, 0);
  fMinuitFitter->SetParameter(1, "Amplitude", amp, 0.1, 0, 0);
  fMinuitFitter->SetParameter(2, "Time", time, 1., 0, 0); // Timing should have step size no smaller than binning,
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
}

// Want getters to get the parameters from the fit (if it's been successful)
