#ifndef TTEMPLATECONVOLVEANALYSEDPULSE_H
#define TTEMPLATECONVOLVEANALYSEDPULSE_H

#include "TAnalysedPulse.h"
#include "TTemplate.h"
#include <TRef.h>
#include <TObject.h>
#ifndef __CINT__
#include "debug_tools.h"
#include <iostream>
#endif// __CINT__

class TTemplateConvolveAnalysedPulse:public TAnalysedPulse{

    public:
      typedef std::vector<double> SamplesVector;

    public:
      /// Needed by ROOT but not expected to be used
      TTemplateConvolveAnalysedPulse();

      /// @brief Construct a TTemplateConvolveAnalysedPulse.
      /// @details Same signature as for TAnalysedPulse so that MakeNewTAP can
      /// create these specialised TAPs
      TTemplateConvolveAnalysedPulse(const IDs::source& sourceID,
				     const TPulseIslandID& parentID, const TAnalysedPulseID& tapID, const TPulseIsland* parentTPI);

      // defined in .cpp file to force vtable to be built
      virtual ~TTemplateConvolveAnalysedPulse();

      virtual void Copy(TObject& rhs)const{
         TAnalysedPulse::Copy(rhs);
         if(rhs.InheritsFrom(Class())){
           TTemplateConvolveAnalysedPulse* tap=static_cast<TTemplateConvolveAnalysedPulse*>(&rhs);
            tap->fNPeaks       =fNPeaks;
            tap->fPeakRank     =fPeakRank;
            tap->fIntegralRatio=fIntegralRatio;
            tap->fQuad=fQuad;
            tap->fLinear=fLinear;
            tap->fConstant=fConstant;
            tap->fTimeOffset=fTimeOffset;
            tap->fAmplitudeScale=fAmplitudeScale;
         }
      }

      /// @name Getters
      /// @{
      double GetIntegralRatio()const{return fIntegralRatio;}
      int GetNPeaks()const{return fNPeaks;}
      int GetPeakRank()const{return fPeakRank;}
      const TTemplate* GetTemplate()const{return (const TTemplate*) fTemplate.GetObject();}
      /// @}

      /// @name Setters
      /// @{
      void SetNPeaks(int val){fNPeaks=val;}
      void SetPeakRank(int val){ fPeakRank=val; }
      void SetTimeOffset(double val){ fTimeOffset=val; }
      void SetAmplitudeScale(double val){ fAmplitudeScale=val; }
      void SetIntegralRatio(double val){fIntegralRatio=val;}
      void SetQuadraticFit(double a, double b, double c){fQuad=a; fLinear=b; fConstant=c;};
      void SetEnergyConvolve(const SamplesVector& r){fEnergySamples=r;};
      void SetTimeConvolve(const SamplesVector& r){fTimeSamples=r;};
      void SetTemplate(TTemplate* val){fTemplate=val;}
      /// @}

      /// @@brief overload the TAnalysedPulse::Draw method
      virtual void Draw(const TH1F* tpi_pulse)const;

    private:
      int fNPeaks;
      int fPeakRank;
      double fIntegralRatio;
      double fQuad, fLinear, fConstant;
      double fTimeOffset, fAmplitudeScale;
        TRef fTemplate;//!
      SamplesVector fEnergySamples; //!
      SamplesVector fTimeSamples; //!

      ClassDef(TTemplateConvolveAnalysedPulse,2);
};

#endif // TTEMPLATECONVOLVEANALYSEDPULSE_H
