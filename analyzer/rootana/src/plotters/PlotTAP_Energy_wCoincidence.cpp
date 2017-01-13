#include "PlotTAP_Energy_wCoincidence.h"
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

#include "ExportPulse.h"


#include <cmath>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <sstream>
//#include <debug_tools.h>

MAKE_EXCEPTION(ModulesOptionError, Base)

extern SourceAnalPulseMap gAnalysedPulseMap;

PlotTAP_Energy_wCoincidence::PlotTAP_Energy_wCoincidence(modules::options* opts) :
  BaseModule("PlotTAP_Energy_wCoincidence",opts),
  fDetNameA(opts->GetString("det1")), fDetNameB(opts->GetString("det2")),
  fCoincidenceTimeLow(opts->GetDouble("coincidence_time_low")), fCoincidenceTimeHigh(opts->GetDouble("coincidence_time_high")),
  fExportPulses(opts->GetBool("export_pulse", false)), fTotalPulsesExported(0) {  
  if (fDetNameA == std::string("") || fDetNameB == std::string(""))
    throw Except::ModulesOptionError("Two detectors must be provided");
  else if (fDetNameA == fDetNameB)
    throw Except::ModulesOptionError((fDetNameA + "==" + fDetNameB).c_str());
  //  else if (fExportSQL && fDetNameB != "SiT-1-F")
  //    throw Except::ModulesOptionError("If exporting to calibration DB, second detector must be SiT-1-F");
}



PlotTAP_Energy_wCoincidence::~PlotTAP_Energy_wCoincidence() {
}

////////////////////////////////////////////////

int PlotTAP_Energy_wCoincidence::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){

  for(SourceAnalPulseMap::const_iterator sourceIt = gAnalysedPulseMap.begin();
      sourceIt != gAnalysedPulseMap.end(); sourceIt++) {

    if( sourceIt->first.Channel() != fDetNameA)
      continue;   //check for detector A
    
    for(SourceAnalPulseMap::const_iterator sourceIt2 = gAnalysedPulseMap.begin(); sourceIt2 != gAnalysedPulseMap.end(); ++sourceIt2)
      {
	if(sourceIt2->first.Channel() != fDetNameB)
	  continue;  //check for detector B

	//I should make a vector of the sources for detA and detB
	fDetASources.push_back(sourceIt->first);
	fDetBSources.push_back(sourceIt2->first);
	break;
      }
  }
  BookHistograms(setup);

   // Check we're also running with the ExportPulse module
   if(fExportPulses && !ExportPulse::Instance()){
     std::cout<<"PlotTAP_Energy_wCoincidence: Error: You need to run with the ExportPulse module to be able to export pulses"<<std::endl;
     return 1;
   }

  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int PlotTAP_Energy_wCoincidence::ProcessEntry(TGlobalData* gData,const TSetupData *setup) {

  for(unsigned int i = 0; i < fDetASources.size(); ++i) {
    const AnalysedPulseList& detAPulses = gAnalysedPulseMap[fDetASources[i]];
    const AnalysedPulseList& detBPulses = gAnalysedPulseMap[fDetBSources[i]];
    const std::vector<TH2F*>& hists = fHists[fDetASources[i].str()];
    
    for(AnalysedPulseList::const_iterator pulseIt = detAPulses.begin();
	pulseIt != detAPulses.end(); ++pulseIt) {

      for(AnalysedPulseList::const_iterator pulseIt2 = detBPulses.begin();
	  pulseIt2 != detBPulses.end(); ++pulseIt2) {
	double tDiff = (*pulseIt)->GetTime() - (*pulseIt2)->GetTime();

	if (tDiff >= fCoincidenceTimeLow && tDiff <= fCoincidenceTimeHigh) {
	  hists[0]->Fill((*pulseIt)->GetEnergy(), (*pulseIt2)->GetEnergy());

	  if(fExportPulses && fTotalPulsesExported <= 40) {
	    ExportPulse::Instance()->AddToExportList(*pulseIt);
	    ExportPulse::Instance()->AddToExportList(*pulseIt2);
	    fTotalPulsesExported += 2;
	  }
	}
      }//end detBPulse loop
    }//end detAPulse loop
  }//end sources loop

  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int PlotTAP_Energy_wCoincidence::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
  return 0;
}

void PlotTAP_Energy_wCoincidence::BookHistograms(const TSetupData* setup) {
  for (unsigned int i = 0; i < fDetASources.size(); ++i) {
    const std::string gen = fDetASources.at(i).Generator().str();
    double gainA  = 1;
    double offsetA= 0;
    try{
      gainA  = SetupNavigator::Instance()->GetAdcToEnergyGain(fDetNameA);
      offsetA= SetupNavigator::Instance()->GetAdcToEnergyConstant(fDetNameA);
    }catch( Except::InvalidDetector& e){};
    double gainB  = 1;
    double offsetB= 0;
    try{
      gainB  = SetupNavigator::Instance()->GetAdcToEnergyGain(fDetNameB);
      offsetB= SetupNavigator::Instance()->GetAdcToEnergyConstant(fDetNameB);
    }catch( Except::InvalidDetector& e){};

    const int maxAmpA = std::pow(2, setup->GetNBits(setup->GetBankName(fDetNameA)));
    const int maxAmpB = std::pow(2, setup->GetNBits(setup->GetBankName(fDetNameB)));
    std::vector<TH2F*>& hists = fHists[fDetASources.at(i).str()];
    
    //within coincidence plots
    std::string histname("h" + fDetNameB + "_" + fDetNameA + "_inCoincidence");
    std::stringstream histtitle;
    histtitle << "Energy of " << fDetNameA << " vs Energy " << fDetNameB << " detectors with tdiff > " << fCoincidenceTimeLow << " && tdiff < " << fCoincidenceTimeHigh << gen << " generator;E_{" << fDetNameA << "} [keV];E_{" << fDetNameB << "} [keV]";
    hists.push_back(new TH2F(histname.c_str(), histtitle.str().c_str(), maxAmpA,0,gainA*maxAmpA+offsetA, maxAmpB/16, 0, gainB*maxAmpB+offsetB));

  }
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(PlotTAP_Energy_wCoincidence,det1,det2,coincidence_time_low,coincidence_time_high,export_pulse);
