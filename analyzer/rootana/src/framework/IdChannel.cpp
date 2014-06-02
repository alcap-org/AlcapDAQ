#include "IdChannel.h"

ClassImp(IDs::channel);

std::map<std::string, IDs::Detector_t> IDs::channel::fStringToDetector;
std::map<IDs::Detector_t, std::string> IDs::channel::fDetectorToStrings;

void IDs::channel::InitialiseNames(){
  if(fStringToDetector.empty()){ 
     const char* names[]={
		"Ge", "LiquidSc", "NDet", "NDet2", "ScGe", "ScL", "ScR", 
		"ScVe", "SiL1_1", "SiL1_2", "SiL1_3", "SiL1_4", "SiL2", 
		"SiR1_1", "SiR1_2", "SiR1_3", "SiR1_4", "SiR1_sum", "SiR2", 
		"MuSc", "MuScA" };
     for (int i=0;i<NUM_ALL_DETECTORS;i++){
        NameDetector(names[i],(Detector_t)(i+1));
     }
  }
}

std::string IDs::channel::str()const{
 std::string output;
 output=fDetectorToStrings[this->fDetector];
 switch(fSlowFast){
   case kAnySlowFast:
   case kNotApplicable:
  	 break;
   case kSlow:
  	 output+="_S";
  	 break;
   case kFast:
  	 output+="_F";
  	 break;
 }
 return output;
}

std::string IDs::channel::GetDetectorName(Detector_t det){
	if(fDetectorToStrings.empty()) InitialiseNames();
	std::map<Detector_t,std::string>::iterator it=fDetectorToStrings.find(det);
	if(it==fDetectorToStrings.end()) return "";
	return it->second;
}

