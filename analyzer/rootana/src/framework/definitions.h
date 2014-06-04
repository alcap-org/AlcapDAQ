#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include <vector>
#include "IdSource.h"

// Typedefs for TPulseIslands	
class TPulseIsland;
typedef std::vector<TPulseIsland*> PulseIslandList;
typedef std::map<IDs::channel, PulseIslandList> ChannelPulseIslandMap;
typedef std::map<std::string, PulseIslandList> StringPulseIslandMap;

// Typedefs for TAnalysedPulses	
class TAnalysedPulse;
typedef std::vector<TAnalysedPulse*> AnalysedPulseList;
typedef std::map<IDs::source, AnalysedPulseList> SourceAnalPulseMap;
typedef std::map<std::string, AnalysedPulseList> StringAnalPulseMap;

// Typedefs for TDetectorPulse	
class TDetectorPulse;
typedef std::vector<TDetectorPulse*> DetectorPulseList;
typedef std::map<IDs::source, DetectorPulseList> SourceDetPulseMap;
typedef std::map<std::string, DetectorPulseList> StringDetPulseMap;

// Typedefs for TMuonEvents	
class TMuonEvent;
typedef std::vector<TMuonEvent*> MuonEventList;

#endif // DEFINITIONS_H_
