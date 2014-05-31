#ifndef TVMUONEVENTGENERATOR_H_
#define TVMUONEVENTGENERATOR_H_

#include <vector>
#include <map>
#include "TDetectorPulse.h"
#include "TMuonEvent.h"
#include "ModulesOptions.h"
#include "definitions.h"

class TVMuonEventGenerator{
	protected:
		typedef modules::options options;

	public:
		TVMuonEventGenerator(TVMuonEventGenerator::options* opts):fDebug(false){
			if(opts->HasOption("debug")) fDebug=true;
		};
		virtual ~TVMuonEventGenerator(){};

	public:
		virtual int ProcessPulses(MuonEventList& muonEventsOut,const StringDetPulseMap& detectorPulsesIn)=0;
		virtual bool ChecksForPileUp()const =0;

	protected:
		bool Debug()const {return fDebug;};
	private:
		bool fDebug;
};

#endif //TVMUONEVENTGENERATOR_H_
