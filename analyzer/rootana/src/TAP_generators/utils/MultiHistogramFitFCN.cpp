#include "MultiHistogramFitFCN.h"

#include <cmath>
#include <iostream>
#include <cassert>

MultiHistogramFitFCN::MultiHistogramFitFCN(double refine_factor):
  fRefineFactor(refine_factor){
}

MultiHistogramFitFCN::~MultiHistogramFitFCN() {
}

double MultiHistogramFitFCN::HistogramDetails_t::GetHeight(double t)const{
    int bin=t - fTimeOffset;
    double remainder=t - fTimeOffset - bin;
    assert( remainder<1 );
    double y0 = fTemplateHist->GetBinContent(bin);
    double y1 = fTemplateHist->GetBinContent(bin+1);
    return (y0 + remainder*((y1-y0)))*fAmplitudeScale;
}

double MultiHistogramFitFCN::HistogramDetails_t::GetError2(double t)const{
    int bin= t - fTimeOffset;
    double error= fTemplateHist->GetBinError(bin);
    return error*error;
}

double MultiHistogramFitFCN::operator() (const std::vector<double>& par) const {
  // Chi2 fit with pedestal P, amplitude A, and timing T
  // Warning: The time is truncated to an int, so if there's
  // so if the step size in MINUIT is smalled than 1,
  // Any value of T will probably be seen as minimized, which it
  // almost certainly will not be.
  UnpackParameters(par);
  double chi2 = 0.;

  int safety = 6*fRefineFactor; // remove a few bins from the fit
  int bounds[2];
  GetHistogramBounds(safety,bounds[0], bounds[1]);
  
  if( (bounds[1]-bounds[0]) < 40*fRefineFactor ) throw Except::SlimlyOverlappingTemplates();

  // Calculate the degrees of freedom ( #data - #fit_params)
  // +1 because we include both ends of the bounds when we loop through
  fNDoF = ((bounds[1] - bounds[0] + 1)/fRefineFactor) - par.size(); 

  double tpl_height,tpl_error;
  for (int i = bounds[0]+(fRefineFactor/2.0); i <= bounds[1]-(fRefineFactor/2.0); i += fRefineFactor) { 
    // calculate the chi^2 based on the centre of the 5 bins to avoid getting
    // abonus from mathcing all 5.  We shift and scale the template so that it
    // matches the pulse.  This is because, when we have a normalised template,
    // we will get the actual amplitude, pedestal and time from the fit and not
    // just offsets
    tpl_height=0;
    tpl_error=0;
    
    for(TemplateList::const_iterator i_tpl=fTemplates.begin(); 
         i_tpl!=fTemplates.end(); ++i_tpl){
       tpl_height+=i_tpl->GetHeight(i);
       tpl_error+=i_tpl->fAmplitudeScale*i_tpl->GetError2(i);
    }

    double delta = fPulseHist->GetBinContent(i) - tpl_height;
    chi2 += delta*delta / tpl_error;
  }

  return chi2;
}
