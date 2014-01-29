#ifndef PulseTemplate_h__
#define PulseTemplate_h__

#include "TH1D.h"
#include "TPulseIsland.h"
#include "TFitterMinuit.h"
#include "TFile.h"

#include <vector>

class PulseTemplate {
 private:
  TH1D* fTemplate;          // Template histogram
  TFitterMinuit* fFitter;   // MINUIT fitter to template

  int fNPulses;             // Number of pulses used in template
  int fNSigma;              // Number of sigma around peak (about) to make template
  int fRefine;              // Precision increase, each bin divided up into this many more bins
  int fNBins;               // Number of bins in the template
  double fClockTick;        // Bin width in time

  double fSumX;             // Sum of bin content for correlations
  double fSumX2;            // Sum of square of bin content for correlation
  double fSum2X;            // Square of sum of bin content for correlation

  double fConvergence;      // Average change of cosine of angle of template with each new pulse added
  double fConvergenceLimit; // Value the 
  

 public:
  PulseTemplate(int nSigma = 3, int refine = 100, double convergence = 0.01);
  ~PulseTemplate();

 public:
  void AddPulse(TPulseIsland* pulse, double shift);
  double Correlation(TPulseIsland* pulse, double& ped, double& amp, double& time);
  double Chi2Fit(TPulseIsland* pulse, double& pedestal, double& amplitude, double& time);
  bool Converged();

 public:
  void Save(TFile* outfile);
  void Print();

 public:
  int GetNumberOfPulses();

};

#endif
