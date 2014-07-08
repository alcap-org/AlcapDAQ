#include "EventNavigator.h"

//C++/STL
#include <iostream>
#include <string>
#include <cmath>

//ROOT
#include "TFile.h"
#include "TTree.h"

//Local
#include "format.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "TMuonEvent.h"
#include "debug_tools.h"


namespace Format = AlCapFormat; 

TGlobalData* TGlobalData::Instance()
{
  return g_event;
}

//======================================================================
EventNavigator* EventNavigator::fInstance = 0x0;

//----------------------------------------------------------------------
EventNavigator& EventNavigator::Instance()
{
  if (!fInstance) fInstance = new EventNavigator();
  fInstance->fCopyRaw = true;
  return *fInstance;
}

//----------------------------------------------------------------------
EventNavigator::EventNavigator()
{
  //no-op
}


//----------------------------------------------------------------------
Bool_t EventNavigator::ConnectInput(const char* input_name)
{

  TFile* ifile = TFile::Open(input_name, "READ");
  if (!ifile) return false;
  TList* lok = ifile->GetListOfKeys();
  bool success_data = false;
  bool success_meta = false;
  fSetupTree = 0x0;
  if (lok->Contains(Format::Raw::SetupTreeName) ) {
    //Read setup data
    success_meta = true;
  }
  TTree* raw_tree = 0x0;
  if (lok->Contains(Format::Raw::DataTreeName) ) {
    TGlobalData* raw_data = ConnectRawData(ifile);
    //Read event data
    success_data = true;
  }
  //if (TODO: lok contians other trees)

  fEntryNo = 0;
  return success_data && success_meta;
}


//----------------------------------------------------------------------
TGlobalData* EventNavigator::ConnectRawData(TFile* raw_file)
{
  raw_file->GetObject(Format::Raw::DataTreeName, fRawTree);
  if ( !VerifyRawData(fRawTree) ){
    raw_file->Delete(Format::Raw::DataTreeName);
    fRawTree = 0x0;
    fRawData = 0x0;
    return 0x0;
  }

  //Allocate memory locations to hold branches
  //TGlobalData* raw_data(0x0);
  fRawData = new TGlobalData;
  int status = fRawTree->SetBranchAddress(Format::Raw::DataBranchName,
                                          &fRawData);
  if (status!=0){
    fRawTree = 0x0;
    fRawData = 0x0;
    return 0x0;
  }
  
  this->GetEntry(1);
  
  return fRawData;
}


//----------------------------------------------------------------------
Bool_t EventNavigator::VerifyRawData(TTree* raw_tree)
{
  if ( !raw_tree ) return false;
  if ( raw_tree->GetEntriesFast() == 0) return false;
  TObjArray* lob = raw_tree->GetListOfBranches(); 
  if ( !lob->Contains(Format::Raw::DataBranchName ) ) return false;
  
  return true;
}


//----------------------------------------------------------------------
Bool_t EventNavigator::ConnectOutput(const char* output_name, OutputMode mode)
{
  static const char* output_mode[3] = {"NEW", "RECREATE", "UPDATE"};
  TFile* ofile = TFile::Open(output_name, output_mode[mode]);
  if (!ofile) return false;
  
  fOutput = ofile;
  if (fRawTree && fRawData && fCopyRaw) MirrorRawInputFormat();
  fOutputTreeTPI->SetAutoSave(3000000);

  return true;
}


//----------------------------------------------------------------------
Bool_t EventNavigator::MirrorRawInputFormat()
{
  int prev_entry_no = fEntryNo;
  //loop untill we find an entry in which the object is filled (should
  //be the first one)
  int nBytes = 0;
  while (( nBytes = NextEntry() )){
    if (fRawData->fPulseIslandToChannelMap.size() > 0) break;
  }
  if (nBytes <= 0) return false;
 
  fOutputTreeTPI = new TTree("TPI","TPulseIslands");

  typedef StringPulseIslandMap::const_iterator raw_map_iter;
  StringPulseIslandMap& rawBanks = fRawData->fPulseIslandToChannelMap; 
  fBufferTPI = new PulseIslandList*[rawBanks.size()];

  //raw_map_iter b_it = fRawData->fPulseIslandToChannelMap.begin();
  Int_t element =0;
  for (raw_map_iter b_it = rawBanks.begin(); b_it != rawBanks.end(); ++b_it) {
    std::string bank_name = b_it->first;
    fBufferTPI[element] = new PulseIslandList;
    fRecordTPI.insert(make_pair(bank_name, fBufferTPI[element]));
    //fRecordTPI[bank_Name] = fBufferTPI[element]
    fOutputTreeTPI->Branch(bank_name.c_str(), &fBufferTPI[element]);
    ++element;
  }

  fOutputTreeTPI->Print();
  
  GetEntry(prev_entry_no);
}


//----------------------------------------------------------------------
Bool_t EventNavigator::ConnectInput(const char* input_file_name,
                                    Bool_t read_only)
{
  /*  //For now we  dont support identical output files
  if (read_only == false){
    std::cout << "WARN:  " << "re-writable files not supported!"
	      << std::endl;
    read_only = true;
  }


  //Not very careful about tidying up after failures or repeat attempts
  //here.  Probably doesen't matter since the program is likely to
  //abort shortly afterward.  Revisit if that is not true.

  //Open the file
  fRawInput = TFile::Open(input_file_name, (read_only ? "READ" : "UPDATE"));
  if (!fRawInput || fRawInput->IsZombie())  return false;

  //Look for setup tree
  TTree* setup_tree = 0x0;
  fRawInput->GetObject("SetupTree", setup_tree);
  if (!setup_tree || setup_tree->GetEntriesFast() == 0) return false;

  //Look for data tree
  TTree* event_tree = 0x0;
  fRawInput->GetObject("EventTree", event_tree);
  if (!event_tree || event_tree->GetEntriesFast() == 0) return false;

  //All set
  fSetupTree = setup_tree;
  fEventTree = event_tree;
  */
  return true;
  
}


//----------------------------------------------------------------------
Bool_t EventNavigator::ConnectOutputFile(const char* output_file_name,
					 Bool_t append)
{
  //Not very careful about tidying up after failures or repeat attempts
  //here.  Probably doesen't matter since the program is likely to
  //abort shortly afterward.  Revisit if that is not true.
  TFile* prev_output = fOutput;

  fOutput = TFile::Open(output_file_name, (append ? "UPDATE" : "RECREATE"));
  if (!fOutput || fOutput->IsZombie()) return false; 
  
  return true;
}

//----------------------------------------------------------------------
//Int_t EventNavigator::NextEntry()
//{
//  //  return LoadEntry(fEntryNo + 1);
//}


//----------------------------------------------------------------------
Int_t EventNavigator::LoadEntry(Long64_t entry)
{
  Int_t nBytes = 0;
  if (fRawTree){
    fRawData->Clear();
    nBytes = fRawTree->GetEntry(entry);
  } 
  else if (fEventTree) {
    nBytes = fEventTree->GetEntry(entry);
  }
  else return 0;
  if (nBytes < 0) throw io_error();
  fEntryNo = entry;
  if (fEntryNo % 100 != 10) return nBytes;
  std::cout << fEntryNo << "/" << fRawTree->GetEntriesFast() 
            << ":  " << fRawData->fPulseIslandToChannelMap.size() << std::endl;
  //std::cout << DEBUG::check_mem().str << std::endl;;  
  return nBytes;
}

//----------------------------------------------------------------------
Int_t EventNavigator::WriteCurrentEntry()
{
  fOutputTreeTPI->Fill();
}

extern void ClearGlobalData(TGlobalData* data);
//----------------------------------------------------------------------
void EventNavigator::CopyTree()
{
  //DEBUG_PRINT
  Long64_t nentries = fEventTree->GetEntries();
  //nentries = 100;
  TGlobalData* event = new TGlobalData();
  fEventTree->SetBranchAddress("Event", &event);
  fEventTree->GetDirectory()->Print();

  //DEBUG_PRINT

  fOutput->ls();
  gFile->ls();
  TTree* out_tree = new TTree("output_tree", "out");//fEventTree->CloneTree(0);
  TTree* out_tree2 = new TTree("friend", "Friendly tree");
  out_tree->SetName("ouput_event");
  out_tree2->SetName("friendly_event");
  Int_t something; Int_t something_else; Float_t ratio;
  out_tree->Branch("Event", &event);
  out_tree->Branch("demo",&something,"demo/I");
  out_tree->SetAutoSave(1000000);
  out_tree->AddFriend(out_tree2);
  out_tree2->Branch("mode",&something_else,"mode/I");
  out_tree2->Branch("R",&ratio,"R/F");
  out_tree2->SetAutoSave(1000000);
  
  Long64_t i;
  DEBUG::check_clock();
  for (/*Long64_t*/ i=0; i < nentries; ++i){
    //std::cout << "on entry" << i << "/" << nentries << std::endl;
    //fEventTree->GetEntry(20);
    fEventTree->GetEntry(i);
    something = i % 25;
    something_else = i % 7;
    ratio = (1. * something) / (1+something_else);
    out_tree->Fill();
    out_tree2->Fill();
    //out_tree->AutoSave();
    out_tree2->AutoSave();
    
    if ( i ==    1 || i ==    2 || i ==    5 ||
	 i ==   10 || i ==   20 || i ==   50 ||
	 i ==  100 || i ==  200 || i ==  500 ||
	 i % 1000 == 0 ) {
      std::cout << "Event " << i  
		<< ": \t" << DEBUG::check_clock().str
		<< "\t" << DEBUG::check_mem().str 
		<< std::endl;
      
    }
    //std::cout << "map size: "<<event->fPulseIslandToChannelMap.size() << std::endl;
    typedef std::vector<TPulseIsland*> PI_list_t;
    typedef std::map<std::string, PI_list_t> PI2CM_t;
    typedef PI2CM_t::iterator PI2C_iter_t;
    int npi  =0; int npi2 = 0;
    for (PI2C_iter_t it = event->fPulseIslandToChannelMap.begin();
	 it != event->fPulseIslandToChannelMap.end(); ++it){
      npi += it->second.size();
      PI_list_t& bank = it->second;
      for (int pui = 0; pui < bank.size() ; ++pui){
	delete bank[pui];
      }
      bank.clear();
      npi2 +=  it->second.size();
      //typedef std::vector<int> PIdata_t; 
      //PIdata_t samples = 
      //for (int s =  
      
    }
    //std::cout << "N islands: "<< npi << std::endl;
    //std::cout << "After del: "<< npi2 << std::endl;
    
    
    //event->Clear();
    //ClearGlobalData(event);
    fEventTree->Clear();
    out_tree->Clear();
    out_tree2->Clear();
  }
  std::cout << nentries << " Entries" << std::endl;
  delete event;
  out_tree->Print();
  out_tree->AutoSave();
  out_tree2->Print();
  out_tree2->AutoSave();
  
  fOutput->ls();
  fOutput->Close();
  //DEBUG_PRINT

}
