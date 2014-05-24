#ifndef TVDETECTORPULSEGENERATOR_H__
#define TVDETECTORPULSEGENERATOR_H__

#include "TSetupData.h"
#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include <vector>
#include <string>

class TVDetectorPulseGenerator {
 protected:
  typedef std::vector<TAnalysedPulse*> AnalysedPulseList_t;
  typedef std::vector<TDetectorPulse*> DetectorPulseList_t;
  typedef std::map<std::string, AnalysedPulseList_t > BankPulseList_t;

 public:
  TVDetectorPulseGenerator(){};
  virtual ~TVDetectorPulseGenerator(){};

 public:
  virtual void ProcessPulses(const TSetupData* setup, const
		  AnalysedPulseList_t* fast, const AnalysedPulseList_t* slow,
		  DetectorPulseList_t& output)=0;
};

#endif // TVDETECTORPULSEGENERATOR_H__
