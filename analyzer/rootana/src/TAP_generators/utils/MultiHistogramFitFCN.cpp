#include "MultiHistogramFitFCN.h"

#include <cmath>
#include <iostream>
#include <cassert>
#include <iomanip>

MultiHistogramFitFCN::MultiHistogramFitFCN(double refine_factor):
  fRefineFactor(refine_factor){
}

MultiHistogramFitFCN::~MultiHistogramFitFCN() {
}

double MultiHistogramFitFCN::HistogramDetails_t::GetHeight(double template_bin)const{
  int bin = template_bin;
  double remainder=template_bin - bin; //???
  //  double remainder=t - fTimeOffset - bin;
    assert( remainder<1 );
    double y0 = fTemplateHist->GetBinContent(template_bin);
    double y1 = fTemplateHist->GetBinContent(template_bin+1);
    if (template_bin < 1 || template_bin > fTemplateHist->GetNbinsX()) { // if we go out of the range of the template,
      y0 = y1 = fPedestal; // assume it is at its pedestal
    }
    double height = ((y0 + remainder*((y1-y0))) - fPedestal)*fAmplitudeScale + fPedestal;
    //    std::cout << "(template) bin #" << template_bin << " ((" << y0 << " + " << remainder*((y1-y0)) << ") - " << fPedestal << ")*" << fAmplitudeScale << " + " << fPedestal << " = " << height << std::endl;
    return height;
}

double MultiHistogramFitFCN::HistogramDetails_t::GetError2(double template_bin)const{
    double error= fTemplateHist->GetBinError(template_bin);
    if (template_bin < 1 || template_bin > fTemplateHist->GetNbinsX()) {
      error = fTemplateHist->GetBinError(1); // assume it's the same error as in bin 1
    }
    //    std::cout << "AE2: Error @bin " << template_bin << " = " << error << std::endl;
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

  int safety = 0*fRefineFactor; // remove a few bins from the fit
  int bounds[2];
  GetHistogramBounds(safety,bounds[0], bounds[1]);

  if( (bounds[1]-bounds[0]) < 40*fRefineFactor ) throw Except::SlimlyOverlappingTemplates();

  // Calculate the degrees of freedom ( #data - #fit_params)
  // +1 because we include both ends of the bounds when we loop through
  fNDoF = 0;
  //  fNDoF = ((bounds[1] - bounds[0] + 1)/fRefineFactor) - par.size(); 
  //  fNDoF = (fPulseHist->GetNbinsX()/fRefineFactor) - par.size(); 

  double tpl_height,tpl_error2;
  //  std::cout << "Pulse" << std::setw(7) << "\tPedOffset" << std::setw(7) << "\tTemplate" << std::setw(7) << "\tDelta" << std::setw(7) << "\tError^2" << std::setw(7) << "\tThisChi2" << std::setw(7) << "\tTotalChi2" << std::endl;
  for (int i_global_bin = bounds[0]; i_global_bin <= bounds[1]; i_global_bin += fRefineFactor) { 
    // calculate the chi^2 based on the bin in the centre of the 5 refined bins to avoid getting
    // abonus from mathcing all 5 (AE: not really sure that this means).  We shift and scale the template so that it
    // matches the pulse.  This is because, when we have a normalised template,
    // we will get the actual amplitude, pedestal and time from the fit and not
    // just offsets
    tpl_height=0;
    tpl_error2=0;

    int i_pulse_bin = i_global_bin;

    // The bounds have been defined relative to the pulse histogram but includes regions where the template exists but the pulse doesn't
    double pulse_sample_value = fPulseHist->GetBinContent(i_pulse_bin); // includes pedestal
    if (i_pulse_bin < 1 || i_pulse_bin > fPulseHist->GetNbinsX()) {
      continue; // can't assume that the pulse is nice and goes to exactly the pedestal at either end
    }
    
    
    for(TemplateList::const_iterator i_tpl=fTemplates.begin(); 
         i_tpl!=fTemplates.end(); ++i_tpl){
      int i_template_bin = i_global_bin - i_tpl->fTimeOffset;

      double this_tpl_height = i_tpl->GetHeight(i_template_bin);
      double this_tpl_error2 = i_tpl->fAmplitudeScale*i_tpl->fAmplitudeScale*i_tpl->GetError2(i_global_bin);
      //      std::cout << "AE2: " << this_tpl_height << " (" << i_template_bin << ") ";
      tpl_height+=this_tpl_height;
      tpl_error2+=this_tpl_error2; // need to add uncertainty in the amplitude scale factor?
    }
    //    std::cout << std::endl;
    //    std::cout << "tpl_height (pre pedestal) = " << tpl_height << std::endl;
    tpl_height += fPedestalOffset; // move to the pulse's pedestal
    double delta = pulse_sample_value - tpl_height;
    if (tpl_error2 != 0) {
      chi2 += (delta*delta) / tpl_error2;
    }
    ++fNDoF;
    //    std::cout << pulse_sample_value << std::setw(7) << " " << fPedestalOffset << std::setw(7) << " " << tpl_height << std::setw(7) << " " << delta << std::setw(7) << " " << tpl_error2 << std::setw(7) << " " << (delta*delta) / tpl_error2 << " " << std::setw(7) << chi2 << std::endl;
  }
  fNDoF -= par.size();
  
  /*  std::cout << "AE2: " << fPedestalOffset << " ";
  for(TemplateList::const_iterator i_tpl=fTemplates.begin(); 
      i_tpl!=fTemplates.end(); ++i_tpl){
    std::cout << i_tpl->fAmplitudeScale << " ";
  }
  std::cout << chi2 << " " << fNDoF << std::endl;
  */
    
  return chi2;
}
