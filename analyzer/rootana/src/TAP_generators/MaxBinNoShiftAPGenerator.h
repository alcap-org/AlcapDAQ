#ifndef MAXBINNOSHIFTAPGENERATOR_H__
#define MAXBINNOSHIFTAPGENERATOR_H__

#include "TVAnalysedPulseGenerator.h"
#include "TAPAlgorithms.h"
#include <vector>

class TPulseIsland;
class TAnalysedPulse;

class MaxBinNoShiftAPGenerator:public TVAnalysedPulseGenerator {

 public:
  MaxBinNoShiftAPGenerator(TAPGeneratorOptions* opts);
  virtual ~MaxBinNoShiftAPGenerator(){};
  
public:
  virtual int ProcessPulses(const PulseIslandList&,AnalysedPulseList&);
  virtual bool MayDivideTPIs(){return false;};

private:

  /// \brief
  /// The algorithms that this generator uses
  Algorithm::MaxBinAmplitude fMaxBinAmplitude;
  Algorithm::MaxBinTime fMaxBinTime;
};

#endif // MAXBINNOSHIFTAPGENERATOR_H__
