#ifndef ISLANDLENGTH_H_
#define ISLANDLENGTH_H_

#include "definitions.h"
#include "BaseModule.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;};

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
class IslandLength : public BaseModule {

 public:
  /// \brief
  /// Constructor description. If necessary, add a details tag like above.
  ///
  /// \param[in] opts Describe the options this module takes.
  IslandLength(modules::options* opts);
  /// \brief
  /// Is anything done in the destructor?
  ~IslandLength();

 private:
  /// \brief
  /// What's calculated for every entry?
  /// Don't hesitate to repeat what was said in the class description.
  /// 
  /// \param[in] gData See BaseModule::ProcessEntry
  /// \param[in] gSetup See BaseModule::ProcessEntry
  /// \return Non-zero to indicate a problem.
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  /// \brief
  /// What needes to be done before each run?
  /// Don't hesitate to repeat what was said in the class description.
  ///
  /// \param[in] gData See BaseModule::BeforeFirstEntry
  /// \param[in] setup See BaseModule::BeforeFirstEntry
  /// \return Non-zero to indicate a problem.
  virtual int BeforeFirstEntry(TGlobalData* gData, TSetupData *setup);
  /// \brief
  /// What needs to be done after each run?
  /// Don't hesitate to repeat what was said in the class description.
  ///
  /// \param[in] gData See BaseModule::AfterLastEntry
  /// \param[in] setup See BaseModule::AfterLastEntry
  /// \return Non-zero to indicate a problem.
  virtual int AfterLastEntry(TGlobalData* gData, TSetupData *setup);

private:
  typedef const std::map<std::string, std::string> mapSS_t;
  typedef mapSS_t::const_iterator mapSS_iter;
  typedef std::map<std::string, TH1F*> mapSH_t;
  typedef StringPulseIslandMap::iterator mapSP_iter;
  typedef PulseIslandList::iterator PIL_iter;
  

  /// Copy C'tor is diallowed becasue i dont know what it might do
  IslandLength(const IslandLength& src);

  /// Copy assignment is disallowed becasue i dont know what it might
  /// do
  IslandLength& operator() (const IslandLength& rhs);

  /// Book histograms and similar
  virtual void Book(TGlobalData* data, TSetupData* setup);

  /// The directory where the histograms will reside
  std::string fDirName;

  /// The directory where histograms arre stored
  TDirectory* fHistDir;

  /// Map of histograms, keyed by bank_name;
  mapSH_t fHistos;

  /// Map of normalised histograms, keyed by bank_name;
  mapSH_t fHistosNorm;

  /// The histogram that we use for normalising to;
  TH1* fNormHist; 

  //Counter for the number of entries we have processed;
  int fNProcessed;
};

#endif //ISLANDLENGTH_H_

