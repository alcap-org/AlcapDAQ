#ifndef TEMPLATE_FITTING_TEMPLATECONVOLVER_H
#define TEMPLATE_FITTING_TEMPLATECONVOLVER_H

#include <set>
#include <vector>
#include "IdChannel.h"
#include "Convolver.h"

namespace IDs{ class channel;}
class TTemplate;
class TPulseIsland;
class TDirectory;

class TemplateConvolver{
    public:
      struct FoundPeaks{
         double time;
         double amplitude;
         double second_diff;
         bool operator<(const FoundPeaks& rhs)const{
           return amplitude>rhs.amplitude;
         }
      };
      typedef std::set<FoundPeaks> PeaksVector;
      typedef std::vector<double> SamplesVector;

    public:
      TemplateConvolver(const IDs::channel ch, TTemplate*, double cut);
      ~TemplateConvolver();
      bool IsValid()const{return fTemplateLength>0;}

    public:
      int Convolve(const TPulseIsland* tpi,double pedestal=0);
      void AutoCorrelateTemplate();

      const SamplesVector& GetEnergyConvolution()const{return fEnergySamples;};
      const SamplesVector& GetTimeConvolution()const{return fTimeSamples;};
      const PeaksVector& GetPeaks()const{return fPeaks;};
      double GetTime()const{return fPeaks.begin()->time;}
      double GetAmplitude()const{return fPeaks.begin()->amplitude;}

    private:
      int FindPeaks();
      bool ResetVectors(int size);

    private:
      IDs::channel fChannel;
      TTemplate* fTemplate;
      int fTemplateLength;
      Algorithm::Convolver<Algorithm::TH1_c_iterator>* fEnergyConvolve;
      Algorithm::Convolver<std::vector<int>::iterator>* fTimeConvolve;
      int fLeftSafety, fRightSafety;
      double fFoundPeakCut;
      PeaksVector fPeaks;
      SamplesVector fEnergySamples;
      SamplesVector fTimeSamples;
    std::vector<int> fTimeWeights;

};

#endif // TEMPLATE_FITTING_TEMPLATECONVOLVER_H
