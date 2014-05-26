#include "TMuonEvent.h"
#include <iostream>
using std::cout;
using std::endl;

// This is a static member so we need to define it here
std::vector<std::string> TMuonEvent::fDetectorNames;

TMuonEvent::TMuonEvent(){
	// The first muon_event to be created will fill the vector of detector names
	if(fDetectorNames.size() ==0){
		const int num_det_names=21;
		const char* det_names[num_det_names]={ 
			"Ge", "LiquidSc", "NDet", "NDet2", "ScGe", "ScL", "ScR",
			"ScVe", "SiL1_1", "SiL1_2", "SiL1_3", "SiL1_4", "SiL2", "SiR1_1",
			"SiR1_2", "SiR1_3", "SiR1_4", "SiR1_sum", "SiR2", "MuSc", "MuScA" };
		for(int i=0;i<num_det_names;i++) fDetectorNames.push_back(det_names[i]);
	}
	ResetDetectors();
}

TMuonEvent::~TMuonEvent(){
}

void TMuonEvent::ResetDetectors(){
   // For now just NULL the pointers, but we need to decide an ownership scheme
   // in which case we may want to delete them as well
   for(size_t i=0;i<fDetectorNames.size();i++){
       SetPulse(fDetectorNames[i],NULL);
   }
}

int TMuonEvent::GetNumPulses()const{
	int count=0;
	for(size_t i=0;i<fDetectorNames.size();i++){
		if(GetPulse(fDetectorNames[i])) count++;
	}
	return count;
}

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
