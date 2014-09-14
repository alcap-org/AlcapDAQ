#ifndef TTEMPLATECONVOLVEANALYSEDPULSE_H
#define TTEMPLATECONVOLVEANALYSEDPULSE_H

#include "TAnalysedPulse.h"
#include "TTemplate.h"
#include <TRef.h>
#include <TObject.h>

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
      void SetNPeaks(double val){fNPeaks=val;}
      void SetIntegralRatio(double val){fIntegralRatio=val;}
      void SetEnergyConvolve(const SamplesVector& r){fEnergyConvolve=r;};
      void SetTimeConvolve(const SamplesVector& r){fTimeConvolve=r;};
      /// @}

    private:
      int fNPeaks;
      double fIntegralRatio;
      SamplesVector fEnergyConvolve; //!
      SamplesVector fTimeConvolve; //!

      ClassDef(TTemplateConvolveAnalysedPulse,1);
};

#endif // TTEMPLATECONVOLVEANALYSEDPULSE_H
