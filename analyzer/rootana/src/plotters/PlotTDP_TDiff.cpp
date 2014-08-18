#include "PlotTDP_TDiff.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"

#include "TDetectorPulse.h"

#include <iostream>
using std::cout;
using std::endl;

extern SourceDetPulseMap gDetectorPulseMap;

PlotTDP_TDiff::PlotTDP_TDiff(modules::options* opts):
   BaseModule("PlotTDP_TDiff",opts){

  // Do something with opts here.  Has the user specified any
  // particular configuration that you want to know?
  // For example, perhaps this module wants an axis range:  

  fDetNameA = IDs::channel("muSc");
  fDetNameB = IDs::channel("Ge");
}

PlotTDP_TDiff::~PlotTDP_TDiff(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int PlotTDP_TDiff::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){
  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----PlotTDP_TDiff::BeforeFirstEntry(): I'm debugging!"<<endl;
  }

  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int PlotTDP_TDiff::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

  // Loop over all TDP sources
  for(SourceDetPulseMap::const_iterator i_source=gDetectorPulseMap.begin();
      i_source!= gDetectorPulseMap.end(); ++i_source){
    
    IDs::channel i_detname = (i_source->first).Channel();

    if (i_detname == fDetNameA) {
      fDetPulsesA = i_source->second;
      if (Debug()) {
	std::cout << "PlotTDP_TDiff: " << fDetNameA << " pulses found. (" << fDetPulsesA.size() << " pulses)" << std::endl;;
      }
    }
    else if (i_detname == fDetNameB) {
      fDetPulsesB = i_source->second;
      if (Debug()) {
	std::cout << "PlotTDP_TDiff: " << fDetNameB << " pulses found. (" << fDetPulsesB.size() << " pulses)" << std::endl;
      }
    }
  }

  // Loop through the TDPs in both channels
  std::cout << "AE: " << fDetPulsesA.size() << ", " << fDetPulsesB.size() << std::endl;
  for (DetectorPulseList::const_iterator i_det_pulse_a = fDetPulsesA.begin(); i_det_pulse_a != fDetPulsesA.end(); ++i_det_pulse_a) {
    for (DetectorPulseList::const_iterator i_det_pulse_b = fDetPulsesB.begin(); i_det_pulse_b != fDetPulsesB.end(); ++i_det_pulse_b) {

      std::cout << (*i_det_pulse_a)->GetTime() << ", " << (*i_det_pulse_b)->GetTime() << std::endl;

    }
  }
  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int PlotTDP_TDiff::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){

  // Print extra info if we're debugging this module:
  if(Debug()){
     cout<<"-----PlotTDP_TDiff::AfterLastEntry(): I'm debugging!"<<endl;
  }

  return 0;
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(PlotTDP_TDiff,x_max);
