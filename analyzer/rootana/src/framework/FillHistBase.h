#ifndef FillHistBase_h__
#define FillHistBase_h__ 1

#include "TGlobalData.h"
#include "TSetupData.h"
#include "TDirectory.h"
namespace modules{
   class options;
}

#include <iostream>

/// Base class for a generic analysis module.  
/// Provides several virtual methods for the derived class to overload.
/// Also sets up a directory where all histograms and other ROOT objects created
/// by the module will be saved in the output root file.
class FillHistBase
{
 public:
   /// @brief Constructor of a module class. Provides several virtual methods
   /// that should be overloaded in the derived class.
   /// Derived classes should hard-code their name as the first argument to the
   /// constructor.
   ///
   /// @param module_name The name of the module
   /// @param opts Options given to the module from the modules file
   /// @param setup [To be removed soon]
  FillHistBase(const char* module_name, modules::options* opts=NULL, TSetupData* setup=NULL);
  virtual ~FillHistBase();

  /// Method called by the main event loop for each entry in the input root tree.
  /// Does some simple work, then hooks into the derived class through ProcessEntry.
  /// @return 0 on sucess and non-zero if a problem occurred
  int ProcessGenericEntry(TGlobalData *gData, TSetupData *gSetup);

  /// Optional method which is called once before the main event loop
  /// Can be used to parse options and setup histograms.
  /// @return 0 on sucess and non-zero if a problem occurred
  virtual int BeforeFirstEntry(TGlobalData* gData, TSetupData *setup){return 0;};

  /// Optional method which is called once after the main event loop
  /// Can be used to dump a summary or finalise a histogram
  /// @return 0 on sucess and non-zero if a problem occurred
  virtual int AfterLastEntry(TGlobalData* gData, TSetupData *setup){return 0;};
	  
  /// Sets the alias for this module, which should be provided in the modules
  /// file
  void SetAlias(const std::string& alias){fAlias=alias;};

  /// Returns a string for the alias of this module.  May be empty if no alias
  /// was given in the modules file.
  std::string GetAlias()const{return fAlias;};

  /// Get the name of this module as given to the constructor of the base class
  const char* GetName()const{return dir->GetName();};
 
 protected:
  /// Check whether this module was asked to print extra debug information
  /// @returns true if 'debug' was given in the modules file, false if not
  bool Debug()const{return fDebug;};

 private:
   /// @brief The big kahuna.  Overload this in the derived class and it will be
   /// called for each event.
   ///
   /// @param gData Pointer to current map of all TPIs [ WILL CHANGE SHORTLY ]
   /// @param gSetup Pointer to TSetupData for this run [ WILL CHANGE SHORTLY ]
   ///
   /// @return 0 on success and non-zero on failure
   virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);

 protected:
  TDirectory *dir;
  TSetupData* fSetup;

 private:
   bool fDebug;
   std::string fAlias;
};

#endif
