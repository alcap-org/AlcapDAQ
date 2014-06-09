#include "PulseViewer.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "ExportPulse.h"
#include "TAnalysedPulse.h"
#include "ModulesParser.h"

#include <iostream>
using std::cout;
using std::endl;
using modules::parser::GetOneWord;
using modules::parser::GetDouble;

#define PrintHelp std::cout<<__FILE__<<":"<<__LINE__<<": "
#define PrintValue(value) PrintHelp<<#value "= |"<<value<<"|"<<std::endl;

extern StringAnalPulseMap gAnalysedPulseMap;

PulseViewer::PulseViewer(modules::options* opts):
   FillHistBase("PulseViewer",opts),fAPList(NULL){
  fRequestedChannel=opts->GetString("channel"); 
  fChannel=fRequestedChannel;
  fTriggerCondition=opts->GetString("trigger"); 
  
}

PulseViewer::~PulseViewer(){
}

int PulseViewer::BeforeFirstEntry(TGlobalData* gData,TSetupData *setup){
   // Check we're also running with the ExportPulse module
   if(!ExportPulse::Instance()){
	   cout<<"PulseViewer: Error: You need to run with ExportPulse to use PulseViewer module"<<std::endl;
	   return 1;
   }
   
   // Check the channel is valid
   if(!fChannel.isValid()){
	   cout<<"PulseViewer: Error: No detector called '"<<fRequestedChannel<<"' exists"<<endl;
	   return 2;
   }
   
   // Parse the trigger string
   return ParseTriggerString();
}

int PulseViewer::ParseTriggerString(){
	// Look for something like 'amplitude > 3'
	size_t equality_start=fTriggerCondition.find_first_of("<>=");
	if(equality_start==std::string::npos) {
		cout<<"Error: trigger '"<<fTriggerCondition<<"'' doesn't contain an equality (==,>,<,<=,>=)"<<endl;
		return 1;
	}
	// Check the type of equality given
	int retVal=0;
	retVal=SetTriggerType(fTriggerCondition.substr(equality_start,2));
	if(retVal!=0) return retVal;

	// Check the parameter to compare
	retVal=SetTriggerParameter(GetOneWord(fTriggerCondition,0,equality_start));
	if(retVal!=0) return retVal;

	// Get the value to compare to
	retVal=SetTriggerValue(fTriggerCondition.substr(equality_start+fTypeString.size()));
	return retVal;
}

int PulseViewer::SetTriggerType(const std::string& equality){
	bool problem=false;
	if(equality[1]=='='){
		switch(equality[0]){
			case '<': fTriggerType=kLE; break;
			case '>': fTriggerType=kGE; break;
			case '=': fTriggerType=kE; break;
			default:
				  problem=true;
		}
		if(!problem)fTypeString=equality.substr(0,2);
	} else{
		switch(equality[0]){
			case '<': fTriggerType=kL; break;
			case '>': fTriggerType=kG; break;
			case '=': 
			default:
				  problem=true;
		}
		if(!problem)fTypeString=equality[0];
	}
	if(problem){
	  cout<<"Error: Unknown equality-type passed to PulseViewer: '"<<equality<<"'"<<endl;
	  return 2;
	}
	return 0;
}

int PulseViewer::SetTriggerParameter(const std::string& parameter){
	if(parameter=="amplitude") fTriggerParameter=kAmplitude;
	else if(parameter=="time") fTriggerParameter=kTime;
	else if(parameter=="TPI_length") fTriggerParameter=kTPILength;
	//else if(parameter=="integral") fTriggerParameter=kIntegral;
	else{
		cout<<"Error: Unknown parameter requested: '"<<parameter<<"'"<<endl;
		return 1;
	}
	fParameterString=parameter;
	return 0;
}

int PulseViewer::SetTriggerValue(const std::string& parameter){
	fTriggerValue=GetDouble(parameter);
	return 0;
}

int PulseViewer::ProcessEntry(TGlobalData* gData,TSetupData *setup){

    // Get the TAPs for this channel
    fAPList=&gAnalysedPulseMap[GetChannel()];

    if(!fAPList){
       cout<<"Problem getting TAP list for "<<GetChannel()<<endl;
	    return 1;
    }else if(fAPList->empty() ){
       cout<<"List of TAPS for '"<< GetChannel()<<"' was empty "<<endl;
       return 0;
    }

    // Check each TAP against trigger
    int retVal=0;
    for(AnalysedPulseList::iterator i_pulse=fAPList->begin();
		i_pulse!=fAPList->end() && retVal==0;
		i_pulse++){
	    retVal=ShouldDraw(*i_pulse);
    }
    if(retVal!=0) return retVal;

  return 0;
}

double PulseViewer::GetParameterValue(const TAnalysedPulse& pulse){
	double retVal=0;
	switch (fTriggerParameter){
  	case kAmplitude:
		retVal=pulse.GetAmplitude();
		break;
  	case kTime:
		retVal=pulse.GetTime();
		break;
  	case kIntegral:
		retVal=pulse.GetIntegral();
		break;
  	case kTPILength:
		//retVal=pulse.GetTPILength();
		break;
	}
	return retVal;
}

bool PulseViewer::ValuePassesTrigger(const double& value){
	bool retVal=false;
  switch (fTriggerType){
	  case kE:
		  retVal=(value==fTriggerValue);
		  break;
	  case kG:
		  retVal=(value>fTriggerValue);
		  break;
	  case kL:
		  retVal=(value<fTriggerValue);
		  break;
	  case kGE:
		  retVal=(value>=fTriggerValue);
		  break;
	  case kLE:
		  retVal=(value<=fTriggerValue);
		  break;
  }
  return retVal;
}

int PulseViewer::ShouldDraw(const TAnalysedPulse* pulse){
	// Check pulse passes trigger condition
	double value=GetParameterValue(*pulse);
	if(!ValuePassesTrigger(value)) return 0;
	if(Debug()){
		cout<<fParameterString<<" = "<<value<<" which is "<<fTypeString<<" " <<fTriggerValue<<endl; 
	}

	// If it does, ask ExportPulse to draw it
	// We're safe to assume Instance will return becuase we test it's
	// existence in BeforeFirstEntry
	ExportPulse::Instance()->AddToExportList(GetChannel(),pulse);
	return 0;
}

int PulseViewer::AfterLastEntry(TGlobalData* gData,TSetupData *setup){

  // Print extra info if we're debugging this module:
  if(Debug()){
  }

  return 0;
}

ALCAP_REGISTER_MODULE(PulseViewer,channel,trigger,pulse_type);
