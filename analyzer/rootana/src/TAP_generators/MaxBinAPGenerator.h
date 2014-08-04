#ifndef MAXBINAPGENERATOR_H__
#define MAXBINAPGENERATOR_H__

#include "TVAnalysedPulseGenerator.h"
#include "TAPAlgorithms.h"
#include <vector>
#include <string>

class TPulseIsland;
class TAnalysedPulse;

class MaxBinAPGenerator:public TVAnalysedPulseGenerator {

 public:
  MaxBinAPGenerator(TAPGeneratorOptions* opts)
    : TVAnalysedPulseGenerator("MaxBinAPGenerator", opts)
  {};

  virtual ~MaxBinAPGenerator(){};
  
public:
  virtual int ProcessPulses(const PulseIslandList&,AnalysedPulseList&);
  virtual bool MayDivideTPIs(){return false;};

private:

  /// \brief
  /// The algorithms that this generator uses
  Algorithm::MaxBinAmplitude fMaxBinAmplitude;
  Algorithm::MaxBinTime fMaxBinTime;
};

#endif // MAXBINAPGENERATOR_H__
