#include "TMuonEvent.h"
#include <iostream>
using std::cout;
using std::endl;

TMuonEvent::TMuonEvent(){
	ResetDetectors();
}

TMuonEvent::~TMuonEvent(){
}

#define RESET_DET(detector)\
	Set##detector(NULL);

void TMuonEvent::ResetDetectors(){
	RESET_DET( Ge )
	RESET_DET( LiquidSc )
	RESET_DET( NDet )
	RESET_DET( NDet2 )
	RESET_DET( ScGe )
	RESET_DET( ScL )
	RESET_DET( ScR )
	RESET_DET( ScVe )
	RESET_DET( SiL1_1 )
	RESET_DET( SiL1_2 )
	RESET_DET( SiL1_3 )
	RESET_DET( SiL1_4 )
	RESET_DET( SiL2 )
	RESET_DET( SiR1_1 )
	RESET_DET( SiR1_2 )
	RESET_DET( SiR1_3 )
	RESET_DET( SiR1_4 )
	RESET_DET( SiR1_sum )
	RESET_DET( SiR2 )
	RESET_DET( MuSc )
	RESET_DET( MuScA )
}

#undef RESET_DET

#define GET_DET_IF(var,detector)\
	else if(var==#detector) return Get##detector();


TDetectorPulse* TMuonEvent::GetPulse(const std::string& detector)const{
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
	else if(var==#detector) Set##detector(pulse);

void TMuonEvent::SetPulse(const std::string& detector,TDetectorPulse* pulse){
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
