#include "AlcapExcept.h"
#include "ModulesParser.h"
#include "IdChannel.h"
#include <iostream>
#include <ostream>
#include <algorithm>

MAKE_EXCEPTION(InvalidDetector,Base)

ClassImp(IDs::channel);

std::string IDs::channel::str()const{
   std::string output=GetDetectorString(this->fDetector);
   output+=GetSlowFastString(this->fSlowFast);
   return output;
}

std::string IDs::channel::GetDetectorString(Detector_t det){
	std::string output;
	switch(det){
            case kErrorDetector  : output="Unknown"        ; break ; 
            case kAnyDetector    : output="*"              ; break ; 
            case kGe             : output="Ge"             ; break ; 
            case kLiquidSc       : output="LiquidSc"       ; break ; 
            case kNDet           : output="NDet"           ; break ; 
            case kNDet2          : output="NDet2"          ; break ; 
            case kScGe           : output="ScGe"           ; break ; 
            case kScL            : output="ScL"            ; break ; 
            case kScR            : output="ScR"            ; break ; 
            case kScVe           : output="ScVe"           ; break ; 
            case kSiL1_1         : output="SiL1-1"         ; break ; 
            case kSiL1_2         : output="SiL1-2"         ; break ; 
            case kSiL1_3         : output="SiL1-3"         ; break ; 
            case kSiL1_4         : output="SiL1-4"         ; break ; 
            case kSiL2           : output="SiL2"           ; break ; 
            case kSiR1_1         : output="SiR1-1"         ; break ; 
            case kSiR1_2         : output="SiR1-2"         ; break ; 
            case kSiR1_3         : output="SiR1-3"         ; break ; 
            case kSiR1_4         : output="SiR1-4"         ; break ; 
            case kSiR1_sum       : output="SiR1-sum"       ; break ; 
            case kSiR2           : output="SiR2"           ; break ; 
            case kMuSc           : output="muSc"           ; break ; 
            case kMuScA          : output="muScA"          ; break ; 
	// Add
	case kTRollover      : output="TRollover"      ; break ;
	case kTSync          : output="TSync"          ; break ;
	case kTMuSc_Hi       : output="TMuSc-Hi"       ; break ;
	case kTMuSc_Lo       : output="TMuSc-Lo"       ; break ;
	case kTGe            : output="TGe"            ; break ;
	case kTScVe          : output="TScVe"          ; break ;
	case kTProtonsOver10 : output="TProtonsOver10" ; break ;
	case kMuScan         : output="muScan"         ; break ;
	case kSiL1_A         : output="SiL1-A"         ; break ;
	case kSiL1_B         : output="SiL1-B"         ; break ;
	case kSiT_1          : output="SiT-1"          ; break ;
	case kSiT_2          : output="SiT-2"          ; break ;
	case kSiT_3          : output="SiT-3"          ; break ;
	case kSiT_4          : output="SiT-4"          ; break ;
	case kSiL3           : output="SiL3"           ; break ;
	case kSiR3           : output="SiR3"           ; break ;
	case kGeLoGain       : output="GeLoGain"       ; break ;
	case kGeHiGain       : output="GeHiGain"       ; break ;
	case kSiL1_5         : output="SiL1-5"         ; break ;
	case kSiL1_6         : output="SiL1-6"         ; break ;
	case kSiL1_7         : output="SiL1-7"         ; break ;
	case kSiL1_8         : output="SiL1-8"         ; break ;
	case kSiL1_9         : output="SiL1-9"         ; break ;
	case kSiL1_10        : output="SiL1-10"         ; break ;
	case kSiL1_11        : output="SiL1-11"         ; break ;
	case kSiL1_12        : output="SiL1-12"         ; break ;
	case kSiL1_13        : output="SiL1-13"         ; break ;
	case kSiL1_14        : output="SiL1-14"         ; break ;
	case kSiL1_15        : output="SiL1-15"         ; break ;
	case kSiL1_16        : output="SiL1-16"         ; break ;
	case kSync           : output="Sync"           ; break ;
	case kTMuScA         : output="TMuScA"           ; break ;
	}
     return output;
}

IDs::Detector_t IDs::channel::GetDetectorEnum(const std::string& det){
     const char* names[1+IDs::num_detector_enums]={ 
	     "*"        ,                                                   // 0
	     "Ge"       , "LiquidSc" , "NDet"          , "NDet2"   , "ScGe"    , // 1-5   
	     "ScL"      , "ScR"      , "ScVe"          , "SiL1-1"  , "SiL1-2"  , // 6-10  
	     "SiL1-3"   , "SiL1-4"   , "SiL2"          , "SiR1-1"  , "SiR1-2"  , // 11-15 
	     "SiR1-3"   , "SiR1-4"   , "SiR1-sum"      , "SiR2"    , "muSc"    , // 16-20 
	     "muScA"    , "TRollover", "TSync"         , "TMuSc_Hi", "TMuSc_Lo", // 21-25
	     "TGe"      , "TScVe"    , "TProtonsOver10", "muScan"  , "SiL1-A"  , // 26-30
	     "SiL1-B"   , "SiT-1"    , "SiT-2"         , "SiT-3"   , "SiT-4"   , // 31-35
	     "SiL3"     , "SiR3"     , "GeLoGain"      , "GeHiGain", "SiL1-5"  , // 36-40
	     "SiL1-6"   , "SiL1-7"   , "SiL1-8"        , "SiL1-9"  , "SiL1-10" , // 41-45
	     "SiL1-11"  , "SiL1-12"  , "SiL1-13"       , "SiL1-14" , "SiL1-15" , // 46-50
	     "SiL1-16"  , "Sync"     , "TMuScA" // 53
     };                                                 
     for (int i=0;i<=IDs::num_detector_enums;i++){
        if(modules::parser::iequals(det,names[i])) return (Detector_t)i;
     } 
     std::cout<<"Unknown detector name given to IDs::channel: '"<<det<<"'"<<std::endl;
     throw Except::InvalidDetector(det.c_str());
}

IDs::channel& IDs::channel::operator=(const std::string& rhs){
   // Search for a fast slow string at the end of the end of the string
   static const int num_strs=3;
   static const char fast_slow_strs[num_strs]={'*','F','S'};
   int i=0;
   size_t sz = rhs.size();
   for(i=0;i<num_strs ; i++){
       if(modules::parser::iequals(rhs[sz-1],fast_slow_strs[i])){
           if(modules::parser::iequals(rhs[sz-2],'-')){
               break;
           }
       }
   }
   // Have we found a map
   size_t boundary=std::string::npos;
   if(i< num_strs){
       boundary=sz-2;
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
	else if(type=="-*"|| type=="any") return kAnySlowFast;
	return kNotApplicable;
}

IDs::channel IDs::channel::GetCorrespondingFastSlow()const{
    switch(fSlowFast){
        case kFast: return IDs::channel(fDetector,kSlow); break;
        case kSlow: return IDs::channel(fDetector,kFast); break;
        case kNotApplicable: 
        case kAnySlowFast: 
        case kErrorSlowFast:return *this; break;
    }
    return *this;
}

std::ostream& operator<< (ostream& os ,const IDs::channel& id){
  os<<id.str();
  return os;
}

void IDs::channel::Debug()const{
    std::cout<<"Detector: "<<GetDetectorString(Detector())<<std::endl;
    std::cout<<"Slow/fast: "<<GetSlowFastString(SlowFast())<<std::endl;
}
