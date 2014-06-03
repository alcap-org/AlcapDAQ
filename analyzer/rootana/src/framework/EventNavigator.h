#ifndef EVENTNAVIGATOR_H
#define EVENTNAVIGATOR_H


//C++/STL

//ROOT
class TFile;
class TTree;

//Local
#include "definitions.h"
#include "TGlobalData.h"
#include "BankIter.h"

static TGlobalData *g_event=NULL; 

/// The EventNavigator class provides acess to all the collections in
/// the (MIDAS) Event, which corresponds to a single ROOT branch  

class EventNavigator {
 public:
  /// The user accessor. Not sure yet if it can be used without
  /// providing initalisation info.
  static EventNavigator& Instance();
  ~EventNavigator();
  
  /// Opens an input file and connects to the trees therein.  Returns
  /// true if file exists, is a ROOT file, and contains at least one EACH of 
  /// setup and event trees that we recognise. Else false.
  /// Request for read+write files (i.e. read_only=0) currently ignored.
  Bool_t ConnectInput(const char* input_file_name, Bool_t read_only =true);
  
  /// Opens an output file.  By default this overwrites the output file
  /// Appending not yet implemented
  Bool_t ConnectOutputFile(const char* output_file_name, Bool_t append =false);

  /// Returns the current event number
  Long64_t EntryNo();

  /// Load the next entry in the input tree. Returns the number of
  /// bytes if sucessful, 0 if reached the end, and -1 for an I/O error
  /// [Return values are from TTree::GetEntry()]
  Int_t NextEntry();
  
  /// Load the branches for a particular entry from the input
  /// file. Will probably only make sense if output is not being written.
  /// Return values are same as for NextEntry()
  Int_t GetEntry(Long64_t entry);

  /// Save (or not) TPulseIslands in output file. By default they are
  /// not saved, call this method to enable saving.
  void SetSavePulseIslands(bool save = 1);

  /// Adoption: take ownership of a container of reconstruction
  /// objects. The calling code should lose ownership so the object is
  /// frozen at adoption.  Implemetaion could use a swaped body. 
  void Adopt(PulseIslandList pil, ChannelID cid, GeneratorID gid,
	     GeneratorConfig cfg);
  void Adopt(AnalysedPulseList apl, ChannelID cid, GeneratorID gid,
	     GeneratorConfig cfg);
  void Adopt(DetectorPulseList dpl, ChannelID cid, GeneratorID gid,
	     GeneratorConfig cfg);
  //void Adopt(MuonEventList mel, ChannelID cid, GeneratorID gid,
  //           GeneratorConfig cfg);
  

  /// Adoption of temporaries: As Adopt(), but the adopted objects
  /// expire and are deleted at the end of the current event.
  void AdoptTemporary(PulseIslandList pil, ChannelID cid, GeneratorID gid,
	     GeneratorConfig cfg);
  void AdoptTemporary(AnalysedPulseList apl, ChannelID cid, GeneratorID gid,
	     GeneratorConfig cfg);
  void AdoptTemporary(DetectorPulseList dpl, ChannelID cid, GeneratorID gid,
	     GeneratorConfig cfg);
  //void Adopt(MuonEventList mel, ChannelID cid, GeneratorID gid,
  //           GeneratorConfig cfg);
  

  /// Read-only getters for an individual bank.  
  /// Need to work out how to deal with read-mode and write-mode trees
  const PulseIslandList& FindIslandBank(ChannelID cid,
					GeneratorID gid = "",
					GeneratorConfig cfg ="") const;
				       
  const AnalysedPulseList& FindAnalysedBank(ChannelID cid, 
					    GeneratorID gid = "",
					    GeneratorConfig cfg ="") const;
  
  const DetectorPulseList& FindDetectorBank(ChannelID cid,
					    GeneratorID gid = "",
					    GeneratorConfig cfg ="" ) const;
  
  //const MuonEventList& GetMuonEventList(GeneratorID gid = "",
  //                                      int count = 0) const;

  ///Iterable getters for a list of banks
  PulseIslandBankIter MatchIslandBanks(ChannelID cid,
				       GeneratorID gid ="") const;

  AnalysedPulseBankIter MatchAnalysedBanks(ChannelID cid,
					   GeneratorID gid ="") const;
  
  AnalysedPulseBankIter MatchDetectorBanks(ChannelID cid,
					   GeneratorID gid ="") const;

  
  
  ///A test
  void CopyTree();

 private: 
  EventNavigator();
  EventNavigator(const EventNavigator& src){};
  EventNavigator& operator=(const EventNavigator& rhs){return *this;}

  static EventNavigator* fInstance; 

  ///Input ROOT file
  TFile* fInput;

  ///Output ROOT file (may be same as input file)
  TFile* fOutput;

  ///Input Setup Tree
  TTree* fSetupTree;

  ///Input Data (Event) Tree
  TTree* fEventTree;
};



#endif //EVENTNAVIGATOR_H
