#include "PlotTME_Test.h"
#include <TMuonEvent.h>
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

PlotTME_Test::PlotTME_Test(modules::options* opts):
  BaseModule("PlotTME_Test",opts,false),fOptions(opts){

  dir->cd("/");
}

PlotTME_Test::~PlotTME_Test(){
}

int PlotTME_Test::BeforeFirstEntry(TGlobalData *aData, const TSetupData* aSetup){
  fRightCounter = fLeftCounter = 0;
    return 0;
}

int PlotTME_Test::ProcessEntry(TGlobalData *aData, const TSetupData* aSetup){
  //gMuonEvents.clear();
  for (MuonEventList::const_iterator i_muonEvent = gMuonEvents.begin(); i_muonEvent != gMuonEvents.end(); ++i_muonEvent) {
    int n_sources = (*i_muonEvent)->GetNumSources();

    for (int i_source = 0; i_source < n_sources; ++i_source) {
      IDs::source i_sourceID = (*i_muonEvent)->GetSource(i_source);
      if (i_sourceID.matches(IDs::channel("SiR2"))) {
	//	std::cout << "TME #" << i_muonEvent - gMuonEvents.begin() << ": " << (*i_muonEvent)->GetSource(i_source).str() << std::endl;
	int n_source_pulses = (*i_muonEvent)->NumPulses(i_sourceID);
	fRightCounter += n_source_pulses;
      }
      else if (i_sourceID.matches(IDs::channel("SiL2"))) {
	//	std::cout << "TME #" << i_muonEvent - gMuonEvents.begin() << ": " << (*i_muonEvent)->GetSource(i_source).str() << std::endl;
	int n_source_pulses = (*i_muonEvent)->NumPulses(i_sourceID);
	fLeftCounter += n_source_pulses;
      }
    }
  }
  return 0;
}

int PlotTME_Test::AfterLastEntry(TGlobalData *aData, const TSetupData* aSetup){
  std::cout << "Left : Right = " << fLeftCounter << " : " << fRightCounter << std::endl;
  std::cout << "L / R = " << (double)fLeftCounter / (double)fRightCounter << std::endl;
    return 0;
}


ALCAP_REGISTER_MODULE(PlotTME_Test);
