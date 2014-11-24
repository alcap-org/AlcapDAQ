#include "PlotTAP_selfCorrelation.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "TAnalysedPulse.h"
#include <TH2F.h>

#include <iostream>
#include <cmath>
using std::cout;
using std::endl;

extern SourceAnalPulseMap gAnalysedPulseMap;

PlotTAP_selfCorrelation::PlotTAP_selfCorrelation(modules::options* opts):
  BaseModule("PlotTAP_selfCorrelation",opts),
  fDetNameA(opts->GetString("det1")), fDetNameB(opts->GetString("det2")),
  fTimeLow(opts->GetDouble("time_low",-1.e5)),
  fTimeHigh(opts->GetDouble("time_high",1.e5)),
  fMinAmpA(opts->GetDouble("min_amp",0)),
  fMaxAmpA(opts->GetDouble("max_amp",1e20)),
  fMuScWindow(opts->GetDouble("musc_window",10)*1e3){
}

PlotTAP_selfCorrelation::~PlotTAP_selfCorrelation(){
}

int PlotTAP_selfCorrelation::BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup){

IDs::channel muSc("muSc");
  for(SourceAnalPulseMap::const_iterator sourceIt = gAnalysedPulseMap.begin();
      sourceIt != gAnalysedPulseMap.end(); sourceIt++) {

    // only use the last matching muSc source for now
    if( sourceIt->first.matches(muSc)) fMuScSource=sourceIt->first;

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

int PlotTAP_selfCorrelation::ProcessEntry(TGlobalData* gData,const TSetupData *setup){
  const AnalysedPulseList& muScPulseList=gAnalysedPulseMap.at(fMuScSource);
  for(unsigned int i = 0; i < fDetASources.size(); ++i) {
    const AnalysedPulseList& detAPulses = gAnalysedPulseMap[fDetASources[i]];
    const AnalysedPulseList& detBPulses = gAnalysedPulseMap[fDetBSources[i]];
    const std::vector<TH2F*>& hists = fHists[fDetASources[i].str()];
    
    for(AnalysedPulseList::const_iterator pulseIt = detAPulses.begin();
	pulseIt != detAPulses.end(); ++pulseIt) {
      if(!PassesCutA(*pulseIt)) continue;
      if(!PassesMuScCut(*pulseIt, muScPulseList)) continue;
      
      for(AnalysedPulseList::const_iterator pulseIt2 = detBPulses.begin();
	  pulseIt2 != detBPulses.end(); ++pulseIt2) {
	double tDiff = (*pulseIt)->GetTime() - (*pulseIt2)->GetTime();
	
	hists[0]->Fill(tDiff, (*pulseIt)->GetAmplitude());
	hists[1]->Fill(tDiff, (*pulseIt2)->GetAmplitude());
	hists[2]->Fill(tDiff, (*pulseIt)->GetIntegral());
	hists[3]->Fill(tDiff, (*pulseIt2)->GetIntegral());
	
      }//end detBPulse loop
    }//end detAPulse loop
  }//end sources loop

  return 0;
}

bool PlotTAP_selfCorrelation::PassesCutA(const TAnalysedPulse* tap)const{
  const double amp=tap->GetAmplitude();
  if(amp < fMinAmpA || amp > fMaxAmpA) return false;
  return true;
}

bool PlotTAP_selfCorrelation::PassesMuScCut(const TAnalysedPulse* tap, const AnalysedPulseList& muScPulseList)const{
  return true;
}

int PlotTAP_selfCorrelation::AfterLastEntry(TGlobalData* gData,const TSetupData *setup){
  return 0;
}

void PlotTAP_selfCorrelation::BookHistograms(const TSetupData* setup) {
  for (unsigned int i = 0; i < fDetASources.size(); ++i) {
    const std::string gen = fDetASources.at(i).Generator().str();
    const int maxAmpA = std::pow(2, setup->GetNBits(setup->GetBankName(fDetNameA)));
    const int maxAmpB = std::pow(2, setup->GetNBits(setup->GetBankName(fDetNameB)));
    std::vector<TH2F*>& hists = fHists[fDetASources.at(i).str()];
    
    //ampA plots
    std::string histname("hTDiff_AmpA");
    std::string histtitle="Amplitude of " + fDetNameA + " vs time difference with " + fDetNameB 
              + " detectors with the " + gen + " generator;Time Difference (ns);Amplitude (ADC counts)";
    hists.push_back(new TH2F(histname.c_str(), histtitle.c_str(), 200, fTimeLow, fTimeHigh, 200, 0, maxAmpA));

    //ampB plots
    histname = "hTDiff_AmpB";
    histtitle = "Amplitude of " + fDetNameB + " vs time difference with " + fDetNameA 
              + " detectors with the " + gen + " generator;Time Difference (ns);Amplitude (ADC counts)";
    hists.push_back(new TH2F(histname.c_str(), histtitle.c_str(), 200, fTimeLow, fTimeHigh, 200, 0, maxAmpB));

    //intA plots
    histname = "hTDiff_IntA";
    histtitle = "Integral of " + fDetNameA + " vs time difference with " + fDetNameB 
              + " detectors with the " + gen + " generator;Time Difference (ns);Integral (ADC counts)";
    hists.push_back(new TH2F(histname.c_str(), histtitle.c_str(), 200, fTimeLow, fTimeHigh, 200, 0, 5*maxAmpA));

    //intB plots
    histname = "hTDiff_IntB";
    histtitle = "Integral of " + fDetNameB + " vs time difference with " + fDetNameA 
              + " detectors with the " + gen + " generator;Time Difference (ns);Integral (ADC counts)";
    hists.push_back(new TH2F(histname.c_str(), histtitle.c_str(), 200, fTimeLow, fTimeHigh, 200, 0, 5*maxAmpB));
    
  }
}

ALCAP_REGISTER_MODULE(PlotTAP_selfCorrelation ,det1,det2,time_low,time_high,min_amp,max_amp);
