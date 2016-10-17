#ifndef RUN_H
#define RUN_H

#include "TTimeStamp.h"
#include "Source.h"

class Run {
  int n;
  int nblock;
  TTimeStamp t0;
  const static double tblock = 0.098;
  Source src;
  bool eff;
  Double_t hiscaleest;
public:
  Run(int n, int nblock, TTimeStamp t0, Source src,
      bool eff, Double_t hiscaleest);
  int N();
  Double_t LT();
  TTimeStamp T0();
  Double_t Decays();
  int Spectrum(const Double_t*& e, const Double_t*& i,
	       const Double_t*& adc);
  bool ValidForEff();
  Double_t HiScaleEstimate();
};

#endif /* RUN_H */
