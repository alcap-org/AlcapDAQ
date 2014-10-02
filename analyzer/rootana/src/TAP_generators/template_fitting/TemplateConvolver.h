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
         double pedestal;
         double quad;
         double linear;
         double constant;
         bool operator<(const FoundPeaks& rhs)const{
           return time<rhs.time;
           //return amplitude>rhs.amplitude;
         }
         FoundPeaks& operator=(const FoundPeaks& rhs){
           time=rhs.time;
           amplitude=rhs.amplitude;
           pedestal=rhs.pedestal;
           quad=rhs.quad;
           linear=rhs.linear;
           constant=rhs.constant;
           return *this;
         }
      };
      typedef std::set<FoundPeaks> PeaksVector;
      typedef std::vector<double> SamplesVector;

    public:
      TemplateConvolver(const IDs::channel ch, TTemplate* tpl, int peak_fit_samples, int left, int right);
      ~TemplateConvolver();
      bool IsValid()const{return fTemplateLength>0;}

    public:
      int Convolve(const Algorithm::TpiMinusPedestal_iterator& begin,const Algorithm::TpiMinusPedestal_iterator& end);
      void CharacteriseTemplate();

      const SamplesVector& GetEnergyConvolution()const{return fEnergySamples;};
      const SamplesVector& GetTimeConvolution()const{return fTimeSamples;};
      const PeaksVector& GetPeaks()const{return fPeaks;};
      double GetTimeShift()const{return fTemplateTime;}
      double GetLeftSafety()const{return fLeftSafety;}
      double GetAmplitudeScale()const{return fTemplateScale;}
      TH1* GetTemplateACF()const {return fTemplateACFHist;}

      int FindPeaks(bool expect_pile_up, const Algorithm::TpiMinusPedestal_iterator& waveform);

    private:
      int FindAllPeaks( const SamplesVector&, const SamplesVector&, const Algorithm::TpiMinusPedestal_iterator* pedestal);
      int FindBestPeak( const SamplesVector&, const SamplesVector&, const Algorithm::TpiMinusPedestal_iterator* pedestal);
      void FitPeak(FoundPeaks& output, int index, const SamplesVector& energy, const SamplesVector& time, double pedestal);
      bool ResetVectors(int size);

    private:
      IDs::channel fChannel;
      TTemplate* fTemplate;
      Algorithm::Convolver<Algorithm::TH1_c_iterator>* fEnergyConvolve;
      Algorithm::Convolver<std::vector<int>::iterator>* fTimeConvolve;
      functions::QuadraticFit fQuadFit;
      const int fPolarity;
      const int fLeftSafety, fRightSafety;
      const int fTemplateLength;
      const double fTemplateTime;
      double fTemplateQuad, fTemplateLin, fTemplateConst, fTemplateScale;
      PeaksVector fPeaks;
      SamplesVector fEnergySamples;
      SamplesVector fTimeSamples;
      std::vector<int> fTimeWeights;
      SamplesVector fTemplateACF,fTemplateACFDerivative;
      TH1 *fTemplateACFHist, *fTemplateACFDerivativeHist;

};

#endif // TEMPLATE_FITTING_TEMPLATECONVOLVER_H
