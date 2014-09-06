#include "TemplateCreator.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "EventNavigator.h"
#include "SetupNavigator.h"
#include "ExportPulse.h"
#include "debug_tools.h"

#include "TMath.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
using std::cout;
using std::endl;


TemplateCreator::TemplateCreator(modules::options* opts):
  BaseModule("TemplateCreator",opts), fOpts(opts),
  fPulseDebug(false), fAnalyseAllChannels(false)
{
  fRefineFactor = opts->GetInt("refine_factor", 5);
  fPulseDebug = opts->GetBool("pulse_debug", false);
  opts->GetVectorStringsByDelimiter("channels",fRequestedChannels);
  if(fRequestedChannels.empty()) fAnalyseAllChannels=true;
  
  fCutIntegralRatio=opts->GetBool("use_IR_cut",false);
  if(fCutIntegralRatio){
    fIntegralMax=opts->GetDouble("max_integral");
    fIntegralMin=opts->GetDouble("min_integral");
    fIntegralRatioMax=opts->GetDouble("max_ratio");
    fIntegralRatioMin=opts->GetDouble("min_ratio");
  }
}

TemplateCreator::~TemplateCreator(){
}

TemplateCreator::ChannelSet::ChannelSet(const std::string& det, const std::string& bank,
  modules::options* opts, int refine):
    converged_status(false),
    detname(det), bankname(bank),
    fit_attempts(0),
    fit_successes(0),
    pulses_in_template(0),
    template_pulse(NULL),
    pulse_finder(NULL),
    integralRatio(new Algorithm::IntegralRatio(
                          opts->GetInt("start_integral",10),
                          opts->GetInt("start_tail",60),
                          opts->GetInt("stop_integral",0),
                          TSetupData::Instance()->GetTriggerPolarity(bank)))
{
       // Create the pulse candidate finder for this detector
       if(!opts->GetFlag("no_PCF_check")){
          pulse_finder = new PulseCandidateFinder(detname, opts);
       }

       // Create the TemplateFitter that we will use for this channel
       fitter = new TemplateFitter(detname, refine);

       // Setup the error hist
       std::string error_histname = "hErrorVsPulseAdded_" + detname;
       std::string error_histtitle = "Plot of the Error as each new Pulse is added to the template for the " + detname + " channel";
       int n_bins = 10000;
       error_v_pulse_hist = new TH1D(error_histname.c_str(), error_histtitle.c_str(), n_bins,0,n_bins);
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int TemplateCreator::BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup){

  // Prepare the template archive
  fTemplateArchive = new TemplateArchive("templates.root", "RECREATE");

  // Set all the converged statuses to false
  StringPulseIslandMap::const_iterator it;
  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); ++it){
       const std::string bankname = it->first;
       const std::string detname = TSetupData::Instance()->GetDetectorName(bankname);
       if(!fAnalyseAllChannels &&
          std::find(fRequestedChannels.begin(), fRequestedChannels.end(), detname)
              ==fRequestedChannels.end()) {
           continue;
       }
       if(Debug()) cout<<"TemplateCreator::BeforeFirstEntry: Will make template for '"<<detname<<"'"<<endl;

       fChannels.push_back(ChannelSet(detname,bankname,fOpts,fRefineFactor));
  }
  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int TemplateCreator::ProcessEntry(TGlobalData* gData, const TSetupData* setup){

  // Loop over each detector
  for(ChannelList::iterator i_ch=fChannels.begin(); i_ch!=fChannels.end(); ++i_ch){

    // Get the bank and detector names for this detector
    const std::string& bankname = i_ch->bankname;
    const std::string& detname = i_ch->detname;
    const PulseIslandList& thePulseIslands= gData->fPulseIslandToChannelMap.at(bankname);
    fTemplateFitter=i_ch->fitter;

    // See if we already have a converged template for this detector
    if (i_ch->converged_status == true) {
      continue;
    }

    // Get the TPIs
    if (thePulseIslands.size() == 0) continue; // no pulses here..

    // Loop through all the pulses
    for (PulseIslandList::const_iterator pulseIter = thePulseIslands.begin(); pulseIter != thePulseIslands.end(); ++pulseIter) {

      TPulseIsland* pulse = *pulseIter;

      if(i_ch->pulse_finder){
         // First we will see how many candidate pulses there are on the TPI
         i_ch->pulse_finder->FindPulseCandidates(pulse);
         int n_pulse_candidates = i_ch->pulse_finder->GetNPulseCandidates();

         // only continue if there is one pulse candidate on the TPI
         if (n_pulse_candidates != 1) continue;
      }

      if(fCutIntegralRatio){
         (*i_ch->integralRatio)(pulse);
     const double& integral=i_ch->integralRatio->GetTotal();
     const double& ratio=i_ch->integralRatio->GetRatio();
         if( fIntegralMax > integral || fIntegralMin < integral
             || fIntegralRatioMax > ratio || fIntegralRatioMin < ratio) {
             DEBUG_VALUE(fIntegralMax, fIntegralMin, fIntegralRatioMax, fIntegralRatioMin);
             DEBUG_VALUE(integral, ratio,i_ch->pulses_in_template);
             continue;
         }
      }

      // Add the first pulse directly to the template (although we may try and choose a random pulse to start with)
      if (i_ch->template_pulse == NULL) {
        i_ch->template_pulse=StartTemplate(pulseIter- thePulseIslands.begin(), pulse,detname);
        i_ch->pulses_in_template+=1;
        continue;
      }

      // Loop through the samples and check for digitizer overflow
      int over_under_flow=HasPulseOverflowed(pulse,i_ch->bankname);
      if(over_under_flow!=0){
          if (Debug()) {
            cout << "TemplateCreator: Pulse #" << pulseIter - thePulseIslands.begin() << " has ";
            if(over_under_flow>0) cout << "overflowed";
            else cout<<"undeflowed";
            cout <<" the digitizer and won't be added to the template" << endl;
          }
        continue; // skip this pulse
      }

      // Create the refined pulse waveform
      TH1D* hPulseToFit = CreateRefinedPulseHistogram(pulse, "hPulseToFit", "hPulseToFit", false);

      // all the other pulses will be fitted to the template and then added to it
      // Get some initial estimates for the fitter
      double template_pedestal = i_ch->template_pulse->GetBinContent(1);
      double template_amplitude = definitions::DefaultValue;
      double template_time = definitions::DefaultValue;

      double pulse_pedestal = hPulseToFit->GetBinContent(1);
      double pulse_amplitude = definitions::DefaultValue;
      double pulse_time = definitions::DefaultValue;

      // now we're dealing with actual pulses since we subtract the template_pedestal in the transformation
      double pedestal_offset_estimate = pulse_pedestal; 
      double amplitude_scale_factor_estimate = definitions::DefaultValue;
      double time_offset_estimate = definitions::DefaultValue;

      // Define the values to scale and shift things by 
      if (TSetupData::Instance()->GetTriggerPolarity(bankname) == 1) { 
        template_amplitude = (i_ch->template_pulse->GetMaximum() - template_pedestal);
        template_time = i_ch->template_pulse->GetMaximumBin() - 1; 

        pulse_amplitude = (hPulseToFit->GetMaximum() - pulse_pedestal);
        pulse_time = hPulseToFit->GetMaximumBin() - 1;

        amplitude_scale_factor_estimate = pulse_amplitude / template_amplitude;  // estimated scale factor
        time_offset_estimate = pulse_time - template_time;
      }
      else if (TSetupData::Instance()->GetTriggerPolarity(bankname) == -1) {
        template_amplitude = (template_pedestal - i_ch->template_pulse->GetMinimum());
        template_time = i_ch->template_pulse->GetMinimumBin() - 1; // go from bin numbering (1, n_samples) to clock ticks (0, n_samples-1)

        pulse_amplitude = (pulse_pedestal - hPulseToFit->GetMinimum());
        pulse_time = hPulseToFit->GetMinimumBin() - 1; // go from bin numbering (1, n_samples) to clock ticks (0, n_samples-1)

        amplitude_scale_factor_estimate = pulse_amplitude / template_amplitude;  // estimated scale factor
        time_offset_estimate = pulse_time - template_time;
      }

      i_ch->fitter->SetInitialParameterEstimates(pedestal_offset_estimate, amplitude_scale_factor_estimate, time_offset_estimate);
      
      if (Debug()) {
        cout << "TemplateCreator: " << detname << "(" << bankname << "): Pulse #" 
             << pulseIter - thePulseIslands.begin() << ": " << std::endl
      	     << "TemplateCreator: Template: pedestal = " << template_pedestal 
             << ", amplitude = " << template_amplitude << ", time = " << template_time << std::endl
      	     << "TemplateCreator: Pulse: pedestal = " << pulse_pedestal << ", amplitude = "
             << pulse_amplitude << ", time = " << pulse_time << std::endl
      	     << "TemplateCreator: Initial Estimates: pedestal = " << pedestal_offset_estimate
             << ", amplitude = " << amplitude_scale_factor_estimate 
      	     << ", time = " << time_offset_estimate << std::endl;
      }

      int fit_status = i_ch->fitter->FitPulseToTemplate(i_ch->template_pulse, hPulseToFit, bankname);
      ++i_ch->fit_attempts;
      if (fit_status != 0) {
        if (Debug()) {
          std::cout << "TemplateCreator: Problem with fit (status = " << fit_status << ")" << std::endl;
        }
        delete hPulseToFit; // delete this here since it is no longer needed
        continue;
      }
      ++i_ch->fit_successes;

      if (Debug()) {
        cout << "Template Creator: Fitted Parameters: PedOffset = " 
             << i_ch->fitter->GetPedestalOffset() << ", AmpScaleFactor = "
             << i_ch->fitter->GetAmplitudeScaleFactor() << ", TimeOffset = "
             << i_ch->fitter->GetTimeOffset() << ", Chi2 = "
             << i_ch->fitter->GetChi2() << ", NDoF = "
             << i_ch->fitter->GetNDoF() << ", Prob = "
             << TMath::Prob(i_ch->fitter->GetChi2(), i_ch->fitter->GetNDoF()) << std::endl << std::endl;
      }

      if (fPulseDebug) {
        // Print out some templates as we go along
        if (i_ch->pulses_in_template <= 10 || 
            (i_ch->pulses_in_template <= 100 && i_ch->pulses_in_template%10 == 0) ||
            (i_ch->pulses_in_template%100 == 0) ) {
          std::stringstream newhistname;
          newhistname << "hTemplate_" << i_ch->pulses_in_template << "Pulses_" << detname;
          i_ch->template_pulse->Clone(newhistname.str().c_str());
        }
      }

      // Add the pulse to the template (we'll do correct the sample values there)
      AddPulseToTemplate(*i_ch, hPulseToFit);
      ++i_ch->pulses_in_template;
      

      if (fPulseDebug) {
        // Print out the uncorrected and corrected pulse that has been added to the template

        // Create the histograms that we will use to plot the corrected and uncorrected pulses
        std::stringstream histname;
        std::string template_name = "hTemplate_" + detname;
        histname << template_name << "_Event" 
                 << EventNavigator::Instance().EntryNo() << "_Pulse" 
                 << pulseIter - thePulseIslands.begin() << "_" 
                 << i_ch->pulses_in_template << "Added";
        /*TH1D* hUncorrectedPulse =*/ (TH1D*) hPulseToFit->Clone(histname.str().c_str());
        histname << "_Corrected";
        TH1D* hCorrectedPulse = (TH1D*) hPulseToFit->Clone(histname.str().c_str());
        hCorrectedPulse->SetEntries(0); // set entries back to 0
        
        double pedestal_error = SetupNavigator::Instance()->GetNoise(detname);

        // Loop through the bins of the uncorrected pulse and set the values in the corrected pulse histogram
        for (int iPulseBin = 1; iPulseBin <= hPulseToFit->GetNbinsX(); ++iPulseBin) {
          
          double uncorrected_value = hPulseToFit->GetBinContent(iPulseBin);
          double corrected_value = CorrectSampleValue(uncorrected_value, template_pedestal);
          
          hCorrectedPulse->SetBinContent(iPulseBin +0.5 - i_ch->fitter->GetTimeOffset(), corrected_value); 
          hCorrectedPulse->SetBinError(iPulseBin +0.5 - i_ch->fitter->GetTimeOffset(), pedestal_error);
        }

      delete hPulseToFit;

      // we keep on adding pulses until adding pulses has no effect on the template
      bool converged = CheckConvergence(*i_ch);
      if (converged) {
        i_ch->converged_status = true;
        std::cout << "TemplateCreator: " << detname << " template terminated at iteration " << i_ch->pulses_in_template << std::endl;
        break; // break from the for loop
      }
      } // end if only one pulse candidate
    } //end for loop through channels
  }

  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int TemplateCreator::AfterLastEntry(TGlobalData* gData, const TSetupData* setup){

  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----I'm debugging TemplateCreator::AfterLastEntry()"<<endl;
  }

  // Print to stdout the percentage of successful fit for each channel
  for(ChannelList::iterator i_ch=fChannels.begin(); i_ch!=fChannels.end(); ++i_ch){

    TH1D* hTemplate = i_ch->template_pulse;
    
    if (!hTemplate) { // if there's no template been created for this channel
      continue;
    }
    
    if(Debug()){
       cout << "TemplateCreator: " << i_ch->detname 
            << ": " << i_ch->fit_attempts << " fits attempted with "
            << i_ch->fit_successes << " successful (" 
            << ((double)i_ch->fit_successes/(double)i_ch->fit_attempts)*100 << "%)" << endl;
    }

    // Normalise the template so that it has pedestal=0 and amplitude=1
    // Work out the pedestal of the template from the first 5 bins
    int n_bins_for_template_pedestal = 5;
    double total = 0;
    for (int iBin = 1; iBin <= n_bins_for_template_pedestal; ++iBin) {
      total += hTemplate->GetBinContent(iBin);
    }
    double template_pedestal = total / n_bins_for_template_pedestal;
   
    // Subtract off the pedesal
    for (int iBin = 1; iBin <= hTemplate->GetNbinsX(); ++iBin) {
      double old_value = hTemplate->GetBinContent(iBin);
      double new_value = old_value - template_pedestal;

      hTemplate->SetBinContent(iBin, new_value);
    }

    // Integrate over the histogram and scale to give an area of 1
    double integral = std::fabs(hTemplate->Integral()); // want the absolute value for the integral because of the negative polarity pulses
    hTemplate->Scale(1.0/integral);
    integral = std::fabs(hTemplate->Integral());

    // Save the template to the file
    fTemplateArchive->SaveTemplate(hTemplate);
  }

  // Clean up the template archive
  delete fTemplateArchive;

  return 0;
}

/// AddPulseToTemplate()
/// Adds the given pulse to the template
void TemplateCreator::AddPulseToTemplate(ChannelSet& current, TH1D* & hPulse) {

  if (Debug()) {
    std::cout << "AddPulseToTemplate(): pulses = " << current.pulses_in_template << std::endl;
  }

  TH1D* hTemplate=current.template_pulse;
  double template_pedestal = hTemplate->GetBinContent(1);

  // Loop through the pulse histogram
  for (int iPulseBin = 1; iPulseBin < hPulse->GetNbinsX(); ++iPulseBin) {

    // First, get the corrected sample value based on the fitted parameters
    double uncorrected_value = hPulse->GetBinContent(iPulseBin);
    double corrected_value = CorrectSampleValue(uncorrected_value, template_pedestal);

    // Get the corrected bin number based on the fitted time offset We add 0.5
    // so that we round to the nearest integer and then subtract time offset
    // because this value might not want to go directly into the template
    int bin_number = iPulseBin + 0.5 - fTemplateFitter->GetTimeOffset(); 

    // Only change the bin contents of bins within the range of the template histogram
    if (bin_number < 1 || bin_number > hTemplate->GetNbinsX()) {
      continue;
    }

    // Store the old bin content and error for later
    double old_bin_content = hTemplate->GetBinContent(bin_number);
    double old_bin_error = hTemplate->GetBinError(bin_number);

    // Calculate the new bin content...
    double new_bin_content = current.pulses_in_template * old_bin_content + corrected_value;
    new_bin_content /= (current.pulses_in_template + 1);

    /// ...and new bin error
    double new_bin_error = ((current.pulses_in_template - 1)*old_bin_error*old_bin_error)
                         + (corrected_value - old_bin_content)*(corrected_value - new_bin_content);
    new_bin_error = std::sqrt(new_bin_error / current.pulses_in_template);

    if (Debug()) {
      cout << "TemplateCreator::AddPulseToTemplate(): Bin #" << bin_number 
           << ": Corrected Sample Value = " << corrected_value << endl
    	   << "\t\t\tOld Value (Error) = " << old_bin_content << "(" << old_bin_error << ")" << endl
    	   << "\t\t\tNew Value (Error) = " << new_bin_content << "(" << new_bin_error << ")" << endl;
    }

    // Set the new bin content and error of the template
    hTemplate->SetBinContent(bin_number, new_bin_content);
    hTemplate->SetBinError(bin_number, new_bin_error);
  }
}

double TemplateCreator::CorrectSampleValue(double old_value, double template_pedestal) {

  double new_value = old_value - fTemplateFitter->GetPedestalOffset();
  new_value /= fTemplateFitter->GetAmplitudeScaleFactor();
  new_value += template_pedestal;

  return new_value;
}

// Creates a histogram with sub-bin resolution
TH1D* TemplateCreator::CreateRefinedPulseHistogram(
    const TPulseIsland* pulse, std::string histname,
     std::string histtitle, bool interpolate) {

  // Get a few things first
  const std::string& bankname = pulse->GetBankName();
  const std::string detname = TSetupData::Instance()->GetDetectorName(bankname);
  const std::vector<int>& theSamples = pulse->GetSamples();
  int n_samples = theSamples.size();
  int n_bins = fRefineFactor*n_samples; // number of bins in the template

  // Create the higher resolution histogram
  TH1D* hist = new TH1D(histname.c_str(), histtitle.c_str(), n_bins, 0, n_samples);

  double pedestal_error = SetupNavigator::Instance()->GetNoise(detname);

  // Go through the bins in the high-resolution histogram
  // NB sample numbers go grom 0 to n-1 and bins go from 1 to n
  for (int i = 0; i < n_bins; ++i) {
    int bin = i+1; // bins go from 1 to n rather than 0 to n-1
    int sample_number = i / fRefineFactor;
    double remainder = i % fRefineFactor;
    double sample_value;

    // We may want to interpolate between the samples in the samples vector
    if (interpolate) {
      try {
    sample_value = theSamples.at(sample_number) 
                     + (remainder / fRefineFactor)*(theSamples.at(sample_number+1) 
                     - theSamples.at(sample_number));
      }
      catch (const std::out_of_range& oor) { // if we'll be going out of range of the samples vector
    sample_value = theSamples.at(sample_number);
      }
    }
    else {
      sample_value = theSamples.at(sample_number);
    }

    // Set the bin contents and bin error
    hist->SetBinContent( bin, sample_value);
    hist->SetBinError( bin, pedestal_error);
  }
  hist->SetDirectory(0);

  return hist;
}


bool TemplateCreator::CheckConvergence(ChannelSet& current) {

  const int n_pulses_in_template = current.pulses_in_template;
  int trigger_polarity = TSetupData::Instance()->GetTriggerPolarity(current.bankname);
  const TH1D* hTemplate=current.template_pulse;

  double error = 0;
  if (trigger_polarity == -1) {
    error = hTemplate->GetBinError(hTemplate->GetMinimumBin());
  }
  else if (trigger_polarity == 1) {
    error = hTemplate->GetBinError(hTemplate->GetMaximumBin());
  }
  current.error_v_pulse_hist->Fill(n_pulses_in_template, error);

  // Check the difference between this iteration and previous ones and, if it's small, the template has converged
  int n_bins_to_check = 10;
  double convergence_limit = 0.1;
  for (int iPrevBin = 0; iPrevBin < n_bins_to_check; ++iPrevBin) {
    // we have just added error to bin number n_pulses_in_template+1
    int newest_bin=current.error_v_pulse_hist->FindBin(n_pulses_in_template);
    double previous_error = current.error_v_pulse_hist->GetBinContent(newest_bin-iPrevBin);

    if ( std::fabs(previous_error - error) > convergence_limit) {
       return false;
    }
  }

  return true;
}

int TemplateCreator::HasPulseOverflowed(const TPulseIsland* pulse, const std::string& bankname){
    // Get the samples so we can check for digitiser overflow
    const std::vector<int>& theSamples = (pulse)->GetSamples();
    int n_samples = theSamples.size();

    // Calculate the maximum ADC value for this digitiser
    int n_bits = TSetupData::Instance()->GetNBits(bankname);
    double max_adc_value = std::pow(2, n_bits);

    for (int i = 0; i < n_samples; ++i) {
      int sample_value = theSamples.at(i);
      if (sample_value >= max_adc_value-1 && sample_value <= max_adc_value+1) {
         return 1;
      }
      else if (sample_value == 0) {
         return -1;
      }
    }
    // no under or overflow
    return 0;
}

TH1D* TemplateCreator::StartTemplate(int pulseID,const TPulseIsland* pulse,const std::string& detname){
  std::string histname = "hTemplate_" + detname;
  std::string histtitle = "Template Histogram for the " + detname + " channel";

  // check this pulse is safely within the waveform
  int pulse_length = pulse->GetSamples().size();
  if (pulse->GetPeakSample() >= pulse_length - pulse_length/5.0) {
    if (Debug()) {
      cout << "TemplateCreator: Pulse #" << pulseID
           << " is too close to one end of the island and so won't be used as "
              "the first pulse in the template." << endl;
    }
    return NULL;
  }
  if (Debug()) {
    cout << "TemplateCreator: Adding " << detname << " Pulse #" 
         << pulseID << " directly to the template" << endl;
  }

  TH1D* tpl=CreateRefinedPulseHistogram(pulse, histname.c_str(), histtitle.c_str(), true);
  tpl->SetDirectory(GetDirectory());
  return tpl;
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.
ALCAP_REGISTER_MODULE(TemplateCreator);
