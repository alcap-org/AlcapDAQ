#ifndef TemplateFastFitter_h_
#define TemplateFastFitter_h_

#include "TFitterMinuit.h"
#include "TPulseIsland.h"
#include "TTemplate.h"

class MultiHistogramFitFCN;
#include "definitions.h"
#include "AlcapExcept.h"

MAKE_EXCEPTION(TemplateFastFitter, Base);
MAKE_EXCEPTION(MismatchedTemplateRefineFactors, TemplateFastFitter);

class TemplateFastFitter {
  void operator=(const TemplateFastFitter&);

  struct TemplateDetails_t{
    double fAmplitudeScaleFactor;
    double fTimeOffset;
    TTemplate* fTemplate;
    double GetTime()const{ return  fTemplate->GetTime()+fTimeOffset; }
    double GetAmplitude()const{ return  fTemplate->GetAmplitude()*fAmplitudeScaleFactor; }
    void SetTime( double time){
      fTimeOffset=time- fTemplate->GetTime();
    }
  };
  typedef std::vector<TemplateDetails_t> TemplateList;

 public:
  TemplateFastFitter(const IDs::channel& ch);
  ~TemplateFastFitter();
  void Init();

 public:
  enum FitStatus{
    kSuccess=0 ,
    kPedestalOutOfBounds= -100,
    kAmplitudeOutOfBounds= -101,
    kTimeOutOfBounds= -102,
    kFitNotConverged= 1,
    kInitialised=-200
  };
  void AddTemplate(TTemplate* tpl);

  /// @name fitting
  /// @{
  /// \brief
  /// Sets the intial estimates for the template fitter
  //void SetPedestal(double v){fPedestal=v;}
  void SetPulseEstimates( int index,double time){
      fTemplates.at(index).SetTime(time);
  }
  int FitWithOneTimeFree(int index,const TH1D* hPulse);
  int FitWithAllTimesFixed(const TH1D* hPulse);
  /// @}

  /// @name getters
  /// @{
  double GetPedestal()const { return fPedestal; }
  double GetAmplitude(int i)const { return fTemplates.at(i).GetAmplitude();}
  double GetTime(int i)const { return fTemplates.at(i).GetTime(); }
  double GetChi2()const { return fChi2; }
  double GetNDoF()const { return fNDoF; }
  /// @}

 private:
  TFitterMinuit* fMinuitFitter;
  MultiHistogramFitFCN* fFitFCN;
  IDs::channel fChannel;
  double fMaxADC,fMinADC;

  double fPedestal;
  int fRefineFactor;
  TemplateList fTemplates;
  int fFreeTemplate;

  double fChi2;
  int fNDoF; // number of degress of freedom in fit
};

#endif // TemplateFastFitter_h_
