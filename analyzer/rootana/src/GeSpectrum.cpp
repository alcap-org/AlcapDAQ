#include "GeSpectrumWithTFix.h"
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
#include <cmath>
#include <climits>


static bool TPITimeLess(const TPulseIsland*, const TPulseIsland*);
static double GetTime(const TPulseIsland*, const int);
static double GetGeTime(const TPulseIsland* slow, const TPulseIsland* fast);
static double GetMuScTime(const TPulseIsland*);
static double GetGeEnergy(const TPulseIsland*);

GeSpectrumWithTFix::GeSpectrumWithTFix(modules::options* opts): BaseModule("GeSpectrumWithTFix",opts) {
  const static double max = std::pow(2.,14);
  TDirectory* cwd = TDirectory::CurrentDirectory();
  dir->cd();
  fHist_Energy       = new TH1I("hEnergy", "Energy of Gammas", (int)max, 0., max);
  fHist_Time         = new TH1I("hTime", "Time of Gammas within Energy Window", 1000, -10000., 10000.);
  fHist_MoreTime     = new TH1I("hMoreTime", "Time of Gammas within Energy Window (Wide)", 1000, -100000., 100000.);
  fHist_EnergyOOT    = new TH1I("hEnergyOOT", "Energy of Gammas outside of Time Window", (int)max, 0., max);
  fHist_EnergyFarOOT = new TH1I("hEnergyFarOOT", "Energy of Gammas far from Muons", (int)max, 0., max);
  fHist_TimeEnergy   = new TH2I("hTimeEnergy", "Energy of Gammas within Time Window", 100, -200., 200., (int)max, 0., max);
  fHist_MeanTOffset  = new TH1I("hMeanTOffset", "Mean offset from nearest muon taken over MIDAS event", 400, -200., 200.);
  cwd->cd();
}

GeSpectrumWithTFix::~GeSpectrumWithTFix(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int GeSpectrumWithTFix::BeforeFirstEntry(TGlobalData* gData, TSetupData *setup){
  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int GeSpectrumWithTFix::ProcessEntry(TGlobalData* gData,TSetupData *setup){
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

  static const double time_window_small = 200.; // ns
  static const double time_window_big = 2000.; // ns

  const std::map< std::string, std::vector<TPulseIsland*> >& TPIMap = gData->fPulseIslandToChannelMap;
  if (!(TPIMap.count(bank_muSc) && TPIMap.count(bank_GeS) && TPIMap.count(bank_GeF))) {
    // Sanity check for GeS/GeF
    if (TPIMap.count(bank_GeS) != TPIMap.count(bank_GeF)) {
      std::cout << "There is only one type of Germanium pulse!" << std::endl;
      return 1;
    }
    return 0;
  }
  else if (TPIMap.at(bank_muSc).size() == 0 || TPIMap.at(bank_GeS).size() == 0 || TPIMap.at(bank_GeF).size() == 0) {
    // Sanity check for GeS/GeF
    if ( (TPIMap.at(bank_GeS).size() == 0) != (TPIMap.at(bank_GeF).size() == 0) ) {
      std::cout << "Only one of Germanium pulse lists is empty!" << std::endl;
      return 1;
    }
    return 0;
  }

  const std::vector<TPulseIsland*>::const_iterator beg_musc = TPIMap.at(bank_muSc).begin();
  const std::vector<TPulseIsland*>::const_iterator beg_ges  = TPIMap.at(bank_GeS).begin();
  const std::vector<TPulseIsland*>::const_iterator beg_gef  = TPIMap.at(bank_GeF).begin();
  const std::vector<TPulseIsland*>::const_iterator end_musc = TPIMap.at(bank_muSc).end();
  const std::vector<TPulseIsland*>::const_iterator end_ges  = TPIMap.at(bank_GeS).end();
  const std::vector<TPulseIsland*>::const_iterator end_gef  = TPIMap.at(bank_GeF).end();
  std::vector<TPulseIsland*>::const_iterator musc;
  std::vector<TPulseIsland*>::const_iterator ges;
  std::vector<TPulseIsland*>::const_iterator gef;
  std::vector<TPulseIsland*>::const_iterator next = end_musc;
  std::vector<TPulseIsland*>::const_iterator prev = beg_musc;


  //************************************//
  //***** First get average offset *****//
  //************************************//
  TH1I hTOff("hTOff", "Time Offset", 400, -200., 200.);
  ges = beg_ges;
  gef = beg_gef;
  while (ges != end_ges && gef != end_gef) {
    static const double unfound = 1e9;
    double ge_time = GetGeTime(*ges, *gef);
    double dt[2] = {unfound, unfound}, &dt_prev = dt[0], &dt_next = dt[1];
    next = std::upper_bound(prev, end_musc, *gef, TPITimeLess);
    prev = next - 1;
    if (next == end_musc)
      dt_prev = dt_next = ge_time - GetMuScTime(*prev);
    else if (next == beg_musc) {
      dt_prev = dt_next = ge_time - GetMuScTime(*next);
      prev = next;
    } else {
      dt_prev = ge_time - GetMuScTime(*prev);
      dt_next = ge_time - GetMuScTime(*next);
    }
    
    for (unsigned int it = 0; it < 2; ++it)
      if (std::abs(dt[it]) < time_window_small)
	hTOff.Fill(dt[it]);

    ++ges; ++gef;
    // Sanity check
    if ( (ges == end_ges) != (gef == end_gef) ) {
      static const std::string msg("Fast and slow timestamps don't match up when getting time");
      throw std::logic_error(msg);
    }
  }
  double tOff = 0.;
  if (hTOff.Integral() > 1)
    tOff = hTOff.GetMean();
    
  fHist_MeanTOffset->Fill(tOff);

  //**************************//
  //***** Now make plots *****//
  //**************************//
  prev = beg_musc;
  ges = beg_ges - 1;
  gef = beg_gef - 1;
  while (++ges != end_ges && ++gef != end_gef) {
    static const double unfound = 1e9;
    double ge_time = GetGeTime(*ges, *gef) - tOff;
    double ge_energy = GetGeEnergy(*ges);
    double dt_prev = unfound, dt_next = unfound;

    // Find the time difference with the most recent muon
    // in any direction.
    next = std::upper_bound(prev, end_musc, *gef, TPITimeLess);
    prev = next - 1;
    unsigned int l = 0;
    if (next == end_musc) {
      dt_prev = ge_time - GetMuScTime(*prev);
      l = 1;
    } else if (next == beg_musc) {
      dt_next = ge_time - GetMuScTime(*next);
      prev = next;
      l = 2;
    } else {
      dt_prev = ge_time - GetMuScTime(*prev);
      dt_next = ge_time - GetMuScTime(*next);
      l = 3;
    }


    if ((dt_prev == unfound || dt_prev > time_window_big) && (dt_next == unfound || dt_next < -time_window_big))
      fHist_EnergyFarOOT->Fill(ge_energy);
    else if ((dt_prev == unfound || dt_prev > time_window_small) && (dt_next == unfound || dt_next < -time_window_small))
      fHist_EnergyOOT->Fill(ge_energy);
    else {
      if (dt_prev != unfound && dt_prev < time_window_small)
	fHist_TimeEnergy->Fill(dt_prev, ge_energy);
      if (dt_next != unfound && dt_next > -time_window_small)
	fHist_TimeEnergy->Fill(dt_next, ge_energy);
    }

    fHist_Energy->Fill(ge_energy);

    // Silicon XRay
    //if (ge_energy >= 3260 && energy_ge <= 3290)
    // Aluminium XRay
    if (ge_energy >= 2813 && ge_energy <= 2853) {
      if (dt_prev != unfound) {
	fHist_Time->Fill(dt_prev);
	fHist_MoreTime->Fill(dt_prev);
      }
      if (dt_next != unfound) {
	  fHist_Time->Fill(dt_next);
	  fHist_MoreTime->Fill(dt_next);
      }
    }
  }
  
  return 0;
}

// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int GeSpectrumWithTFix::AfterLastEntry(TGlobalData* gData,TSetupData *setup){
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
  while (m != b && (pol > 0 ? *--m > (int)cf : *--m < (int)cf));
  double dx = (double)(m-b);
  if (*(m+1) != *m)
    dx += (double)((int)cf - *m)/(double)(*(m+1) - *m);
  return (dx + (double)tpi->GetTimeStamp()) * tpi->GetClockTickInNs() - TSetupData::Instance()->GetTimeShift(tpi->GetBankName());
}

double GetPeakTime(const TPulseIsland* tpi, const int pol) {
  const std::vector<int>& samps = tpi->GetSamples();
  const std::vector<int>::const_iterator b = samps.begin(), e = samps.end();
  const int p = pol > 0 ? std::max_element(b, e) - b : std::min_element(b, e) - b;
  return (double)(p + tpi->GetTimeStamp()) * tpi->GetClockTickInNs() - TSetupData::Instance()->GetTimeShift(tpi->GetBankName());
}

double GetStampTime(const TPulseIsland* tpi, const int pol) {
  return (double)tpi->GetTimeStamp() * tpi->GetClockTickInNs() - TSetupData::Instance()->GetTimeShift(tpi->GetBankName());
}

double GetGeEnergy(const TPulseIsland* tpi) {
  const std::vector<int>& samps = tpi->GetSamples();
  const std::vector<int>::const_iterator b = samps.begin(), e = samps.end();
  const int ped = std::accumulate(b, b + 5, 0)/5.;
  const int max = *std::max_element(b, e);
  return (double)(max - ped);
}

bool TPITimeLess(const TPulseIsland* tpi1, const TPulseIsland* tpi2) {
  static const int pol = -1;
  return GetTime(tpi1, pol) < GetTime(tpi2, pol);
}


// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(GeSpectrumWithTFix);
