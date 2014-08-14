#ifndef MAXBINNOSHIFTAPGENERATOR_H__
#define MAXBINNOSHIFTAPGENERATOR_H__

#include "TVAnalysedPulseGenerator.h"
#include "TAPAlgorithms.h"
#include <vector>

class TPulseIsland;
class TAnalysedPulse;

class MaxBinNoShiftAPGenerator:public TVAnalysedPulseGenerator {

 public:
  MaxBinNoShiftAPGenerator(TAPGeneratorOptions* opts)
    : TVAnalysedPulseGenerator("MaxBinNoShiftAPGenerator", opts)
    {fThresholdPercent=opts->GetDouble("threshold_percent_of_range", 0.05);};

  virtual ~MaxBinNoShiftAPGenerator(){};
  
public:
  virtual int ProcessPulses(const PulseIslandList&,AnalysedPulseList&);
  virtual bool MayDivideTPIs(){return false;};

private:

  /// \brief
  /// The algorithms that this generator uses
  Algorithm::MaxBinAmplitude fMaxBinAmplitude;
  Algorithm::MaxBinTime fMaxBinTime;

  double fThresholdPercent;
};

#endif // MAXBINNOSHIFTAPGENERATOR_H__
