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


  /// Adoption: take ownership of a container of reconstruction
  /// objects. The calling code should lose ownership so the object is
  /// frozen at adoption.  Implemetaion could use a swaped body. 
  void Adopt(AnalysedPulseList apl, SourceID did, GeneratorID gid);
  void Adopt(DetectorPulseList dpl, SourceID did, GeneratorID gid);
  //void Adopt(MuonEventList mel, SourceID did, GeneratorID gid );
  

  /// Read-only getters for an individual bank.  
  /// Need to work out how to deal with read-mode and write-mode trees
  const AnalysedPulseList& GetAnalysedBank(SourceID did, 
					   GeneratorID gid = "",
					   int count = 0) const;

  const DetectorPulseList& GetDetectorBank(SourceID did,
					   GeneratorID gid = "",
					   int count = 0) const;

  //const MuonEventList& GetMuonEventList(GeneratorID gid = "",
  //                                      int count = 0) const;

  ///Iterable getters for a list of banks
  

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
