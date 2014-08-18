#ifndef TVMUONEVENTGENERATOR_H_
#define TVMUONEVENTGENERATOR_H_

#include <vector>
#include <map>
#include "TDetectorPulse.h"
#include "TMuonEvent.h"
#include "TMEGeneratorOptions.h"
#include "definitions.h"
#include "IdGenerator.h"

class TVMuonEventGenerator{
	public:
		TVMuonEventGenerator(const char* name,TMEGeneratorOptions* opts):
            fDebug(false),fGenerator(name,opts->StringDescription()){
			if(opts && opts->GetFlag("debug")) fDebug=true;
		};
		virtual ~TVMuonEventGenerator(){};

	public:
		virtual int ProcessPulses(MuonEventList& muonEventsOut,const StringDetPulseMap& detectorPulsesIn)=0;
        const IDs::generator& GetGenerator()const{return fGenerator;}

	protected:
		bool Debug()const {return fDebug;};
	private:
		bool fDebug;
        IDs::generator fGenerator;
};

#endif //TVMUONEVENTGENERATOR_H_
