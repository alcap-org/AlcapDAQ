#ifndef TDetectorPulse_h
#define TDetectorPulse_h

#include <vector>
#include <string>

#include <TObject.h>
#include <TH1.h>

#include "TAnalysedPulse.h"
#include "TSetupData.h"


#define TDP_GetField(ch, field)\
    return fParentPulse[ch]? \
    fParentPulse[ch]->Get##field():\
    definitions::DefaultValue

class TDetectorPulse : public TObject {
    public:
        enum ParentChannel_t { kFast, kSlow,kNumParents};
        struct Tag{};

    public:
        TDetectorPulse();
        TDetectorPulse(const IDs::source& sourceID,
                int s_parentID, const TAnalysedPulse* s_parent,
                int f_parentID, const TAnalysedPulse* f_parent
                );
        virtual ~TDetectorPulse();

        void Reset(Option_t* o = "");

        double GetEnergy(ParentChannel_t ch=kSlow)const{ 
            TDP_GetField(ch,Energy); }
        double GetTime(ParentChannel_t ch=kFast)const{ 
            TDP_GetField(ch,Time); }
        double GetAmplitude(ParentChannel_t ch=kFast)const{ 
            TDP_GetField(ch,Amplitude); }
        double GetIntegral(ParentChannel_t ch=kFast)const{ 
            TDP_GetField(ch,Integral); }
        double GetPedestal(ParentChannel_t ch=kFast)const{ 
            TDP_GetField(ch,Pedestal); }
        const TAnalysedPulse* GetTAP(ParentChannel_t ch)const {return fParentPulse[ch];}

        const IDs::source& GetSource() const { return fSource.GetValue(); }

        bool IsPileUpSafe()const{return fPileUpSafe&&fCheckedForPileUp;};
        void SetPileUpSafe(const bool& val=true){fPileUpSafe=val;fCheckedForPileUp=true;};
        bool WasPileUpChecked()const{return fCheckedForPileUp;};
        void SetPileUpChecked(const bool& val=true){ fCheckedForPileUp=val;};

        bool IsPairedPulse()const {return fParentPulse[kFast]&&fParentPulse[kSlow];}
        bool CouldBePaired()const {return fCouldBePaired;}
        bool IsGood()const {return ( !CouldBePaired() ) || IsPairedPulse();}

    private:
        bool fCheckedForPileUp;
        bool fPileUpSafe;
        bool fCouldBePaired;

        const TAnalysedPulse* fParentPulse[kNumParents];
        int fParentID[kNumParents];
        //IDs::source fParentSource[kNumParents];
        //IDs::source fSource;
        FlyWeight<IDs::source,TAnalysedPulse::Tag> fParentSource[kNumParents];
        FlyWeight<IDs::source,Tag> fSource;

        ClassDef(TDetectorPulse, 2);
};

#undef TDP_GetField
#endif
