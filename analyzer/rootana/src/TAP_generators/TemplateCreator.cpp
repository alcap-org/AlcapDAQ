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

    // Create the template for this bank and detector
    std::string histname = "hTemplate_" + detname + "_" + bankname;
    TH2F* hTemplate = NULL; 

    // Now loop through the TPIs and create the templates
    for (PulseIslandList_t::const_iterator pulseIter = thePulseIslands.begin(); pulseIter != thePulseIslands.end(); ++pulseIter) {
      
      // Get the samples
      std::vector<int> theSamples = (*pulseIter)->GetSamples();

      // Create the template if it doesn't exist
      if (hTemplate == NULL) {
	int n_bits = gSetup->GetNBits(bankname);
	int max_adc_value = std::pow(2, n_bits);

	hTemplate = new TH2F(histname.c_str(), histname.c_str(), theSamples.size(),0,theSamples.size(), max_adc_value,0,max_adc_value);
      }

      // Loop through the samples and add to the template
      for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {
	hTemplate->Fill(sampleIter - theSamples.begin(), *sampleIter);
      }
    }

    // Save the template
    archive->SaveTemplate(hTemplate);
  }

  delete archive; // close the template archive file
  return 0;
}

ALCAP_REGISTER_MODULE(TemplateCreator);
