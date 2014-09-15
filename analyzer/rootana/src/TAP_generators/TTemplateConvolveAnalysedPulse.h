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
      TTemplateConvolveAnalysedPulse():TAnalysedPulse(){};

      /// @brief Construct a TTemplateConvolveAnalysedPulse.
      /// @details Same signature as for TAnalysedPulse so that MakeNewTAP can
      /// create these specialised TAPs
      TTemplateConvolveAnalysedPulse(const IDs::source& sourceID,
              const TPulseIslandID& parentID, const TPulseIsland* parentTPI);

      // defined in .cpp file to force vtable to be built
      virtual ~TTemplateConvolveAnalysedPulse();

      /// @name Getters
      /// @{
      double GetIntegralRatio()const{return fIntegralRatio;}
      /// @}

      /// @name Setters
      /// @{
      void SetNPeaks(int val){fNPeaks=val;}
      void SetPeakRank(int val){
fPeakRank=val;
//if(val>1) DEBUG_VALUE(val,fPeakRank);
}
      void SetIntegralRatio(double val){fIntegralRatio=val;}
      void SetEnergyConvolve(const SamplesVector& r){fEnergySamples=r;};
      void SetTimeConvolve(const SamplesVector& r){fTimeSamples=r;};
      /// @}

      /// @@brief overload the TAnalysedPulse::Draw method
      virtual void Draw(const TH1F* tpi_pulse)const;

    private:
      int fNPeaks;
      int fPeakRank;
      double fIntegralRatio;
      SamplesVector fEnergySamples; //!
      SamplesVector fTimeSamples; //!

      ClassDef(TTemplateConvolveAnalysedPulse,1);
};

#endif // TTEMPLATECONVOLVEANALYSEDPULSE_H
