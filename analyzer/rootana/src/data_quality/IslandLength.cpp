#include <iostream>
#include <utility>
#include "IslandLength.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"



//----------------------------------------------------------------------
IslandLength::IslandLength(modules::options* opts)
  : BaseModule("IslandLength",opts)
  , fDirName("IslandLength")
{

  // Do something with opts here.  Has the user specified any
  // particular configuration that you want to know?
  // For example, perhaps this module wants an axis range:
  //fXMax=opts->GetDouble("x_max",100); 
  
}


//----------------------------------------------------------------------
IslandLength::~IslandLength()
{
}


//----------------------------------------------------------------------
int IslandLength::BeforeFirstEntry(TGlobalData* data,TSetupData *setup)
{
  // Called before the main event loop
  // Can be used to set things up, like histograms etc
  // Return non-zero to indicate a problem

  // Print extra info if we're debugging this module:
  if(Debug()){
    std::cout<<"-----IslandLength::BeforeFirstEntry(): I'm debugging!"
             <<std::endl;
  }

  fNProcessed = 0;
  Book(data, setup);
  return 0;
}


//----------------------------------------------------------------------
int IslandLength::ProcessEntry(TGlobalData* data,TSetupData* setup)
{
  ++fNProcessed;

  // Called once for each event in the main event loop
  // Return non-zero to indicate a problem and terminate the event loop
  StringPulseIslandMap& banks = data->fPulseIslandToChannelMap;
  //typedef StringPulseIslandMap::iterator mapSP_iter;
  int entry_norm = -1;
  for (mapSP_iter biter = banks.begin(); biter != banks.end(); ++biter){
    std::string det_name = setup->GetDetectorName(biter->first);
    if (det_name == "muSc"){
      entry_norm = biter->second.size();
    }
  }  
  for (mapSP_iter biter = banks.begin(); biter != banks.end(); ++biter){
    std::string bank_name = biter->first;
    std::string det_name = setup->GetDetectorName(bank_name);
    PulseIslandList& islands = biter->second;
    
    TH1F* hist = fHistos.find(bank_name)->second;
    TH1F* histNorm = fHistosNorm.find(bank_name)->second;
   
    for (PIL_iter piter = islands.begin(); piter != islands.end(); ++piter){
      int length = (*piter)->GetSamples().size();
      hist->Fill(length);
      if (entry_norm!=0) histNorm->Fill(length, 1./entry_norm);
    }
  }

  return 0;
}


//----------------------------------------------------------------------
int IslandLength::AfterLastEntry(TGlobalData* gData,TSetupData *setup)
{
  // Return non-zero to indicate a problem
  if(Debug()){
    std::cout << "-----IslandLength::AfterLastEntry(): I'm debugging!"
              << std::endl;    
  }

  TDirectory* pwd = TDirectory::CurrentDirectory();
  fHistDir->cd();
  
  double run_norm = fNormHist->Integral(0,-1);
  for (mapSH_t::iterator it = fHistos.begin(); it != fHistos.end(); ++it){
    TH1F* h = it->second;
    TObject* obj = h->Clone((std::string(h->GetName()) + "_RunNorm").c_str());
    TH1* hn = static_cast<TH1*>(obj);
    hn->SetTitle((std::string(h->GetTitle()) + " (run normalised)").c_str());
    hn->Scale(1./run_norm);
  }
  for (mapSH_t::iterator it = fHistosNorm.begin(); it != fHistosNorm.end(); ++it){
    it->second->Scale(1./fNProcessed);
  }
  pwd->cd();

  return 0;
}

//----------------------------------------------------------------------
void IslandLength::Book(TGlobalData* data, TSetupData* setup)
{
  if (!TDirectory::CurrentDirectory()->cd(fDirName.c_str())){
    // this will blow up if the file is not writable.  Good!
    TDirectory::CurrentDirectory()->mkdir(fDirName.c_str())->cd();
  }
  fHistDir = TDirectory::CurrentDirectory();

  //Change: Use the same map as when processing entries. This should
  //guarentee that all histograms we try to fill also exist 
  StringPulseIslandMap& islands = data->fPulseIslandToChannelMap;
  for (mapSP_iter it = islands.begin(); it != islands.end(); ++it){

    std::string bank_name = it->first;
    std::string det_name = setup->GetDetectorName(bank_name);
    std::string hist_name = "hDQ_IslandLength_" + det_name + "" + bank_name;
    std::string hist_title = "Length of TPulseIslands in " + det_name;
    std::string axis_title = ";Length /samples;Number of Islands";
    TH1F* histo = new TH1F(hist_name.c_str(), (hist_title + axis_title).c_str(),
                           500, 0, 2000);
    fHistos[bank_name] = histo;
    if (det_name == "muSc") fNormHist = histo;

    hist_name += "_EvtNorm";
    hist_title += " (event normalised)";
    histo = new TH1F(hist_name.c_str(), (hist_title + axis_title).c_str(),
                     500, 0, 2000);
    fHistosNorm[bank_name] = histo;
  }
  TDirectory::CurrentDirectory()->cd("../");
}

//----------------------------------------------------------------------
// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly 
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(IslandLength);
