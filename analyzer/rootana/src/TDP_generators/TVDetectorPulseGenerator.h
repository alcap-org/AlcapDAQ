#ifndef TVDETECTORPULSEGENERATOR_H__
#define TVDETECTORPULSEGENERATOR_H__

#include "TSetupData.h"
#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "ModulesOptions.h"
#include <vector>
#include <string>

class TVDetectorPulseGenerator {
 public:
  typedef modules::options options;
 protected:
  typedef std::vector<TAnalysedPulse*> AnalysedPulseList;
  typedef std::vector<TDetectorPulse*> DetectorPulseList;
  typedef std::map<std::string, AnalysedPulseList > StringAnalPulseMap;

 public:
  TVDetectorPulseGenerator():fDebug(false){};
  virtual ~TVDetectorPulseGenerator(){};

 public:
  virtual void ProcessPulses(const TSetupData* setup,const std::string& detector, 
		  const AnalysedPulseList* fast, const AnalysedPulseList* slow,
		  DetectorPulseList& output)=0;
  virtual bool ChecksForPileUp()const =0;

  void SetDebug(const bool& val=true){fDebug=val;};
  bool Debug(){return fDebug;};

 private:
  bool fDebug;
};

#endif // TVDETECTORPULSEGENERATOR_H__
