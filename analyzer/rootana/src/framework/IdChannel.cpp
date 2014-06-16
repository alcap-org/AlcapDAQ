#include "IdChannel.h"
#include <ostream>

ClassImp(IDs::channel);

std::string IDs::channel::str()const{
   std::string output=GetDetectorString(this->fDetector);
   output+=GetSlowFastString(this->fSlowFast);
   return output;
}

std::string IDs::channel::GetDetectorString(Detector_t det){
	std::string output;
	switch(det){
            case kErrorDetector : output="Unknown"; break ; 
            case kAnyDetector : output="*"        ; break ; 
            case kGe          : output="Ge"       ; break ; 
            case kLiquidSc    : output="LiquidSc" ; break ; 
            case kNDet        : output="NDet"     ; break ; 
            case kNDet2       : output="NDet2"    ; break ; 
            case kScGe        : output="ScGe"     ; break ; 
            case kScL         : output="ScL"      ; break ; 
            case kScR         : output="ScR"      ; break ; 
            case kScVe        : output="ScVe"     ; break ; 
            case kSiL1_1      : output="SiL1_1"   ; break ; 
            case kSiL1_2      : output="SiL1_2"   ; break ; 
            case kSiL1_3      : output="SiL1_3"   ; break ; 
            case kSiL1_4      : output="SiL1_4"   ; break ; 
            case kSiL2        : output="SiL2"     ; break ; 
            case kSiR1_1      : output="SiR1_1"   ; break ; 
            case kSiR1_2      : output="SiR1_2"   ; break ; 
            case kSiR1_3      : output="SiR1_3"   ; break ; 
            case kSiR1_4      : output="SiR1_4"   ; break ; 
            case kSiR1_sum    : output="SiR1_sum" ; break ; 
            case kSiR2        : output="SiR2"     ; break ; 
            case kMuSc        : output="MuSc"     ; break ; 
            case kMuScA       : output="MuScA"    ; break ; 
	}
     return output;
}

IDs::Detector_t IDs::channel::GetDetectorEnum(const std::string& det){
     const char* names[IDs::num_detector_enums]={ 
	     "*", "Ge", "LiquidSc", "NDet", "NDet2", "ScGe", "ScL", "ScR", 
		"ScVe", "SiL1_1", "SiL1_2", "SiL1_3", "SiL1_4", "SiL2", 
		"SiR1_1", "SiR1_2", "SiR1_3", "SiR1_4", "SiR1_sum", "SiR2", 
		"MuSc", "MuScA" };
     for (int i=0;i<IDs::num_detector_enums;i++){
        if(det==names[i]) return (Detector_t)i;
     } 
     return kErrorDetector;
}

IDs::channel& IDs::channel::operator=(const std::string& rhs){
   // Search for a fast slow string at the end of the end of the string
   const char* fast_slow_strs[3]={"-*","-S","-F"};
   size_t match=std::string::npos;
   for(int i=0;i<3 && match==std::string::npos; i++){
   	match=rhs.rfind(fast_slow_strs[i]);
   }
   // Have we found a map
   if(match!=std::string::npos){
	   fSlowFast=GetSlowFastEnum(rhs.substr(match));
   }
   // Use what's left to make the channel part
   fDetector=GetDetectorEnum(rhs.substr(0,match));

   return *this;
}

std::string IDs::channel::GetSlowFastString(SlowFast_t sf){
 std::string output;
 switch(sf){
   case kAnySlowFast   : output+="-*" ; break ; 
   case kSlow          : output+="-S" ; break ; 
   case kFast          : output+="-F" ; break ; 
   case kNotApplicable : case kErrorSlowFast: break ; 
 }
 return output;
}

IDs::SlowFast_t IDs::channel::GetSlowFastEnum(const std::string& type){
	if(type=="-S"|| type=="slow") return kSlow;
	else if(type=="-F"|| type=="fast") return kFast;
	else if(type=="*"|| type=="any") return kAnySlowFast;
	return kNotApplicable;
}

ostream& operator<< (ostream& os , IDs::channel& id){
  os<<id.str();
  return os;
}
