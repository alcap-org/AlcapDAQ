#include "PlotTME_SiliconArmAsymmetry.h"
#include <TMuonEvent.h>
#include <TDetectorPulse.h>
#include "ModulesOptions.h"
#include "definitions.h"

#include <iostream>
#include <string>
#include <map>
#include <utility>

#include "MaxTimeDiffMEGenerator.h"
#include "RegisterModule.inc"
#include "TMEGeneratorFactory.h"

using std::string;
using std::map;
using std::cout;
using std::endl;

extern MuonEventList gMuonEvents;

PlotTME_SiliconArmAsymmetry::PlotTME_SiliconArmAsymmetry(modules::options* opts):
  BaseModule("PlotTME_SiliconArmAsymmetry",opts),fOptions(opts){

  dir->cd("/");
}

PlotTME_SiliconArmAsymmetry::~PlotTME_SiliconArmAsymmetry(){
}

int PlotTME_SiliconArmAsymmetry::BeforeFirstEntry(TGlobalData *aData, const TSetupData* aSetup){
  fRightCounter = fLeftCounter = 0;
  fAmplitudeHistogram = new TH1F("fAmplitudeHistogram", "fAmplitudeHistogram", 500,0,5000);
    return 0;
}

int PlotTME_SiliconArmAsymmetry::ProcessEntry(TGlobalData *aData, const TSetupData* aSetup){
  //gMuonEvents.clear();
  for (MuonEventList::const_iterator i_muonEvent = gMuonEvents.begin(); i_muonEvent != gMuonEvents.end(); ++i_muonEvent) {
    int n_sources = (*i_muonEvent)->GetNumSources();

    for (int i_source = 0; i_source < n_sources; ++i_source) {
      IDs::source i_sourceID = (*i_muonEvent)->GetSource(i_source);
      int n_source_pulses = 0;
      if (i_sourceID.matches(IDs::channel("SiR2"))) {
	//	std::cout << "TME #" << i_muonEvent - gMuonEvents.begin() << ": " << (*i_muonEvent)->GetSource(i_source).str() << std::endl;
	n_source_pulses = (*i_muonEvent)->NumPulses(i_sourceID);
	  
	fRightCounter += n_source_pulses;
      }
      else if (i_sourceID.matches(IDs::channel("SiL2"))) {
	//	std::cout << "TME #" << i_muonEvent - gMuonEvents.begin() << ": " << (*i_muonEvent)->GetSource(i_source).str() << std::endl;
	n_source_pulses = (*i_muonEvent)->NumPulses(i_sourceID);
	fLeftCounter += n_source_pulses;
      }

      for (int index = 0; index < n_source_pulses; ++index) {
	const TDetectorPulse* i_pulse = (*i_muonEvent)->GetPulse(i_sourceID, index);
	
	// Get the amplitude
	int amplitude = i_pulse->GetAmplitude(); // fast by default
	
	// Check to see if the amplitude is valid
	if (amplitude == definitions::DefaultValue) {
	  amplitude = i_pulse->GetAmplitude(TDetectorPulse::kSlow);
	}

	//	std::cout << i_sourceID << ": Amplitude of pulse = " << i_pulse->GetAmplitude(TDetectorPulse::kSlow) << std::endl;
	fAmplitudeHistogram->Fill(amplitude);
      }
    }
  }
  return 0;
}

int PlotTME_SiliconArmAsymmetry::AfterLastEntry(TGlobalData *aData, const TSetupData* aSetup){
  std::cout << "Left : Right = " << fLeftCounter << " : " << fRightCounter << std::endl;
  std::cout << "Asymmetry ((N_L - N_R)/(N_L + N_R)) = " << (fLeftCounter - fRightCounter) / (fLeftCounter + fRightCounter) << std::endl;
    return 0;
}


ALCAP_REGISTER_MODULE(PlotTME_SiliconArmAsymmetry);
