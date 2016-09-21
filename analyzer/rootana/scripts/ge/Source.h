#ifndef SOURCE_H
#define SOURCE_H

#include "TTimeStamp.h"

class Source {
public:
  enum Isotope { Eu152, Co60, Y88, NIso };
  static const Double_t thalf[NIso];
  static const int npeaks[NIso];
  static const int n_eu152 = 10;
  static const int n_co60  = 2;
  static const int n_y88   = 2;
  static const Double_t e_Eu152[n_eu152];
  static const Double_t i_Eu152[n_eu152];
  static const Double_t a_Eu152[n_eu152];
  static const Double_t e_Co60[n_co60];
  static const Double_t i_Co60[n_co60];
  static const Double_t a_Co60[n_co60];
  static const Double_t e_Y88[n_y88];
  static const Double_t i_Y88[n_y88];
  static const Double_t a_Y88[n_y88];
  static const Double_t* E[NIso];
  static const Double_t* I[NIso];
  static const Double_t* ADC[NIso];

  TTimeStamp t0;  // Date of initial activity measurement
  Double_t a0;    // Initial activity in Bq
  Isotope iso;
  Source(TTimeStamp t0, Double_t a0, Isotope iso);
  Double_t Activity(TTimeStamp t) const;
  Double_t Decays(TTimeStamp t, Double_t lt) const;
  int Spectrum(const Double_t*& e, const Double_t*& i, const Double_t*& adc);
};
#endif /* SOURCE_H */
