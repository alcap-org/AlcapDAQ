#ifndef LOADPULSES_H_
#define LOADPULSES_H_

#include "BaseModule.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}
namespace IDs {class source;}
class TClonesArray;
class TTree;

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author AuthorName
///
/// \brief
/// A one line description of what your module does.
///
/// \details
/// A longer, more descriptive block of text.
/// Specifics like members and methods will be described later.
/// You can add this to other groups instead of rootana_modules or in addition
/// to rootana_modules by adding more of the ingroup tags.
////////////////////////////////////////////////////////////////////////////////
class LoadPulses : public BaseModule {

 public:
  /// \brief
  /// Constructor description. If necessary, add a details tag like above.
  ///
  /// \param[in] opts Describe the options this module takes.
  LoadPulses(modules::options* opts);
  /// \brief
  /// Is anything done in the destructor?
  ~LoadPulses();

 private:
  /// \brief
  /// What's calculated for every entry?
  /// Don't hesitate to repeat what was said in the class description.
  /// 
  /// \return Non-zero to indicate a problem.
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);
  /// \brief
  /// What needes to be done before each run?
  /// Don't hesitate to repeat what was said in the class description.
  ///
  /// \return Non-zero to indicate a problem.
  virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup);
  /// \brief
  /// What needs to be done after each run?
  /// Don't hesitate to repeat what was said in the class description.
  ///
  /// \return Non-zero to indicate a problem.
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData *setup);

  std::string fFileName; 
  std::string fTreeName;
  TTree* fInTree;
  Long64_t fCurrentEntry;
  typedef std::map<IDs::source,TClonesArray*> SourceToClonesArrayMap;
  SourceToClonesArrayMap fArrays;
  typedef std::vector<std::pair<IDs::source, TBranch*> > BranchList;
  BranchList fBranches;
};

#endif //LOADPULSES_H_
