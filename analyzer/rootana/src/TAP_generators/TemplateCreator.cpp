#include "ModulesManager.h"
#include "TemplateCreator.h"
#include <iostream>
#include <utility>
#include <cmath>
#include "RegisterModule.inc"

#include "TemplateArchive.h"
#include "TH2.h"

using std::cout;
using std::endl;
using std::string;

TemplateCreator::TemplateCreator(modules::options* opts):
   FillHistBase("TemplateCreator",opts),fOptions(opts){
	dir->cd("/");
}

TemplateCreator::~TemplateCreator(){
}

int TemplateCreator::BeforeFirstEntry(TGlobalData* gData,TSetupData *setup){
    return 0;
}

int TemplateCreator::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){

  // Loop over each detector
  string bankname, detname;
  PulseIslandList_t thePulseIslands;
  BankPulseList_t::const_iterator it;

  // Open the template archive, ready for saving
  TemplateArchive* archive = new TemplateArchive("templates.root");

  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); it++){
    bankname = it->first;
    detname = gSetup->GetDetectorName(bankname);

    // Get the TPIs
    thePulseIslands = it->second;
    if (thePulseIslands.size() == 0) continue; // no pulses here...

    // Here we will create the template histogram

    // Now loop through the TPIs and create the templates
    for (PulseIslandList_t::const_iterator pulseIter = thePulseIslands.begin(); pulseIter != thePulseIslands.end(); ++pulseIter) {
      
      // Make an initial guess at the parameters
      double amplitude = 0;
      double time = 0;
      InitialParameterGuess(*pulseIter, amplitude, time);

      //      std::cout << "Amplitude = " << amplitude << ", time = " << time << std::endl;

      // Then we will fit this pulse to the template

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
void TemplateCreator::InitialParameterGuess(const TPulseIsland* pulse, double& amplitude, double& time) {

  // Just use the TPI's methods for the time being
  amplitude = pulse->GetAmplitude();
  time = pulse->GetPulseTime();

}
ALCAP_REGISTER_MODULE(TemplateCreator);
