#ifndef TEMPLATEFITAPGENERATOR_H__
#define TEMPLATEFITAPGENERATOR_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"
#include "TemplateArchive.h"
#include "TemplateFitter.h"
#include "TTemplate.h"
#include "TTemplateFitAnalysedPulse.h"
#include "TAPAlgorithms.h"

class TemplateFitAPGenerator:public TVAnalysedPulseGenerator {

 public:
    TemplateFitAPGenerator(TAPGeneratorOptions* opts);
    virtual ~TemplateFitAPGenerator();

 public:
    virtual int ProcessPulses( const PulseIslandList&,AnalysedPulseList&);

    // This function should return true if this generator could break up a TPI
    // into more than one TAP
    virtual bool MayDivideTPIs(){return true;};
   

    static const char* TapType() {
        return TTemplateFitAnalysedPulse::Class()->GetName();
    }

  private:
    bool PassesIntegralRatio(const TPulseIsland* pulse,double& integral, double& ratio)const;
    bool RefitWithTwo( TH1D* tpi, TTemplateFitAnalysedPulse*& tap_one, TTemplateFitAnalysedPulse*& tap_two)const;

  private:
    static TemplateArchive* fTemplateArchive;
    static TemplateArchive* fTemplateArchive2;
    double fIntegralMax, fIntegralMin;
    double fIntegralRatioMax, fIntegralRatioMin;
    double fTemplateAmp, fTemplatePed, fTemplateTime;
    double fTemplate2Amp, fTemplate2Ped, fTemplate2Time;
    double fInitPedestal;
    double fChi2MinToRefit;
    TTemplate* fTemplate;
    TTemplate* fTemplate2;
    TemplateFitter* fFitter;
    TemplateMultiFitter* fDoubleFitter;
    Algorithm::IntegralRatio* fIntegralRatio;
    Algorithm::MaxBinAmplitude fMaxBin;

};

#endif //TEMPLATEFITAPGENERATOR_H__
