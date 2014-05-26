#include "TMuonEvent.h"

TMuonEvent::TMuonEvent(){
}

TMuonEvent::~TMuonEvent(){
        fDetectorPulses.clear();
}

bool TMuonEvent::HasPulseIn(std::string detector,const char& type='A')const{
       TEventList::const_iterator det=fDetectorPulses.find(detector);
       if(det==fDetectorPulses.end()) return false;
       switch (type){
               case 'F': case 'f':
                       return det->fast!=NULL;
                       break;
               case 'S': case 's':
                       return det->slow!=NULL;
                       break;
               case 'A': case 'a':
                       return true;
                       break;
               case 'B': case 'b':
                       return det->slow!=NULL && det->fast != NULL;
                       break;
               default:
                       cout<<"Unknown type of pulse requested"<<std::endl;
                       cout<<"Must be either fast ('F'), slow ('S'), any ('A') or both ('B')"<<std::endl;
       }
       return false;
}
