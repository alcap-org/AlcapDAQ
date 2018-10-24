#include "TemplateCreatorModule.h"
#include "TemplateFitter.h"
#include <iostream>
#include <utility>
#include <cmath>
#include "RegisterModule.inc"

#include "TemplateArchive.h"
#include "TH2.h"

using std::cout;
using std::endl;
using std::string;

TemplateCreatorModule::TemplateCreatorModule(modules::options* opts):
   BaseModule("TemplateCreatorModule",opts),fOptions(opts){
	dir->cd("/");
}

int TemplateCreatorModule::ProcessEntry(TGlobalData *gData,
                                        const TSetupData *gSetup){

  // Loop over each detector
  string bankname, detname;
  PulseIslandList_t thePulseIslands;
  BankPulseList_t::const_iterator it;

  // Open the template archive, ready for saving
  TemplateArchive* archive = new TemplateArchive("templates.root", "RECREATE");

  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); it++){
    bankname = it->first;
    detname = gSetup->GetDetectorName(bankname);

    // Get the TPIs
    thePulseIslands = it->second;
    if (thePulseIslands.size() == 0) continue; // no pulses here...

    // Create the template fitter and the template histogram itself
    // TH1D* hTemplate = NULL;
    // TemplateFitter* template_fitter = new TemplateFitter();

    // Now loop through the TPIs and create the templates
    for (PulseIslandList_t::const_iterator pulseIter = thePulseIslands.begin(); pulseIter != thePulseIslands.end(); ++pulseIter) {

      // See if there is more than one pulse on the TPI and only continue if there is one

      // If this is the first pulse, just add it directly to the template (we may want to randomly choose an initial pulse)

      // Make an initial guess at the parameters
      double amplitude = 0;
      double time = 0;
      InitialParameterGuess(*pulseIter, amplitude, time);

      std::cout << "Amplitude = " << amplitude << ", time = " << time << std::endl;

      // Fit the pulse to the template

      // Then we will add the fitted pulse to the template

      // When the fit has converged, we stop
    }

    // Save the template
    //    archive->SaveTemplate(hTemplate);
  }

  delete archive; // close the template archive file
  return 0;
}


// InitialParameterGuess
// -- makes an initial guess at the parameters from the TPulseIsland
void TemplateCreatorModule::InitialParameterGuess(const TPulseIsland* pulse, double& amplitude, double& time) {

  // Just use the TPI's methods for the time being
  amplitude = pulse->GetAmplitude();
  time = pulse->GetPulseTime();

}


ALCAP_REGISTER_MODULE(TemplateCreatorModule);
