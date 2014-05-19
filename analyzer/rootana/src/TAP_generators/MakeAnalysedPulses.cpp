#include "ModulesManager.h"
#include "MakeAnalysedPulses.h"
#include "TVAnalysedPulseGenerator.h"
#include "MaxBinAPGenerator.h"
#include <iostream>
#include <utility>
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
		    det!=detectors.end();
		    det++){
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
         bool success=fGenerators[*det]=ParseGeneratorList(des_gen);
	 if(! sucess) return 1;
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

    // clear the list of analyse_pulses from the last iteration
    theAnalysedPulses.clear();
    // generate the new list of analyse_pulses
    gen->ProcessPulses( gSetup, thePulseIslands,theAnalysedPulses);
    // add these into the map of analysed pulses
    gAnalysedPulseMap.insert(std::make_pair(detname,theAnalysedPulses));
  }
  return 0;
}

bool MakeAnalysedPulses::ParseGeneratorList(std::string generatorList){
	// Creates a generator for each one in the list
	// Returns true on success or false if there was a problem

	//scan to next item in the list
	size_t remaining_chars=generatorList.length();
	size_t start_gen=0;
	size_t end_gen=std::string::npos;
	string generator;
	modules::options* gen_opts;
	while (remaining_chars>0){
	    end_gen=generatorList.find(':',start_gen);

	    if( Debug() ) std::cout<<"
	    // decrement the number of chars we still need to consider by the
	    // length of the current generators description
	    remaining_chars -= end_gen - start_gen;
	    // Move the start of the next description to the end of the current one
	    start_gen=end_gen+1;
	}

//	char line[400];
//	strcpy(line,generatorList.c_str());
//	// Read until the first whitespace or close bracket
//	char* word = strtok(line,"\t )");
//	while(word != NULL){ 
//	    vect.push_back(word);
//            word = strtok(line,"\t )");
//	}
	return true;
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

ALCAP_REGISTER_MODULE(MakeAnalysedPulses,slow_gen,fast_gen);
