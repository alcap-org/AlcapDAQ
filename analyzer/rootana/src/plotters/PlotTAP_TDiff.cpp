#include "PlotTAP_TDiff.h"
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
#include <TF1.h>
//#include <debug_tools.h>

MAKE_EXCEPTION(ModulesOptionError, Base)

extern SourceAnalPulseMap gAnalysedPulseMap;

PlotTAP_TDiff::PlotTAP_TDiff(modules::options* opts) :
  BaseModule("PlotTAP_TDiff",opts),
  fDetNameA(opts->GetString("det1")), fDetNameB(opts->GetString("det2")),
  fTimeLow(opts->GetDouble("time_low",-1.e5)), fTimeHigh(opts->GetDouble("time_high",1.e5)),
  fExportSQL(opts->GetBool("export_sql", false)) {  
  if (fDetNameA == std::string("") || fDetNameB == std::string(""))
    throw Except::ModulesOptionError("Two detectors must be provided");
  //  else if (fDetNameA == fDetNameB)
  //    throw Except::ModulesOptionError((fDetNameA + "==" + fDetNameB).c_str());
  //  else if (fExportSQL && fDetNameB != "SiT-1-F")
  //    throw Except::ModulesOptionError("If exporting to calibration DB, second detector must be SiT-1-F");
}



PlotTAP_TDiff::~PlotTAP_TDiff() {
}

////////////////////////////////////////////////

int PlotTAP_TDiff::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){

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
  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int PlotTAP_TDiff::ProcessEntry(TGlobalData* gData,const TSetupData *setup) {

  for(unsigned int i = 0; i < fDetASources.size(); ++i) {
    const AnalysedPulseList& detAPulses = gAnalysedPulseMap[fDetASources[i]];
    const AnalysedPulseList& detBPulses = gAnalysedPulseMap[fDetBSources[i]];
    const std::vector<TH2F*>& hists = fHists[fDetASources[i].str()];
    const std::vector<TH1F*>& projs = fProjs[fDetASources[i].str()];
    
    for(AnalysedPulseList::const_iterator pulseIt = detAPulses.begin();
	pulseIt != detAPulses.end(); ++pulseIt) {

      for(AnalysedPulseList::const_iterator pulseIt2 = detBPulses.begin();
	  pulseIt2 != detBPulses.end(); ++pulseIt2) {
	double tDiff = (*pulseIt)->GetTime() - (*pulseIt2)->GetTime();
	
	hists[0]->Fill(tDiff, (*pulseIt)->GetAmplitude());
	hists[1]->Fill(tDiff, (*pulseIt2)->GetAmplitude());
	//	hists[2]->Fill(tDiff, (*pulseIt)->GetTime());
	//	hists[3]->Fill(tDiff, (*pulseIt2)->GetTime());

	projs[0]->Fill(tDiff);
	
      }//end detBPulse loop
    }//end detAPulse loop
  }//end sources loop

  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int PlotTAP_TDiff::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
  if (fExportSQL) {
    for (unsigned int i = 0; i < fDetASources.size(); ++i) {
      TH1D* h = fHists[fDetASources[i].str()][0]->ProjectionX();

      int binMax = h->GetMaximumBin();
      int maxPoint = h->GetXaxis()->GetBinCenter(binMax);
      int window_size = 1000;
      
      TF1 * fitter = new TF1("fitter", "gaus", maxPoint - window_size/2, maxPoint + window_size/2);
      fitter->SetParameter(1, maxPoint);
      h->Fit(fitter, "RQ+");
      SetupNavigator::Instance()->SetCoarseTimeOffset(fDetASources[i], fitter->GetParameter(1), fitter->GetParameter(2) );
    }
  }
  return 0;
}

void PlotTAP_TDiff::BookHistograms(const TSetupData* setup) {
  for (unsigned int i = 0; i < fDetASources.size(); ++i) {
    const std::string gen = fDetASources.at(i).Generator().str();
    const int maxAmpA = std::pow(2, setup->GetNBits(setup->GetBankName(fDetNameA)));
    const int maxAmpB = std::pow(2, setup->GetNBits(setup->GetBankName(fDetNameB)));
    std::vector<TH2F*>& hists = fHists[fDetASources.at(i).str()];
    std::vector<TH1F*>& proj = fProjs[fDetASources.at(i).str()];
    
    //ampA plots
    std::string histname("h" + fDetNameB + "_" + fDetASources.at(i).str() + "TDiff_AmpA");
    std::string histtitle("Amplitude of " + fDetNameA + " vs time difference with " + fDetNameB + " detectors with the " + gen + " generator;t_{" + fDetNameA + "} - t_{" + fDetNameB + "} (ns);Amplitude of " + fDetNameA + " [ADC]");
    hists.push_back(new TH2F(histname.c_str(), histtitle.c_str(), 100, fTimeLow, fTimeHigh, 100, 0, maxAmpA));

    //ampB plots
    histname = "h" + fDetNameB + "_" + fDetASources.at(i).str() + " TDiff_AmpB";
    histtitle = "Amplitude of " + fDetNameB + " vs time difference with " + fDetNameA + " detectors with the " + gen + " generator;t_{" + fDetNameA + "} - t_{" + fDetNameB + "} (ns);Amplitude of " + fDetNameB + " [ADC]";
    hists.push_back(new TH2F(histname.c_str(), histtitle.c_str(), 100, fTimeLow, fTimeHigh, 100, 0, maxAmpB));

    //intA plots
    /*    histname = "h" + fDetNameB + "_" + fDetASources.at(i).str() + " TDiff_TimeA";
    histtitle = "Time of " + fDetNameA + " vs time difference with " + fDetNameB + " detectors with the " + gen + " generator;t_{" + fDetNameA + "} - t_{" + fDetNameB + "} (ns);Time of " + fDetNameA + " [ns]";
    hists.push_back(new TH2F(histname.c_str(), histtitle.c_str(), 100, fTimeLow, fTimeHigh, 1000, 0, 120e6));

    //intB plots
    histname = "h" + fDetNameB + "_" + fDetASources.at(i).str() + " TDiff_TimeB";
    histtitle = "Time of " + fDetNameB + " vs time difference with " + fDetNameA + " detectors with the " + gen + " generator;t_{" + fDetNameA + "} - t_{" + fDetNameB + "} (ns);Time of " + fDetNameB + " [ns]";
    hists.push_back(new TH2F(histname.c_str(), histtitle.c_str(), 100, fTimeLow, fTimeHigh, 1000, 0, 120e6));
    */
    //projection
    histname = "h" + fDetNameB + "_" + fDetASources.at(i).str() + "TDiff";
    histtitle = "Time difference of " + fDetNameA + " vs " + fDetNameB + " detectors with the " + gen + " generator;t_{" + fDetNameA + "} - t_{" + fDetNameB + "} (ns);Amplitude of " + fDetNameA + " [ADC]";
    proj.push_back(new TH1F(histname.c_str(), histtitle.c_str(), 100, fTimeLow, fTimeHigh) );
  }
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(PlotTAP_TDiff,det1,det2,time_low,time_high,export_sql);
