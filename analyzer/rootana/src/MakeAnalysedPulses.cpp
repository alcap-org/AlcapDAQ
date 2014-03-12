#include "MakeAnalysedPulses.h"

#include "TVAnalysedPulseGenerator.h"
#include "MaxBinAPGenerator.h"

#include "TVPulseFinder.h"
#include "FirstPulseFinder.h"

#include <iostream>
#include <sstream>
#include <cmath>
#include <utility>

using std::cout;
using std::endl;
using std::string;

extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;

MakeAnalysedPulses::MakeAnalysedPulses(
    char *HistogramDirectoryName,const char* fastGen, const char* slowGen):
    FillHistBase(HistogramDirectoryName),
    fSlowGenerator(NULL),
    fFastGenerator(NULL),
    fSlowGeneratorType(slowGen),
    fFastGeneratorType(fastGen), fPulseFinder(NULL){
	dir->cd("/");
}

MakeAnalysedPulses::MakeAnalysedPulses(modules::options* opts):
   FillHistBase("MakeAnalysedPulses"),
    fSlowGenerator(NULL),
   fFastGenerator(NULL){
	fSlowGeneratorType=opts->GetString("slow_gen");
	fFastGeneratorType=opts->GetString("fast_gen");
	if(fSlowGeneratorType==""){
	    fSlowGeneratorType=opts->GetString("0");
	}
	if(fFastGeneratorType==""){
	    fFastGeneratorType=opts->GetString("1");
	}

	fPulseFinderType=opts->GetString("pulse_finder");
	if (fPulseFinderType==""){
	  fPulseFinderType=opts->GetString("2");
	}

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

    fPulseFinder = MakeFinder(fPulseFinderType);

    // check we have a genarator for both fast and slow pulses
    if (fFastGenerator && fSlowGenerator && fPulseFinder) return 0;
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

    PulseIslandList_t theSubPulses = fPulseFinder->FindPulses(thePulseIslands);

    // If this is a detector's fast channel use fFastGenerator
    if ( TSetupData::IsFast(detname) ) {
       fFastGenerator->ProcessPulses( gSetup, thePulseIslands,theAnalysedPulses);
    } else {// Else it is a slow pulse or not labelled, so use fSlowGenerator
       fSlowGenerator->ProcessPulses( gSetup, thePulseIslands,theAnalysedPulses);
    }

    gAnalysedPulseMap.insert(std::make_pair(detname,theAnalysedPulses));
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

TVPulseFinder* MakeAnalysedPulses::MakeFinder(const string& finderType){

    // Select the finder type
    TVPulseFinder* finder=NULL;
    // As we develop newer techniques we can add to the list here
    if (finderType == "first"){
	finder = new FirstPulseFinder();
    } else if( finderType == "PeakFitter") {
    } else {
	cout<<"Unknown finder requested: "<<finderType<<endl;	
	return NULL;
    }
    return finder;
}

ALCAP_REGISTER_MODULE(MakeAnalysedPulses)
