#include "TemplateFitter.h"

#include "TemplateFitFCN.h"

TemplateFitter::TemplateFitter() {

  //  Three (3) parameters to modify (amplitude, time, pedestal)
  fNumParameters = 3;

  // Create the fitter
  TemplateFitFCN* fcn = new TemplateFitFCN();
  fFitter = new TFitterMinuit(fNumParameters);
  fFitter->SetMinuitFCN(fcn);
}

TemplateFitter::~TemplateFitter() {
}

void TemplateFitter::FitPulse(TH1F* hTemplate, const TPulseIsland* pulse) {
  // First make a histogram out of the pulse with the same bin width as the template,
  // Then pass to fitter.
  // Returns ped in units of ADC counts, amp in... scale units(?), and time in units
  // of bins since the beginning of pulse
  std::vector<int> samples = pulse->GetSamples();
  int nel = samples.size();
  TH1D* rebinned_pulse = new TH1D("pulse2fit", "pulse2fit", nel * fRefine, -0.5, nel - 0.5);
  for (int i = 0; i < nel; ++i)
    for (int j = 1; j <= fRefine; ++j)
      rebinned_pulse->SetBinContent(i * fRefine + j, samples.at(i));
  time *= fRefine;

  // Prepare for minimizations
  fFitter->Clear();
  TemplateFitFCN* fcn = (TemplateFitFCN*)fFitter->GetMinuitFCN();
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
}
