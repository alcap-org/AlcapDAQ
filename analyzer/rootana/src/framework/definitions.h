#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include <vector>
#include <map>
#include "IdSource.h"
#include <string>


typedef int TPulseIslandID;

// Typedefs for TPulseIslands	
class TPulseIsland;
typedef std::vector<TPulseIsland*> PulseIslandList;
typedef std::vector<const TPulseIsland*> ConstPulseIslandList;
typedef std::map<IDs::channel, PulseIslandList> ChannelPulseIslandMap;
typedef std::map<std::string, PulseIslandList> StringPulseIslandMap;
typedef std::map<std::string, ConstPulseIslandList> StringConstPulseIslandMap;

// Typedefs for TAnalysedPulses	
class TAnalysedPulse;
typedef std::vector<TAnalysedPulse*> AnalysedPulseList;
typedef std::vector<const TAnalysedPulse*> ConstAnalysedPulseList;
typedef std::map<IDs::source, AnalysedPulseList> SourceAnalPulseMap;
typedef std::map<std::string, AnalysedPulseList> StringAnalPulseMap;
typedef std::map<std::string, ConstAnalysedPulseList> StringConstAnalPulseMap;

// Typedefs for TDetectorPulse	
class TDetectorPulse;
typedef std::vector<TDetectorPulse*> DetectorPulseList;
typedef std::vector<const TDetectorPulse*> ConstDetectorPulseList;
typedef std::map<IDs::source, DetectorPulseList> SourceDetPulseMap;
typedef std::map<std::string, DetectorPulseList> StringDetPulseMap;
typedef std::map<std::string, ConstDetectorPulseList> StringConstDetPulseMap;

// Typedefs for TMuonEvents	
class TMuonEvent;
typedef std::vector<TMuonEvent*> MuonEventList;

namespace definitions{
    /// default value used by TAPs, TDPs and other algorithms to signify an empty
    /// field
    enum { DefaultValue=-99999 };
}

namespace Except{
    class Base;
}

namespace alcap{
   template < typename ExceptType, typename KeyType, typename ValueType>
       const ValueType& at(const std::map<KeyType,ValueType>& map, const KeyType& key, const char* msg=""){
           typedef std::map<KeyType,ValueType> MapType;
           typename MapType::const_iterator it=map.find(key);
           if(it==map.end()) throw ExceptType(msg);
           return it->second;
       }
}

#endif // DEFINITIONS_H_
