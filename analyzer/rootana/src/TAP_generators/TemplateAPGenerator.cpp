#include "TAPGeneratorFactory.h"
#include "TemplateAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"

#include "utils/TemplateFitter.h"

#include "TMath.h" 

#include <iostream>
using std::cout;
using std::endl;

TemplateAPGenerator::TemplateAPGenerator(TAPGeneratorOptions* opts):
	TVAnalysedPulseGenerator("Template",opts){
	// Do things to set up the generator here. 

  fTemplateArchive = new TemplateArchive("templates.root", "READ");
}

int TemplateAPGenerator::ProcessPulses( 
		const PulseIslandList& pulseList,
		AnalysedPulseList& analysedList){
  // Do something here that takes the TPIs in the PulseIslandList and
  // fills the list of TAPS

  // Loop over all the TPIs given to us
  double amplitude;
  TAnalysedPulse* tap;

  // Analyse each TPI
  for (PulseIslandList::const_iterator tpi=pulseList.begin();
       tpi!=pulseList.end(); tpi++){


    // Get the template from the archive
    std::string channel = GetChannel().str();
    std::string templatename = "hTemplate_" + channel;
    TH1D* hTemplate = fTemplateArchive->GetTemplate(templatename.c_str());

    // Then we try and fit this TPI to the template
    TemplateFitter* template_fitter = new TemplateFitter(channel, 5);

    // Create the histogram of the pulse
    TH1D* hPulseToFit = template_fitter->CreateRefinedPulseHistogram(*tpi, "hPulseToFit", "hPulseToFit", false);


    // Get some initial estimates for the fitter
    std::string bankname = (*tpi)->GetBankName();

    double template_pedestal = hTemplate->GetBinContent(1);
    double template_amplitude;
    double template_time;

    double pulse_pedestal = hPulseToFit->GetBinContent(1);
    double pulse_amplitude;
    double pulse_time;

    double pedestal_offset_estimate = pulse_pedestal; // now we're dealing with actual pulses since we subtract the template_pedestal in the transformation
    double amplitude_scale_factor_estimate;
    double time_offset_estimate;
    if (TSetupData::Instance()->GetTriggerPolarity(bankname) == 1) { 
      template_amplitude = (hTemplate->GetMaximum() - template_pedestal);
      template_time = hTemplate->GetMaximumBin() - 1; // go from bin numbering (1, n_samples) to clock ticks (0, n_samples-1)

      pulse_amplitude = (hPulseToFit->GetMaximum() - pulse_pedestal);
      pulse_time = hPulseToFit->GetMaximumBin() - 1;

      amplitude_scale_factor_estimate = pulse_amplitude / template_amplitude;  // estimated scale factor
      time_offset_estimate = pulse_time - template_time;
    }
    else if (TSetupData::Instance()->GetTriggerPolarity(bankname) == -1) {
      template_amplitude = (template_pedestal - hTemplate->GetMinimum());
      template_time = hTemplate->GetMinimumBin() - 1; // go from bin numbering (1, n_samples) to clock ticks (0, n_samples-1)

      pulse_amplitude = (pulse_pedestal - hPulseToFit->GetMinimum());
      pulse_time = hPulseToFit->GetMinimumBin() - 1; // go from bin numbering (1, n_samples) to clock ticks (0, n_samples-1)

      amplitude_scale_factor_estimate = pulse_amplitude / template_amplitude;  // estimated scale factor
      time_offset_estimate = pulse_time - template_time;
    }

    template_fitter->SetInitialParameterEstimates(pedestal_offset_estimate, amplitude_scale_factor_estimate, time_offset_estimate);

    if (Debug()) {
      std::cout << "TemplateAPGenerator: " << channel << "(" << bankname << "): Pulse #" << tpi - pulseList.begin() << ": " << std::endl
		<< "TemplateAPGenerator: Template: pedestal = " << template_pedestal << ", amplitude = " << template_amplitude << ", time = " << template_time << std::endl
		<< "TemplateAPGenerator: Pulse: pedestal = " << pulse_pedestal << ", amplitude = " << pulse_amplitude << ", time = " << pulse_time << std::endl
		<< "TemplateAPGenerator: Initial Estimates: pedestal = " << pedestal_offset_estimate << ", amplitude = " << amplitude_scale_factor_estimate 
		<< ", time = " << time_offset_estimate << std::endl;
    }
    
    int fit_status = template_fitter->FitPulseToTemplate(hTemplate, hPulseToFit, bankname);
    if (fit_status != 0) {
      if (Debug()) {
	std::cout << "TemplateAPGenerator: Problem with fit (status = " << fit_status << ")" << std::endl;
      }
      delete hPulseToFit; // delete this here since it is no longer needed
      continue;
    }

    if (Debug()) {
      std::cout << "TemplateAPGenerator: Fitted Parameters: PedOffset = " << template_fitter->GetPedestalOffset() << ", AmpScaleFactor = " << template_fitter->GetAmplitudeScaleFactor()
		<< ", TimeOffset = " << template_fitter->GetTimeOffset() << ", Chi2 = " << template_fitter->GetChi2() << ", NDoF = " << template_fitter->GetNDoF() 
		<< ", Prob = " << TMath::Prob(template_fitter->GetChi2(), template_fitter->GetNDoF()) << std::endl << std::endl;
    }

    // Now that we've found the information we were looking for make a TAP to
    // hold it.  This method makes a TAP and sets the parent TPI info.  It needs
    // the index of the parent TPI in the container as an argument
    tap = MakeNewTAP(tpi-pulseList.begin());

    // Get the amplitude and time of the TAP
    tap->SetAmplitude(template_fitter->GetAmplitudeScaleFactor());
    //tap->SetTime(time);

    // Finally add the new TAP to the output list
    analysedList.push_back(tap);

    delete hPulseToFit;
   }
	
  // Generators have a Debug method similar to modules
  if(Debug()){
    // They also have a unique ID, retrievable by GetSource and
    // a GetChannel method to get the ID of just the channel
    cout<<"Now running source: "<<GetSource().str()<<" which looks for TAPs on "
      "channel: "<<GetChannel().str()<<'\n';
  }
  
  // returning 0 tells the caller that we were successful, return non-zero if not
  return 0;
}

// Similar to the modules, this macro registers the generator with
// MakeAnalysedPulses. The first argument is compulsory and gives the name of
// this generator. All subsequent arguments will be used as names for arguments
// given directly within the modules file.  See the github wiki for more.
//
// NOTE: for TAP generators OMIT the APGenerator part of the class' name
ALCAP_TAP_GENERATOR(Template);
