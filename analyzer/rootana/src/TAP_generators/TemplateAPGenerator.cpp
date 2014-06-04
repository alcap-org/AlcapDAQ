#include "TemplateAPGenerator.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"

#include "TemplateArchive.h"
#include "TCanvas.h"
#include <algorithm>
#include <cmath>
#include <iostream>

// IsTimeOrdered()
// -- Returns tru of the first pulse is before the second
// Static so that only this file sees it
// Useful for sorting the pulses
static bool IsTimeOrdered(TAnalysedPulse* a, TAnalysedPulse* b) {
  // Want a to be before b
  return ( a->GetTime() < b->GetTime() );
}

void TemplateAPGenerator::ProcessPulses(const TSetupData* eventSetup,
      const PulseIslandList& pulseList, AnalysedPulseList& analysedList){

      SetBankInfo(eventSetup,pulseList[0]->GetBankName());

      // Get the template from the ROOT file
      TemplateArchive* archive = new TemplateArchive("templates.root", "READ");
      TH1F* hTemplate = archive->GetTemplate("template");

      double amplitude, time, integral, energy;

      for (PulseIslandList::const_iterator pulseIter = pulseList.begin(); pulseIter != pulseList.end(); pulseIter++) {
         amplitude = 0;
         time = 0;
         integral = 0;
         energy = 0.;

	 // Here we will fit the template to the TPulseIsland

	 // Here we will extract the variables from the fit
	 
         // Add the pulse into the list
         analysedList.push_back(new  TAnalysedPulse(amplitude, time, integral, energy, fDetName));

      }
      std::sort(analysedList.begin(), analysedList.end(), IsTimeOrdered);

      delete archive; // close the template archive
}
