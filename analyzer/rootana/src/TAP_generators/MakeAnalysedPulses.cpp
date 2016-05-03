#include "MakeAnalysedPulses.h"
#include "ModulesParser.h"
#include "SetupNavigator.h"
#include "TVAnalysedPulseGenerator.h"
#include "TAPGeneratorFactory.h"
#include <iostream>
#include <utility>
#include <sstream>
#include "RegisterModule.inc"
#include "EventNavigator.h"
#include "debug_tools.h"

using modules::parser::GetOneWord;
using std::cout;
using std::endl;
using std::string;

extern SourceAnalPulseMap gAnalysedPulseMap;
extern Long64_t* gEntryNumber;

MakeAnalysedPulses::MakeAnalysedPulses(modules::options* opts):
  BaseModule("MakeAnalysedPulses",opts,false),
  fSlowGeneratorType(opts->GetString("default_slow_generator")),
  fFastGeneratorType(opts->GetString("default_fast_generator")),
  fChannelsToAnalyse(),
  fOptions(opts),
  fDefaultOpts(new TAPGeneratorOptions("default generator options")){
    opts->GetVectorStringsByWhiteSpace("analyse_channels",fChannelsToAnalyse);
    if(Debug()) fDefaultOpts->SetOption("debug","true");
}

MakeAnalysedPulses::~MakeAnalysedPulses(){
}

int MakeAnalysedPulses::BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup){
    // Loop over every named detector channel in TSetupData
    std::vector<std::string> detectors;
    setup->GetAllDetectors(detectors);

    // do we analyse all channels?
    bool analyse_all=fChannelsToAnalyse.empty();
    if(!analyse_all){
        for(unsigned i=0;i<fChannelsToAnalyse.size();i++){
            if (fChannelsToAnalyse[i]=="all"){
                analyse_all=true;
                break;
            }
        }
    }
    std::cout<<"MakeAnalysedPulses::BeforeFirstEntry: Will analyse ";
    if(analyse_all) std::cout<<"all ";
    else std::cout<<fChannelsToAnalyse.size()<<" of ";
    std::cout<<detectors.size()<<" channels"<<std::endl;

    // For each channel:
    bool skip_detector=false;
    std::string des_gen;
    for(std::vector<std::string>::const_iterator det=detectors.begin();
            det!=detectors.end(); det++){
        // if we should not analyse det, leave generator as NULL
        skip_detector=false;
        if(! analyse_all ){
            std::vector<std::string>::const_iterator it_chan;
            for(it_chan=fChannelsToAnalyse.begin();
                    it_chan!=fChannelsToAnalyse.end();
                    it_chan++){
                if(modules::parser::iequals((*it_chan), (*det)) )break;
            }
            if(it_chan== fChannelsToAnalyse.end() ) skip_detector=true;
        }
	else {
	  // We want to skip the TDCs anyway
	  if (TSetupData::IsTDC(setup->GetBankName(*det))) {
	    std::cout << "Skipping detector " << *det << " because it is a TDC channel" << std::endl;
	    skip_detector=true;
	  }
	}
        if(skip_detector)
	  continue;
        // else find the right generator to build
        if(fOptions->HasOption(*det)){
            // If this channel is named explicitly, use that generator type
            // Get a vector for the generator(s) that we want to use for this detector
            std::vector<std::string> generatorList;
            fOptions->GetVectorStringsByDelimiter(*det,generatorList);
            bool success=ParseGeneratorList(*det,generatorList);
            if(! success) return 1;
        } else{
            // else use default value for this type of channel (fast or slow)
            if(TSetupData::IsFast(*det)){
                std::vector<std::string> generatorList;
                fOptions->GetVectorStringsByDelimiter("default_fast_generator",generatorList);
                bool success=ParseGeneratorList(*det,generatorList);
                if(!success) return 1;
            } else {
                std::vector<std::string> generatorList;
                fOptions->GetVectorStringsByDelimiter("default_slow_generator",generatorList);
                bool success=ParseGeneratorList(*det,generatorList);
                if(!success) return 1;
            }
        }
    }

    // now make an entry in gAnalysedPulseMap for each generator added
    for(ChannelGenerators_t::const_iterator i_gen=fGenerators.begin();
            i_gen!=fGenerators.end();i_gen++){
        gAnalysedPulseMap[(*i_gen)->GetSource()];
    }
    return 0;
}

int MakeAnalysedPulses::ProcessEntry(TGlobalData *gData, const TSetupData* gSetup){
    // Generator just receives a bunch of TPIs and must return a list of TAPs

    // Loop over each generator
    IDs::channel detector;
    PulseIslandList* thePulseIslands;
    ChannelGenerators_t::iterator generator;
    AnalysedPulseList theAnalysedPulses;
    int retVal=0;
    for(generator = fGenerators.begin(); generator != fGenerators.end(); generator++){
        // Get the bank name
        detector = (*generator)->GetChannel().str();

        // Get the TPIs
        thePulseIslands=&gData->fPulseIslandToChannelMap[(*generator)->GetBank()];
        if(thePulseIslands->empty() ){
          if( Debug()) cout << "Event No: " 
                            << EventNavigator::Instance().EntryNo() 
                            <<": List of TPIs for '"<< detector 
                            <<"' was empty "<< endl;
          continue;
        }

        // clear the list of analyse_pulses from the last iteration
        theAnalysedPulses.clear();

        // generate the new list of analyse_pulses
        (*generator)->SetPulseList(thePulseIslands);
        retVal=(*generator)->ProcessPulses( *thePulseIslands,theAnalysedPulses);
        if(retVal!=0) return retVal;

        // find the pulse list in gAnalysedPulseMap
        SourceAnalPulseMap::iterator it=gAnalysedPulseMap.find((*generator)->GetSource());
        if(it==gAnalysedPulseMap.end()){
            // source doesn't seem to exist in gAnalysedPulseMap
          cout <<"Error: New TAP source \"" << (*generator)->GetSource() 
               <<"\" seems to have been created during processing of pulses."
               <<endl;
            return 1;
        }
        // add the pulses into the map
        it->second=theAnalysedPulses;
        
        (*generator)->CalibratePulses(it->second);
    }
    return 0;
}

// Creates a generator for each one in the list
// Returns true on success or false if there was a problem
bool MakeAnalysedPulses::ParseGeneratorList(std::string detector,
        const std::vector<std::string>& generatorList){

    //scan to next item in the list
    size_t start_br=0;
    size_t end_br=std::string::npos;
    std::vector<std::string>::const_iterator gen;
    std::string arg,generator;
    TAPGeneratorOptions* opts;
    bool still_good=true;
    for(gen=generatorList.begin();gen!= generatorList.end();gen++){
      // check if we have options for this generator
      start_br=gen->find('(');
      generator=GetOneWord(*gen,0,start_br);
      if(start_br!=std::string::npos){
	std::stringstream sstream;
	// There are options for this generator
	++start_br; // Move to first character after '('
	end_br=gen->find(')');
	sstream.str(gen->substr(start_br,end_br-start_br));
	opts=new TAPGeneratorOptions(generator);
	for(int count=0; std::getline(sstream, arg,','); count++){
	  opts->AddArgument(count,arg);
	}
      }
      still_good = AddGenerator(detector,generator,opts);
      // Is everything ok to continue?
      if (!still_good) {
	return false;
      }
      // Get ready for next iteration
      opts=NULL;
    }
    // Everything went ok, return true
    return true;
}

bool MakeAnalysedPulses::AddGenerator(const string& detector,string generatorType,TAPGeneratorOptions* opts){
    // If generatorType is '!FAST!' or '!SLOW!' replace with default value
    enum {kFast, kSlow, kArbitrary} requestType;
    if(generatorType=="!FAST!") {
        generatorType=fFastGeneratorType;
        requestType=kFast;
    }else if(generatorType=="!SLOW!"){
        generatorType=fSlowGeneratorType;
        requestType=kSlow;
    }else requestType=kArbitrary;

    // Make sure opts is not null
    if(!opts) opts=new TAPGeneratorOptions(*fDefaultOpts);
    opts->SetChannel(detector);
    opts->Debug(Debug());

    // Get the requested generator
    TVAnalysedPulseGenerator* generator=
        TAPGeneratorFactory::Instance()->createModule(generatorType,opts);
    if(!generator) return false;

    // print something
    if(Debug()) {
        cout<<detector<<": using ";
        if (requestType ==kFast) cout<<"default fast: ";
        else if (requestType ==kSlow) cout<<"default slow: ";
        else cout<<"generator: ";
        cout<<generatorType ;
        if(opts) cout<<" with "<<opts->GetNumOptions()<<" option(s).";
        cout<<endl;
    }

    // Add this generator to the list for the required detector
    fGenerators.push_back(generator);
    return true;
}

ALCAP_REGISTER_MODULE(MakeAnalysedPulses,slow_gen,fast_gen);
