#include "RegisterModule.inc"
#include "MakeDetectorPulses.h"
#include "TVDetectorPulseGenerator.h"
#include "MaxTimeDiffDPGenerator.h"

extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;
extern std::map<std::string, std::vector<TDetectorPulse*> > gDetectorPulseMap;

MakeDetectorPulses::MakeDetectorPulses(modules::options* opts):FillHistBase("MakeDetectorPulses",opts){
    if(opts->HasOption("algorithm")){
      MakeGenerator(opts->GetString("algorithm"));
    }
    dir->cd("/");
}

MakeDetectorPulses::~MakeDetectorPulses(){
}

int MakeDetectorPulses::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  std::string fast_det_name;
  std::string slow_det_name;
  AnalysedPulseList_t *slow_pulses;
  AnalysedPulseList_t *fast_pulses;
  BankAnalPulseList_t::iterator current;
  //DetectorPulseList_t *detectorPulses;

  // Loop through paired channels
  for (ChannelPairing_t::iterator it_det = fFastSlowPairs.begin();
		  it_det != fFastSlowPairs.end(); it_det++) {
    // find the list of analyzed pulses for this channel
    if(it_det->second.first!="") {
      current=gAnalysedPulseMap.find(it_det->second.first);
      if(current==gAnalysedPulseMap.end()){
        std::cout<<"Unable to find channel for detector: "<<it_det->second.first<<std::endl;
        return 1;
      }
      fast_pulses=&(current->second);
    }else{
      fast_pulses=NULL;
    }

    if(it_det->second.second!="") {
      current=gAnalysedPulseMap.find(it_det->second.second);
      if(current==gAnalysedPulseMap.end()){
        std::cout<<"Unable to find channel for detector: "<<it_det->second.second<<std::endl;
        return 2;
      }
      slow_pulses=&(current->second);
    }else{
      slow_pulses=NULL;
    }

    fGenerator->ProcessPulses(gSetup,it_det->first,fast_pulses,slow_pulses,gDetectorPulseMap[it_det->first]);
  }
  return 0;
}

std::string MakeDetectorPulses::GetDetectorName(std::string in_name){
   if (*(in_name.end() -1)  == 'F' || *(in_name.end() -1)  == 'S' ){
	in_name.resize(in_name.length()-2);
	return in_name;
   } 
   return in_name;
}

std::string MakeDetectorPulses::GetOtherChannelName(std::string in_name,std::string det_name){
    if(det_name=="") det_name=GetDetectorName(in_name);

    if(in_name==det_name) return "";

    if (TSetupData::IsFast(in_name)){
       return det_name+"-S";
    }
    return det_name+"-F";
}

int MakeDetectorPulses::BeforeFirstEntry(TGlobalData* gData, TSetupData* setup){
  std::string det_name_a, det_name_b,det_name;
  bool det_a_isFast;
  //bool det_b_isSlow;

  // Find all fast detectors
  for (BankAnalPulseList_t::iterator channel = gAnalysedPulseMap.begin();
		  channel != gAnalysedPulseMap.end(); channel++) {
      // get the detector name which is the common part of the fast and slow channel names
      det_name_a=channel->first;
      det_name=GetDetectorName(det_name);
      if(fFastSlowPairs.find(det_name) != fFastSlowPairs.end() ) continue;
      det_name_b=GetOtherChannelName(det_name_a,det_name);

      det_a_isFast=TSetupData::IsFast(det_name_a);
      //det_b_isSlow=TSetupData::IsSlow(det_name_b);

      if(det_a_isFast) {
         fFastSlowPairs[det_name]=std::make_pair(det_name_a,det_name_b);
      }else{
         fFastSlowPairs[det_name]=std::make_pair(det_name_b,det_name_a);
      }

  }

  for( ChannelPairing_t::iterator i=fFastSlowPairs.begin();i!=fFastSlowPairs.end();i++){
    std::cout<<i->first<<'\t'<<i->second.first<<'\t'<<i->second.second<<std::endl;
  }
  return 0;
}

TVDetectorPulseGenerator* MakeDetectorPulses::MakeGenerator(const std::string& generatorType){
    // Select the generator type
    TVDetectorPulseGenerator* generator=NULL;
    // As we develop newer techniques we can add to the list here
    if (generatorType == "MaxTimeDiff"){
	generator = new MaxTimeDiffDPGenerator();
    } else {
    std::cout<<"Error: Unknown generator requested: "<<generatorType<<std::endl;	
	throw "Unknown generator requested";
	return NULL;
    }
    // setup some options on the generator
    generator->SetDebug(Debug());
    return generator;
}

ALCAP_REGISTER_MODULE(MakeDetectorPulses,algorithm);
