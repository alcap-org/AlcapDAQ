#include "ScGeConditionalHeight.h"
#include "RegisterModule.inc"
#include "TPulseIsland.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "TAPAlgorithms.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "SetupNavigator.h"
#include "ExportPulse.h"
#include "PulseCandidateFinder.h"

#include "TTree.h"
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

const IDs::channel ScGeConditionalHeight::fGeS (IDs::kGe,   IDs::kSlow);
const IDs::channel ScGeConditionalHeight::fGeF (IDs::kGe,   IDs::kFast);
const IDs::channel ScGeConditionalHeight::fScGe(IDs::kScGe, IDs::kNotApplicable);

ScGeConditionalHeight::ScGeConditionalHeight(modules::options* opts) :
  BaseModule("ScGeConditionalHeight", opts),
  fhADC(new TH1D("hADC", "Energy of ScGe;Energy (ADC)", std::pow(2, 14), 0., std::pow(2, 14))),
  fCFTimeGe(SetupNavigator::Instance()->GetPedestal(fGeF),
    TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fGeF.str())),
	  TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fGeF.str())),
	  SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(fGeF, IDs::generator(opts->GetString("ge_gen"), opts->GetString("ge_cfg")))),
	  opts->GetDouble("ge_cf")),
  fCFTimeScGe(SetupNavigator::Instance()->GetPedestal(fScGe),
    TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fScGe.str())),
    TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fScGe.str())),
    SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(fScGe, IDs::generator(opts->GetString("scge_gen"), opts->GetString("scge_cfg")))),
    opts->GetDouble("scge_cf")),
  {

  SaveInputsAndThrowIfInputsInsane(opts);
  const static int nbins = std::pow(2.,14);

}

ScGeConditionalHeight::~ScGeConditionalHeight(){
}

int ScGeConditionalHeight::BeforeFirstEntry(TGlobalData* gData, const TSetupData *setup){
  return 0;
}

int ScGeConditionalHeight::ProcessEntry(TGlobalData* gData, const TSetupData *setup){

  static const std::string bank_musc = TSetupData::Instance()->GetBankName(fMuSc.str());
  static const std::string bank_ges  = TSetupData::Instance()->GetBankName(fGeS .str());
  static const std::string bank_gef  = TSetupData::Instance()->GetBankName(fGeF .str());
  static const std::string bank_scge = TSetupData::Instance()->GetBankName(fScGe.str());

  const std::map< std::string, std::vector<TPulseIsland*> >& TPIMap = gData->fPulseIslandToChannelMap;
  if (!fUseSlowTiming)
    ThrowIfGeInsane(TPIMap.at(bank_ges), TPIMap.at(bank_gef));
  if (!(TPIMap.count(bank_musc) && TPIMap.count(bank_ges) && (fUseSlowTiming || TPIMap.count(bank_gef))))
    return 0;
  else if (TPIMap.at(bank_musc).size() == 0 || TPIMap.at(bank_ges).size() == 0 || (TPIMap.at(bank_gef).size() == 0 && !fUseSlowTiming))
    return 0;

  std::vector<double> muScTimes    = CalculateTimes   (fMuSc,                        TPIMap.at(bank_musc));
  std::vector<double> muScEnergies = CalculateEnergies(fMuSc,                        TPIMap.at(bank_musc));
  std::vector<double> geTimes      = CalculateTimes   (fUseSlowTiming ? fGeS : fGeF, TPIMap.at(fUseSlowTiming ? bank_ges : bank_gef));
  std::vector<double> geEnergies   = CalculateEnergies(fGeS,                         TPIMap.at(bank_ges));
  std::vector<double> scGeTimes    = CalculateTimes   (fScGe,                        TPIMap.at(bank_scge));

  // Apply some cuts
  RemoveSmallMuScPulses (muScTimes, muScEnergies);
  if (fPileupProtected)
    RemovePileupMuScPulses(muScTimes, muScEnergies);
  if (fChargedParticleProtected)
    RemoveChargedParticles(geTimes, geEnergies, scGeTimes);

  //**************************//
  //***** Now make plots *****//
  //**************************//
  for (std::vector<double>::const_iterator geT = geTimes.begin(), geE = geEnergies.begin(), prev = muScTimes.begin(), next;
   geT != geTimes.end() && geE != geEnergies.end();
   ++geT, ++geE) {
    // Short-ciruit each iteration if only interested in calibrating
    if (fCalibration) {
      fhADC->Fill(*geE);
      continue;
    }

    bool prev_found = false, next_found = false;
    double dt_prev = 1e9, dt_next = 1e9;

    /////////////////////////////////////////////////////
    // Find the time difference with the most recent muon
    // both before and after.
    /////////////////////////////////////////////////////
    next = std::upper_bound(prev, (std::vector<double>::const_iterator)muScTimes.end(), *geT);
    prev = next - 1;
    if (next == muScTimes.end()) {
      dt_prev = *geT - *prev;
      prev_found = true;
    } else if (next == muScTimes.begin()) {
      dt_next = *geT - *next;
      next_found = true;
    } else {
      // The vast majority of cases are here
      dt_prev = *geT - *prev;
      dt_next = *geT - *next;
      prev_found = next_found = true;
    }

    ////////////////////////////////////////
    // Plot energy in different time windows
    ////////////////////////////////////////
    const Double_t adc = *geE;
    const Double_t en = fADC2Energy->Eval(adc);
    if ( prev_found && dt_prev <= fTimeWindow_Small ) {
      fhTimeADC   ->Fill(dt_prev, adc);
      fhTimeEnergy->Fill(dt_prev, en);
    } else if ( next_found && dt_next >= -fTimeWindow_Small ) {
      fhTimeADC   ->Fill(dt_next, adc);
      fhTimeEnergy->Fill(dt_next, en);
    } else if ( prev_found && dt_prev <= fTimeWindow_Big ) {
      fhADCOOT   ->Fill(adc);
      fhEnergyOOT->Fill(en);
    } else {
      fhADCFarOOT   ->Fill(adc);
      fhEnergyFarOOT->Fill(en);
    }
    if ((prev_found && dt_prev > fTimeWindow_Asymmetric[0] && dt_prev < fTimeWindow_Asymmetric[1]) ||
        (next_found && dt_next > fTimeWindow_Asymmetric[0] && dt_next < fTimeWindow_Asymmetric[1])) {
      fhADCTWindow   ->Fill(adc);
      fhEnergyTWindow->Fill(en);
    }
    fhADC   ->Fill(adc);
    fhEnergy->Fill(en);
    for (unsigned int i = 0; i < fvPeaksOfInterest.size(); ++i)
      if (en < fvPeaksOfInterest[i] + 2. && en > fvPeaksOfInterest[i] - 2) {
       if (prev_found)
         fvhTimePeak[i]->Fill(dt_prev,en);
       if (next_found)
         fvhTimePeak[i]->Fill(dt_next,en);
     }
   }

   return 0;
 }

int ScGeConditionalHeight::AfterLastEntry(TGlobalData* gData, const TSetupData *setup){
  return 0;
}

std::vector<double> ScGeConditionalHeight::CalculateTimes(const IDs::channel& ch, const std::vector<TPulseIsland*>& tpis) {
  std::vector<double> t;
  Algorithm::ConstantFractionTime cf;
  if      (ch == fGeF)  cf = fCFTimeGe;
  else if (ch == fScGe) cf = fCFTimeScGe;
  else throw std::logic_error("ScGeConditionalHeight: Invalid channel to calculate times for.");
  for (unsigned int i = 0; i < tpis.size(); ++i)
    t.push_back(cf(tpis[i]));
  return t;
}

void ScGeConditionalHeight::ThrowIfGeInsane(const std::vector<TPulseIsland*>& ge1s, const std::vector<TPulseIsland*>& ge2s) {
  if (ge1s.size() != ge2s.size())
    throw std::logic_error("ScGeConditionalHeight: Fast and slow vectors are not same size!");
  for (std::vector<TPulseIsland*>::const_iterator ge1 = ge1s.begin(), ge2 = ge2s.begin(); ge1 != ge1s.end(); ++ge1, ++ge2)
    if ((*ge1)->GetTimeStamp() != (*ge2)->GetTimeStamp())
      throw std::logic_error("ScGeConditionalHeight: Fast and slow timestamps don't match up when getting time");
}

void ScGeConditionalHeight::SaveInputsAndThrowIfInputsInsane(const modules::options* opts) {
  // Load all of the
  std::string  ge_cfg          = opts->GetString("ge_cfg");
  double       ge_cf           = opts->GetDouble("ge_cf");
  std::string  scge_cfg        = opts->GetString("scge_cfg");
  double       scge_cf         = opts->GetDouble("scge_cf");

  // Save all of the module options
  TTree* optTree = new TTree("opts", "ModuleOptions");
  optTree->Branch("musc_cf", &musc_cf);
  optTree->Branch("ge_cfg", &ge_cfg);
  optTree->Branch("ge_cf", &ge_cf);
  optTree->Branch("scge_cfg", &scge_cfg);
  optTree->Branch("scge_cf", &scge_cf);
  optTree->Branch("ge_slow_timing", &ge_slow_timing);
  optTree->Branch("tw_small", &tw_small);
  optTree->Branch("tw_big", &tw_big);
  optTree->Branch("tw_asymmetric", &tw_asymmetric);
  optTree->Branch("calib", &calib);
  optTree->Branch("pu_protect", &pu_protect);
  optTree->Branch("pu_window", &pu_window);
  optTree->Branch("cp_protect", &cp_protect);
  optTree->Branch("cp_window", &cp_window);
  optTree->Branch("musc_min_height", &musc_min_height);
  optTree->Branch("peaks_of_interest", &peaks_of_interest);
  opts->GetVectorDoublesByWhiteSpace("peaks_of_interest", peaks_of_interest);

  optTree->Fill();
  // Sanity check arguments
 // std::stringstream ss(ge_cfg.substr(ge_cfg.find(cf_str) + cf_str.size()));
//  double cf;
//  ss >> cf;
//  if (cf != ge_cf)
//    throw std::logic_error("ScGeConditionalHeight: Ge CF in options don't match.");
//  if (ge_cf <= 0. || ge_cf > 1. || musc_cf <= 0. || musc_cf > 1.)
//    throw std::logic_error("ScGeConditionalHeight: CF are out of reasonable bounds (0. to 1.).");
}

ALCAP_REGISTER_MODULE(ScGeConditionalHeight,musc_cf,ge_gen,ge_cfg,ge_cf,scge_gen,scge_cfg,scge_cf,ge_slow_timing,tw_small,tw_big,tw_asymmetric,calib,pu_protect,pu_window,cp_protect,cp_window,musc_min_height,peaks_of_interest);
