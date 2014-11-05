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

static bool IsGeXRay(double en) {
  if ( (en > 344. && en < 350.) || // Al
       (en > 935. && en < 941.) || // Pb
       (en > 969. &&  en < 975.) ) // Pb
    return true;
  return false;
}

static void RemoveSmallMuScPulses(std::vector<double>& t, std::vector<double>& e) {
  for (unsigned int i = 0; i < t.size(); ++i) {
    if (e[i] < 350) {
      t.erase(t.begin() + i);
      e.erase(e.begin() + i);
    }
  }
}

static void RemoveIfCloseToTimeBoundaries(std::vector<double>& t, std::vector<double>& e) {
  for (unsigned int i = 0; i < t.size(); ++i) {
    if (t[i] < 2e6 || t[i] > 110e6) { // 2 ms < t < 110 ms
      t.erase(t.begin() + i);
      e.erase(e.begin() + i);
    }
  }
}

GeSpectrum::GeSpectrum(modules::options* opts) :
  BaseModule("GeSpectrum",opts),
  fhADC(NULL), fhEnergy(NULL), fhTime(NULL), fhMoreTime(NULL),
  fhADCOOT(NULL), fhEnergyOOT(NULL), fhADCFarOOT(NULL), fhEnergyFarOOT(NULL),
  fhTimeOOT(NULL), fhTimeFarOOT(NULL), fhTimeADC(NULL), fhTimeEnergy(NULL),
  fhLivetime(NULL), fhNMuons(NULL),
  fhPP_ADC(NULL), fhPP_Energy(NULL), fhPP_Time(NULL), fhPP_MoreTime(NULL),
  fhPP_ADCOOT(NULL), fhPP_EnergyOOT(NULL), fhPP_ADCFarOOT(NULL), fhPP_EnergyFarOOT(NULL),
  fhPP_TimeOOT(NULL), fhPP_TimeFarOOT(NULL), fhPP_TimeADC(NULL), fhPP_TimeEnergy(NULL),
  fhPP_Livetime(NULL), fhPP_NMuons(NULL),
  fhMeanTOffset(NULL), fhGePhysRes(NULL), fhGePhysResECut(NULL), fhNXRayCoinc(NULL),
  fUseSlowTiming(opts->GetBool("ge_slow_timing")),
  fMBAmpMuSc(SetupNavigator::Instance()->GetPedestal(fMuSc), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fMuSc.str()))),
  fMBAmpGe(SetupNavigator::Instance()->GetPedestal(fGeS), TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fGeS.str()))),
  fCFTimeMuSc(SetupNavigator::Instance()->GetPedestal(fMuSc),
	      TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fMuSc.str())),
	      TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fMuSc.str())),
	      0.,
	      opts->GetDouble("musc_cf")),
  fCFTimeGe(SetupNavigator::Instance()->GetPedestal(fUseSlowTiming ? fGeS : fGeF),
	    TSetupData::Instance()->GetTriggerPolarity(TSetupData::Instance()->GetBankName(fUseSlowTiming ? fGeS.str() : fGeF.str())),
	    TSetupData::Instance()->GetClockTick(TSetupData::Instance()->GetBankName(fUseSlowTiming ? fGeS.str() : fGeF.str())),
	    SetupNavigator::Instance()->GetCoarseTimeOffset(IDs::source(fUseSlowTiming ? fGeS : fGeF, IDs::generator(opts->GetString("ge_gen"), opts->GetString("ge_cfg")))),
	    opts->GetDouble("ge_cf")),
  fADC2Energy(new TF1("adc2energy","[0]*x+[1]")),
  fTimeWindow_Small(opts->GetDouble("tw_small")), fTimeWindow_Big(opts->GetDouble("tw_big")), fPileupProtectionWindow(10000.) {
  ThrowIfInputsInsane(opts);

  const static int nbins = std::pow(2.,14);
  const std::pair<double,double> adc2energy_par = SetupNavigator::Instance()->GetEnergyCalibrationConstants(IDs::channel("Ge-S"));
  fADC2Energy->SetParameters(adc2energy_par.first, adc2energy_par.second);
  TDirectory* cwd = TDirectory::CurrentDirectory();
  dir->cd();

  fhADC          = new TH1D("hADC", "Energy of Gammas;Energy (ADC)", nbins, 0., nbins);
  fhEnergy       = new TH1D("hEnergy", "Energy of Gammas;Energy (keV)", nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
  fhTime         = new TH1D("hTime", "Time of Gammas within Energy Window;Time (ns)", 2000, -1000., 1000.);
  fhMoreTime     = new TH1D("hMoreTime", "Time of Gammas within Energy Window (Wide);Time (ns)", 1000, -100000., 100000.);
  fhADCOOT       = new TH1D("hADCOOT", "Energy of Gammas outside of Time Window;Energy (ADC)", nbins, 0., nbins);
  fhEnergyOOT    = new TH1D("hEnergyOOT", "Energy of Gammas outside of Time Window;Energy (keV)", nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
  fhADCFarOOT    = new TH1D("hADCFarOOT", "Energy of Gammas far from Muons;Energy (ADC)", nbins, 0., nbins);
  fhEnergyFarOOT = new TH1D("hEnergyFarOOT", "Energy of Gammas far from Muons;Energy (keV)", nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
  fhTimeOOT      = new TH1D("hTimeOOT", "Time of Gammas outside of Time Window;Time (ns)", 1000., -2.*fTimeWindow_Small, 2.*fTimeWindow_Small);
  fhTimeFarOOT   = new TH1D("hTimeFarOOT", "Time of Gammas far from Muons;Time (ns)", 1000., -100.*fTimeWindow_Big, 100.*fTimeWindow_Big);
  fhTimeADC      = new TH2D("hTimeADC", "Energy of Gammas within Time Window;Time (ns);Energy (ADC)", 500, -fTimeWindow_Small, fTimeWindow_Small, nbins, 0., nbins);
  fhTimeEnergy   = new TH2D("hTimeEnergy", "Energy of Gammas within Time Window;Time (ns);Energy (keV)", 500, -fTimeWindow_Small, fTimeWindow_Small, nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
  fhLivetime     = new TH1D("hLivetime", "Livetime of different windows;Window;Livetime (ns)", 3, 0, 3.);
  fhNMuons       = new TH1D("hNMuons", "Number of muons in MIDAS event;Number", 1000, 0., 1000.);

  fhPP_ADC          = new TH1D("hPPADC", "Energy of Gammas;Energy (ADC) (PP)", nbins, 0., nbins);
  fhPP_Energy       = new TH1D("hPPEnergy", "Energy of Gammas;Energy (keV) (PP)", nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
  fhPP_Time         = new TH1D("hPPTime", "Time of Gammas within Energy Window (PP);Time (ns)", 2000, -1000., 1000.);
  fhPP_MoreTime     = new TH1D("hPPMoreTime", "Time of Gammas within Energy Window (Wide) (PP);Time (ns)", 1000, -100000., 100000.);
  fhPP_ADCOOT       = new TH1D("hPPADCOOT", "Energy of Gammas outside of Time Window (PP);Energy (ADC)", nbins, 0., nbins);
  fhPP_EnergyOOT    = new TH1D("hPPEnergyOOT", "Energy of Gammas outside of Time Window (PP);Energy (keV)", nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
  fhPP_ADCFarOOT    = new TH1D("hPPADCFarOOT", "Energy of Gammas far from Muons (PP);Energy (ADC)", nbins, 0., nbins);
  fhPP_EnergyFarOOT = new TH1D("hPPEnergyFarOOT", "Energy of Gammas far from Muons (PP);Energy (keV)", nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
  fhPP_TimeOOT      = new TH1D("hPPTimeOOT", "Time of Gammas outside of Time Window (PP);Time (ns)", 1000., -2.*fTimeWindow_Small, 2.*fTimeWindow_Small);
  fhPP_TimeFarOOT   = new TH1D("hPPTimeFarOOT", "Time of Gammas far from Muons (PP);Time (ns)", 1000., -100.*fTimeWindow_Big, 100.*fTimeWindow_Big);
  fhPP_TimeADC      = new TH2D("hPPTimeADC", "Energy of Gammas within Time Window (PP);Time (ns);Energy (ADC)", 500, -fTimeWindow_Small, fTimeWindow_Small, nbins, 0., nbins);
  fhPP_TimeEnergy   = new TH2D("hPPTimeEnergy", "Energy of Gammas within Time Window (PP);Time (ns);Energy (keV)", 500, -fTimeWindow_Small, fTimeWindow_Small, nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
  fhPP_Livetime     = new TH1D("hPPLivetime", "Livetime of different windows (PP);Window;Livetime (ns)", 3, 0, 3.);
  fhPP_NMuons       = new TH1D("hPPNMuons", "Number of muons in MIDAS event (PP);Number", 1000, 0., 1000.);

  fhMeanTOffset   = new TH1D("hMeanTOffset", "Mean offset from nearest muon taken over MIDAS event", 4000, -4.*fTimeWindow_Big, 4.*fTimeWindow_Big);
  fhGePhysRes     = new TH2D("hGePhysRes", "Physical Germanium Timing", 100, -500., 500., 100, 0., 100e6);
  fhGePhysResECut = new TH1D("hGePhysResECut", "Physical Germanium Timing", 1000, -500., 500.);
  fhNXRayCoinc    = new TH1D("hNXRayCoinc", "Number XRay MuSc Coincidences", 100, 0., 100.);

  fhLivetime->GetXaxis()->SetBinLabel(1, "FarOOT"); fhLivetime->GetXaxis()->SetBinLabel(2, "OOT"); fhLivetime->GetXaxis()->SetBinLabel(3, "Prompt");
  fhPP_Livetime->GetXaxis()->SetBinLabel(1, "FarOOT"); fhPP_Livetime->GetXaxis()->SetBinLabel(2, "OOT"); fhPP_Livetime->GetXaxis()->SetBinLabel(3, "Prompt");
  cwd->cd();
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
  RemoveSmallMuScPulses(muScTimes, muScEnergies);
  RemoveIfCloseToTimeBoundaries(muScTimes, muScEnergies);
  RemoveIfCloseToTimeBoundaries(geTimes, geEnergies);

  //************************************//
  //***** First get average offset *****//
  //************************************//
  TH1I hTOff("hTOff", "Time Offset", 4000, -20000., 20000.);
  std::vector<double> muScResTimes, geResTimes;
  std::vector<double> muScResTimesECut, geResTimesECut;
  for (std::vector<double>::const_iterator geT = geTimes.begin(), geE = geEnergies.begin(), prev = muScTimes.begin(), next;
       geT != geTimes.end();
       ++geT, ++geE) {
    static const double unfound = 1e9;
    double dt[2] = {unfound, unfound}, &dt_prev = dt[0], &dt_next = dt[1];
    next = std::upper_bound(prev, (std::vector<double>::const_iterator)muScTimes.end(), *geT);
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

    if (std::abs(dt_next) < 200. && std::abs(dt_next) < std::abs(dt_prev)) {
      muScResTimes.push_back(*next);
      geResTimes.push_back(*geT);
    } else if (std::abs(dt_prev) < 200.) {
      muScResTimes.push_back(*prev);
      geResTimes.push_back(*geT);
    }
    if (IsGeXRay(fADC2Energy->Eval(*geE))) {
      if (std::abs(dt_next) < 200. && std::abs(dt_next) < std::abs(dt_prev)) {
	muScResTimesECut.push_back(*next);
	geResTimesECut.push_back(*geT);
      } else if (std::abs(dt_prev) < 200.) {
	muScResTimesECut.push_back(*prev);
	geResTimesECut.push_back(*geT);
      }
    }

  }
  double tOff = 0.;
  if (hTOff.Integral() > 1) {
    tOff = hTOff.GetMean();
    fhMeanTOffset->Fill(tOff);
  }

  //******************************************************************************//
  //***** Try to determine germanium physical timing resolution restrictions *****//
  //******************************************************************************//
  if (muScResTimes.size() > 1)
    for (unsigned int i = 1; i < muScResTimes.size(); ++i)
      for (unsigned int j = 0; j < i; ++j)
	fhGePhysRes->Fill( (geResTimes[i] - geResTimes[j]) - (muScResTimes[i] - muScResTimes[j]) , geResTimes[i] - geResTimes[j]);
  if (muScResTimesECut.size() > 1)
    for (unsigned int i = 1; i < muScResTimesECut.size(); ++i)
      for (unsigned int j = 0; j < i; ++j)
	fhGePhysResECut->Fill( (geResTimesECut[i] - geResTimesECut[j]) - (muScResTimesECut[i] - muScResTimesECut[j]) );
  fhNXRayCoinc->Fill(muScResTimesECut.size());

  //**************************//
  //***** Now make plots *****//
  //**************************//
  for (std::vector<double>::const_iterator geT = geTimes.begin(), geE = geEnergies.begin(), prev = muScTimes.begin(), next;
       geT != geTimes.end() && geE != geEnergies.end();
       ++geT, ++geE) {
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
    const bool pp = IsGePileupProtected(geT, prev, next, muScTimes);

    ////////////////////////////////////////
    // Plot energy in different time windows
    ////////////////////////////////////////
    const Double_t adc = *geE;
    const Double_t en = fADC2Energy->Eval(adc);
    if ( (!prev_found || dt_prev > fTimeWindow_Big) && (!next_found || dt_next < -fTimeWindow_Big) ) {
      fhADCFarOOT->Fill(adc);
      fhEnergyFarOOT->Fill(en);
      if (pp) {
	fhPP_ADCFarOOT->Fill(adc);
	fhPP_EnergyFarOOT->Fill(en);
      }
    } else if ( (!prev_found || dt_prev > fTimeWindow_Small) && (!next_found || dt_next < -fTimeWindow_Small) ) {
      fhADCOOT->Fill(adc);
      fhEnergyOOT->Fill(en);
      if (pp) {
	fhPP_ADCOOT->Fill(adc);
	fhPP_EnergyOOT->Fill(en);
      }
    } else {
      if (prev_found && dt_prev <= fTimeWindow_Small) {
	fhTimeADC->Fill(dt_prev, adc);
	fhTimeEnergy->Fill(dt_prev, en);
	if (pp) {
	  fhPP_TimeADC->Fill(dt_prev, adc);
	  fhPP_TimeEnergy->Fill(dt_prev, en);
	}
      } else if (next_found && dt_next >= -fTimeWindow_Small) {
	fhTimeADC->Fill(dt_next, adc);
	fhTimeEnergy->Fill(dt_next, en);
	if (pp) {
	  fhPP_TimeADC->Fill(dt_next, adc);
	  fhPP_TimeEnergy->Fill(dt_next, en);
	}
      } else {
	std::cout << "WARNING: Unexpected logical branch! Prev: (" << prev_found << ", " << dt_prev << "), Next: (" << next_found << ", " << dt_next << ")" << std::endl;
      }
    }
    fhADC->Fill(adc);
    fhEnergy->Fill(en);
    if (pp) {
      fhPP_ADC->Fill(adc);
      fhPP_Energy->Fill(en);
    }
    // Plot time
    if (prev_found) {
      if (dt_prev > fTimeWindow_Big) {
	fhTimeFarOOT->Fill(dt_prev);
	if (pp)
	  fhPP_TimeFarOOT->Fill(dt_prev);
      } else if (dt_prev > fTimeWindow_Small) {
	fhTimeOOT->Fill(dt_prev);
	if (pp)
	  fhPP_TimeFarOOT->Fill(dt_next);
      }
    }
    if (next_found) {
      if (dt_next < -fTimeWindow_Big) {
	fhTimeFarOOT->Fill(dt_next);
	if (pp)
	  fhPP_TimeFarOOT->Fill(dt_next);
      } else if (dt_next < -fTimeWindow_Small) {
	fhTimeOOT->Fill(dt_next);
	if (pp)
	  fhPP_TimeOOT->Fill(dt_next);
      }
    }

    const Double_t en_bounds[2] = { 344., 350. }; // Aluminium XRay
    if (en >= en_bounds[0] && en <= en_bounds[1]) {
      if (prev_found) {
    	fhTime->Fill(dt_prev);
    	fhMoreTime->Fill(dt_prev);
	if (pp) {
	  fhPP_Time->Fill(dt_prev);
	  fhPP_MoreTime->Fill(dt_prev);
	}
      }
      if (next_found) {
    	fhTime->Fill(dt_next);
    	fhMoreTime->Fill(dt_next);
	if (pp) {
	  fhPP_Time->Fill(dt_next);
	  fhPP_MoreTime->Fill(dt_next);
	}
      }
    }
  }

  //*************************************//
  //************ Count Muons ************//
  //*************************************//
  unsigned int pp_nmu = 0;
  for (unsigned int i = 0; i < muScTimes.size(); ++i)
    if (IsMuPileupProtected(muScTimes.begin() + i, muScTimes))
      ++pp_nmu;
  fhNMuons->Fill(muScEnergies.size());
  fhPP_NMuons->Fill((Double_t)pp_nmu);

  //*************************************//
  //******** Calculate Livetimes ********//
  //*************************************//
  Double_t t_Prompt = 2.*fTimeWindow_Small*muScTimes.size();
  Double_t t_OOT = 2.*fTimeWindow_Big*muScTimes.size() - t_Prompt;
  Double_t t_FarOOT = muScTimes.back() - muScTimes.front() - t_OOT - t_Prompt;
  Double_t t_PP_Prompt = 2.*fTimeWindow_Small*pp_nmu;
  Double_t t_PP_OOT = 2.*fTimeWindow_Big*pp_nmu - t_PP_Prompt;
  // Double_t t_PP_FarOOT = ?????
  fhLivetime->Fill("Prompt", t_Prompt);
  fhLivetime->Fill("OOT", t_OOT);
  fhLivetime->Fill("FarOOT", t_FarOOT);
  // if (pp) {
  //   fhPP_Livetime->Fill("Prompt", t_Prompt);
  //   fhPP_Livetime->Fill("OOT", t_OOT);
  //   fhPP_Livetime->Fill("FarOOT", t_FarOOT);
  // }

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

bool GeSpectrum::IsGePileupProtected(const std::vector<double>::const_iterator& ge, const std::vector<double>::const_iterator& m1, const std::vector<double>::const_iterator& m2,
				     const std::vector<double>& v) {
  if (std::abs(*ge-*m1) < fPileupProtectionWindow)
    if (!IsMuPileupProtected(m1, v))
      return false;
  if (std::abs(*ge-*m2) < fPileupProtectionWindow)
    return IsMuPileupProtected(m2, v);
  return true;
}

bool GeSpectrum::IsMuPileupProtected(const std::vector<double>::const_iterator& i, const std::vector<double>& v) {
  if (i == v.begin()) {
    if (*i > fPileupProtectionWindow)
      return true;
    return false;
  } else if (i == v.end()) {
    return false;
  } else if ( (*i - *(i-1) > fPileupProtectionWindow) && (*(i+1) - *i > fPileupProtectionWindow) ) {
    return true;
  }
  return false;
}

ALCAP_REGISTER_MODULE(GeSpectrum, musc_cf, ge_gen, ge_cfg, ge_cf, ge_slow_timing, tw_small, tw_big);
