#include "FastSlowCompare.h"
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
#include <map>
#include <string>

static std::vector<double> TPI2Times(const std::vector<TPulseIsland*>&);
static double GetTime(const TPulseIsland*, const int);
static bool isValid(const TPulseIsland*);

FastSlowCompare::FastSlowCompare(modules::options* opts): BaseModule("FastSlowCompare",opts) {
  const static std::string title_time("hTime");
  const static std::string title_moretime("hMoreTime");
  const static std::string title_meantime("hMeanTime");
  const static std::string title_nperwide("hNPerSlowWide");
  const static std::string title_npertight("hNPerSlowTight");
  fPairs[std::string("SiR2-S")] = std::string("SiR2-F");
  fPairs[std::string("SiL2-S")] = std::string("SiL2-F");
  fPairs[std::string("SiR1-1-S")] = std::string("SiR1-1-F");
  fPairs[std::string("SiR1-2-S")] = std::string("SiR1-2-F");
  fPairs[std::string("SiR1-3-S")] = std::string("SiR1-3-F");
  fPairs[std::string("SiR1-4-S")] = std::string("SiR1-4-F");
  fPairs[std::string("SiL1-1-S")] = std::string("SiL1-1-F");
  fPairs[std::string("SiL1-2-S")] = std::string("SiL1-2-F");
  fPairs[std::string("SiL1-3-S")] = std::string("SiL1-3-F");
  fPairs[std::string("SiL1-4-S")] = std::string("SiL1-4-F");
  fPairs[std::string("Ge-S")] = std::string("Ge-F");

  TDirectory* par_dir = TDirectory::CurrentDirectory();
  dir->cd();
  std::map<std::string, std::string>::iterator iPair;
  for (iPair = fPairs.begin(); iPair != fPairs.end(); ++iPair) {
    fHist_Time[iPair->first] = new TH1I((title_time + iPair->first).c_str(), "Relative Time of Fast", 400, -200., 200.);
    fHist_MoreTime[iPair->first] = new TH1I((title_moretime + iPair->first).c_str(), "Relative Time of Fast", 1000, -100000., 100000.);
    fHist_NPerSlowWide[iPair->first] = new TH1I((title_nperwide + iPair->first).c_str(), "Number of Fast Pulses within 100us", 10, -0.5, 9.5);
    fHist_NPerSlowTight[iPair->first] = new TH1I((title_npertight + iPair->first).c_str(), "Number of Fast Pulses within 400ns", 10, -0.5, 9.5);
  }
  par_dir->cd();
}

FastSlowCompare::~FastSlowCompare(){
}

// Called before the main event loop
// Can be used to set things up, like histograms etc
// Return non-zero to indicate a problem
int FastSlowCompare::BeforeFirstEntry(TGlobalData* gData, TSetupData *setup){
  return 0;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int FastSlowCompare::ProcessEntry(TGlobalData* gData, const TSetupData *setup){
  const std::map< std::string, std::vector<TPulseIsland*> >& TPIMap = gData->fPulseIslandToChannelMap;
  
  std::map<std::string, std::string>::iterator iPair;
  for (iPair = fPairs.begin(); iPair != fPairs.end(); ++iPair) {
    const std::vector<double> slow_times = TPI2Times(TPIMap.at(setup->GetBankName(iPair->first)));
    const std::vector<double> fast_times = TPI2Times(TPIMap.at(setup->GetBankName(iPair->second)));
    const std::vector<double>::const_iterator beg_slow = slow_times.begin();
    const std::vector<double>::const_iterator beg_fast  = fast_times.begin();
    const std::vector<double>::const_iterator end_slow = slow_times.end();
    const std::vector<double>::const_iterator end_fast  = fast_times.end();
    std::vector<double>::const_iterator slow;
    std::vector<double>::const_iterator fast[2];

    for (slow = beg_slow; slow < end_slow; ++slow) {
      static std::vector<double>::const_iterator iTime;
      fast[0] = std::upper_bound(beg_fast, end_fast, *slow + 100000.);
      fast[1] = std::upper_bound(beg_fast, end_fast, *slow - 100000.);
      fHist_NPerSlowWide[iPair->first]->Fill(fast[0] - fast[1]);
      for(iTime = fast[1]; iTime < fast[0]; ++iTime) {
	fHist_Time.at(iPair->first)->Fill(*iTime - *slow);
	fHist_MoreTime.at(iPair->first)->Fill(*iTime - *slow);
      }
      fast[0] = std::lower_bound(beg_fast, end_fast, *slow + 400.);
      fast[1] = std::upper_bound(beg_fast, end_fast, *slow - 400.);
      fHist_NPerSlowTight[iPair->first]->Fill(fast[0] - fast[1]);
    }
  }


  return 0;
}


// Called just after the main event loop
// Can be used to write things out, dump a summary etc
// Return non-zero to indicate a problem
int FastSlowCompare::AfterLastEntry(TGlobalData* gData,TSetupData *setup) {
  return 0;
}

/* Constant Fraction */
/**/
double GetTime(const TPulseIsland* tpi, const int pol) {
  const static double cf_frac = 0.2;
  const std::vector<int>& samps = tpi->GetSamples();
  const std::vector<int>::const_iterator b = samps.begin(), e = samps.end();
  const unsigned int ped = std::accumulate(b, b + 5, 0)/5.;
  std::vector<int>::const_iterator m = pol > 0 ? std::max_element(b, e) : std::min_element(b, e);
  const unsigned int amp = *m;
  const unsigned int cf = pol > 0 ? (unsigned int)(cf_frac*(double)(amp-ped)) + ped : (unsigned int)((double)(ped-amp)*(1.-cf_frac) + amp);
  while (m != b && (pol > 0 ? *--m > (int)cf : *--m < (int)cf));
  //m = pol > 0 std::upper_bound(b, m, cf, ) : ;
  double dx = (double)(m-b);
  if (*(m+1) != *m)
    dx += (double)((int)cf - *m)/(double)(*(m+1) - *m);
  return (dx + (double)tpi->GetTimeStamp()) * tpi->GetClockTickInNs() - TSetupData::Instance()->GetTimeShift(tpi->GetBankName());
}
//*/

/* Peak Time */
/*
double GetTime(const TPulseIsland* tpi, const int pol) {
  const std::vector<int>& samps = tpi->GetSamples();
  const std::vector<int>::const_iterator b = samps.begin(), e = samps.end();
  const int p = pol > 0 ? std::max_element(b, e) - b : std::min_element(b, e) - b;
  return (double)(p + tpi->GetTimeStamp()) * tpi->GetClockTickInNs() - TSetupData::Instance()->GetTimeShift(tpi->GetBankName());
}
//*/

/* Time Stamp */
/*
double GetTime(const TPulseIsland* tpi, const int pol) {
  return (double)tpi->GetTimeStamp() * tpi->GetClockTickInNs() - TSetupData::Instance()->GetTimeShift(tpi->GetBankName());
}
//*/

std::vector<double> TPI2Times(const std::vector<TPulseIsland*>& vec) {
  static std::vector<TPulseIsland*>::const_iterator tpi;
  std::vector<double> times;
  for (tpi = vec.begin(); tpi < vec.end(); ++tpi)
    if (isValid(*tpi))
      times.push_back(GetTime(*tpi, (*tpi)->GetTriggerPolarity()));
  return times;
}

bool isValid (const TPulseIsland* tpi) {
  const std::vector<int>& samps = tpi->GetSamples();
  const int min = 0;
  const int max = (int)std::pow(2., (double)TSetupData::Instance()->GetNBits(tpi->GetBankName())) - 1;
  if (*std::min_element(samps.begin(), samps.end()) <= min ||
      *std::max_element(samps.begin(), samps.end()) >= max)
    return false;
  return true;
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(FastSlowCompare);
