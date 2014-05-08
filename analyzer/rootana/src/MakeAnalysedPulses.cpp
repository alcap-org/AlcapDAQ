#include "ModulesManager.h"
#include "MakeAnalysedPulses.h"
#include "TVAnalysedPulseGenerator.h"
#include "MaxBinAPGenerator.h"
#include <iostream>
#include <utility>
#include <cmath>
#include "RegisterModule.inc"

using std::cout;
using std::endl;
using std::string;

extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;

MakeAnalysedPulses::MakeAnalysedPulses(modules::options* opts):
   FillHistBase("MakeAnalysedPulses",opts),fOptions(opts){
	fSlowGeneratorType=opts->GetString("default_slow_generator");
	fFastGeneratorType=opts->GetString("default_fast_generator");
	opts->GetVectorStrings("analyse_channels",fChannelsToAnalyse);
	dir->cd("/");
}

MakeAnalysedPulses::~MakeAnalysedPulses(){
}

int MakeAnalysedPulses::BeforeFirstEntry(TGlobalData* gData,TSetupData *setup){
    // Loop over every named detector channel in TSetupData
    std::vector<std::string> detectors;
    setup->GetAllDetectors(detectors);
    std::cout<<"Will analyse "<<fChannelsToAnalyse.size()<<" of "<<detectors.size()<<" channels"<<std::endl;

    // do we analyse all channels?
    bool analyse_all=fChannelsToAnalyse.size()==0;
    if(!analyse_all){
        for(unsigned i=0;i<fChannelsToAnalyse.size();i++){
	     if (fChannelsToAnalyse[i]=="all"){
		analyse_all=true;
		break;
	     }
	}
    }

    // For each channel:
    bool skip_detector=false;
    std::string des_gen;
    for(std::vector<std::string>::const_iterator det=detectors.begin();
		    det!=detectors.end(); det++){
       // if we should not analyse det, leave generator as NULL
       skip_detector=false;
       if(! analyse_all ){
        for(unsigned i=0; true;i++){
         if(i>=fChannelsToAnalyse.size()){
	   skip_detector=true;
           break;
         }
         if(fChannelsToAnalyse[i]== (*det)) break;
        }
       }
       if(skip_detector){
        fGenerators[*det]=NULL;
        continue;
       }
       // else find the name of the desired generator
       des_gen=fOptions->GetOption(*det);
       if(des_gen!=""){
         // If this channel is named explicitly, use that generator type
         fGenerators[*det]=MakeGenerator(des_gen);
         if(Debug()) std::cout<<*det<<": requested "<<des_gen<<std::endl;
       } else{
         // else use default value for this type of channel (fast or slow)
         if(TSetupData::IsFast(*det)){
            fGenerators[*det]=MakeGenerator(fFastGeneratorType);
            if(Debug()) std::cout<<*det<<": default fast generator: "<<fFastGeneratorType<<std::endl;
	 } else {
            fGenerators[*det]=MakeGenerator(fSlowGeneratorType);
            if(Debug()) std::cout<<*det<<": default slow generator: "<<fSlowGeneratorType<<std::endl;
	 }
       }
    }
    return 0;
}

int MakeAnalysedPulses::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
    // Generator just receives a bunch of TPIs and must return a list of TAPs

  // Loop over each detector
  string bankname, detname;
  PulseIslandList_t thePulseIslands;
  BankPulseList_t::const_iterator it;
  AnalysedPulseList_t theAnalysedPulses;
  TVAnalysedPulseGenerator* gen;
  for(it = gData->fPulseIslandToChannelMap.begin(); it != gData->fPulseIslandToChannelMap.end(); it++){
    bankname = it->first;
    detname = gSetup->GetDetectorName(bankname);

    // Get this channels generator
    gen=fGenerators.find(detname)->second;
    if(! gen) continue; // no generator for this channel, we don't want to analyse it

    // Get the TPIs
    thePulseIslands = it->second;
    if (thePulseIslands.size() == 0) continue; // no pulses here...

    // Remove all the TPIs that are just noise (e.g. from channels with a common trigger)
    PulseIslandList_t theTruePulseIslands = RemoveFalseTPIs(thePulseIslands);

    // clear the list of analyse_pulses from the last iteration
    theAnalysedPulses.clear();
    // generate the new list of analyse_pulses
    gen->ProcessPulses( gSetup, theTruePulseIslands,theAnalysedPulses);
    // add these into the map of analysed pulses
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
	throw "Unknown generator requested";
	return NULL;
    }
    return generator;
}

MakeAnalysedPulses::PulseIslandList_t MakeAnalysedPulses::RemoveFalseTPIs(PulseIslandList_t& theIslands) {

  // Get the output ready
  PulseIslandList_t output;

  // Loop through the TPIs
  for (PulseIslandList_t::iterator pulseIter = theIslands.begin(); pulseIter != theIslands.end(); ++pulseIter) {

    // Get the samples
    std::vector<int> theSamples = (*pulseIter)->GetSamples();

    // Get some useful TSetupData stuff
    std::string bankname = (*pulseIter)->GetBankName();
    int n_bits = TSetupData::Instance()->GetNBits(bankname);

    // Calculate the maximum digitised value
    int max_digitised_value = std::pow(2, n_bits) - 1;

    // First, check if there are any overflow samples
    bool overflow_sample = false;
    for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {

      if (*sampleIter >= max_digitised_value) {
	overflow_sample = true;
	break;
      }
    }

    if (overflow_sample == true) {
      continue;
    }


    // Second, check that there is a sample that goes significantly above the pedestal
    int pedestal = TSetupData::Instance()->GetPedestal(bankname);
    int trigger_polarity = TSetupData::Instance()->GetTriggerPolarity(bankname);
    int RMS = 10; // hard-coded for the time being
    int n_sigma = 2; // to be optimised

    bool significant_sample = false;

    for (std::vector<int>::iterator sampleIter = theSamples.begin(); sampleIter != theSamples.end(); ++sampleIter) {

      int test_value = trigger_polarity * ((*sampleIter) - pedestal);

      if (test_value >= RMS*n_sigma) {
	significant_sample = true;
	break;
      }
    }

    if (significant_sample == false) {
      continue;
    }
    
    // All OK, so allow this TPI to be passed back
    output.push_back(*pulseIter);
  }

  std::cout << "# of TPI (before): " << theIslands.size() << std::endl;
  std::cout << "# of TPI (after): " << output.size() << std::endl;

  return output;
}

ALCAP_REGISTER_MODULE(MakeAnalysedPulses,slow_gen,fast_gen);
