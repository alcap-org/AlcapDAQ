#ifndef MultiHistogramFastFitFCN_h__
#define MultiHistogramFastFitFCN_h__

#include "Minuit2/FCNBase.h"
#include "TH1D.h"

#include "AlcapExcept.h"
#include "TemplateFastFitParameters.h"
#include "debug_tools.h"

#include <iostream>
#include <vector>

MAKE_EXCEPTION(MultiHistogramFastFitFCN, Base);
MAKE_EXCEPTION(SlimlyOverlappingTemplates,MultiHistogramFastFitFCN);

class MultiHistogramFastFitFCN : public ROOT::Minuit2::FCNBase {
 struct HistogramDetails_t {
  double fTimeOffset, fAmplitudeScale;
  //const TH1D* fTemplateHist; 
  int fNDoF;
  double GetHeight(double t, const TH1* )const;
  double GetError2(double t, const TH1* )const;
 };
 typedef std::vector<HistogramDetails_t> TemplateList;

 public:
  MultiHistogramFastFitFCN(double refine_factor=1);
  ~MultiHistogramFastFitFCN();

  void AddTemplate(/*const TH1D* hist,*/ double time=0){
     HistogramDetails_t tmp;
     //tmp.fTemplateHist=hist;
     tmp.fTimeOffset=time;
     tmp.fNDoF=0;
     fTemplates.push_back(tmp);
  }
  void SetTimeOffset(int n, double offset){
    fTemplates.at(n).fTimeOffset=offset;
  }
  void SetTime(int n, double time){
    fTemplates.at(n).fTimeOffset=time - fTemplateHist->GetMaximumBin();
  }
  void SetAmplitudeScaleFactor(int n, double scale){
    fTemplates.at(n).fAmplitudeScale=scale;
  }

  void Initialise();

  void SetPulseHist(const TH1* pulse){fPulseHist=pulse;}
  void SetRefineFactor(int refine_factor) {fRefineFactor = refine_factor;}
  void SetTemplateHist( const TH1* hist){ fTemplateHist=hist; }
  void SetFitNTemplates(int);
  
  double& GetPedestal()const{return fPedestal;}
  double& GetAmplitudeScaleFactor(int i)const{return fTemplates.at(i).fAmplitudeScale;}
  double& GetTimeOffset(int i)const{return fTemplates.at(i).fTimeOffset;}
  int GetNTemplates()const{return fTemplates.size();}
  TemplateFastFitParameters GetFitParameters()const{
	  return fFitParameters;
  }

  TH1* GetHistTpl()const{return hTpl;}
  TH1* GetHistSum()const{return hSum;}
  TH1* GetHistSumSq()const{return hSumSq;}
  TH1* GetHistSumCross()const{return hSumCross;}
  TH1* GetHistAmplitudes()const{return fHAmplitude;}
  TH1* GetHistPedestals()const{return fHPedestal;}

  void HistogramFittingProcedure(int dims);

  /// @brief Used to calculate the chi-2
  /// @details
  /// The return value is the chi squared
  /// weighted by errors in fTemplateHist
  /// Parameters:
  /// 0. Time of first pulse
  /// 1. Time of second pulse
  /// ....
  /// N Time of Nth pulse
  double operator() (const std::vector<double>& par) const;

  /// Used for error... somehow?
  double Up() const {return 1.;}

  int GetNDoF() { return fNDoF; }
  int GetChi2() { return fChi2; }

  double FitOne(double time_offset)const;
  double FitTwo(double time_offset,double time_offset2)const;


 private:
  inline void GetHistogramBounds(int safety, int &low_edge, int& high_edge)const;

  /// record this for TemplateFitter to retrieve later (NB mutable so that it
  /// can be set in operator(), which is const)
  mutable int fNDoF;
  mutable double fChi2;
  mutable double fPedestal;
  
  int fRefineFactor;
  int fNTemplatesToFit;
  mutable TemplateFastFitParameters fFitParameters;

  mutable TemplateList fTemplates;

  const TH1* fPulseHist; // The histogram to fit
  const TH1* fTemplateHist; // The histogram of the template we will fit with
  TH1 *hTpl, *hSum, *hSumSq, *hSumCross;
  TH1 *fHAmplitude, *fHPedestal;
  bool fHistogramFitPoints;

  // The following vectors get used when we fit things
  // K is used as the index within the vector and will correspond to the offset between the two templates in sample units
  typedef std::vector<double> SamplesVector;
  SamplesVector sum_tpl;       // The sum of all values in the template up-to the k-th entry;
  SamplesVector sum_sq_tpl;    // The sum of squares of all values in the template up-to the k-th entry;
  SamplesVector sum_cross_tpl; // The sum of all cross terms between the templates for the k-th offest

  struct SumMatrix{
    // Matrix representing the inversion of the matrix of summations found on the left side of the solution for the fit parameters
    // It's 3x3 symmetric for fitting two identical templates so we only need 6 values here
    double two_11, two_12, two_13, two_22, two_23, two_33;
    double one_11, one_12, one_22;
    double one_determinant, two_determinant;
  };
  typedef std::vector<SumMatrix> SumMatrices;
  SumMatrices fInvertedSums;

};

inline void MultiHistogramFastFitFCN::GetHistogramBounds(int safety, int &low_edge, int& high_edge)const{
  // init to pulse histogram
  low_edge=safety;
  high_edge=fPulseHist->GetNbinsX()-safety;

  // for each templatint 
  int tpl_start, tpl_stop;
  for(TemplateList::const_iterator i_tpl=fTemplates.begin(); i_tpl!=fTemplates.end(); ++i_tpl){
    tpl_start=i_tpl->fTimeOffset + safety;
    tpl_stop=i_tpl->fTimeOffset+fTemplateHist->GetNbinsX() - safety;

    // find the maximum of all the starts of a template
    if(high_edge>tpl_stop) high_edge=tpl_stop;

    // find the minimum of all the ends of a template
    if(low_edge>tpl_start) low_edge=tpl_start;
  }
}

#endif // MultiHistogramFastFitFCN_h__
