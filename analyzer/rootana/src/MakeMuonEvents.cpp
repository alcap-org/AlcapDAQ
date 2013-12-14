#define USE_PRINT_OUT 1

#include "MakeMuonEvents.h"
#include <TMuonEvent.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>

#include "TH2.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

const int NCHAN = 8;
static TH1 *hSamples1 = 0;
static TH1 *hSamples2 = 0;
static TH1 *hSamples3 = 0;
static TH2 *hBankSize = 0;
static TH1 *bankHists[NCHAN];

extern std::vector<TMuonEvent* > gMuonEvents;
extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;

MakeMuonEvents::MakeMuonEvents(char *HistogramDirectoryName,TSetupData* setup) :
  FillHistBase(HistogramDirectoryName,setup){
      // Get all detectors
      setup->GetAllDetectors(fDetectors);
      fDetectors.erase("muSc");

   std::map<std::string, std::string>::const_iterator it;
   for (it=fDetectors.begin();it!= fDetectors.end();it++){
                   std::cout<<it->first<<'\t'<<it->second<<std::endl;
                   }

}

MakeMuonEvents::~MakeMuonEvents(){
}

int MakeMuonEvents::ProcessEntry(TGlobalData *aData){
    typedef std::vector<TAnalysedPulse* > PulseList;
    typedef std::string DetectorName;

    // how far away can pulses be (in ns) to be considered coincident
    int coincidence_window=10;

    // Loop over each muSc pulse
    PulseList muSc_pulses=gAnalysedPulseMap["muSc"];
    PulseList::iterator i_muSc;
    double muSc_time;
    for(i_muSc=muSc_pulses.begin();i_muSc!=muSc_pulses.end();i_muSc++){
            // The arrival time of the muSc
            muSc_time=(*i_muSc)->GetTime();

            // Loop over each detector and look for pulses that are within the
            // coincidence window of the muSc


    }

  return 0;
}
