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
  typedef std::vector<TAnalysedPulse*> AnalysedPulseList_t;
  typedef std::vector<TDetectorPulse*> DetectorPulseList_t;
  typedef std::map<std::string, AnalysedPulseList_t > BankPulseList_t;

 public:
  TVDetectorPulseGenerator():fDebug(false){};
  virtual ~TVDetectorPulseGenerator(){};

 public:
  virtual void ProcessPulses(const TSetupData* setup,const std::string& detector, 
		  const AnalysedPulseList_t* fast, const AnalysedPulseList_t* slow,
		  DetectorPulseList_t& output)=0;

  void SetDebug(const bool& val=true){fDebug=val;};
  bool Debug(){return fDebug;};

 private:
  bool fDebug;
};

#endif // TVDETECTORPULSEGENERATOR_H__
