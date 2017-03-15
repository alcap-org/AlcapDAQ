#include "TMuonEvent.h"
#include "AlcapExcept.h"
#include <iostream>
#include <iterator>
#include "debug_tools.h"
using std::cout;
using std::endl;

namespace {
    IDs::channel MuSc(IDs::kMuSc,IDs::kNotApplicable);
    IDs::channel MuScA(IDs::kMuScA,IDs::kNotApplicable);
  IDs::channel SiT_1(IDs::kSiT_1, IDs::kNotApplicable);
  IDs::channel SiT_2(IDs::kSiT_2, IDs::kNotApplicable);
  IDs::channel SiT_3(IDs::kSiT_3, IDs::kNotApplicable);
  IDs::channel SiT_4(IDs::kSiT_4, IDs::kNotApplicable);
}

MAKE_EXCEPTION(TMuonEvent,Base)
MAKE_EXCEPTION(OutOfRange,TMuonEvent)
MAKE_EXCEPTION(EndOfWindowBeforeStart,TMuonEvent)
MAKE_EXCEPTION(InvalidSource,TMuonEvent)

const TDetectorPulse* TMuonEvent::GetPulse(const IDs::source& source, int index)const{
    SourceDetPulseMap::const_iterator i_source=fPulseLists.find(source);
    if(i_source==fPulseLists.end()) return NULL;
    if(index>(int)i_source->second.size() || index<0) throw Except::OutOfRange();
    const TDetectorPulse* pulse= *(i_source->second.begin()+index);
    return pulse;
}

void TMuonEvent::AddPulse(const IDs::source& source, TDetectorPulse* pulse){
    fPulseLists[source].push_back(pulse);
}

void TMuonEvent::AddPulses(const IDs::source& source,
        DetectorPulseList::const_iterator start,
        DetectorPulseList::const_iterator stop){
    if(stop-start <0)  throw Except::EndOfWindowBeforeStart();
    for(DetectorPulseList::const_iterator i_p=start; i_p!=stop; ++i_p){
      if( (*i_p)->IsGood()) fPulseLists[source].push_back(*i_p);
    }
}

const IDs::source& TMuonEvent::GetSource(int n)const{
    if(n>(int)fPulseLists.size() || n<0) throw Except::OutOfRange();
    SourceDetPulseMap::const_iterator i_source=fPulseLists.begin();
    std::advance(i_source,n);
    return i_source->first;
}

int TMuonEvent::GetSourceIndex(const IDs::channel& ch, int start)const{
    if(start>(int)fPulseLists.size() || start<0) throw Except::OutOfRange();
    SourceDetPulseMap::const_iterator i_source=fPulseLists.begin();
    for(std::advance(i_source,start);
            i_source!=fPulseLists.end(); ++i_source){
        if(i_source->first.matches(ch)) 
          return std::distance( fPulseLists.begin(),i_source);
    }
    return -1;
}

int TMuonEvent::GetFirstSourceIndex(const IDs::channel& ch)const{
    for( SourceDetPulseMap::const_iterator i_source=fPulseLists.begin();
          i_source!=fPulseLists.end(); ++i_source){
        if(i_source->first.matches(ch)) 
          return std::distance( fPulseLists.begin(),i_source);
    }
    return -1;
}

int TMuonEvent::GetLastSourceIndex(const IDs::channel& ch)const{
    for( SourceDetPulseMap::const_reverse_iterator i_source=fPulseLists.rbegin();
          i_source!=fPulseLists.rend(); ++i_source){
        if(i_source->first.matches(ch)) 
          return std::distance( i_source,fPulseLists.rend());
    }
    return -1;
}



int TMuonEvent::TotalNumPulses()const{
    int size=0;
    for(SourceDetPulseMap::const_iterator i_source=fPulseLists.begin();
            i_source!=fPulseLists.end(); ++i_source){
        size+=i_source->second.size();
    }
    return size;
}

int TMuonEvent::NumPulses(const IDs::channel& channel)const{
    int size=0;
    for(SourceDetPulseMap::const_iterator i_source=fPulseLists.begin();
            i_source!=fPulseLists.end(); ++i_source){
        if(i_source->first==channel) size+=i_source->second.size();
    }
    return size;
}

int TMuonEvent::NumPulses(const IDs::source& source)const{
    SourceDetPulseMap::const_iterator i_source=fPulseLists.find(source);
    if(i_source==fPulseLists.end()) return 0;
    return i_source->second.size();
}

int TMuonEvent::NumPulses(const std::vector<IDs::channel>& channels)const {
    int size=0;
    // Loop through the given channels first
    for (std::vector<IDs::channel>::const_iterator i_channel = channels.begin(); i_channel != channels.end(); ++i_channel) {

      // Now loop through all the sources in the TME
      for(SourceDetPulseMap::const_iterator i_source=fPulseLists.begin(); i_source!=fPulseLists.end(); ++i_source) {
	// if the channel of this source matches the given channel
        if(i_source->first==(*i_channel)) {
	  size+=i_source->second.size(); // add to the number of pulses
	  break; // from this for loop
	}
      }
    }
    return size;  
}

bool TMuonEvent::HasMuonHit()const{
    DEBUG_PRINT("TMuonEvent::HasMuonHit() has not been updated to R15b yet");
    return true;
    //    return NumPulses(MuSc); // && NumPulses(MuPCX) && NumPulses(MuPCY);
}

bool TMuonEvent::HasMuonPileup()const{
  return (NumPulses(SiT_1)+NumPulses(SiT_2)+NumPulses(SiT_3)+NumPulses(SiT_4))>1 ;
}

bool TMuonEvent::WasEarlyInEvent()const{
    DEBUG_PRINT("TMuonEvent::WasEarlyInEvent is not yet implemented");
    return true;
}

bool TMuonEvent::WasLateInEvent(double event_length, double event_uncertainty)const{
    DEBUG_PRINT("TMuonEvent::WasLateInEvent is not yet implemented");
    return true;
}

DetectorPulseList::const_iterator TMuonEvent::BeginPulses(const IDs::source& detector)const{
    if(NumPulses(detector)==0) return DetectorPulseList::const_iterator();
    return fPulseLists.at(detector).begin();
}
DetectorPulseList::const_iterator TMuonEvent::EndPulses(const IDs::source& detector)const{
    if(NumPulses(detector)==0) return DetectorPulseList::const_iterator();
    return fPulseLists.at(detector).end();
}
