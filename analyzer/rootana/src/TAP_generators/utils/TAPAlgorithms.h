#ifndef TAPAlgorithms_h_
#define TAPAlgorithms_h_

#include "TPulseIsland.h"

// We will keep all the algorthims in their own namespace
namespace Algorithm {

  // The list of algorthims we have
  struct MaxBinAmplitude;
  struct MaxBinTime;
  struct ConstantFractionTime;
  struct SimpleIntegral;
};

struct Algorithm::MaxBinAmplitude {
  public:
  double operator() (const TPulseIsland* tpi);

  int trigger_polarity;
  double pedestal;
};

struct Algorithm::MaxBinTime {
  public:
  double operator() (const TPulseIsland* tpi);

  int trigger_polarity;
  double clock_tick_in_ns;
  double time_shift;
};

struct Algorithm::ConstantFractionTime {
  public:
  double operator() (const TPulseIsland* tpi);

  double pedestal;
  int trigger_polarity;
  int max_adc_value;
  double clock_tick_in_ns;
  double time_shift;

  double constant_fraction;
  double th_frac;
};

#endif
