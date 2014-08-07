#include "PulseViewer.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "ExportPulse.h"
#include "TAnalysedPulse.h"
#include "ModulesParser.h"
#include "debug_tools.h"
#include "IdSource.h"
#include "EventNavigator.h"

#include <TFormula.h>

#include <algorithm>
#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
using modules::parser::GetOneWord;
using modules::parser::GetDouble;

extern SourceAnalPulseMap gAnalysedPulseMap;
PulseViewer::ParameterKeys PulseViewer::fAvailableParams;

PulseViewer::PulseViewer(modules::options* opts):
    BaseModule("PulseViewer",opts),fTotalPlotted(0),fFormula(NULL){
        fRequestedSource=opts->GetString("source"); 
        fSource.Channel()=fRequestedSource;
        fTriggerCondition=opts->GetString("trigger"); 
        fSummarize=opts->GetBool("summarize"); 
        fMaxToPlot=opts->GetInt("max_plots",-1);
        fStopAtMax=opts->GetBool("stop_at_max_plots",true);
    }

PulseViewer::~PulseViewer(){
    if(fFormula) delete fFormula;
}

int PulseViewer::BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup){

   // Check we're also running with the ExportPulse module
   if(!ExportPulse::Instance()){
	   cout<<"PulseViewer: Error: You need to run with ExportPulse to use PulseViewer module"<<std::endl;
	   return 1;
   }
   
   // Check the channel is valid
   if(!fSource.Channel().isValid()){
	   cout<<"PulseViewer: Error: No detector called '"<<fRequestedSource<<"' exists"<<endl;
	   return 2;
   }

   // setup the possible parameters
   if(fAvailableParams.empty()){
     fAvailableParams["amplitude"]=kAmplitude;
     fAvailableParams["time"]=kTime;
     fAvailableParams["TPI_length"]=kTPILength;
     fAvailableParams["integral"]=kIntegral;
     fAvailableParams["energy"]=kEnergy;
     fAvailableParams["pedestal"]=kPedestal;
     fAvailableParams["trigger_time"]=kTriggerTime;
   }

   // Parse the trigger string
   return ParseTriggerString(fTriggerCondition);
}

int PulseViewer::ParseTriggerString(const std::string& trigger_condition){
    std::string expression =fTriggerCondition;
    for(ParameterKeys::const_iterator i_key=fAvailableParams.begin();
            i_key!=fAvailableParams.end();i_key++){
        modules::parser::ReplaceWords(expression,i_key->first,Form("[%d]",i_key->second));
    }
    DEBUG_VALUE(expression);

    fFormula=new TFormula("PulseViewerTrigger",expression.c_str());
    int ret= fFormula->Compile();
    if(ret){
        cout<<"Error: Bad trigger expression: '"<<fTriggerCondition<<"'"<<endl;
        cout<<"   Only use parameters from"<<endl;
        for(ParameterKeys::const_iterator it=fAvailableParams.begin();
                it!=fAvailableParams.end();
                it++){
            cout<<"    |-"<<it->first<<endl;
        }
    }
    return ret;
}

int PulseViewer::ProcessEntry(TGlobalData* gData, const TSetupData* setup){
    // Have we drawn the number of pulses we were meant to?
    if(fMaxToPlot<0 || fTotalPlotted>=fMaxToPlot){
        if(fStopAtMax){
            cout<<"PulseViewer::ProcessEntry: "<<fTotalPlotted<<" pulses have already been drawn which "
                <<"is greater than the limit of "<<fMaxToPlot<<" so I'm stopping execution.\n";
            //cout<<"You can probably ignore any of the immediate errors that follow this."<<endl;
            return 1;
        }
        return 0;
    }

    // Get the TAPs for this channel
    AnalysedPulseList* allTAPs=NULL;
    for(SourceAnalPulseMap::iterator i_source=gAnalysedPulseMap.begin();
            i_source!=gAnalysedPulseMap.end(); ++i_source){
        if(i_source->first.matches(GetSource())){
            allTAPs=&i_source->second;
            break;
        }
    }

    if(!allTAPs){
       cout<<"Problem getting TAP list for "<<GetSource()<<endl;
	    return 1;
    }else if(allTAPs->empty() ){
       if(Debug()) cout<<"List of TAPS for '"<< GetSource()<<"' was empty "<<endl;
       return 0;
    }

    // Check each TAP against trigger
    int retVal=0;
    for(AnalysedPulseList::iterator i_pulse=allTAPs->begin();
		i_pulse!=allTAPs->end() && retVal==0;
		i_pulse++){
	    retVal=ConsiderDrawing((*i_pulse)->GetParentID() ,*i_pulse);
    }
    if(retVal!=0) return retVal;

  return 0;
}

double PulseViewer::GetParameterValue(const TAnalysedPulse& pulse,const ParameterType& parameter){
	double retVal=0;
	switch (parameter){
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
        retVal=pulse.GetTPILength();
        break;
    case kEnergy:
        retVal=pulse.GetEnergy();
        break;
    case kPedestal:
        retVal=pulse.GetPedestal();
        break;
    case kTriggerTime:
        retVal=pulse.GetTriggerTime();
        break;
    }
    return retVal;
}

int PulseViewer::ConsiderDrawing(const TAnalysedPulseID& id, const TAnalysedPulse* pulse){
  // Check pulse passes trigger condition
    double vals[fAvailableParams.size()];
    for(ParameterKeys::const_iterator i_key=fAvailableParams.begin();
            i_key!=fAvailableParams.end();++i_key){
        vals[i_key->second]=GetParameterValue(*pulse,i_key->second);
    }
    double value=fFormula->EvalPar(vals);
  if(!value) return 0;
  if(Debug()){
    cout<<"PulseViewer: Event: "<<EventNavigator::Instance().EntryNo()
        <<" Plotting pulse "<<id<<" [ "<<fFormula->GetExpFormula("P")<<" ]"<<endl;
  }
  
  // If it does, ask ExportPulse to draw it
  // We're safe to assume Instance will return becuase we test it's
  // existence in BeforeFirstEntry
  if(fPulsesPlotted[EventNavigator::Instance().EntryNo()].count(id)==0){
      ExportPulse::Instance()->AddToExportList(pulse);
      fTotalPlotted++;
  }
  fPulsesPlotted[EventNavigator::Instance().EntryNo()][id]++;
  return 0;
}

int PulseViewer::AfterLastEntry(TGlobalData* gData, const TSetupData* setup){

  // Print extra info if we're requested to summarize the found pulses
  if(SummarisePlots()){
      const std::string prefix="     ";
      cout<<"Summary for pulse criteria: ("<<fTriggerCondition <<") on channel "<<fSource<<endl;
      if(!fPulsesPlotted.empty()){
          cout<<prefix<<" Event | Pulses drawn (times requested)"<<endl;
          cout<<prefix<<" ----- | ------------ "<<endl;
          for(EventPulseIDList_t::const_iterator i_event=fPulsesPlotted.begin();
                  i_event!=fPulsesPlotted.end();i_event++){
              cout<<prefix<<std::setw(6)<< i_event->first<<" | ";
              for(PulseIDList_t::const_iterator i_pulse=i_event->second.begin();
                      i_pulse!=i_event->second.end();i_pulse++){
                  cout<<std::setw(2)<< i_pulse->first<<"("<< i_pulse->second<<")"<<", ";
              }
              cout<<endl;
          }
          cout<<prefix<<" ----- | ------------ "<<endl;
      }
      cout<<prefix<<"Total pulses plotted = "<<fTotalPlotted<<endl;

  }

  return 0;
}

ALCAP_REGISTER_MODULE(PulseViewer,source,trigger,summarize);
