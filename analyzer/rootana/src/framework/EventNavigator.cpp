#include "EventNavigator.h"

#undef DEFER

//C++/STL
#include <iostream>
#include <string>
#include <cmath>

//ROOT
#include "TFile.h"
#include "TTree.h"

//Local
#include "format.h"
#include "SetupRecord.h"
#include "BankBranch.h"
#include "TPulseIsland.h"
#include "TAnalysedPulse.h"
#include "TDetectorPulse.h"
#include "TMuonEvent.h"
#include "debug_tools.h"
#include "CommandLine.h"

namespace Format = AlCapFormat; 

//TGlobalData* TGlobalData::Instance()
//{
//  return g_event;
//}

//======================================================================
EventNavigator* EventNavigator::fInstance = 0x0;

//----------------------------------------------------------------------
EventNavigator& EventNavigator::InitInstance(const ARGUMENTS& command_line)
{
  if (fInstance) {
    std::cout << "WARNING: "
              << "Instance has already been created and will not be altered"
              << std::endl;
  }
  else {
    fInstance = new EventNavigator(command_line);
    //fInstance->fCopyRaw = true;
  }
  return *fInstance;
}
//----------------------------------------------------------------------
EventNavigator& EventNavigator::Instance()
{
  if (!fInstance){
    fInstance = new EventNavigator();
  }
  return *fInstance;
}

//----------------------------------------------------------------------
EventNavigator::EventNavigator()
  : fCopyRaw(true)
  , fLoopSequence(0x0)
{
  fRawInput = 0x0;
  fOutput = 0x0;
  //no-op
}

//----------------------------------------------------------------------
EventNavigator::EventNavigator(const ARGUMENTS& args)
  : fCopyRaw(true)
  , fRawInput(0x0)
  , fOutput(0x0)
  , fLoopSequence(new LoopSequence(args))
{
}


//----------------------------------------------------------------------
const LoopSequence& EventNavigator::GetLoopSequence() const
{
  if (!fLoopSequence) throw std::runtime_error("No LoopSequence");
  return *fLoopSequence;
}

//----------------------------------------------------------------------
const LoopSequence& EventNavigator::MakeLoopSequence(const ARGUMENTS& args)
{
  //Not sure if allowing this to be replaced is safe. Lets err on the
  //side of caution and make it a runtime error for now.
  if (fLoopSequence) throw std::runtime_error("Redfined LoopSequence");
  fLoopSequence = new LoopSequence(args);
  return GetLoopSequence();
}

//----------------------------------------------------------------------
const char* EventNavigator::GetOutputFileName()const{
  return fOutput? fOutput->GetName():"";
}

//----------------------------------------------------------------------
Long64_t EventNavigator::GetStartEntry() const
{
  return GetLoopSequence().StartEntry();
}  


//----------------------------------------------------------------------
Long64_t EventNavigator::GetStopEntry() const
{
  return GetLoopSequence().StopEntry();
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
    if ( ConnectSetupData(ifile) != 0x0)
      success_meta = true;
  }
  //TTree* raw_tree = 0x0;
  if (lok->Contains(Format::Raw::DataTreeName) ) {
    if ( ConnectRawData(ifile) != 0x0)
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
  
  this->LoadEntry(0);
  
  return fRawData;
}


//----------------------------------------------------------------------
Bool_t EventNavigator::VerifyRawData(TTree* raw_tree)
{
  if ( !raw_tree ){
    std::cerr << "No raw data tree in file" << std::endl;
    return false;
  }
  if ( raw_tree->GetEntriesFast() == 0 ){
    std::cerr << "Tree " << raw_tree->GetName() << " is empty!" 
              << std::endl;
    return false;
  }
  TObjArray* lob = raw_tree->GetListOfBranches(); 
  if ( !lob->Contains(Format::Raw::DataBranchName ) ){
    std::cerr << "Tree " << raw_tree->GetName()
              << " has no branch " << Format::Raw::SetupBranchName
              << "!" << std::endl;    
    return false;
  }
  return true;
}

template<typename T, unsigned int& I> T ADV(T t) {
  if (I == 0) return t;
  return ADV<T,I-1>(++t);
};


//----------------------------------------------------------------------
TSetupData* EventNavigator::ConnectSetupData(TFile* raw_file)
{
  raw_file->GetObject(Format::Raw::SetupTreeName, fSetupTree);
  if ( !VerifySetupData(fSetupTree) ){
    raw_file->Delete(Format::Raw::SetupTreeName);
    fSetupData = 0x0;
    fSetupTree = 0x0;
    return 0x0;
  }
  //Allocate memory locations to hold branches
  fSetupData = new TSetupData;
  int status = fSetupTree->SetBranchAddress(Format::Raw::SetupBranchName,
                                            &fSetupData);
  if (status!=0){
    fSetupTree = 0x0;
    fSetupData = 0x0;
    return 0x0;
  }

  fSetupTree->GetEntry(0);
  fSetupRecord = new SetupRecord(fSetupData);
  return fSetupData;

}


//----------------------------------------------------------------------
Bool_t EventNavigator::VerifySetupData(TTree* setup_tree)
{
  if ( !setup_tree ){
    std::cerr << "No 'Setup' tree in file" << std::endl;
    return false;
  }
  if ( setup_tree->GetEntriesFast() == 0 ){
    std::cerr << "Tree " << setup_tree->GetName() << " is empty!" 
              << std::endl;
    return false;
  }
  TObjArray* lob = setup_tree->GetListOfBranches();
  if ( !lob->Contains(Format::Raw::SetupBranchName ) ){
    std::cerr << "Tree " << setup_tree->GetName() 
              << " has no branch " << Format::Raw::SetupBranchName
              << "!" << std::endl;
    return false;
  }
  return true;
}


//----------------------------------------------------------------------
Bool_t EventNavigator::ConnectOutput(const char* output_name, OutputMode mode)
{
  static const char* output_mode[3] = {"NEW", "RECREATE", "UPDATE"};
  TFile* ofile = TFile::Open(output_name, output_mode[mode]);
  if (!ofile) return false;
  
  fOutput = ofile;
#ifdef DEFER
  if (fRawTree && fRawData && fCopyRaw) MirrorRawInputFormat();
#endif

  return true;
}


//----------------------------------------------------------------------
Bool_t EventNavigator::MirrorRawInputFormat()
{
  int prev_entry_no = fEntryNo;
  //loop untill we find an entry in which the object is filled (should
  //be the first one)
  int nBytes = 0;
  int entry = fEntryNo;
  while (( nBytes = LoadEntry(++entry) )){
    if (fRawData->fPulseIslandToChannelMap.size() > 0) break;
  }
  if (nBytes <= 0) return false;
 
  fOutputTreeTPI = new TTree("TPI","TPulseIslands");
  fOutputTreeTPI->SetAutoSave(3000000);

  typedef StringPulseIslandMap::const_iterator raw_map_iter;
  StringPulseIslandMap& rawBanks = fRawData->fPulseIslandToChannelMap; 
  //fBufferTPI = new PulseIslandList*[rawBanks.size()];

  TObjArray* obj_arr = new TObjArray(rawBanks.size());
  //obj_arr = new TObjArray(rawBanks.size());
  //raw_map_iter b_it = fRawData->fPulseIslandToChannelMap.begin();
  Int_t element =0;
  for (raw_map_iter b_it = rawBanks.begin(); b_it != rawBanks.end(); ++b_it) {
    std::string bank_name = b_it->first;
    //fBufferTPI[element] = new PulseIslandList;
    //fRecordTPI.insert(make_pair(bank_name, fBufferTPI[element]));
    //fRecordTPI[bank_Name] = fBufferTPI[element]
    //fOutputTreeTPI->Branch(bank_name.c_str(), &fBufferTPI[element]);
    ++element;
    
    //TNamed* foo = new TNamed((bank_name + "_name").c_str(), (bank_name + "_title").c_str());
    BankBranch<PulseIslandList>* foo = 
      new BankBranch<PulseIslandList>(IDs::generator(bank_name));
    obj_arr->Add(foo);
  }
  fOutputTreeTPI->Branch(obj_arr);

  fOutputTreeTPI->Print();
 
  fOutputTreeTPI->Write();
  GetEntry(prev_entry_no);
  return true;
}


//----------------------------------------------------------------------
#ifdef DEFER
const PulseIslandList& EventNavigator::FindIslandBank(const SourceID& sid) const{
  if (sid.isWildCard() ) return PulseIslandList(0);
  else return PulseIslandList(0);
}
#endif

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
  
  prev_output->cd();
  return true;
}

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
  return nBytes;
}

//----------------------------------------------------------------------
Int_t EventNavigator::WriteCurrentEntry()
{
  fOutputTreeTPI->Fill();
  return 0;
}

//----------------------------------------------------------------------
void EventNavigator::Close()
{
  if (fRawInput) fRawInput->Close();
  if (fOutput) fOutput->Close();
}

//======================================================================
extern void ClearGlobalData(TGlobalData* data);


//======================================================================

/// @deprecated This method currently needs to exist for the benefit
/// of online classes that expect to be able to get a non-const
/// pointer this way, but it may be removed in the future.  Offline
/// code should use EventNavigator::Instance().GetSetupData()
/// directly.
TSetupData* TSetupData::Instance(){
  return const_cast<TSetupData*>(EventNavigator::Instance().GetSetupData());
}
