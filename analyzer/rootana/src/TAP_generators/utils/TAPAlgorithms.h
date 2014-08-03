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
};

struct Algorithm::MaxBinTime {
  public:
  double operator() (const TPulseIsland* tpi);
};

struct Algorithm::ConstantFractionTime {
  public:
  double operator() (const TPulseIsland* tpi);
  double constant_fraction;
  double th_frac;
};

#endif
