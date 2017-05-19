#include "ExportAllTAP.h"

#include "ExportPulse.h"
#include "TAnalysedPulse.h"
#include "RegisterModule.inc"
#include "definitions.h"
#include "SetupNavigator.h"

#include <iostream>

extern SourceAnalPulseMap gAnalysedPulseMap;

int ExportAllTAP::ProcessEntry(TGlobalData* gData, const TSetupData* gSetup) {
  for (SourceAnalPulseMap::const_iterator i_det = gAnalysedPulseMap.begin();
       i_det != gAnalysedPulseMap.end();
       i_det++) {
    // Create the histogram if it's not been created yet
    const AnalysedPulseList& taps = i_det->second;
    for (AnalysedPulseList::const_iterator tap = taps.begin();
         tap != taps.end();
         ++tap) {
      std::cout << i_det->first << " " << tap-taps.begin() << std::endl;
      ExportPulse::Instance()->AddToExportList(*tap);
    }
  }
  return 0;
}

ALCAP_REGISTER_MODULE(ExportAllTAP)
