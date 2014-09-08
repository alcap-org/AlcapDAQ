#ifndef EVENTNAVIGATOR_H
#define EVENTNAVIGATOR_H


//C++/STL
#include <stdexcept>
#undef DEFER
//ROOT
class TFile;
class TTree;

//Local
#include "definitions.h"
#include "TGlobalData.h"
#include "BankIter.h"
#include "SetupRecord.h"
#include "LoopSequence.h"


///This shoud get a proper home and some more flexability
class io_error : public std::runtime_error {
public:
  io_error() 
    : std::runtime_error("I/O failed")  
  {}
};


class ARGUMENTS;


/// @brief The EventNavigator class provides acess to all the collections in
/// the (MIDAS) Event, which corresponds to a single ROOT branch
///
/// @section Object Ownership
///
/// The general principle is that the event navigator owns all objects
/// that are read or written to disk. Thus....

class EventNavigator {
  typedef IDs::channel ChannelID;
  typedef IDs::generator GeneratorID;
  typedef IDs::source SourceID;
  typedef IDs::Generator_t GeneratorStr;
  typedef IDs::Config_t Config;


 public:
  enum OutputMode {kNew = 0, kOverwrite }; //, kAppend};

  //The initial 
  static EventNavigator& InitInstance(const ARGUMENTS& command_line);

  /// The user accessor. Not sure yet if it can be used without
  /// providing initalisation info.
  static EventNavigator& Instance();

  ~EventNavigator(){}
  
  /// Transitional function to replace gSetup.  
  /// Users should use the GetSetupRecord interface. 
  const TSetupData* const GetSetupData() {return fSetupData;}

  /// Gives access to the run meta-data, including all information
  /// stored in the TSetupData struct
  const SetupRecord& GetSetupRecord() {return *fSetupRecord;}

  /// Gives read access to the LoopSequence, which must first be
  /// initailised with MakeLoopSequence() in main()
  const LoopSequence& GetLoopSequence() const;

  const char* GetOutputFileName()const;

  /// Initialises the LoopSequence this must be called once beforit is
  /// used.  Calling it a second time is undefined behavior (currently
  /// forces an exception)
  const LoopSequence& MakeLoopSequence(const ARGUMENTS& args);

  /// Opens an input file and connects to the trees therein.  Returns
  /// true if file exists, is a ROOT file, and contains at least one EACH of 
  /// setup and event trees that we recognise. Else false.
  Bool_t ConnectInput(const char* input_name);

  
  /// Opens an output file to put trees and histograms in.  By default
  /// this overites any existing file with the same name but we can
  /// change it to fail instead.
  Bool_t ConnectOutput(const char* output_name, OutputMode mode = kOverwrite);

  /// Opens an output file.  By default this overwrites the output file
  /// Appending not yet implemented
  Bool_t ConnectOutputFile(const char* output_file_name, Bool_t append =false);

  /// Returns the current event number
  Long64_t EntryNo() const {return fEntryNo;}

  /// Returns the number of entries in the input file.
  /// TODO revisit this with multiple inputs??
  Long64_t GetInputNEntries() const {return fRawTree->GetEntriesFast();}

  /// Returns the entry processing starts from. 
  Long64_t GetStartEntry() const;
  
  /// Returns the past-the-end entry processing stops at. 
  Long64_t GetStopEntry() const;

  /// Returns the total number of entries selected for processing,
  /// assuming no errors
  Long64_t GetLoopNEntries() const
  { return GetStopEntry() - GetStartEntry(); }
  

  /// Load the next entry in the input tree. Returns the number of
  /// bytes if sucessful, 0 if reached the end or there is no input tree.
  /// Throws an exception for an underlying I/O error
  inline Int_t NextEntry() {
    return LoadEntry(fEntryNo + 1);
  }

  /// Load the branches for a particular entry from the input
  /// file. Random access probably only make sense if it output is not
  /// being written.  Return values are same as for NextEntry()
  inline Int_t GetEntry(Long64_t entry){
    return LoadEntry(entry);
  }

  
  Int_t WriteCurrentEntry();

  /// Save (or not) TPulseIslands in output file. By default they are
  /// not saved, call this method to enable saving.
  void SetSavePulseIslands(bool save = 1){fCopyRaw =save;};

  /// Adoption: take ownership of a container of reconstruction
  /// objects. The calling code should lose ownership so the object is
  /// frozen at adoption.  Implemetaion could use a swaped body. 
  void Adopt(PulseIslandList& pil, const SourceID& sid);
  void Adopt(AnalysedPulseList& apl, const SourceID& sid);
  void Adopt(DetectorPulseList& dpl, const SourceID& sid);
  //void Adopt(MuonEventList& mel, const SourceID& sid);
  ///This will probably become protected, for copying TAPs
  void Adopt(PulseIslandList& pil, const ChannelID& cid);
  

  /// Adoption of temporaries: As Adopt(), but the adopted objects
  /// expire and are deleted at the end of the current event.
  void AdoptTemporary(PulseIslandList& pil, const SourceID& sid);
  void AdoptTemporary(AnalysedPulseList& apl, const SourceID& sid);
  void AdoptTemporary(DetectorPulseList& dpl, const SourceID& sid); 
  //void AdoptTemporary(MuonEventList& mel, const SourceID& sid);
  ///This will probably become protected, for copying TAPs
  void AdoptTemporary(PulseIslandList& pil, const ChannelID& cid); 

#ifdef DEFER
  /// Read-only getters for an individual bank.  
  /// Need to work out how to deal with read-mode and write-mode trees
  const PulseIslandList& FindIslandBank(const SourceID& sid) const;
  const PulseIslandList& FindAnalysedBank(const SourceID& sid) const;
  const PulseIslandList& FindDetectorBank(const SourceID& sid) const;
#endif 

  //TODO 
  // What shortcuts alowing strings/enums are sensible & useful? 
  // If any?
  /*
  const PulseIslandList& FindIslandBank(ChannelID& cid,
					GeneratorID& gid = "*");
  const PulseIslandList& FindIslandBank(const ChannelID& cid,
					const GeneratorID& gid);
  const AnalysedPulseList& FindAnalysedBank(ChannelID cid, 
					    GeneratorStr gid = "*",
					    Config cfg ="*") const;
  const PulseIslandList& FindIslandBank(const ChannelID& cid,
					const GeneratorID& gid);
  const DetectorPulseList& FindDetectorBank(ChannelID cid,
					    GeneratorStr gid = "*",
					    Config cfg ="*" ) const;  
  //const MuonEventList& GetMuonEventList(GeneratorID gid = "",
  //                                      int count = 0) const;
  */

  ///Iterable getters for a list of banks
  PulseIslandBankIter MatchIslandBanks(const SourceID& sid) const;
  PulseIslandBankIter MatchAnalysedBanks(const SourceID& sid) const;
  PulseIslandBankIter MatchDetectorBanks(const SourceID& sid) const;

  PulseIslandBankIter MatchIslandBanks(const ChannelID& cid,
				       GeneratorStr gid ="") const;

  AnalysedPulseBankIter MatchAnalysedBanks(const ChannelID& cid,
					   GeneratorStr gid ="") const;
  
  AnalysedPulseBankIter MatchDetectorBanks(const ChannelID& cid,
					   GeneratorStr gid ="") const;

  void Close();

  TGlobalData* GetRawData() {return fRawData;}
  TTree* GetRawTree() {return fRawTree;}

protected:
  ///Attach a raw data tree (found in raw_file). If sucessful will
  ///load the zeroth entry return a non-null pointer
  TGlobalData* ConnectRawData(TFile* raw_file);

  ///Verifies that the raw data tree found conforms to the expected
  ///format.
  Bool_t VerifyRawData(TTree* raw_tree);

  ///Attach a setup data tree (found in raw_file). If sucessful will
  ///load the SetupDAta into memory, which can be accesed via
  ///fSetupData (depricated) or fSetupRecord (preferred)
  TSetupData* ConnectSetupData(TFile* raw_file);

  ///Verifies that the setup data tree found conforms to the expected
  ///format.
  Bool_t VerifySetupData(TTree* setup_tree);


  ///Copy the input raw data tree (TGlobalData) to the output file
  Bool_t MirrorRawInputFormat();

  ///Implementation for loading an entry
  Int_t LoadEntry(Long64_t entry);


private: 
  EventNavigator();
  EventNavigator(const ARGUMENTS& command_line);
  EventNavigator(const EventNavigator& src){};
  EventNavigator& operator=(const EventNavigator& rhs){return *this;}

  static EventNavigator* fInstance; 

  Bool_t fCopyRaw;

  ///Input ROOT file, raw format
  TFile* fRawInput;

  ///Output ROOT file (may be same as input file)
  TFile* fOutput;

  ///The current entry number
  Int_t fEntryNo;

  ///Input Raw Data (Event) Tree
  TTree* fRawTree;

  ///Input Setup Tree
  TTree* fSetupTree;

  ///
  TTree* fEventTree;

  TTree* fOutputTreeTPI;

  TGlobalData* fRawData;
  
  TSetupData* fSetupData;

  SetupRecord* fSetupRecord;

  LoopSequence* fLoopSequence;


};


//inline Int_t EventNavigator::NextEntry() {return LoadEntry(fEntryNo + 1);}

#endif //EVENTNAVIGATOR_H

// emacs metadata
// Local Variables:
// mode: c++
// End:
