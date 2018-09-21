#ifndef RUN_H
#define RUN_H

#include "TTimeStamp.h"
#include "Source.h"

double sync_pulse_cut_min = 10e3; // [ns]
double sync_pulse_cut_max = 96e6; // [ns]

class Run {

 private:
  int n;
  int nblock;
  TTimeStamp t0;
  //  double tblock;
  //  const static double tblock = (96e6 - 10e3) * 1e-9; // convert from ns to s
  //  const static double tblock = 0.098;
  const static double tblock = 0.096;
  Source src;
  bool eff;
  Double_t hiscaleest;
  Double_t deadtime;
public:
  Run(int n, int nblock, TTimeStamp t0, Source src,
      bool eff, Double_t hiscaleest, Double_t dead_time = 1.0);

  int N();
  Double_t LT();
  TTimeStamp T0();
  Double_t Decays();
  int Spectrum(const Double_t*& e, const Double_t*& i,
	       const Double_t*& adc);
  bool ValidForEff();
  Double_t HiScaleEstimate();
  Double_t DeadTime() { return deadtime; }
  //  Double_t GetMeasurementTimeRatio() { return (nblock*tblock) / (end_time.GetSec() - start_time.GetSec()); }
};

#endif /* RUN_H */
