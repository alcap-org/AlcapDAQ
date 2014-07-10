#ifndef TUT_EXTERNALS_H
#define TUT_EXTERNALS_H

// define various extern symbols the way main.cpp does. <sigh>
#include "definitions.h"
//class StringAnalPulseMap;

SourceAnalPulseMap gAnalysedPulseMap;
StringDetPulseMap gDetectorPulseMap;
MuonEventList gMuonEvents;
Long64_t* gTotalEntries;
Long64_t* gEntryNumber;
//static TTree *gAnalysedPulseTree = NULL

#endif //TUT_EXTERNALS_H
