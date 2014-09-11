#ifndef ISLANDAMPLITUDE_H_
#define ISLANDAMPLITUDE_H_

#include "definitions.h"
#include "BaseModule.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}

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
class IslandAmplitude : public BaseModule {

 public:
  /// \brief
  /// Constructor description. If necessary, add a details tag like above.
  ///
  /// \param[in] opts Describe the options this module takes.
  IslandAmplitude(modules::options* opts);
  /// \brief
  /// Is anything done in the destructor?
  ~IslandAmplitude();

 private:

  typedef const std::map<std::string, std::string> mapSS_t;
  typedef mapSS_t::const_iterator mapSS_iter;
  typedef std::map<std::string, TH1F*> mapSH_t;
  typedef PulseIslandList::iterator PIL_iter;
  

  /// \brief
  /// What's calculated for every entry?
  /// Don't hesitate to repeat what was said in the class description.
  /// 
  /// \param[in] gData See BaseModule::ProcessEntry
  /// \param[in] gSetup See BaseModule::ProcessEntry
  /// \return Non-zero to indicate a problem.
  virtual int ProcessEntry(TGlobalData *gData,const  TSetupData *gSetup);
  /// \brief
  /// What needes to be done before each run?
  /// Don't hesitate to repeat what was said in the class description.
  ///
  /// \param[in] gData See BaseModule::BeforeFirstEntry
  /// \param[in] setup See BaseModule::BeforeFirstEntry
  /// \return Non-zero to indicate a problem.
  virtual int BeforeFirstEntry(TGlobalData* gData,const  TSetupData *setup);
  /// \brief
  /// What needs to be done after each run?
  /// Don't hesitate to repeat what was said in the class description.
  ///
  /// \param[in] gData See BaseModule::AfterLastEntry
  /// \param[in] setup See BaseModule::AfterLastEntry
  /// \return Non-zero to indicate a problem.
  virtual int AfterLastEntry(TGlobalData* gData,const  TSetupData *setup);

  /// \brief
  /// Don't forget to forget descirptions for each field...
  ///
  /// \details
  /// ...and don't hesitate to include details.

  mapSH_t fAmpHist;
  mapSH_t fAmpHistNorm;

  TH1* fAmpNorm;

  int fNProcessed;


};

#endif //ISLANDAMPLITUDE_H_
