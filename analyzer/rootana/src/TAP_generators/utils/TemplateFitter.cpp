#include "TemplateFitter.h"

#include "HistogramFitFCN.h"

#include <TSQLiteServer.h>
#include <TSQLiteResult.h>
#include <TSQLiteRow.h>

#include <sstream>

TemplateFitter::TemplateFitter(std::string detname): fChannel(detname) {

  HistogramFitFCN* fcn = new HistogramFitFCN();
  fMinuitFitter = new TFitterMinuit(3); //  Three (3) parameters to modify (amplitude, time, pedestal)
  fMinuitFitter->SetMinuitFCN(fcn);
  fMinuitFitter->SetPrintLevel(1); // set the debug level to quiet (-1=quiet, 0=normal, 1=verbose)
}

TemplateFitter::~TemplateFitter() {
}

void TemplateFitter::FitPulseToTemplate(TH1D* hTemplate, const TPulseIsland* pulse) {
  // First make a histogram out of the pulse with the same bin width as the template,
  // Then pass to the Minuit fitter.
  // Returns ped in units of ADC counts, amp in... scale units(?), and time in units
  // of bins since the beginning of pulse
  std::vector<int> samples = pulse->GetSamples();
  int n_samples = samples.size();

  TH1D* hPulse = new TH1D("hPulseToFit", "hPulseToFit", n_samples, -0.5, n_samples - 0.5);
  std::string bankname = pulse->GetBankName();
  double pedestal_error = GetPedestalError(bankname); // should probably move to TSetupData or whatever
  for (int i = 0; i < n_samples; ++i) {
    hPulse->SetBinContent(i+1, samples.at(i));
    hPulse->SetBinError(i+1, pedestal_error);
  }

  // Prepare for minimizations
  fMinuitFitter->Clear();
  HistogramFitFCN* fcn = (HistogramFitFCN*)fMinuitFitter->GetMinuitFCN();
  fcn->SetTemplateHist(hTemplate);
  fcn->SetPulseHist(hPulse);
  fMinuitFitter->SetParameter(0, "PedestalOffset", fPedestalOffset, 0.1, 0, 0);
  fMinuitFitter->SetParameter(1, "AmplitudeScaleFactor", fAmplitudeScaleFactor, 0.1, 0, 0);
  fMinuitFitter->SetParameter(2, "TimeOffset", fTimeOffset, 1., 0, 0); // Timing should have step size no smaller than binning,
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

  // Get the fitted values
  fPedestalOffset = fMinuitFitter->GetParameter(0);
  fAmplitudeScaleFactor = fMinuitFitter->GetParameter(1);
  fTimeOffset = fMinuitFitter->GetParameter(2);

  // Store the Chi2, and then we can delete the pulse
  std::vector<double> params; 
  params.push_back(fPedestalOffset); 
  params.push_back(fAmplitudeScaleFactor); 
  params.push_back(fTimeOffset); 
  fChi2 = (*fcn)(params);

  static int print_dbg = false;
  if (print_dbg) {
    std::cout << "TemplateFitter::FitPulseToTemplate(): Fit:\tChi2 " << fChi2 << "\tP "
	      << fPedestalOffset << "(" << params.at(0) << ")\tA " << fAmplitudeScaleFactor << "(" << params.at(1) << ")\tT " << fTimeOffset << "(" << params.at(2) << ")" << std::endl;
  }

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

void TemplateFitter::SetInitialParameterEstimates(double pedestal, double amplitude, double time) {
  fPedestalOffset = pedestal;
  fAmplitudeScaleFactor = amplitude;
  fTimeOffset = time;
}


double TemplateFitter::GetPedestalError(std::string bankname) {

  // The values that we will read in
  double noise;

  // Get the SQLite database file
  TSQLiteServer* server = new TSQLiteServer("sqlite://pedestals-and-noises.sqlite");

  std::stringstream query; 
  std::string tablename = "pedestals_and_noises";
  if (server) {

    query << "SELECT * FROM " << tablename << " WHERE bank=\'" << bankname << "\';"; // get all the pedestals and noises
    TSQLiteResult* result = (TSQLiteResult*) server->Query(query.str().c_str());  // get the result of this query
    query.str(""); // clear the stringstream after use

    TSQLiteRow* row = (TSQLiteRow*) result->Next(); // get the first row
    while (row != NULL) {
      noise = atof(row->GetField(3));
      
      delete row;
      row = (TSQLiteRow*) result->Next(); // get the next row
    }
    delete result; // user has to delete the result
  }
  else {
    std::cout << "Error: Couldn't connect to SQLite database" << std::endl;
  }
  server->Close();
  return noise;
}
