#ifndef TGAUSSFITANALYSEDPULSE_H
#define TGAUSSFITANALYSEDPULSE_H

#include "TAnalysedPulse.h"
#include <TObject.h>

class TGaussFitAnalysedPulse:public TAnalysedPulse{
    public:
        /// Needed by ROOT but not expected to be used
        TGaussFitAnalysedPulse():TAnalysedPulse(){};

        /// @brief Construct a TGaussFitAnalysedPulse.
        /// @details Same signature as for TAnalysedPulse so that MakeNewTAP can
        /// create these specialised TAPs
        TGaussFitAnalysedPulse(const IDs::source& sourceID,
                const TPulseIslandID& parentID, const TPulseIsland* parentTPI):
            TAnalysedPulse(sourceID,parentID,parentTPI){}

        // defined in .cpp file to force vtable to be built
        virtual ~TGaussFitAnalysedPulse(){};

        /// @name Getters
        /// @{
        int GetFitStatus()const{return fStatus;}
        double GetChi2()const{return fChi2;}
        double GetGradient()const{return fGradient;}
        double GetWidth()const{return fWidth;}
        /// @}

        /// @name Error_Getters
        /// @{
        double GetGradientErr()const{return fGradientErr;}
        double GetWidthErr()const{return fWidthErr;}
        double GetTimeErr()const{return fTimeErr;}
        double GetPedestalErr()const{return fPedestalErr;}
        double GetAmplitudeErr()const{return fAmplitudeErr;}
        /// @}

        /// @name Setters
        /// Set both the value and error for each field
        /// @{
        void SetTime(const double& val, const double& err){TAnalysedPulse::SetTime(val); fTimeErr=err;}
        void SetAmplitude(const double& val, const double& err){TAnalysedPulse::SetAmplitude(val); fAmplitudeErr=err;}
        void SetPedestal(const double& val, const double& err){TAnalysedPulse::SetPedestal(val); fPedestalErr=err;}
        void SetGradient(const double& val, const double& err){fGradient=val; fGradientErr=err;}
        void SetWidth(const double& val, const double& err){fWidth=val; fWidthErr=err;}
        void SetChi2(const double& val){fChi2=val;}
        void SetFitStatus(const double& val){fStatus=val;}
        /// @}

        /// @@brief overload the TAnalysedPulse::Draw method
        virtual void Draw(const TH1F* tpi_pulse)const;

    private:
        int fStatus;
        double fChi2, fWidth,fGradient;
        double fTimeErr, fWidthErr, fAmplitudeErr, fPedestalErr, fGradientErr; 

        ClassDef(TGaussFitAnalysedPulse,1);
};

#endif // TGaussFitAnalysedPulse.h
