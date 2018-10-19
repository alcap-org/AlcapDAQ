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
    case kErrorDetector  : return "Unknown";
    case kAnyDetector    : return "*";
    case kGeLoGain       : return "GeLoGain";
    case kGeHiGain       : return "GeHiGain";
    case kMuSc           : return "muSc";
    case kScGe           : return "ScGe";
    case kMuScan         : return "muScan";
    case kSiT_1          : return "SiT1-1";
    case kSiT_2          : return "SiT1-2";
    case kSiT_3          : return "SiT1-3";
    case kSiT_4          : return "SiT1-4";
    case kScVe           : return "ScVe";
    case kSiL1_1         : return "SiL1-1";
    case kSiL1_2         : return "SiL1-2";
    case kSiL1_3         : return "SiL1-3";
    case kSiL1_4         : return "SiL1-4";
    case kSiL1_5         : return "SiL1-5";
    case kSiL1_6         : return "SiL1-6";
    case kSiL1_7         : return "SiL1-7";
    case kSiL1_8         : return "SiL1-8";
    case kSiL1_9         : return "SiL1-9";
    case kSiL1_10        : return "SiL1-10";
    case kSiL1_11        : return "SiL1-11";
    case kSiL1_12        : return "SiL1-12";
    case kSiL1_13        : return "SiL1-13";
    case kSiL1_14        : return "SiL1-14";
    case kSiL1_15        : return "SiL1-15";
    case kSiL1_16        : return "SiL1-16";
    case kSiL1_A         : return "SiL1-A";
    case kSiL1_B         : return "SiL1-B";
    case kSiL2           : return "SiL2";
    case kSiL3           : return "SiL3";
    case kSiR1_1         : return "SiR1-1";
    case kSiR1_2         : return "SiR1-2";
    case kSiR1_3         : return "SiR1-3";
    case kSiR1_4         : return "SiR1-4";
    case kSiR2           : return "SiR2";
    case kSiR3           : return "SiR3";
    case kMuScA          : return "muScA";
    case kTMuSc_Lo       : return "TMuScLo";
    case kTMuSc_Hi       : return "TMuScHi";
    case kTMuScA         : return "TMuScA";
    case kTRollover      : return "TRollover";
    case kTSync          : return "TSync";
    case kSync           : return "Sync";
    case kTProtonsOver10 : return "TProtonsOver10";
    case kTScVe          : return "TScVe";
  }
  return "";
}

IDs::Detector_t IDs::channel::GetDetectorEnum(const std::string& det){
  const char* names[1+IDs::num_detector_enums]={
    "*",                                                          // 0
    "GeLoGain", "GeHiGain" ,                                      // 2
    "MuSc",     "ScGe",           "MuScan",                       // 5
    "SiT1-1",   "SiT1-2",         "SiT1-3",  "SiT1-4",            // 9
    "SiL2",     "SiL3",           "SiR2",    "SiR3",              // 13
    "SiR1-1",   "SiR1-2",         "SiR1-3",  "SiR1-4",            // 17
    "SiL1-1",   "SiL1-2",         "SiL1-3",  "SiL1-4",            // 21
    "SiL1_5",   "SiL1_6",         "SiL1_7",  "SiL1_8",            // 25
    "SiL1_9",   "SiL1_10",        "SiL1_11", "SiL1_12",           // 29
    "SiL1_13",  "SiL1_14",        "SiL1_15", "SiL1_16",           // 33
    "SiL1_A",   "SiL1_B",                                         // 35
    "ScVe",     "MuScA"                                           // 37
    "TMuScLo",  "TMuScHi",        "TMuScA", "TRollover", "TSync", // 42
    "Sync",     "TProtonsOver10", "TScVe"                         // 45
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
