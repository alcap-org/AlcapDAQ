#include "CalcCoarseTimeOffset.h"
#include <iostream>
#include <string>
#include <sstream>
#include "TGlobalData.h"
#include "TSetupData.h"

#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "RegisterModule.inc"
#include "definitions.h"
#include "SetupNavigator.h"
//#include "ModulesOptions.h"


#include <cmath>
#include <TH1F.h>
#include <TH2F.h>
//#include <debug_tools.h>


using modules::parser::GetOneWord;
using namespace std;
using std::string;

extern SourceAnalPulseMap gAnalysedPulseMap;

CalcCoarseTimeOffset::CalcCoarseTimeOffset(modules::options* opts):
  BaseModule("CalcCoarseTimeOffset",opts){    
  fDetNameA = opts->GetString("det1");
  fDetNameB = opts->GetString("det2");
}



CalcCoarseTimeOffset::~CalcCoarseTimeOffset(){
}

////////////////////////////////////////////////

int CalcCoarseTimeOffset::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){

  //call my source finders and build histogram for each source with
  //a time component.
  
  //should I continue to work on just two? or should I have this loop 
  //through all pairs
  
  for(SourceAnalPulseMap::const_iterator sourceIt = gAnalysedPulseMap.begin();
      sourceIt != gAnalysedPulseMap.end(); sourceIt++) {
    if( sourceIt->first.Channel() != fDetNameA) {
      continue;   //check for detector A
    }
    
    //I need to check that this source generates a time
    AnalysedPulseList pulses = sourceIt->second;
    
    for(SourceAnalPulseMap::const_iterator sourceIt2 = gAnalysedPulseMap.begin(); sourceIt2 != gAnalysedPulseMap.end(); sourceIt2++) {
      if(sourceIt2->first.Channel() != fDetNameB)
	continue;  //check for detector B
      if(sourceIt2->first.Generator() != sourceIt->first.Generator())
	continue;  //make sure they have the same generator
      
      //I should make a vector of the sources for detA and detB
      fDetASources.push_back(sourceIt->first);
      fDetBSources.push_back(sourceIt2->first);
    }
  }
  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int CalcCoarseTimeOffset::ProcessEntry(TGlobalData* gData,const TSetupData *setup) {
  for(SourceVector::const_iterator sourceIt = fDetASources.begin(); sourceIt != fDetASources.end(); sourceIt++) {
    const AnalysedPulseList& detAPulses = gAnalysedPulseMap[*sourceIt];
    const AnalysedPulseList& detBPulses = gAnalysedPulseMap[fDetBSources.at(sourceIt - fDetASources.begin())];

    const std::string keyname = sourceIt->str();
    CreateHistogramsIfDontExist(keyname, (*sourceIt).Generator().str());

    for(AnalysedPulseList::const_iterator pulseIt = detAPulses.begin();
	pulseIt != detAPulses.end(); ++pulseIt) {
      double timeA = (*pulseIt)->GetTime();
      for(AnalysedPulseList::const_iterator pulseIt2 = detBPulses.begin();
	  pulseIt2 != detBPulses.end(); ++pulseIt2) {

	double timeB = (*pulseIt2)->GetTime();
	double tDiff = timeA - timeB;

	oned_plots[keyname]->Fill(tDiff);

      }//end detBPulse loop
    }//end detAPulse loop
  }//end sourceIt loop

  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int CalcCoarseTimeOffset::AfterLastEntry(TGlobalData* gData, const TSetupData *setup){
  for (unsigned int i = 0; i < fDetASources.size(); ++i) {
    const std::string keyname = fDetASources[i].str();
    const double dt = oned_plots[keyname]->GetBinCenter(oned_plots[keyname]->GetMaximumBin());
    double sigma = 0;
    SetupNavigator::Instance()->SetCoarseTimeOffset(fDetASources[i], dt, sigma);
  }
  return 0;
}

void CalcCoarseTimeOffset::CreateHistogramsIfDontExist(const std::string& src, const std::string& gen) {
  if( oned_plots.find(src) == oned_plots.end() ) {
    std::string histname = "h" + fDetNameB + "_" + src + "TDiff_1D";
    std::string histtitle = "Time difference between  " + fDetNameA + " and " + fDetNameB + " detectors with the " + gen + " generator";
    TH1F* dplots = new TH1F(histname.c_str(), histtitle.c_str(), 500, -10000, 10000);
    dplots->GetXaxis()->SetTitle("Time Difference (ns)");
    dplots->GetYaxis()->SetTitle("Count");
    oned_plots[src] = dplots;
  }      
}


// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(CalcCoarseTimeOffset,det1,det2);
