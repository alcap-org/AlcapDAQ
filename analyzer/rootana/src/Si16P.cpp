#include "Si16P.h"
#include "RegisterModule.inc"
#include "TPulseIsland.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "IdGenerator.h"
#include "SetupNavigator.h"
#include "ExportPulse.h"
#include "PulseCandidateFinder.h"

#include "TH1I.h"
#include "TH2D.h"
#include "TF1.h"
#include "TDirectory.h"

#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <climits>
#include <string>
#include <sstream>

const IDs::channel Si16P::fMuSc  (IDs::kMuSc,   IDs::kNotApplicable);
const IDs::channel Si16P::fSiR11S(IDs::kSiR1_1, IDs::kSlow);
const IDs::channel Si16P::fSiR12S(IDs::kSiR1_2, IDs::kSlow);
const IDs::channel Si16P::fSiR13S(IDs::kSiR1_3, IDs::kSlow);
const IDs::channel Si16P::fSiR14S(IDs::kSiR1_4, IDs::kSlow);
const IDs::channel Si16P::fSiR2S (IDs::kSiR2,   IDs::kSlow);
const IDs::channel Si16P::fSiL11S(IDs::kSiL1_1, IDs::kSlow);
const IDs::channel Si16P::fSiL12S(IDs::kSiL1_2, IDs::kSlow);
const IDs::channel Si16P::fSiL13S(IDs::kSiL1_3, IDs::kSlow);
const IDs::channel Si16P::fSiL14S(IDs::kSiL1_4, IDs::kSlow);
const IDs::channel Si16P::fSiL2S (IDs::kSiL2,   IDs::kSlow);

const IDs::generator Si16P::fGenerator("FirstComplete", "{constant_fraction=0.60}");
const double         Si16P::fConstantFraction(0.60);

Si16P::Si16P(modules::options* opts) :
  BaseModule("Si16P",opts),
  fhPIDRight(NULL), fhPIDLeft(NULL),
  fMBAmp_MuSc  (SetupNavigator::Instance()->GetPedestal(fMuSc),   TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fMuSc  .str()))),
  fMBAmp_SiR11S(SetupNavigator::Instance()->GetPedestal(fSiR11S), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR11S.str()))),
  fMBAmp_SiR12S(SetupNavigator::Instance()->GetPedestal(fSiR12S), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR12S.str()))),
  fMBAmp_SiR13S(SetupNavigator::Instance()->GetPedestal(fSiR13S), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR13S.str()))),
  fMBAmp_SiR14S(SetupNavigator::Instance()->GetPedestal(fSiR14S), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR14S.str()))),
  fMBAmp_SiR2S (SetupNavigator::Instance()->GetPedestal(fSiR2S),  TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR2S .str()))),
  fMBAmp_SiL11S(SetupNavigator::Instance()->GetPedestal(fSiL11S), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL11S.str()))),
  fMBAmp_SiL12S(SetupNavigator::Instance()->GetPedestal(fSiL12S), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL12S.str()))),
  fMBAmp_SiL13S(SetupNavigator::Instance()->GetPedestal(fSiL13S), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL13S.str()))),
  fMBAmp_SiL14S(SetupNavigator::Instance()->GetPedestal(fSiL14S), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL14S.str()))),
  fMBAmp_SiL2S (SetupNavigator::Instance()->GetPedestal(fSiL2S),  TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL2S .str()))),
  fCFTime_MuSc (SetupNavigator::Instance()->GetPedestal(fMuSc),
		TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fMuSc.str())),
		TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fMuSc.str())),
		0.,
		Si16P::fConstantFraction),
  fCFTime_SiR11S(SetupNavigator::Instance()->GetPedestal(fSiR11S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR11S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiR11S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiR11S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fCFTime_SiR12S(SetupNavigator::Instance()->GetPedestal(fSiR12S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR12S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiR12S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiR12S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fCFTime_SiR13S(SetupNavigator::Instance()->GetPedestal(fSiR13S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR13S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiR13S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiR13S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fCFTime_SiR14S(SetupNavigator::Instance()->GetPedestal(fSiR14S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR14S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiR14S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiR14S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fCFTime_SiR2S (SetupNavigator::Instance()->GetPedestal(fSiR2S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiR2S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiR2S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiR2S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fCFTime_SiL11S(SetupNavigator::Instance()->GetPedestal(fSiL11S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL11S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiL11S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiL11S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fCFTime_SiL12S(SetupNavigator::Instance()->GetPedestal(fSiL12S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL12S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiL12S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiL12S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fCFTime_SiL13S(SetupNavigator::Instance()->GetPedestal(fSiL13S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL13S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiL13S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiL13S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fCFTime_SiL14S(SetupNavigator::Instance()->GetPedestal(fSiL14S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL14S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiL14S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiL14S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fCFTime_SiL2S (SetupNavigator::Instance()->GetPedestal(fSiL2S),
		 TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fSiL2S.str())),
		 TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fSiL2S.str())),
		 SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(Si16P::fSiL2S, Si16P::fGenerator)),
		 Si16P::fConstantFraction),
  fADC2E_SiR11S(new TF1("adc2e_sir11s", "[0]+[1]*x")),
  fADC2E_SiR12S(new TF1("adc2e_sir12s", "[0]+[1]*x")),
  fADC2E_SiR13S(new TF1("adc2e_sir13s", "[0]+[1]*x")),
  fADC2E_SiR14S(new TF1("adc2e_sir14s", "[0]+[1]*x")),
  fADC2E_SiR2S (new TF1("adc2e_sir2s",  "[0]+[1]*x")),
  fADC2E_SiL11S(new TF1("adc2e_sil11s", "[0]+[1]*x")),
  fADC2E_SiL12S(new TF1("adc2e_sil12s", "[0]+[1]*x")),
  fADC2E_SiL13S(new TF1("adc2e_sil13s", "[0]+[1]*x")),
  fADC2E_SiL14S(new TF1("adc2e_sil14s", "[0]+[1]*x")),
  fADC2E_SiL2S (new TF1("adc2e_sil2s",  "[0]+[1]*x")),
  fdTMuScPP (opts->GetDouble("tcut_musc")),    fdTPID        (opts->GetDouble("tcut_pid")),
  fdTScatter(opts->GetDouble("tcut_scatter")), fdTSiLow      (opts->GetDouble("tcut_si_low")),
  fdTSiHigh (opts->GetDouble("tcut_si_high")), fMuonEnergyCut(opts->GetDouble("adccut_musc")) {

  const int nbins_dE = std::pow(2., 12);
  const int nbins_E  = std::pow(2., 12);

  // Set ADC->E functions' parameters
  const std::pair<double,double> adc2epar_sir11s = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiR11S);
  const std::pair<double,double> adc2epar_sir12s = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiR12S);
  const std::pair<double,double> adc2epar_sir13s = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiR13S);
  const std::pair<double,double> adc2epar_sir14s = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiR14S);
  const std::pair<double,double> adc2epar_sir2s  = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiR2S );
  const std::pair<double,double> adc2epar_sil11s = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiL11S);
  const std::pair<double,double> adc2epar_sil12s = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiL12S);
  const std::pair<double,double> adc2epar_sil13s = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiL13S);
  const std::pair<double,double> adc2epar_sil14s = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiL14S);
  const std::pair<double,double> adc2epar_sil2s  = SetupNavigator::Instance()->GetEnergyCalibrationConstants(Si16P::fSiL2S );
  fADC2E_SiR11S->SetParameters(adc2epar_sir11s.first, adc2epar_sir11s.second);
  fADC2E_SiR12S->SetParameters(adc2epar_sir12s.first, adc2epar_sir12s.second);
  fADC2E_SiR13S->SetParameters(adc2epar_sir13s.first, adc2epar_sir13s.second);
  fADC2E_SiR14S->SetParameters(adc2epar_sir14s.first, adc2epar_sir14s.second);
  fADC2E_SiR2S ->SetParameters(adc2epar_sir2s .first, adc2epar_sir2s .second);
  fADC2E_SiL11S->SetParameters(adc2epar_sil11s.first, adc2epar_sil11s.second);
  fADC2E_SiL12S->SetParameters(adc2epar_sil12s.first, adc2epar_sil12s.second);
  fADC2E_SiL13S->SetParameters(adc2epar_sil13s.first, adc2epar_sil13s.second);
  fADC2E_SiL14S->SetParameters(adc2epar_sil14s.first, adc2epar_sil14s.second);
  fADC2E_SiL2S ->SetParameters(adc2epar_sil2s .first, adc2epar_sil2s .second);

  TDirectory* cwd = TDirectory::CurrentDirectory();
  dir->cd();
  fhPIDRight = new TH2D("hPIDRight", "PID (Right);E+dE (keV);dE (keV);Counts",
			    nbins_E,  fADC2E_SiR2S->Eval(0.),  fADC2E_SiR2S->Eval(nbins_E),
			    nbins_dE, fADC2E_SiR11S->Eval(0.), fADC2E_SiR11S->Eval(nbins_dE));
  fhPIDLeft  = new TH2D("hPIDLeft",  "PID (Right);E+dE (keV);dE (keV);Counts",
			    nbins_E,  fADC2E_SiL2S->Eval(0.),  fADC2E_SiL2S->Eval(nbins_E),
			    nbins_dE, fADC2E_SiL11S->Eval(0.), fADC2E_SiL11S->Eval(nbins_dE));
  cwd->cd();
  ThrowIfInputsInsane(opts);
}

Si16P::~Si16P(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int Si16P::BeforeFirstEntry(TGlobalData* gData, const TSetupData *setup){
  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int Si16P::ProcessEntry(TGlobalData* gData, const TSetupData *setup){

  const std::map< std::string, std::vector<TPulseIsland*> >& TPIMap = gData->fPulseIslandToChannelMap;

  static const std::string bank_musc   = TSetupData::Instance()->GetBankName(Si16P::fMuSc.str());
  static const std::string bank_sir11s = TSetupData::Instance()->GetBankName(Si16P::fSiR11S.str());
  static const std::string bank_sir12s = TSetupData::Instance()->GetBankName(Si16P::fSiR12S.str());
  static const std::string bank_sir13s = TSetupData::Instance()->GetBankName(Si16P::fSiR13S.str());
  static const std::string bank_sir14s = TSetupData::Instance()->GetBankName(Si16P::fSiR14S.str());
  static const std::string bank_sir2s  = TSetupData::Instance()->GetBankName(Si16P::fSiR2S.str());
  static const std::string bank_sil11s = TSetupData::Instance()->GetBankName(Si16P::fSiL11S.str());
  static const std::string bank_sil12s = TSetupData::Instance()->GetBankName(Si16P::fSiL12S.str());
  static const std::string bank_sil13s = TSetupData::Instance()->GetBankName(Si16P::fSiL13S.str());
  static const std::string bank_sil14s = TSetupData::Instance()->GetBankName(Si16P::fSiL14S.str());
  static const std::string bank_sil2s  = TSetupData::Instance()->GetBankName(Si16P::fSiL2S.str());

  std::vector< std::vector<double> > siR1STimes, siL1STimes;
  std::vector<double> muScTimes = CalculateTimes(Si16P::fMuSc, TPIMap.at(bank_musc));
  siR1STimes.push_back(CalculateTimes(Si16P::fSiR11S, TPIMap.at(bank_sir11s)));
  siR1STimes.push_back(CalculateTimes(Si16P::fSiR12S, TPIMap.at(bank_sir12s)));
  siR1STimes.push_back(CalculateTimes(Si16P::fSiR13S, TPIMap.at(bank_sir13s)));
  siR1STimes.push_back(CalculateTimes(Si16P::fSiR14S, TPIMap.at(bank_sir14s)));
  const std::vector<double> siR2STimes = CalculateTimes(Si16P::fSiR2S, TPIMap.at(bank_sir2s));
  siL1STimes.push_back(CalculateTimes(Si16P::fSiL11S, TPIMap.at(bank_sil11s)));
  siL1STimes.push_back(CalculateTimes(Si16P::fSiL12S, TPIMap.at(bank_sil12s)));
  siL1STimes.push_back(CalculateTimes(Si16P::fSiL13S, TPIMap.at(bank_sil13s)));
  siL1STimes.push_back(CalculateTimes(Si16P::fSiL14S, TPIMap.at(bank_sil14s)));
  const std::vector<double> siL2STimes = CalculateTimes(Si16P::fSiL2S, TPIMap.at(bank_sil2s));

  std::vector< std::vector<double> > siR1SEnergies, siL1SEnergies;
  std::vector<double> muScEnergies = CalculateEnergies(Si16P::fMuSc, TPIMap.at(bank_musc));
  siR1SEnergies.push_back(CalculateEnergies(Si16P::fSiR11S, TPIMap.at(bank_sir11s)));
  siR1SEnergies.push_back(CalculateEnergies(Si16P::fSiR12S, TPIMap.at(bank_sir12s)));
  siR1SEnergies.push_back(CalculateEnergies(Si16P::fSiR13S, TPIMap.at(bank_sir13s)));
  siR1SEnergies.push_back(CalculateEnergies(Si16P::fSiR14S, TPIMap.at(bank_sir14s)));
  const std::vector<double> siR2SEnergies = CalculateEnergies(Si16P::fSiR2S, TPIMap.at(bank_sir2s));
  siL1SEnergies.push_back(CalculateEnergies(Si16P::fSiL11S, TPIMap.at(bank_sil11s)));
  siL1SEnergies.push_back(CalculateEnergies(Si16P::fSiL12S, TPIMap.at(bank_sil12s)));
  siL1SEnergies.push_back(CalculateEnergies(Si16P::fSiL13S, TPIMap.at(bank_sil13s)));
  siL1SEnergies.push_back(CalculateEnergies(Si16P::fSiL14S, TPIMap.at(bank_sil14s)));
  const std::vector<double> siL2SEnergies = CalculateEnergies(Si16P::fSiL2S, TPIMap.at(bank_sil2s));


  // Check for no hits.
  bool rempty = true, lempty = true;
  for (unsigned int i = 0; i < siR1STimes.size(); ++i)
    rempty &= siR1STimes[i].empty();
  for (unsigned int i = 0; i < siL1STimes.size(); ++i)
    lempty &= siL1STimes[i].empty();
  if ( muScTimes.empty()              ||
       (rempty && lempty)             ||
       (rempty && siL2STimes.empty()) ||
       (lempty && siR2STimes.empty())    )
    return 0;

  RemoveMuScPileUp(muScTimes, muScEnergies);
  RemoveElectrons (muScTimes, muScEnergies);

  RemoveScatteredMuons(muScTimes, muScEnergies, siR1STimes);
  RemoveScatteredMuons(muScTimes, muScEnergies, siR2STimes);
  RemoveScatteredMuons(muScTimes, muScEnergies, siL1STimes);
  RemoveScatteredMuons(muScTimes, muScEnergies, siL2STimes);

  RemoveMultipleDetectorHits(muScTimes, muScEnergies, siR1STimes);
  RemoveMultipleDetectorHits(muScTimes, muScEnergies, siR2STimes);
  RemoveMultipleDetectorHits(muScTimes, muScEnergies, siL1STimes);
  RemoveMultipleDetectorHits(muScTimes, muScEnergies, siL2STimes);


  //*************************//
  //**** Find Candidates ****//
  //*************************//
  for (unsigned int iMuSc = 0; iMuSc < muScTimes.size(); ++iMuSc) {

    // Get first hit after muSc
    unsigned int iSiR2S  = std::upper_bound(siR2STimes.begin(),  siR2STimes.end(),  muScTimes[iMuSc]) - siR2STimes.begin();
    unsigned int iSiL2S  = std::upper_bound(siL2STimes.begin(),  siL2STimes.end(),  muScTimes[iMuSc]) - siL2STimes.begin();

    if ( iSiR2S < siR2STimes.size() &&
	 siR2STimes[iSiR2S] - muScTimes[iMuSc] > fdTSiLow  &&
	 siR2STimes[iSiR2S] - muScTimes[iMuSc] < fdTSiHigh ) {
      double e  = siR2SEnergies[iSiR2S];
      double de = FindFirstMatchingThinHit(siR2STimes[iSiR2S], siR1STimes, siR1SEnergies);
      if (de > 0)
	fhPIDRight->Fill(e+de, de);
    }
    if ( iSiL2S < siL2STimes.size() &&
	 siL2STimes[iSiL2S] - muScTimes[iMuSc] > fdTSiLow  &&
	 siL2STimes[iSiL2S] - muScTimes[iMuSc] < fdTSiHigh ) {
      double e  = siR2SEnergies[iSiR2S];
      double de = FindFirstMatchingThinHit(siL2STimes[iSiL2S], siL1STimes, siL1SEnergies);
      if (de > 0)
	fhPIDLeft->Fill(e+de, de);
    }    
  }
  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int Si16P::AfterLastEntry(TGlobalData* gData, const TSetupData *setup){
  return 0;
}


std::vector<double> Si16P::CalculateTimes(const IDs::channel& ch, const std::vector<TPulseIsland*>& tpis) {
  std::vector<double> t;
  if      (ch == Si16P::fMuSc)   for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_MuSc  (tpis[i]));
  else if (ch == Si16P::fSiR11S) for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiR11S(tpis[i]));
  else if (ch == Si16P::fSiR12S) for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiR12S(tpis[i]));
  else if (ch == Si16P::fSiR13S) for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiR13S(tpis[i]));
  else if (ch == Si16P::fSiR14S) for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiR14S(tpis[i]));
  else if (ch == Si16P::fSiR2S)  for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiR2S (tpis[i]));
  else if (ch == Si16P::fSiL11S) for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiL11S(tpis[i]));
  else if (ch == Si16P::fSiL12S) for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiL12S(tpis[i]));
  else if (ch == Si16P::fSiL13S) for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiL13S(tpis[i]));
  else if (ch == Si16P::fSiL14S) for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiL14S(tpis[i]));
  else if (ch == Si16P::fSiL2S)  for (unsigned int i = 0; i < tpis.size(); ++i) t.push_back(fCFTime_SiL2S (tpis[i]));
  else throw std::logic_error("Si16P: Invalid channel to calculate times for.");
  return t;
}

std::vector<double> Si16P::CalculateEnergies(const IDs::channel& ch, const std::vector<TPulseIsland*>& tpis) {
  std::vector<double> e;
  if      (ch == Si16P::fMuSc)   for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(                    fMBAmp_MuSc  (tpis[i]));
  else if (ch == Si16P::fSiR11S) for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiR11S->Eval(fMBAmp_SiR11S(tpis[i])));
  else if (ch == Si16P::fSiR12S) for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiR12S->Eval(fMBAmp_SiR12S(tpis[i])));
  else if (ch == Si16P::fSiR13S) for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiR13S->Eval(fMBAmp_SiR13S(tpis[i])));
  else if (ch == Si16P::fSiR14S) for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiR14S->Eval(fMBAmp_SiR14S(tpis[i])));
  else if (ch == Si16P::fSiR2S)  for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiR2S ->Eval(fMBAmp_SiR2S (tpis[i])));
  else if (ch == Si16P::fSiL11S) for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiL11S->Eval(fMBAmp_SiL11S(tpis[i])));
  else if (ch == Si16P::fSiL12S) for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiL12S->Eval(fMBAmp_SiL12S(tpis[i])));
  else if (ch == Si16P::fSiL13S) for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiL13S->Eval(fMBAmp_SiL13S(tpis[i])));
  else if (ch == Si16P::fSiL14S) for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiL14S->Eval(fMBAmp_SiL14S(tpis[i])));
  else if (ch == Si16P::fSiL2S)  for (unsigned int i = 0; i < tpis.size(); ++i) e.push_back(fADC2E_SiL2S ->Eval(fMBAmp_SiL2S (tpis[i])));
  else throw std::logic_error("Si16P: Invalid channel to calculate energies for.");
  return e;
}

void Si16P::RemoveMuScPileUp(std::vector<double>& t, std::vector<double>& e) {
  if (t.empty())  return;

  std::vector<bool> rm(t.size(), false);
  if (t.front() < fdTMuScPP) rm.front() = true;
  for (unsigned int i = 1; i < t.size(); ++i)
    if (t[i] - t[i-1] < fdTMuScPP)
      rm[i] = rm[i-1] = true;
  rm.back() = true;
  for (unsigned int i = 0; i < t.size(); ++i) {
    if (rm[i]) {
      t .erase(t .begin()+i);
      e .erase(e .begin()+i);
      rm.erase(rm.begin()+i);
      --i;
    }
  }
}

void Si16P::RemoveElectrons(std::vector<double>& t, std::vector<double>& e) {
  for (unsigned int i = 0; i < t.size(); ++i) {
    if (e[i] < fMuonEnergyCut) {
      t.erase(t.begin() + i);
      e.erase(e.begin() + i);
      --i;
    }
  }
}

void Si16P::RemoveScatteredMuons(std::vector<double>& muT, std::vector<double>& muE,
				 const std::vector<double>& siT) {
  if (siT.empty()) return;
  const std::vector<double>::const_iterator b = siT.begin(), e = siT.end();
  for (unsigned int i = 0; i < muT.size(); ++i) {
    std::vector<double>::const_iterator cSi = std::upper_bound(b, e, muT[i]);
    if ( (  cSi <  e && *cSi - muT[i] < fdTScatter) ||
	 (--cSi >= b && muT[i] - *cSi < fdTScatter) ) {
      muT.erase(muT.begin()+i);
      muE.erase(muE.begin()+i);
      --i;
    }
  }
}

void Si16P::RemoveScatteredMuons(std::vector<double>& muT, std::vector<double>& muE,
				 const std::vector< std::vector<double> >& siT) {
  for (unsigned int i = 0; i < siT.size(); ++i)
    RemoveScatteredMuons(muT, muE, siT[i]);
}

void Si16P::RemoveMultipleDetectorHits(std::vector<double>& muT, std::vector<double>& muE,
				       const std::vector<double>& siT) {
  const std::vector<double>::const_iterator b = siT.begin(), e = siT.end();
  for (unsigned int i = 0; i < muT.size(); ++i) {
    const unsigned int n = std::upper_bound(b, e, muT[i]+fdTSiHigh+fdTPID) - std::upper_bound(b, e, muT[i]+fdTSiLow-fdTPID);
    if (n > 1) {
      muT.erase(muT.begin()+i);
      muE.erase(muE.begin()+i);
      --i;
    }
  }
}

void Si16P::RemoveMultipleDetectorHits(std::vector<double>& muT, std::vector<double>& muE,
				       const std::vector< std::vector<double> >& siT) {
  std::vector<double> all;
  for (unsigned int i = 0; i < siT.size(); ++i)
    all.insert(all.end(), siT[i].begin(), siT[i].end());
  std::sort(all.begin(), all.end());
  RemoveMultipleDetectorHits(muT, muE, all);
}

double Si16P::FindFirstMatchingThinHit(const double t,
				       const std::vector< std::vector<double> >& vvt,
				       const std::vector< std::vector<double> >& vve) {
  for (unsigned int i = 0; i < vvt.size(); ++i) {
    std::vector<double>::const_iterator ct = std::upper_bound(vvt[i].begin(), vvt[i].end(), t);
    if ( ct < vvt[i].end() && std::abs(t-*ct) <= fdTPID)
      return vve[i][ct-vvt[i].begin()];
  }
  return -1.;
}


void Si16P::ThrowIfInputsInsane(modules::options* opts) {
}

ALCAP_REGISTER_MODULE(Si16P,tcut_musc,tcut_si_low,tcut_si_high,tcut_pid,tcut_scatter,tcut_detpp,adccut_musc);
