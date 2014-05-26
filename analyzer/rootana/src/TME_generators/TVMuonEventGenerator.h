#ifndef TVMUONEVENTGENERATOR_H_
#define TVMUONEVENTGENERATOR_H_

#include <vector>
#include <map>
#include "TDetectorPulse.h"
#include "TMuonEvent.h"
#include "ModulesOptions.h"

class TVMuonEventGenerator{
	protected:
		typedef std::vector<TDetectorPulse*> DetectorPulseList_t;
		typedef std::map<std::string, DetectorPulseList_t > BankDetPulseList_t;
		typedef std::vector<TMuonEvent*> MuonCentredTree_t;
		typedef modules::options options;

	public:
		TVMuonEventGenerator(TVMuonEventGenerator::options* opts):fDebug(false){
			if(opts->HasOption("debug")) fDebug=true;
		};
		virtual ~TVMuonEventGenerator(){};

	public:
		virtual int ProcessPulses(MuonCentredTree_t& muonEventsOut,const BankDetPulseList_t& detectorPulsesIn)=0;
		virtual bool ChecksForPileUp()const =0;

	protected:
		bool Debug()const {return fDebug;};
	private:
		bool fDebug;
};

#endif //TVMUONEVENTGENERATOR_H_
