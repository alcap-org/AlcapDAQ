#ifndef TEMPLATECONVOLVEAPGENERATOR_H__
#define TEMPLATECONVOLVEAPGENERATOR_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"
#include "TTemplate.h"
#include "TemplateArchive.h"
#include "TTemplateConvolveAnalysedPulse.h"
#include "TAPAlgorithms.h"
#include <vector>

class TemplateConvolver;

class TemplateConvolveAPGenerator:public TVAnalysedPulseGenerator {

 public:
  TemplateConvolveAPGenerator(TAPGeneratorOptions* opts);
  virtual ~TemplateConvolveAPGenerator(){};

 public:
   virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);

   virtual bool MayDivideTPIs(){return true;};

   static const char* TapType() {
       return TTemplateConvolveAnalysedPulse::Class()->GetName();
   }

  private:
    bool PassesIntegralRatio(const TPulseIsland* pulse,double& integral, double& ratio)const;

  private:
    static TemplateArchive* fTemplateArchive;
    TTemplate* fTemplate;
    TemplateConvolver* fConvolver;

    double fIntegralMax, fIntegralMin;
    double fIntegralRatioMax, fIntegralRatioMin;
    double fTemplateAmp, fTemplatePed, fTemplateTime;
    double fPedestal;
    bool fExpectPileUp; // Do we expect this generator to have to deal with pile-up waveforms?

    Algorithm::IntegralRatio* fIntegralRatio;

};

#endif //TEMPLATECONVOLVEAPGENERATOR_H__
