#include "Stats.h"
#include "RegisterModule.inc"
#include "TPulseIsland.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "SetupNavigator.h"
#include "PulseCandidateFinder.h"

#include "TH1I.h"
#include "TNtupleD.h"
#include "TDirectory.h"

#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <climits>
#include <string>
#include <sstream>

const IDs::channel Stats::fMuSc(IDs::kMuSc, IDs::kNotApplicable);

Stats::Stats(modules::options* opts) : BaseModule("Stats",opts),
ftRunStats  (new TNtupleD("RunStats",   "Run Statistics",   "tpp:muscADCcut")),
ftBlockStats(new TNtupleD("BlockStats", "Block Statistics", "lt:nmu:ppnmu")),
fMBAmpMuSc(SetupNavigator::Instance()->GetPedestal(fMuSc),
  TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fMuSc.str()))),
fCFTimeMuSc(SetupNavigator::Instance()->GetPedestal(fMuSc),
  TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fMuSc.str())),
  TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fMuSc.str())),
  0.,
  opts->GetDouble("musc_cf")),
fTPP(opts->GetDouble("pp_window")),
fMuScHeightCut(opts->GetDouble("musc_height_cut")) {

  ThrowIfInputsInsane(opts);
}

Stats::~Stats(){
}

int Stats::BeforeFirstEntry(TGlobalData* gData, const TSetupData *setup){
  ftRunStats->Fill(fTPP, fMuScHeightCut);
  return 0;
}

int Stats::ProcessEntry(TGlobalData* gData, const TSetupData *setup){

  static const std::string bank_musc = TSetupData::Instance()->GetBankName(fMuSc.str());
  const std::vector<TPulseIsland*>& muScHits = gData->fPulseIslandToChannelMap.at(bank_musc);

  if (muScHits.empty()) {
    ftBlockStats->Fill(0., 0., 0.);
    return 0;
  }

  std::vector<double> muTs = CalculateTimes(muScHits);
  std::vector<double> muEs = CalculateEnergies(muScHits);
  RemoveElectrons(muTs, muEs);

  const long double tmu    = muTs.back();
  const long double nmu    = muTs.size();
  RemovePileupMuScPulses(muTs, muEs);
  const long double nmu_pp = muTs.size();

  ftBlockStats->Fill(tmu, nmu, nmu_pp);


  return 0;
}

int Stats::AfterLastEntry(TGlobalData* gData, const TSetupData *setup){
  return 0;
}


std::vector<double> Stats::CalculateTimes(const std::vector<TPulseIsland*>& tpis) {
  std::vector<double> t;
  for (unsigned int i = 0; i < tpis.size(); ++i)
    t.push_back(fCFTimeMuSc(tpis[i]));
  return t;
}


std::vector<double> Stats::CalculateEnergies(const std::vector<TPulseIsland*>& tpis) {
  std::vector<double> e;
  for (unsigned int i = 0; i < tpis.size(); ++i)
    e.push_back(fMBAmpMuSc(tpis[i]));
  return e;
}


void Stats::ThrowIfInputsInsane(const modules::options* opts) {
}

void Stats::RemovePileupMuScPulses(std::vector<double>& t, std::vector<double>& e) {
  if (t.empty()) return;
  std::vector<bool> rm(t.size(), false);
  if (t.front() < fTPP) rm.front() = true;
  for (unsigned int i = 1; i < t.size(); ++i)
    if (t[i] - t[i-1] < fTPP)
      rm[i] = rm[i-1] = true;
  rm.back() = true;
  for (unsigned int i = 0; i < t.size(); ++i) {
    if (rm[i]) {
      t.erase(t.begin()+i);
      e.erase(e.begin()+i);
      rm.erase(rm.begin()+i);
      --i;
    }
  }
}

void Stats::RemoveElectrons(std::vector<double>& t, std::vector<double>& e) {
  for (unsigned int i = 0; i < t.size(); ++i) {
    if (e[i] < fMuScHeightCut) {
      t.erase(t.begin()+i);
      e.erase(e.begin()+i);
      --i;
    }
  }
}
ALCAP_REGISTER_MODULE(Stats, musc_cf, musc_height_cut, pp_window);
