#ifndef MAXBINBRANCHAPGENERATOR_H__
#define MAXBINBRANCHAPGENERATOR_H__

#include "TVAnalysedPulseGenerator.h"
#include "TAPAlgorithms.h"
#include <vector>
#include <string>

class TPulseIsland;
class TAnalysedPulse;
class TTree;

class MaxBinBranchAPGenerator:public TVAnalysedPulseGenerator {

 public:
  MaxBinBranchAPGenerator(TAPGeneratorOptions* opts);

  virtual ~MaxBinBranchAPGenerator(){};
  
public:
  virtual int ProcessPulses(const PulseIslandList&,AnalysedPulseList&);
  virtual bool MayDivideTPIs(){return false;};

private:
  /// \brief
  /// The algorithms that this generator uses
  Algorithm::MaxBinAmplitude fMaxBinAmplitude;
  Algorithm::MaxBinTime fMaxBinTime;

  TTree* fTree;
  Double_t fAmp;
  Double_t fTime;
  Int_t fBlock;
  bool fRenamed;
};

#endif // MAXBINBRANCHAPGENERATOR_H__
