#include "MakeTemplate.h"
#include "utils/HistogramFitFCN.h"
#include "PulseTemplate.h"
#include "PrePulse.h"

#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <map>

#include "TH1.h"
#include "TFile.h"

MakeTemplate::MakeTemplate(char *HistogramDirectoryName) :
	BaseModule(HistogramDirectoryName) 
{
  fTemplateFile = new TFile("template.root", "RECREATE");
  gDirectory->cd("/");
}

MakeTemplate::~MakeTemplate(){ // Save and destroy templates
  std::map<std::string, PulseTemplate*>::iterator iTemplate;
  for (iTemplate = fTemplates.begin(); iTemplate != fTemplates.end(); ++iTemplate) {
    if (iTemplate->second) {
      iTemplate->second->Save(fTemplateFile);
      delete iTemplate->second;
      iTemplate->second = NULL;
    }
  }
  fTemplates.clear();
  fTemplateFile->Close();
  delete fTemplateFile;
}

int MakeTemplate::ProcessEntry(TGlobalData *gData, TSetupData *gSetup) {
  
  std::map< std::string, std::vector<TPulseIsland*> >& detPulses = gData->fPulseIslandToChannelMap;
  std::map< std::string, std::vector<TPulseIsland*> >::iterator iDetPulses;

  for (iDetPulses = detPulses.begin(); iDetPulses != detPulses.end(); ++iDetPulses) {
    std::string bank = iDetPulses->first;
    if (!fTemplates.count(bank))
      fTemplates[bank] = new PulseTemplate();
    PulseTemplate* pTemplate = fTemplates[bank];
    double pDT = gSetup->GetClockTick(bank);
    int pPol = gSetup->GetTriggerPolarity(bank);
    double pPed = (double)gSetup->GetPedestal(bank);
    double pMax = std::pow(2.,(double)(gSetup->GetNBits(bank))) - 1.;
    double pThresh = (pPol > 0 ? (pMax - pPed) / 2. : pPed / 2.);
    
    std::vector<TPulseIsland*>& pulses = iDetPulses->second;
    int iPulse = 0;
    while (!pTemplate->Converged() && iPulse < pulses.size()) {
      TPulseIsland* pulse = pulses[iPulse++];
      std::vector<PrePulse> prepulses = PrePulse::FindPrePulses(pulse, pThresh/2., pThresh/4.);
      if (prepulses.size() == 1) {
	if (pTemplate->GetNumberOfPulses() == 0) {
	  // If this is the first pulse, first guess at timing is
	  // weighted based on peak size relative to immediate
	  // neighbors
	  std::vector<int> samples = pulse->GetSamples();
	  int i = prepulses.at(0).GetLocation();
	  double s1 = (double)samples[i-1];
	  double s2 = (double)samples[i];
	  double s3 = (double)samples[i+1];
	  double t = (s3 - s1) / (s1 + s2 + s3);
	  pTemplate->AddPulse(pulse, t);
	  pTemplate->Save(fTemplateFile);
	} else {
	  double p, a, t, t0;
	  p = pPed;
	  a = pPol * ((double)prepulses.at(0).GetHeight() - pPed);
	  t0 = (double)prepulses.at(0).GetLocation();
	  t = t0;
	  pTemplate->Chi2Fit(pulse, p, a, t);
	  pTemplate->AddPulse(pulse, t - t0);
	}
	prepulses.at(0).Print();
	pTemplate->Print();
      }
    }
  }
  

  return 0;
}
