#include "PlotTPI_TimeBits.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"

#include "TH1.h"
#include "TH1D.h"

using std::cout;
using std::endl;


PlotTPI_TimeBits::PlotTPI_TimeBits(modules::options* opts) :
  BaseModule("PlotTPI_TimeBits",opts) {
}

PlotTPI_TimeBits::~PlotTPI_TimeBits() {
}

int PlotTPI_TimeBits::BeforeFirstEntry(TGlobalData* gData,
                                       const TSetupData* setup) {
  return 0;
}

void PlotTPI_TimeBits::BookHistogram(const std::string& bank) {
  std::string hname  = "fTimeBits_" + bank;
  std::string htitle = "Distributiom of Timestamp Bits for " + bank;
  TH1* h = new TH1D((hname+"_all").c_str(), htitle.c_str(), 32, -0.5, 31.5);
  h->GetXaxis()->SetTitle("Bit");
  fHists[0][bank] = h;
  h = new TH1D((hname+"_nosyncs").c_str(), htitle.c_str(), 32, -0.5, 31.5);
  h->GetXaxis()->SetTitle("Bit");
  fHists[1][bank] = h;
  h = new TH1D((hname+"_syncs").c_str(), htitle.c_str(), 32, -0.5, 31.5);
  h->GetXaxis()->SetTitle("Bit");
  fHists[2][bank] = h;
}

// Called once for each event in the main event loop
// Return non-zero to indicate a problem and terminate the event loop
int PlotTPI_TimeBits::ProcessEntry(TGlobalData* gData,const TSetupData *setup){

  // Prepare a few variables
  std::string bankname, detname;
  PulseIslandList thePulseIslands;

  // Loop over each detector
  for(StringPulseIslandMap::const_iterator it = gData->fPulseIslandToChannelMap.begin();
      it != gData->fPulseIslandToChannelMap.end();
      ++it) {

    const std::string&     bank = it->first;
    const PulseIslandList& tpis = it->second;
    if (tpis.size() == 0)        continue;
    if (fHists[0].count(bank) == 0) BookHistogram(bank);
    TH1* h[3] = { fHists[0][bank], fHists[1][bank], fHists[2][bank] };

    for (PulseIslandList::const_iterator tpi = tpis.begin();
         tpi != tpis.end();
         ++tpi) {
      int t = (*tpi)->GetTimeStamp();
      for (int b = 0; b < 32; ++b)
        if ((1 << b) & t)
          h[0]->Fill(b);
      if (10.e3 < (*tpi)->GetPulseTime() && (*tpi)->GetPulseTime() < 90.e6) {
        for (int b = 0; b < 32; ++b)
          if ((1 << b) & t)
            h[1]->Fill(b);
      } else {
        for (int b = 0; b < 32; ++b)
          if ((1 << b) & t)
            h[2]->Fill(b);
      }
    }
  }
  return 0;
}

int PlotTPI_TimeBits::AfterLastEntry(TGlobalData* gData,
                                     const TSetupData* setup){
  return 0;
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(PlotTPI_TimeBits);
