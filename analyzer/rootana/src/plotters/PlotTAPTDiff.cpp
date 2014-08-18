#include "PlotTAPTDiff.h"
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
#include "ModulesOptions.h"
#include "AlcapExcept.h"


#include <cmath>
#include <TH1F.h>
#include <TH2F.h>
//#include <debug_tools.h>

MAKE_EXCEPTION(ModulesOptionError, Base)

extern SourceAnalPulseMap gAnalysedPulseMap;

PlotTAPTDiff::PlotTAPTDiff(modules::options* opts) :
  BaseModule("PlotTAPTDiff",opts),
  fDetNameA(opts->GetString("det1")), fDetNameB(opts->GetString("det2")),
  fTimeLow(opts->GetDouble("time_low",-1.e5)), fTimeHigh(opts->GetDouble("time_high",1.e5)),
  fExportSQL(opts->GetBool("export_sql", false)) {
  if (fDetNameA == std::string("") || fDetNameB == std::string(""))
    throw Except::ModulesOptionError("Two detectors must be provided");
  else if (fDetNameA == fDetNameB)
    throw Except::ModulesOptionError((fDetNameA + "==" + fDetNameB).c_str());
  else if (fExportSQL && fDetNameB != "muSc")
    throw Except::ModulesOptionError("If exporting to calibration DB, second detector must be muSc");
}



PlotTAPTDiff::~PlotTAPTDiff() {
}

////////////////////////////////////////////////

int PlotTAPTDiff::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){

  for(SourceAnalPulseMap::const_iterator sourceIt = gAnalysedPulseMap.begin();
      sourceIt != gAnalysedPulseMap.end(); sourceIt++) {

    if( sourceIt->first.Channel() != fDetNameA)
      continue;   //check for detector A
    
    for(SourceAnalPulseMap::const_iterator sourceIt2 = gAnalysedPulseMap.begin(); sourceIt2 != gAnalysedPulseMap.end(); ++sourceIt2)
      {
	if(sourceIt2->first.Channel() != fDetNameB)
	  continue;  //check for detector B
	if(sourceIt2->first.Generator() != sourceIt->first.Generator())
	  continue;  //make sure they have the same generator

	//I should make a vector of the sources for detA and detB
	fDetASources.push_back(sourceIt->first);
	fDetBSources.push_back(sourceIt2->first);
	break;
      }
  }
  BookHistograms(setup);
  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int PlotTAPTDiff::ProcessEntry(TGlobalData* gData,const TSetupData *setup) {

  for(unsigned int i = 0; i < fDetASources.size(); ++i) {
    const AnalysedPulseList& detAPulses = gAnalysedPulseMap[fDetASources[i]];
    const AnalysedPulseList& detBPulses = gAnalysedPulseMap[fDetBSources[i]];
    const std::string& key = fDetASources[i].str();
    
    for(AnalysedPulseList::const_iterator pulseIt = detAPulses.begin();
	pulseIt != detAPulses.end(); ++pulseIt) {
      
      for(AnalysedPulseList::const_iterator pulseIt2 = detBPulses.begin();
	  pulseIt2 != detBPulses.end(); ++pulseIt2) {
	double tDiff = (*pulseIt)->GetTime() - (*pulseIt2)->GetTime();
	
	ampA_plots[key]->Fill(tDiff, (*pulseIt)->GetAmplitude());
	ampB_plots[key]->Fill(tDiff, (*pulseIt2)->GetAmplitude());
	intA_plots[key]->Fill(tDiff, (*pulseIt)->GetIntegral());
	intB_plots[key]->Fill(tDiff, (*pulseIt2)->GetIntegral());
	
      }//end detBPulse loop
    }//end detAPulse loop
  }//end sources loop

  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int PlotTAPTDiff::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
  if (fExportSQL) {
    for (unsigned int i = 0; i < fDetASources.size(); ++i) {
      TH1D* h = ampA_plots[fDetASources[i].str()]->ProjectionX();
      SetupNavigator::Instance()->SetCoarseTimeOffset(fDetASources[i], h->GetBinCenter(h->GetMaximumBin()));
    }
  }
  return 0;
}

void PlotTAPTDiff::BookHistograms(const TSetupData* setup) {
  for (unsigned int i = 0; i < fDetASources.size(); ++i) {
    const std::string key = fDetASources.at(i).str();
    const std::string gen = fDetASources.at(i).Generator().str();
    const int maxAmpA = std::pow(2, setup->GetNBits(setup->GetBankName(fDetNameA)));
    const int maxAmpB = std::pow(2, setup->GetNBits(setup->GetBankName(fDetNameB)));

    //ampA plots
    std::string histname("h" + fDetNameB + "_" + key + "TDiff_AmpA");
    std::string histtitle("Amplitude of " + fDetNameA + " vs time difference with " + fDetNameB + " detectors with the " + gen + " generator;Time Difference (ns);Amplitude (ADC counts)");
    ampA_plots[key] = new TH2F(histname.c_str(), histtitle.c_str(), 200, fTimeLow, fTimeHigh, 200, 0, maxAmpA);


    //ampB plots
    histname = "h" + fDetNameB + "_" + key + " TDiff_AmpB";
    histtitle = "Amplitude of " + fDetNameB + " vs time difference with " + fDetNameA + " detectors with the " + gen + " generator;Time Difference (ns);Amplitude (ADC counts)";
    ampB_plots[key] = new TH2F(histname.c_str(), histtitle.c_str(), 200, fTimeLow, fTimeHigh, 200, 0, maxAmpB);
 
    //intA plots
    histname = "h" + fDetNameB + "_" + key + " TDiff_IntA";
    histtitle = "Integral of " + fDetNameA + " vs time difference with " + fDetNameB + " detectors with the " + gen + " generator;Time Difference (ns);Integral (ADC counts)";
    intA_plots[key] = new TH2F(histname.c_str(), histtitle.c_str(), 200, fTimeLow, fTimeHigh, 200, 0, 5*maxAmpA);
      
    //intB plots
    histname = "h" + fDetNameB + "_" + key + " TDiff_IntB";
    histtitle = "Integral of " + fDetNameB + " vs time difference with " + fDetNameA + " detectors with the " + gen + " generator;Time Difference (ns);Integral (ADC counts)";
    intB_plots[key] = new TH2F(histname.c_str(), histtitle.c_str(), 200, fTimeLow, fTimeHigh, 200, 0, 5*maxAmpB);
    
  }
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(PlotTAPTDiff,det1,det2,time_low,time_high,export_sql);
