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

  const int trigger_polarity;
  const double pedestal;
};

struct Algorithm::MaxBinTime {
  public:
  double operator() (const TPulseIsland* tpi);

  const int trigger_polarity;
  const double clock_tick_in_ns;
  const double time_shift;
};

struct Algorithm::ConstantFractionTime {
  public:
  double operator() (const TPulseIsland* tpi);

  const double pedestal;
  const int trigger_polarity;
  const int max_adc_value;
  const double clock_tick_in_ns;
  const double time_shift;

  const double constant_fraction;
};


struct Algorithm::SimpleIntegral {
  public:
  double operator() (const TPulseIsland* tpi);

  const double pedestal;
  const int trigger_polarity;
};

#endif
