#ifndef TINTEGRALRATIOAUSSFITANALYSEDPULSE_H
#define TINTEGRALRATIOAUSSFITANALYSEDPULSE_H

#include "TAnalysedPulse.h"
#include <TObject.h>

class TIntegralRatioAnalysedPulse:public TAnalysedPulse{
    public:
        /// Needed by ROOT but not expected to be used
        TIntegralRatioAnalysedPulse():TAnalysedPulse(){};

        /// @brief Construct a TIntegralRatioAnalysedPulse.
        /// @details Same signature as for TAnalysedPulse so that MakeNewTAP can
        /// create these specialised TAPs
        TIntegralRatioAnalysedPulse(const IDs::source& sourceID,
                const TPulseIslandID& parentID, const TPulseIsland* parentTPI):
            TAnalysedPulse(sourceID,parentID,parentTPI){}

        // defined in .cpp file to force vtable to be built
        virtual ~TIntegralRatioAnalysedPulse(){};

        /// @name Getters
        /// @{
        double GetIntegralSmall()const{return fIntegralSmall;}
        double GetIntegralRatio()const{return fIntegralRatio;}
        /// @}

        /// @name Setters
        /// Set both the value and error for each field
        /// @{
        void SetIntegralSmall(const double& val){fIntegralSmall=val;}
        void SetIntegralRatio(const double& val){fIntegralRatio=val;}
        /// @}

        /// @@brief overload the TAnalysedPulse::Draw method
        virtual void Draw(const TH1F* tpi_pulse)const;

    private:
        double fIntegralSmall;
        double fIntegralRatio;

        ClassDef(TIntegralRatioAnalysedPulse,1);
};

#endif //TINTEGRALRATIOAUSSFITANALYSEDPULSE_H
