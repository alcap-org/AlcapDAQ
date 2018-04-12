#ifndef TEMPLATEFITAPGENERATOR_H__
#define TEMPLATEFITAPGENERATOR_H__

#include "TSetupData.h"
#include "TVAnalysedPulseGenerator.h"
#include "definitions.h"
#include "TemplateArchive.h"
#include "TemplateFitter.h"
#include "TemplateMultiFitter.h"
#include "TTemplate.h"
#include "TTemplateFitAnalysedPulse.h"
#include "TAPAlgorithms.h"
#include "PulseCandidateFinder_TSpectrum.h"

#include "TTree.h"

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
    void InitializeSecondPulse( TH1D* tpi, const TTemplateFitAnalysedPulse* tap_one,
          int& second_time, double& second_scale)const;

  private:
    static TemplateArchive* fTemplateArchive;
    double fIntegralMax, fIntegralMin;
    double fIntegralRatioMax, fIntegralRatioMin;
    double fTemplateAmp, fTemplatePed, fTemplateTime;
    double fInitPedestal;
    double fChi2MinToRefit;
    TTemplate* fTemplate;
    Algorithm::IntegralRatio* fIntegralRatio;
    Algorithm::MaxBinAmplitude fMaxBin;
    Algorithm::ConstantFractionTime fCFTime;
    PulseCandidateFinder_TSpectrum* fPCF;
    int fMaxTemplates;

    TemplateMultiFitter* fMultiFitter;
    
    bool fNoTimeShift;
    bool fSyncPulses;
};

#endif //TEMPLATEFITAPGENERATOR_H__
