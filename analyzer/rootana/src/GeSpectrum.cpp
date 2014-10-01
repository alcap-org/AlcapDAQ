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

static double E(const int adc);

GeSpectrum::GeSpectrum(modules::options* opts) :
  BaseModule("GeSpectrum",opts),
  fHist_Energy(NULL), fHist_Time(NULL), fHist_MoreTime(NULL),
  fHist_EnergyOOT(NULL), fHist_EnergyFarOOT(NULL), fHist_TimeEnergy(NULL), fHist_MeanTOffset(NULL),
  fMBAmpGe(SetupNavigator::Instance()->GetPedestal(fGeS), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fGeS.str()))),
  fCFTimeGe(SetupNavigator::Instance()->GetPedestal(fGeF),
	    TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fGeF.str())),
	    TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fGeF.str())),
	    SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(fGeF, IDs::generator(opts->GetString("gef_gen"), opts->GetString("gef_cfg")))),
	    opts->GetDouble("gef_cf")),
  fCFTimeMuSc(SetupNavigator::Instance()->GetPedestal(fMuSc),
	      TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fMuSc.str())),
	      TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fMuSc.str())),
	      0.,
	      opts->GetDouble("musc_cf")) {
  const static int nbins = std::pow(2.,14);
  const static double max = E(nbins);
  TDirectory* cwd = TDirectory::CurrentDirectory();
  dir->cd();
  fHist_Energy       = new TH1I("hEnergy", "Energy of Gammas", nbins, 0., max);
  fHist_Time         = new TH1I("hTime", "Time of Gammas within Energy Window", 1000, -10000., 10000.);
  fHist_MoreTime     = new TH1I("hMoreTime", "Time of Gammas within Energy Window (Wide)", 1000, -100000., 100000.);
  fHist_EnergyOOT    = new TH1I("hEnergyOOT", "Energy of Gammas outside of Time Window", nbins, 0., max);
  fHist_EnergyFarOOT = new TH1I("hEnergyFarOOT", "Energy of Gammas far from Muons", nbins, 0., max);
  fHist_TimeEnergy   = new TH2D("hTimeEnergy", "Energy of Gammas within Time Window", 100, -500., 500., nbins, 0., max);
  fHist_MeanTOffset  = new TH1I("hMeanTOffset", "Mean offset from nearest muon taken over MIDAS event", 4000, -20000., 20000.);
  cwd->cd();
  ThrowIfInputsInsane(opts);
}

GeSpectrum::~GeSpectrum(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int GeSpectrum::BeforeFirstEntry(TGlobalData* gData, const TSetupData *setup){
  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int GeSpectrum::ProcessEntry(TGlobalData* gData, const TSetupData *setup){

  static const double time_window_small = 500.; // ns
  static const double time_window_big = 5000.; // ns

  static const std::string bank_musc = TSetupData::Instance()->GetBankName(fMuSc.str());
  static const std::string bank_ges  = TSetupData::Instance()->GetBankName(fGeS.str());
  static const std::string bank_gef  = TSetupData::Instance()->GetBankName(fGeF.str());

  const std::map< std::string, std::vector<TPulseIsland*> >& TPIMap = gData->fPulseIslandToChannelMap;
  ThrowIfGeInsane(TPIMap.at(bank_ges), TPIMap.at(bank_gef));
  if (!(TPIMap.count(bank_musc) && TPIMap.count(bank_ges) && TPIMap.count(bank_gef)))
    return 0;
  else if (TPIMap.at(bank_musc).size() == 0 || TPIMap.at(bank_ges).size() == 0 || TPIMap.at(bank_gef).size() == 0)
    return 0;

  const std::vector<double> muScTimes  = CalculateTimes(fMuSc,   TPIMap.at(bank_musc));
  const std::vector<double> geTimes    = CalculateTimes(fGeF,    TPIMap.at(bank_gef));
  const std::vector<double> geEnergies = CalculateEnergies(fGeS, TPIMap.at(bank_ges));


  //************************************//
  //***** First get average offset *****//
  //************************************//
  TH1I hTOff("hTOff", "Time Offset", 4000, -20000., 20000.);
  for (std::vector<double>::const_iterator geT = geTimes.begin(), prev = muScTimes.begin(), next;
       geT != geTimes.end();
       ++geT) {
    static const double unfound = 1e9;
    double dt[2] = {unfound, unfound}, &dt_prev = dt[0], &dt_next = dt[1];
    next = std::upper_bound(prev, muScTimes.end(), *geT);
    prev = next - 1;
    if (next == muScTimes.end())
      dt_prev = *geT - *prev;
    else if (next == muScTimes.begin()) {
      dt_next = *geT - *next;
      prev = next;
    } else {
      dt_prev = *geT - *prev;
      dt_next = *geT - *next;
    }
    
    for (unsigned int it = 0; it < 2; ++it)
      if (std::abs(dt[it]) < 20000.)
	hTOff.Fill(dt[it]);

  }
  double tOff = 0.;
  if (hTOff.Integral() > 1) {
    tOff = hTOff.GetMean();
    fHist_MeanTOffset->Fill(tOff);
  }


  //**************************//
  //***** Now make plots *****//
  //**************************//
  for (std::vector<double>::const_iterator geT = geTimes.begin(), geE = geEnergies.begin(), prev = muScTimes.begin(), next;
       geT != geTimes.end() && geE != geEnergies.end();
       ++geT, ++geE) {
    const double unfound = 1e9;
    double dt_prev = unfound, dt_next = unfound;
  
    // Find the time difference with the most recent muon
    // in any direction.
    next = std::upper_bound(prev, muScTimes.end(), *geT);
    prev = next - 1;
    if (next == muScTimes.end()) {
      dt_prev = *geT - *prev;
    } else if (next == muScTimes.begin()) {
      dt_next = *geT - *next;
      prev = next;
    } else {
      dt_prev = *geT - *prev;
      dt_next = *geT - *next;
    }

    if ((dt_prev == unfound || dt_prev > time_window_big) && (dt_next == unfound || dt_next < -time_window_big))
      fHist_EnergyFarOOT->Fill(*geE);
    else if ((dt_prev == unfound || dt_prev > time_window_small) && (dt_next == unfound || dt_next < -time_window_small))
      fHist_EnergyOOT->Fill(*geE);
    else {
      if ((dt_prev != unfound && dt_prev < time_window_small) && (dt_next != unfound && dt_next > -time_window_small)) {
	fHist_TimeEnergy->Fill(dt_prev, *geE);
      } else if (dt_next != unfound && dt_next > -time_window_small) {
	fHist_TimeEnergy->Fill(dt_next, *geE);
      } else if (dt_prev != unfound && dt_prev < time_window_small) {
	fHist_TimeEnergy->Fill(dt_prev, *geE);
      }
    }

    fHist_Energy->Fill(*geE);

    // Silicon XRay
    //if (ge_energy >= 3260 && energy_ge <= 3290)
    // Aluminium XRay
    //if (*geE >= 340 && *geE <= 350) {
    // Mg27 Gamma
    // if (*geE >= 838 && *geE <= 848) {
    //   if (dt_prev != unfound) {
    // 	fHist_Time->Fill(dt_prev);
    // 	fHist_MoreTime->Fill(dt_prev);
    //   }
    //   if (dt_next != unfound) {
    // 	fHist_Time->Fill(dt_next);
    // 	fHist_MoreTime->Fill(dt_next);
    //   }
    // }
  }
  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int GeSpectrum::AfterLastEntry(TGlobalData* gData, const TSetupData *setup){
  return 0;
}


std::vector<double> GeSpectrum::CalculateTimes(const IDs::channel& ch, const std::vector<TPulseIsland*>& tpis) {
  std::vector<double> t;
  if (ch == fMuSc)
    for (unsigned int i = 0; i < tpis.size(); ++i)
      t.push_back(fCFTimeMuSc(tpis[i]));
  else if (ch == fGeF)
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
      e.push_back(E(fMBAmpGe(tpis[i])));
  else
    throw std::logic_error("GeSpectrum: Invalid channel to calculate energies for.");
  return e;
}


double E(const int adc) {
  return 0.1221*(double)adc+0.7987;
}


void GeSpectrum::ThrowIfGeInsane(const std::vector<TPulseIsland*>& ge1s, const std::vector<TPulseIsland*>& ge2s) {
  if (ge1s.size() != ge2s.size())
    throw std::logic_error("Fast and slow vectors are not same size!");
  for (std::vector<TPulseIsland*>::const_iterator ge1 = ge1s.begin(), ge2 = ge2s.begin(); ge1 != ge1s.end(); ++ge1, ++ge2)
    if ((*ge1)->GetTimeStamp() != (*ge2)->GetTimeStamp())
      throw std::logic_error("Fast and slow timestamps don't match up when getting time");
}

void GeSpectrum::ThrowIfInputsInsane(const modules::options* opts) {
  const std::string& gef_cfg = opts->GetString("gef_cfg");
  const double       gef_cf  = opts->GetDouble("gef_cf");
  const double       musc_cf = opts->GetDouble("musc_cf");
  const std::string  cf_str("constant_fraction=");
  std::stringstream ss(gef_cfg.substr(gef_cfg.find(cf_str) + cf_str.size()));
  double cf;
  ss >> cf;
  if (cf != gef_cf)
    throw std::logic_error("GeSpectrum: GeF CF in options don't match.");
  if (gef_cf <= 0. || gef_cf > 1. || musc_cf <= 0. || musc_cf > 1.)
    throw std::logic_error("GeSpectrum: CF are out of reasonable bounds (0. to 1.).");
}

ALCAP_REGISTER_MODULE(GeSpectrum, musc_cf, gef_gen, gef_cfg, gef_cf);
