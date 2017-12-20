#include "TMEGeneratorFactory.h"
#include "FixedWindowMEGenerator.h"
#include "TDetectorPulse.h"
#include "TMuonEvent.h"
//#define ALCAP_NO_DEBUG
#include "debug_tools.h"

#include <iostream>
using std::cout;
using std::endl;

FixedWindowMEGenerator::FixedWindowMEGenerator(TMEGeneratorOptions* opts):
	TVMuonEventGenerator("FixedWindow",opts),fInit(true){
        fEventWindow=opts->GetDouble("event_window",1e4);
	fCentralMuonEnergyCut=opts->GetDouble("central_muon_energy_cut");
	opts->GetVectorStringsByDelimiter("muon_channels",fMuonChannels, " : ");
}

int FixedWindowMEGenerator::Init(const SourceDetPulseMap& detectorPulsesIn){

  // Define our muon counters
  std::vector<IDs::channel> muon_counters;
  for (std::vector<std::string>::const_iterator i_name = fMuonChannels.begin(); i_name != fMuonChannels.end(); ++i_name) {
    muon_counters.push_back(IDs::channel(*i_name));
  }

  // Loop through the "sources" (channel + generator(?)) and get the sources corresponding to our muon counters
  for(SourceDetPulseMap::const_iterator i_source=detectorPulsesIn.begin();
      i_source!=detectorPulsesIn.end();++i_source){

    bool is_muon_counter = false;
    for(std::vector<IDs::channel>::const_iterator i_muon_counter = muon_counters.begin(); i_muon_counter != muon_counters.end(); ++i_muon_counter) {
      if(i_source->first.matches(*i_muon_counter)){
	//	// if source is wildcard then select fMuSc 
	//	if(!fMuSc.source || fMuSc.source->isWildCardChannel()){
	//	  fMuSc=Detector_t(i_source);
	//	}else{
	//	  cout<<"FixedWindowMEGenerator: Error: Multiple muSc TDP sources found"<<endl;
	//	  return 1;
	//	}
	fMuonCounters.push_back(Detector_t(i_source));
	is_muon_counter = true;
      }
    }

    if (!is_muon_counter) {
	fOtherDetectors.push_back(Detector_t(i_source));
      }  
    }
  return 0;
}

void FixedWindowMEGenerator::Reset(){
    for(SourceList::iterator i_mu_counter=fMuonCounters.begin();
            i_mu_counter!=fMuonCounters.end(); ++i_mu_counter){
        i_mu_counter->Reset();
    }

    for(SourceList::iterator i_det=fOtherDetectors.begin();
            i_det!=fOtherDetectors.end(); ++i_det){
        i_det->Reset();
    }
}

int FixedWindowMEGenerator::ProcessPulses(MuonEventList& muonEventsOut,
        const SourceDetPulseMap& detectorPulsesIn){
    if(fInit) {
        int error=Init(detectorPulsesIn);
        if(error) return error;
        fInit=false;
    }else{
        Reset();
    }

    // Loop over all muon counters
    for(SourceList::const_iterator i_mu_counter=fMuonCounters.begin();
            i_mu_counter!=fMuonCounters.end(); ++i_mu_counter){

      // Loop over all the "muons" (actually pulses) in this muon counter pulse list
      for(DetectorPulseList::const_iterator i_muons=(*i_mu_counter).pulses->begin();
	  i_muons!=(*i_mu_counter).pulses->end(); ++i_muons){
	double muon_energy = (*i_muons)->GetEnergy();
	if (muon_energy < 0) {
	  // could be doing muSc which doesn't have a calibration
	  muon_energy = (*i_muons)->GetAmplitude();
	}
	if (muon_energy < fCentralMuonEnergyCut) {
	  continue;
	}

        // Make a TME centred on this muon
        TMuonEvent* tme=new TMuonEvent(*i_muons, fEventWindow);

        // Add all pulses from all muon counters in the event window to this TME
        for(SourceList::iterator i_other_mu_counter=fMuonCounters.begin();
                i_other_mu_counter!=fMuonCounters.end(); ++i_other_mu_counter){
	  AddPulsesInWindow(tme,fEventWindow,(*i_other_mu_counter));
	}

	// Add all pulses from all other detectors in the event window to this TME
        for(SourceList::iterator i_det=fOtherDetectors.begin();
                i_det!=fOtherDetectors.end(); ++i_det){

	  AddPulsesInWindow(tme,fEventWindow,*i_det);
        }
        //if(Debug()) cout<<"FixedWindowMEGenerator::ProcessPulses:"
        //    " Created TME with "<<tme->TotalNumPulses()<<" pulses" <<endl;

        // Add TME to list of event
        muonEventsOut.push_back(tme);
      }
    }
    return 0;
}

void FixedWindowMEGenerator::AddPulsesInWindow(TMuonEvent* tme, double window, Detector_t& detector){
    // Get the central time for this event;
    double central_time=tme->GetTime();
    double early_edge=central_time-window;
    double late_edge=central_time+window;

    // advance the start and stop iterators that define the range of pulses
    // contained in the window
    //    DetectorPulseList::const_iterator& start=detector.start_window;
    //    DetectorPulseList::const_iterator& stop=detector.end_window;
    DetectorPulseList::const_iterator start=detector.start_window;
    DetectorPulseList::const_iterator stop=detector.end_window;
    const DetectorPulseList::const_iterator& end=detector.pulses->end();
    //const DetectorPulseList::const_iterator& begin=detector.pulses->begin();

    // skip empty lists
    if(start==end) return;

    // Move the iterator for the start of the window
    DetectorPulseList::const_iterator i_tmp=start;
    while( (i_tmp !=end)){
        // skip over bad TDPs
        while( (i_tmp!=end) && !(*i_tmp)->IsGood() ) ++i_tmp;
        // if there are no TDPs left we can finish already
        if(i_tmp==end) return;
        double time=(*i_tmp)->GetTime(TDetectorPulse::kFast);
        if(time<early_edge) ++i_tmp;
        else break;
    }
    start=i_tmp;
    // Move the iteator for the end of the window
    while( (i_tmp !=end)){
        // skip over bad TDPs
        while( (i_tmp!=end) && !(*i_tmp)->IsGood() ) ++i_tmp;
        // if there are no TDPs left we can finish already
        if(i_tmp==end) break;
        double time=(*i_tmp)->GetTime(TDetectorPulse::kFast);
        if(time<late_edge) ++i_tmp;
        else break;
    }
    stop=i_tmp;

    // Add all pulses in the current window to the tme
    tme->AddPulses(*detector.source,start,stop);

    // Flag if we've used all of a channels pulses
    if(stop==detector.pulses->end()) tme->AllPulsesUsed(*detector.source);
}

ALCAP_TME_GENERATOR(FixedWindow, event_window);
