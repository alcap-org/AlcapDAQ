#include "ExportPulse.h"

//C++/STL
#include <iostream>
#include <algorithm> //std::replace_if
#include <utility> //std::pair
#include <sstream>
#include <stdexcept>
#include <cmath>

//ROOT
#include <TH1F.h>
#include <THStack.h>
#include <TDirectory.h>
#include <TCanvas.h>
#include <TPolyMarker.h>

//Local
#include "ModulesFactory.h"
#include "ModulesParser.h"
#include "ModulesNavigator.h"
#include "TVAnalysedPulseGenerator.h"
#include "MaxBinAPGenerator.h"
#include "RegisterModule.inc"
#include "EventNavigator.h"
#include "SetupNavigator.h"
#include "debug_tools.h"

#include "TTemplateFitAnalysedPulse.h"
#include "InterpolatePulse.h"
#include "TPaveText.h"

using std::cout;
using std::endl;
using std::string;

//extern SourceAnalPulseMap gAnalysedPulseMap;
extern Long64_t* gTotalEntries;

//----------------------------------------------------------------------
ExportPulse::ExportPulse(modules::options* opts)
  : BaseModule("ExportPulse",opts),fGuidanceShown(false)
  , fSetup(NULL), fOptions(opts), fPulseFinder_TSpectrum(NULL)
{
  fPulseInfo.pulseID=-1;
  fPulseInfo.event=-1;
  fPulseInfo.bankname="";
  fPulseInfo.detname="";
  
  fUsePCF=opts->GetFlag("run_pulse_finder");
  if(fUsePCF){
      fPulseFinder_TSpectrum=new PulseCandidateFinder_TSpectrum();
  }
  std::string x_axis = opts->GetString("x_axis");
  if (x_axis == "sample_number") {
    fXAxis = kSampleNumber;
  }
  else if (x_axis == "block_time") {
    fXAxis = kBlockTime;
  }
  else if (x_axis == "pulse_time") {
    fXAxis = kPulseTime;
  }
  else {
    fXAxis = kXAxisError;
  }

  std::string y_axis = opts->GetString("y_axis");
  if (y_axis == "not_pedestal_subtracted") {
    fYAxis = kNotPedestalSubtracted;
  }
  else if (y_axis == "pedestal_subtracted") {
    fYAxis = kPedestalSubtracted;
  }
  else {
    fYAxis = kYAxisError;
  }
  
  fTPIDirectory=GetDirectory("TPIs");
  fTAPDirectory=GetDirectory("TAPs");
}


//----------------------------------------------------------------------
ExportPulse::~ExportPulse()
{}


//----------------------------------------------------------------------
int ExportPulse::BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup){
  fSetup=setup;
  if(!fSetup){
    cout<<"Error: TSetupData passed to ExportPulse is NULL..."<<endl;
    return 1 ;
  }
  fTotalEvents = EventNavigator::Instance().GetInputNEntries();
  
  ///// Check if we have a request to draw a specific set of events
  if(fOptions){
    int num=0;
    std::vector<std::string> currentList;
    std::vector<EventID_t> event_list;
    std::vector<TPulseIslandID> pulse_list;
    modules::parser::Constructor_t event_pulse_request;
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
        if(event_pulse_request.before=="" )error_type="event";
        else if( event_pulse_request.inside=="")error_type="pulse";
        else if( !ParseEventRequest(event_pulse_request.before, event_list)) error_type="event";
        else if( !ParsePulseRequest(event_pulse_request.inside,pulse_list)) error_type="pulse";
        if(error_type != ""){
          cout<<"Skipping badly formatted "<<error_type<<" specification: "<<*i_request<<endl;
          cout<<"event="<<event_pulse_request.before<<", pulse="<<event_pulse_request.inside<<endl;
          ShowGuidance();
          continue;
        }
        // Everything is ok so add this request to the list
        for(std::vector<EventID_t>::const_iterator i_event=event_list.begin();
            i_event!=event_list.end();
            i_event++){
          for(std::vector<TPulseIslandID>::const_iterator i_pulse=pulse_list.begin();
              i_pulse!=pulse_list.end(); i_pulse++){
            AddToConfigRequestList(*i_event,i_opt->first,*i_pulse);
          }
        }
      }
    }
  }

  // Check that the x- and y-axes are well defined
  if (fXAxis == kXAxisError || fYAxis == kYAxisError) {
    std::cout << "ExportPulse: fXAxis or fYAxis are not defined" << std::endl;
    return 1;
  }
  return 0;
}


//----------------------------------------------------------------------
int ExportPulse::ProcessEntry(TGlobalData *gData, const TSetupData* gSetup){
  // To be corrected once Phill finishes the event navigator
  fGlobalData=gData; 
  SetCurrentEventNumber(EventNavigator::Instance().EntryNo());
  
  // Check if we have any pulses to draw that were requested through the MODULEs file
  LoadPulsesRequestedByConfig();

  fTPIDirectory->cd();
  int ret_val=DrawTPIs();
  if(ret_val!=0) return ret_val;

  fTAPDirectory->cd();
  ret_val=DrawTAPs();
  if(ret_val!=0) return ret_val;

  ClearPulsesToExport();
  return 0;
}


//----------------------------------------------------------------------
int ExportPulse::DrawTAPs(){
  
  // Loop over all the TAPs we've been asked to export
  //  std::cout << "AE (ExportPulse) TAPsToPlot = " << fTAPsToPlot.size() << std::endl;
  fPulseInfo.subPulseID=-1;
  for(ChannelTAPs_t::const_iterator i_channel_tap=fTAPsToPlot.begin();
      i_channel_tap!=fTAPsToPlot.end();
      i_channel_tap++){

    SetCurrentDetectorName(i_channel_tap->first);
    
    const TAnalysedPulse* i_pulse = i_channel_tap->second;
    if (i_pulse->GetParentID() == fPulseInfo.pulseID) {
      // already on the same TPI
      SetCurrentSubPulseID(fPulseInfo.subPulseID+1);
    }
    else {
      SetCurrentPulseID(i_pulse->GetParentID());
      SetCurrentSubPulseID(0); //reset
    }
    
    // Draw the pulse
    PlotTAP(i_pulse,fPulseInfo);

    // if we've also get another TAP from the same TPI
    // Draw the summed TAP
  }
  return 0;
}


//----------------------------------------------------------------------
int ExportPulse::DrawTPIs(){
  // Initialise variables that would be used in the loops
  TPulseIsland* pulse;
  const PulseIDList_t* requestedPulses;
  PulseIslandList* pulseList;

  // Loop over all the pulses that have been requested (this should be time ordered)
  //  std::cout << "AE (ExportPulse) TPIsToPlot = " << fTPIsToPlot.size() << std::endl;
  for(ChannelPulseIDs_t::const_iterator i_channel_pulseid=fTPIsToPlot.begin();
      i_channel_pulseid!=fTPIsToPlot.end();
      i_channel_pulseid++){
    SetCurrentDetectorName(i_channel_pulseid->first);
    
    // Get the pulse list for this channel
    TPulseIslandID i_pulseID = i_channel_pulseid->second;
    pulseList=GetTPIsFromDetector();
    
    SetCurrentPulseID(i_pulseID);
      
    // Get the current requested TPI for this channel
    try{
      pulse=pulseList->at(i_pulseID);
    }
    catch(const std::out_of_range& oor){
      cout<<"Skipping out of range pulse: "<<i_pulseID<<" on detector '"<<i_channel_pulseid->first<<endl;
      continue;
    }
      
    // Draw the pulse
    PlotTPI(pulse,fPulseInfo);
  }
  return 0;
}

//----------------------------------------------------------------------
std::string ExportPulse::PulseInfo_t::MakeTPIName()const{
  std::stringstream histname;
  histname << "Pulse_" << detname;
  histname << "_" << event;
  histname << "_" << pulseID;
  std::string hist=histname.str();
  // replace all non c++ characters with underscore so we can use the
  // histograms in root directly.
  modules::parser::ToCppValid(hist);
  return hist;
}

std::string ExportPulse::PulseInfo_t::MakeTAPName()const{
  std::stringstream histname;
  histname << "Pulse_" << detname;
  histname << "_" << event;
  histname << "_" << pulseID;
  histname << "_" << subPulseID;
  std::string hist=histname.str();
  // replace all non c++ characters with underscore so we can use the
  // histograms in root directly.
  modules::parser::ToCppValid(hist);
  return hist;
}

//----------------------------------------------------------------------
int ExportPulse::PlotTPI(const TPulseIsland* pulse, const PulseInfo_t& info){
  
  std::string hist=info.MakeTPIName();
  
  std::stringstream title;
  title << "Pulse " << info.pulseID;
  title << " from event " << info.event;
  title << " on detector " << info.detname;
  title << " (" << info.bankname<<")";
  
  // Print some stuff if wanted
  if(Debug()){
    cout<<"Plotting "<<title.str()<<"' ["<<hist<<"]"<<endl;
  }
  
  TH1F* fullPulse=MakeHistTPI(pulse,hist,info);
  fullPulse->SetDirectory(fTPIDirectory);
  fullPulse->SetTitle(title.str().c_str());

  if(fUsePCF){
      // don't save the original
    //      fullPulse->SetDirectory(0);

      // make the stack
      //      THStack* stack=new THStack((hist+"_pulse_candidates").c_str(),title.str().c_str());
      //      stack->Add(fullPulse);

    //      std::string canvasname = "c_" + hist;
    //      TCanvas* c_export = new TCanvas(canvasname.c_str(), canvasname.c_str());
    //      fullPulse->Draw("HIST");

    fPulseFinder_TSpectrum->FindPulseCandidates(pulse, fullPulse);
      TList *functions = fullPulse->GetListOfFunctions();
      TPolyMarker *pm = (TPolyMarker*)functions->FindObject("TPolyMarker");
      if (pm) {
	//	pm->SetMarkerSize(0.5);
      	pm->Draw();
      }
      //      std::cout << "AE: Event #" << info.event << ", Pulse #" << info.pulseID << ": NPulses = " << fPulseFinder_TSpectrum->GetNPulseCandidates() << std::endl;
      /*      fPulseFinder_TSpectrum->GetPulseCandidates(fSubPulses);
      std::stringstream sub_pulse_name;
      for(PulseIslandList::const_iterator i_tpi=fSubPulses.begin(); i_tpi!=fSubPulses.end(); ++i_tpi){
	//          if((*i_tpi)->GetPulseLength() < 14) continue;

	int i_sub_pulse = i_tpi-fSubPulses.begin();
	int shift=(*i_tpi)->GetTimeStamp()-pulse->GetTimeStamp();
	sub_pulse_name.str("");
	sub_pulse_name << "Sub" << hist << "_" << i_sub_pulse;
	TH1F* sub_pulse=MakeHistTPI(*i_tpi,sub_pulse_name.str(),info,shift,pulse->GetPulseLength());
	sub_pulse->SetTitle(title.str().c_str());
	sub_pulse->SetLineColor(kMagenta);
	// need to subtract found pulse from full pulse else THStack
	// superposes the two regions 
	sub_pulse->Draw("HIST SAME");
	
	double new_x = sub_pulse->GetBinCenter(sub_pulse->GetMaximumBin());
	double new_y = sub_pulse->GetMaximum();
	TPolyMarker* new_marker = new TPolyMarker(1, &new_x, &new_y);
	new_marker->SetMarkerColor(kBlue);
	new_marker->SetMarkerStyle(kOpenTriangleDown);
	new_marker->Draw("SAME");

	  //	  fTPIDirectory->Add(sub_pulse);
	  //	  fullPulse->Add(sub_pulse,-1);
	  //          stack->Add(sub_pulse);
      }
      */
      // Save the stack
      //      fTPIDirectory->Add(stack);
      //      fTPIDirectory->Add(c_export);
  }

  return 0;
}

//----------------------------------------------------------------------
TH1F* ExportPulse::MakeHistTPI(const TPulseIsland* pulse, const std::string& name, const PulseInfo_t& info, int shift, int samples)const{

  int downsampling = TSetupData::GetDownSampling(info.bankname.c_str(), SetupNavigator::Instance()->GetRunNumber());
  size_t num_samples = samples? samples: pulse->GetPulseLength();
  double min=0;
  double max=num_samples;
  double pedestal = 0;
  if (fXAxis != kSampleNumber) {
    max *= TSetupData::Instance()->GetClockTick(info.bankname)*downsampling;
  }
  if (fXAxis == kBlockTime) {
    double start_time = pulse->GetTimeStamp() * TSetupData::Instance()->GetClockTick(info.bankname);
    min += start_time;
    max += start_time;
  }
  if (fYAxis == kPedestalSubtracted) {
    pedestal = SetupNavigator::Instance()->GetPedestal(info.detname);
  }

  TH1F* hPulse = new TH1F(name.c_str(), name.c_str(), num_samples,min,max);
  hPulse->GetXaxis()->SetNoExponent(true);
  hPulse->SetDirectory(0);

  switch (fXAxis) {
  case kSampleNumber:
    hPulse->SetXTitle("sample number [ct]");
    break;

  case kPulseTime:
    hPulse->SetXTitle("pulse time [ns]");
    break;

  case kBlockTime:
    hPulse->SetXTitle("block time [ns]");
    break;

  case kXAxisError:
    return NULL;
  }

  switch (fYAxis) {
  case kNotPedestalSubtracted:
    hPulse->SetYTitle("pedestal not subtracted [ADC]");
    break;
  case kPedestalSubtracted:
    hPulse->SetYTitle("pedestal subtracted [ADC]");
    break;
  case kYAxisError:
    return NULL;
  }

  double pedestal_error = SetupNavigator::Instance()->GetNoise(info.detname);
  int n_bits = TSetupData::Instance()->GetNBits(info.bankname);
  double max_adc_value = std::pow(2, n_bits)-1;
  int trigger_polarity = TSetupData::Instance()->GetTriggerPolarity(info.bankname);

  size_t bin=0;
  for ( size_t i=0;i <(size_t)pulse->GetPulseLength(); ++i) {
    bin=i+1+shift;
    int sample_value = pulse->GetSamples().at(i);
    if (fYAxis == kPedestalSubtracted) {
      hPulse->SetBinContent(bin, trigger_polarity*(sample_value-pedestal));
    }
    else if (fYAxis == kNotPedestalSubtracted) {
      hPulse->SetBinContent(bin, trigger_polarity*sample_value);
    }

    // Set the bin contents and bin error
    if (sample_value >= max_adc_value) {
      hPulse->SetBinError( bin, max_adc_value); // set a large error bar so that template fitting can work
    }
    else {
      hPulse->SetBinError( bin, pedestal_error);
    }
  }
  hPulse->SetStats(false);
  return hPulse;
}


//----------------------------------------------------------------------
int ExportPulse::PlotTAP(const TAnalysedPulse* pulse, const PulseInfo_t& info)const{
  std::string hist=info.MakeTPIName();
  TH1F* tpi_hist=NULL;
  fTPIDirectory->GetObject(hist.c_str(),tpi_hist);
  TH1F* tap_hist=MakeHistTAP(tpi_hist, pulse, info);

  tap_hist->SetDirectory(fTAPDirectory);

  return 0;
}

TH1F* ExportPulse::MakeHistTAP(TH1F* tpi_hist, const TAnalysedPulse* pulse, const PulseInfo_t& info)const{
  if(tpi_hist) {
    std::stringstream name;
    name.str("");
    name << tpi_hist->GetName();
    
    double x_max=tpi_hist->GetXaxis()->GetXmax();
    double x_min=tpi_hist->GetXaxis()->GetXmin();
    TH1F* tap_pulse=NULL;
    if (dynamic_cast<const TTemplateFitAnalysedPulse*>(pulse)) {
      // Currently TTemplateFitAnalysedPulse defined time as block time and amplitude as pedestal subtracted
      // the template is defined to have time at its maximum bin and amplitude is normalised to 1
      name << "_" << info.subPulseID << "_templateAP";
      const TTemplateFitAnalysedPulse* tf_pulse =static_cast<const TTemplateFitAnalysedPulse*>(pulse);

      int n_bins=tf_pulse->GetTPILength()*tf_pulse->GetTemplate()->GetRefineFactor();
      tap_pulse=new TH1F(name.str().c_str(), name.str().c_str(),n_bins,x_min,x_max);

      double time = tf_pulse->GetTime(); // CF time
      
      //      double time_shift = 0;
      double time_shift = SetupNavigator::Instance()->GetCoarseTimeOffset(pulse->GetSource());
      double clock_tick_in_ns = TSetupData::Instance()->GetClockTick(info.bankname);
      int down_samp = TSetupData::GetDownSampling(info.bankname.c_str(), SetupNavigator::Instance()->GetRunNumber());
      double sample_time = (time - tf_pulse->GetTriggerTime()) / (clock_tick_in_ns * down_samp);

      if (fXAxis == kBlockTime) {
	time += time_shift;
      }
      else if (fXAxis == kPulseTime) {
	time -= tf_pulse->GetTriggerTime();
	time += time_shift;
      }
      else if (fXAxis == kSampleNumber) {
	time = sample_time;
      }

      for ( int i_bin=0; i_bin<=n_bins; i_bin++){

	//	int tpl_bin=tf_pulse->GetTemplate()->GetTime() // the template time in template bins
	//	    - sample_time*tf_pulse->GetTemplate()->GetRefineFactor() // the shift
	//	    + i_bin;
	int tpl_bin = -tf_pulse->GetTemplateTimeOffset() / (down_samp)
	  + i_bin;

	// get bin sample
	double sample=tf_pulse->GetTemplate()->GetHisto()->GetBinContent(tpl_bin);
	sample *= tf_pulse->GetAmplitude();
	if (fYAxis == kNotPedestalSubtracted) {
	  sample += tf_pulse->GetPedestal();
	}
	
	tap_pulse->SetBinContent(i_bin+1,sample);
      }

      // Print some useful information
      TPaveText* text_b=new TPaveText(0.66,0.7,0.9,0.9,"NB NDC");
      text_b->AddText(Form("#chi^2 = %3.2g",tf_pulse->GetChi2()));
      text_b->AddText(Form("Status = %d",tf_pulse->GetFitStatus()));
      text_b->AddText(Form("Time = %g",time));//tf_pulse->GetTime()));
      text_b->AddText(Form("Ampl. = %g",tf_pulse->GetAmplitude()));
      text_b->AddText(Form("NDoF = %d",tf_pulse->GetNDoF()));
      text_b->SetFillColor(kWhite);
      text_b->SetBorderSize(1);
      tap_pulse->SetStats(false);
      tap_pulse->GetListOfFunctions()->Add(text_b);

      TLine* time_line = new TLine(time, 0, time, tap_pulse->GetMaximum());
      time_line->SetLineColor(kBlue);
      time_line->SetLineWidth(2);
      tap_pulse->GetListOfFunctions()->Add(time_line);

      //      TH1F* residual=(TH1F*) tpi_hist->Clone(Form("%s_residual",tap_pulse->GetName()));
      TH1F* tap_rebinned=(TH1F*) tap_pulse->Rebin(tf_pulse->GetTemplate()->GetRefineFactor(),Form("%s_rebin",tap_pulse->GetName()));
      tap_rebinned->Scale(1.0/tf_pulse->GetTemplate()->GetRefineFactor());
      tap_rebinned->Write();
      //      residual->Add(tap_rebinned, -1./tf_pulse->GetTemplate()->GetRefineFactor());
      //      residual->Write();
    }
    else {
      name << "_AP";
      int n_bins=tpi_hist->GetXaxis()->GetNbins();
      tap_pulse = new TH1F(name.str().c_str(),("TAP for "+name.str()).c_str(),n_bins,x_min,x_max);
      // for standard TAnalysedPulses: time is defined as block time and amplitudes are given as pedestal subtracted
      // do the conversion if needs be
      double time = pulse->GetTime();
      //      std::cout << "Time before conversion = " << time << std::endl;
      if (fXAxis == kBlockTime) {
	double time_shift = SetupNavigator::Instance()->GetCoarseTimeOffset(pulse->GetSource());
	time += time_shift;
      }
      else if (fXAxis == kPulseTime) {
	double time_shift = SetupNavigator::Instance()->GetCoarseTimeOffset(pulse->GetSource());
	time -= pulse->GetTriggerTime();
	time += time_shift;
      }
      else if (fXAxis == kSampleNumber) {
	time -= pulse->GetTriggerTime();
	double clock_tick_in_ns = TSetupData::Instance()->GetClockTick(info.bankname);
	//	double time_shift = SetupNavigator::Instance()->GetCoarseTimeOffset(pulse->GetSource());
	int down_samp = TSetupData::GetDownSampling(info.bankname.c_str(), SetupNavigator::Instance()->GetRunNumber());
	//	time += time_shift;
	time = time / (clock_tick_in_ns * down_samp);
      }
      //      std::cout << "Time after conversion = " << time << std::endl;

      double amplitude = pulse->GetAmplitude();
      //      std::cout << "Amplitude before converion = " << amplitude << std::endl;
      if (fYAxis == kNotPedestalSubtracted) {
	double pedestal = SetupNavigator::Instance()->GetPedestal(info.detname);
	amplitude += pedestal;
      }
      //      std::cout << "Amplitude after cnoversio = " << amplitude << std::endl;

      int bin=tap_pulse->FindBin(time);	    
    
      int trigger_polarity = TSetupData::Instance()->GetTriggerPolarity(info.bankname);
      tap_pulse->SetBinContent(bin,trigger_polarity*amplitude);
    }


    tap_pulse->SetLineColor(kRed);
    return tap_pulse;
  }
}

//----------------------------------------------------------------------
PulseIslandList* ExportPulse::GetTPIsFromDetector(std::string bank){
  if(bank=="") bank=this->GetCurrentBankName();
  return &fGlobalData->fPulseIslandToChannelMap[bank];
}


//----------------------------------------------------------------------
void ExportPulse::ShowGuidance()
{
  // Only show guidance once
  if(fGuidanceShown) return;
  fGuidanceShown=true;
  cout<<"Requests to draw a pulse should be formatted as event(pulse),"<<endl;
  cout<<" where event and pulse are the numbers of each event and pulse you wish to draw"<<endl;
  //", a range (specified by A-Z, where A and Z are the first and last value of the range),"
  //" or a number of random events to select (specified by ~N where N is the number of events)";
}


//----------------------------------------------------------------------
bool ExportPulse::ParseEventRequest(std::string input, std::vector<EventID_t>& event_list)
{
  return ParseRequest(input,event_list,"event",0, GetTotalNumberOfEvents());
}


//----------------------------------------------------------------------
bool ExportPulse::ParsePulseRequest(std::string input, std::vector<EventID_t>& list)
{
  return ParseRequest(input,list,"pulse",0, GetTotalNumberOfEvents());
}


//----------------------------------------------------------------------
bool ExportPulse::ParseRequest(std::string input, std::vector<EventID_t>& list, const std::string& type, Long64_t lower_limit, Long64_t upper_limit)
{
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


//----------------------------------------------------------------------
void ExportPulse::LoadPulsesRequestedByConfig(){
  // 7) In Process entry, invoke a method to check if there were requested or a pulse on a channel in this event
  //    and load them into fTPIsToPlot;
  
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


//----------------------------------------------------------------------
void ExportPulse::ClearPulsesToExport(){
  ChannelPulseIDs_t::iterator i_channel;

  fTPIsToPlot.clear();
  fTAPsToPlot.clear();
}

ALCAP_REGISTER_MODULE(ExportPulse,subtract_pedestal,use_block_time,run_pulse_finder);
