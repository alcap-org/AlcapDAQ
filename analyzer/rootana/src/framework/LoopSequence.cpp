#include "LoopSequence.h"


//C++/STL
#include <cmath>
#include <iomanip>

//ROOT
#include "Rtypes.h"

//Local
#include "EventNavigator.h"
#include "ModulesNavigator.h"
#include "BaseModule.h"
#include "AlcapExcept.h"

extern SourceAnalPulseMap gAnalysedPulseMap;
//extern StringDetPulseMap gDetectorPulseMap;

extern void ClearGlobalData(TGlobalData* data);

//----------------------------------------------------------------------
///This shoud get a proper home and some more flexability, perhaps a
///nicer name. Also consider splittin into muiltiple classes.
class module_error : public std::runtime_error {
public:
  module_error(int evt,BaseModule* mod) 
    : std::runtime_error("")
    , fEvent(evt), fModule(mod)
  {}
  int fEvent;
  BaseModule* fModule;
};

//----------------------------------------------------------------------
class preprocess_error : public module_error {
public:
  preprocess_error(int evt,BaseModule* mod)
    : module_error(evt,mod)
  {}
};

//----------------------------------------------------------------------
class process_error : public module_error {
public:
  process_error(int evt,BaseModule* mod)
    : module_error(evt,mod)
  {}
};

//----------------------------------------------------------------------
class postprocess_error : public module_error {
public:
  postprocess_error(int evt,BaseModule* mod)
    : module_error(evt,mod)
  {}
};




//======================================================================
void LoopSequence::Init()
{
  // How many entries should we loop over?
  Long64_t nEntries = EventNavigator::Instance().GetInputNEntries();
  bool has_start = (fArgs.start > 0) && (fArgs.start < nEntries);
  fStart = (has_start) ? fArgs.start : 0;
  bool has_stop = (fArgs.stop > 0) && (fArgs.stop < nEntries);
  fStop = (has_stop) ? fArgs.stop : nEntries;
}


//----------------------------------------------------------------------
void LoopSequence::Preprocess() const
{
  EventNavigator& enav = EventNavigator::Instance();
  enav.GetEntry(fStart);
  int err_code =0;
  modules::navigator& mn = *modules::navigator::Instance();
  BaseModule* mod =NULL;
  for (modules::iterator it = mn.Begin(); it != mn.End() && !err_code; ++it) {
    mod = it->second;
    err_code |= mod->Preprocess(enav.GetRawData(), enav.GetSetupData());
  }
  if (err_code) throw preprocess_error(fStart,mod);
}


//----------------------------------------------------------------------
void LoopSequence::Process() const
{
  EventNavigator& enav = EventNavigator::Instance();
  BaseModule* mod =NULL;
  for (Long64_t jentry = fStart; jentry < fStop; ++jentry){
    //This to move unrder event navigator
    TGlobalData* raw_data = enav.GetRawData();
    if (raw_data){
      raw_data->Clear("C");
      ClearGlobalData(raw_data);
    }
    
    Checkpoint(jentry);
    
    enav.GetEntry(jentry);
    int err_code = 0;
    modules::navigator& mn = *modules::navigator::Instance();
    for (modules::iterator it = mn.Begin(); it != mn.End() && ! err_code; ++it) {
      mod = it->second;
      err_code |= mod->ProcessGenericEntry(raw_data,enav.GetSetupData());
    }
    if (err_code) throw process_error(jentry,mod);
  
  }
  return;
}


//----------------------------------------------------------------------
void LoopSequence::Postprocess() const
{
  EventNavigator& enav = EventNavigator::Instance();
  int err_code =0;
  modules::navigator& mn = *modules::navigator::Instance();
  BaseModule* mod =NULL;
  for (modules::iterator it = mn.Begin(); it != mn.End() && !err_code; ++it) {
    mod = it->second;
    err_code |= mod->Postprocess(enav.GetRawData(), enav.GetSetupData());
  }
  if ( err_code ) throw postprocess_error(fStop-1,mod);
}


//----------------------------------------------------------------------
void LoopSequence::Run() const
{
  try {
    this->Preprocess();
    this->Process();
    this->Postprocess();
  }
  catch (preprocess_error& e){
    std::cout << "\nError while preprocessing first entry (" 
              << e.fEvent << ")";
    if(e.fModule) std::cout<<" for module "<<e.fModule->GetName();
  }
  catch (process_error& e){
    try{
        // try to let each module finish to save plots etc
        this->Postprocess();
    } catch(...) {
        // if postprocess throws an error assume it's related to the process
        // error
        if(e.fModule) std::cout<<"\nModule "<<e.fModule->GetName();
        else std::cout << "\nA module";
        std::cout<<" returned non-zero on entry " << e.fEvent;
        throw;
    }
    if(e.fModule) std::cout<<"\nModule "<<e.fModule->GetName();
    else std::cout << "\nA module";
    std::cout<<" returned non-zero on entry " << e.fEvent;
  }
  catch (postprocess_error& e){
    if(e.fModule) std::cout<<"\nModule "<<e.fModule->GetName();
    else std::cout << "\nA module";
    std::cout<<" returned non-zero on during post-processing last entry " << e.fEvent;
  }
  catch (module_error& e){
    if(e.fModule) std::cout<<"\nModule "<<e.fModule->GetName();
    else std::cout << "\nA module";
    std::cout<<" returned non-zero on during pre-processing first entry " << e.fEvent;
  }
  catch (Except::Base& e){
      std::cout<<"Alcap exception was thrown: "<<std::endl;
      std::cout<<e.what()<<std::endl;
      std::cout<<e.bt()<<std::endl;
  }
  std::cout << std::endl;
}  


//----------------------------------------------------------------------
void LoopSequence::Checkpoint(Long64_t entry) const
{
  Long64_t n = entry-fStart;
  static Long64_t nTot = fStop-fStart;
  static int nDig = 1+std::floor(std::log10(nTot));
  if ( n  < 10 || n %(nTot/10)==0 ) {
    std::cout << "Processed  " << std::setw(nDig) << n 
              << "/" << std::setw(nDig) << nTot
              << "  (" <<std::setw(2)<< std::floor((100.*n) / nTot) 
              << "%)" << std::endl;  
  }
  return;
}


/*
//----------------------------------------------------------------------
void LoopSequence::ClearGlobalData(TGlobalData* data)
{
  // TODO: this comment is now gibberish.
  // We could put this into TGlobalData::Clear(), but we need
  // to be sure that's okay at the alcapana level. That is, if
  // Clear() deletes the TPulseIsland objects, but g_event doesn't
  // own the pulses, they would be deleted later. A solution is to
  // be sure that TGlobalData isn't called in alcapana, or ensure
  // that g_event owns the pulse islands at that level.  
  StringPulseIslandMap::iterator mapIter;
  StringPulseIslandMap::iterator mapEnd = data->fPulseIslandToChannelMap.end();
  for(mapIter = data->fPulseIslandToChannelMap.begin(); mapIter != mapEnd; mapIter++) {
    // The iterator is pointing to a pair<string, vector<TPulseIsland*> >
    std::vector<TPulseIsland*>& pulse_vector= mapIter->second;
    for(size_t i=0; i<pulse_vector.size(); i++){
      delete pulse_vector[i];
      pulse_vector[i] = NULL;
    }
    pulse_vector.clear();
  }
  
  
  for(StringAnalPulseMap::iterator mapIter=gAnalysedPulseMap.begin();
      mapIter != gAnalysedPulseMap.end(); mapIter++) {
    
    // The iterator is pointing to a pair<string, vector<TPulseIsland*> >
    AnalysedPulseList& pulse_vector= mapIter->second;
    for(size_t i=0; i<pulse_vector.size(); i++){
      delete pulse_vector[i];
      pulse_vector[i] = NULL;
    }
    pulse_vector.clear();
  }
  gAnalysedPulseMap.clear();
  
  for(StringDetPulseMap::iterator mapIter = gDetectorPulseMap.begin(); mapIter != gDetectorPulseMap.end(); mapIter++) {
    // The iterator is pointing to a pair<string, vector<TPulseIsland*> >
    std::vector<TDetectorPulse*>& pulse_vector= mapIter->second;
    for(size_t i=0; i<pulse_vector.size(); i++){
      delete pulse_vector[i];
      pulse_vector[i] = NULL;
    }
    pulse_vector.clear();
  }
  gDetectorPulseMap.clear();
}

*/
