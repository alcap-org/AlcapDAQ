#include "TMuonEvent.h"
#include <iostream>
using std::cout;
using std::endl;

TMuonEvent::TMuonEvent(){
}

TMuonEvent::~TMuonEvent(){
        fDetectorPulses.clear();
}

bool TMuonEvent::HasPulse(std::string detector,const char& type='A')const{
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

#define GET_DET_IF(var,detector)\
	else if(var==##detector) return Get#detector();


TDetectorEvent* TMuonEvent::GetPulse(std::string detector)const{
	if(detector=="") return NULL;
	GET_DET_IF(detector,Ge)
	GET_DET_IF(detector,LiquidSc)
	GET_DET_IF(detector,NDet)
	GET_DET_IF(detector,NDet2)
	GET_DET_IF(detector,ScGe)
	GET_DET_IF(detector,ScL)
	GET_DET_IF(detector,ScR)
	GET_DET_IF(detector,ScVe)
	GET_DET_IF(detector,SiL1_1)
	GET_DET_IF(detector,SiL1_2)
	GET_DET_IF(detector,SiL1_3)
	GET_DET_IF(detector,SiL1_4)
	GET_DET_IF(detector,SiL2)
	GET_DET_IF(detector,SiR1_1)
	GET_DET_IF(detector,SiR1_2)
	GET_DET_IF(detector,SiR1_3)
	GET_DET_IF(detector,SiR1_4)
	GET_DET_IF(detector,SiR1_sum)
	GET_DET_IF(detector,SiR2)
	GET_DET_IF(detector,MuSc)
	GET_DET_IF(detector,MuScA)
	// No detector exists with that name, complain and return NULL
	cout<<"No detector called '"<<detector<<"' exists."<<endl;
	return NULL;
}
#undef GET_DET_IF

#define SET_DET_IF(var,detector,value)\
	else if(var==##detector) Set#detector(pulse);
void TMuonEvent::SetPulse(std::string detector,TDetectorPulse* pulse){
	if(detector=="") return;
	SET_DET_IF(detector , Ge       , pulse)
	SET_DET_IF(detector , LiquidSc , pulse)
	SET_DET_IF(detector , NDet     , pulse)
	SET_DET_IF(detector , NDet2    , pulse)
	SET_DET_IF(detector , ScGe     , pulse)
	SET_DET_IF(detector , ScL      , pulse)
	SET_DET_IF(detector , ScR      , pulse)
	SET_DET_IF(detector , ScVe     , pulse)
	SET_DET_IF(detector , SiL1_1   , pulse)
	SET_DET_IF(detector , SiL1_2   , pulse)
	SET_DET_IF(detector , SiL1_3   , pulse)
	SET_DET_IF(detector , SiL1_4   , pulse)
	SET_DET_IF(detector , SiL2     , pulse)
	SET_DET_IF(detector , SiR1_1   , pulse)
	SET_DET_IF(detector , SiR1_2   , pulse)
	SET_DET_IF(detector , SiR1_3   , pulse)
	SET_DET_IF(detector , SiR1_4   , pulse)
	SET_DET_IF(detector , SiR1_sum , pulse)
	SET_DET_IF(detector , SiR2     , pulse)
	SET_DET_IF(detector , MuSc     , pulse)
	SET_DET_IF(detector , MuScA    , pulse)
	// No detector exists with that name, complain and return NULL
	cout<<"No detector called '"<<detector<<"' exists."<<endl;
	return;
}
#undef SET_DET_IF
