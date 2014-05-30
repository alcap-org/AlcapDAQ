#include "ModulesFactory.h"
#include "MakeAnalysedPulses.h"
#include "TVAnalysedPulseGenerator.h"
#include "MaxBinAPGenerator.h"
#include <iostream>
#include <utility>
#include <sstream>
#include "RegisterModule.inc"

using std::cout;
using std::endl;
using std::string;

extern StringAnalPulseMap gAnalysedPulseMap;

static std::string getOneWord(const std::string& in, size_t start=0, size_t stop=std::string::npos){
	std::stringstream ss(in.substr(start,stop));
	std::string word;
	ss>>word;
	return word;
}

static void getSeveralWords(const std::string& in, std::vector<std::string> &vect, size_t start=0, size_t stop=std::string::npos){
	char line[400];
	strcpy(line,in.substr(start,stop-start).c_str());
	char* word = strtok(line,", ");
	while(word != NULL){ 
	    vect.push_back(word);
	    word = strtok(NULL,", ");
	}
}

MakeAnalysedPulses::MakeAnalysedPulses(modules::options* opts):
   FillHistBase("MakeAnalysedPulses",opts),fOptions(opts){
	fSlowGeneratorType=opts->GetString("default_slow_generator");
	if(fSlowGeneratorType=="") fSlowGeneratorType="MaxBin";
	fFastGeneratorType=opts->GetString("default_fast_generator");
	if(fFastGeneratorType=="") fSlowGeneratorType="MaxBin";
	opts->GetVectorStrings("analyse_channels",fChannelsToAnalyse);
	dir->cd("/");
}

MakeAnalysedPulses::~MakeAnalysedPulses(){
}

int MakeAnalysedPulses::BeforeFirstEntry(TGlobalData* gData,TSetupData *setup){
    // Loop over every named detector channel in TSetupData
    std::vector<std::string> detectors;
    setup->GetAllDetectors(detectors);

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
    std::cout<<"Will analyse ";
    if(analyse_all) std::cout<<"all ";
    else std::cout<<fChannelsToAnalyse.size()<<" of ";
    std::cout<<detectors.size()<<" channels"<<std::endl;

    // For each channel:
    bool skip_detector=false;
    std::string des_gen;
    for(std::vector<std::string>::const_iterator det=detectors.begin();
		    det!=detectors.end();
		    det++){
       // if we should not analyse det, leave generator as NULL
       skip_detector=false;
       if(! analyse_all ){
          std::vector<std::string>::const_iterator it_chan;
          for(it_chan=fChannelsToAnalyse.begin(); it_chan!=fChannelsToAnalyse.end();it_chan++){
             if((*it_chan)== (*det)) break;
          }
          if(it_chan== fChannelsToAnalyse.end() ) skip_detector=true;
       }
       if(skip_detector){
       // fGenerators[*det]=NULL;
          continue;
       }
       // else find the right generator to build
       if(fOptions->HasOption(*det)){
         // If this channel is named explicitly, use that generator type
         bool success=ParseGeneratorList(*det);
	 if(! success) return 1;
       } else{
         // else use default value for this type of channel (fast or slow)
         if(TSetupData::IsFast(*det)){
            AddGenerator(*det,"!FAST!");
	 } else {
            AddGenerator(*det,"!SLOW!");
	 }
       }
    }
    return 0;
}

int MakeAnalysedPulses::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  // Generator just receives a bunch of TPIs and must return a list of TAPs

  // Loop over each generator
  string detname,bankname;
  PulseIslandList thePulseIslands;
  ChannelGenerators_t::iterator generator;
  AnalysedPulseList theAnalysedPulses;
  for(generator = fGenerators.begin(); generator != fGenerators.end(); generator++){
    // Get the bank name
    detname = (*generator)->GetDetector();
    bankname = gSetup->GetBankName(detname);

    // Get the TPIs
    thePulseIslands=gData->fPulseIslandToChannelMap[bankname];
    if(thePulseIslands.size()<=0){
       cout<<"List of TPIs for '"<< detname<<"' was empty "<<endl;
       continue;
    }

    // clear the list of analyse_pulses from the last iteration
    theAnalysedPulses.clear();

    // generate the new list of analyse_pulses
    (*generator)->ProcessPulses( gSetup, thePulseIslands,theAnalysedPulses);

    // add these into the map of analysed pulses
    gAnalysedPulseMap.insert(std::make_pair(detname,theAnalysedPulses));
  }
  return 0;
}

// Creates a generator for each one in the list
// Returns true on success or false if there was a problem
bool MakeAnalysedPulses::ParseGeneratorList(std::string detector){
	// Get a vector for the generator(s) that we want to use for this detector
	std::vector<std::string> generatorList;
	fOptions->GetVectorStringsByDelimiter(detector,generatorList);

	//scan to next item in the list
	size_t start_br=0;
	size_t end_br=std::string::npos;
	std::vector<std::string>::iterator gen;
	std::stringstream sstream;
	std::string arg,generator;
	TAPGeneratorOptions* opts;
	bool still_good=true;
	for(gen=generatorList.begin();gen!= generatorList.end();gen++){
	    // check if we have options for this generator
	    start_br=gen->find('(');
	    generator=getOneWord(*gen,0,start_br);
	    if(start_br!=std::string::npos){
	        // There are options for this generator
	        end_br=gen->find(')');
		sstream.str(gen->substr(start_br,end_br-start_br));
		opts=new TAPGeneratorOptions(detector+"::"+generator);
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

    // Get the requested generator
    TVAnalysedPulseGenerator* generator=NULL;
    try{
        generator=MakeGenerator(generatorType,opts);
    }catch(char const* error){
        return false;
    }
    generator->SetDetector(detector);

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

TVAnalysedPulseGenerator* MakeAnalysedPulses::MakeGenerator(const string& generatorType, TAPGeneratorOptions* opts){

    // Select the generator type
    TVAnalysedPulseGenerator* generator=NULL;
    // As we develop newer techniques we can add to the list here
    if (generatorType == "MaxBin"){
	generator = new MaxBinAPGenerator();
    } else if( generatorType == "PeakFitter") {
	// Temporarily I'm putting this here so I can demo how the config file
	// handles multiple generators.  Long term this will be removed
	generator = new MaxBinAPGenerator();
    } else {
	cout<<"Error: Unknown generator requested: "<<generatorType<<endl;	
	throw "Unknown generator requested";
	return NULL;
    }
    return generator;
}

ALCAP_REGISTER_MODULE(MakeAnalysedPulses,slow_gen,fast_gen);
