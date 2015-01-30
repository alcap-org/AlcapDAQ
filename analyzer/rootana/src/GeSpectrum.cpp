#include "GeSpectrum.h"
#include "RegisterModule.inc"
#include "TPulseIsland.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
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

const IDs::channel GeSpectrum::fGeS(IDs::kGe, IDs::kSlow);
const IDs::channel GeSpectrum::fGeF(IDs::kGe, IDs::kFast);
const IDs::channel GeSpectrum::fMuSc(IDs::kMuSc, IDs::kNotApplicable);

GeSpectrum::GeSpectrum(modules::options* opts) :
  BaseModule("GeSpectrum",opts),
  fhADC(NULL), fhEnergy(NULL),
  fhADCOOT(NULL), fhEnergyOOT(NULL), fhADCFarOOT(NULL), fhEnergyFarOOT(NULL),
  fhTimeADC(NULL), fhTimeEnergy(NULL), fhNMuons(NULL),
  fhPPTimeADC(NULL), fhPPTimeEnergy(NULL), fhPPNMuons(NULL),
  fvhTimePeak(), fvhPPTimePeak(),
  fUseSlowTiming(opts->GetBool("ge_slow_timing")), fCalibration(opts->GetBool("calib")),
  fMBAmpMuSc(SetupNavigator::Instance()->GetPedestal(fMuSc), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fMuSc.str()))),
  fMBAmpGe(SetupNavigator::Instance()->GetPedestal(fGeS), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fGeS.str()))),
  fCFTimeMuSc(SetupNavigator::Instance()->GetPedestal(fMuSc),
	      TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fMuSc.str())),
	      TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fMuSc.str())),
	      0.,
	      fCalibration ? 0. : opts->GetDouble("musc_cf")),
  fCFTimeGe(SetupNavigator::Instance()->GetPedestal(fUseSlowTiming ? fGeS : fGeF),
	    TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fUseSlowTiming ? fGeS.str() : fGeF.str())),
	    TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fUseSlowTiming ? fGeS.str() : fGeF.str())),
	    fCalibration ? 0. : SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(fUseSlowTiming ? fGeS : fGeF, IDs::generator(opts->GetString("ge_gen"), opts->GetString("ge_cfg")))),
	    opts->GetDouble("ge_cf")),
  fADC2Energy(new TF1("adc2energy","[0]*x+[1]")),


  fTimeWindow_Small(opts->GetDouble("tw_small")), fTimeWindow_Big(opts->GetDouble("tw_big")), fPileupProtectionWindow(opts->GetDouble("tw_pp")),
  fMuScPulseHeightCut(opts->GetInt("musc_min_height")), fvPeaksOfInterest() {
  ThrowIfInputsInsane(opts);

  const static int nbins = std::pow(2.,14);
  const std::pair<double,double> adc2energy_par = fCalibration ? std::pair<double, double>(0., 0.) : SetupNavigator::Instance()->GetEnergyCalibrationConstants(IDs::channel("Ge-S"));
  fADC2Energy->SetParameters(adc2energy_par.first, adc2energy_par.second);
  TDirectory* cwd = TDirectory::CurrentDirectory();
  dir->cd();

  fhADC            = new TH1D("hADC",          "Energy of Gammas;Energy (ADC)",                        nbins, 0.,                    nbins);
  if (!fCalibration) {
    fhEnergy       = new TH1D("hEnergy",       "Energy of Gammas;Energy (keV)",                        nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
    fhADCOOT       = new TH1D("hADCOOT",       "Energy of Gammas outside of Time Window;Energy (ADC)", nbins, 0.,                    nbins);
    fhEnergyOOT    = new TH1D("hEnergyOOT",    "Energy of Gammas outside of Time Window;Energy (keV)", nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
    fhADCFarOOT    = new TH1D("hADCFarOOT",    "Energy of Gammas far from Muons;Energy (ADC)",         nbins, 0.,                    nbins);
    fhEnergyFarOOT = new TH1D("hEnergyFarOOT", "Energy of Gammas far from Muons;Energy (keV)",         nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
    fhTimeADC      = new TH2D("hTimeADC",      "Energy of Gammas within Time Window;Time (ns);Energy (ADC)",      500, -fTimeWindow_Small, fTimeWindow_Small, nbins, 0.,                    nbins);
    fhTimeEnergy   = new TH2D("hTimeEnergy",   "Energy of Gammas within Time Window;Time (ns);Energy (keV)",      500, -fTimeWindow_Small, fTimeWindow_Small, nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
    fhPPTimeADC    = new TH2D("hPPTimeADC",    "Energy of Gammas within Time Window (PP);Time (ns);Energy (ADC)", 500, -fTimeWindow_Small, fTimeWindow_Small, nbins, 0.,                    nbins);
    fhPPTimeEnergy = new TH2D("hPPTimeEnergy", "Energy of Gammas within Time Window (PP);Time (ns);Energy (keV)", 500, -fTimeWindow_Small, fTimeWindow_Small, nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
    opts->GetVectorDoublesByWhiteSpace("peaks_of_interest", fvPeaksOfInterest);
    for (unsigned int i = 0; i < fvPeaksOfInterest.size(); ++i) {
      char name[64], title[64];
      const double bounds[2] = { fvPeaksOfInterest[i] - 2., fvPeaksOfInterest[i] + 2. };
      sprintf(name,  "hTimePeak%ld", ::lround(fvPeaksOfInterest[i]));
      sprintf(title, "Timing around %ld keV Peak", ::lround(fvPeaksOfInterest[i]));
      fvhTimePeak.push_back(new TH2D(name, title, 2000, -5000., 5000., 32, bounds[0], bounds[1]));
    }
  }
  fhNMuons         = new TH1D("hNMuons",   "Number of muons in MIDAS event;Number",      2000, 0., 2000.);
  fhPPNMuons       = new TH1D("hPPNMuons", "Number of muons in MIDAS event;Number (PP)", 2000, 0., 2000.);
  cwd->cd();
}

GeSpectrum::~GeSpectrum(){
}

int GeSpectrum::BeforeFirstEntry(TGlobalData* gData, const TSetupData *setup){
  return 0;
}

int GeSpectrum::ProcessEntry(TGlobalData* gData, const TSetupData *setup){

  static const std::string bank_musc = TSetupData::Instance()->GetBankName(fMuSc.str());
  static const std::string bank_ges  = TSetupData::Instance()->GetBankName(fGeS.str());
  static const std::string bank_gef  = TSetupData::Instance()->GetBankName(fGeF.str());

  const std::map< std::string, std::vector<TPulseIsland*> >& TPIMap = gData->fPulseIslandToChannelMap;
  if (!fUseSlowTiming)
    ThrowIfGeInsane(TPIMap.at(bank_ges), TPIMap.at(bank_gef));
  if (!(TPIMap.count(bank_musc) && TPIMap.count(bank_ges) && (fUseSlowTiming || TPIMap.count(bank_gef))))
    return 0;
  else if (TPIMap.at(bank_musc).size() == 0 || TPIMap.at(bank_ges).size() == 0 || (TPIMap.at(bank_gef).size() == 0 && !fUseSlowTiming))
    return 0;

  std::vector<double> muScTimes    = CalculateTimes(fMuSc, TPIMap.at(bank_musc));
  std::vector<double> muScEnergies = CalculateEnergies(fMuSc, TPIMap.at(bank_musc));
  std::vector<double> geTimes      = CalculateTimes(fUseSlowTiming ? fGeS : fGeF, TPIMap.at(fUseSlowTiming ? bank_ges : bank_gef));
  std::vector<double> geEnergies   = CalculateEnergies(fGeS, TPIMap.at(bank_ges));
  
  // Apply some cuts
  RemoveSmallMuScPulses (muScTimes, muScEnergies);
  //  std::vector<double> muScTimesPP(muScTimes), muScEnergiesPP(muScEnergies);
  RemovePileupMuScPulses(muScTimes, muScEnergies);

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
    if ( (!prev_found || dt_prev > fTimeWindow_Big) && (!next_found || dt_next < -fTimeWindow_Big) ) {
      fhADCFarOOT->Fill(adc);
      fhEnergyFarOOT->Fill(en);
    } else if ( (!prev_found || dt_prev > fTimeWindow_Small) && (!next_found || dt_next < -fTimeWindow_Small) ) {
      fhADCOOT->Fill(adc);
      fhEnergyOOT->Fill(en);
    } else {
      if (prev_found && dt_prev <= fTimeWindow_Small) {
	fhTimeADC->Fill(dt_prev, adc);
	fhTimeEnergy->Fill(dt_prev, en);
      } else if (next_found && dt_next >= -fTimeWindow_Small) {
	fhTimeADC->Fill(dt_next, adc);
	fhTimeEnergy->Fill(dt_next, en);
      } else {
	std::cout << "WARNING: Unexpected logical branch! Prev: (" << prev_found << ", " << dt_prev << "), Next: (" << next_found << ", " << dt_next << ")" << std::endl;
      }
    }
    fhADC->Fill(adc);
    fhEnergy->Fill(en);
    for (unsigned int i = 0; i < fvPeaksOfInterest.size(); ++i)
      if (en < fvPeaksOfInterest[i] + 2. && en > fvPeaksOfInterest[i] - 2) {
	if (prev_found)
	  fvhTimePeak[i]->Fill(dt_prev,en);
	if (next_found)
	  fvhTimePeak[i]->Fill(dt_next,en);
      }
  }

  //*************************************//
  //************ Count Muons ************//
  //*************************************//
  fhNMuons  ->Fill(muScEnergies  .size());
  //fhPPNMuons->Fill(muScEnergiesPP.size());

  return 0;
}

int GeSpectrum::AfterLastEntry(TGlobalData* gData, const TSetupData *setup){
  return 0;
}


std::vector<double> GeSpectrum::CalculateTimes(const IDs::channel& ch, const std::vector<TPulseIsland*>& tpis) {
  std::vector<double> t;
  if (ch == fMuSc)
    for (unsigned int i = 0; i < tpis.size(); ++i)
      t.push_back(fCFTimeMuSc(tpis[i]));
  else if (ch == (fUseSlowTiming ? fGeS : fGeF))
    for (unsigned int i = 0; i < tpis.size(); ++i)
      t.push_back(fCFTimeGe(tpis[i]));
  else
    throw std::logic_error("GeSpectrum: Invalid channel to calculate times for.");
  return t;
}


std::vector<double> GeSpectrum::CalculateEnergies(const IDs::channel& ch, const std::vector<TPulseIsland*>& tpis) {
  std::vector<double> e;
  if (ch == fGeS)
    for (unsigned int i = 0; i < tpis.size(); ++i)
      e.push_back(fMBAmpGe(tpis[i]));
  else if (ch == fMuSc)
    for (unsigned int i = 0; i < tpis.size(); ++i)
      e.push_back(fMBAmpMuSc(tpis[i]));
  else
    throw std::logic_error("GeSpectrum: Invalid channel to calculate energies for.");
  return e;
}

void GeSpectrum::RemoveSmallMuScPulses(std::vector<double>& t, std::vector<double>& e) {
  for (unsigned int i = 0; i < t.size(); ++i) {
    if (e[i] < fMuScPulseHeightCut) {
      t.erase(t.begin() + i);
      e.erase(e.begin() + i);
      --i;
    }
  }
}

void GeSpectrum::RemovePileupMuScPulses(std::vector<double>& t, std::vector<double>& e) {
  if (t.empty()) return;
  
  std::vector<bool> rm(t.size(), false);
  if (t.front() < fPileupProtectionWindow) rm.front() = true;
  for (unsigned int i = 1; i < t.size(); ++i)
    if (t[i] - t[i-1] < fPileupProtectionWindow)
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

void GeSpectrum::ThrowIfGeInsane(const std::vector<TPulseIsland*>& ge1s, const std::vector<TPulseIsland*>& ge2s) {
  if (ge1s.size() != ge2s.size())
    throw std::logic_error("Fast and slow vectors are not same size!");
  for (std::vector<TPulseIsland*>::const_iterator ge1 = ge1s.begin(), ge2 = ge2s.begin(); ge1 != ge1s.end(); ++ge1, ++ge2)
    if ((*ge1)->GetTimeStamp() != (*ge2)->GetTimeStamp())
      throw std::logic_error("Fast and slow timestamps don't match up when getting time");
}

void GeSpectrum::ThrowIfInputsInsane(const modules::options* opts) {
  const std::string ge_cfg = opts->GetString("ge_cfg");
  const double      ge_cf  = opts->GetDouble("ge_cf");
  const double      musc_cf = opts->GetDouble("musc_cf");
  const std::string cf_str("constant_fraction=");
  std::stringstream ss(ge_cfg.substr(ge_cfg.find(cf_str) + cf_str.size()));
  double cf;
  ss >> cf;
  if (cf != ge_cf)
    throw std::logic_error("GeSpectrum: Ge CF in options don't match.");
  if (ge_cf <= 0. || ge_cf > 1. || musc_cf <= 0. || musc_cf > 1.)
    throw std::logic_error("GeSpectrum: CF are out of reasonable bounds (0. to 1.).");
}

ALCAP_REGISTER_MODULE(GeSpectrum,musc_cf,ge_gen,ge_cfg,ge_cf,ge_slow_timing,tw_small,tw_big,calib,pp_window,musc_min_height,peaks_of_interest);
