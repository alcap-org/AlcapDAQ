#ifndef TEMPLATE_FITTING_TEMPLATECONVOLVER_H
#define TEMPLATE_FITTING_TEMPLATECONVOLVER_H

#include <set>
#include <vector>
#include "IdChannel.h"
#include "Convolver.h"
#include "QuadraticFit.h"

namespace IDs{ class channel;}
class TTemplate;
class TPulseIsland;
class TDirectory;

class TemplateConvolver{
    public:
      struct FoundPeaks{
         double time;
         double amplitude;
         double quad;
         double linear;
         double constant;
         bool operator<(const FoundPeaks& rhs)const{
           return amplitude>rhs.amplitude;
         }
      };
      typedef std::set<FoundPeaks> PeaksVector;
      typedef std::vector<double> SamplesVector;

    public:
      TemplateConvolver(const IDs::channel ch, TTemplate*, int peak_fit_samples);
      ~TemplateConvolver();
      bool IsValid()const{return fTemplateLength>0;}

    public:
      int Convolve(const TPulseIsland* tpi,double pedestal=0);
      void CharacteriseTemplate();

      const SamplesVector& GetEnergyConvolution()const{return fEnergySamples;};
      const SamplesVector& GetTimeConvolution()const{return fTimeSamples;};
      const PeaksVector& GetPeaks()const{return fPeaks;};
      double GetTimeShift()const{return fTemplateTime;}
      double GetAmplitudeScale()const{return fTemplateScale;}

    private:
      int FindPeaks( const SamplesVector&, const SamplesVector&);
      void FitPeak(int index, const SamplesVector&, const SamplesVector&);
      bool ResetVectors(int size);

    private:
      IDs::channel fChannel;
      TTemplate* fTemplate;
      Algorithm::Convolver<Algorithm::TH1_c_iterator>* fEnergyConvolve;
      Algorithm::Convolver<std::vector<int>::iterator>* fTimeConvolve;
      functions::QuadraticFit fQuadFit;
      const int fLeftSafety, fRightSafety;
      const int fTemplateLength;
      const double fTemplateTime;
      double fTemplateQuad, fTemplateLin, fTemplateConst, fTemplateScale;
      PeaksVector fPeaks;
      SamplesVector fEnergySamples;
      SamplesVector fTimeSamples;
      std::vector<int> fTimeWeights;
      SamplesVector fTemplateACF,fTemplateACFDerivative;

};

#endif // TEMPLATE_FITTING_TEMPLATECONVOLVER_H
