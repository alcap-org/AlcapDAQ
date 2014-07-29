#include "GeSpectrum.h"
#include "RegisterModule.inc"
#include "TPulseIsland.h"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"

#include "TH1I.h"
#include "TH2I.h"
#include "TDirectory.h"

#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <numeric>

static bool MuScLess(TPulseIsland*, TPulseIsland*)
static double GetTime(const TPulseIsland*, const int);
static double GetGeTime(const TPulseIsland* slow, const TPulseIsland* fast);
static double GetMuScTime(const TPulseIsland*);
static double GetGeEnergy(const TPulseIsland*);
static bool IsMuScHit(const TPulseIsland*);
static bool IsGeHit(const TPulseIsland* slow, const TPulseIsland* fast);

GeSpectrum::GeSpectrum(modules::options* opts): BaseModule("GeSpectrum",opts) {
  const static double max = std::pow(2.,14);
  TDirectory* cwd = TDirectory::CurrentDirectory();
  dir->cd();
  fHist_Energy       = new TH1I("hEnergy", "Energy of Gammas", (int)max, 0., max);
  fHist_Time         = new TH1I("hTime", "Time of Gammas withing Energy Window", 1000, -10000., 10000.);
  fHist_EnergyOOT    = new TH1I("hEnergyOOT", "Energy of Gammas outside of Time Window", (int)max, 0., max);
  fHist_EnergyFarOOT = new TH1I("hEnergyFarOOT", "Energy of Gammas far from Muons", (int)max, 0., max);
  fHist_TimeEnergy   = new TH2I("hTimeEnergy", "Energy of Gammas within Time Window", 100, -200., 200., (int)max, 0., max);
  cwd->cd();
}

GeSpectrum::~GeSpectrum(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int GeSpectrum::BeforeFirstEntry(TGlobalData* gData, TSetupData *setup){
  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int GeSpectrum::ProcessEntry(TGlobalData* gData,TSetupData *setup){
  static const std::string name_muSc("muSc");
  static const std::string name_GeS("Ge-S");
  static const std::string name_GeF("Ge-F");
  static const std::string name_SiRS("SiR1-S");
  static const std::string name_SiRF("SiR1-F");
  static const std::string bank_muSc(setup->GetBankName(name_muSc));
  static const std::string bank_GeS(setup->GetBankName(name_GeS));
  static const std::string bank_GeF(setup->GetBankName(name_GeF));
  static const std::string bank_SiRS(setup->GetBankName(name_SiRS));
  static const std::string bank_SiRF(setup->GetBankName(name_SiRF));

  static const double time_window = 200.; // ns
  static const double time_window_big = 2000.; // ns

  std::map< std::string, std::vector<TPulseIsland*> >& TPIMap = gData->fPulseIslandToChannelMap;
  if (!(TPIMap.count(bank_muSc) and TPIMap.count(bank_GeS) and TPIMap.count(bank_GeF))) {
    // Sanity check for GeS/GeF
    if (TPIMap.count(bank_GeS) != TPIMap.count(bank_GeF)) {
      std::cout << "There is only one type of Germanium pulse!" << std::endl;
      return 1;
    }
    return 0;
  }
  else if (TPIMap[bank_muSc].size() == 0 or TPIMap[bank_GeS].size() == 0 or TPIMap[bank_GeF].size() == 0) {
    // Sanity check for GeS/GeF
    if ( (TPIMap[bank_GeS].size() == 0) != (TPIMap[bank_GeF].size() == 0) ) {
      std::cout << "Only one of Germanium pulse lists is empty!" << std::endl;
      return 1;
    }
    return 0;
  }

  std::vector<TPulseIsland*>::iterator beg_musc = TPIMap[bank_muSc].begin();
  std::vector<TPulseIsland*>::iterator beg_ges  = TPIMap[bank_GeS].begin();
  std::vector<TPulseIsland*>::iterator beg_gef  = TPIMap[bank_GeF].begin();
  std::vector<TPulseIsland*>::iterator end_musc = TPIMap[bank_muSc].end();
  std::vector<TPulseIsland*>::iterator end_ges  = TPIMap[bank_GeS].end();
  std::vector<TPulseIsland*>::iterator end_gef  = TPIMap[bank_GeF].end();
  std::vector<TPulseIsland*>::iterator musc;
  std::vector<TPulseIsland*>::iterator ges;
  std::vector<TPulseIsland*>::iterator gef;
  std::vector<TPulseIsland*>::iterator next = end_musc;
  std::vector<TPulseIsland*>::iterator prev = beg_musc;

  ges = beg_ges - 1;
  gef = beg_gef - 1;
  while (++ges != end_ges && ++gef != end_gef) {
    double ge_time = GetGeTime(ges, gef);
    double ge_energy = GetGeEnergy(ges);
    double dt;

    // Find the time difference with the most recent muon
    // in any direction.
    next = std::upper_bound(prev, end_musc, gef, MuScLess);
    prev = next - 1;
    if (next == end_musc)
      // If we've reached the end of the muSc hits,
      // the last Ge hit happened after the last muSc
      // hit.
      dt = GetGeTime(ges, gef) - GetMuScTime(prev);
    else if (next == beg_musc)
      // If we're at the beginning, then the first
      // Ge hit happened before the first muSc hit.
      dt = GetGeTime(ges, gef) - GetMuScTime(next);
    else {
      // Here there is a muSc hit before and after
      // this Ge hit; we look at the closest muon.
      double dt_prev = ge_time - GetMuScTime(prev);
      double dt_next = ge_time - GetMuScTime(next);
      dt = std::abs(dt_next) < std::abs(dt_prev) ? dt_next : dt_prev;
    }

    if (std::abs(dt) > time_window_big)
      fHist_EnergyFarOOT->Fill(ge_energy);
    else if (std::abs(dt) > time_window_small)
      fHist_EnergyOOT->Fill(ge_energy);
    else
      fHist_TimeEnergy->Fill(dt, ge_energy);
    fHist_Energy->Fill(ge_energy);
    // Silicon XRay
    //if (ge_energy >= 3260 && energy_ge <= 3290)
    // Aluminium XRay
    if (ge_energy >= 2813 && ge_energy <= 2853)
      fHist_Time->Fill(dt);

    ++ges; ++gef;
    // Sanity check
    if ( (ges == end_ges) != (gef == end_gef) ) {
      static const std::string msg("Fast and slow timestamps don't match up when getting time");
      throw std::logic_error(msg);
    }
  }

  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int GeSpectrum::AfterLastEntry(TGlobalData* gData,TSetupData *setup){
  return 0;
}


double GetMuScTime(const TPulseIsland* tpi) {
  const static int pol = -1;
  return GetTime(tpi, pol);
}

double GetGeTime(const TPulseIsland* slow, const TPulseIsland* fast) {
  const static int pol = -1;
  if (slow->GetTimeStamp() != fast->GetTimeStamp()) {
    static const std::string msg("Fast and slow timestamps don't match up when getting time");
    throw std::logic_error(msg);
  }
  return GetTime(fast, pol);
}
 
double GetTime(const TPulseIsland* tpi, const int pol) {
  const static double cf_frac = 0.2;
  const std::vector<int>& samps = tpi->GetSamples();
  const std::vector<int>::const_iterator b = samps.begin(), e = samps.end();
  const unsigned int ped = std::accumulate(b, b + 5, 0)/5.;
  std::vector<int>::const_iterator m = pol > 0 ? std::max_element(b, e) : std::min_element(b, e);
  const unsigned int amp = *m;
  const unsigned int cf = pol > 0 ? (unsigned int)(cf_frac*(double)(amp-ped)) + ped : (unsigned int)((double)(ped-amp)*(1.-cf_frac) + amp);
  std::vector<int>::const_iterator c = m;
  unsigned int x = 0;
  while (m != b && (pol > 0 ? *--m > (int)cf : *--m < (int)cf)) {
    if (++x > 100000) {
      std::cout << "ERROR: ";
      for (unsigned int i = 0; i < samps.size(); ++i)
	std::cout << samps[i] << " ";
      std::cout << std::endl;
    }
  }
  double dx = (double)(m-b);
  if (*(m+1) != *m)
    dx += (double)((int)cf - *m)/(double)(*(m+1) - *m);
  return (dx + (double)tpi->GetTimeStamp()) * tpi->GetClockTickInNs() - TSetupData::Instance()->GetTimeShift(tpi->GetBankName());
}

double GetGeEnergy(const TPulseIsland* tpi) {
  const std::vector<int>& samps = tpi->GetSamples();
  const std::vector<int>::const_iterator b = samps.begin(), e = samps.end();
  const int ped = std::accumulate(b, b + 5, 0)/5.;
  const int max = *std::max_element(b, e);
  return (double)(max - ped);
}

bool MuScLess(const TPulseIsland* tpi1, const TPulseIsland* tpi2) {
  return GetMuScTime(tpi1) < GetMuScTime(tpi2);
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(GeSpectrum);
