#include "ModulesManager.h"
#include "ExportPulse.h"
#include "TVAnalysedPulseGenerator.h"
#include "MaxBinAPGenerator.h"
#include <iostream>
#include <utility>
#include <sstream>
#include "RegisterModule.inc"

using std::cout;
using std::endl;
using std::string;

extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;

ExportPulse::ExportPulse(modules::options* opts):
   FillHistBase("ExportPulse",opts),fOptions(opts){
        fPulseCounter = 0;
	dir->cd("/");
}

ExportPulse::~ExportPulse(){
}

int ExportPulse::BeforeFirstEntry(TGlobalData* gData,TSetupData *setup){
  fPulsesToPlot.push_back(3289);
    return 0;
}

int ExportPulse::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){

  PulseIslandList_t thePulseIslands;
  std::cout << "ExportPulse::ProcessEntry()" << std::endl;
  // Loop through the banks
  for (BankPulseList_t::const_iterator it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); it++){
    std::string bankname = it->first;
    std::string detname = gSetup->GetDetectorName(bankname);
    std::cout << "In " << detname << " (" << bankname << ")\n";
    // Loop through the pulses
    thePulseIslands = it->second;
    for (PulseIslandList_t::iterator pulseIter = thePulseIslands.begin(); pulseIter != thePulseIslands.end(); ++pulseIter) {
      std::vector<int> theSamples = (*pulseIter)->GetSamples();

      // Loop through the pulse IDs we want to plot
      for (std::vector<int>::iterator pulseIDIter = fPulsesToPlot.begin(); pulseIDIter != fPulsesToPlot.end(); ++pulseIDIter) {

	if (fPulseCounter == *pulseIDIter) {
	  std::cout << "Exporting pulse #" << fPulseCounter << std::endl;
	  // Export the pulse island as a histogram
	  std::stringstream histname;
	  histname << "Pulse_" << bankname << "_" << detname << "_" << fPulseCounter;
	  TH1F* hPulse = new TH1F(histname.str().c_str(), histname.str().c_str(), theSamples.size(),0,theSamples.size());

	  for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {
	    hPulse->Fill(sampleIter - theSamples.begin(), *sampleIter);
	  }

	  break;
	}
      } 
      ++fPulseCounter;
    }
  }

  return 0;
}

ALCAP_REGISTER_MODULE(ExportPulse);
