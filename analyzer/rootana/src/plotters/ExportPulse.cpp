#include "ModulesFactory.h"
#include "ModulesNavigator.h"
#include "ExportPulse.h"
#include "TVAnalysedPulseGenerator.h"
#include "MaxBinAPGenerator.h"
#include <iostream>
#include <algorithm> //replace_if
#include <sstream>
#include "RegisterModule.inc"
using std::cout;
using std::endl;
using std::string;

extern std::map<std::string, std::vector<TAnalysedPulse*> > gAnalysedPulseMap;
extern Long64_t* gEntryNumber;

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
   FillHistBase("ExportPulse",opts){
	dir->cd("/");
}

ExportPulse::~ExportPulse(){
}

int ExportPulse::BeforeFirstEntry(TGlobalData* gData,TSetupData *setup){
   fSetup=TSetupData::Instance();
   if(!fSetup){
      cout<<"Error: TSetupData::Instance() is returning NULL..."<<endl;
      return 1 ;
   }

    return 0;
}

int ExportPulse::ProcessEntry(TGlobalData *gData, TSetupData *gSetup){
  // To be corrected once Phill finishes the event navigator
  fGlobalData=gData; 
  SetCurrentEventNumber(*gEntryNumber);

  // Initialise variables that would be used in the loops
  TPulseIsland* pulse;
  const PulseIDList_t* requestedPulses;
  PulseIslandList_t* pulseList;

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
	pulse=pulseList->at(*i_pulseID);

	// Print some stuff if wanted
	if(Debug()){
		cout<<"Plotting pulse: "<<*i_pulseID<<" for event "<<GetCurrentEventNumber()<<", detector: "<<GetCurrentDetectorName()<<endl;
	}
	
	// Draw the pulse
        MakePlot(pulse);
     }
  }
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
   if(Debug()){
	   cout <<"histname: "<<histname.str()<<endl;
	   cout <<"Title: "<<title.str()<<endl;
   }

   size_t num_samples = pulse->GetPulseLength();
   TH1F* hPulse = new TH1F(hist.c_str(), title.str().c_str(), num_samples,0,num_samples);
   
   for ( size_t i=0;i <num_samples; ++i) {
     hPulse->SetBinContent(i+1, pulse->GetSamples().at(i));
   }
   hPulse->Write();

   return 0;
}

ExportPulse::PulseIslandList_t* ExportPulse::GetPulsesFromDetector(std::string bank){
   if(bank=="") bank=this->GetCurrentBankName();
   return &fGlobalData->fPulseIslandToChannelMap[bank];
}

ALCAP_REGISTER_MODULE(ExportPulse);
