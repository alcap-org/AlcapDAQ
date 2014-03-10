#include "MakeAnalysedPulses.h"
#include "TVAnalysedPulseGenerator.h"
#include "MaxBinAPGenerator.h"
#include <iostream>
#include <sstream>
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
    fFastGeneratorType(fastGen), fPulseCounter(0){
	dir->cd("/");
}

MakeAnalysedPulses::MakeAnalysedPulses(modules::options* opts):
   FillHistBase("MakeAnalysedPulses"),
    fSlowGenerator(NULL),
   fFastGenerator(NULL), fPulseCounter(0){
	fSlowGeneratorType=opts->GetString("slow_gen");
	fFastGeneratorType=opts->GetString("fast_gen");
	if(fSlowGeneratorType==""){
	    fSlowGeneratorType=opts->GetString("0");
	}
	if(fFastGeneratorType==""){
	    fFastGeneratorType=opts->GetString("1");
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

    PulseIslandList_t theSubPulses = FindPulses(thePulseIslands);

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

// FindPulses()
// -- Finds all pulses on an island and returns the vector of sub pulses
MakeAnalysedPulses::PulseIslandList_t MakeAnalysedPulses::FindPulses(PulseIslandList_t theIslands) {

  // Get the output ready
  PulseIslandList_t output;

  bool plot_pulses = true;

  for (PulseIslandList_t::iterator islandIter = theIslands.begin(); islandIter != theIslands.end(); ++islandIter) {

    // Things we need from the oldpulse island
    std::string bankname = (*islandIter)->GetBankName();
    std::vector<int> theSamples = (*islandIter)->GetSamples();
    int pedestal = TSetupData::Instance()->GetPedestal(bankname);
    int trigger_polarity = TSetupData::Instance()->GetTriggerPolarity(bankname);
    int RMS = 20; // hard-coded for the time being

    // Histograms
    TH1F* new_pulses = NULL;
    TH1F* old_pulse = NULL;
    ++fPulseCounter;
    
    if (plot_pulses) {
      std::stringstream histname;
      histname << bankname << "_SubPulses_" << fPulseCounter;
      new_pulses = new TH1F(histname.str().c_str(), histname.str().c_str(), theSamples.size(),0,theSamples.size());
    }
    
    if (plot_pulses) {
      std::stringstream histname;
      histname << bankname << "_OriginalIsland_" << fPulseCounter;
      old_pulse = new TH1F(histname.str().c_str(), histname.str().c_str(), theSamples.size(),0,theSamples.size());
    }
    
    // What we need for the new pulses
    int timestamp = 0;
    std::vector<int> newSamples;
    
    // Loop through the samples
    bool start_pulse = false;
    for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {
      
      // Get the current height (taking into account pulse polarity)
      int height = trigger_polarity*(*(sampleIter) - pedestal);

      if (plot_pulses)
	old_pulse->Fill(sampleIter - theSamples.begin(), *sampleIter);
      
      if (start_pulse == false) { // if we haven't found a pulse yet then see if we do
	if (height > 0 + RMS) {
	  timestamp = sampleIter - theSamples.begin();
	  start_pulse = true;
	}
      }
      
      if (start_pulse == true) {
	// see if the pulse has ended
	if (height < 0) {
	  start_pulse = false; // the pulse is over
	  
	  // Add the TPulseIsland
	  output.push_back(new TPulseIsland(timestamp, newSamples, bankname));
	  
	  // Clear the old information
	  newSamples.clear();
	  timestamp = 0;
	}
	else {
	  newSamples.push_back(*sampleIter);
	  if (plot_pulses)
	    new_pulses->Fill(sampleIter - theSamples.begin(), *sampleIter);
	}
      }
    }
  }

  return output;
}


ALCAP_REGISTER_MODULE(MakeAnalysedPulses)
