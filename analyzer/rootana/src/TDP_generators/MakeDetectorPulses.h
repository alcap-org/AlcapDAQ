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

    void SetDefaultAlgorithm(const TString& algorithm){fDefaultAlgorithm=algorithm;};
    void SetDetectorPulseMap(StringDetPulseMap& aMap){fDetectorPulseMap=&aMap;}

    IDs::generator GetPassThruGeneratorID()const {
        return fPassThruGenerator?fPassThruGenerator->GetSource().Generator():IDs::generator();
    }

    private:
    TVDetectorPulseGenerator* MakeGenerator(const IDs::source& current_source, const IDs::source& partner_source, const std::string& generatorType, TDPGeneratorOptions* opts );
    virtual int ProcessEntry(TGlobalData *gData, const TSetupData* gSetup);
    virtual int BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup);
    virtual int AfterLastEntry(TGlobalData* gData, const TSetupData* setup){return 0;}

    void DumpgAnalysedPulseMap(const SourceAnalPulseMap& aMap);
    bool ParseGeneratorList(const IDs::source& current_source, const IDs::source& partner_source, const std::string& generatorList);

    private:
    modules::options* fOptions;
    TVDetectorPulseGenerator* fPassThruGenerator; 
    TVDetectorPulseGenerator* fGenerator; 
    StringDetPulseMap* fDetectorPulseMap;
    typedef std::set<Detector_t > ChannelPairing_t;
    ChannelPairing_t fFastSlowPairs;
    std::string fDefaultAlgorithm, fPassThruName;
    std::vector<std::string> fDetectorsToAnalyse;

};

#endif // MAKEDETECTORPULSES_H__
