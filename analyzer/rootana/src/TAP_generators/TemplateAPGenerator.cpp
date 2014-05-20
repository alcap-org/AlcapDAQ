#include "TemplateAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"

#include "TemplateArchive.h"
#include "TCanvas.h"
#include <algorithm>
#include <cmath>
#include <iostream>

#include "TH2.h"

static TH2F* hTemplate = NULL;

// IsTimeOrdered()
// -- Returns tru of the first pulse is before the second
// Static so that only this file sees it
// Useful for sorting the pulses
static bool IsTimeOrdered(TAnalysedPulse* a, TAnalysedPulse* b) {
  // Want a to be before b
  return ( a->GetTime() < b->GetTime() );
}

void TemplateAPGenerator::ProcessPulses(const TSetupData* eventSetup,
      const PulseIslandList_t& pulseList, AnalysedPulseList_t& analysedList){

      SetBankInfo(eventSetup,pulseList[0]->GetBankName());

      // Get the template from the ROOT file
      TemplateArchive* archive = new TemplateArchive("templates.root");
      TH1F* hTemplate = archive->GetTemplate("template");

      TH1F* hTemplate = archive->GetTemplate("template");

      TemplateArchive* new_archive = new TemplateArchive("new_archive.root");
      new_archive->SaveTemplate(hTemplate);
      delete new_archive;

      double amplitude, time, integral, energy;

      for (PulseIslandList_t::const_iterator pulseIter = pulseList.begin(); pulseIter != pulseList.end(); pulseIter++) {
         amplitude = 0;
         time = 0;
         integral = 0;
         energy = 0.;
	 
	 AddToTemplate(*pulseIter);

	 // Here we will fit the template to the TPulseIsland

	 // Here we will extract the variables from the fit
	 
         // Add the pulse into the list
         analysedList.push_back(new  TAnalysedPulse(amplitude, time, integral, energy, fDetName));

      }
      std::sort(analysedList.begin(), analysedList.end(), IsTimeOrdered);

      delete archive; // close the template archive
}


// AddToTemplate()
// -- adds the given pulse to the template
// -- this is just rough for the time being and may be moved elsewhere
void TemplateAPGenerator::AddToTemplate(const TPulseIsland* pulse) {

  // Get the samples
  std::vector<int> theSamples = pulse->GetSamples();

  if (hTemplate == NULL) {
    double max_adc_value = std::pow(2, fNBits);
    hTemplate = new TH2F("Template", "Template", theSamples.size(),0,theSamples.size(), max_adc_value,0,max_adc_value);
  }
  
  // Loop through the samples and add them to the template
  for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {
    hTemplate->Fill(sampleIter - theSamples.begin(), *sampleIter);
  }
}
