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

#include "TH1D.h"
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
  fArchiveName=opts->GetString("file_name","templates.root");
  if(fRequestedChannels.empty()) fAnalyseAllChannels=true;

  // Prepare integral ratio cuts
  fCutIntegralRatio=opts->GetBool("use_IR_cut",false);
  if(fCutIntegralRatio){
    fIntegralMax=opts->GetDouble("max_integral");
    fIntegralMin=opts->GetDouble("min_integral");
    fIntegralRatioMax=opts->GetDouble("max_ratio");
    fIntegralRatioMin=opts->GetDouble("min_ratio");
  }

}

TemplateCreator::~TemplateCreator(){
///  delete fTemplateArchive;
///  fChannels.clear();
}

TemplateCreator::ChannelSet::ChannelSet(const std::string& det, const std::string& bank,
  modules::options* opts, int refine):
    detname(det), bankname(bank),
    fit_successes(0),fit_attempts(0),
    trigger_polarity(TSetupData::Instance()->GetTriggerPolarity(bank)),
    pulse_finder(NULL),
    integralRatio(new Algorithm::IntegralRatio(
                          opts->GetInt("start_integral",10),
                          opts->GetInt("start_tail",60),
                          opts->GetInt("stop_integral",0),
                          trigger_polarity))
{
       // Create the pulse candidate finder for this detector
       if(!opts->GetFlag("no_PCF_check")){
          pulse_finder = new PulseCandidateFinder(detname, opts);
       }

       // Create the TemplateFitter that we will use for this channel
       fitter = new TemplateFitter(detname, refine);
       
       template_pulse=new TTemplate(detname,refine,trigger_polarity,opts->GetFlag("debug"));
}

void TemplateCreator::ChannelSet::Clear(){
  delete fitter;
  delete template_pulse;
  if(pulse_finder) delete pulse_finder;
  if(integralRatio) delete integralRatio;
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int TemplateCreator::BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup){

  // Prepare the template archive
  if(fArchiveName==EventNavigator::Instance().GetOutputFileName()){
    fTemplateArchive = new TemplateArchive(GetDirectory());
  } else{
    fTemplateArchive = new TemplateArchive(fArchiveName.c_str(), "RECREATE");
  }

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
  unsigned no_converged=0;
  for(ChannelList::iterator i_ch=fChannels.begin(); i_ch!=fChannels.end(); ++i_ch){

    // See if we already have a converged template for this detector
    if (i_ch->template_pulse->HasConverged()) {
      no_converged++;
      continue;
    }

    // Get the bank and detector names for this detector
    const std::string& bankname = i_ch->bankname;
    const std::string& detname = i_ch->detname;
    const PulseIslandList& thePulseIslands= gData->fPulseIslandToChannelMap.at(bankname);
    fTemplateFitter=i_ch->fitter;

    // Get the TPIs
    if (thePulseIslands.size() == 0) continue; // no pulses here..

    // Loop through all the pulses
    for (PulseIslandList::const_iterator pulseIter = thePulseIslands.begin();
          pulseIter != thePulseIslands.end(); ++pulseIter) {

      TPulseIsland* pulse = *pulseIter;

      if(i_ch->pulse_finder){
         // First we will see how many candidate pulses there are on the TPI
         i_ch->pulse_finder->FindPulseCandidates(pulse);
         int n_pulse_candidates = i_ch->pulse_finder->GetNPulseCandidates();

         // only continue if there is one pulse candidate on the TPI
         if (n_pulse_candidates != 1) continue;
      }

      if(fCutIntegralRatio){
         try{
           (*i_ch->integralRatio)(pulse);
         }catch(std::out_of_range& e){
           continue;
         }
         const double& integral=i_ch->integralRatio->GetTotal();
         const double& ratio=i_ch->integralRatio->GetRatio();
         if( fIntegralMax < integral || fIntegralMin > integral
             || fIntegralRatioMax < ratio || fIntegralRatioMin > ratio) {
             continue;
         }
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

      // Add the first pulse directly to the template (although we may try and choose a random pulse to start with)
      if (i_ch->template_pulse->Empty()) {
        // Create the refined pulse waveform
        int pulseID=pulseIter- thePulseIslands.begin();
        TH1D* tpl=StartTemplate(pulseID, pulse,i_ch->detname);
        if(tpl) i_ch->template_pulse->Initialize(pulseID,tpl,GetDirectory());
        continue;
      }


      // Create the refined pulse waveform
      TH1D* hPulseToFit = CreateRefinedPulseHistogram(pulse, "hPulseToFit", "hPulseToFit", false);

      // all the other pulses will be fitted to the template and then added to it
      // Get some initial estimates for the fitter
      double template_amplitude = i_ch->template_pulse->GetAmplitude();
      double template_time = i_ch->template_pulse->GetTime();

      double pulse_pedestal = hPulseToFit->GetBinContent(1);
      double pulse_amplitude = definitions::DefaultValue;
      double pulse_time = definitions::DefaultValue;

      // now we're dealing with actual pulses since we subtract the template_pedestal in the transformation
      double pedestal_offset_estimate = pulse_pedestal; 
      double amplitude_scale_factor_estimate = definitions::DefaultValue;
      double time_offset_estimate = definitions::DefaultValue;

      // Define the values to scale and shift things by 
      if (TSetupData::Instance()->GetTriggerPolarity(bankname) >0) { 
        pulse_amplitude = (hPulseToFit->GetMaximum() - pulse_pedestal);
        pulse_time = hPulseToFit->GetMaximumBin() - 1;
      }
      else if (TSetupData::Instance()->GetTriggerPolarity(bankname) <0) {
        pulse_amplitude = (pulse_pedestal - hPulseToFit->GetMinimum());
        pulse_time = hPulseToFit->GetMinimumBin() - 1; // go from bin numbering (1, n_samples) to clock ticks (0, n_samples-1)
      }
      amplitude_scale_factor_estimate = pulse_amplitude / template_amplitude;  // estimated scale factor
      time_offset_estimate = pulse_time - template_time;

      i_ch->fitter->SetInitialParameterEstimates(pedestal_offset_estimate, amplitude_scale_factor_estimate, time_offset_estimate);
      
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
        if (i_ch->template_pulse->PulsesMerged() <= 10 
             || ( i_ch->template_pulse->PulsesMerged() <= 100 
                     && i_ch->template_pulse->PulsesMerged()%10 == 0)
             || ( i_ch->template_pulse->PulsesMerged()%100 == 0) ) {
          std::stringstream newhistname;
          newhistname << "hTemplate_" << i_ch->template_pulse->PulsesMerged() << "Pulses_" << detname;
          i_ch->template_pulse->Clone(newhistname.str().c_str());
        }
      }

      // Add the pulse to the template (we'll do correct the sample values there)
      i_ch->template_pulse->AddPulse(
                  fTemplateFitter->GetTimeOffset(),
                  fTemplateFitter->GetAmplitudeScaleFactor(),
                  fTemplateFitter->GetPedestalOffset(),
                  hPulseToFit);

      // we keep on adding pulses until adding pulses has no effect on the template
      if (i_ch->template_pulse->CheckConverged()) {
        cout << "TemplateCreator: " << detname << " template terminated containing " 
             << i_ch->template_pulse->PulsesMerged() <<" pulses "<< std::endl;
        break; // break from the for loop
      }
    } // end for loop over TPIs
  } //end for loop through channels
  
  if(no_converged==fChannels.size()){
  cout<<"All channels converged so end run"<<endl;
  return -1;
  }

  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int TemplateCreator::AfterLastEntry(TGlobalData* gData, const TSetupData* setup){

  // Print to stdout the percentage of successful fit for each channel
  for(ChannelList::iterator i_ch=fChannels.begin(); i_ch!=fChannels.end(); ++i_ch){

    if (i_ch->template_pulse->Empty()) { // if there's no template been created for this channel
      continue;
    }
    
    cout << "TemplateCreator: " << i_ch->detname 
         << ": " << i_ch->fit_attempts << " fits attempted with "
         << i_ch->fit_successes << " successful (" 
         << ((double)i_ch->fit_successes/(double)i_ch->fit_attempts)*100 << "%)" << endl;

    // Normalise the templates
    //i_ch->template_pulse->NormaliseToAmplitude();
    i_ch->template_pulse->AddToDirectory(GetDirectory());

    // Save the template to the file
    fTemplateArchive->SaveTemplate(i_ch->template_pulse);
    
    // tidy up
    i_ch->Clear();
  }

  return 0;
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

TH1D* TemplateCreator::StartTemplate(int pulseID, const TPulseIsland* pulse,const std::string& detname){

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

  std::string histname = "hTemplate_" + detname;
  std::string histtitle = "Template Histogram for the " + detname + " channel";
  return CreateRefinedPulseHistogram(pulse, histname.c_str(), histtitle.c_str(), true);
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.
ALCAP_REGISTER_MODULE(TemplateCreator);
