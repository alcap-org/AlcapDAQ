#include "TriggerPlotter.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"

#include <iostream>
using std::cout;
using std::endl;

extern StringAnalPulseMap gAnalysedPulseMap;

TriggerPlotter::TriggerPlotter(modules::options* opts):
   FillHistBase("TriggerPlotter",opts),fAPList(NULL){
  fChannel=opts->GetString("channel"); 
  fTriggerCondition=opts->GetString("trigger"); 
  
}

TriggerPlotter::~TriggerPlotter(){
}

int TriggerPlotter::BeforeFirstEntry(TGlobalData* gData,TSetupData *setup){
	// Check we're also running with the ExportPulse module
	// Check the channel is valid
	// Parse the trigger string
  return 0;
}

int TriggerPlotter::ProcessEntry(TGlobalData* gData,TSetupData *setup){

    // Get the TAPs for this channel
    fAPList=&gAnalysedPulseMap[GetChannel()];

    if(!fAPList){
       cout<<"Problem getting TAP list for "<<GetChannel()<<endl;
	    return 1;
    }else if(fAPList.empty() ){
       cout<<"List of TAPS for '"<< GetChannel()<<"' was empty "<<endl;
       return 0;
    }

    // Check each TAP against trigger
    int retVal=0;
    for(AnalysedPulseList::iterator i_pulse=fAPList->begin();
		i_pulse!=fAPList->end() && retVal==0;
		i_pulse++){
	    retVal=ShouldDraw(i);
    }
    if(retVal!=0) return retVal;

  return 0;
}

double TriggerPlotter::GetParameterValue(const TAnalysedPulse& pulse){
	double retVal=0;
	switch (fTriggerParameter){
  	case kAmplitude:
		pulse.GetAmplitude();
		break;
  	case kTime:
		pulse.GetTime();
		break;
  	case kIntegral:
		pulse.GetIntegral();
		break;
  	case kTPILength:
		//pulse.GetTPILength();
		break;
	}
}

bool TriggerPlotter::ValuePassesTrigger(const double& value){
	bool retVal=false;
  switch (fTriggerType){
	  case kE:
		  retVal==(value==fTriggerValue);
		  break;
	  case kG:
		  retVal==(value>fTriggerValue);
		  break;
	  case kL:
		  retVal==(value<fTriggerValue);
		  break;
	  case kGE:
		  retVal==(value>=fTriggerValue);
		  break;
	  case kLE:
		  retVal==(value<=fTriggerValue);
		  break;
  }
  return retVal;
}

int TriggerPlotter::ShouldDraw(i_pulse){
	// Check pulse passes trigger condition
	double value=GetParameterValue(i_pulse);
	if(!ValuePassesTrigger(value)) return 0;

	// If it does, ask ExportPulse to draw it
	// We're safe to assume Instance will return becuase we test it's
	// existence in BeforeFirstEntry
	ExportPulse::Instance()->AddToExportList(GetChannel(),pulse);
	return 0;
}

int TriggerPlotter::AfterLastEntry(TGlobalData* gData,TSetupData *setup){

  // Print extra info if we're debugging this module:
  if(Debug()){
  }

  return 0;
}

// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(TriggerPlotter,channel,trigger);
