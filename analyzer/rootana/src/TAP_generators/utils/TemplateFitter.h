#ifndef TemplateFitter_h_
#define TemplateFitter_h_

#include "TFitterMinuit.h"
#include "TPulseIsland.h"
#include "TH1D.h"

#include "definitions.h"

class TemplateFitter {
 public:
  TemplateFitter(std::string detname, int refine_factor);
  ~TemplateFitter();

 private:
  TFitterMinuit* fMinuitFitter;

 public:
  int FitPulseToTemplate(TH1D* hTemplate, TH1D* hPulse, std::string bankname);
  double GetPedestalOffset() { return fPedestalOffset; }
  double GetAmplitudeScaleFactor() { return fAmplitudeScaleFactor; }
  double GetTimeOffset() { return fTimeOffset; }
  double GetChi2() { return fChi2; }
  double GetNDoF() { return fNDoF; }

 private:
  IDs::channel fChannel;
  double fPedestalOffset;
  double fAmplitudeScaleFactor;
  double fTimeOffset;
  double fChi2;
  int fNDoF; // number of degress of freedom in fit

  /// \brief
  /// Store the initial estimates for the parameters
  double fPedestalOffset_estimate;
  double fAmplitudeScaleFactor_estimate;
  double fTimeOffset_estimate;

  /// \brief
  /// Store the minimum parameter bounds
  double fPedestalOffset_minimum;
  double fAmplitudeScaleFactor_minimum;
  double fTimeOffset_minimum;

  /// \brief
  /// Store the maximum parameter bounds
  double fPedestalOffset_maximum;
  double fAmplitudeScaleFactor_maximum;
  double fTimeOffset_maximum;

 public:
  /// \brief
  /// Sets the intial estimates for the template fitter
  void SetInitialParameterEstimates(double pedestal, double amplitude, double time);

  /// \brief
  /// Creates a refined histogram for a given TPulseIsland
  TH1D* CreateRefinedPulseHistogram(const TPulseIsland* pulse, std::string histname, std::string histtitle, bool interpolate);

 private:
  int fRefineFactor;
};

#endif
