#include "Source.h"

#include "TMath.h"

#include <iostream>

Source::Source(TTimeStamp t0, Double_t a0, Isotope iso)
  : t0(t0), a0(a0), iso(iso) {}

Double_t Source::Activity(TTimeStamp t) const {
  Double_t dt = (t.GetSec() - t0.GetSec())/60./60./24./365.2425;
  double activity = a0*TMath::Power(0.5, dt/thalf[iso]);
  std::cout << "Activity = " << activity << std::endl;
  return activity;
}

Double_t Source::Decays(TTimeStamp t, Double_t lt) const {
  return Activity(t)*lt;
}

int Source::Spectrum(const Double_t*& e, const Double_t*& i,
		     const Double_t*& adc) {
  e = E[iso];
  i = I[iso];
  adc = ADC[iso];
  return npeaks[iso];
}

const Double_t Source::thalf[] = { 13.537, 5.2714, 0.291899 }; // Years
const int Source::npeaks[] = { Source::n_eu152, Source::n_co60, Source::n_y88 };

const Double_t Source::e_Eu152[] = { 244.6975, 344.2785, 411.1163, 443.965,
				     778.904,  867.378,  964.079,  1085.9,
				     1112.1,   1408.0 };
const Double_t Source::i_Eu152[] = { 0.0758,   0.265,    0.0223,   0.031,
				     0.1294,   0.0425,   0.146,    0.102,
				     0.136,    0.21 };
const Double_t Source::a_Eu152[] = { 615.,     865.,     1030.,    1115.,
				     1955.,    2180.,    2425.,    2730.,
				     2795.,    3540. };

const Double_t Source::e_Co60[]  = { 1173.237, 1332.501 };
const Double_t Source::i_Co60[]  = { 0.999736, 0.999856 };
const Double_t Source::a_Co60[]  = { 2968.,    3370. };

const Double_t Source::e_Y88[]   = { 898.042, 1836.063 };
const Double_t Source::i_Y88[]   = { 0.937,   0.992 };
const Double_t Source::a_Y88[]   = { 2250.,   4600. };

const Double_t* Source::E[]   = { Source::e_Eu152, Source::e_Co60,
				  Source::e_Y88 };
const Double_t* Source::I[]   = { Source::i_Eu152, Source::i_Co60,
				  Source::i_Y88 };
const Double_t* Source::ADC[] = { Source::a_Eu152, Source::a_Co60,
				  Source::a_Y88 };
