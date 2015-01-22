#include "TTemplate.h"
#include "debug_tools.h"

#include <cmath>
#include <iostream>
using std::cout;
using std::endl;

TTemplate::TTemplate():
  fDebug(false),
  fConverged(false),
  fTotalPulses(0),
  fRefineFactor(0),
  fTriggerPolarity(0),
  fChannel(),
  fErrors(NULL),
  fTemplatePulse(NULL){
}

TTemplate::TTemplate(const std::string& det,int refine,int trigger_polarity, bool debug):
  fDebug(debug),
  fConverged(false),
  fIsRebinnedToOriginalBinning(false),
  fTotalPulses(0),
  fRefineFactor(refine),
  fTriggerPolarity(trigger_polarity),
  fChannel(det),
  fName(MakeName(fChannel)),
  fTemplatePulse(NULL){

       // Setup the error hist
       std::string error_histname = "hErrorVsPulseAdded_" + fChannel.str();
       std::string error_histtitle = "Plot of the Error as each new Pulse is added to the template for the " + fChannel.str() + " channel";
       int n_bins = 10000;
       fErrors = new TH1D(error_histname.c_str(), error_histtitle.c_str(), n_bins,0,n_bins);
}

TTemplate::~TTemplate(){
}

void TTemplate::Initialize(int pulseID, TH1D* pulse, TDirectory* dir){
  
  fTemplatePulse=pulse;
  fTemplatePulse->SetBit(TH1::kIsAverage);
  ++fTotalPulses;
}

void TTemplate::AddPulse(double x_offset, double y_scale, double y_offset, const TH1D* hPulse){

  if (fDebug) {
    std::cout << "TTemplate::AddPulse(): # pulses = " << fTotalPulses << std::endl;
  }

  double template_pedestal = fTemplatePulse->GetBinContent(1);
  double total_error=0;

  // Loop through the pulse histogram
  for (int iPulseBin = 1; iPulseBin < hPulse->GetNbinsX(); ++iPulseBin) {

    // First, get the corrected sample value based on the fitted parameters
    double uncorrected_value = hPulse->GetBinContent(iPulseBin);
    double corrected_value = uncorrected_value - y_offset;
    corrected_value /= y_scale;
    corrected_value += template_pedestal;

    // Get the corrected bin number based on the fitted time offset We add 0.5
    // so that we round to the nearest integer and then subtract time offset
    // because this value might not want to go directly into the template
    int bin_number = iPulseBin + 0.5 - x_offset; 

    // Only change the bin contents of bins within the range of the template histogram
    if (bin_number < 1 || bin_number > fTemplatePulse->GetNbinsX()) {
      continue;
    }

    // Store the old bin content and error for later
    double old_bin_content = fTemplatePulse->GetBinContent(bin_number);
    double old_bin_error = fTemplatePulse->GetBinError(bin_number);

    // Calculate the new bin content...
    double new_bin_content = fTotalPulses * old_bin_content + corrected_value;
    new_bin_content /= (fTotalPulses + 1);

    /// ...and new bin error
    double new_bin_error = ((fTotalPulses - 1)*old_bin_error*old_bin_error)
                         + (corrected_value - old_bin_content)*(corrected_value - new_bin_content);
    new_bin_error = std::sqrt(new_bin_error / fTotalPulses);

    // increment total error
    total_error+=new_bin_error;

    if (fDebug) {
      cout << "TemplateCreator::AddPulseToTemplate(): Bin #" << bin_number 
           << ": Corrected Sample Value = " << corrected_value << endl
    	   << "\t\t\tOld Value (Error) = " << old_bin_content << "(" << old_bin_error << ")" << endl
    	   << "\t\t\tNew Value (Error) = " << new_bin_content << "(" << new_bin_error << ")" << endl;
    }

    // Set the new bin content and error of the template
    fTemplatePulse->SetBinContent(bin_number, new_bin_content);
    fTemplatePulse->SetBinError(bin_number, new_bin_error);
  }

  // increment the total number of pulses
  ++fTotalPulses;

  // fill the error histoo
  fErrors->Fill(fTotalPulses, total_error);
}

bool TTemplate::CheckConverged(){
  if(fConverged) return true;

  // Check the difference between this iteration and previous ones and, if it's small, the template has converged
  int n_bins_to_check = 10;
  if (fTotalPulses<n_bins_to_check) return false;
  double convergence_limit = 0.1;
  int newest_bin=fErrors->FindBin(fTotalPulses);
  double error=fErrors->GetBinContent(newest_bin)/fTotalPulses;
  for (int iPrevBin = 0; iPrevBin < n_bins_to_check; ++iPrevBin) {
    double previous_error = fErrors->GetBinContent(newest_bin-iPrevBin);
    previous_error /= fTotalPulses- iPrevBin;

    if ( std::fabs(previous_error - error) > convergence_limit) {
       fConverged=false;
       break;
    } else{
       fConverged=true;
    }
  }

  return fConverged;
}

void TTemplate::NormaliseToAmplitude(){
    if(!fTemplatePulse) return;
    fTemplatePulse->SetBit(TH1::kIsAverage);
    SubtractPedestal();

    double norm = std::fabs(fTemplatePulse->GetMaximum()); 
    ScaleHist(1./norm);
}

void TTemplate::NormaliseToIntegral(){
    if(!fTemplatePulse) return;
    fTemplatePulse->SetBit(TH1::kIsAverage);
    SubtractPedestal();

    double norm = fTemplatePulse->Integral(); 
    ScaleHist(1./norm);
}

void TTemplate::NormaliseToSumSquares(){
    if(!fTemplatePulse) return;
    fTemplatePulse->SetBit(TH1::kIsAverage);
    SubtractPedestal();

    TH1D* tmp = (TH1D*) fTemplatePulse->Clone("tmp"); 
    tmp->Multiply(tmp);
    double norm = sqrt(tmp->Integral());
    delete tmp;
    ScaleHist(1./norm);
}

TH1* TTemplate::RebinToOriginalSampling(){
   if(fIsRebinnedToOriginalBinning) return fTemplatePulse;
   fIsRebinnedToOriginalBinning=true;
   return fTemplatePulse->Rebin(fRefineFactor);
}
    
void TTemplate::SubtractPedestal(){

    // Normalise the template so that it has pedestal=0 and amplitude=1
    // Work out the pedestal of the template from the first 5 bins
    int n_bins_for_template_pedestal = 5;
    double total = 0;
    for (int iBin = 1; iBin <= n_bins_for_template_pedestal; ++iBin) {
      total += fTemplatePulse->GetBinContent(iBin);
    }
    double template_pedestal = total / n_bins_for_template_pedestal;
   
    // Subtract off the pedesal
    for (int iBin = 1; iBin <= fTemplatePulse->GetNbinsX(); ++iBin) {
      double old_value = fTemplatePulse->GetBinContent(iBin);
      double new_value = old_value - template_pedestal;

      fTemplatePulse->SetBinContent(iBin, new_value);
    }
}

void TTemplate::ScaleHist(double factor){

    // Subtract off the pedesal
    for (int iBin = 0; iBin <= fTemplatePulse->GetNbinsX(); ++iBin) {
      double old_value = fTemplatePulse->GetBinContent(iBin);
      double new_value = old_value * factor;

      fTemplatePulse->SetBinContent(iBin, new_value);
    }
}


double TTemplate::GetPedestal()const{
  return fTemplatePulse->GetBinContent(1);
}

double TTemplate::GetTime()const{
  if(fTriggerPolarity>0) return fTemplatePulse->GetMaximumBin()-1;
  return fTemplatePulse->GetMinimumBin()-1;
}

double TTemplate::GetAmplitude()const{
  if(fTriggerPolarity>0) return fTemplatePulse->GetMaximum() - GetPedestal();
  return GetPedestal() - fTemplatePulse->GetMinimum();
}
