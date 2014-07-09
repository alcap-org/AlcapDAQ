#include "ModulesParser.h"
#include "IdChannel.h"
#include <iostream>
#include <ostream>
#include <algorithm>

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
     const char* names[1+IDs::num_detector_enums]={ 
	     "*"        ,                                                // 0
	     "Ge"       , "LiquidSc" , "NDet"     , "NDet2"  , "ScGe"   ,// 1-5   
	     "ScL"      , "ScR"      , "ScVe"     , "SiL1_1" , "SiL1_2" ,// 6-10  
	     "SiL1_3"   , "SiL1_4"   , "SiL2"     , "SiR1_1" , "SiR1_2" ,// 11-15 
	     "SiR1_3"   , "SiR1_4"   , "SiR1_sum" , "SiR2"   , "MuSc"   ,// 16-20 
	     "MuScA" };                                                  // 21
     for (int i=0;i<=IDs::num_detector_enums;i++){
        if(modules::parser::iequals(det,names[i])) return (Detector_t)i;
     } 
     return kErrorDetector;
}

IDs::channel& IDs::channel::operator=(const std::string& rhs){
   // Search for a fast slow string at the end of the end of the string
   static const int num_strs=3;
   static const std::string fast_slow_strs[num_strs]={"-*","-S","-F"};
   int i=0;
   std::string::const_iterator match;
   // Create a functino pointer to the iequals method
   bool (*compare) (const char a, const char b);
   compare = modules::parser::iequals;
   for(i=0;i<num_strs ; i++){
   	match=std::find_end(rhs.begin(),rhs.end(),
            fast_slow_strs[i].begin(),fast_slow_strs[i].end(),
            compare);
    if (match!=rhs.end()) break;
   }
   // Have we found a map
   size_t boundary=std::string::npos;
   if(i<= num_strs){
       boundary=match - rhs.begin();
   std::string fs=rhs.substr(boundary);
	   fSlowFast=GetSlowFastEnum(fs);
   }else fSlowFast=kNotApplicable;

   // Use what's left to make the channel part
   fDetector=GetDetectorEnum(rhs.substr(0,boundary));

   return *this;
}

std::string IDs::channel::GetSlowFastString(SlowFast_t sf){
 std::string output;
 switch(sf){
   case kAnySlowFast   : output+="-*" ; break ; 
   case kSlow          : output+="-S" ; break ; 
   case kFast          : output+="-F" ; break ; 
   case kNotApplicable : break;
   case kErrorSlowFast: output+="-Unknown" ;break ; 
 }
 return output;
}

IDs::SlowFast_t IDs::channel::GetSlowFastEnum(const std::string& type){
	if(modules::parser::iequals(type,"-S") || type=="slow") return kSlow;
	else if(modules::parser::iequals(type,"-F")|| type=="fast") return kFast;
	else if(type=="*"|| type=="any") return kAnySlowFast;
	return kNotApplicable;
}

IDs::channel IDs::channel::GetCorrespondingFastSlow()const{
    switch(fSlowFast){
        case kFast: return IDs::channel(fDetector,kSlow); break;
        case kSlow: return IDs::channel(fDetector,kFast); break;
        case kNotApplicable: 
        case kErrorSlowFast:return *this; break;
    }
}

std::ostream& operator<< (ostream& os ,const IDs::channel& id){
  os<<id.str();
  return os;
}

