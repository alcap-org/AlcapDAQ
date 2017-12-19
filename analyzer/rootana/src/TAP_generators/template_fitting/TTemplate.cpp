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

void TTemplate::AddPulse(double pulse_time, double pulse_amplitude, double pulse_pedestal, const TH1D* hPulse){

  if (fDebug) {
    std::cout << "TTemplate::AddPulse(): # pulses = " << fTotalPulses << std::endl;
  }

  // Want to move the pulse to the template
  double template_pedestal = GetPedestal();
  double y_offset = template_pedestal - pulse_pedestal;
  double total_error=0;

  double template_time = GetTime();
  double x_offset = template_time - pulse_time;

  double template_amplitude = GetAmplitude();
  double y_scale = template_amplitude / pulse_amplitude;

  // Loop through the template histogram
  //  for (int iPulseBin = 1; iPulseBin < hPulse->GetNbinsX(); ++iPulseBin) {
  for (int iTemplateBin = 1; iTemplateBin <= fTemplatePulse->GetNbinsX(); ++iTemplateBin) {

    // Get the corrected bin number based on the fitted time offset We add 0.5
    // so that we round to the nearest integer and then subtract time offset
    // because this value might not want to go directly into the template
    int iPulseBin = iTemplateBin + 0.5 - x_offset; 

    // If we are outside of the bounds of the pulse we are adding,
    // assume that the uncorrected value is the same as the pulse's pedestal
    double uncorrected_value;
    if (iPulseBin < 1 || iPulseBin > hPulse->GetNbinsX()) {
      uncorrected_value = pulse_pedestal;
      //      continue;
    }
    else {
      uncorrected_value = hPulse->GetBinContent(iPulseBin);
    }


    // First, get the corrected sample value based on the fitted parameters
    double corrected_value = uncorrected_value + y_offset - template_pedestal; // want to scale the amplitude which is defined relative to the tempalte pedestal
    corrected_value *= y_scale;
    corrected_value += template_pedestal; // add it back because we want to find the template pedestal
    //    std::cout << "Corrected = " << y_scale << "*(" << uncorrected_value << " + " << y_offset << " - " << template_pedestal << ") + " << template_pedestal << " = " << corrected_value << std::endl;

    // Store the old bin content and error for later
    double old_bin_content = fTemplatePulse->GetBinContent(iTemplateBin);
    double old_bin_error = fTemplatePulse->GetBinError(iTemplateBin);

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
      cout << "TTemplate::AddPulse(): Template Bin #" << iTemplateBin << " Pulse Bin #" << iPulseBin
           << ": Corrected Sample Value = " << corrected_value << endl
    	   << "\t\t\tOld Value (Error) = " << old_bin_content << "(" << old_bin_error << ")" << endl
    	   << "\t\t\tNew Value (Error) = " << new_bin_content << "(" << new_bin_error << ")" << endl;
    }

    // Set the new bin content and error of the template
    fTemplatePulse->SetBinContent(iTemplateBin, new_bin_content);
    fTemplatePulse->SetBinError(iTemplateBin, new_bin_error);
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

/*void TTemplate::CreateTemplateErrorsHist() {
  std::string histname = fTemplatePulse->GetName();
  histname += "_errors";
  fTemplateErrors = (TH1D*) fTemplatePulse->Clone(histname.c_str());

  for (int i_bin = 1; i_bin <= fTemplatePulse->GetNbinsX(); ++i_bin) {
    double error = fTemplatePulse->GetBinError(i_bin);
    fTemplateErrors->SetBinContent(i_bin, error);
  }
}
*/
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

      double old_error = fTemplatePulse->GetBinError(iBin);
      double new_error = old_error * factor;

      fTemplatePulse->SetBinContent(iBin, new_value);
      fTemplatePulse->SetBinError(iBin, new_error);
    }
}


double TTemplate::GetPedestal()const{
    int n_bins_for_template_pedestal = 5;
    double total = 0;
    for (int iBin = 1; iBin <= n_bins_for_template_pedestal; ++iBin) {
      total += fTemplatePulse->GetBinContent(iBin);
    }
    double template_pedestal = total / n_bins_for_template_pedestal;

    return template_pedestal;
}

double TTemplate::GetTime()const{
  if(fTriggerPolarity>0) return fTemplatePulse->GetMaximumBin()-1;
  return fTemplatePulse->GetMinimumBin()-1;
}

double TTemplate::GetAmplitude()const{
  if(fTriggerPolarity>0) return fTemplatePulse->GetMaximum() - GetPedestal();
  return GetPedestal() - fTemplatePulse->GetMinimum();
}
