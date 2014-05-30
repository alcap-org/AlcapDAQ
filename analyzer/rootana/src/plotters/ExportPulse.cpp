#include "ModulesFactory.h"
#include "ModulesParser.h"
#include "ModulesNavigator.h"
#include "ExportPulse.h"
#include "TVAnalysedPulseGenerator.h"
#include "MaxBinAPGenerator.h"
#include <iostream>
#include <algorithm> //std::replace_if
#include <utility> //std::pair
#include <sstream>
#include "RegisterModule.inc"
#include <stdexcept>

using std::cout;
using std::endl;
using std::string;

extern StringAnalPulseMap gAnalysedPulseMap;
extern Long64_t* gEntryNumber;
extern Long64_t* gTotalEntries;

static bool isNonCpp(char c){ 
  bool retVal=false;
  switch(c){
  case '-': case '/':
    retVal=true;
    break;
  default:
    retVal=false;
  }
  return retVal;
}

ExportPulse::ExportPulse(modules::options* opts):
   FillHistBase("ExportPulse",opts),fGuidanceShown(false),
	fEventNumber(-1), fSetup(NULL),fOptions(opts)
	{
  dir->cd("/");
  fPulseInfo.ID=-1;
  fPulseInfo.bankname="";
  fPulseInfo.detname="";
}

ExportPulse::~ExportPulse(){
}

int ExportPulse::BeforeFirstEntry(TGlobalData* gData,TSetupData *setup){
   fSetup=setup;
   if(!fSetup){
      cout<<"Error: TSetupData passed to ExportPulse is NULL..."<<endl;
      return 1 ;
   }
   fTotalEvents=*gTotalEntries;

   ///// Check if we have a request to draw a specific set of events
   if(fOptions){
       int num=0;
       std::vector<std::string> currentList;
       std::vector<EventID_t> event_list;
       std::vector<PulseID_t> pulse_list;
       std::pair<std::string,std::string> event_pulse_request;
       std::string error_type;
       // Loop over all options
       for(modules::options::OptionsList_t::const_iterator i_opt = fOptions->begin();
                i_opt != fOptions->end(); i_opt++){
          // Continue if option is not "all" and not one of the detector names:
	  if( i_opt->first != "all" && fSetup->GetBankName(i_opt->first)=="") continue;

          // Break the list into items
	  currentList.clear();
	  event_list.clear();
	  pulse_list.clear();
	  num=fOptions->GetVectorStringsByDelimiter(i_opt->first,currentList);
	  if(num<=0) continue; // no options were actually specified

	  for(std::vector<std::string>::iterator i_request=currentList.begin();
		  i_request!=currentList.end();
		  i_request++){
	      event_pulse_request=modules::parser::ParseConstructor(*i_request,'(',')');
	      // Check things look healthy
	      if(event_pulse_request.first=="" )error_type="event";
	      else if( event_pulse_request.second=="")error_type="pulse";
	      else if( !ParseEventRequest(event_pulse_request.first, event_list)) error_type="event";
	      else if( !ParsePulseRequest(event_pulse_request.second,pulse_list)) error_type="pulse";
	      if(error_type != ""){
	          cout<<"Skipping badly formatted "<<error_type<<" specification: "<<*i_request<<endl;
		  cout<<"event="<<event_pulse_request.first<<", pulse="<<event_pulse_request.second<<endl;
		  ShowGuidance();
		  continue;
	      }
	      // Everything is ok so add this request to the list
	      for(std::vector<EventID_t>::const_iterator i_event=event_list.begin();
		      i_event!=event_list.end();
		      i_event++){
		 for(std::vector<PulseID_t>::const_iterator i_pulse=pulse_list.begin();
		        i_pulse!=pulse_list.end(); i_pulse++){
		    AddToConfigRequestList(*i_event,i_opt->first,*i_pulse);
		 }
	      }
	  }

       }
   }
   return 0;
}

int ExportPulse::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  // To be corrected once Phill finishes the event navigator
  fGlobalData=gData; 
  SetCurrentEventNumber(*gEntryNumber);

  // Check if we have any pulses to draw that were requested through the MODULEs file
  LoadPulsesRequestedByConfig();

  // Initialise variables that would be used in the loops
  TPulseIsland* pulse;
  const PulseIDList_t* requestedPulses;
  PulseIslandList* pulseList;

  // Loop over channel that we've been requested to draw a pulse from
  for(ChannelPulseIDs_t::const_iterator i_detector=fPulsesToPlot.begin();
		  i_detector!=fPulsesToPlot.end();
		  i_detector++){
     SetCurrentDetectorName(i_detector->first);

     // Get the pulse list for this channel
     requestedPulses=&(i_detector->second);
     pulseList=GetPulsesFromDetector();

     // Loop over every requested pulse for that channel
     for(PulseIDList_t::const_iterator i_pulseID=requestedPulses->begin();
		     i_pulseID!=requestedPulses->end();
		     i_pulseID++){
	     // Update the pulse info struct
	     SetCurrentPulseID(*i_pulseID);

        // Get the current requested TPI for this channel
	try{
		pulse=pulseList->at(*i_pulseID);
	}catch(const std::out_of_range& oor){
		cout<<"Skipping out of range pulse: "<<*i_pulseID<<" on detector '"<<i_detector->first<<endl;
		continue;
	}

	// Draw the pulse
        MakePlot(pulse);
     }
  }
  ClearPulsesToExport();
  return 0;
}

int ExportPulse::MakePlot(const TPulseIsland* pulse)const{
	
   std::stringstream histname;
   histname << "Pulse_" << GetCurrentBankName();
   histname << "_" << GetCurrentDetectorName();
   histname << "_" << GetCurrentEventNumber();
   histname << "_" << GetCurrentPulseID();
   std::string hist=histname.str();
   // replace all non c++ characters with underscore so we can use the
   // histograms in root directly.
   std::replace_if(hist.begin(),hist.end(), isNonCpp, '_');

   std::stringstream title;
   title << "Pulse " << GetCurrentPulseID();
   title << " from event " << GetCurrentEventNumber();
   title << " on detector " << GetCurrentDetectorName();
   title << " (" << GetCurrentBankName()<<")";

   // Print some stuff if wanted
   if(Debug()){
   	cout<<"Plotting pulse "<<GetCurrentPulseID()<<" for event "<<GetCurrentEventNumber();
   	cout<<", detector '"<<GetCurrentDetectorName()<<"' ["<<histname.str()<<"]"<<endl;
   }

   size_t num_samples = pulse->GetPulseLength();
   TH1F* hPulse = new TH1F(hist.c_str(), title.str().c_str(), num_samples,0,num_samples);
   
   for ( size_t i=0;i <num_samples; ++i) {
     hPulse->SetBinContent(i+1, pulse->GetSamples().at(i));
   }
   hPulse->Write();

   return 0;
}

PulseIslandList* ExportPulse::GetPulsesFromDetector(std::string bank){
   if(bank=="") bank=this->GetCurrentBankName();
   return &fGlobalData->fPulseIslandToChannelMap[bank];
}

void ExportPulse::ShowGuidance(){
// Only show guidance once
  if(fGuidanceShown) return;
  fGuidanceShown=true;
  cout<<"Requests to draw a pulse should be formatted as event(pulse),"<<endl;
  cout<<" where event and pulse are the numbers of each event and pulse you wish to draw"<<endl;
	 //", a range (specified by A-Z, where A and Z are the first and last value of the range),"
	 //" or a number of random events to select (specified by ~N where N is the number of events)";
}

bool ExportPulse::ParseEventRequest(std::string input, std::vector<EventID_t>& event_list){
	return ParseRequest(input,event_list,"event",0, GetTotalNumberOfEvents());
}

bool ExportPulse::ParsePulseRequest(std::string input, std::vector<EventID_t>& list){
	return ParseRequest(input,list,"pulse",0, GetTotalNumberOfEvents());
}

bool ExportPulse::ParseRequest(std::string input, std::vector<EventID_t>& list, const std::string& type, Long64_t lower_limit, Long64_t upper_limit){
	// Strip all whitespace
	modules::parser::RemoveWhitespace(input);

	// Check if input string contains a number,
	if(modules::parser::IsNumber(input)){
		int event = modules::parser::GetNumber(input);
		if (event >=lower_limit && event < upper_limit ){
			list.push_back(event);
		} else {
			cout<<"Requested "<<type<<", "<<event<<" is out of range."<<endl;
			cout<<"Acceptable values are: ["<<lower_limit<<","<<upper_limit<<"[."<<endl;
			return false;
		}
		return true;
	}
	// Future Ideas:
	// Check if input contains a range,
	// Check if input contains a random request
	return false;
}

void ExportPulse::LoadPulsesRequestedByConfig(){
	  // 7) In Process entry, invoke a method to check if there were requested or a pulse on a channel in this event
	  //    and load them into fPulsesToPlot;

	for(EventChannelPulseIDs_t::iterator i_channel=fRequestedByConfig.begin();
			i_channel!=fRequestedByConfig.end(); i_channel++){
		EventPulseIDList_t::iterator i_event=i_channel->second.find(GetCurrentEventNumber());
		if(i_event!=i_channel->second.end()){
			for(PulseIDList_t::const_iterator i_pulse=i_event->second.begin();
					i_pulse!=i_event->second.end(); i_pulse++){
				AddToExportList(i_channel->first,*i_pulse);
			}
		}
	}
	
	// Check if all channels were requested
}

void ExportPulse::ClearPulsesToExport(){
  ChannelPulseIDs_t::iterator i_channel;
  for (i_channel=fPulsesToPlot.begin();i_channel!=fPulsesToPlot.end();i_channel++){
		  i_channel->second.clear();
  }
  fPulsesToPlot.clear();
}

ALCAP_REGISTER_MODULE(ExportPulse);
