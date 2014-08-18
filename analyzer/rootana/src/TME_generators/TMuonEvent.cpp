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
}

MAKE_EXCEPTION(TMuonEvent,Base)
MAKE_EXCEPTION(OutOfRange,TMuonEvent)

const TDetectorPulse* TMuonEvent::GetPulse(const IDs::source& source, int index)const{
    SourceDetPulseMap::const_iterator i_source=fPulseLists.find(source);
    if(i_source==fPulseLists.end()) return NULL;
    if(index>(int)i_source->second.size() || index<0) throw Except::OutOfRange();
    return *(i_source->second.begin()+index);
}

void TMuonEvent::AddPulse(const IDs::source& source, TDetectorPulse* pulse){
    fPulseLists[source].push_back(pulse);
}

void TMuonEvent::AddPulses(const IDs::source& source,
        DetectorPulseList::const_iterator start,
        DetectorPulseList::const_iterator stop){
    fPulseLists[source].insert(fPulseLists[source].end(),start,stop);
}

const IDs::source& TMuonEvent::GetSource(int n)const{
    if(n>(int)fPulseLists.size() || n<0) throw Except::OutOfRange();
    SourceDetPulseMap::const_iterator i_source=fPulseLists.begin();
    std::advance(i_source,n);
    return i_source->first;
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

bool TMuonEvent::HasMuonHit()const{
    return NumPulses(MuSc); // && NumPulses(MuPCX) && NumPulses(MuPCY);
}

bool TMuonEvent::HasMuonPileup()const{
    return NumPulses(MuSc)>1 || NumPulses(MuScA);
}

bool TMuonEvent::WasEarlyInEvent()const{
    DEBUG_PRINT("TMuonEvent::WasEarlyInEvent is not yet implemented");
    return true;
}

bool TMuonEvent::WasLateInEvent(double event_length, double event_uncertainty)const{
    DEBUG_PRINT("TMuonEvent::WasLateInEvent is not yet implemented");
    return true;
}
