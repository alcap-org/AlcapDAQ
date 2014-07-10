#ifndef TVDETECTORPULSEGENERATOR_H__
#define TVDETECTORPULSEGENERATOR_H__

#include "TSetupData.h"
#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "TDPGeneratorOptions.h"
#include "definitions.h"

class TVDetectorPulseGenerator {
 public:

 public:
  TVDetectorPulseGenerator(TDPGeneratorOptions* opts):fDebug(false){};
  virtual ~TVDetectorPulseGenerator(){};

 public:
  virtual int ProcessPulses(const TSetupData* setup,const std::string& detector, 
		  const AnalysedPulseList* fast, const AnalysedPulseList* slow,
		  DetectorPulseList& output)=0;
  virtual bool ChecksForPileUp()const =0;

  void SetDebug(const bool& val=true){fDebug=val;};
  bool Debug(){return fDebug;};

 private:
  bool fDebug;
};

#endif // TVDETECTORPULSEGENERATOR_H__
