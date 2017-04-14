#ifndef CFBRANCHAPGENERATOR_H__
#define CFBRANCHAPGENERATOR_H__

#include "TVAnalysedPulseGenerator.h"
#include "TAPAlgorithms.h"
#include <vector>
#include <string>
#include <fstream>

class TPulseIsland;
class TAnalysedPulse;
class TTree;

class CFBranchAPGenerator:public TVAnalysedPulseGenerator {

 public:
  CFBranchAPGenerator(TAPGeneratorOptions* opts);

  virtual ~CFBranchAPGenerator(){};

public:
  virtual int ProcessPulses(const PulseIslandList&,AnalysedPulseList&);
  virtual bool MayDivideTPIs(){return false;};

private:
  /// \brief
  /// The algorithms that this generator uses
  Algorithm::MaxBinAmplitude fMaxBinAmplitude;
  Algorithm::MaxBinTime fMaxBinTime;
  Algorithm::ConstantFractionTime fCFTime;

  TTree* fTree;
  int                 fEvent;
  std::vector<int>    fAmp;
  std::vector<double> fTime;
  bool fInitialized;
};

#endif // CFBRANCHAPGENERATOR_H__
