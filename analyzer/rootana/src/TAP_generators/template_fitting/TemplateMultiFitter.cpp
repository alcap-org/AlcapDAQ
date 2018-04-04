#define ALCAP_NO_DEBUG
#include "TemplateMultiFitter.h"
#include "MultiHistogramFitFCN.h"
#include "SetupNavigator.h"
#include "EventNavigator.h"
#include "debug_tools.h"
#include <iostream>

#include "TMath.h"

using std::cout;
using std::endl;

TemplateMultiFitter::TemplateMultiFitter(const IDs::channel& ch, int refine_factor): 
   fMinuitFitter(NULL), fFitFCN(NULL), fChannel(ch),fPedestal(0), fRefineFactor(refine_factor) {

  // Calculate the max ADC value
  fMinADC = 0;
  int n_bits = TSetupData::Instance()->GetNBits(TSetupData::Instance()->GetBankName(fChannel.str()));
  fMaxADC = std::pow(2, n_bits)*10;
}

void TemplateMultiFitter::Init(){
  if(fFitFCN || fMinuitFitter) {
    cout<< "TemplateMultiFitter: Warning: trying to initialize fitter which was already initialised!"<<endl;
    return;
  }

  //  fFitFCN=new MultiHistogramFitFCN(fRefineFactor);
  fFitFCN=new MultiHistogramFitFCN(fRefineFactor);
  fFitFCN->SetRefineFactor(fRefineFactor);
  for(TemplateList::iterator i_tpl=fTemplates.begin(); i_tpl!=fTemplates.end(); ++i_tpl){
     fFitFCN->AddTemplate(i_tpl->fTemplate.GetHisto());
  }
 
  int num_params=fTemplates.size()+1; //  1 parameter per template (amplitude) + global pedestal.
  fMinuitFitter = new TFitterMinuit(num_params);
  //  fMinuitFitter = new BrentDeckerMinimizer(num_params);
  fMinuitFitter->SetMinuitFCN(fFitFCN);
  fMinuitFitter->SetPrintLevel(-1); // set the debug level to quiet (-1=quiet, 0=normal, 1=verbose)

  fChi2 = 0;
}

void TemplateMultiFitter::Reset() {
  if (fMinuitFitter) {
    delete fMinuitFitter;
    fMinuitFitter = NULL;
    fFitFCN = NULL;
  }
  fTemplates.clear();
}

TemplateMultiFitter::~TemplateMultiFitter() {
  if (fMinuitFitter) {
    delete fMinuitFitter;
  }
  fTemplates.clear();
}

int TemplateMultiFitter::FitWithOneTimeFree(int index, const TH1D* hPulse, double offset_range){

  PrepareForMinimizations(hPulse);
  ResetFitValues();

  TemplateDetails_t& current = fTemplates.at(index);
  //  double offset_range = 5*fRefineFactor;
  TimeOffsetLoop(current, offset_range);
  StoreFinalBestValues(current);
  
  return fBestValues.status; // return status for the calling module to look at
}

int TemplateMultiFitter::FitWithAllTimesFixed( const TH1D* hPulse){
  PrepareForMinimizations(hPulse);
  ResetFitValues();

  TemplateDetails_t& current = fTemplates.at(0); // just pass it the first template
  double offset_range = 0;
  TimeOffsetLoop(current, offset_range);
  StoreFinalBestValues(current);

  return fBestValues.status;
}


void TemplateMultiFitter::PrepareForMinimizations(const TH1D* hPulse) {
  // Prepare for minimizations
  fMinuitFitter->Clear();
  fFitFCN->SetPulseHist(hPulse);
  fMinuitFitter->CreateMinimizer(TFitterMinuit::kMigrad);
}

void TemplateMultiFitter::ResetFitValues() {
  fCurrentValues.time_offset = 0;
  fCurrentValues.parameters.resize(1+fTemplates.size());
  fCurrentValues.status = kInitialised;

  fBestValues.time_offset = 0;
  fBestValues.parameters.reserve(1+fTemplates.size());
  fBestValues.chi2 = 1e34;
  fBestValues.ndof = 0;
  fBestValues.status = kInitialised;
}


void TemplateMultiFitter::TimeOffsetLoop(TemplateDetails_t& current_template, double offset_range) {

  // Loop through some time offsets ourselves

  // Set the initial time offsets for all the tempaltes
  for(TemplateList::iterator i_tpl=fTemplates.begin(); i_tpl!=fTemplates.end(); ++i_tpl){
    fFitFCN->SetTimeOffset(i_tpl-fTemplates.begin(), i_tpl->fTimeOffset);
  }

  // Calculate the bounds of the time offset
  int min_offset = current_template.fTimeOffset - offset_range;
  int max_offset = current_template.fTimeOffset + offset_range;
  // Loop through the time offsets from the middle out
  int mid_offset = current_template.fTimeOffset;
  int counter = 0;
  //  std::cout << "Start (min = " << min_offset << ", max = " << max_offset << ", mid = " << mid_offset << ")"<< std::endl;
  for (double time_offset = mid_offset; time_offset <= max_offset && time_offset >= min_offset; time_offset += std::pow(-1,counter%2)*counter) {
    //    std::cout << "TemplateMultiFitter::SingleTimeOffsetLoop(): Checking Time Offset = " << time_offset << "\n";
    fMinuitFitter->Clear();
    fMinuitFitter->CreateMinimizer(TFitterMinuit::kMigrad);
    fFitFCN->SetTimeOffset(current_template.fIndex, time_offset);

    FitThisTime(current_template, time_offset); // try to fit with this time
    ++counter;
    /*    std::cout << "TemplateMultiFitter::SingleTimeOffsetLoop(): Current Values:\tStatus = " << fCurrentValues.status << "\tChi2 = " << fCurrentValues.chi2 << "\tPedOffset = "
	      << fCurrentValues.parameters[0] << " Time Offset = " << fCurrentValues.time_offset << "\n";
    for(TemplateList::iterator i_tpl=fTemplates.begin(); i_tpl!=fTemplates.end(); ++i_tpl){
      int n = i_tpl - fTemplates.begin();
      std::cout << "\t\tTemplate #" << n << ": Amp SF = " << fCurrentValues.parameters[n+1] << "\tT " << i_tpl->fTemplate.GetTime()+fCurrentValues.time_offset << std::endl;
    }
    */
  }

  // If we actually changed the time,
  // make sure the time is within bounds
  if (offset_range > 0) {
    static int offset_safety = 1;

    if ( (fBestValues.time_offset < min_offset + offset_safety && fBestValues.time_offset > min_offset - offset_safety) ||
	 (fBestValues.time_offset < max_offset + offset_safety && fBestValues.time_offset > max_offset - offset_safety) ) {
      
      fBestValues.status = kTimeOutOfBounds;
    }
  }
}

void TemplateMultiFitter::FitThisTime(TemplateDetails_t& current_template, double time_offset) {

  fCurrentValues.time_offset = time_offset;
  // Reset the estimates for the non-time offset parameters
  double ped_offset = fPedestal - current_template.fTemplate.GetPedestal();
  double min_ped_offset = ped_offset-500;
  double max_ped_offset = ped_offset+500;
  fMinuitFitter->SetParameter(0, "PedestalOffset", ped_offset, 0.1, min_ped_offset, max_ped_offset);
  for(TemplateList::const_iterator i_tpl=fTemplates.begin(); i_tpl!=fTemplates.end(); ++i_tpl){
    int i=i_tpl-fTemplates.begin();
    fMinuitFitter->SetParameter(i+1, Form("AmplitudeScaleFactor_%d",i), i_tpl->fAmplitudeScaleFactor, 0.1, fMinADC, fMaxADC);
  }

  // Minimize and notify if there was a problem
  fCurrentValues.status = fMinuitFitter->Minimize(1000); // set limit of 1000 calls to FCN
  DEBUG_VALUE(fCurrentValues.status);
  if(fCurrentValues.status!=0) return;

  // Store the Chi2 and degrees of freedom
  fCurrentValues.parameters[0]=fMinuitFitter->GetParameter(0); 
  for(std::vector<double>::iterator i_par=fCurrentValues.parameters.begin()+1; i_par!=fCurrentValues.parameters.end(); ++i_par){
    int n=i_par-fCurrentValues.parameters.begin();
    *i_par= fMinuitFitter->GetParameter(n);
  }

  // Check the bounds
  const double delta_ped_offset_error = 1; // if the given parameter is within this much of the boundaries, then we will ignore it
  const double delta_amp_sf_error = 0.001;

  if ( (fCurrentValues.parameters[0] < min_ped_offset+delta_ped_offset_error ) || (fCurrentValues.parameters[0] > max_ped_offset - delta_ped_offset_error) ) {
    fCurrentValues.status = kPedestalOutOfBounds;
  } else {
    for(std::vector<double>::const_iterator i_par=fCurrentValues.parameters.begin()+1; i_par!=fCurrentValues.parameters.end(); ++i_par){
      if (    ( *i_par < fMinADC + delta_amp_sf_error ) || ( *i_par > fMaxADC - delta_amp_sf_error ) ) {
	DEBUG_VALUE(*i_par, delta_amp_sf_error, fMinADC, fMaxADC);
	fCurrentValues.status = kAmplitudeOutOfBounds;
	break;
      }
    }
  }

  // get the chi-2
  fCurrentValues.chi2 = (*fFitFCN)(fCurrentValues.parameters);
  fCurrentValues.ndof = fFitFCN->GetNDoF();
  DEBUG_VALUE(fCurrentValues.status,fCurrentValues.parameters[0],fCurrentValues.chi2,fCurrentValues.ndof);

  // if this is the best fit yet, then store it
  //  if (fCurrentValues.status == 0 && fCurrentValues.chi2 < fBestValues.chi2) {
  if (fCurrentValues.chi2 < fBestValues.chi2) {
    fBestValues.time_offset = fCurrentValues.time_offset;
    fBestValues.parameters=fCurrentValues.parameters;
    fBestValues.chi2 = fCurrentValues.chi2;
    fBestValues.status = fCurrentValues.status;
    fBestValues.ndof = fCurrentValues.ndof;
  }

  /*
  std::cout << "TemplateMultiFitter::FitThisTime(): Current Values:\tStatus = " << fCurrentValues.status << "\tChi2 = " << fCurrentValues.chi2 << "\tPedOffset = "
	      << fCurrentValues.parameters[0] << " Time Offset = " << fCurrentValues.time_offset << "\n";
  for(TemplateList::iterator i_tpl=fTemplates.begin(); i_tpl!=fTemplates.end(); ++i_tpl){
    int n = i_tpl - fTemplates.begin();
    std::cout << "\t\tTemplate #" << n << ": Amp SF = " << fCurrentValues.parameters[n+1] << "\tT " << i_tpl->GetTime() << std::endl;
  }
  */
}

void TemplateMultiFitter::StoreFinalBestValues(TemplateDetails_t& current_template) {

  // Store the final best values
  fChi2 = fBestValues.chi2;
  fNDoF = fBestValues.ndof;
  current_template.fTimeOffset = fBestValues.time_offset;
  fPedestal = current_template.fTemplate.GetPedestal() + fBestValues.parameters[0];
  for(TemplateList::iterator i_tpl=fTemplates.begin(); i_tpl!=fTemplates.end(); ++i_tpl){
      i_tpl->fAmplitudeScaleFactor=fBestValues.parameters[i_tpl-fTemplates.begin()+1];
  }

  /*  std::cout << "TemplateMultiFitter::StoreFinalBestValues(): Best Values:\tStatus = " << fBestValues.status << "\tChi2 " << fChi2 << "\tP "
	    << fPedestal << " (offset = " << fBestValues.parameters[0] << ")\n";
  for(TemplateList::iterator i_tpl=fTemplates.begin(); i_tpl!=fTemplates.end(); ++i_tpl){
    std::cout << "Template #" << i_tpl - fTemplates.begin() << ": Amp " << i_tpl->fTemplate.GetAmplitude()*i_tpl->fAmplitudeScaleFactor << " (SF = " <<i_tpl->fAmplitudeScaleFactor << ")" << "\tTime " << i_tpl->GetTime() << " (Pulse Offset = " << i_tpl->fTimeOffset << ")" << std::endl;
  }
  std::cout << "ndof = " << fNDoF << ", prob = " << TMath::Prob(fChi2, fNDoF) << std::endl;
  */
}


void TemplateMultiFitter::AddTemplate(TTemplate* tpl){
  TemplateDetails_t tmp;
  tmp.fTemplate=*tpl;
  tmp.fIndex = fTemplates.size();
  fTemplates.push_back(tmp);
  if(fRefineFactor==0) fRefineFactor=tpl->GetRefineFactor();
  else if(fRefineFactor!= tpl->GetRefineFactor()){
    throw Except::MismatchedTemplateRefineFactors();
  }
}

bool TemplateMultiFitter::FindLargeResidual(const TH1D* hPulse, double& amplitude, double& time) {

  double max_delta = 0;
  for (int i_bin = 1; i_bin < hPulse->GetNbinsX(); ++i_bin) {
    double delta = fFitFCN->CalculateDelta(i_bin);
    //    std::cout << i_bin << " " << delta << std::endl;
    if (delta > max_delta && std::fabs(i_bin-hPulse->GetMaximumBin()) > 20) {
      max_delta = delta;
      amplitude = hPulse->GetBinContent(i_bin);
      time = hPulse->GetBinCenter(i_bin);
    }    
  }

  if (max_delta > 100) {
    return true;
  }
  else {
    return false;
  }
}
