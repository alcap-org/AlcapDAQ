#include "PlotTDP_TDiff.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "ModulesNavigator.h"
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

  fSourceA = IDs::source(opts->GetString("source_a"));
  fSourceB = IDs::source(opts->GetString("source_b"));
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

  if(!modules::navigator::Instance()->Before("MakeDetectorPulses",GetName())){
    cout<<"MakeDetectorPulses must be used before PlotTDP_TDiff"<<endl;
    return 1;
  }

  // Create the histogram
  std::string detname_a = fSourceA.Channel().str();
  std::string detname_b = fSourceB.Channel().str();

  std::string histogram_name = modules::parser::ToCppValid("h" + fSourceA.str() + "_" + fSourceB.str() + "_TDiff");
  std::string histogram_title = "Time Difference between TDP source " + fSourceA.str() + "_" + fSourceB.str();
  int x_max = 1000000;
  int x_min = -1000000;
  int bin_width = 100;
  int n_bins = (x_max - x_min) / bin_width;
  fTDiffPlot = new TH1F(histogram_name.c_str(), histogram_title.c_str(), n_bins, x_min, x_max);

  std::string axis_title = "t_{" + detname_a + "} - t_{" + detname_b + "} [ns]";
  fTDiffPlot->GetXaxis()->SetTitle(axis_title.c_str());

  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int PlotTDP_TDiff::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

  // Loop over all TDP sources
  for(SourceDetPulseMap::const_iterator i_source=gDetectorPulseMap.begin();
      i_source!= gDetectorPulseMap.end(); ++i_source){
    
    //    std::cout << i_source->first << ", " << fSourceA << std::endl;
    if ( (i_source->first).matches(fSourceA)) {
      fDetPulsesA = i_source->second;
      if (Debug()) {
	std::cout << "PlotTDP_TDiff: " << fSourceA << " pulses found. (" << fDetPulsesA.size() << " pulses)" << std::endl;;
      }
    }
    else if ((i_source->first).matches(fSourceB)) {
      fDetPulsesB = i_source->second;
      if (Debug()) {
	std::cout << "PlotTDP_TDiff: " << fSourceB << " pulses found. (" << fDetPulsesB.size() << " pulses)" << std::endl;
      }
    }
  }

  // Loop through the TDPs in both channels
  for (DetectorPulseList::const_iterator i_det_pulse_a = fDetPulsesA.begin(); i_det_pulse_a != fDetPulsesA.end(); ++i_det_pulse_a) {
    for (DetectorPulseList::const_iterator i_det_pulse_b = fDetPulsesB.begin(); i_det_pulse_b != fDetPulsesB.end(); ++i_det_pulse_b) {

      double time_det_pulse_a = (*i_det_pulse_a)->GetTime();
      double time_det_pulse_b = (*i_det_pulse_b)->GetTime();
      double time_difference = time_det_pulse_a - time_det_pulse_b; // this is the same way as specified in PlotTAP_TDiff to maintain consistency

      fTDiffPlot->Fill(time_difference);
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
ALCAP_REGISTER_MODULE(PlotTDP_TDiff,source_a,source_b);
