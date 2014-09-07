#include "TemplateFitter.h"

#include "HistogramFitFCN.h"
#include "SetupNavigator.h"

#include "TMath.h"

TemplateFitter::TemplateFitter(std::string detname, int refine_factor): fChannel(detname), fRefineFactor(refine_factor) {

  HistogramFitFCN* fcn = new HistogramFitFCN();
  fMinuitFitter = new TFitterMinuit(2); //  Two (2) parameters to modify (amplitude, pedestal). We will try and do the time one ourselves
  fMinuitFitter->SetMinuitFCN(fcn);
  fMinuitFitter->SetPrintLevel(-1); // set the debug level to quiet (-1=quiet, 0=normal, 1=verbose)
  fcn->SetRefineFactor(fRefineFactor);
}

TemplateFitter::~TemplateFitter() {
}

int TemplateFitter::FitPulseToTemplate(const TTemplate* hTemplate, const TH1D* hPulse, const std::string& bankname){

  int status; // the status of the minimisation
  static int print_dbg = false;


  // Calculate the max ADC value
  int n_bits = TSetupData::Instance()->GetNBits(bankname);
  double max_adc_value = std::pow(2, n_bits);


  // Prepare for minimizations
  fMinuitFitter->Clear();
  HistogramFitFCN* fcn = (HistogramFitFCN*)fMinuitFitter->GetMinuitFCN();
  fcn->SetTemplateHist(hTemplate->GetHisto());
  fcn->SetPulseHist(hPulse);

  //  fMinuitFitter->SetParameter(2, "TimeOffset", fTimeOffset, 1., -10, 10); // Timing should have step size no smaller than binning,
                                                    // *IF* the fourth argument is step size this is okay,
                                                    // or later implement some interpolation method, note
                                                    // *DERIVATIVES* at bounderies of interpolation may cause
                                                    // problems since MIGRAD (the default method) relies on
                                                    // these heavily.

  // Loop through some time offsets ourselved
  double max_time_offset = 4*fRefineFactor; // maximum distance to go from the initial estimate
  double best_time_offset = 0;
  double best_pedestal_offset = 0;
  double best_amplitude_scale_factor = 0;
  double best_chi2 = 99999999999;
  int best_status = -10000;

  // Calculate the bounds of the parameters
  fTimeOffset_minimum = fTimeOffset_estimate - max_time_offset;
  fTimeOffset_maximum = fTimeOffset_estimate + max_time_offset;

  fPedestalOffset_minimum = 0;
  fPedestalOffset_maximum = max_adc_value;

  fAmplitudeScaleFactor_minimum = 0.1;
  fAmplitudeScaleFactor_maximum = 100;

  for (double time_offset = fTimeOffset_minimum; time_offset <= fTimeOffset_maximum; ++time_offset) {
    if (print_dbg) {
      std::cout << "TemplateFitter: Checking time_offset = " << time_offset << std::endl;
    }
    fcn->SetTimeOffset(time_offset);

    // Reset the estimates
    fPedestalOffset = fPedestalOffset_estimate;
    fAmplitudeScaleFactor = fAmplitudeScaleFactor_estimate;
    fMinuitFitter->SetParameter(0, "PedestalOffset", fPedestalOffset, 0.1, fPedestalOffset_minimum, fPedestalOffset_maximum);
    fMinuitFitter->SetParameter(1, "AmplitudeScaleFactor", fAmplitudeScaleFactor, 0.1, fAmplitudeScaleFactor_minimum, fAmplitudeScaleFactor_maximum);
    fMinuitFitter->CreateMinimizer(TFitterMinuit::kMigrad);


    // Minimize and notify if there was a problem
    status = fMinuitFitter->Minimize(1000); // set limit of 1000 calls to FCN

    if (print_dbg) {
      if (status != 0)
	std::cout << "ERROR: Problem with fit (" << status << ")!" << std::endl;
    }

    // Get the fitted values
    fPedestalOffset = fMinuitFitter->GetParameter(0);
    fAmplitudeScaleFactor = fMinuitFitter->GetParameter(1);

    double delta_ped_offset_error = 1; // if the given parameter is within this much of the boundaries, then we will ignore it
    double delta_amp_sf_error = 0.001;
    if ( (fPedestalOffset < fPedestalOffset_minimum + delta_ped_offset_error && fPedestalOffset > fPedestalOffset_minimum - delta_ped_offset_error) ||
	 (fPedestalOffset < fPedestalOffset_maximum + delta_ped_offset_error && fPedestalOffset > fPedestalOffset_maximum - delta_ped_offset_error) ) {

      if (print_dbg) {
	std::cout << "ERROR: PedestalOffset has hit a boundary (" << fPedestalOffset << ")" << std::endl;
      }
      status = -9999;
    }
    else if ( (fAmplitudeScaleFactor < fAmplitudeScaleFactor_minimum + delta_amp_sf_error && fAmplitudeScaleFactor > fAmplitudeScaleFactor_minimum - delta_amp_sf_error) ||
	      (fAmplitudeScaleFactor < fAmplitudeScaleFactor_maximum + delta_amp_sf_error && fAmplitudeScaleFactor > fAmplitudeScaleFactor_maximum - delta_amp_sf_error) ) {

      if (print_dbg) {
	std::cout << "ERROR: AmplitudeScaleFactor has hit a boundary (" << fAmplitudeScaleFactor << ")" << std::endl;
      }
      status = -9999;
    }

    //    fTimeOffset = fMinuitFitter->GetParameter(2);

    // Store the Chi2, and then we can delete the pulse
    std::vector<double> params; 
    params.push_back(fPedestalOffset); 
    params.push_back(fAmplitudeScaleFactor); 
    //    params.push_back(fTimeOffset); 
    fChi2 = (*fcn)(params);

    fNDoF = fcn->GetNDoF();

    if (status == 0 && fChi2 < best_chi2) {
      best_time_offset = time_offset;
      best_pedestal_offset = fPedestalOffset;
      best_amplitude_scale_factor = fAmplitudeScaleFactor;
      best_chi2 = fChi2;
      best_status = status;
    }

    if (print_dbg) {
      std::cout << "TemplateFitter::FitPulseToTemplate(): Fit:\tChi2 " << fChi2 << "\tP "
		<< fPedestalOffset << "\tA " << fAmplitudeScaleFactor << "\tT " << fTimeOffset 
		<< ", ndof = " << fNDoF << ", prob = " << TMath::Prob(fChi2, fNDoF) << std::endl;
    }
  }

  // Store the final best values
  fChi2 = best_chi2;
  fTimeOffset = best_time_offset;
  fPedestalOffset = best_pedestal_offset;
  fAmplitudeScaleFactor = best_amplitude_scale_factor;

  double delta_time_offset_error = 1;
  if ( (fTimeOffset < fTimeOffset_minimum + delta_time_offset_error && fTimeOffset > fTimeOffset_minimum - delta_time_offset_error) ||
       (fTimeOffset < fTimeOffset_maximum + delta_time_offset_error && fTimeOffset > fTimeOffset_maximum - delta_time_offset_error) ) {

      if (print_dbg) {
	std::cout << "ERROR: TimeOffset has hit a boundary (" << fTimeOffset << ")" << std::endl;
      }
      best_status = -9999;
  }

  return best_status; // return status for the calling module to look at
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
  fPedestalOffset_estimate = pedestal;
  fAmplitudeScaleFactor_estimate = amplitude;
  fTimeOffset_estimate = time;
}
