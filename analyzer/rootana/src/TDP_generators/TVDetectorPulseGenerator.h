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
  TVDetectorPulseGenerator(TVDetectorPulseGenerator::options *opts);
  virtual ~TVDetectorPulseGenerator(){};

 public:
  virtual void ProcessPulses(const TSetupData* setup, const
		  AnalysedPulseList_t* fast, const AnalysedPulseList_t* slow,
		  DetectorPulseList_t& output)=0;

  void SetDebug(const bool& val=true){fDebug=val;};
  bool Debug(){return fDebug;};

  void SetDetector(const std::string& val){fDetector=val;};
  std::string GetDetector(){return fDetector;};

 private:
  bool fDebug;
  std::string fDetector;
};

TVDetectorPulseGenerator::TVDetectorPulseGenerator(TVDetectorPulseGenerator::options *opts):
	fDebug(false){
		if(opts){
			SetDebug(opts->HasOption("debug"));
			SetDetector(opts->GetString("detector"));
		}
	}

#endif // TVDETECTORPULSEGENERATOR_H__
