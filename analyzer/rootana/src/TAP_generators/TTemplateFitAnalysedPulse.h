#ifndef TTEMPLATEFITANALYSEDPULSE_H
#define TTEMPLATEFITANALYSEDPULSE_H

#include "TAnalysedPulse.h"
#include "TTemplate.h"
#include <TRef.h>
#include <TObject.h>

class TTemplateFitAnalysedPulse:public TAnalysedPulse{
    public:
        /// Needed by ROOT but not expected to be used
        TTemplateFitAnalysedPulse():TAnalysedPulse(){};

        /// @brief Construct a TTemplateFitAnalysedPulse.
        /// @details Same signature as for TAnalysedPulse so that MakeNewTAP can
        /// create these specialised TAPs
        TTemplateFitAnalysedPulse(const IDs::source& sourceID,
                const TPulseIslandID& parentID, const TPulseIsland* parentTPI):
            TAnalysedPulse(sourceID,parentID,parentTPI){}

        // defined in .cpp file to force vtable to be built
        virtual ~TTemplateFitAnalysedPulse(){};

        /// @name Getters
        /// @{
        int GetFitStatus()const{return fStatus;}
        double GetChi2()const{return fChi2;}
        /// @}

        /// @name Error_Getters
        /// @{
        double GetTimeErr()const{return fTimeErr;}
        double GetPedestalErr()const{return fPedestalErr;}
        double GetAmplitudeErr()const{return fAmplitudeErr;}
        const TTemplate* GetTemplate()const{return (const TTemplate*) fTemplate.GetObject();}

        /// @}

        /// @name Setters
        /// Set both the value and error for each field
        /// @{
        void SetTime(const double& val, const double& err){TAnalysedPulse::SetTime(val); fTimeErr=err;}
        void SetAmplitude(const double& val, const double& err){TAnalysedPulse::SetAmplitude(val); fAmplitudeErr=err;}
        void SetPedestal(const double& val, const double& err){TAnalysedPulse::SetPedestal(val); fPedestalErr=err;}
        void SetChi2(const double& val){fChi2=val;}
        void SetNDoF(const double& val){fNDoF=val;}
        void SetFitStatus(const double& val){fStatus=val;}
        void SetTemplate(TTemplate* val){fTemplate=val;}
        /// @}
        
        using TAnalysedPulse::SetAmplitude;
        using TAnalysedPulse::SetPedestal;
        using TAnalysedPulse::SetTime;

        /// @@brief overload the TAnalysedPulse::Draw method
        virtual void Draw(const TH1F* tpi_pulse)const;

    private:
        int fStatus;
        double fChi2, fNDoF;
        double fTimeErr, fAmplitudeErr, fPedestalErr; 
        TRef fTemplate;

        ClassDef(TTemplateFitAnalysedPulse,1);
};

#endif // TTEMPLATEFITANALYSEDPULSE_H
