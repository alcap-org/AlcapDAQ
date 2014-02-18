#include "MakeAnalysedPulses.h"
#include "TVAnalysedPulseGenerator.h"
#include "MaxBinAPGenerator.h"
#include <iostream>
#include <utility>

using std::cout;
using std::endl;
using std::string;

MakeAnalysedPulses::MakeAnalysedPulses(
    char *HistogramDirectoryName,const char* fastGen, const char* slowGen):
    FillHistBase(HistogramDirectoryName),
    fSlowGenerator(NULL),
    fFastGenerator(NULL),
    fSlowGeneratorType(slowGen),
    fFastGeneratorType(fastGen){
	dir->cd("/");
}

MakeAnalysedPulses::~MakeAnalysedPulses(){
    if(fFastGenerator) delete fFastGenerator;
    if(fSlowGenerator) delete fSlowGenerator;
}

int MakeAnalysedPulses::BeforeFirstEntry(TGlobalData* gData){
    if(fSlowGeneratorType=="") fSlowGeneratorType=fFastGeneratorType;
    fFastGenerator = MakeGenerator(fFastGeneratorType);
    fSlowGenerator = MakeGenerator(fSlowGeneratorType);

    // check we have a genarator for both fast and slow pulses
    if (fFastGenerator && fSlowGenerator) return 0;
    return 1;
}

int MakeAnalysedPulses::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
    // Generator just receives a bunch of TPIs and must return a list of TAPs

  // Loop over each detector
  string bankname, detname;
  PulseIslandList_t thePulseIslands;
  BankPulseList_t::const_iterator it;
  AnalysedPulseList_t theAnalysedPulses;
  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); it++){
    bankname = it->first;
    detname = gSetup->GetDetectorName(bankname);
    thePulseIslands = it->second;

    if (thePulseIslands.size() == 0) continue; // no pulses here...

    theAnalysedPulses.clear();

    // If this is a detector's fast channel use fFastGenerator
    if ( TSetupData::IsFast(detname) ) {
       fFastGenerator->ProcessPulses( gSetup, thePulseIslands,theAnalysedPulses);
    } else {// Else it is a slow pulse or not labelled, so use fSlowGenerator
       fSlowGenerator->ProcessPulses( gSetup, thePulseIslands,theAnalysedPulses);
    }

    fAnalysedPulseMap->insert(std::make_pair(detname,theAnalysedPulses));
  }
  return 0;
}

TVAnalysedPulseGenerator* MakeAnalysedPulses::MakeGenerator(const string& generatorType){

    // Select the generator type
    TVAnalysedPulseGenerator* generator=NULL;
    // As we develop newer techniques we can add to the list here
    if (generatorType == "MaxBin"){
	generator = new MaxBinAPGenerator();
    } else if( generatorType == "PeakFitter") {
    } else {
	cout<<"Unknown generator requested: "<<generatorType<<endl;	
	return NULL;
    }
    return generator;
}
