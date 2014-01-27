#include <vector>

class TH1D;
class TPulseIsland;
class TFitterMinuit;

class PulseTemplate {
 private:
  TH1D* fTemplate;
  TFitterMinuit* fFitter;
  int fNPulses;
  int fNSigma;
  int fRefine;
  int fNBins;
  double fSumX;
  double fSumX2;
  double fSum2X;
  double fAverageChange;
  double fClockTick;

 public:
  PulseTemplate();
  PulseTemplate(int nSigma, int refine);
  ~PulseTemplate();

 public:
  double AddPulse(TPulseIsland* pulse, double shift);
  double Correlation(TPulseIsland* pulse, double& ped, double& amp, double& time);
  double Fit(TPulseIsland* pulse, double& pedestal, double& amplitude, double& time);

 public:
  int GetNumberOfPulses();

};
