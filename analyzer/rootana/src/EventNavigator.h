#ifndef EVENTNAVIGATOR_H
#define EVENTNAVIGATOR_H

#include "TGlobalData.h"

//C++/STL

//ROOT
class TFile;
class TTree;

//Local


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
  Bool_t ConnectInputFile(const char* input_file_name, Bool_t read_only =1);
  


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
