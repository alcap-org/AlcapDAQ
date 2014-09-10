#ifndef MultiHistogramFitFCN_h__
#define MultiHistogramFitFCN_h__

#include "Minuit2/FCNBase.h"
#include "TH1D.h"
#include "AlcapExcept.h"

#include <vector>

MAKE_EXCEPTION(MultiHistogramFitFCN, Base);
MAKE_EXCEPTION(SlimlyOverlappingTemplates,MultiHistogramFitFCN);

class MultiHistogramFitFCN : public ROOT::Minuit2::FCNBase {
 struct HistogramDetails_t {
  double fTimeOffset, fAmplitudeScale;
  const TH1D* fTemplateHist; 
  int fNDoF;
  bool fFixed;
  double GetHeight(double t)const;
  double GetError2(double t)const;
 };
 typedef std::vector<HistogramDetails_t> TemplateList;

 public:
  MultiHistogramFitFCN(double refine_factor=1);
  ~MultiHistogramFitFCN();

  void AddTemplate(const TH1D* hist, double time=0, bool fixed=false){
     HistogramDetails_t tmp;
     tmp.fTemplateHist=hist;
     tmp.fTimeOffset=time;
     tmp.fNDoF=0;
     tmp.fFixed=fixed;
     fTemplates.push_back(tmp);
  }
  void SetTemplateHist( int n , const TH1D* hist){
    fTemplates.at(n).fTemplateHist=hist;
  }
  void SetTime(int n, double time){
    fTemplates.at(n).fTimeOffset=time - fTemplates[n].fTemplateHist->GetMaximumBin();
  }
  void SetAmplitude(int n, double amp){
    fTemplates.at(n).fAmplitudeScale=amp/fTemplates[n].fTemplateHist->GetMaximum();
  }
  void SetInitialValues(int n, double time_offset, double amp){
    SetTime(n, time_offset);
    SetAmplitude(n, amp);
  }
  void FixTemplate(int n, bool fix=true){
    fTemplates.at(n).fFixed=fix;
  }
  void FixAllTemplatesBut(int n, bool fix=true){
    for(TemplateList::iterator i_hist=fTemplates.begin();
        i_hist!=fTemplates.end(); ++i_hist){
       if(i_hist- fTemplates.begin() == n) continue;
       i_hist->fFixed=fix;
    }
  }

  void SetPulseHist(const TH1D* pulse){fPulseHist=pulse;}
  void SetRefineFactor(int refine_factor) {fRefineFactor = refine_factor;}

  /// @brief Used to calculate the chi-2
  /// @details
  /// The return value is the chi squared
  /// weighted by errors in fTemplateHist
  /// Parameters:
  /// 0. Pedestal 
  /// 1. Amplitude of first pulse
  /// 2. Amplitude of second pulse
  /// ....
  /// N Amplitude of Nth pulse
  double operator() (const std::vector<double>& par) const;

  /// Used for error... somehow?
  double Up() const {return 1.;}

  int GetNDoF() { return fNDoF; }

 private:
  inline void UnpackParameters(const std::vector<double>& par)const;
  inline void GetHistogramBounds(int safety, int &low_edge, int& high_edge)const;

  /// record this for TemplateFitter to retrieve later (NB mutable so that it
  /// can be set in operator(), which is const)
  mutable int fNDoF;
  mutable double fChi2;
  mutable double fPedestal;
  
  int fRefineFactor;

  mutable TemplateList fTemplates;

  const TH1D* fPulseHist; // The histogram to fit

};

inline void MultiHistogramFitFCN::GetHistogramBounds(int safety, int &low_edge, int& high_edge)const{
  // init to pulse histogram
  low_edge=safety;
  high_edge=fPulseHist->GetNbinsX()-safety;

  // for each templatint 
  int tpl_start, tpl_stop;
  for(TemplateList::const_iterator i_tpl=fTemplates.begin(); i_tpl!=fTemplates.end(); ++i_tpl){
    tpl_start=i_tpl->fTimeOffset + safety;
    tpl_stop=i_tpl->fTimeOffset+i_tpl->fTemplateHist->GetNbinsX() - safety;

    // find the maximum of all the starts of a template
    if(high_edge>tpl_stop) high_edge=tpl_stop;

    // find the minimum of all the ends of a template
    if(low_edge>tpl_start) low_edge=tpl_start;
  }
}

inline void MultiHistogramFitFCN::UnpackParameters(const std::vector<double>& par)const{
    if ( par.size() < fTemplates.size()+1) return;
    const std::vector<double>::const_iterator begin=par.begin();
    std::vector<double>::const_iterator i_par=begin;
    fPedestal=*i_par;
    for( ; i_par!=par.end(); ++i_par){
      int n= (i_par-begin+1) ;
      fTemplates.at(n).fAmplitudeScale=*i_par;
    }
  }

#endif
