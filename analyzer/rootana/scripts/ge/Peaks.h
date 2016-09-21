#ifndef PEAKS_H
#define PEAKS_H

#include "Peak.h"

#include "TFitResult.h"
class TF1;
class TGraphErrors;
class TH1;

#include <vector>

class Peaks {
  std::vector<Peak> peaks;
  TF1 *enfcn, *efffcn;
  TGraphErrors *engr, *effgr;
  TFitResultPtr enfitresult, efffitresult;
 public:
  Peaks(int n, const Double_t E[], const Double_t I[], const Double_t ADCs[]);
  int N();
  Peak& operator[](int);
  void EstimateParameters(TH1*);
  void Fit(TH1*, bool print = false);
  void Calibrate(Run, bool print = false);
  void Save();
};

#endif /* PEAKS_H */
