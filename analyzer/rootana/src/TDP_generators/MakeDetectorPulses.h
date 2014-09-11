#ifndef MAKEDETECTORPULSES_H__
#define MAKEDETECTORPULSES_H__

#include <string>
#include <set>

#include "BaseModule.h"
#include "definitions.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"

#include "TVDetectorPulseGenerator.h"
class TDPGeneratorOptions;

class MakeDetectorPulses : public BaseModule{
    struct Detector_t{
        IDs::source source;
        IDs::source fast;
        IDs::source slow;
        TVDetectorPulseGenerator* generator;
        bool operator<(const Detector_t& rhs)const{
            return fast<rhs.fast || (fast==rhs.fast && slow<rhs.slow);
        }
        Detector_t(const IDs::source& so,
                const IDs::source& f,
                const IDs::source& s,
                TVDetectorPulseGenerator* g):
            source(so),fast(f),slow(s),generator(g){}
    };

    public:
    MakeDetectorPulses(modules::options* opts);
    ~MakeDetectorPulses();

    void SetAlgorithm(const TString& algorithm){fAlgorithm=algorithm;};
    void SetDetectorPulseMap(StringDetPulseMap& aMap){fDetectorPulseMap=&aMap;}

    IDs::generator GetPassThruGeneratorID()const {
        return fPassThruGenerator?fPassThruGenerator->GetSource().Generator():IDs::generator();
    }

    private:
    TVDetectorPulseGenerator* MakeGenerator(const std::string& generatorType,TDPGeneratorOptions* opts);
    virtual int ProcessEntry(TGlobalData *gData, const TSetupData* gSetup);
    virtual int BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup);
    virtual int AfterLastEntry(TGlobalData* gData, const TSetupData* setup){return 0;}

    void DumpgAnalysedPulseMap(const SourceAnalPulseMap& aMap);

    private:
    TVDetectorPulseGenerator* fPassThruGenerator; 
    TVDetectorPulseGenerator* fGenerator; 
    StringDetPulseMap* fDetectorPulseMap;
    typedef std::set<Detector_t > ChannelPairing_t;
    ChannelPairing_t fFastSlowPairs;
    modules::options* fOptions;
    std::string fAlgorithm, fPassThruName;

};

#endif // MAKEDETECTORPULSES_H__
