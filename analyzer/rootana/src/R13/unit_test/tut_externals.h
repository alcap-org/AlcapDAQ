#ifndef TUT_EXTERNALS_H
#define TUT_EXTERNALS_H

// define various extern symbols the way main.cpp does. <sigh>
#include "definitions.h"
#include "TAnalysedPulseMapWrapper.h"
#include "TTree.h"
class TGlobalData;
//class StringAnalPulseMap;

SourceAnalPulseMap gAnalysedPulseMap;
StringDetPulseMap gDetectorPulseMap;
MuonEventList gMuonEvents;
Long64_t* gTotalEntries;
Long64_t* gEntryNumber;
TTree *gAnalysedPulseTree = NULL;
TAnalysedPulseMapWrapper *gAnalysedPulseMapWrapper=NULL;

void ClearGlobalData(TGlobalData* data) { }

#endif //TUT_EXTERNALS_H
