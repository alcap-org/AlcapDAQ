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
  fHist_ADC(NULL), fHist_Energy(NULL), fHist_Time(NULL), fHist_MoreTime(NULL),
  fHist_ADCOOT(NULL), fHist_EnergyOOT(NULL), fHist_ADCFarOOT(NULL), fHist_EnergyFarOOT(NULL),
  fHist_TimeOOT(NULL), fHist_TimeFarOOT(NULL), fHist_TimeADC(NULL), fHist_TimeEnergy(NULL), fHist_Livetime(NULL),
  fHist_PP_ADC(NULL), fHist_PP_Energy(NULL), fHist_PP_Time(NULL), fHist_PP_MoreTime(NULL),
  fHist_PP_ADCOOT(NULL), fHist_PP_EnergyOOT(NULL), fHist_PP_ADCFarOOT(NULL), fHist_PP_EnergyFarOOT(NULL),
  fHist_PP_TimeOOT(NULL), fHist_PP_TimeFarOOT(NULL), fHist_PP_TimeADC(NULL), fHist_PP_TimeEnergy(NULL), fHist_PP_Livetime(NULL),
  fHist_MeanTOffset(NULL),
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
	      opts->GetDouble("musc_cf")),
  fADC2Energy(new TF1("adc2energy","[0]*x+[1]")),
  fTimeWindow_Small(200.), fTimeWindow_Big(5000.) {
  ThrowIfInputsInsane(opts);

  const static int nbins = std::pow(2.,14);
  const std::pair<double,double> adc2energy_par = SetupNavigator::Instance()->GetEnergyCalibrationConstants(IDs::channel("Ge-S"));
  fADC2Energy->SetParameters(adc2energy_par.first, adc2energy_par.second);
  TDirectory* cwd = TDirectory::CurrentDirectory();
  dir->cd();

  fHist_ADC          = new TH1D("hADC", "Energy of Gammas;Energy (ADC);Counts", nbins, 0., nbins);
  fHist_Energy       = new TH1D("hEnergy", "Energy of Gammas;Energy (keV);Counts", nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
  fHist_Time         = new TH1D("hTime", "Time of Gammas within Energy Window", 1000, -10000., 10000.);
  fHist_MoreTime     = new TH1D("hMoreTime", "Time of Gammas within Energy Window (Wide)", 1000, -100000., 100000.);
  fHist_ADCOOT       = new TH1D("hADCOOT", "Energy of Gammas outside of Time Window;Energy (ADC);Counts", nbins, 0., nbins);
  fHist_EnergyOOT    = new TH1D("hEnergyOOT", "Energy of Gammas outside of Time Window;Energy (keV);Counts", nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
  fHist_ADCFarOOT    = new TH1D("hADCFarOOT", "Energy of Gammas far from Muons;Energy (ADC);Counts", nbins, 0., nbins);
  fHist_EnergyFarOOT = new TH1D("hEnergyFarOOT", "Energy of Gammas far from Muons;Energy (keV);Counts", nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
  fHist_TimeOOT      = new TH1D("hTimeOOT", "Time of Gammas outside of Time Window", 1000., -2.*fTimeWindow_Small, 2.*fTimeWindow_Small);
  fHist_TimeFarOOT   = new TH1D("hTimeFarOOT", "Time of Gammas far from Muons", 1000., -100.*fTimeWindow_Big, 100.*fTimeWindow_Big);
  fHist_TimeADC      = new TH2D("hTimeADC", "Energy of Gammas within Time Window;Energy (ADC);Time (ns);Counts", 100, -fTimeWindow_Small, fTimeWindow_Small, nbins, 0., nbins);
  fHist_TimeEnergy   = new TH2D("hTimeEnergy", "Energy of Gammas within Time Window;Energy (keV);Time (ns);Counts", 100, -fTimeWindow_Small, fTimeWindow_Small, nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
  fHist_Livetime     = new TH1D("hLivetime", "Livetime of different windows;Window;Livetime", 3, 0, 3.);

  fHist_PP_ADC          = new TH1D("hPPADC", "Energy of Gammas;Energy (ADC);Counts", nbins, 0., nbins);
  fHist_PP_Energy       = new TH1D("hPPEnergy", "Energy of Gammas;Energy (keV);Counts", nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
  fHist_PP_Time         = new TH1D("hPPTime", "Time of Gammas within Energy Window", 1000, -10000., 10000.);
  fHist_PP_MoreTime     = new TH1D("hPPMoreTime", "Time of Gammas within Energy Window (Wide)", 1000, -100000., 100000.);
  fHist_PP_ADCOOT       = new TH1D("hPPADCOOT", "Energy of Gammas outside of Time Window;Energy (ADC);Counts", nbins, 0., nbins);
  fHist_PP_EnergyOOT    = new TH1D("hPPEnergyOOT", "Energy of Gammas outside of Time Window;Energy (keV);Counts", nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
  fHist_PP_ADCFarOOT    = new TH1D("hPPADCFarOOT", "Energy of Gammas far from Muons;Energy (ADC);Counts", nbins, 0., nbins);
  fHist_PP_EnergyFarOOT = new TH1D("hPPEnergyFarOOT", "Energy of Gammas far from Muons;Energy (keV);Counts", nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
  fHist_PP_TimeOOT      = new TH1D("hPPTimeOOT", "Time of Gammas outside of Time Window", 1000., -2.*fTimeWindow_Small, 2.*fTimeWindow_Small);
  fHist_PP_TimeFarOOT   = new TH1D("hPPTimeFarOOT", "Time of Gammas far from Muons", 1000., -100.*fTimeWindow_Big, 100.*fTimeWindow_Big);
  fHist_PP_TimeADC      = new TH2D("hPPTimeADC", "Energy of Gammas within Time Window;Energy (ADC);Time (ns);Counts", 100, -fTimeWindow_Small, fTimeWindow_Small, nbins, 0., nbins);
  fHist_PP_TimeEnergy   = new TH2D("hPPTimeEnergy", "Energy of Gammas within Time Window;Energy (keV);Time (ns);Counts", 100, -fTimeWindow_Small, fTimeWindow_Small, nbins, fADC2Energy->Eval(0.), fADC2Energy->Eval(nbins));
  fHist_PP_Livetime     = new TH1D("hPPLivetime", "Livetime of different windows;Window;Livetime", 3, 0, 3.);

  fHist_MeanTOffset  = new TH1D("hMeanTOffset", "Mean offset from nearest muon taken over MIDAS event", 4000, -4.*fTimeWindow_Big, 4.*fTimeWindow_Big);
  fHist_Livetime->GetXaxis()->SetBinLabel(1, "FarOOT"); fHist_Livetime->GetXaxis()->SetBinLabel(2, "OOT"); fHist_Livetime->GetXaxis()->SetBinLabel(3, "Prompt");
  fHist_PP_Livetime->GetXaxis()->SetBinLabel(1, "FarOOT"); fHist_PP_Livetime->GetXaxis()->SetBinLabel(2, "OOT"); fHist_PP_Livetime->GetXaxis()->SetBinLabel(3, "Prompt");
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
    bool prev_found = false, next_found = false;
    double dt_prev = 1e9, dt_next = 1e9;
  
    // Find the time difference with the most recent muon
    // both before and after.
    next = std::upper_bound(prev, muScTimes.end(), *geT);
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
    const bool pp = IsGePileupProtected(geT, prev, next, muScTimes, 10000.);

    // Plot energy
    const Double_t adc = *geE;
    const Double_t en = fADC2Energy->Eval(adc);
    if ( (!prev_found || dt_prev > fTimeWindow_Big) && (!next_found || dt_next < -fTimeWindow_Big) ) {
      fHist_ADCFarOOT->Fill(adc);
      fHist_EnergyFarOOT->Fill(en);
      if (pp) {
	fHist_PP_ADCFarOOT->Fill(adc);
	fHist_PP_EnergyFarOOT->Fill(en);
      }
    } else if ( (!prev_found || dt_prev > fTimeWindow_Small) && (!next_found || dt_next < -fTimeWindow_Small) ) {
      fHist_ADCOOT->Fill(adc);
      fHist_EnergyOOT->Fill(en);
      if (pp) {
	fHist_PP_ADCOOT->Fill(adc);
	fHist_PP_EnergyOOT->Fill(en);
      }
    } else {
      if (prev_found && dt_prev <= fTimeWindow_Small) {
	fHist_TimeADC->Fill(dt_prev, adc);
	fHist_TimeEnergy->Fill(dt_prev, en);
	if (pp) {
	  fHist_PP_TimeADC->Fill(dt_prev, adc);
	  fHist_PP_TimeEnergy->Fill(dt_prev, en);
	}
      } else if (next_found && dt_next >= -fTimeWindow_Small) {
	fHist_TimeADC->Fill(dt_next, adc);
	fHist_TimeEnergy->Fill(dt_next, en);
	if (pp) {
	  fHist_PP_TimeADC->Fill(dt_next, adc);
	  fHist_PP_TimeEnergy->Fill(dt_next, en);
	}
      } else {
	std::cout << "WARNING: Unexpected logical branch! Prev: (" << prev_found << ", " << dt_prev << "), Next: (" << next_found << ", " << dt_next << ")" << std::endl;
      }
    }
    fHist_ADC->Fill(adc);
    fHist_Energy->Fill(en);
    // Plot time
    if (prev_found) {
      if (dt_prev > fTimeWindow_Big)
	fHist_TimeFarOOT->Fill(dt_prev);
      else if (dt_prev > fTimeWindow_Small)
	fHist_TimeOOT->Fill(dt_prev);
    }
    if (next_found) {
      if (dt_next < -fTimeWindow_Big)
	fHist_TimeFarOOT->Fill(dt_next);
      else if (dt_next < -fTimeWindow_Small)
	fHist_TimeOOT->Fill(dt_next);
    }

    // Aluminium XRay
    if (en >= 344 && en <= 350) {
      if (prev_found) {
    	fHist_Time->Fill(dt_prev);
    	fHist_MoreTime->Fill(dt_prev);
      } else if (next_found) {
    	fHist_Time->Fill(dt_next);
    	fHist_MoreTime->Fill(dt_next);
      }
    }
  }

  //*************************************//
  //******** Calculate Livetimes ********//
  //*************************************//
  Double_t t_Prompt = 2.*fTimeWindow_Small*muScTimes.size();
  Double_t t_OOT = 2.*fTimeWindow_Big*muScTimes.size() - t_Prompt;
  Double_t t_FarOOT = muScTimes.back() - muScTimes.front() - t_OOT - t_Prompt;
  fHist_Livetime->Fill("Prompt", t_Prompt);
  fHist_Livetime->Fill("OOT", t_OOT);
  fHist_Livetime->Fill("FarOOT", t_FarOOT);
  // if (pp) {
  //   fHist_PP_Livetime->Fill("Prompt", t_Prompt);
  //   fHist_PP_Livetime->Fill("OOT", t_OOT);
  //   fHist_PP_Livetime->Fill("FarOOT", t_FarOOT);
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
      e.push_back(fMBAmpGe(tpis[i]));
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

bool GeSpectrum::IsGePileupProtected(const std::vector<double>::const_iterator& ge, const std::vector<double>::const_iterator& m1, const std::vector<double>::const_iterator& m2,
				     const std::vector<double>& v, const double dt) {
  if (std::abs(*ge-*m1) < fTimeWindow_Big)
    if (!IsMuPileupProtected(m1, v, dt))
      return false;
  if (std::abs(*ge-*m2) < fTimeWindow_Big)
    return IsMuPileupProtected(m2, v, dt);
  return true;
}

bool GeSpectrum::IsMuPileupProtected(const std::vector<double>::const_iterator& i, const std::vector<double>& v, const double dt) {
  if (i == v.begin()) {
    if (*i > dt)
      return true;
    return false;
  } else if (i == v.end()) {
    return false;
  } else if (*i - *(i-1) > dt && *(i+1) - *i > dt) {
    return true;
  }
  return false;
}

ALCAP_REGISTER_MODULE(GeSpectrum, musc_cf, gef_gen, gef_cfg, gef_cf);
