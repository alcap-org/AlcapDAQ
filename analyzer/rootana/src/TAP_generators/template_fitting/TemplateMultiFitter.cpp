#include "TemplateMultiFitter.h"
#include "MultiHistogramFitFCN.h"
#include "SetupNavigator.h"
#include "EventNavigator.h"
#include "debug_tools.h"
#include <iostream>

#include "TMath.h"

using std::cout;
using std::endl;

TemplateMultiFitter::TemplateMultiFitter(const IDs::channel& ch): 
   fMinuitFitter(NULL), fFitFCN(NULL), fChannel(ch),fPedestal(0), fRefineFactor(0) {

  // Calculate the max ADC value
  fMinADC = EventNavigator::Instance().GetSetupRecord().GetMaxADC(fChannel);
  fMaxADC = EventNavigator::Instance().GetSetupRecord().GetMaxADC(fChannel);
}

void TemplateMultiFitter::Init(){
  if(fFitFCN || fMinuitFitter) {
     cout<< "TemplateMultiFitter: Warning: trying to initialize fitter which was already initialised!"<<endl;
     return;
  }

  fFitFCN=new MultiHistogramFitFCN(fRefineFactor);
  fFitFCN->SetRefineFactor(fRefineFactor);
  for(TemplateList::iterator i_tpl=fTemplates.begin(); i_tpl!=fTemplates.end(); ++i_tpl){
     fFitFCN->AddTemplate(i_tpl->fTemplate->GetHisto());
  }
 
  int num_params=fTemplates.size()+1; //  1 parameter per template (amplitude) + global pedestal.
  fMinuitFitter = new TFitterMinuit(num_params);
  fMinuitFitter->SetMinuitFCN(fFitFCN);
  fMinuitFitter->SetPrintLevel(-1); // set the debug level to quiet (-1=quiet, 0=normal, 1=verbose)
}

TemplateMultiFitter::~TemplateMultiFitter() {
}

int TemplateMultiFitter::FitWithOneTimeFree(int index, const TH1D* hPulse){

  // Get the template we're going to shift around
  TemplateDetails_t& current=fTemplates.at(index);

  // Prepare for minimizations
  fMinuitFitter->Clear();
  fFitFCN->SetPulseHist(hPulse);
  fMinuitFitter->CreateMinimizer(TFitterMinuit::kMigrad);

  // Loop through some time offsets ourselved
  const double offset_range = 10*fRefineFactor; // maximum distance to go from the initial estimate
  double best_time_offset = 0;
  std::vector<double> best_parameters(1+fTemplates.size());
  double best_chi2 = 1e11;
  int best_ndof = 0;
  int best_status = kInitialised;
  int status = kInitialised;

  // Calculate the bounds of the parameters
  int min_offset = current.fTimeOffset - offset_range;
  int max_offset = current.fTimeOffset + offset_range;

  std::vector<double> parameters(1+fTemplates.size());
  for (double time_offset = min_offset; time_offset <= max_offset; ++time_offset) {
    fFitFCN->SetTimeOffset(index, time_offset);

    // Reset the estimates
    fMinuitFitter->SetParameter(0, "PedestalOffset", fPedestal, 0.1, fMinADC, fMaxADC);
    for(TemplateList::const_iterator i_tpl=fTemplates.begin(); i_tpl!=fTemplates.end(); ++i_tpl){
        int i=i_tpl-fTemplates.begin();
        fMinuitFitter->SetParameter(i+1, Form("AmplitudeScaleFactor_%d",i), i_tpl->fAmplitudeScaleFactor, 0.1, fMinADC, fMaxADC);
    }

    // Minimize and notify if there was a problem
    status = fMinuitFitter->Minimize(1000); // set limit of 1000 calls to FCN
    if(status!=0) continue;

    // Store the Chi2 and degrees of freedom
    parameters[0]=fMinuitFitter->GetParameter(0); 
    for(std::vector<double>::iterator i_par=parameters.begin()+1; i_par!=parameters.end(); ++i_par){
       int n=i_par-parameters.begin();
       *i_par= fMinuitFitter->GetParameter(n);
    }

    // Check the bounds
    const double delta_ped_offset_error = 1; // if the given parameter is within this much of the boundaries, then we will ignore it
    const double delta_amp_sf_error = 0.001;
    if ( (parameters[0] < delta_ped_offset_error ) || (parameters[0] > fMaxADC - delta_ped_offset_error) ) {
      status = kPedestalOutOfBounds;
    } else {
       for(std::vector<double>::const_iterator i_par=parameters.begin()+1; i_par!=parameters.end(); ++i_par){
          if (    ( *i_par > fMinADC + delta_amp_sf_error ) || ( *i_par < fMaxADC - delta_amp_sf_error ) ) {
                status = kAmplitudeOutOfBounds;
                break;
          }
       }
    }

    // get the chi-2
    fChi2 = (*fFitFCN)(parameters);
    fNDoF = fFitFCN->GetNDoF();

    if (status == 0 && fChi2 < best_chi2) {
      best_time_offset = time_offset;
      best_parameters=parameters;
      best_chi2 = fChi2;
      best_status = status;
      best_ndof = fNDoF;
    }
  }

  // Store the final best values
  fChi2 = best_chi2;
  fNDoF = best_ndof;
  current.fTimeOffset = best_time_offset;
  fPedestal = best_parameters[0];
  for(TemplateList::iterator i_tpl=fTemplates.begin(); i_tpl!=fTemplates.end(); ++i_tpl){
      i_tpl->fAmplitudeScaleFactor=best_parameters[i_tpl-fTemplates.begin()+1];
  }

  static int offset_safety = 1;
  if ( (current.fTimeOffset < min_offset + offset_safety && current.fTimeOffset > min_offset - offset_safety) ||
       (current.fTimeOffset < max_offset + offset_safety && current.fTimeOffset > max_offset - offset_safety) ) {

      best_status = kTimeOutOfBounds;
  }

  return best_status; // return status for the calling module to look at
}

int TemplateMultiFitter::FitWithAllTimesFixed( const TH1D* hPulse){
  // Prepare for minimizations
  fMinuitFitter->Clear();
  fFitFCN->SetPulseHist(hPulse);
  fMinuitFitter->CreateMinimizer(TFitterMinuit::kMigrad);

  // initialise fit
  fMinuitFitter->SetParameter(0, "PedestalOffset", fPedestal, 0.1, fMinADC, fMaxADC);
  for(TemplateList::const_iterator i_tpl=fTemplates.begin(); i_tpl!=fTemplates.end(); ++i_tpl){
      int i=i_tpl-fTemplates.begin();
      fMinuitFitter->SetParameter(i+1, Form("AmplitudeScaleFactor_%d",i), i_tpl->fAmplitudeScaleFactor, 0.1, fMinADC, fMaxADC);
      fFitFCN->SetTimeOffset(i, i_tpl->fTimeOffset);
  }

  // run the fitter
  int status = fMinuitFitter->Minimize(1000); 

  // get fitted parameters
  std::vector<double> parameters(1+fTemplates.size());
  parameters[0]=fPedestal=fMinuitFitter->GetParameter(0); 
  for(TemplateList::iterator i_tpl=fTemplates.begin(); i_tpl!=fTemplates.end(); ++i_tpl)
     parameters[i_tpl-fTemplates.begin()+1]=i_tpl->fAmplitudeScaleFactor= fMinuitFitter->GetParameter(i_tpl-fTemplates.begin()+1);

  // check chi-2
  fChi2 = (*fFitFCN)(parameters);
  fNDoF = fFitFCN->GetNDoF();

  // return status
  return status;

}

void TemplateMultiFitter::AddTemplate(TTemplate* tpl){
  TemplateDetails_t tmp;
  tmp.fTemplate=tpl;
  fTemplates.push_back(tmp);
  if(fRefineFactor==0) fRefineFactor=tpl->GetRefineFactor();
  else if(fRefineFactor!= tpl->GetRefineFactor()){
    throw Except::MismatchedTemplateRefineFactors();
  }
}
