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
  fhADC(new TH1D("hADC", "Energy of ScGe;Energy (ADC)", std::pow(2, 12), 0., std::pow(2, 12))),
  fTCoinc(opts->GetDouble("tcoinc")),
  fCFTimeGe(SetupNavigator::Instance()->GetPedestal(fGeF),
    TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fGeF.str())),
    TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fGeF.str())),
    SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(fGeF, IDs::generator(opts->GetString("ge_gen"), opts->GetString("ge_cfg")))),
    opts->GetDouble("ge_cf")),
  fCFTimeScGe(SetupNavigator::Instance()->GetPedestal(fScGe),
    TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fScGe.str())),
    TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fScGe.str())),
    SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(fScGe, IDs::generator(opts->GetString("scge_gen"), opts->GetString("scge_cfg")))),
    opts->GetDouble("scge_cf"))
  {

  SaveInputsAndThrowIfInputsInsane(opts);

}

ScGeConditionalHeight::~ScGeConditionalHeight(){
}

int ScGeConditionalHeight::BeforeFirstEntry(TGlobalData* gData, const TSetupData *setup){
  return 0;
}

int ScGeConditionalHeight::ProcessEntry(TGlobalData* gData, const TSetupData *setup){

  const unsigned int NBITS      = 14;             // UH CAEN, GeS
  const unsigned int MAX_HEIGHT = (1<<NBITS) - 1; // GeS max height
  
  static const std::string bank_ges  = TSetupData::Instance()->GetBankName(fGeS .str());
  static const std::string bank_gef  = TSetupData::Instance()->GetBankName(fGeF .str());
  static const std::string bank_scge = TSetupData::Instance()->GetBankName(fScGe.str());

  const std::map< std::string, std::vector<TPulseIsland*> >& TPIMap = gData->fPulseIslandToChannelMap;
  ThrowIfGeInsane(TPIMap.at(bank_ges), TPIMap.at(bank_gef));
  if (TPIMap.at(bank_scge).size() == 0 || TPIMap.at(bank_ges).size() == 0 || TPIMap.at(bank_gef).size() == 0 )
    return 0;

  std::vector<double>       geTimes    = CalculateTimes  (fGeF,  TPIMap.at(bank_gef));
  std::vector<unsigned int> geHeights  = CalculateHeights(TPIMap.at(bank_ges));
  std::vector<double>       scGeTimes  = CalculateTimes  (fScGe, TPIMap.at(bank_scge));
  std::vector<unsigned int> scGeHeights = CalculateHeights(TPIMap.at(bank_scge));
  
  for (unsigned int iGe = 0; iGe < geHeights.size(); ++iGe) {
    if (geHeights[iGe] == MAX_HEIGHT) {
      std::vector<double>::const_iterator cScGeT = std::upper_bound(scGeTimes.begin(), scGeTimes.end(), geTimes[iGe] - fTCoinc);
      if (*cScGeT < geTimes[iGe] - fTCoinc)
	fhADC->Fill(geHeights[iGe]);
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

std::vector<unsigned int> ScGeConditionalHeight::CalculateHeights(const std::vector<TPulseIsland*>& tpis) {
  std::vector<unsigned int> h;
  for(unsigned int i = 0; i < tpis.size(); ++i) {
    const std::vector<int>& samps = tpis[i]->GetSamples();
    h.push_back(*std::max_element(samps.begin(), samps.end()));
  }
  return h;
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
  std::string ge_gen   = opts->GetString("ge_gen");
  std::string ge_cfg   = opts->GetString("ge_cfg");
  double      ge_cf    = opts->GetDouble("ge_cf");
  std::string scge_gen = opts->GetString("scge_gen");
  std::string scge_cfg = opts->GetString("scge_cfg");
  double      scge_cf  = opts->GetDouble("scge_cf");
  double      tcoinc   = opts->GetDouble("tcoinc");
  
  // Save all of the module options
  TTree* optTree = new TTree("opts", "ModuleOptions");
  optTree->Branch("ge_gen",   &ge_gen);
  optTree->Branch("ge_cfg",   &ge_cfg);
  optTree->Branch("ge_cf",    &ge_cf);
  optTree->Branch("scge_gen", &scge_gen);
  optTree->Branch("scge_cfg", &scge_cfg);
  optTree->Branch("scge_cf",  &scge_cf);
  optTree->Branch("tcoinc",   &tcoinc);

  optTree->Fill();
}

ALCAP_REGISTER_MODULE(ScGeConditionalHeight,ge_gen,ge_cfg,ge_cf,scge_gen,scge_cfg,scge_cf,tcoinc);
